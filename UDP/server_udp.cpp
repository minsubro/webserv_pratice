#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int main()
{
    int retval;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        err_quit("socket()");

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(sock, (sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        err_quit("bind()");

    sockaddr_in clinetaddr;
    socklen_t addrlen;
    char buf[BUFSIZE + 1];

    while (1)
    {
        addrlen = sizeof(clinetaddr);
        retval = recvfrom(sock, buf, BUFSIZE, 0, (sockaddr *)&clinetaddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            break ;
        }

        buf[retval] = '\0';
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clinetaddr.sin_addr, addr, sizeof(addr));
        printf("[UDP/%s:%d] %s\n", addr, ntohs(clinetaddr.sin_port), buf);

        retval = sendto(sock, buf, retval, 0, (sockaddr *)&clinetaddr, sizeof(clinetaddr));
        if (retval == SOCKET_ERROR)
        {
            err_display("sendto()");
            break ;
        }
    }
    close(sock);
}