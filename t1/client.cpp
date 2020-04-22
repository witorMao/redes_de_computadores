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
    //criando socket do cliente
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(clientSocket == -1) {
        cerr << "Failure at creating client socket!" << endl;
        return -1;
    }

    int port;
    string ipValue;
    //lendo endereço e porta do servidor ao qual o cliente vai se conectar
    cout << "Type \"ip port\" form the server: " << endl;
    cin >> ipValue >> port;

    //endereço onde o cliente vai se conectar
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //conectando cliente ao servidor
    int connectResult = connect(clientSocket, (struct sockaddr *) &serverAddress, (socklen_t) sizeof(serverAddress));
    if(connectResult == -1){
        cout << "Can not connect!" << endl;
    }

    string serverResponse;
    string auxBuffer; //string auxiliar pra ler input de boa
    auxBuffer.resize(4096);
    char buffer[4096]; //buffer que realmente vai ser utilizado nas funções de envio e recebimento de dados
    int bytesReceived; //recebe o retorno de recv(), para verificação de erros no recebimento de mensagens
    int bytesSend = -1; //recebe o retorno de send(), para verificação de erros no envio de mensagem
    bool bigMessage = false; //auxiliar pra verificação de mensagens com mais de 4096 bytes

    //mensagem pro servidor
    cout << "Type the message:" << endl;    

    while(true){
        //reinicializando serverResponse em todo loop
        serverResponse = "";

        //lendo entrada na string auxbuffer pra facilitar e mais pra frente copia os dados pra buffer que eh um array 
        //de char pra não bugar a send()  e a recv()
        cout << ">";

        //usa assim pra ler :
        getline(cin, auxBuffer);

        //n usa assim pra ler
        // cin >> auxBuffer;

        cout << endl <<  "auxBuffer: " << endl;
        cout << auxBuffer << endl;
    
        if(auxBuffer.size() > 4096) bigMessage = true;
        
        // se o tamanho da mensagem lida for maior do que 4096 caracteres, manda os 4096 primeiros e dps apaga eles do auxBuffer
        // isso se repete até o auxBuffer ficar com tamanho <= 4096, quando atingir isso ele vai pra a verificação de baixo que 
        // é pra mandar uma mensagem que pode ser enviada de uma vez só

        while(auxBuffer.size() > 4096){
            //limpando buffer pra poder enviar dados ao servidor
            memset(buffer, 0, 4096);

            //copiando os 4095 primeiros caracteres de auxBuffer pra buffer, o último caracter de buffer vai receber valor pra 
            //indicar que a msg n terminou ainda
            strncpy(buffer, auxBuffer.c_str(), 4095);
            
            //colocando  3 aqui só pra indicar que a msg n terminou ainda
            buffer[4095] = 3;
            
            //enviando os 4096 primeiros caracteres de auxBuffer que já estão em buffer
            bytesSend = send(clientSocket, buffer, 4096, 0);

            //aqui eu dou um break pq deu ruim nome meio de uma mensagem grande então, mando o maluco reenviar tudo
            if(bytesSend == -1) {
                cerr << "Could not send the message! Try again! while" << endl;
                break;
            }

            //limpando buffer pra receber resposta do servidor
            memset(buffer, 0, 4096);

            //recebendo resposta do servidor
            bytesReceived = recv(clientSocket, buffer, 4096, 0);

            serverResponse += buffer;

            //excluindo a ultima posição de server por ter 3 no valor
            if(buffer[4095] == 3) serverResponse.erase(serverResponse.size()-1); 

            if(bytesReceived == -1) {
                cerr << "Error in connection!" << endl;
                // break;
            }
            
            //apaga os 4095 primeiros caracteres do auxBuffer (os que acabaram de ser enviados) e deixa só o resto
            auxBuffer.erase(0, 4095);
        }

        //limpando buffer pra poder enviar dados ao servidor
        memset(buffer, 0, 4096);

        //copiando input (ou o que resta dele, caso tenha entrado no while de cima) pra buffer
        strncpy(buffer, auxBuffer.c_str(), auxBuffer.size()+1);

        //enviando caracteres de auxBuffer que já estão em buffer
        bytesSend = send(clientSocket, buffer, 4096, 0);

        if(bytesSend == -1) {
            if(bigMessage == true) continue; // se mensagem for > 4096 caracteres, já mostrou a mensagem la em cima
            cerr << "Could not send the message! Try again! 1" << endl;
            continue;
        }

        //limpando buffer pra receber resposta do servidor
        memset(buffer, 0, 4096);

        //recebendo resposta do servidor
        bytesReceived = recv(clientSocket, buffer, 4096, 0);
        
        serverResponse += buffer;

        if(bytesReceived == -1) {
            cerr << "Error in connection!" << endl;
            // break;
        }else cout << "Server response: " << serverResponse << endl;

    }   
 
    close(clientSocket);
    return 0;
}