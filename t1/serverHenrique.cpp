/*
    Establishing a socket to the server side of application.
*/

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) {

    /*
        WHY SOCKADDR BESIDES SOCKADDR_IN?

        "Struct sockaddr is a general structure valid for any protocol. 
        It is the generic socket address type and any socket function 
        requires the pointer to this generic type. But struct sockaddr_in 
        is protocol specific, to be specific for IPv4 address family."

        Answer found on Quora: https://bit.ly/2VH4EFZ

    */

    // Creating a sockaddr_in which describes the internet protocol version 4 (ipv4)

    sockaddr_in server_address;
    socklen_t serverSize = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &server_address.sin_addr);

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
        cerr << "Can't bind to IP/port.";
        return -2;
    }

    // Listenning for connections by marking 'socket' as a passive connection, i.e., 'a socket
    // that will be used to accept incoming connection requests using accept()' later.

    if (listen(socket, SOMAXCONN) != 0) {
        cerr << "Can't listen to this port.";
        return -3;
    }

    // Accepting connections by extracting the first request on queue.
    // If everything goes right, 'server_address' receives the connecting
    // peer, 'address_length' receives the address' actual length and
    // 'clientSocket' receives socket's descriptor.

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    int clientSocket;

    clientSocket = accept(socket, (sockaddr *)&client, &clientSize);

    if (clientSocket == -1) {
        cerr << "Error on connecting to socket.";
        return -4;
    }

    close(socket);

    memset(host, 0, NI_MAXHOST);    // cleaning possible garbage on host
    memset(service, 0, NI_MAXSERV); // cleaning possible garbage on service

    // get computer information
    int info = getnameinfo((sockaddr *)&server_address, serverSize, host, NI_MAXHOST, service, NI_MAXSERV, 0);

    if (info)
        cout << host << " connect on " << service << endl;
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connect on port " << ntohs(server_address.sin_port) << endl;
    }

    char buff[4096];
    int bytesReceived = 0;

    char welcomeMsg[42] = "\nHello, there! Welcome to my TCP server.\n";
    char nickname[21], nicknameMsg[51] = "\nPlease, enter your nickname (max of 20 letters): ";
    char askMsg[29] = "Please, enter your message: ";

    send(clientSocket, welcomeMsg, sizeof(welcomeMsg) - 1, 0);             // sending welcome message
    send(clientSocket, nicknameMsg, sizeof(nicknameMsg) - 1, 0);           // asking user for a nickname
    recv(clientSocket, nickname, sizeof(nickname) - 1, 0); // waiting for user to input his nickname

    while (true) {

        memset(buff, 0, 4096); // cleaning the buffer

        send(clientSocket, askMsg, sizeof(askMsg) - 1, 0); // asking for a message
        bytesReceived = recv(clientSocket, buff, 4096, 0); // waiting for a message

        // checking for any errors
        if (bytesReceived == -1) {
            cerr << "error on receiving message" << endl;
            break;
        }

        if (bytesReceived == 0) {
            cout << "client disconnected" << endl;
            break;
        }

        cout << "Msg from " << nickname << string(buff, 0, bytesReceived) << endl; // display message

        // send(clientSocket, buff, bytesReceived+1, 0); // ressend message
    }

    close(clientSocket);

    return 1;
}