/*  
    EQ2Emulator:  Everquest II Server Emulator
    Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

    This file is part of EQ2Emulator.

    EQ2Emulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EQ2Emulator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EQ2Emulator.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LOGINSERVER_H
#define LOGINSERVER_H

#include "../common/servertalk.h"
#include "../common/linked_list.h"
#include "../common/timer.h"
#include "../common/queue.h"
#include "../common/Mutex.h"
#include "../common/TCPConnection.h"
#include <deque>
#include "MutexMap.h"

#ifdef WIN32
	void AutoInitLoginServer(void *tmp);
#else
	void *AutoInitLoginServer(void *tmp);
#endif
bool InitLoginServer();

class LoginServer{
public:
	LoginServer(const char* iAddress = 0, int16 iPort = 5999);
    ~LoginServer();

	bool Process();
	bool Connect(const char* iAddress = 0, int16 iPort = 0);

	bool ConnectToUpdateServer(const char* iAddress = 0, int16 iPort = 0);

	void SendInfo();
	void SendStatus();
	void GetLatestTables();

	void SendPacket(ServerPacket* pack) { tcpc->SendPacket(pack); }
	int8 GetState() { return tcpc->GetState(); }
	bool Connected() { return tcpc->Connected(); }
	int32 ProcessTableUpdates(uchar* data);
	int32 ProcessDataUpdates(uchar* data);
	void ProcessTableUpdate(uchar* data);
	void ProcessDataUpdate(uchar* data);
	bool CheckAndWait(Timer* timer);
	bool UpdatesAuto(){ return updates_always; }
	void UpdatesAuto(bool val){ updates_always = val; }
	void UpdatesAsk(bool val){ updates_ask = val; }
	bool UpdatesAsk(){ return updates_ask; }
	void UpdatesVerbose(bool val){ updates_verbose = val; }
	bool UpdatesVerbose(){ return updates_verbose; }
	void UpdatesAutoData(bool val){ updates_auto_data = val; }
	bool UpdatesAutoData(){ return updates_auto_data; }

	void SendFilterNameResponse ( int8 resp , int32 acct_id , int32 char_id );

	void SendDeleteCharacter ( CharacterTimeStamp_Struct* cts );

	int32 DetermineCharacterLoginRequest ( UsertoWorldRequest_Struct* utwr );
	
	void InitLoginServerVariables();

	sint16 minLockedStatus;
	sint16 maxPlayers;
	sint16 minGameFullStatus;

	void SendImmediateEquipmentUpdatesForChar(int32 char_id);

	bool CanReconnect() { return pTryReconnect; }

private:
	bool try_auto_update;
	bool pTryReconnect;
	TCPConnection* tcpc;
	int32	LoginServerIP;
	int32	UpdateServerIP;
	int16	LoginServerPort;
	int16	UpdateServerPort;
	bool updates_ask;
	bool updates_always;
	bool updates_verbose;
	bool updates_auto_data;
	bool update_server_verified;
	bool update_server_completed;
	uchar* data_waiting;
	string last_data_update_table;
	deque<uchar*> table_updates_waiting;
	deque<uchar*> data_updates_waiting;
	MutexMap<int32, LoginZoneUpdate>* zone_updates;
	MutexMap<int32, LoginEquipmentUpdate>* loginEquip_updates;
	int32 last_checked_time;

	Timer* statusupdate_timer;
};
#endif
