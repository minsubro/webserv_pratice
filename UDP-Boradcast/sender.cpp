#include "../Common.h"

#define REMOTEIP "255.255.255.255"
#define REMOTEPORT 9000
#define BUFSIZE 512

int main()
{
    int retval;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        err_quit("socket()");
    
    int bEnable = 1;
    retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&bEnable, sizeof(bEnable));
    if (retval == SOCKET_ERROR)
        err_quit("setsockopt()");
    
    sockaddr_in remoteaddr;
    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    inet_pton(AF_INET, REMOTEIP, &remoteaddr.sin_addr);
    remoteaddr.sin_port = htons(REMOTEPORT);

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
        
        retval = sendto(sock, buf, strlen(buf), 0, (sockaddr *)&remoteaddr, sizeof(remoteaddr));
        if (retval == SOCKET_ERROR) {
            err_display("sendto()");
            break ;
        }

        printf("[UDP] %d바이트를 보냈습니다.\n", retval);
    }

    close(sock);
}