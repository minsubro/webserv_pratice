#include "../Common.h"

#define SERVERPORT 9000
#define BUFSIZE 512

typedef struct SOCKINFO
{
    SOCKET sock;
    char buf[BUFSIZE + 1];
    int recvbyte;
    int sendbyte;
}   SOCKETINFO;

void RegisterEvent(int epollfd, SOCKET sock, uint32_t events)
{
    SOCKETINFO *ptr = new SOCKETINFO;
    ptr->sock = sock;
    ptr->recvbyte = 0;
    ptr->sendbyte = 0;

    epoll_event
}