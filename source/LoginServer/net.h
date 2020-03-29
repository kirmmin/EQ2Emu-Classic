/*  
	EQ2Emulator:  Everquest II Server Emulator
	Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

	This file is part of EQ2Emulator.
*/
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

//#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#include "../common/types.h"

void CatchSignal(int sig_num);
enum eServerMode { Standalone, Master, Slave, Mesh };

class NetConnection
{
public:
	NetConnection() {
		port = 5999;
		listening_socket = 0;
		memset(masteraddress, 0, sizeof(masteraddress));
		uplinkport = 0;
		memset(uplinkaccount, 0, sizeof(uplinkaccount));
		memset(uplinkpassword, 0, sizeof(uplinkpassword));
		LoginMode = Standalone;
		Uplink_WrongVersion = false;
		numclients = 0;
		numservers = 0;
		allowAccountCreation = true;
	}
	void UpdateWindowTitle(char* iNewTitle = 0);
	bool Init();
	void ListenNewClients();
	void HitKey(int keyhit);
	char address[1024];
	int32 numclients;
	int32 numservers;

	int16	GetPort()	{ return port; }
	void	SetPort(int16 in_port) { port = in_port; }
	eServerMode	GetLoginMode()	{ return LoginMode; }

	bool	ReadLoginConfig();
	char*	GetMasterAddress()	{ return masteraddress; }
	int16	GetUplinkPort()		{ if (uplinkport != 0) return uplinkport; else return port; }
	char*	GetUplinkAccount()	{ return uplinkaccount; }
	char*	GetUplinkPassword()	{ return uplinkpassword; }

	bool	IsAllowingAccountCreation() { return allowAccountCreation; }

protected:
	friend class LWorld;
	bool	Uplink_WrongVersion;
private:
	int16	port;
	int		listening_socket;
	char	masteraddress[300];
	int16	uplinkport;
	char	uplinkaccount[300];
	char	uplinkpassword[300];
	eServerMode	LoginMode;
	bool	allowAccountCreation;
};
