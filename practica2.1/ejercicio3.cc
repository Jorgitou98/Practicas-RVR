#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;

#define BUF_SIZE 500

int main(int argc, char *argv[]){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 4) {
        cout << "Usage: " << argv[0] << " host port msg\n";
        exit(1);
    }

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
        if (sfd != -1)
            break;
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not use socket\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */
    char recbuff[BUF_SIZE];
    if (sendto(sfd, argv[3], strlen(argv[3]), 0, rp->ai_addr, rp->ai_addrlen) != strlen(argv[3])) {
        cout << "Partial/failed write";
        exit(1);
    }

    if (strcmp(argv[3], "q") == 0) return 0;

    if(recvfrom(sfd, recbuff, BUF_SIZE, 0, rp->ai_addr, &rp->ai_addrlen) == -1){
        perror("recvfrom");
        exit(1);
    }
    cout << recbuff;
    return 0;
}