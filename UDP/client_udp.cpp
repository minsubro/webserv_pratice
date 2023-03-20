#include "../Common.h"

char *SERVERIP = (char *)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 512

int main(int ac, char **av)
{  
    int retval;

    if (ac > 1)
        SERVERIP = av[1];

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        err_quit("socket()");

    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);

    sockaddr_in peeraddr;
    socklen_t addrlen;
    char buf[BUFSIZE + 1];
    int len;

    while (1)
    {
        printf("\n[보낼 데이터] ");
        if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
            break ;
        
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        if (strlen(buf) == 0)
            break ;
        
        retval = sendto(sock, buf, strlen(buf), 0, (sockaddr *)&serveraddr, sizeof(serveraddr));
        if (retval == SOCKET_ERROR){
            err_display("sendto()");
            break;
        }
        printf("[UDP 클라이언트] %d 바이트를 보냈습니다.\n", retval);

        addrlen = sizeof(peeraddr);
        retval = recvfrom(sock, buf, BUFSIZE, 0, (sockaddr *)&peeraddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            break ;
        }

        // if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
        //     printf("[오류] 잘못된 데이터 입니다.\n");
        //     break ;
        // }

        buf[retval] = '\0';
        printf("[UDP 클라이언트] %d 바이트를 받았습니다.\n", retval);
        printf("[받은 데이터] %s\n", buf);
    }
    close(sock);
}