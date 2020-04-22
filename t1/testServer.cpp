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

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //vai escolher um endereço de ip automaticamente pro server
    serverAddress.sin_port = htons(5100); //vai escolher uma porta aleatória que eteja disponível

   
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverSocket == -1) {
        cerr << "Failure at creating socket!" << endl;
        return -1;
    }
   
    if(bind(serverSocket, (struct sockaddr *) &serverAddress, (socklen_t) sizeof(serverAddress)) != 0) {
        cerr << "Can not connect to the IP/Port" << endl;
        return -1;
    }

    // char host[NI_MAXHOST];
    // char svc[NI_MAXSERV];

    // int getInfo = getnameinfo((struct sockaddr *) &serverAddress, (socklen_t) sizeof(serverAddress), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    
    // //isso aqui é só pra pegar o socket que se conectou ao server
    // char hostClient[NI_MAXHOST];
    // char svcClient[NI_MAXSERV];
    sockaddr_in client;

    // //mostrando o socket do sservidor pra dps de ter sido criado pegar e colocar ele no cliente pra conectar
    
    // if(getInfo) cout << "Bind sucucessfully done! Socket: " << host << ":" << svc << endl;  
    // else{
    //     inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    //     cout << host << " connect on port " << ntohs(serverAddress.sin_port) << endl;
    // }
    
    if(listen(serverSocket, QUEUELIM) != 0) cerr << "Can not listen!" << endl;

    
    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(serverSocket, (struct sockaddr *) &client, &clientSize);

    getnameinfo((sockaddr*)&client, sizeof(client), hostClient, NI_MAXHOST, svcClient, NI_MAXSERV, 0); 
    
    cout << hostClient << " connected on port " << svcClient << endl;

    close(serverSocket);

    
    string buffer;
    // buffer.resize(4096);

    int bytesReceived;

    while(true){
        memset(&buffer, 0, 4096);
        bytesReceived = recv(clientSocket, &buffer, 4096, 0);
        if(bytesReceived == -1) {
            cout << "Error in connection!" << endl;
            break;          
        }
        if(bytesReceived == 0) {
            cout << "Client disconnected!" << endl;
            break;
        }
        
        cout << "Recebido: " << string(buffer, 0, bytesReceived) << endl; 
        send(clientSocket, &buffer, buffer.size(), 0);
    }

    close(clientSocket);

    return 0;
}
