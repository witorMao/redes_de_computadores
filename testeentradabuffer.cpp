#include <iostream>

using namespace std;

int main(){
    char buffer[4096];
    streambuf *input;
    string aux;
    getline(cin, aux);

    input = cin.rdbuf();
    // cout << input->pubseekoff(0, cin.end);
    // input->sgetn(buffer, 2);
    cout << cin.rdbuf()->in_avail();

    cout << endl << buffer << endl;
}


