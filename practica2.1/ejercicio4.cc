#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

#define BUF_SIZE 500


int accept_client(int sfd){
    struct sockaddr_storage peer_addr;
    // Establecemos la conexión
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    int cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    if(cfd == -1){
        perror("accept");
        exit(1);
    }

    // Mostramos quién se ha conectado
    char host[NI_MAXHOST], service[NI_MAXSERV];
    int s = getnameinfo((struct sockaddr *) &peer_addr,
                    peer_addr_len, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    if (s == 0)
        printf("Conectado a %s:%s\n", host, service);
    else
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
    return cfd;
}

int main(int argc, char *argv[]){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s dir port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    // Ponemos en listen
    if (listen(sfd, 5) == -1){
        perror("listen");
        exit(1);
    }
    // Aceptamos un cliente
    int cfd = accept_client(sfd);

    /* Read datagrams and echo them back to sender */

    for (;;) {
        nread = recv(cfd, buf, BUF_SIZE, 0);
        buf[nread] = '\0';
        if (nread == -1)
            continue;               /* Ignore failed request */
        if (nread == 0){
            cout << "Cliente cerrado\n";
            close(cfd);
            // Aceptamos un cliente
            cfd = accept_client(sfd);
            continue;
        }
        if (send(cfd, buf, nread, 0) != nread)
            fprintf(stderr, "Error sending response\n");
    }
}
