/*===============================================
*   文件名称：server.c
*   创 建 者：     
*   创建日期：2026年01月09日
*   描    述：gcc -o server server.c
================================================*/
// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8521 // 定义服务器监听的端口号
#define BUF_SIZE 1024 // 定义缓冲区大小

int main() {
    int server_fd, new_socket; // server_fd: 监听套接字, new_socket: 与客户端通信的连接套接字
    struct sockaddr_in address; // 用于存储服务器地址信息
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE] = {0}; // 用于接收客户端消息的缓冲区

    // 1. 创建套接字
    // AF_INET: IPv4协议族
    // SOCK_STREAM: TCP协议 (可靠的、面向连接的流)
    // 0: 自动选择协议 (对于SOCK_STREAM通常是TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("服务器套接字创建成功\n");

    // 2. 设置套接字选项，允许地址重用
    // 这样可以避免程序异常终止后，短时间内无法再次启动绑定同一端口的问题
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. 绑定地址和端口
    address.sin_family = AF_INET; // IPv4地址族
    address.sin_addr.s_addr = INADDR_ANY; // 绑定到所有可用的本地网络接口
    address.sin_port = htons(PORT); // 将主机字节序的端口号转换为网络字节序

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("绑定端口成功：%d.\n", PORT);

    // 4. 开始监听
    // backlog参数为3，表示最多允许3个连接请求排队等待
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("正在监听端口：%d...\n", PORT);

    // 5. 循环接受连接
    while(1) { // 无限循环，持续服务客户端
        printf("\n等待客户端连接...\n");
        
        // accept会阻塞，直到有客户端连接
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("客户端连接成功. IP: %s, Port: %d\n", 
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // 6. 与客户端通信循环
        while(1) {
           
            //自己实现
            memset(buffer, 0, BUF_SIZE);
    
    // 接收客户端消息
    ssize_t recv_len = recv(new_socket, buffer, BUF_SIZE, 0);
    if (recv_len < 0) {
        perror("接收客户端消息失败");
        break;
    } else if (recv_len == 0) {
        printf("客户端 %s:%d 已断开连接\n", 
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        break;
    }

    // 打印客户端发送的消息
    printf("客户端 %s:%d > %s\n", 
           inet_ntoa(address.sin_addr), ntohs(address.sin_port), buffer);

    // 构造回复消息（原样返回+提示）
    char reply[BUF_SIZE];
    snprintf(reply, BUF_SIZE, "服务器已收到：%s", buffer);

    // 发送回复给客户端
    ssize_t send_len = send(new_socket, reply, strlen(reply), 0);
    if (send_len < 0) {
        perror("发送回复失败");
        break;
    }
        }

        // 7. 关闭当前客户端的连接套接字
        close(new_socket);
    }

    // 注意：由于是无限循环，下面的代码实际上不会执行到
    // close(server_fd); // 关闭监听套接字
    return 0;
} 
