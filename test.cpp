#include "Common.h"

int main()
{
    in_addr addr;
    hostent *a;

    a = gethostbyname("www.intra.42.fr");
    if (!a)
    {
        printf("asdf\n");
        exit(1);
    }
    printf("%s\n", a->h_name);
    printf("-------------\n");
    for (int i = 0; a->h_aliases[i]; i++)
        printf("%s\n", a->h_aliases[i]);
	

	return 0;
}