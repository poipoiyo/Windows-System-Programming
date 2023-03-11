#include "Define.h"
#include "Server.h"
#include <iostream>

#define PLAYER_TURN		1
#define MONSTER_TURN	2

CRITICAL_SECTION cs;
int TURN = PLAYER_TURN;

unsigned __stdcall playerFunc(void* pArguments) {
	Server* game = reinterpret_cast<Server*>(pArguments);

	while (true) {
		EnterCriticalSection(&cs);
		if (TURN == PLAYER_TURN && game->isGameOver() == false) {
			cout << "-- player's turn ->" << endl;
			game->mPlayer.newTurn();
			game->showStatus();
			//logger.writeLogToFile("--- Player's turn ---\n");
			int iInd = game->selectMonster();
			game->mPlayer.attack(game->mMonsters[iInd]);
			TURN = MONSTER_TURN;
		}
		
		LeaveCriticalSection(&cs);
		if (game->isGameOver()) {
			break;
		}
	}
	return 1;
}

unsigned __stdcall monsterFunc(void* pArguments) {
	Server* game = reinterpret_cast<Server*>(pArguments);
	size_t iMonster = game->mMonsters.size();

	while (true) {
		EnterCriticalSection(&cs);
		if (TURN == MONSTER_TURN && game->isGameOver() == false) {
			game->mMonsters[0].newTurn();
			cout << "-- monster's turn ->" << endl;
			for (size_t i = 0; i < iMonster; ++i) {
				game->mMonsters[i].attack(game->mPlayer);
			}
			TURN = PLAYER_TURN;
		}
		
		LeaveCriticalSection(&cs);
		if (game->isGameOver()) {
			break;
		}
	}
	return 1;
}

Server::Server(): mPlayer(Role()), mMonsters(vector<Role> ()) {
}

Server::~Server() {
}

void Server::startServer() {
	HANDLE hPipe;
	DWORD dwBytesRead;
	bool bExit = 0;

	BOOL ret = false;
	while (bExit == false) {
		hPipe = CreateNamedPipe(PIPENAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			cout << "CreateNamedPipe failed, error: [" << GetLastError() << "]." << endl;
			return;
		}
		cout << "Server is now running ..." << endl;
	
		ret = ConnectNamedPipe(hPipe, NULL);
		if (ret == 0) {
			cout << "ConnectNamePipe failed, error : [" << GetLastError() << "]." << endl;
			CloseHandle(hPipe);
			return;
		}
	
		CHAR buffer[1024] = { 0 };
		ret = ReadFile(hPipe, buffer, sizeof(buffer), &dwBytesRead, NULL);
		if (ret == 0) {
			cout << "ReadFile failed, error: [" << GetLastError() << "]." << endl;
			CloseHandle(hPipe);
			return;
		}
		cout << "Receive data, byteread = [" << dwBytesRead << "], buffer = [" << buffer << "]." << endl;

		if (string(buffer).find(FIGHT) != string::npos) {
			bExit = true;
		}
		else {
			setMember(buffer);
		}

		ret = DisconnectNamedPipe(hPipe);
		if (ret == 0) {
			cout << "DisconnectNamedPipe failed, error: [" << GetLastError() << "]." << endl;
			return;
		}

		CloseHandle(hPipe);
	}
	
	return battleStart();
}

void Server::enroll(const string& strMsg) {
	HANDLE hPipe;
	DWORD dwBytesWritten;

	BOOL ret = WaitNamedPipe(PIPENAME, NMPWAIT_WAIT_FOREVER);
	if (ret == 0) {
		cout << "WaitNamedPipe failed, error [" << GetLastError() << "]." << endl;
		return;
	}

	hPipe = CreateFile(PIPENAME, GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		cout << "CreateFile failed, error [" << GetLastError() << "]." << endl;
		return;
	}

	ret = WriteFile(hPipe, strMsg.c_str(), strMsg.length(), &dwBytesWritten, NULL);
	if (ret == 0) {
		cout << "WriteFile failed, error [" << GetLastError() << "]." << endl;
		CloseHandle(hPipe);
		return;
	}

	cout << "Send data, wrote [" << dwBytesWritten << "] bytes." << endl;
	CloseHandle(hPipe);
}

void Server::setMember(const string& strMsg) {
	const int iValidResultSize = 3;
	vector<string> result;
	string pattern = ",";
	string::size_type begin = 0, end = strMsg.find(pattern);

	while (end != std::string::npos) {
		if (end - begin != 0) {
			result.push_back(strMsg.substr(begin, end - begin));
		}
		begin = end + pattern.size();
		end = strMsg.find(pattern, begin);
	}

	if (begin != strMsg.length()) {
		result.push_back(strMsg.substr(begin));
	}

	if (result.empty() || 
		(strcmp(result[0].c_str(), PLAYER.c_str()) != 0 && strcmp(result[0].c_str(), MONSTER.c_str()) != 0)) {
		cout << "Error command, drop it." << endl;
		return;
	}

	if (strcmp(result[0].c_str(), PLAYER.c_str()) == 0 && result.size() == iValidResultSize) {
		const int iHp = stoi(result[1]), iAtk = stoi(result[2]);
		if (iHp <= 0 || iAtk <= 0) {
			cout << "Hp and Atk should be positive." << endl;
			return;
		}

		mPlayer = Role(PLAYER, iHp, iAtk);
		cout << "Enroll player, Hp = " << iHp << ", Atk = " << iAtk << "." << endl;
		return;
	}

	if (strcmp(result[0].c_str(), PLAYER.c_str()) == 0 && result.size() == iValidResultSize) {
		const int iHp = stoi(result[1]), iAtk = stoi(result[2]);
		if (iHp <= 0 || iAtk <= 0) {
			cout << "Hp and Atk should be positive." << endl;
			return;
		}

		mPlayer = Role(PLAYER, iHp, iAtk);
		cout << "Enroll player, Hp = " << iHp << ", Atk = " << iAtk << endl;
		return;
	}
	else if (strcmp(result[0].c_str(), MONSTER.c_str()) == 0 && result.size() == iValidResultSize) {
		const int iHp = stoi(result[1]), iAtk = stoi(result[2]);
		Role monster(MONSTER, iHp, iAtk);
		mMonsters.push_back(monster);
		cout << "Enroll Monster" << mMonsters.size() << ", Hp = " << iHp << ", Atk = " << iAtk << "." << endl;
		return;
	}
	else {
		cout << "Error command, drop it." << endl;
	}

	return;
}

void Server::battleStart() {
	cout << "[[[ GAME START ]]]" << endl;
	InitializeCriticalSection(&cs);

	if (mMonsters.empty() == true || isGameOver()) {
		cout << "Enroll chacters first." << endl;
	}

	HANDLE hPlayer = (HANDLE)_beginthreadex(NULL, 0, &playerFunc, this, 0, NULL);
	HANDLE hMonster = (HANDLE)_beginthreadex(NULL, 0, &monsterFunc, this, 0, NULL);

	WaitForSingleObject(hPlayer, INFINITE);
	WaitForSingleObject(hMonster, INFINITE);

	CloseHandle(hPlayer);
	CloseHandle(hMonster);

	return;
}

int Server::selectMonster() {
	int iInd = -1, iAtk = 0;
	for (int i = 0; i < mMonsters.size(); ++i) {
		if (mMonsters[i].isAlive() && mMonsters[i].getAtk() > iAtk) {
			iInd = i;
		}
	}
	return iInd;
}

void Server::showStatus() {
	mPlayer.showStatus();
	for (int i = 0; i < mMonsters.size(); ++i) {
		mMonsters[i].showStatus();
	}
}

bool Server::isGameOver() {
	if (mPlayer.isAlive()) {
		for (int i = 0; i < mMonsters.size(); ++i) {
			if (mMonsters[i].isAlive()) {
				return false;
			}
		}
	}
	cout << "[[[ GAME OVER ]]]" << endl;
    return true;
}
