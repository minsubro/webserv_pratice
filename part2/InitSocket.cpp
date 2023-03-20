#include "../Common.h"

int main(int ac, char **av)
{
	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");
	printf("소켓 생성 성공\n");

	// 소켓 닫기
	close(sock);
	return 0;
}