#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

// Compilar con: g++ -o ejercicio1 ejercicio1.cc

using namespace std;

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo* res;
    memset((void *) &hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0){
        cout << "Error: Name or service not known\n";
        exit(1);
    }
    for (struct addrinfo* p_res = res; p_res != NULL; p_res = p_res->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        if (getnameinfo((struct sockaddr *) p_res->ai_addr, p_res->ai_addrlen,
                       host, NI_MAXHOST,
                       serv, NI_MAXSERV, NI_NUMERICHOST) == 0){
            cout << host << ' ' << p_res->ai_family << ' ' << p_res->ai_socktype << '\n';
        }
        else{
            perror("getnameinfo");
            exit(1);
        }
    }
    freeaddrinfo(res);

    return 0;
}