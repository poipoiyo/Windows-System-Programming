#pragma once
#include "Logger.h"
#include <string>

using namespace std;

class Role {
public:
    Role();
    Role(const string& name, const int hp, const int atk);
    ~Role();

    bool isAlive();
    bool attack(Role &defender);
    bool getAttack(const int iAttack);
    void showStatus();
    void newTurn();

    const string& getName();
    const int getHp();
    const int getAtk();

private:
    string m_name;
    int m_hp;
    int m_atk;
};
