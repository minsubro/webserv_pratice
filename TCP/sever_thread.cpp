#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

// 클라이언트와 데이터 통신
void *ProcessClient(void *arg)
{
    SOCKET client_sock = (SOCKET)(long long)arg;
    int retval;
    sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    socklen_t addrlen;
    char buf[BUFSIZE + 1];

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (sockaddr *)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    while (1)
    {
        // 데이터 받기
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break ;
        }
        else if (retval == 0)
            break ;

        // 받은 데이터 출력
        buf[retval] = '\0';
        printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

        retval = send(client_sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break ;
        }
    }
    close(client_sock);
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
    return (0);
}

int main(int ac, char **av)
{
    int retval;

    (void)ac;
    (void)av;
    
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        err_quit("socket()");
    }

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        err_quit("bind()");
    }

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        err_quit("listen()");
    }

    SOCKET client_sock;
    sockaddr_in clientaddr;
    socklen_t addrlen;
    pthread_t tid;

    while (1)
    {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (sockaddr *)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break ;
        }

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));

        retval = pthread_create(&tid, NULL, ProcessClient, (void *)(long long)client_sock);
        if (retval != 0) {
            close(client_sock);
        }
        
    }
    close(listen_sock);
}