#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int ac, char **av)
{
    int retval;

    (void)ac;
    (void)av;
    
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        err_quit("socket()");
    }

    // 넌블로킹 소켓으로 전환
    int flag = fcntl(listen_sock, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(listen_sock, F_SETFL, flag);

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
    char buf[BUFSIZE + 1];

    while (1)
    {
    ACCEPT_AGAIN:
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (sockaddr *)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            if (errno == EWOULDBLOCK)
                goto ACCEPT_AGAIN;
            err_display("accept()");
            break ;
        }

        // 넌블로킹 소켓으로 전환
        int flag = fcntl(client_sock, F_GETFL);
        flag |= O_NONBLOCK;
        fcntl(client_sock, F_SETFL, flag);

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));

        while (1)
        {
        RECEIVE_AGAIN:
            retval = recv(client_sock, buf, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) {
                if (errno == EWOULDBLOCK)
                    goto RECEIVE_AGAIN;
                err_display("recv()");
                break ;
            }
            else if (retval == 0)
                break ;

            buf[retval] = '\0';
            printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

        SEND_AGAIN:
            retval = send(client_sock, buf, retval, 0);
            if (retval == SOCKET_ERROR) {
                if (errno == EWOULDBLOCK)
                    goto SEND_AGAIN;
                err_display("send()");
                break ;
            }
        }
        close(client_sock);
        printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
        
    }
    close(listen_sock);
}