#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{

    // Creating a client socket to connect with the server
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (socket == -1)
    {
        cout << "Can't create a socket\n";
        return -1;
    }

    // Creating a hint for the server
    int port;
    string ipAddress;
    sockaddr_in server;

    // Getting server's ip address
    cout << "Enter ip address: "
         << "\n";
    getline(cin, ipAddress);

    // Getting server's port
    cout << "Enter port: "
         << "\n";
    cin >> port;

    // Adressing some info in order to connect to the server
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr);

    // Connecting to the server on 'socket'
    int connect = ::connect(socket, (sockaddr *)&server, sizeof(server));

    if (connect == -1)
    {
        cout << "Can't connect\n";
        return -2;
    }

    int send = 0;          // variable responsible to store results of send()
    string input;          // all the user input messages will be stored here
    char buff[4096];       // all the messages from the server will be stored here
    string nickname;       // variable to store users nickname
    int bytesReceived = 0; // variable responsible to store results of recv()
    char bigNickname[67] = "\nYour nickname is bigger than 20 letters. Please, enter it again: ";
    bytesReceived = recv(socket, buff, 91, 0);   // receiving welcome message from server
    cout << string(buff, bytesReceived) << "\n"; // printing welcome message from server
    getchar();                                   // removing the '\n' present in the buffer from past inputs
    getline(cin, nickname); // saving client's nickname

    if (nickname.size() > 20)
    {

        while (true)
        {
            cout << bigNickname << "\n"; // printing error message
            getline(cin, nickname);      // saving new client's nickname

            if (nickname.size() <= 20)
            {
                ::send(socket, nickname.c_str(), nickname.size(), 0); // sending new nickname
                bytesReceived = recv(socket, buff, 40, 0);            // receiving confirmation message from server
                cout << string(buff, bytesReceived) << "\n";          // displaying received message
                break;
            }
        }
    }

    else
    {
        ::send(socket, nickname.c_str(), nickname.size(), 0); // sending nickname
        bytesReceived = recv(socket, buff, 40, 0);            // receiving confirmation message from server
        cout << string(buff, bytesReceived) << "\n";          // displaying received message
    }

    string serverResponse;//armazena a resposta recebida do servidor, pra realizar uma verificação com a entrada depois
    string auxCompare; //string auxiliar pra verificação das mensagens recebidas pelo servidor, guarda as mensagens recebidas na entrada do programa
    string auxDoAux;
    bool bigMessage = false; //auxiliar pra verificação de mensagens com mais de 4096 bytes

    while (true)
    {
      //reinicializando serverResponse em todo loop
        serverResponse = "";
        auxCompare = "";

        cout << "Type your message:" << endl
             << "> ";

        //lendo tudo quanto é tipo de entrada até algum ser "/enter", que é o comando pra enviar msg
        //isso só foi feito pra permitir que o usuario digite mais de um parágrafo em uma mensagem
        while (true)
        {
            getline(cin, auxDoAux);
            if (auxDoAux.compare("/send") != 0)
            {
                //saindo fechando a conexão
                if (auxDoAux.compare("/quit") == 0)
                    return 0;

                input += auxDoAux;
                input += "\n";
            }
            else
            {
                input.erase(input.size() - 1); //tirando o ultimo "\n" que foi adicionado ali em cima
                break;
            }
        }

        if (input.size() > 4096)
            bigMessage = true;
        
        auxCompare = input;//guardando input nessa string auxiliar pra depois verificar se a mensagem enviada foi recebida corretamente

        while (input.size() > 4096)
        {
            memset(buff, 0, 4096); // cleaning the buffer
            bytesReceived = 0;

            //copiando string input no array de caracteres buff
            strncpy(buff, input.c_str(), 4095);

            //colocando 4 aqui só pra indicar que a msg n terminou ainda
            buff[4095] = 4;

            send = ::send(socket, buff, 4096, 0); // send the message to the server

            if (send == -1)
            {
                cout << "Couldn't send to server. Maybe you should try again.";
                continue;
            }

            //limpando buffer pra receber resposta do servidor
            memset(buff, 0, 4096);

            bytesReceived = recv(socket, buff, 4096, 0); // receiving message from server

            if(buff[4095] == 4) buff[4095] = 0;

            serverResponse += buff;
            // serverResponse.erase(serverResponse.size()-1);

            if (bytesReceived == -1)
                cout << "Error on getting a server response.\n\n";

            //apaga os 4095 primeiros caracteres do auxBuffer (os que acabaram de ser enviados) e deixa só o resto
            input.erase(0, 4095);
        }

        //limpando buffer pra poder enviar dados ao servidor
        memset(buff, 0, 4096);

        //copiando input (ou o que resta dele, caso tenha entrado no while de cima) pra buffer
        strncpy(buff, input.c_str(), input.size());

        //enviando caracteres de auxBuffer que já estão em buffer
        send = ::send(socket, buff, 4096, 0);

        if (send == -1)
        {
            if (bigMessage == true)
                continue; // se mensagem for > 4096 caracteres, já mostrou a mensagem la em cima
            cerr << "Could not send the message! Try again! 1" << endl;
            continue;
        }

        //limpando buffer pra receber resposta do servidor
        memset(buff, 0, 4096);

        //recebendo resposta do servidor
        bytesReceived = recv(socket, buff, 4096, 0);

        serverResponse += buff;

        if (bytesReceived == -1)
        {
            cerr << "Error in connection!" << endl;
            break;
        }
        else if(serverResponse.compare(auxCompare) == 0) cout << "Message sent!" << "\n";
    
        input.clear();
        auxDoAux.clear();
    }

    close(socket);

    return 0;
}