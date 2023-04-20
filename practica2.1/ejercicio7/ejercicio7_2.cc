#include "ejercicio7_2.h"
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

#define BUF_SIZE 100

using namespace std;


ThreadMsg::ThreadMsg(int cfd){
    this->cfd = cfd;
}

void ThreadMsg::msg_task(){
     // Leer mensajes, mostrar de quien vienen y responder con la hora
    char buf[BUF_SIZE];
    ssize_t nread;
    for (;;) {
        nread = recv(cfd, buf, BUF_SIZE, 0);
        buf[nread] = '\0';
        if (nread == -1)
            continue;            
        if (nread == 0){
            cout << "Cliente cerrado\n";
            close(cfd);
            break;
        }
        if (send(cfd, buf, nread, 0) != nread)
            fprintf(stderr, "Error sending response\n");
    }
}
