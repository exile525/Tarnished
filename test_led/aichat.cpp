#include "aichat.h"
#include "ui_aichat.h"
#include "widget.h"
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>

AiChatWidget::AiChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AiChatWidget),
    currentLlmReply(nullptr),
    llmStreaming(false)
{
    ui->setupUi(this);

    recordProcess = new QProcess(this);
    netManager = new QNetworkAccessManager(this);

   
    QFile file(":/prompt.txt");
    if (file.open(QIODevice::ReadOnly)) {
        systemPrompt = QString::fromUtf8(file.readAll());
        file.close();
    } else {
        systemPrompt = "你是智能管家，名叫“哈基米”。";
        qDebug() << "提示词加载失败，使用默认内容";
    }

    appendMessage("哈基米", "主人，我是哈基米，有什么可以效劳的？");

    ui->pushButton_record->setStyleSheet(
        "QPushButton { background-color: #428bca; color: white; border: none; border-radius: 20px; }"
        "QPushButton:hover { background-color: #3071a9; }"
        "QPushButton:pressed { background-color: #1d4568; }"
    );
}

AiChatWidget::~AiChatWidget()
{
    if (recordProcess && recordProcess->state() != QProcess::NotRunning) {
        recordProcess->terminate();
        recordProcess->waitForFinished(1000);
    }
    delete ui;
}

void AiChatWidget::appendMessage(const QString &sender, const QString &message)
{
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm");
    QString displayText;

    if (sender == "我") {
        displayText = QString("<div align='right'><b>%1 %2</b><br>"
                              "<span style='background-color:#95ec69; padding:8px 12px; border-radius:15px;'>%3</span></div><br>")
                              .arg(sender, timeStr, message);
    } else {
        displayText = QString("<div align='left'><b>%1 %2</b><br>"
                              "<span style='background-color:#ffffff; padding:8px 12px; border-radius:15px; border:1px solid #d0d0d0;'>%3</span></div><br>")
                              .arg(sender, timeStr, message);
    }

    ui->textBrowser_chat->append(displayText);
}

void AiChatWidget::on_pushButton_record_pressed()
{
    startRecord();
    ui->pushButton_record->setText("松开结束");
    ui->pushButton_record->setStyleSheet(
        "QPushButton { background-color: #d9534f; color: white; border: none; border-radius: 20px; }"
    );
}

void AiChatWidget::on_pushButton_record_released()
{
    stopRecord();
    ui->pushButton_record->setText("按住录音");
    ui->pushButton_record->setStyleSheet(
        "QPushButton { background-color: #428bca; color: white; border: none; border-radius: 20px; }"
        "QPushButton:hover { background-color: #3071a9; }"
    );
    sendAsrRequest(inputWavPath);
}

void AiChatWidget::startRecord()
{
    if (recordProcess->state() != QProcess::NotRunning) {
        recordProcess->terminate();
        recordProcess->waitForFinished(1000);
    }
    QFile::remove(inputWavPath);

    QString program = "arecord";
    QStringList args;
    args << "-D" << "plughw:1,0"
         << "-f" << "S16_LE"
         << "-r" << "16000"
         << "-c" << "1"
         << "-t" << "wav"
         << inputWavPath;

    recordProcess->start(program, args);
    if (!recordProcess->waitForStarted(1000)) {
        appendMessage("系统", "录音启动失败，请检查麦克风");
    }
}

void AiChatWidget::stopRecord()
{
    if (recordProcess->state() == QProcess::NotRunning)
        return;
    recordProcess->terminate();
    if (!recordProcess->waitForFinished(1500)) {
        recordProcess->kill();
        recordProcess->waitForFinished(1000);
    }
}


void AiChatWidget::sendAsrRequest(const QString &wavFilePath)
{
    QFile file(wavFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        appendMessage("系统", "无法读取录音文件");
        return;
    }
    QByteArray wavData = file.readAll();
    file.close();
    if (wavData.isEmpty()) {
        appendMessage("系统", "录音文件为空");
        return;
    }

    QJsonObject obj;
    obj["format"] = "wav";
    obj["rate"] = 16000;
    obj["dev_pid"] = 1537;
    obj["channel"] = 1;
    obj["cuid"] = "baidu_workshop";
    obj["len"] = wavData.size();
    obj["speech"] = QString::fromLatin1(wavData.toBase64());

    QNetworkRequest req{QUrl(asrUrl)};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

    QNetworkReply *reply = netManager->post(req, QJsonDocument(obj).toJson());
    connect(reply, &QNetworkReply::finished, this, &AiChatWidget::onAsrReplyFinished);
    appendMessage("系统", "正在识别语音...");
}

void AiChatWidget::onAsrReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        appendMessage("系统", "语音识别失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    int errNo = obj.value("err_no").toInt(-1);
    if (errNo != 0) {
        appendMessage("系统", "识别失败: " + obj.value("err_msg").toString());
        reply->deleteLater();
        return;
    }

    QString resultText;
    if (obj.contains("result") && obj["result"].isArray()) {
        QJsonArray arr = obj["result"].toArray();
        if (!arr.isEmpty()) resultText = arr.at(0).toString();
    }

    if (resultText.isEmpty()) {
        appendMessage("系统", "未识别到有效语音");
    } else {
        appendMessage("我", resultText);
        sendLlmRequest(resultText);
    }
    reply->deleteLater();
}

// ================== LLM (千帆) ==================
void AiChatWidget::sendLlmRequest(const QString &userMessage)
{
    QJsonArray messages;
    messages.append(QJsonObject{{"role", "system"}, {"content", systemPrompt}});
    messages.append(QJsonObject{{"role", "user"}, {"content", userMessage}});

    QJsonObject body;
    body["messages"] = messages;
    body["model"] = "ernie-5.0";
    body["stream"] = true;

    QNetworkRequest req{QUrl(llmUrl)};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

    QSslConfiguration sslConfig = req.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(sslConfig);

    currentLlmReply = netManager->post(req, QJsonDocument(body).toJson());
    connect(currentLlmReply, &QNetworkReply::readyRead, this, &AiChatWidget::onLlmReadyRead);
    connect(currentLlmReply, &QNetworkReply::finished, this, &AiChatWidget::onLlmReplyFinished);

    llmBuffer.clear();
    llmStreaming = true;
    appendMessage("哈基米", "（思考中...）");
}

void AiChatWidget::onLlmReadyRead()
{
    if (currentLlmReply)
        llmBuffer.append(currentLlmReply->readAll());
}

void AiChatWidget::onLlmReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        appendMessage("系统", "大模型请求失败: " + reply->errorString());

        reply->deleteLater();
        currentLlmReply = nullptr;
        return;
    }

    llmBuffer.append(reply->readAll());

    QString fullResponse;
    QString dataStr = QString::fromUtf8(llmBuffer);
    QStringList lines = dataStr.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("data: ")) {
            QString jsonStr = trimmed.mid(6);
            if (jsonStr == "[DONE]") continue;

            QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
            if (doc.isObject()) {
                QJsonArray choices = doc.object()["choices"].toArray();
                if (!choices.isEmpty()) {
                    fullResponse += choices.first().toObject()["delta"].toObject()["content"].toString();
                }
            }
        }
    }

    if (!fullResponse.isEmpty()) {
        handleLlmResponse(fullResponse.trimmed());
    } else {
        appendMessage("哈基米", "抱歉，我没有理解你的意思。");
    }

    reply->deleteLater();
    currentLlmReply = nullptr;
    llmBuffer.clear();
    llmStreaming = false;
}

void AiChatWidget::handleLlmResponse(const QString &response)
{
    // 尝试解析为 JSON 指令
    if (response.startsWith("{") && response.endsWith("}")) {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8(), &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            executeDeviceCommand(response);
            return;
        }
    }

    // 普通文本回复
    appendMessage("哈基米", response);
    sendTtsRequest(response);
}

void AiChatWidget::executeDeviceCommand(const QString &jsonStr)
{
    QJsonObject cmd = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    QString dev = cmd["dev"].toString();
    QString stat = cmd["stat"].toString();
    QString room = cmd["room"].toString();
    int temp = cmd["temp"].toInt();

    QString resultMsg;

    // ========== LED 控制 ==========
    if (dev == "led") {
        if (room.isEmpty()) {
            // 没有指定房间，控制所有灯
            if (stat == "on") {
                leds.on(fsmpLeds::LED1);
                leds.on(fsmpLeds::LED2);
                leds.on(fsmpLeds::LED3);
                resultMsg = "好的，已打开所有灯光。";
            } else if (stat == "off") {
                leds.off(fsmpLeds::LED1);
                leds.off(fsmpLeds::LED2);
                leds.off(fsmpLeds::LED3);
                resultMsg = "好的，已关闭所有灯光。";
            }
        } else {
            // 控制指定房间的灯
            fsmpLeds::lednum targetLed;
            QString roomName;
            if (room == "living") {
                targetLed = fsmpLeds::LED1;
                roomName = "客厅";
            } else if (room == "cook") {
                targetLed = fsmpLeds::LED2;
                roomName = "厨房";
            } else if (room == "master") {
                targetLed = fsmpLeds::LED3;
                roomName = "卧室";
            } else {
                resultMsg = "抱歉，我没有找到这个房间。";
                appendMessage("哈基米", resultMsg);
                sendTtsRequest(resultMsg);
                return;
            }

            if (stat == "on") {
                leds.on(targetLed);
                resultMsg = QString("好的，已打开%1的灯光。").arg(roomName);
            } else if (stat == "off") {
                leds.off(targetLed);
                resultMsg = QString("好的，已关闭%1的灯光。").arg(roomName);
            }
        }
    }
    // ========== 蜂鸣器控制 ==========
    else if (dev == "beeper") {
        if (stat == "on") {
            beeper.setRate(1000);
            beeper.start();
            resultMsg = "警报已触发，请注意安全！";
        } else if (stat == "off") {
            beeper.stop();
            resultMsg = "警报已关闭。";
        } else {
            resultMsg = "蜂鸣器指令格式错误。";
        }
    }
    // ========== 风扇控制 ==========
    else if (dev == "fan") {
        if (stat == "on") {
            fan.setSpeed(100);
            fan.start();
            resultMsg = "好的，已开启风扇。";
        } else if (stat == "off") {
            fan.stop();
            resultMsg = "好的，已关闭风扇。";
        }
    }
    else {
        resultMsg = "收到指令，但暂不支持该设备。";
    }

    appendMessage("哈基米", resultMsg);
    sendTtsRequest(resultMsg);
}

// ================== TTS ==================
void AiChatWidget::sendTtsRequest(const QString &text)
{
    QUrlQuery query;
    query.addQueryItem("tex", text);
    query.addQueryItem("tok", apiKey);
    query.addQueryItem("cuid", "baidu_workshop");
    query.addQueryItem("ctp", "1");
    query.addQueryItem("lan", "zh");
    query.addQueryItem("aue", "6");
    query.addQueryItem("spd", "5");
    query.addQueryItem("pit", "5");
    query.addQueryItem("vol", "5");
    query.addQueryItem("per", "4105");

    QNetworkRequest req{QUrl(ttsUrl)};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

    QSslConfiguration sslConfig = req.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(sslConfig);

    QNetworkReply *reply = netManager->post(req, query.query(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, &AiChatWidget::onTtsReplyFinished);
}

void AiChatWidget::onTtsReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QByteArray contentType = reply->header(QNetworkRequest::ContentTypeHeader).toByteArray();

    if (contentType.startsWith("audio")) {
        QFile file(outputWavPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();
            playAudioFile(outputWavPath);
        }
    }
    reply->deleteLater();
}

void AiChatWidget::playAudioFile(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        qDebug() << "[PLAY] 音频文件不存在:" << filePath;
        return;
    }

    qDebug() << "[PLAY] 准备播放文件:" << filePath;

    QProcess *player = new QProcess(this);

    connect(player, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            player, &QObject::deleteLater);

    connect(player, &QProcess::errorOccurred, this,
            [this, player, filePath](QProcess::ProcessError err) {
        qDebug() << "[PLAY] 播放进程错误:" << err
                 << "errorString:" << player->errorString();
        qDebug() << "[PLAY] stdout:" << player->readAllStandardOutput();
        qDebug() << "[PLAY] stderr:" << player->readAllStandardError();
        appendMessage("系统", "语音播放失败，请检查音频设备");
        player->deleteLater();
    });

    connect(player, &QProcess::readyReadStandardError, this, [player]() {
        QByteArray err = player->readAllStandardError();
        if (!err.isEmpty()) {
            qDebug() << "[PLAY] aplay stderr:" << err;
        }
    });

    QStringList args;
    args << filePath;

    qDebug() << "[PLAY] 执行命令: aplay" << args;

    player->start("aplay", args);

    if (!player->waitForStarted(1000)) {
        qDebug() << "[PLAY] aplay 启动失败:" << player->errorString();
        qDebug() << "[PLAY] stdout:" << player->readAllStandardOutput();
        qDebug() << "[PLAY] stderr:" << player->readAllStandardError();
        appendMessage("系统", "aplay 启动失败");
        player->deleteLater();
        return;
    }

    qDebug() << "[PLAY] 播放进程 PID:" << player->processId() << " 已启动";
}

void AiChatWidget::on_pushButton_back_clicked()
{
  
    QWidgetList topWidgets = QApplication::topLevelWidgets();
    for (QWidget *w : topWidgets) {
        if (qobject_cast<Widget*>(w)) {
            w->show();
            w->raise();
            break;
        }
    }
    this->close();
}
void AiChatWidget::on_pushButton_disconnect_clicked()
{
    if (currentLlmReply) {
        currentLlmReply->abort();
        currentLlmReply->deleteLater();
        currentLlmReply = nullptr;
    }
    appendMessage("系统", "已断开云端连接");
}

void AiChatWidget::on_pushButton_send_clicked()
{
    
}
