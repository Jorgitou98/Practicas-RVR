#include "ejercicio6_2.h"
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <thread>

#define BUF_SIZE 50

using namespace std;

ThreadMsg::ThreadMsg(int sfd){
    this->sfd = sfd;
}

void ThreadMsg::msg_task(){
     // Leer mensajes, mostrar de quien vienen y responder con la hora
    char buf[BUF_SIZE];
    char response[BUF_SIZE];
    ssize_t nread;
    for (;;) {
        struct sockaddr peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
                &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;

        char host[NI_MAXHOST], service[NI_MAXSERV];

        int s = getnameinfo(&peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        cout << "Está atendiendo el thread " << this_thread::get_id() << '\n';
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
        if(strcmp(buf, "t") == 0){
            rc = strftime(response, BUF_SIZE, "%r", lt);
        }
        else if(strcmp(buf, "d") == 0){
            rc = strftime(response, BUF_SIZE, "%F", lt);
        }
        else if(strcmp(buf, "q") == 0){
            cout << "Cliente se desconecta...\n";
            continue;
        }
        else{
            strcpy(response, "Comando incorrecto");
            rc = strlen(response);
        }

        if (sendto(sfd, response, rc, 0, &peer_addr, peer_addr_len) != rc)
            perror("sendto");
    }
}