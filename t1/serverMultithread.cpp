/*
    Establishing a socket to the server side of application.
*/

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <dequeue>


#include "classChannel.h"

using namespace std;


void newConnection(sockaddr_in server_address, int serverSize, int **clientSocket, int *num, int i, sockaddr_in client, char host[1025], char service[32]) {

    memset(host, 0, NI_MAXHOST);    // Cleaning possible garbage on host
    memset(service, 0, NI_MAXSERV); // Cleaning possible garbage on service

    // Getting computer information
    int info = getnameinfo((sockaddr *)&server_address, serverSize, host, NI_MAXHOST, service, NI_MAXSERV, 0);

    if (info) {
        // cout << host << " connected on " << service << "\n";
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        // cout << host << " connected on port " << ntohs(server_address.sin_port) << "\n";
    }

    string msg;
    string aux; // Auxiliar pra avisar q um cliente desconectou
    string auxToClientResponse;

    char buff[4096];
    char nickname[20];
    char auxBuff[4096];
    int bytesSend = 0, bytesReceived = 0, bigNick = 0;
    char confirmNick[56] = "Your nickname is registered. Have fun!!\n\nStart to text:";
    char welcomeMsg[94] = "Hello, there! Welcome to my TCP server chat.\nPlease, enter your nickname (max of 20 letters):";

    memset(nickname, 0, 20); // Emptying nickname buffer

    for (int count = 0; count < 5; count++) {

        bytesSend = send((*clientSocket)[i], welcomeMsg, sizeof(welcomeMsg), 0); // Sending welcome message

        if (bytesSend < 0)
            continue;
        else {
            bytesSend = 0;
            break;
        }
    }

    bytesReceived = recv((*clientSocket)[i], nickname, sizeof(nickname), 0); // Waiting for user to input his nickname

    if (bytesReceived < 0)
        cout << "Error on receiving user's nickname.\n";

    for (int count = 0; count < 5; count++) {

        bytesSend = send((*clientSocket)[i], confirmNick, sizeof(confirmNick), 0); // Sending confirmation of nickname

        if (bytesSend < 0)
            continue;
        else {
            bytesSend = 0;
            break;
        }
    }

    string(nickname, 0, bytesReceived); // Storing client's nickname

    cout << "\n~ "
         << nickname << " entered on chat.\n";

    while (true) {

        aux = "";                 // Cleaning aux.
        msg.erase(0, msg.size()); // Cleaning message.
        memset(buff, 0, 4096);    // Cleaning buffer.
        bytesReceived = 0;        // Emptying bytesReceived.
        bytesSend = 0;            // Emptying bytesSend.

        bytesReceived = recv((*clientSocket)[i], buff, 4096, 0); // Waiting for a message

        // Checking for any errors
        if (bytesReceived == -1) {
            cerr << "Error on receiving message. Stopping.\n";
            return;
        }

        else if (bytesReceived > 0) {

            //implementation of /quit command
            if (strcmp(buff, "/quit") == 0) {

                for (int j = 0; j < SOMAXCONN; j++) {
                    if ((*clientSocket)[j] != -1) {

                        // sending to clients that one of them diconnected from the server
                        if (j != i) {

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], nickname, sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }

                            aux = " disconnected!\n";

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], aux.c_str(), aux.size(), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }

                        // sending to client that user diconnected from the server
                        else {

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], nickname, sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }

                            aux = "You disconnected! Bye.";

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], aux.c_str(), aux.size(), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }
                }

                // Removing client from the list of connections
                (*clientSocket)[i] = -1;
                (*num)--;
                cout << nickname << " disconnected.\n";
                memset(nickname, 0, 20); // Emptying nickname buffer
                return;
            }

            // Implementation of /ping command
            else {

                // cout << buff;

                if (strcmp(buff, "/ping") == 0) {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*clientSocket)[i], nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    aux = "pong";

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*clientSocket)[i], aux.c_str(), aux.size(), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                else {
                    //normal messages

                    //sending nickname
                    for (int j = 0; j < SOMAXCONN; j++) {
                        if ((*clientSocket)[j] != -1 && j != i) {
                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], nickname, sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }

                    // If the received message ends with value 4, then the message is bigger than 4096 bytes.
                    while (buff[4095] == 4) {

                        buff[4095] = 0;

                        memset(buff, 0, 4096); // Cleaning buffer.
                        bytesReceived = 0;     // Emptying var.

                        bytesReceived = recv((*clientSocket)[i], buff, 4096, 0); // Receiving

                        if (bytesReceived < 0)
                            cout << "Error on getting a server response.\n\n";

                        else if (bytesReceived == 0) {
                            // cout << nickname << " disconnected.\n";
                            // return;
                        }

                        else {
                            strcpy(auxBuff, buff);
                            msg += buff;

                            for (int j = 0; j < SOMAXCONN; j++) {

                                if ((*clientSocket)[j] != -1 && j != i) {

                                    for (int count = 0; count < 5; count++) {

                                        bytesSend = send((*clientSocket)[j], buff, 4096, 0);

                                        if (bytesSend < 0)
                                            continue;
                                        else {
                                            bytesSend = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    strcpy(auxBuff, buff);

                    // Sending received message for all the clients except the one who send the current message
                    for (int j = 0; j < SOMAXCONN; j++) {

                        if ((*clientSocket)[j] != -1 && j != i) {

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*clientSocket)[j], buff, 4096, 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }

                    // If the message is smaller than 4096 chars, i.e, the remaining of
                    // the big message, we are storing it in buff and sending to client.
                    buff[4095] = 0;
                    msg += buff;

                    cout << "\nMessage from " << nickname << ": " << msg << "\n"; // Displaying message
                }
            }
        }
    }
}

void newChannel(){
    // Accepting connections by extracting the first request on queue.
    // If everything goes right, 'server_address' receives the connecting
    // peer, 'address_length' receives the address' actual length and
    // 'clientSocket' receives socket's descriptor.
    int **clientSocket = new int *[SOMAXCONN];
    int *numberActualClientSockets = new int;

    int i = 0;
    socklen_t clientSize;
    int currentClientSocket;
    sockaddr_in client[SOMAXCONN];
    char host[SOMAXCONN][NI_MAXHOST];
    char service[SOMAXCONN][NI_MAXSERV];

    for (int i = 0; i < SOMAXCONN; i++) {
        clientSocket[i] = new int;
        (*clientSocket)[i] = -1;
    }

    *numberActualClientSockets = 0;

    // Will store the threads created to the new connections on the server
    thread threads[SOMAXCONN];

    while (true) {

        // Searching for a empty position to put the new connection
        for (int l = 0; l < SOMAXCONN; l++) {
            if ((*clientSocket)[l] == -1)
                currentClientSocket = l;
        }

        if (*numberActualClientSockets < SOMAXCONN) {

            clientSize = sizeof(client[currentClientSocket]);

            (*clientSocket)[currentClientSocket] = accept(socket, (sockaddr *)&client[currentClientSocket], &clientSize);

            if ((*clientSocket)[currentClientSocket] == -1) {
                cerr << "Error on connecting to socket.";
                return -4;
            }

            // Adding the one that will be created on the line above
            (*numberActualClientSockets)++;

            // Creating one thread for the requisition that just was accepted
            threads[currentClientSocket] = thread(newConnection, server_address, serverSize, clientSocket, numberActualClientSockets, currentClientSocket, client[currentClientSocket], host[currentClientSocket], service[currentClientSocket]);

            // Detach the new thread from the main thread
            threads[currentClientSocket].detach();

            // Add one more connection
            i++;
        }
    }

    for (int j = 0; j < SOMAXCONN; j++) {
        delete clientSocket[j];
    }

    delete[] clientSocket;
    delete numberActualClientSockets;

}

int main(int argc, char const *argv[]) {

    // Creating a sockaddr_in which describes the internet protocol version 4 (ipv4)
    int port = 53000;
    string ipAddress = "0.0.0.0";

    sockaddr_in server_address;
    socklen_t serverSize = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &server_address.sin_addr);

    cout << "Openned server on ip address " << ipAddress << " port " << port << "\n";

    // Creating a socket with IPv4 Internet protocols as communication domain and TCP as communication semantics.
    // AF_INET = IPv4 protocol, SOCK_STREAM = TCP.
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);

    // Checking for errors while creating a socket
    if (socket == -1) {
        cerr << "Can't create a socket.";
        return -1;
    }

    // Binding the created socket 'socket' to an available address 'server_address' of size 'sizeof(server_address)' bytes.
    // This step is commonly named as 'lqassigning a name to a socketrq'.
    if (bind(socket, (sockaddr *)&server_address, serverSize) != 0) {
        cerr << "Can't bind to IP/port.\n";
        return -2;
    }

    // Listenning for connections by marking 'socket' as a passive connection, i.e., 'a socket
    // that will be used to accept incoming connection requests using accept()' later.
    if (listen(socket, SOMAXCONN) != 0) {
        cerr << "Can't listen to this port.\n";
        return -3;
    }



    // // Accepting connections by extracting the first request on queue.
    // // If everything goes right, 'server_address' receives the connecting
    // // peer, 'address_length' receives the address' actual length and
    // // 'clientSocket' receives socket's descriptor.
    int **clientSocket = new int *[SOMAXCONN];
    for (int i = 0; i < SOMAXCONN; i++) {
        clientSocket[i] = new int;
        (*clientSocket)[i] = -1;
    }

    int *numberActualClientSockets = new int;
    *numberActualClientSockets = 0;

    int i = 0;
    socklen_t clientSize;
    int currentClientSocket;
    sockaddr_in client[SOMAXCONN];
    char host[SOMAXCONN][NI_MAXHOST];
    char service[SOMAXCONN][NI_MAXSERV];




    string showChannels = "These are the current channels open in this server:\n";
    string askOption = "Please type the correspondet name of the channel that you want to join, then type enter and type \"/send\" and enter again.\n";
    char buffer[4096];

    //list of channels
    dequeue<Channel> channels;
    int channelsNum = channels.size();

    //auxiliar to insert in the list
    Channel aux;
    string channelName;

    while(true){
        channelName = "";
        // Searching for a empty position to put the new connection
        for (int l = 0; l < SOMAXCONN; l++) {
            if ((*clientSocket)[l] == -1)
                currentClientSocket = l;
        }

        if (*numberActualClientSockets < SOMAXCONN) {

            clientSize = sizeof(client[currentClientSocket]);

            (*clientSocket)[currentClientSocket] = accept(socket, (sockaddr *)&client[currentClientSocket], &clientSize);

            if ((*clientSocket)[currentClientSocket] == -1) {
                cerr << "Error on connecting to socket.";
                return -4;
            }

            // Adding the one that will be created on the line above
            (*numberActualClientSockets)++;

            i++;

            bytesReceived = recv((*clientSocket)[currentClientSocket], buffer, sizeof(buffer), 0); // Waiting command from client
            
            if (strlen(buffer) > 10 && strncmp(buffer, "/join", 5) == 0) {
                channelName += buffer;
                channelName.erase(0, 6); //leaving only the name of the channel in the string

                for(int k = 0; k < channels.size(); k++){
                    //asking the client to choose a channel to join
                    if(strcmp(channels[k].name, channelName.c_str()) == 0){
                        channel[k].addUser((*clientSocket)[currentClientSocket]);
                    }
                }    

                //creating the channel with the received name
                aux(buffer, auxVector, (*clientSocket)[currentClientSocket]);
                channels.push_back(aux);

                //inserting the data of the new channel on the list of channels
                channels.insert(aux, channels.size()+1);

                //calling a function that will create a thread for the new channel
                threads[currentClientSocket] = thread(newChannel, *channels[channels.size()]);
                        
                // Detach the new thread from the main thread
                // // threads[currentClientSocket].detach();
            }
        }
    }

    close(socket);

    return 1;
}