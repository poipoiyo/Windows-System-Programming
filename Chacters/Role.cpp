#include "Role.h"
#include <iostream>
#include <windows.h>

Logger logger;

Role::Role() : m_name(""), m_hp(0), m_atk(0) {
}

Role::Role(const string& name, const int hp, const int atk) : m_name(name), m_hp(hp), m_atk(atk) {
}

Role::~Role() {
}

bool Role::isAlive() {
    return this->m_hp > 0;
}

bool Role::attack(Role &defender) {
    if (this->isAlive() == false)
        return false;

    const int iAttack = this->m_atk;
    cout << this->m_name << " attack " << defender.m_name << "." << endl;

    defender.getAttack(iAttack);
    logger.writeAttackLog(*this, defender);
    return true;
}

bool Role::getAttack(const int iAttack) {
    this->m_hp -= iAttack;
    cout << this->m_name << ", Hp: " << this->m_hp << "." << endl;
    
    if(this->isAlive() == false) 
        logger.writeDeathLog(*this);

    return true;
}

void Role::showStatus() {
    cout << "[" << this->m_name << "] Hp: " << this->m_hp << ", Atk: " << this->m_atk << endl;
    logger.writeStatusLog(*this);
}

void Role::newTurn() {
    string strMsg = "\n" + this->m_name + "'s turn...\n";
    logger.writeLogToFile(strMsg);
}

const string& Role::getName() {
    return this->m_name;
}

const int Role::getHp() {
    return max(0, this->m_hp);
}

const int Role::getAtk() {
    return this->m_atk;
}
