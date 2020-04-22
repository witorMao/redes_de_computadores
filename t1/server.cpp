#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#define QUEUELIM 10

using namespace std;

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverSocket == -1) {
        cerr << "Failure at creating socket!" << endl;
        return -1;
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; //vai escolher um endereço de ip automaticamente pro server
    serverAddress.sin_port = htons(51000); //vai escolher uma porta aleatória que eteja disponível

    if(bind(serverSocket, (struct sockaddr *) &serverAddress, (socklen_t) sizeof(serverAddress)) != 0) {
        cerr << "Can not connect to the IP/Port" << endl;
        return -1;
    }

    if(listen(serverSocket, QUEUELIM) != 0) cerr << "Can not listen!" << endl;

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &client, &clientSize);

    //isso aqui é só pra pegar o socket que se conectou ao server
    char hostClient[NI_MAXHOST];
    char svcClient[NI_MAXSERV];

    getnameinfo((sockaddr*)&client, sizeof(client), hostClient, NI_MAXHOST, svcClient, NI_MAXSERV, 0); 
    
    cout << hostClient << " connected on port " << svcClient << endl;

    close(serverSocket);

    string msg;
    memset(&msg, 0, msg.size());

    char buffer[4096];

    int bytesReceived, bytesSend;

    while(true){
        memset(buffer, 0, 4096);
        
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if(bytesReceived == -1) {
            cout << "Error in connection!" << endl;
            break;          
        }
        if(bytesReceived == 0) {
            cout << "Client disconnected!" << endl;
            break;
        }

        msg = buffer;

        //enviando caracteres de auxBuffer que já estão em buffer
        bytesSend = send(clientSocket, buffer, 4096, 0);
        
        //3 na tabela ascii significa fim de transmissão, por isso coloquei aqui
        while(buffer[4095] == 3){
            //apagando o último caracter da msg, pois ele tem o valor 3
            msg.erase(msg.size()-1); 

            memset(buffer, 0, 4096);
            
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if(bytesReceived == -1) {
                cout << "Error in connection!" << endl;
                break;          
            }
            if(bytesReceived == 0) {
                cout << "Client disconnected!" << endl;
                break;
            }

            msg += buffer;        

            //enviando os 4096 primeiros caracteres de auxBuffer que já estão em buffer
            bytesSend = send(clientSocket, buffer, 4096, 0);

        }
        
        cout << "Recebido: " << msg << endl; 

    }

    close(clientSocket);

    return 0;
}
