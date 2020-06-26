#include "classChannel.h"

class Channel {

    string name;       // name of channel (max of 20 chars)
    vector<int> members; // vector of members (will store each clientSocket)
    int admin;           //administrator of the channel

public:
    // Constructor of Channel class
    Channel(string name, int admin) {
        this->name = name;
        this->members.insert(admin);
        this->admin = admin;
    }

    // Method to search by an user in a channel using its socket as key
    int findMember(int clientSocket) {

        vector<int>::iterator it;

        for (it = this->members.begin(); it < this->members.end(); it++)
            if (this->members.at(*it) == clientSocket)
                return *it;

        return -999;
    }

    // Method to add an user to a channel by adding its socket to members list
    void addUser(int clientSocket) {

        // Checking if member is already in this channel
        if (find(this->members.begin(), this->members.end(), clientSocket) != this->members.end()) {
            cout << "\nMember already in this channel.\n";
            return;
        }
        this->members.push_back(clientSocket); // adds new client to the end of members list of this channel
    }

    // Method to remove an user from a channel by removing its socket from members list
    void removeUser(int clientSocket) {

        for (vector<int>::iterator it = this->members.begin(); it < this->members.end(); it++) {

            // Checking if member is in this channel
            if (this->members.at(*it) == clientSocket) {
                this->members.erase(it); // deleting client from channel members vector
                return;
            }
        }
        cout << "\nMember not in this channel.\n";
    }
};