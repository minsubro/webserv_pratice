#include "../Common.h"

int main(int ac, char **av)
{
    const char *ipv4 = "127.0.0.1";
    printf("IPv4 변환전  = %s\n", ipv4);

    in_addr ipv4num;
    inet_pton(AF_INET, ipv4, &ipv4num);
    printf("IPv4 변환후 = %#x\n", ipv4num.s_addr);

    char ipv4str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
    printf("IPv4 다시 변환 = %s\n", ipv4str);
}