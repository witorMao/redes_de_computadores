/*
    Establishing a socket to the client side of application.
*/

#include <algorithm>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

bool fsigint = false; // bool to check if we caught a SIGINT
struct sigaction sigIntHandler;

int quit() {

    char c;

    while (true) {

        fsigint = false;
        fflush(stdin);
        cin.clear();

        cin >> c;

        if (c == 'y' || c == 'Y' || (cin.eof() && !fsigint)) {
            cout << "Bye!!\n";
            return 0;
        }

        else if (c == 'n' || c == 'N') {
            cout << "Ok. Type /help if you want to see your options again.\n";
            return 1;
        }

        else {
            cout << "Not valid. Press 'y' if you want to leave the application or 'n' otherwise.\n";
            continue;
        }
    }
}

void help() {

    cout << "Here are the commands that you can use to interact with this application:\n\n"
         << "1) /connect - this is used to connect to a server, you will need the ip and the port that the server is listening\n"
         << "2) /nickname yourNickname - you need to provide your nickname before sending messages\n"
         << "3) /ping - you can use this to test if you are connected to a server, if everything is fine, you will see a pong message on your screen sent by the server\n"
         << "4) /quit or press ctrl+D on your keyboard - when you want to disconnect from the server, you can use this commands\n"
         << "5) /help - to show help, this can be used wheter you are connected or not\n\n"
         << "P.S.: When connected to a server, to send messages and use this commands, first provide your nickname and, if everything goes right, you'll be able to send messages. To do so, type what you want, press enter and then type /send and press enter again.\n\n"
            "Enjoy!\n\n";
    return;
}

void my_handler(int s) {

    if (s == SIGINT) {
        cout << "\nCaught signal 2 (SIGINT). Do you want to exit? If so, type /quit.\n";
        fsigint = true;
        fflush(stdin); // clearing stdin
        cin.clear();   // clearing cin

    } else
        cout << "\nCaught signal " << s << "\n";
}

void threadReceive(int socket, string nicknameClient) {

    char buff[4096]; // All messages from the server will be stored here
    char nickname[50];
    int bytesReceived = 0; // Variable responsible to store results of recv()
    string serverResponse; // Stores server response in order to do a later verification
    string aux = "You disconnected! Bye.";

    while (true) {

        memset(nickname, 0, 50); // Cleaning the buffer

        bytesReceived = recv(socket, nickname, sizeof(nickname), 0); // Receiving nickname from server

        if (bytesReceived < 0)
            cout << "Error on getting a server response.\n\n";

        else {
            if (bytesReceived > 0) {

                // if the nickname received is the same as current client then is /ping or /quit command
                if (nicknameClient.compare(nickname) == 0) {

                    memset(buff, 0, 4096); // Cleaning the buffer

                    bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                    // Checks if user entered CTRL+D or wants to quit
                    if (strcmp(buff, aux.c_str()) == 0) {
                        cout << aux << "\n";
                        return;
                    }

                    // Checks if user is testing connection with server
                    else if (strcmp(buff, "pong") == 0) {
                        cout << "pong\n";
                        memset(buff, 0, 4096); // Cleaning the buffer
                    }
                }

                else {

                    serverResponse = "";
                    memset(buff, 0, 4096);                       // Cleaning the buffer
                    bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                    if (strcmp(buff, " disconnected!\n") == 0)
                        cout << nickname << buff << "\n\n";

                    else {
                        while (buff[4095] == 4) {

                            buff[4095] = 0;
                            serverResponse += buff;

                            memset(buff, 0, 4096); // Cleaning the buffer
                            bytesReceived = 0;

                            bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                            if (bytesReceived == -1)
                                cout << "Error on getting a server response.\n\n";
                        }

                        serverResponse += buff;

                        // Printing the messages received fom the server
                        cout << "\n> " << nickname << ": ";
                        cout << serverResponse + "\n";
                    }
                }
            }
        }
    }
}

void threadSend(int socket) {

    int send = 0; // Variable responsible for store results of send()
    string input; // All user input messages will be stored here
    string aux2;  // Second auxiliar string

    char buff[4096];   // All messages from the server will be stored here
    string auxCompare; // Aux string to verify server messages. It stores messages received in the program start

    string serverResponse;   // Stores server response in order to do a later verification
    bool bigMessage = false; // Flag to know if we're sending a big message (more than 4096 bytes)
    int bytesReceived = 0;   // Variable responsible to store results of recv()

    while (true) {

        // Reset serverResponde and auxCompare at each loop
        send = 0;
        input.clear();
        aux2.clear();
        memset(buff, 0, 4096); // Cleaning the buffer
        auxCompare.clear();
        serverResponse.clear();
        bigMessage = false;
        bytesReceived = 0;
        fsigint = false;

        // Now, client app will read everything until it founds a /send, which its the command to send the message.
        // This allows the user to send more than a paragraph per message
        while (true) {

            fsigint = false;
            fflush(stdin);
            cin.clear();
            getline(cin, aux2);

            if (cin.eof() && !fsigint) {
                ::send(socket, "/quit", 4096, 0); // Send the message to server
                // cout << "You disconnected! Bye.";
                input += "/quit";
                break;
                // return;
            }

            else if (aux2.compare("/send") != 0) {
                input += aux2;
                input += "\n";
            }

            else {
                if (input.size() == 0) {
                    cout << "You can't send nothing, please write something.\n";
                    continue;
                }
                input.erase(input.size() - 1); // Removing the last "\n" that was added before
                break;
            }
        }

        cout << "\n< You: " << input << "\n\n";

        if (input.compare("/help") == 0) {
            help();
            continue;
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

            cerr << "Could not send the message! Try again!\n";
            continue;
        }

        if (bytesReceived == -1) {
            cerr << "Error in connection!";
            break;
        }

        // User wants to quit the application
        if (input.compare("/quit") == 0) {
            return;
        }

        memset(buff, 0, 4096); // Cleaning the buffer send = 0;
        bigMessage = false;
        auxCompare.clear();
        serverResponse.clear();
        input.clear();
        aux2.clear();
    }
}

int connectToServer() {

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
    getchar();
    getline(cin, ipAddress);

    if (cin.eof() && !fsigint) {
        cout << "Bye!!\n";
        return 0;
    }

    // Getting server's port
    cout << "Enter port: ";
    cin >> port;

    if (cin.eof() && !fsigint) {
        cout << "Bye!!\n";
        return 0;
    }

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

    int send = 0; // Variable responsible for storing results of send()
    string input; // All user input messages will be stored here

    char buff[4096]; // All messages from the server will be stored here
    string nickname; // Variable to store user nickname

    int bytesReceived = 0; // Variable responsible to store results of recv()
    char bigNickname[67] = "\nYour nickname is bigger than 50 letters. Please, enter it again: ";

    bytesReceived = recv(socket, buff, 107, 0); // Receiving welcome message from server

    cout << "\n" + string(buff, bytesReceived); // Printing welcome message from server

    getchar(); // Removing '\n' present in buffer from past inputs

    // Trying to get user's nickname
    while (true) {

        getline(cin, input);

        if (cin.eof() && !fsigint) {
            close(socket);
            return 0;
        }

        // Checks if user is trying to enter his nickname
        else if (input.size() > 10 && strncmp(input.c_str(), "/nickname", 9) == 0) {

            nickname = input.substr(10, input.length());

            // Checks if user entered a nickname full of spaces
            if (all_of(nickname.begin(), nickname.end(), ::isspace)) {
                cout << "Invalid nickname. Please, do not provide an empty nickname. Try again.\n\n";
                continue;
            }

            // Checking nickname length (must contain less than 50 letters)
            else if (nickname.size() > 50) {

                while (true) {

                    fsigint = false;

                    cout << bigNickname; // Printing error message

                    fflush(stdin);
                    cin.clear();
                    getline(cin, nickname); // Saving new client's nickname

                    if (cin.eof() && !fsigint) {
                        cout << "Bye!\n";
                        close(socket);
                        return 0;
                    }

                    if (nickname.size() <= 50)
                        break;
                }
            }

            break;
        }

        // Checks if user is trying to quit the application
        else if ((input.compare("/quit")) == 0) {
            cout << "You are currently connected. Do you want to leave the application? Press 'y' if so or 'n' otherwise.\n";
            if (quit() == 0) {
                close(socket);
                return 0;
            } else
                continue;
        }

        // Checks if user is trying to get help
        else if ((input.compare("/help")) == 0) {
            help();
        }

        cout << "\nBefore continuing, you'll need to provide a nickname by entering /nickname yourNickname.\n\n";
    }

    int bytesSend = 0;
    bytesSend = ::send(socket, nickname.c_str(), nickname.size(), 0); // Sending nickname

    if (bytesSend > 0)
        bytesReceived = recv(socket, buff, 56, 0); // Receiving confirmation message from server

    if (bytesReceived > 0) {

        cout << "\n" + string(buff, bytesReceived) << "\n"; // Displaying received message

        thread sending(threadSend, socket);
        thread receiving(threadReceive, socket, nickname);

        sending.join();
        receiving.join();
    }

    close(socket);

    return 0;
}

int main(int argc, char const *argv[]) {

    // Checking for SIGNINT
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    cout << "Welcome to this chat application!\n\n";

    char c;
    int value;
    string input;

    help();

    while (true) {

        // Clearing stdin and cin buff and set fsigint flag to false to indicate we havent receive any SIGINT
        fflush(stdin);
        cin.clear();
        fsigint = false;

        getline(cin, input);

        if (cin.eof() && !fsigint) {
            cout << "Bye!!\n";
            return 0;
        }

        else if (strncmp(input.c_str(), "/nickname", 9) == 0)
            cout << "You need to connect to a server first. Use /connect to do this.\n";

        else if (input.compare("/connect") == 0) {

            value = connectToServer();

            if (value == -1 || value == -2)
                cout << "Error in stablishing a connection with the server. Try again.\n\n";

            else {
                cout << "Thank you for using this application. Bye.\n";
                break;
            }
        }

        else {

            if (input.compare("/ping") == 0) {
                cout << "You need to connect to a server first. Use /connect to do this.\n";
            }

            else {

                if (input.compare("/quit") == 0) {
                    cout << "You are not currently connected. Do you want to leave the application? Press 'y' (or ctrl+d) if so or 'n' otherwise.\n";

                    if (quit() == 0)
                        return 0;
                    else
                        continue;
                }

                else {
                    if (input.compare("/help") == 0)
                        help();

                    else if (!fsigint && !input.empty()) {
                        cout << "Your command is not valid. Please try again. If you need help type /help.\n";
                    }
                }
            }
        }
    }

    return 0;
}