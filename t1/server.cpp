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
    //criando o socket do servidor
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverSocket == -1) {
        cerr << "Failure at creating socket!" << endl;
        return -1;
    }

    //escolhendo ip e porta para o servidor escutar 
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; //vai escolher um endereço de ip automaticamente pro server
    serverAddress.sin_port = htons(51000); //vai escolher uma porta aleatória que eteja disponível

    //conectando servidor a porta selecionadad anteriormente
    if(bind(serverSocket, (struct sockaddr *) &serverAddress, (socklen_t) sizeof(serverAddress)) != 0) {
        cerr << "Can not connect to the IP/Port" << endl;
        return -1;
    }

    //escutando a porta
    if(listen(serverSocket, QUEUELIM) != 0) cerr << "Can not listen!" << endl;

    
    struct sockaddr_in client; // guarda o endereço do cliente que acabou de se conectar ao servidor
    socklen_t clientSize = sizeof(client);
    //aceitando conexão com o cliente
    int clientSocket = accept(serverSocket, (struct sockaddr *) &client, &clientSize);

    //isso aqui é só pra pegar o socket que se conectou ao server e mostrar na tela
    char hostClient[NI_MAXHOST];
    char svcClient[NI_MAXSERV];

    getnameinfo((sockaddr*)&client, sizeof(client), hostClient, NI_MAXHOST, svcClient, NI_MAXSERV, 0); 
    
    cout << hostClient << " connected on port " << svcClient << endl;

    close(serverSocket);

    string msg;//string auxiliar pra receber as mensagens recebidas e depois exibilas na tela

    char buffer[4096];//buffer parar se comunicar com o cliente

    int bytesReceived, bytesSend;//contador de bytes, respectivamente, recebidos e enviados, utilizados para verificação de erros

    while(true){
        //limpando msg pra poder mostrar certinho dps
        msg = "";

        //limpando buffer pra poder receber a mensagem do cliente
        memset(buffer, 0, 4096);
        
        //recebendo mensagem
        bytesReceived = recv(clientSocket, buffer, 4096, 0);

        if(bytesReceived == -1) {
            cout << "Error in connection!" << endl;
            break;          
        }
        if(bytesReceived == 0) {
            cout << "Client disconnected!" << endl;
            break;
        }

        //verificando se a mensagem recebida terminou ou tem mais do que 4096 caracteres
        //o valor 4 na última posição do buffer serve pra indicar que ela não terminou
        while(buffer[4095] == 4){
            //apagando o último caracteer que indica que ainda tem mais mensagem
            buffer[4095] = 0;
            
            //guardando o pedaçode 4095 caracteres da mensagem recebida
            msg += buffer;  

            //enviando os 4096 primeiros caracteres de auxBuffer que já estão em buffer
            bytesSend = send(clientSocket, buffer, 4096, 0);

            //limpando o buffer pra receber mensagem novamente do cliente
            memset(buffer, 0, 4096);
            
            //recebendo mensagem
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if(bytesReceived == -1) {
                cout << "Error in connection!" << endl;
                break;          
            }
            if(bytesReceived == 0) {
                cout << "Client disconnected!" << endl;
                break;
            }
        }

        /* caso a mensagem tenha menos do que 4096 caracteres, ou seja o que restou da mensagem com mais de 4096 caracteres, 
        ela é colocada no buffer e enviada ao cliente */
        msg += buffer;

        //enviando caracteres de auxBuffer que já estão em buffer
        bytesSend = send(clientSocket, buffer, 4096, 0);

        cout << endl << "Recebido: " << msg << endl; 
    }

    close(clientSocket);

    return 0;
}
