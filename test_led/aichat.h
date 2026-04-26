#ifndef AICHAT_H
#define AICHAT_H

#include <QWidget>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>


#include "fsmpLed.h"
#include "fsmpFan.h"
#include "fsmpBeeper.h"

namespace Ui {
class AiChatWidget;
}

class AiChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AiChatWidget(QWidget *parent = nullptr);
    ~AiChatWidget();

private slots:
    // 录音相关
    void on_pushButton_record_pressed();
    void on_pushButton_record_released();

    // 返回主界面
    void on_pushButton_back_clicked();

    // 断开云端连接
    void on_pushButton_disconnect_clicked();

    void onAsrReplyFinished();
    void onLlmReplyFinished();
    void onTtsReplyFinished();
    void onLlmReadyRead();

  
    void on_pushButton_send_clicked();

private:
    void startRecord();
    void stopRecord();
    void sendAsrRequest(const QString &wavFilePath);
    void sendLlmRequest(const QString &userMessage);
    void sendTtsRequest(const QString &text);
    void playAudioFile(const QString &filePath);
    void appendMessage(const QString &sender, const QString &message);

  
    void handleLlmResponse(const QString &response);
    void executeDeviceCommand(const QString &jsonStr);

private:
    Ui::AiChatWidget *ui;

    QProcess *recordProcess = nullptr;
    QNetworkAccessManager *netManager = nullptr;
    QNetworkReply *currentLlmReply = nullptr;

    QString inputWavPath = "input.wav";
    QString outputWavPath = "tts_output.wav";

    // ===== 直接使用 API Key，无需换取 Access Token =====
    const QString apiKey = "bce-v3/ALTAK-qUP5JevGlWfQTx3eKolOv/a4f4d07fea220878982b9e501d85798b7bcc5738";

  
    const QString asrUrl = "http://vop.baidu.com/server_api";
    const QString ttsUrl = "https://tsn.baidu.com/text2audio";
    const QString llmUrl = "https://qianfan.baidubce.com/v2/chat/completions";

    // 系统提示词（从资源文件加载）
    QString systemPrompt;

    // 硬件控制对象
    fsmpLeds leds;
    fsmpFan fan;
    fsmpBeeper beeper;

    
    QByteArray llmBuffer;
    bool llmStreaming = false;
};

#endif // AICHAT_H
