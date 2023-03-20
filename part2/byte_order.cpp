#include "../Common.h"

int main(int ac, char **av)
{
	u_short x1 = 0x1234;
    u_long y1 = 0x12345678;
    u_short x2;
    u_long y2;

    printf("%#hx -> %#x\n", x1, x2 = ntohs(x1));
    printf("%#lx -> %#lx\n", y1, y2 = ntohl(y1));

    printf("%#x -> %#lx\n", x2, ntohs(x2));
    printf("%#lx -> %#x\n", y2, ntohs(y2));
}