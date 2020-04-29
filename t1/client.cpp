/*
    Establishing a socket to the client side of application.
*/

#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) {

    // Creating a client socket to connect with the server
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (socket == -1) {
        cout << "Can't create a socket\n";
        return -1;
    }

    // Creating a hint for the server
    int port;
    string ipAddress;
    sockaddr_in server;

    // Getting server's ip address
    cout << "Enter ip address: ";
    getline(cin, ipAddress);

    // Getting server's port
    cout << "Enter port: ";
    cin >> port;

    // Adressing some info in order to connect to the server
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr);

    // Connecting to the server on 'socket'
    int connect = ::connect(socket, (sockaddr *)&server, sizeof(server));

    if (connect == -1) {
        cout << "Can't connect\n";
        return -2;
    }

    int send = 0;          // Variable responsible for store results of send()
    string input;          // All user input messages will be stored here
    char buff[4096];       // All messages from the server will be stored here
    string nickname;       // Variable to store user nickname
    int bytesReceived = 0; // Variable responsible to store results of recv()
    char bigNickname[67] = "\nYour nickname is bigger than 20 letters. Please, enter it again: ";

    bytesReceived = recv(socket, buff, 89, 0);      // Receiving welcome message from server
    cout << "\n" << string(buff, bytesReceived);    // Printing welcome message from server
    getchar();                                      // Removing '\n' present in buffer from past inputs
    getline(cin, nickname);                         // Saving client nickname

    if (nickname.size() > 20) {

        while (true) {

            cout << bigNickname;    // Printing error message
            getline(cin, nickname); // Saving new client's nickname

            if (nickname.size() <= 20)
                break;
        }
    }

    ::send(socket, nickname.c_str(), nickname.size(), 0);   // Sending nickname
    bytesReceived = recv(socket, buff, 40, 0);              // Receiving confirmation message from server
    cout << "\n" << string(buff, bytesReceived) << "\n";    // Displaying received message

    string aux2;             // Second auxiliar string
    string auxCompare;       // Aux string to verify server messages. It stores messages received in the program start
    string serverResponse;   // Stores server response in order to do a later verification
    bool bigMessage = false; // Flag to know if we're sending a big message (more than 4096 bytes)

    while (true) {

        // Reset serverResponde and auxCompare at each loop
        serverResponse = "";
        auxCompare = "";

        cout << "\nType your message:\n> ";

        // Now, client app will read everything until it founds a /send, which its the command to send the message.
        // This allows the user to send more than a paragraph per message
        while (true) {

            getline(cin, aux2);

            if (aux2.compare("/send") != 0) {

                // User wants to quit the application
                if (aux2.compare("/quit") == 0)
                    return 0;

                input += aux2;
                input += "\n";

            } else {
                input.erase(input.size() - 1); // Removing the last "\n" that was added before
                break;
            }
        }

        // Checking if it's a big message (more than 4096 bytes)
        if (input.size() > 4096)
            bigMessage = true;

        auxCompare = input; // Storing input to check if the sent message was correctly received

        while (input.size() > 4096) {

            memset(buff, 0, 4096); // Cleaning the buffer
            bytesReceived = 0;

            strncpy(buff, input.c_str(), 4095); // Copying 'input' to buff

            buff[4095] = 4; // Adding a '4' just to identify that the message isn't finish yet

            send = ::send(socket, buff, 4096, 0); // Send the message to server

            if (send == -1) {
                cout << "Couldn't send to server. Maybe you should try again.";
                continue;
            }

            // Cleaning buffer to receive server response
            memset(buff, 0, 4096);

            bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

            if (buff[4095] == 4)
                buff[4095] = 0;

            serverResponse += buff;

            if (bytesReceived == -1)
                cout << "Error on getting a server response.\n\n";

            input.erase(0, 4095); // Erases first 4096 bytes of auxBuffer
        }

        memset(buff, 0, 4096); // Cleaning buffer to send data to server

        strncpy(buff, input.c_str(), input.size()); // Copying input to buff

        send = ::send(socket, buff, 4096, 0); // Sending bytes from auxBuffer that are in buffer already

        if (send == -1) {

            if (bigMessage == true)
                continue; // If messages if bigger than 4096 chars, we've showed the message already

            cerr << "Could not send the message! Try again! 1" << endl;
            continue;
        }

        memset(buff, 0, 4096);                       // Cleaning buffer to receive server response
        bytesReceived = recv(socket, buff, 4096, 0); // getting server response
        serverResponse += buff;

        if (bytesReceived == -1) {
            cerr << "Error in connection!" << endl;
            break;
        } else if (serverResponse.compare(auxCompare) == 0)
            cout << "\nMessage sent!\n";

        input.clear();
        aux2.clear();
    }

    close(socket);

    return 0;
}