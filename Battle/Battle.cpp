#include <iostream>
#include "Server.h"

using namespace std;


int main(int argc, char* argv[]) {
    Server server;
    
    if (strcmp(argv[1], "--start") == 0)
        server.startServer();
    else if (strcmp(argv[1], "--player") == 0) {
        const string strMsg = PLAYER + "," + argv[2] + "," + argv[3];
        server.enroll(strMsg);
    }
    else if (strcmp(argv[1], "--monster") == 0) {
        const string strMsg = MONSTER + "," + argv[2] + "," + argv[3];
        server.enroll(strMsg);
    }
    else if (strcmp(argv[1], "--fight") == 0)
        server.enroll(FIGHT);
    else
        cout << "Command [" << argv[1] << "] not exists." << endl;
    

    return 0;
}
