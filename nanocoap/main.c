#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>

#include "sock_udp.h"
#include "nanocoap.h"

int main(int argc, char *argv[])
{
    char buf[128];

    sock_udp_t sock;
    udp_endpoint_t local = { .family=AF_INET6, .ipv6.port=5683 };
    ipv6_addr_t remote_addr = {0};
    udp_endpoint_t remote = { .ipv6.addr=&remote_addr };

    ssize_t res = sock_udp_init(&sock, &local, NULL);
    if (res == -1) {
        return -1;
    }

    while(1) {
        res = sock_udp_recv(&sock, buf, sizeof(buf), -1, &remote);
        if (res == -1) {
            perror("recv");
            return -1;
        }
        else {
            char addr_str[INET6_ADDRSTRLEN+5];
            memset(addr_str, '\0', sizeof(addr_str));
            uint16_t port;
            sock_udp_fmt_endpoint(&remote, addr_str, &port);
            buf[res] = '\0';
            printf("recv %zi from %s port %u.\n", res, addr_str, remote.ipv4.port);
            coap_pkt_t pkt;
            coap_parse(&pkt, (uint8_t*)buf, res);
            coap_handle_req(&pkt, buf, sizeof(buf));
        }
        sleep(1);
    }

    return 0;
}
