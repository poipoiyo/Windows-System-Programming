#pragma once

#include <string>
#include "windows.h"
#include "Role.h"
class Role;
using namespace std;

class Logger {
public:
    Logger();
    ~Logger();

    void writeAttackLog(Role& attacker, Role& defender);
    void writeStatusLog(Role& role);
    void writeDeathLog(Role& role);
    void writeLogToFile(const string& strMsg);

    

private:
    void getFileHandle();
    HANDLE m_fileHandle;
}; 
