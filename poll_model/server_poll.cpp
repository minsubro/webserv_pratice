#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

typedef struct SOCKINFO
{
    char buf[BUFSIZE + 1];
    int recvbyte;
    int sendbyte;
}   SOCKETINFO;

int nTOtalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];
pollfd  Pollfds[FD_SETSIZE];

bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

int main()
{
    int retval;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET)
        err_quit("socket()");
    
    sockaddr_in  serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        err_quit("bind()");
    
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR)
        err_quit("listen()");
    
    int flags = fcntl(listen_sock, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(listen_sock, F_SETFL, flags);

    Pollfds[0].fd = listen_sock;
    Pollfds[0].events = POLLIN;
    ++nTOtalSockets;

    int nready;
    SOCKET client_sock;
    sockaddr_in clientaddr;
    socklen_t addrlen;

    while (1) {
        for (int i = 1; i < nTOtalSockets; i++) {
            if (SocketInfoArray[i]->recvbyte > SocketInfoArray[i]->sendbyte)
                Pollfds[i].events = POLLOUT;
            else
                Pollfds[i].events = POLLIN;
        }

        // poll()
        nready = poll(Pollfds, nTOtalSockets, -1);
        if (nready == SOCKET_ERROR)
            err_quit("poll()");
        
        if (Pollfds[0].revents & POLLIN) {
            addrlen = sizeof(clientaddr);
            client_sock = accept(listen_sock, (sockaddr *)&clientaddr, &addrlen);
            if (client_sock == INVALID_SOCKET) {
                err_display("accept()");
                break ;
            }
            else {
                // nonblocking
                int flags = fcntl(client_sock, F_GETFL);
                flags |= O_NONBLOCK;
                fcntl(client_sock, F_SETFL, flags);

                // client info print
                char addr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
                printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
                if (!AddSocketInfo(client_sock))
                    close(client_sock);
            }
            if (--nready <= 0)
                continue ;
        }

        // 소켓 이벤트 검사 : 데이터 통신
        for (int i = 1; i < nTOtalSockets; i++) {
            SOCKETINFO *ptr = SocketInfoArray[i];
            SOCKET sock = Pollfds[i].fd;
            if (Pollfds[i].revents & POLLIN) {
                // 데이터 받기
                retval = recv(sock, ptr->buf, BUFSIZE, 0);
                if (retval == SOCKET_ERROR) {
                    err_display("recv()");
                    RemoveSocketInfo(i);
                }
                else if (retval == 0) {
                    RemoveSocketInfo(i);
                }
                else {
                    ptr->recvbyte = retval;
                    // 클라이언트 정보 얻기
                    addrlen = sizeof(clientaddr);
                    getpeername(sock, (sockaddr *)&clientaddr, &addrlen);
                    // 받은 데이터 출력
                    ptr->buf[ptr->recvbyte] = '\0';
                    char addr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
                    printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), ptr->buf);
                }
            }
            if (Pollfds[i].revents & POLLOUT) {
                // 데이터 보내기
                retval = send(sock, ptr->buf + ptr->sendbyte, ptr->recvbyte - ptr->sendbyte, 0);
                if (retval == SOCKET_ERROR) {
                    err_display("send()");
                    RemoveSocketInfo(i);
                }
                else {
                    ptr->sendbyte += retval;
                    if (ptr->recvbyte == ptr->sendbyte) {
                        ptr->recvbyte = ptr->sendbyte = 0;
                    }
                }
            }
        }
    }
    close(listen_sock);
    return 0;
}

bool AddSocketInfo(SOCKET sock) {
    if (nTOtalSockets >= FD_SETSIZE) {
        printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
        return false;
    }
    SOCKETINFO *ptr = new SOCKETINFO;
    if (ptr == NULL) {
        printf("[오류] 메모리가 부족합니다.!\n");
        return false;
    }
    ptr->recvbyte = 0;
    ptr->sendbyte = 0;
    SocketInfoArray[nTOtalSockets] = ptr;

    Pollfds[nTOtalSockets].fd = sock;
    Pollfds[nTOtalSockets].events = POLLIN;

    ++nTOtalSockets;
    return true;
}

void RemoveSocketInfo(int nIndex) {
    SOCKETINFO *ptr = SocketInfoArray[nIndex];
    SOCKET sock = Pollfds[nIndex].fd;

    sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    getpeername(sock, (sockaddr *)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));

    close(sock);
    delete ptr;

    if (nIndex != (nTOtalSockets - 1)) {
        SocketInfoArray[nIndex] = SocketInfoArray[nTOtalSockets - 1];
        Pollfds[nIndex] = Pollfds[nTOtalSockets - 1];
    }
    --nTOtalSockets;
}