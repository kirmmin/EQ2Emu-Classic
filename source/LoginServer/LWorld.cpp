/*  
	EQ2Emulator:  Everquest II Server Emulator
	Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

	This file is part of EQ2Emulator.
*/
#include "../common/debug.h"
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#include <process.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "../common/unix.h"

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

extern int errno;
#endif

#include "../common/servertalk.h"
#include "LWorld.h"
#include "net.h"
#include "client.h"
#include "../common/packet_dump.h"
#include "login_opcodes.h"
#include "login_structs.h"
#include "LoginDatabase.h"
#include "PacketHeaders.h"
#include "../common/ConfigReader.h"

#ifdef WIN32
#define snprintf	_snprintf
#define vsnprintf	_vsnprintf
#define strncasecmp	_strnicmp
#define strcasecmp  _stricmp
#endif

extern ClientList client_list;
extern NetConnection net;
extern LWorldList	world_list;
extern LoginDatabase database;
extern ConfigReader configReader;
extern volatile bool RunLoops;

#include "../common/Log.h"
using namespace std;
LWorld::LWorld(TCPConnection* in_con, bool in_OutgoingLoginUplink, int32 iIP, int16 iPort, bool iNeverKick) {
	Link = in_con;
	RemoteID = 0;
	LinkWorldID = 0;
	if (iIP)
		ip = iIP;
	else
		ip = in_con->GetrIP();
	if (iPort)
		port = iPort;
	else
		port = in_con->GetrPort();
	ID = 0;
	pClientPort = 0;
	memset(account, 0, sizeof(account));
	memset(address, 0, sizeof(address));
	memset(worldname, 0, sizeof(worldname));
	status = 0;
	accountid = 0;
	admin_id = 0;
	IsInit = false;
	kicked = false;
	pNeverKick = iNeverKick;
	pPlaceholder = false;
	pshowdown = false;
	pConnected = in_con->Connected();
	pReconnectTimer = 0;
	pStatsTimer = NULL;
	isAuthenticated = false;

	if (in_OutgoingLoginUplink) {
		pClientPort = port;
		ptype = Login;
		OutgoingUplink = true;
		if (net.GetLoginMode() == Mesh) {
			pReconnectTimer = new Timer(INTERSERVER_TIMER);
			pReconnectTimer->Trigger();
		}
	}
	else {
		ptype = UnknownW;
		OutgoingUplink = false;
	}

	struct in_addr  in;
	in.s_addr = GetIP();
	strcpy(address, inet_ntoa(in));
	isaddressip = true;

	num_players = 0;
	num_zones = 0;
}

LWorld::LWorld(int32 in_accountid, char* in_accountname, char* in_worldname, int32 in_admin_id) {
	pPlaceholder = true;
	Link = 0;
	ip = 0;
	port = 0;
	ID = 0;
	pClientPort = 0;
	memset(account, 0, sizeof(account));
	memset(address, 0, sizeof(address));
	memset(worldname, 0, sizeof(worldname));
	status = 0;
	accountid = in_accountid;
	admin_id = in_admin_id;
	IsInit = false;
	kicked = false;
	pNeverKick = false;
	pshowdown = true;
	RemoteID = 0;
	LinkWorldID = 0;
	OutgoingUplink = false;
	pReconnectTimer = 0;
	pConnected = false;

	pStatsTimer = NULL;

	ptype = World;
	strcpy(account, in_accountname);
	strcpy(worldname, in_worldname);

	strcpy(address, "none");

	isaddressip = true;

	num_players = 0;
	num_zones = 0;
}

LWorld::LWorld(TCPConnection* in_RemoteLink, int32 in_ip, int32 in_RemoteID, int32 in_accountid, char* in_accountname, char* in_worldname, char* in_address, sint32 in_status, int32 in_adminid, bool in_showdown, int8 in_authlevel, bool in_placeholder, int32 iLinkWorldID) {
	Link = in_RemoteLink;
	RemoteID = in_RemoteID;
	LinkWorldID = iLinkWorldID;
	ip = in_ip;
	port = 0;
	ID = 0;
	pClientPort = 0;
	memset(account, 0, sizeof(account));
	memset(address, 0, sizeof(address));
	memset(worldname, 0, sizeof(worldname));
	status = in_status;
	accountid = in_accountid;
	admin_id = in_adminid;
	IsInit = true;
	kicked = false;
	pNeverKick = false;
	pPlaceholder = in_placeholder;
	pshowdown = in_showdown;
	OutgoingUplink = false;
	pReconnectTimer = 0;
	pConnected = true;
	pStatsTimer = NULL;

	ptype = World;
	strcpy(account, in_accountname);
	strcpy(worldname, in_worldname);

	strcpy(address, in_address);

	isaddressip = false;

	num_players = 0;
	num_zones = 0;
}

LWorld::~LWorld() {

	safe_delete ( pStatsTimer );
	num_zones = 0;
	num_players = 0;
	database.UpdateWorldServerStats(this, -4);

	if (ptype == World && RemoteID == 0) {
		if (net.GetLoginMode() != Mesh || (!pPlaceholder && IsInit)) {
			ServerPacket* pack = new ServerPacket(ServerOP_WorldListRemove, sizeof(int32));
			*((int32*) pack->pBuffer) = GetID();
			world_list.SendPacketLogin(pack);
			delete pack;
		}
	}

	if (Link != 0 && RemoteID == 0) {
		world_list.RemoveByLink(Link, 0, this);
		if (OutgoingUplink)
			delete Link;
		else
			Link->Free();
	}
	Link = 0;
	safe_delete(pReconnectTimer);

	world_list.RemoveByID ( this->GetID ( ) );
}

bool LWorld::Process() {
	bool ret = true;
	if (Link == 0)
		return true;
	if (Link->Connected()) {
		if (!pConnected) {
			pConnected = true;
		}
	}
	else {
		pConnected = false;
		if (pReconnectTimer) {
			if (pReconnectTimer->Check() && Link->ConnectReady()) {
				pReconnectTimer->Start(pReconnectTimer->GetTimerTime() + (rand()%15000), false);
			}
			return true;
		}
		return false;
	}
	if (RemoteID != 0)
		return true;

	if(pStatsTimer && pStatsTimer->Check())
		database.UpdateWorldServerStats(this, GetStatus());

	ServerPacket* pack = 0;
	while (ret && (pack = Link->PopPacket())) {

		// this stops connections from sending invalid packets without first authenticating
		// with the login server to show it is a legit server
		if(!isAuthenticated && pack->opcode != ServerOP_LSInfo)
		{
			Kick("This connection has not authenticated.");
			break;
		}

		switch(pack->opcode) {
		case 0:
			break;
		case ServerOP_KeepAlive: {
			// ignore this
			break;
								 }
		case ServerOP_LSFatalError: {
			net.Uplink_WrongVersion = true;
			ret = false;
			kicked = true;
			break;
									}
		case ServerOP_CharacterCreate: {
			WorldCharNameFilterResponse_Struct* wcnfr = (WorldCharNameFilterResponse_Struct*) pack->pBuffer;

			Client* client = client_list.FindByLSID(wcnfr->account_id);
			if(!client){
				if(wcnfr->account_id == 0){
					client_list.FindByCreateRequest();
				}
				break;
			}
			if(wcnfr->response == 1)
			{
				client->CharacterApproved(GetID(),wcnfr->char_id);
			}
			else
			{
				client->CharacterRejected(wcnfr->response);
			}
			break;
									   }
		case ServerOP_UsertoWorldReq: {
			UsertoWorldRequest_Struct* ustwr = (UsertoWorldRequest_Struct*) pack->pBuffer;
			if (ustwr->ToID) {
				LWorld* world = world_list.FindByID(ustwr->ToID);
				if (!world) {
					break;
				}
				if (this->GetType() != Login) {
					break;
				}
				ustwr->FromID = this->GetID();
				world->SendPacket(pack);
			}
			break;
									  }
		case ServerOP_UsertoWorldResp: {
			if (pack->size != sizeof(UsertoWorldResponse_Struct))
				break;

			UsertoWorldResponse_Struct* seps = (UsertoWorldResponse_Struct*) pack->pBuffer;
			if (seps->ToID) {
				LWorld* world = world_list.FindByID(seps->ToID);
				if (world) {
					seps->ToID = world->GetRemoteID();
					world->SendPacket(pack);
				}
			}
			else {
				Client* client = 0;
				client = client_list.FindByLSID(seps->lsaccountid);
				if(client == 0)
					break;
				if(this->GetID() != seps->worldid && this->GetType() != Login)
					break;

				client->WorldResponse(GetID(),seps->response, seps->ip_address, seps->port, seps->access_key);
			}
			break;
									   }
		case ServerOP_CharTimeStamp: { // This is being sent to synch a new timestamp on the login server
			if(pack->size != sizeof(CharacterTimeStamp_Struct))
				break;

			CharacterTimeStamp_Struct* cts = (CharacterTimeStamp_Struct*) pack->pBuffer;

			if(!database.UpdateCharacterTimeStamp(cts->account_id,cts->char_id,cts->unix_timestamp,GetAccountID()))
				printf("TimeStamp update error with character id %i of account id %i on server %i\n",cts->char_id,cts->account_id,GetAccountID());

			//Todo: Synch with the other login servers

			break;
									 }
		case ServerOP_GetTableData:{
			Kick("This is not an update server, update your LoginServer.ini file.");
			break;
		}
		case ServerOP_GetTableQuery:{
			Kick("This is not an update server, update your LoginServer.ini file.");
			break;
		}
		case ServerOP_GetLatestTables:{
			Kick("This is not an update server, update your LoginServer.ini file.");
			break;
		}
		case ServerOP_ZoneUpdate:{
			if(pack->size > CHARZONESTRUCT_MAXSIZE)
				break;
			CharZoneUpdate_Struct* czu = (CharZoneUpdate_Struct*)pack->pBuffer;
			database.UpdateCharacterZone(czu->account_id, czu->char_id, czu->zone_id, GetAccountID());
			break;
		}
		case ServerOP_RaceUpdate: {

			if(pack->size != sizeof(RaceUpdate_Struct))
				break;

			RaceUpdate_Struct* ru = (RaceUpdate_Struct*) pack->pBuffer;
			database.UpdateCharacterRace(ru->account_id , ru->char_id , ru->model_type , ru->race , this->GetAccountID ( ));
			break;
								  }
		case ServerOP_BasicCharUpdate: {
			if(pack->size != sizeof(CharDataUpdate_Struct))
				break;

			CharDataUpdate_Struct* cdu = (CharDataUpdate_Struct*) pack->pBuffer;

			switch(cdu->update_field)
			{
			case LEVEL_UPDATE_FLAG:
				{
					database.UpdateCharacterLevel(cdu->account_id,cdu->char_id,cdu->update_data,this->GetAccountID());
					break;
				}
			case CLASS_UPDATE_FLAG:
				{
					database.UpdateCharacterClass(cdu->account_id,cdu->char_id,cdu->update_data,this->GetAccountID());
					break;
				}
			case GENDER_UPDATE_FLAG:
				{
					database.UpdateCharacterGender(cdu->account_id,cdu->char_id,cdu->update_data,this->GetAccountID());
					break;
				}
			case DELETE_UPDATE_FLAG:
				{
					if(cdu->update_field == 1)
						database.DeleteCharacter(cdu->account_id,cdu->char_id,this->GetAccountID());
					break;			
				}
			}
			break;
									   }
		case ServerOP_LSInfo: {
			if (pack->size != sizeof(ServerLSInfo_Struct)) {
				this->Kick(ERROR_BADVERSION);
				ret = false;
				struct in_addr  in;
				in.s_addr = GetIP();
			}
			else {
				ServerLSInfo_Struct* lsi = (ServerLSInfo_Struct*) pack->pBuffer;
				if (strcmp(lsi->protocolversion, EQEMU_PROTOCOL_VERSION) != 0 || !database.CheckVersion(lsi->serverversion)) {
					this->Kick(ERROR_BADVERSION);
					ret = false;
					struct in_addr  in;
					in.s_addr = GetIP();
				}
				else if (!SetupWorld(lsi->name, lsi->address, lsi->account, lsi->password, lsi->serverversion)) {
					this->Kick(ERROR_BADPASSWORD);
					ret = false;
					struct in_addr  in;
					in.s_addr = GetIP();
				}
				else{
					isAuthenticated = true;
					devel_server = (lsi->servertype == 4);
				}
			}
			break;
							  }
		case ServerOP_LSStatus: {
			ServerLSStatus_Struct* lss = (ServerLSStatus_Struct*) pack->pBuffer;

			if(lss->num_players > 5000 || lss->num_zones > 500) {
				this->Kick("Your server has exceeded a number of players and/or zone limit.");
				ret = false;
				break;
			}
			UpdateStatus(lss->status, lss->num_players, lss->num_zones);
			break;
								}
		case ServerOP_SystemwideMessage: {
			if (this->GetType() == Login) {
				// no looping plz =p
				//world_list.SendPacket(pack, this);
			}
			else if (this->GetType() == Chat) {
				world_list.SendPacket(pack);
			}
			else {
			}
			break;
										 }
		case ServerOP_ListWorlds: {
			if (pack->size <= 1 || pack->pBuffer[pack->size - 1] != 0) {
				break;
			}
			world_list.SendWorldStatus(this, (char*) pack->pBuffer);
			break;
								  }
		case ServerOP_WorldListUpdate: {
			if (this->GetType() != Login) {
				break;
			}
			if (pack->size != sizeof(ServerSyncWorldList_Struct)) {
				break;
			}
			ServerSyncWorldList_Struct* sswls = (ServerSyncWorldList_Struct*) pack->pBuffer;
			if (!CheckServerName(sswls->name)) {
				struct in_addr  in;
				in.s_addr = sswls->ip;
				break; // Someone needs to tell the other login server to update it's exe!
			}
			LWorld* world = world_list.FindByLink(this->Link, sswls->RemoteID);
			if (world) {
				world->SetRemoteInfo(sswls->ip, sswls->accountid, sswls->account, sswls->name, sswls->address, sswls->status, sswls->adminid, sswls->num_players, sswls->num_zones);
			}
			else {
				world = world_list.FindByAccount(sswls->accountid, World);
				if (world == 0 || sswls->placeholder == false) {
					if (world) {
#ifdef _DEBUG
						cout << "Kick(" << world->GetID() << ") in ServerOP_WorldListUpdate" << endl;
#endif
						world->Kick();
					}
					world = new LWorld(this->Link, sswls->ip, sswls->RemoteID, sswls->accountid, sswls->account, sswls->name, sswls->address, sswls->status, sswls->adminid, sswls->showdown, sswls->authlevel, sswls->placeholder, this->GetID());
					LWorld* world2=world_list.FindByID(sswls->RemoteID);
					if(!world2)
						world_list.Add(world);
				}
			}
			sswls->RemoteID = world->GetID();
			if (net.GetLoginMode() != Mesh)
				world_list.SendPacketLogin(pack, this);
			cout << "Got world update for '" << sswls->name << "', #" << world->GetID() << endl;
			break;
									   }
		case ServerOP_WorldListRemove: {
			if (this->GetType() != Login) {
				//				cout << "Error: ServerOP_WorldListRemove from a non-login connection? WTF!" << endl;
				break;
			}
			if (pack->size != sizeof(int32)) {
				//				cout << "Wrong size on ServerOP_WorldListRemove. Got: " << pack->size << ", Expected: " << sizeof(int32) << endl;
				break;
			}
			cout << "Got world remove for remote #" << *((int32*) pack->pBuffer) << endl;
			if ((*((int32*) pack->pBuffer)) > 0) {
				LWorld* world = world_list.FindByLink(this->GetLink(), *((int32*) pack->pBuffer));
				if (world && world->GetRemoteID() != 0) {
					*((int32*) pack->pBuffer) = world->GetID();
					if (net.GetLoginMode() != Mesh)
						world_list.SendPacketLogin(pack, this);
					world_list.RemoveByID(*((int32*) pack->pBuffer));
				}
			}
			else {
				//				cout << "Error: ServerOP_WorldListRemove: ID = 0? ops!" << endl;
			}
			break;
									   }
		case ServerOP_TriggerWorldListRefresh: {
			world_list.UpdateWorldList();
			if (net.GetLoginMode() != Mesh)
				world_list.SendPacketLogin(pack, this);
			break;
		}
		case ServerOP_ZoneUpdates:{
			pack->Inflate();
			ZoneUpdateList_Struct* updates = 0;
			if(pack->size >= sizeof(ZoneUpdateList_Struct) && ((ZoneUpdateList_Struct*)pack->pBuffer)->total_updates <= MAX_UPDATE_COUNT){
				updates = (ZoneUpdateList_Struct*)pack->pBuffer;
				ZoneUpdate_Struct* zone = 0;
				int32 pos = sizeof(ZoneUpdateList_Struct);
				sint16 num_updates = 0;
				map<int32, LoginZoneUpdate> zone_updates;
				LoginZoneUpdate update;
				while(pos < pack->size && num_updates < updates->total_updates){
					zone = (ZoneUpdate_Struct*)(pack->pBuffer+pos);
					if((pos + zone->zone_name_length + zone->zone_desc_length + sizeof(ZoneUpdate_Struct)) <= pack->size){
						update.name = string(zone->data, zone->zone_name_length);
						update.description = string(zone->data + zone->zone_name_length, zone->zone_desc_length);
						pos += sizeof(ZoneUpdate_Struct) + zone->zone_name_length + zone->zone_desc_length;
						num_updates++;
						zone_updates[zone->zone_id] = update;
					}
					else
						break;
				}
				if(zone_updates.size() == updates->total_updates)
					world_list.AddServerZoneUpdates(this, zone_updates);
				else
					cout << "Error processing zone updates for server: " << GetAccount() << endl;
			}
			else
				Kick("Possible Hacking Attempt");
			break;
		}
		case ServerOP_BugReport:{
			if(pack->size == sizeof(BugReport)){
				BugReport* report = (BugReport*)pack->pBuffer;
				database.SaveBugReport(GetAccountID(), report->category, report->subcategory, report->causes_crash, report->reproducible, report->summary, report->description, report->version, report->player, report->account_id, report->spawn_name, report->spawn_id, report->zone_id);
			}
			break;
		}
		case ServerOP_EncapPacket: {
			if (this->GetType() != Login) {
				//				cout << "Error: ServerOP_EncapPacket from a non-login connection? WTF!" << endl;
				break;
			}
			ServerEncapPacket_Struct* seps = (ServerEncapPacket_Struct*) pack->pBuffer;
			if (seps->ToID == 0xFFFFFFFF) { // Broadcast
				ServerPacket* inpack = new ServerPacket(seps->opcode);
				inpack->size = seps->size;
				// Little trick here to save a memcpy, be careful if you change this any
				inpack->pBuffer = seps->data;
				world_list.SendPacketLocal(inpack, this);
				inpack->pBuffer = 0;
				delete inpack;
			}
			else {
				LWorld* world = world_list.FindByID(seps->ToID);
				if (world) {
					ServerPacket* inpack = new ServerPacket(seps->opcode);
					inpack->size = seps->size;
					// Little trick here to save a memcpy, be careful if you change this any
					inpack->pBuffer = seps->data;
					world->SendPacket(inpack);
					inpack->pBuffer = 0;
					delete inpack;
				}
			}
			if (net.GetLoginMode() != Mesh)
				world_list.SendPacketLogin(pack, this);
			break;
								   }
		default:
			{
				cout << "Unknown LoginSOPcode: 0x" << hex << (int)pack->opcode << dec;
				cout << " size:" << pack->size << " from " << GetAccount() << endl;
				DumpPacket(pack->pBuffer, pack->size);
				Kick("Possible Hacking Attempt");
				break;
			}
		}
		delete pack;
	}
	return ret;
}

void LWorld::SendPacket(ServerPacket* pack) {
	if (Link == 0)
		return;
	if (RemoteID) {
		ServerPacket* outpack = new ServerPacket(ServerOP_EncapPacket, sizeof(ServerEncapPacket_Struct) + pack->size);
		ServerEncapPacket_Struct* seps = (ServerEncapPacket_Struct*) outpack->pBuffer;
		seps->ToID = RemoteID;
		seps->opcode = pack->opcode;
		seps->size = pack->size;
		memcpy(seps->data, pack->pBuffer, pack->size);
		Link->SendPacket(outpack);
		delete outpack;
	}
	else {
		Link->SendPacket(pack);
	}
}

void LWorld::Message(char* to, char* message, ...) {
	va_list argptr;
	char buffer[256];

	va_start(argptr, message);
	vsnprintf(buffer, 256, message, argptr);
	va_end(argptr);

	ServerPacket* pack = new ServerPacket(ServerOP_EmoteMessage, sizeof(ServerEmoteMessage_Struct) + strlen(buffer) + 1);
	ServerEmoteMessage_Struct* sem = (ServerEmoteMessage_Struct*) pack->pBuffer;
	strcpy(sem->to, to);
	strcpy(sem->message, buffer);
	SendPacket(pack);
	delete pack;
}

void LWorld::Kick(char* message, bool iSetKickedFlag) {
	if (iSetKickedFlag)
		kicked = true;
	if (message) {
		ServerPacket* pack = new ServerPacket(ServerOP_LSFatalError, strlen(message) + 1);
		strcpy((char*) pack->pBuffer, message);
		SendPacket(pack);
		delete pack;
	}
	if (Link && GetRemoteID() == 0)
		Link->Disconnect();
}
bool LWorld::CheckServerName(char* name) {
	if (strlen(name) < 10)
		return false;
	for (int i=0; i<strlen(name); i++) {
		if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= '0' && name[i] <= '9') || name[i] == ' ' || name[i] == '\'' || name[i] == '-' || name[i] == '(' || name[i] == ')' || name[i] == '[' || name[i] == ']' || name[i] == '/' || name[i] == '.' || name[i] == ',' || name[i] == '_' || name[i] == '+' || name[i] == '=' || name[i] == ':' || name[i] == '~'))
			return false;
	}
	return true;
}
bool LWorld::SetupWorld(char* in_worldname, char* in_worldaddress, char* in_account, char* in_password, char* in_version) {
	if (strlen(in_worldaddress) > 3) {
		isaddressip = false;
		strcpy(address, in_worldaddress);
	}
	if (strlen(in_worldname) > 3) {
		int32 id = database.CheckServerAccount(in_account, in_password);

		if(id == 0)
			return false;
		LWorld* world = world_list.FindByID(id);
		if(world)
			world->Kick("Ghost Kick!");

		ID = id;
		accountid = id;
		char* name = database.GetServerAccountName(id);
		if(name)
			snprintf(worldname, (sizeof(worldname)) - 1, "%s", name);
		else{ //failed to get account
			account[0] = 0;
			IsInit = false;
			this->Kick ( "Could not load server information." );
			return false;
		}
		//world_list.KickGhostIP(GetIP(), this);
		account[0] = 0;
		IsInit = true;
		ptype = World;
		world_list.SendWorldChanged(id, true);
	}
	else {
		// name too short
		account[0] = 0;
		IsInit = false;
		return false;
	}

	pStatsTimer = new Timer ( 60000 );
	pStatsTimer->Start ( 60000 );

	return true;
}
void LWorldList::SendWorldChanged(int32 server_id, bool sendtoallclients, Client* sendto){
	EQ2Packet* outapp = new EQ2Packet(OP_WorldStatusChangeMsg, 0, sizeof(LS_WorldStatusChanged));
	LS_WorldStatusChanged* world_changed = (LS_WorldStatusChanged*)outapp->pBuffer;

	world_changed->server_id = server_id;
	LWorld* world = world_list.FindByID(server_id);
	if(!world || world->ShowDown())
		world_changed->up = 0;
	else
		world_changed->up = 1;
	if(sendtoallclients || sendto == 0)
		client_list.SendPacketToAllClients(outapp);
	else
		sendto->QueuePacket(outapp);
	world_list.SetUpdateServerList(true);
}
void LWorld::UpdateWorldList(LWorld* to) {
	world_list.SetUpdateServerList( true );
}

void LWorld::ChangeToPlaceholder() {
	ip = 0;
	status = -1;
	pPlaceholder = true;
	if (Link != 0 && RemoteID == 0) {
		Link->Disconnect();
	}
	UpdateWorldList();
}

void LWorld::SetRemoteInfo(int32 in_ip, int32 in_accountid, char* in_account, char* in_name, char* in_address, int32 in_status, int32 in_adminid, sint32 in_players, sint32 in_zones) {
	ip = in_ip;
	accountid = in_accountid;
	strcpy(account, in_account);
	strcpy(worldname, in_name);
	strcpy(address, in_address);
	status = in_status;
	admin_id = in_adminid;
	num_players = in_players;
	num_zones = in_zones;
}


LWorldList::LWorldList() {
	server_update_thread = true;
	NextID = 1;
	tcplistener = new TCPServer(net.GetPort(), true);
	if (net.GetLoginMode() == Slave)
		OutLink = new TCPConnection(true);
	else
		OutLink = 0;

	ServerListData = NULL;
	UpdateServerList = true;
	#ifdef WIN32
		_beginthread(ServerUpdateLoop, 0, this);
	#else
		pthread_t thread;
		pthread_create(&thread, NULL, &ServerUpdateLoop, this);
	#endif
}

LWorldList::~LWorldList() {
	server_update_thread = false;
	while(!server_update_thread){
		Sleep(100);
	}
	safe_delete(tcplistener);
	safe_delete(OutLink);
}

void LWorldList::Shutdown() {
	LinkedListIterator<LWorld*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		iterator.RemoveCurrent ( );
	}

	safe_delete(tcplistener);
}

void LWorldList::Add(LWorld* worldserver) {
	LWorld* worldExist = FindByID(worldserver->GetID ( ) );
	if( worldExist )
	{
		worldExist->Kick();
		MWorldMap.writelock();
		worldmap.erase(worldExist->GetID());
		MWorldMap.releasewritelock();
		safe_delete(worldExist);
	}
	MWorldMap.writelock();
	worldmap[worldserver->GetID()] = worldserver;
	MWorldMap.releasewritelock();
	database.ResetWorldServerStatsConnectedTime(worldserver);
	database.UpdateWorldIPAddress(worldserver->GetID(), worldserver->GetIP());
}

void LWorldList::AddInitiateWorld ( LWorld* world )
{
	list.Insert ( world );
}

void LWorldList::KickGhostIP(int32 ip, LWorld* NotMe, int16 iClientPort) {
	if (ip == 0)
		return;

	map<int32,LWorld*>::iterator map_list;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++ ) {
		LWorld* world = map_list->second;
		if (!world->IsKicked() && world->GetIP() == ip && world != NotMe) {
			if ((iClientPort == 0 && world->GetType() == World) || (iClientPort != 0 && world->GetClientPort() == iClientPort)) {
				struct in_addr  in;
				in.s_addr = world->GetIP();
				//				cout << "Removing GhostIP LWorld(" << world->GetID() << ") from ip: " << inet_ntoa(in) << " port: " << (int16)(world->GetPort());
				if (!world->Connected())
					//					cout << " (it wasnt connected)";
					//				cout << endl;
					if (NotMe) {
						in.s_addr = NotMe->GetIP();
						cout << "NotMe(" << NotMe->GetID() << ") = " << inet_ntoa(in) << ":" << NotMe->GetPort() << " (" << NotMe->GetClientPort() << ")" << endl;
					}
					world->Kick("Ghost IP kick");
			}
		}
	}
	MWorldMap.releasereadlock();
}

void LWorldList::KickGhost(ConType in_type, int32 in_accountid, LWorld* ButNotMe) {
	map<int32,LWorld*>::iterator map_list;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++ ) {
		LWorld* world = map_list->second;
		if (!world->IsKicked() && world->GetType() == in_type && world != ButNotMe && (in_accountid == 0 || world->GetAccountID() == in_accountid)) {
			if (world->GetIP() != 0) {
				struct in_addr  in;
				in.s_addr = world->GetIP();
				//				cout << "Removing GhostAcc LWorld(" << world->GetID() << ") from ip: " << inet_ntoa(in) << " port: " << (int16)(world->GetPort()) << endl;
			}
			if (world->GetType() == Login && world->IsOutgoingUplink()) {
				world->Kick("Ghost Acc Kick", false);
				//				cout << "softkick" << endl;
			}
			else
				world->Kick("Ghost Acc Kick");
		}
	}
	MWorldMap.releasereadlock();
}

void LWorldList::UpdateWorldStats(){
	map<int32,LWorld*>::iterator map_list;
	MWorldMap.readlock();
	for(map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if(world && world->GetAccountID() > 0)
			database.UpdateWorldServerStats(world, world->GetStatus());
	}
	MWorldMap.releasereadlock();
}

void LWorldList::Process() {
	TCPConnection* newtcp = 0;
	LWorld* newworld = 0;

	LinkedListIterator<LWorld*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if(iterator.GetData( )->GetID ( ) > 0 )
		{
			LWorld* world = iterator.GetData ( );
			iterator.RemoveCurrent ( false );
			Add( world );
		}
		else
		{
			if(! iterator.GetData ( )->Process ( ) )
				iterator.RemoveCurrent ( );
			else
				iterator.Advance();
		}
	}

	while ((newtcp = tcplistener->NewQueuePop())) {
		newworld = new LWorld(newtcp);
		newworld->SetID(0);
		AddInitiateWorld(newworld);
		struct in_addr	in;
		in.s_addr = newtcp->GetrIP();
		LogWrite(LOGIN__INFO, 0, "Login", "New Server connection: %s port %i", inet_ntoa(in), ntohs(newtcp->GetrPort()));
		net.numservers++;
		net.UpdateWindowTitle();
	}
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); ) {
		LWorld* world = map_list->second;

		int32 account_id = world->GetAccountID();

		if (world->IsKicked() && !world->IsNeverKick()) {
			map_list++;
			worldmap.erase ( account_id );
			net.numservers--;
			net.UpdateWindowTitle();
			safe_delete ( world );
			continue;
		}
		else if (!world->Process()) {
			struct in_addr  in;
			in.s_addr = world->GetIP();
			if (world->GetAccountID() == 0 || !(world->ShowDown()) || world->GetType() == Chat) {
				map_list++;
				worldmap.erase ( account_id );

				net.numservers--;
				net.UpdateWindowTitle();
				if(account_id > 0){
					LWorld* world2 = FindByID(account_id);
					if(world2)
						world2->ShowDownActive(true);
				}
				SendWorldChanged(account_id, true);
				safe_delete ( world );
				continue;
			}
			else {
				world->ChangeToPlaceholder();
			}
		}
		map_list++;
	}
}

// Sends packet to all World and Chat servers, local and remote (but not to remote login server's ::Process())
void LWorldList::SendPacket(ServerPacket* pack, LWorld* butnotme) {
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if (world != butnotme) {
			if (world->GetType() == Login) {
				ServerPacket* outpack = new ServerPacket(ServerOP_EncapPacket, sizeof(ServerEncapPacket_Struct) + pack->size);
				ServerEncapPacket_Struct* seps = (ServerEncapPacket_Struct*) outpack->pBuffer;
				seps->ToID = 0xFFFFFFFF;
				seps->opcode = pack->opcode;
				seps->size = pack->size;
				memcpy(seps->data, pack->pBuffer, pack->size);
				world->SendPacket(outpack);
				delete outpack;
			}
			else if (world->GetRemoteID() == 0) {
				world->SendPacket(pack);
			}
		}
	}
}

// Sends a packet to every local TCP Connection, all types
void LWorldList::SendPacketLocal(ServerPacket* pack, LWorld* butnotme) {
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if (world != butnotme && world->GetRemoteID() == 0) {
			world->SendPacket(pack);
		}
	}
}

// Sends the packet to all login servers
void LWorldList::SendPacketLogin(ServerPacket* pack, LWorld* butnotme) {
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++ ) {
		LWorld* world = map_list->second;
		if (world != butnotme && world->GetType() == Login) {
			world->SendPacket(pack);
		}
	}
}

void LWorldList::UpdateWorldList(LWorld* to) {
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if (net.GetLoginMode() != Mesh || world->GetRemoteID() == 0)
			world->UpdateWorldList(to);
	}
}

LWorld* LWorldList::FindByID(int32 LWorldID) {
	if(worldmap.count(LWorldID) > 0)
		return worldmap[LWorldID];
	return 0;
}

LWorld* LWorldList::FindByIP(int32 ip) {
	map<int32,LWorld*>::iterator map_list;
	LWorld* world = 0;
	LWorld* ret = 0;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		world = map_list->second;
		if (world->GetIP() == ip){
			ret = world;
			break;
		}
	}
	MWorldMap.releasereadlock();
	return ret;
}

LWorld* LWorldList::FindByAddress(char* address) {
	map<int32,LWorld*>::iterator map_list;
	LWorld* world = 0;
	LWorld* ret = 0;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		world = map_list->second;
		if (strcasecmp(world->GetAddress(), address) == 0){
			ret = world;
			break;
		}
	}
	MWorldMap.releasereadlock();
	return ret;
}

LWorld* LWorldList::FindByLink(TCPConnection* in_link, int32 in_id) {
	if (in_link == 0)
		return 0;
	LWorld* world = 0;
	LWorld* ret = 0;
	map<int32,LWorld*>::iterator map_list;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		world = map_list->second;
		if (world->GetLink() == in_link && world->GetRemoteID() == in_id){
			ret = world;
			break;
		}
	}
	MWorldMap.releasereadlock();
	return ret;
}

LWorld*	LWorldList::FindByAccount(int32 in_accountid, ConType in_type) {
	if (in_accountid == 0)
		return 0;
	LWorld* world = 0;
	LWorld* ret = 0;
	map<int32,LWorld*>::iterator map_list;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		world = map_list->second;
		if (world->GetAccountID() == in_accountid && world->GetType() == in_type){
			ret = world;
			break;
		}
	}
	MWorldMap.releasereadlock();
	return ret;
}

int8 LWorld::GetWorldStatus(){
	if(IsDevelServer() && IsLocked() == false)
		return 1;
	else if(IsInit && IsLocked() == false)
		return 0;
	else
		return 2;
}

void LWorld::SendDeleteCharacter ( int32 char_id , int32 account_id )
{
	ServerPacket* outpack = new ServerPacket(ServerOP_BasicCharUpdate, sizeof(CharDataUpdate_Struct));
	CharDataUpdate_Struct* cdu = (CharDataUpdate_Struct*)outpack->pBuffer;
	cdu->account_id = account_id;
	cdu->char_id = char_id;
	cdu->update_field = DELETE_UPDATE_FLAG;
	cdu->update_data = 1;
	SendPacket(outpack);
}

vector<PacketStruct*>* LWorldList::GetServerListUpdate(int16 version){
	vector<PacketStruct*>* ret = new vector<PacketStruct*>;
	map<int32,LWorld*>::iterator map_list;
	PacketStruct* packet = 0;
	MWorldMap.readlock();
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if ((world->IsInit || (world->ShowDown() && world->ShowDownActive())) && world->GetType() == World) {
			packet = configReader.getStruct("LS_WorldUpdate", version);
			if(packet){
				packet->setDataByName("server_id", world->GetID());
				packet->setDataByName("up", 1);
				if(world->IsLocked())
					packet->setDataByName("locked", 1);
				ret->push_back(packet);
			}
		}
	}
	MWorldMap.releasereadlock();
	return ret;
}

EQ2Packet* LWorldList::MakeServerListPacket(int8 lsadmin, int16 version) {

	// if the latest world list has already been loaded, just return the string
	if(!UpdateServerList && ServerListData)
		return ServerListData;
	
	MWorldMap.readlock();

	LWorld* world = 0;
	uchar num_servers[2];
	int32 ServerNum = 0;
	/*	while(iterator.MoreElements()){
	world = iterator.GetData();
	if ((world->IsInit || (world->ShowDown() && world->ShowDownActive())) && world->GetType() == World)
	ServerNum++;
	iterator.Advance();
	}
	ServerNum+=3;
	*/
	uint32 tmpCount = 0;
	map<int32, LWorld*>::iterator map_list;
	for (map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if ((world->IsInit || (world->ShowDown() && world->ShowDownActive())) && world->GetType() == World) {
			tmpCount++;
		}
	}

	PacketStruct* packet = configReader.getStruct("LS_WorldList", version);
	packet->setArrayLengthByName("num_worlds", tmpCount);

	string world_data;	
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if ((world->IsInit || (world->ShowDown() && world->ShowDownActive())) && world->GetType() == World) {
			ServerNum++;
			packet->setArrayDataByName("id", world->GetID(), ServerNum-1);

			if (version < 1096)
				packet->setDataByName("allowed_races", 0x0007FFFF, ServerNum - 1); // no Freeblood and Aerakyn race
			else if (version < 60006)
				packet->setArrayDataByName("allowed_races", 0x000FFFFF, ServerNum - 1); // + Freeblood
			else
				packet->setArrayDataByName("allowed_races", 0x001FFFFF, ServerNum - 1);	// + Aerakyn

			packet->setArrayDataByName("number_online_flag", 1, ServerNum-1);
			packet->setArrayDataByName("num_players", world->GetPlayerNum(), ServerNum-1);
			packet->setArrayDataByName("name", world->GetName(), ServerNum-1);
			packet->setArrayDataByName("name2", world->GetName(), ServerNum-1);
			packet->setArrayDataByName("feature_set",0, ServerNum-1);
			packet->setArrayDataByName("load", world->GetWorldStatus(), ServerNum-1);
			if(world->IsLocked())
				packet->setArrayDataByName("locked", 1, ServerNum - 1);

			if(world->ShowDown())
				packet->setArrayDataByName("tag", 0, ServerNum - 1);
			else
				packet->setArrayDataByName("tag", 1, ServerNum - 1);
		}
	}

	EQ2Packet* pack = packet->serialize();
	safe_delete(ServerListData);
	ServerListData = pack;
	MWorldMap.releasereadlock();

	SetUpdateServerList( false );

	return ServerListData;
}

void LWorldList::SendWorldStatus(LWorld* chat, char* adminname) {
	struct in_addr  in;

	int32 count = 0;
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if (world->GetIP() != 0 && world->GetType() == World) {
			chat->Message(adminname, "Name: %s", world->GetName());
			in.s_addr = world->GetIP();
			if (world->GetAccountID() != 0) {
				chat->Message(adminname, "   Account:           %s", world->GetAccount());
			}
			chat->Message(adminname, "   Number of Zones:   %i", world->GetZoneNum());
			chat->Message(adminname, "   Number of Players: %i", world->GetPlayerNum());
			chat->Message(adminname, "   IP:                %s", inet_ntoa(in));
			if (!world->IsAddressIP()) {
				chat->Message(adminname, "   Address:			%s", world->GetAddress());
			}
			count++;
		}
	}
	chat->Message(adminname, "%i worlds listed.", count);
}

void LWorldList::RemoveByLink(TCPConnection* in_link, int32 in_id, LWorld* ButNotMe) {
	if (in_link == 0)
		return;
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		if (world != ButNotMe && world->GetLink() == in_link && (in_id == 0 || world->GetRemoteID() == in_id)) {
			//			world->Link = 0;
			map_list++;
			worldmap.erase ( world->GetID ( ) );
			safe_delete ( world );
			continue;
		}
	}
}

void LWorldList::RemoveByID(int32 in_id) {
	if (in_id == 0)
		return;

	LWorld* existWorld = FindByID(in_id);
	if ( existWorld != NULL )
	{
		MWorldMap.writelock();
		worldmap.erase ( in_id );
		MWorldMap.releasewritelock();
		safe_delete ( existWorld );
	}
}

bool LWorldList::Init() {
	
	database.ResetWorldStats ( );

	if (!tcplistener->IsOpen()) {
		return tcplistener->Open(net.GetPort());
	}

	return false;
}

void LWorldList::InitWorlds(){
	vector<LWorld*> server_list;
	database.GetServerAccounts(&server_list);
	vector<LWorld*>::iterator iter;
	int i = 0;
	for(iter = server_list.begin(); iter != server_list.end(); iter++, i++){
		LWorld* world = FindByID(server_list[i]->GetAccountID());
		if(!world){
			server_list[i]->ShowDown(true);
			server_list[i]->ShowDownActive(true);
			server_list[i]->SetID ( server_list[i]->GetAccountID ( ) );
			Add ( server_list[i] );
		}
	}
}

void LWorldList::ListWorldsToConsole() {
	struct in_addr in;

	cout << "World List:" << endl;
	cout << "============================" << endl;	
	map<int32,LWorld*>::iterator map_list;
	for( map_list = worldmap.begin(); map_list != worldmap.end(); map_list++) {
		LWorld* world = map_list->second;
		in.s_addr = world->GetIP();
		if (world->GetType() == World) {
			if (world->GetRemoteID() == 0)
				cout << "ID: " << world->GetID() << ", Name: " << world->GetName() << ", Local, IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", Status: " << world->GetStatus() << ", AccID: " << world->GetAccountID() << endl;
			else
				cout << "ID: " << world->GetID() << ", Name: " << world->GetName() << ", RemoteID: " << world->GetRemoteID() << ", LinkWorldID: " << world->GetLinkWorldID() << ", IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", Status: " << world->GetStatus() << ", AccID: " << world->GetAccountID() << endl;
		}
		else if (world->GetType() == Chat) {
			cout << "ID: " << world->GetID() << ", Chat Server, IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", AccID: " << world->GetAccountID() << endl;
		}
		else if (world->GetType() == Login) {
			if (world->IsOutgoingUplink()) {
				if (world->Connected())
					cout << "ID: " << world->GetID() << ", Login Server (out), IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", AccID: " << world->GetAccountID() << endl;
				else
					cout << "ID: " << world->GetID() << ", Login Server (nc), IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", AccID: " << world->GetAccountID() << endl;
			}
			else
				cout << "ID: " << world->GetID() << ", Login Server (in), IP: " << inet_ntoa(in) << ":" << world->GetPort() << " (" << world->GetClientPort() << "), AccID: " << world->GetAccountID() << endl;
		}
		else {
			cout << "ID: " << world->GetID() << ", Unknown Type, Name: " << world->GetName() << ", IP: " << inet_ntoa(in) << ":" << world->GetPort() << ", AccID: " << world->GetAccountID() << endl;
		}
	}
	cout << "============================" << endl;
}

void LWorldList::AddServerZoneUpdates(LWorld* world, map<int32, LoginZoneUpdate> updates){
	int32 server_id = world->GetID();
	map<int32, LoginZoneUpdate>::iterator itr;
	for(itr = updates.begin(); itr != updates.end(); itr++){
		if(zone_updates_already_used.size() >= 1500 || zone_updates_already_used[server_id].count(itr->first) > 0){
			world->Kick("Hacking attempt.");
			return;
		}
		zone_updates_already_used[server_id][itr->first] = true;
	}
	server_zone_updates.Put(server_id, updates);
}

void LWorldList::RequestServerUpdates(LWorld* world){
	if(world){
		ServerPacket* pack = new ServerPacket(ServerOP_ZoneUpdates, sizeof(ZoneUpdateRequest_Struct));			
		ZoneUpdateRequest_Struct* request = (ZoneUpdateRequest_Struct*)pack->pBuffer;
		request->max_per_batch = MAX_UPDATE_COUNT;
		world->SendPacket(pack);
		delete pack;
		zone_update_timeouts.Put(world->GetID(), Timer::GetCurrentTime2() + 30000);
	}
}

void LWorldList::ProcessServerUpdates(){	
	MutexMap<int32, map<int32, LoginZoneUpdate> >::iterator itr = server_zone_updates.begin();
	while(itr.Next()){
		if(itr->second.size() > 0){
			database.SetServerZoneDescriptions(itr->first, itr->second);			
			if(itr->second.size() == MAX_UPDATE_COUNT)			
				awaiting_zone_update.Put(itr->first, Timer::GetCurrentTime2() + 10000); //only process 20 updates in a 10 second period to avoid network problems
			server_zone_updates.erase(itr->first);
		}
		if(zone_update_timeouts.count(itr->first) == 0 || zone_update_timeouts.Get(itr->first) <= Timer::GetCurrentTime2()){
			zone_update_timeouts.erase(itr->first);
			server_zone_updates.erase(itr->first);
		}
	}
	LWorld* world = 0;
	MWorldMap.readlock();
	map<int32, LWorld*>::iterator map_itr;
	for(map_itr = worldmap.begin(); map_itr != worldmap.end(); map_itr++){
		world = map_itr->second;
		if(world && world->GetID()){
			if(last_updated.count(world) == 0 || last_updated.Get(world) <= Timer::GetCurrentTime2()){
				zone_updates_already_used[world->GetID()].clear();
				RequestServerUpdates(world);
				last_updated.Put(world, Timer::GetCurrentTime2() + 21600000);
			}
			if(awaiting_zone_update.count(world->GetID()) > 0 && awaiting_zone_update.Get(world->GetID()) <= Timer::GetCurrentTime2()){
				awaiting_zone_update.erase(world->GetID());
				RequestServerUpdates(world);				
			}
		}
	}
	MWorldMap.releasereadlock();
}

ThreadReturnType ServerUpdateLoop(void* tmp) {
#ifdef WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
#endif
	if (tmp == 0) {
		ThrowError("ServerUpdateLoop(): tmp = 0!");
		THREAD_RETURN(NULL);
	}
	LWorldList* worldList = (LWorldList*) tmp;
	while (worldList->ContinueServerUpdates()) {
		Sleep(1000);
		worldList->ProcessServerUpdates();
	}
	worldList->ResetServerUpdates();
	THREAD_RETURN(NULL);
}
