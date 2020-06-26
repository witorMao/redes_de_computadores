#include <string>
#include <vector>

class Channel {

    string name;       // name of channel (max of 20 chars)
    vector<int> members; // vector of members (will store each clientSocket)
    int admin;           //administrator of the channel

public:
    // Constructor of Channel class
    Channel(string name, vector<int> members, int admin);

    // Method to search by an user in a channel using its socket as key
    int findMember(int clientSocket);

    // Method to add an user to a channel by adding its socket to members list
    void addUser(int clientSocket);

    // Method to remove an user from a channel by removing its socket from members list
    void removeUser(int clientSocket);
};