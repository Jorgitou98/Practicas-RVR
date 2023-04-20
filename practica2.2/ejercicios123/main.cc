#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = 80 * sizeof(char) + 2* sizeof(int16_t);
        alloc_data(data_size);

        char * my_data = _data;
        memcpy(my_data, name, 80 * sizeof(char));
        my_data += 80 * sizeof(char);
        memcpy(my_data, &x, sizeof(int16_t));
        my_data += sizeof(int16_t);
        memcpy(my_data, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char * my_data = data;
        memcpy(name, my_data, 80 * sizeof(char));
        my_data += 80 * sizeof(char);
        memcpy(&x, my_data, sizeof(int16_t));
        my_data += sizeof(int16_t);
        memcpy(&y, my_data, sizeof(int16_t));
        return 0;
    }

    char* get_name(){
        return name;
    }

    int16_t get_x(){
        return x;
    }

    int16_t get_y(){
        return y;
    }

private:
    char name[80];
    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("./data", O_CREAT | O_TRUNC | O_RDWR, 666);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    if (write(fd, one_w.data(), one_w.size()) == -1){
        perror("write");
        exit(1);
    }
    close(fd);
    // 3. Leer el fichero
    fd = open("./data", O_RDONLY, 666);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    char buf[500];
    int nread = read(fd, buf, 500);
    if (nread == -1){
        perror("read");
        exit(1);
    }
    close(fd);
    // 4. "Deserializar" en one_r
    one_r.from_bin(buf);

    // 5. Mostrar el contenido de one_r
    std::cout << one_r.get_name() << '\n';
    std::cout << one_r.get_x() << '\n';
    std::cout << one_r.get_y() << '\n';
    return 0;
}

