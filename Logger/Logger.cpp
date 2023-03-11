#include "Logger.h"
#include <iostream>
#include <tchar.h>

using namespace std;
# define LOGFILE "battlelog.txt"

BOOL isFileExist(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

Logger::Logger(): m_fileHandle(INVALID_HANDLE_VALUE) {
    if (isFileExist(_T(LOGFILE))) {
        DeleteFile(_T(LOGFILE));
    }
}

Logger::~Logger() {
    CloseHandle(m_fileHandle);
}

void Logger::writeAttackLog(Role& attacker, Role& defender) {
    string strMsg = 
        "[ATK] " + attacker.getName() + " attacked " + defender.getName() + ", left HP: [" + std::to_string(defender.getHp()) + "].\n";
    writeLogToFile(strMsg);
}

void Logger::writeStatusLog(Role& role) {
    string strMsg = "[STAT] " + role.getName() + ", HP: " + std::to_string(role.getHp()) + ", Atk: " + std::to_string(role.getAtk()) + ".\n";
    writeLogToFile(strMsg);
}

void Logger::writeDeathLog(Role& role) {
    string strMsg = "[DEAD] " + role.getName() + " is dead. \n";
    writeLogToFile(strMsg);
}

void Logger::writeLogToFile(const string& strMsg) {
    if (m_fileHandle == INVALID_HANDLE_VALUE) {
        getFileHandle();
        if (m_fileHandle == INVALID_HANDLE_VALUE)
            return;
    }

    auto dwPtr = SetFilePointer(m_fileHandle, 0, NULL, FILE_END);
    auto ret = WriteFile(m_fileHandle, strMsg.c_str(), strMsg.size(), &dwPtr, NULL);

    if (ret != TRUE) {
        cout << "[ERR] WriteFile Failed, error code: [" << GetLastError() << "]." << endl;
        return;
    }
}

void Logger::getFileHandle() {
    DWORD dwCreationDisposition = CREATE_NEW;
    if (isFileExist(_T(LOGFILE))) {
        dwCreationDisposition = OPEN_EXISTING;
    }

    m_fileHandle = CreateFile(
        _T(LOGFILE), GENERIC_ALL, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (m_fileHandle == INVALID_HANDLE_VALUE) {
        cout << "CreateFile Failed, error code: [" << GetLastError() << "]." << endl;
        return;
    }
}
