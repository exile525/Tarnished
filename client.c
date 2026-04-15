/*===============================================
*   文件名称：client.c
*   创 建 者：     
*   创建日期：2026年01月09日
*   描    述：gcc -o client client.c
================================================*/
#include <stdio.h>
// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> // 包含inet_addr, htons等函数

#define SERVER_PORT 8521 // 与server.c中定义的PORT保持一致
#define SERVER_IP "127.0.0.1" // 服务器IP地址，默认连接本机
#define BUF_SIZE 1024 // 定义缓冲区大小

int main(int argc, char const *argv[]) {
    int sock = 0; // 客户端套接字描述符
    struct sockaddr_in serv_addr; // 用于存储服务器地址信息
    char buffer[BUF_SIZE] = {0}; // 用于接收服务器消息的缓冲区

    // 1. 创建套接字
    // AF_INET: IPv4协议族
    // SOCK_STREAM: TCP协议
    // 0: 自动选择协议
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    printf("客户端连接成功.\n");

    // 2. 设置服务器地址信息
    serv_addr.sin_family = AF_INET; // IPv4地址族
    serv_addr.sin_port = htons(SERVER_PORT); // 将端口号转换为网络字节序

    // 将字符串形式的IP地址转换为网络字节序的二进制形式
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("地址错误或者地址不支持\n");
        return -1;
    }

    // 3. 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("连接失败\n");
        return -1;
    }
    printf("连接到服务器 %s:%d\n", SERVER_IP, SERVER_PORT);

    printf("可以发送信息 (Press Ctrl+C to exit):\n");
    
    // 4. 通信循环
    while(1) {
       
        //自己实现
        memset(buffer, 0, BUF_SIZE);
    // 读取用户输入
    printf("客户端 > ");
    fgets(buffer, BUF_SIZE, stdin);

    // 去掉fgets读取的换行符（避免发送多余的换行）
    buffer[strcspn(buffer, "\n")] = '\0';

    // 如果输入exit，退出循环
    if (strcmp(buffer, "exit") == 0) {
        printf("客户端主动退出...\n");
        break;
    }

    // 发送消息到服务器
    ssize_t send_len = send(sock, buffer, strlen(buffer), 0);
    if (send_len < 0) {
        perror("发送消息失败");
        break;
    }

    // 接收服务器的回复
    memset(buffer, 0, BUF_SIZE);
    ssize_t recv_len = recv(sock, buffer, BUF_SIZE, 0);
    if (recv_len < 0) {
        perror("接收消息失败");
        break;
    } else if (recv_len == 0) {
        printf("服务器已断开连接\n");
        break;
    }

    // 打印服务器回复
    printf("服务器 > %s\n", buffer);



    }

    // 5. 关闭套接字
    close(sock);
    printf("成功关闭.\n");
    return 0;
}
