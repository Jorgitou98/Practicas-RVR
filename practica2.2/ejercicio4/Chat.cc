#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char * aux = _data;
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    
    // Me quedo con 7 caracteres y con c_str() pongo el '\0', obteniendo el puntero
    memcpy(aux, nick.substr(0,7).c_str(), 8 * sizeof(char));
    aux += 8 * sizeof(char);
    memcpy(aux, nick.substr(0,79).c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char * aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    nick = string(aux, 8);
    aux += 8 * sizeof(char);
    message = string(aux, 80);
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        // Recibimos un mensaje. Queremos obtener el socket del cliente,
        // así que pasamos un puntero
        ChatMessage msg;
        Socket* sock_client;
        socket.recv(&msg, sock_client);
        std::unique_ptr<Socket> sock_client_up(sock_client);
        switch (msg.type){
            case LOGIN:            
                clients.push_back(move(sock_client_up));
                break;
            case LOGOUT:
                for (auto curr_client_it = clients.begin(); curr_client_it != clients.end(); ++i){
                    // El socket por el que vamos es el del cliente que nos ha hablado, lo borramos
                    if (**curr_client_it == *sock_client) clients.erase(curr_client_it);
                }
                break;  
            case MESSAGE:
                for (auto curr_client_it = clients.begin(); curr_client_it != clients.end(); ++i){
                    if (**curr_client_it != *sock_client) socket.send(&msg, **curr_client_it);
                }
                break;
            default:
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

