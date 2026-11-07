#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

int main(void)
{
    uint16_t a = 0x1234;
    uint32_t b = 0x12345678;

    uint16_t net_a = htons(a);
    uint32_t net_b = htonl(b);

    printf("a original : 0x%04X\n", a);
    printf("a reseau   : 0x%04X\n", net_a);

    printf("b original : 0x%08X\n", b);
    printf("b reseau   : 0x%08X\n", net_b);

    printf("a restaure : 0x%04X\n", ntohs(net_a));
    printf("b restaure : 0x%08X\n", ntohl(net_b));

    return 0;
}
