#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include "ejercicio6_2.h"

// Compilado como: g++ -pthread -o ejercicio6 ejercicio6_1.cc ejercicio6_2.cc
// Quizás hace falta añadir -std=c++11

using namespace std;

#define NUM_THREADS 5

int main(int argc, char *argv[]){
    struct addrinfo hints;
    struct addrinfo *result;
    int sfd;

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
    int s = getaddrinfo(argv[1], argv[2], &hints, &result);
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
    ThreadMsg* thmsgs[NUM_THREADS];
    // Creamos los hilos (detach para eliminar automaticamente sus recursos cuando acabe)
    for (int i = 0; i < NUM_THREADS; ++i){
        ThreadMsg* thmsg = new ThreadMsg(sfd);
        thmsgs[i] = thmsg;
        thread([&thmsg](){
            thmsg->msg_task();
        }).detach();
    }
    string input;
    do{
        cin >> input;
    } while(input != "q");

    cout << "Finalizando hilos...\n";
    // Destruyo las clases de los hilos restantes
    for (int i = 0; i < NUM_THREADS; ++i){
        delete thmsgs[i];
    }
    close(sfd);
    return 0;
}