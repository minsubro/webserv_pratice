#include "../../Common.h"

char *SERVERIP = (char *)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 50

int main(int ac, char **av)
{
    int retval;

    if (ac > 1) {
        SERVERIP = av[1];
    }



    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
   

    char buf[BUFSIZE];
    const char *testdata[] = {
        "안녕하세요",
        "반가워요",
        "오늘따라 할 이야기가 많을 것 같네요.",
        "저도 그렇네요",
    };
    int len;

    for (int i = 0; i < 4; i++)
    {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            err_quit("socket()");
        }

        retval = connect(sock, (sockaddr *)&serveraddr, sizeof(serveraddr));
        if (retval == SOCKET_ERROR) {
            err_quit("connect()");
        }

        len = strlen(testdata[i]);
        strncpy(buf, testdata[i], strlen(testdata[i]));
        
        // 가변 길이 보내기
        retval = send(sock, buf, len, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break ;
        }
        printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

        close(sock);
    }

    // while (1) {
    //     printf("\n[보낼 데이터] ");
    //     if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
    //         break ;
    //     }

    //     len = (int)strlen(buf);
    //     if (buf[len - 1] == '\n')
    //         buf[len - 1] = '\0';
    //     if (strlen(buf) == 0)
    //         break ;
        
    //     retval = send(sock, buf, (int)strlen(buf), 0);
    //     if (retval == SOCKET_ERROR) {
    //         err_display("send()");
    //         break ;
    //     }

    //     printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

    //     retval = recv(sock, buf, retval, MSG_WAITALL);
    //     if (retval == SOCKET_ERROR) {
    //         err_display("recv()");
    //         break ;
    //     }
    //     else if (retval == 0) {
    //         break ;
    //     }

    //     buf[retval] = '\0';
    //     printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
    //     printf("[받은 데이터] %s\n", buf);
        
    // }
    //close(sock);
}