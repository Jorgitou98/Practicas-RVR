#ifndef EJERCICIO_2_H
#define EJERCICIO_2_H


class ThreadMsg{
    public:
        ThreadMsg(int sfd); 
        void msg_task();
    private:
        int sfd;
};


#endif