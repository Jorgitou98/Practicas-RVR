#ifndef EJERCICIO_7_2_H
#define EJERCICIO_7_2_H

//Clase para gestionar los threads
class ThreadMsg{
    public:
        //Costructora
        ThreadMsg(int cdf);
        //Método a llamar en los threads
        void msg_task();
    private:
        int cfd;
};


#endif