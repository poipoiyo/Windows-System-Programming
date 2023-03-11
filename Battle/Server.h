#pragma once
#include <windows.h>
#include <process.h>
#include <vector>
#include "Role.h"

using namespace std;

const string FIGHT = "fight";
const string PLAYER = "player";
const string MONSTER = "monster";

class Server {
public:
    Server();
    ~Server();

    void startServer();
    void enroll(const string& strMsg);
    void setMember(const string& strMsg);
    void battleStart();
    int selectMonster();
    void showStatus();
    bool isGameOver();

public:
    Role mPlayer;
    std::vector<Role> mMonsters;

};