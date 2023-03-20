#include "../Common.h"

#define LOCALPORT 9000
#define BUFSIZE 512

int main()
{
    int retval;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        err_quit("sock()");

    sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = htons(LOCALPORT);
    retval = bind(sock, (sockaddr *)&localaddr, sizeof(localaddr));
    if (retval == SOCKET_ERROR)
        err_quit("bind()");
    
    sockaddr_in peeraddr;
    socklen_t addrlen;
    char buf[BUFSIZE + 1];

    while (1) {
        addrlen = sizeof(peeraddr);
        retval = recvfrom(sock, buf, BUFSIZE, 0, (sockaddr *)&peeraddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            break ;
        }

        buf[retval] = '\0';
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peeraddr.sin_addr, addr, sizeof(addr));
        printf("[UDP/%s:%d] %s\n", addr, ntohs(peeraddr.sin_port), buf);
    }

    close(sock);
}