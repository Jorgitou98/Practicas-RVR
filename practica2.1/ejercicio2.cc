#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>

using namespace std;

#define BUF_SIZE 50

int main(int argc, char *argv[]){
    struct addrinfo hints;
    struct addrinfo *result;
    int sfd, s;
    ssize_t nread;

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " dirIPv4 puerto\n";
        exit(1);
    }

    // Crear los hints
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */

    // Tomar la dirección
    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        perror("getaddrinfo");
        exit(1);
    }
    // Probar a asociar la dirección (bind) hasta que funcione, con los distintas posibles configuraciones
    struct addrinfo* rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

        close(sfd);
    }

    // Si no se ha conseguido descriptor al asociar, se manda un fallo
    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    // Liberar todos los resultados
    freeaddrinfo(result);        

    // Leer mensajes, mostrar de quien vienen y responder con la hora
    char buf[BUF_SIZE];
    char response[BUF_SIZE];
    for (;;) {
        struct sockaddr peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
                &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo(&peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        
        if (s == 0)
            cout << nread << " bytes de " << host << ":" << service << '\n';
        else
            perror("getnameinfo");

        // Obtenemos los datos del tiempo y preparamos la respuesta
        time_t timep = time(NULL);
        struct tm * lt = localtime(&timep);
        size_t rc;
        if(buf[nread-1] == '\n') buf[nread-1] = '\0';
        // Importante colocar el '\0' al final
        buf[nread] = '\0';
        string str(buf);
        if(str == "t"){
            rc = strftime(response, BUF_SIZE, "%r", lt);
        }
        else if(strcmp(buf, "d") == 0){
            rc = strftime(response, BUF_SIZE, "%F", lt);
        }
        else if(strcmp(buf, "q") == 0){
            cout << "Saliendo...\n";
            break;
        }
        else{
            strcpy(response, "Comando incorrecto");
            rc = strlen(response);
        }

        if (sendto(sfd, response, rc, 0, &peer_addr, peer_addr_len) != rc)
            perror("sendto");
    }
    return 0;
}