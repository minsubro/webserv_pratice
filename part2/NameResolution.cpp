#include "../Common.h"

#define TESTNAME "www.google.com"

bool GetIPAddr(const char *name, struct in_addr *addr)
{
	hostent *ptr = gethostbyname(name);
	if (ptr == NULL)
	{ // 오류가 발생하면 오류 출력 후 false 리턴
		err_display("gethostbyname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET) // AF_INET 주소 체계가 아니면 false
		return false;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

// IPv4 주소 -> 도메인 이름
bool getDomainName(in_addr addr, char *name, int namelen)
{
	struct hostent *ptr = gethostbyaddr((const char *)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL)
	{
		err_display("gethostbyaddr()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET) // AF_INET 주소 체계가 아니면 false
		return false;
	strncpy(name, ptr->h_name, namelen);
	return true;
}

int main(int ac, char **av)
{
	in_addr addr;

	if (GetIPAddr(TESTNAME, &addr)) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		printf("IP 주소 변환 후 = %s\n", str);
	}

	char name[256];
	if (getDomainName(addr, name, sizeof(name))) {
		printf("도메인 이름 = %s\n", name);
	}

	return 0;
}