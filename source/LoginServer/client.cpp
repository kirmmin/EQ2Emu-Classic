/*  
	EQ2Emulator:  Everquest II Server Emulator
	Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

	This file is part of EQ2Emulator.
*/
#include "../common/debug.h"
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
#endif
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

#include "net.h"
#include "client.h"
#include "../common/EQStream.h"
#include "../common/packet_dump.h"
#include "../common/packet_functions.h"
#include "../common/emu_opcodes.h"
#include "../common/MiscFunctions.h"
#include "LWorld.h"
#include "LoginDatabase.h"
#include "../common/ConfigReader.h"
#include "../common/Log.h"

extern NetConnection	net;
extern LWorldList		world_list;
extern ClientList		client_list;
extern LoginDatabase	database;
extern map<int16,OpcodeManager*>EQOpcodeManager;
extern ConfigReader configReader;
using namespace std;
Client::Client(EQStream* ieqnc) {
	eqnc = ieqnc;
	ip = eqnc->GetrIP();
	port = ntohs(eqnc->GetrPort());
	account_id = 0;
	lsadmin = 0;
	worldadmin = 0;
	lsstatus = 0;
	origversion = version = 0;
	kicked = false;
	verified = false;
	memset(bannedreason, 0, sizeof(bannedreason));
	//worldresponse_timer = new Timer(10000);
	//worldresponse_timer->Disable();
	memset(key,0,10);
	LoginMode = None;
	num_updates = 0;
	updatetimer = new Timer(500);
	updatelisttimer = new Timer(10000);
	//keepalive = new Timer(5000);
	//logintimer = new Timer(500); // Give time for the servers to send updates
	//keepalive->Start();
	//updatetimer->Start();
	//logintimer->Disable();
	disconnectTimer = 0;
	memset(ClientSession,0,25);
	request_num = 0;
	login_account = 0;
	createRequest = 0;
	playWaitTimer = NULL;
	start = false;	
	update_position = 0;
	update_packets = 0;
	needs_world_list = true;
}

Client::~Client() {
	//safe_delete(worldresponse_timer);
	//safe_delete(logintimer);
	safe_delete(login_account);
	eqnc->Close();
	safe_delete(playWaitTimer);
	safe_delete(createRequest);
	safe_delete(disconnectTimer);
	safe_delete(updatetimer);
}

bool Client::Process() {
	if(!start && !eqnc->CheckActive()){
		if(!playWaitTimer)
			playWaitTimer = new Timer(5000);
		else if(playWaitTimer->Check()){
			safe_delete(playWaitTimer);
			return false;
		}
		return true;
	}
	else if(!start){
		safe_delete(playWaitTimer);
		start = true;
	}
	//if(disconnectTimer && disconnectTimer->Check())
	//	getConnection()->SendDisconnect();
	if (!kicked) {
		/************ Get all packets from packet manager out queue and process them ************/
		EQApplicationPacket *app = 0;
		/*if(logintimer && logintimer->Check())
		{
		database.LoadCharacters(GetLoginAccount());
		SendLoginAccepted();
		logintimer->Disable();
		}*/
		/*if(worldresponse_timer && worldresponse_timer->Check())
		{
		FatalError(WorldDownErrorMessage);
		worldresponse_timer->Disable();
		}*/
		
		if(playWaitTimer != NULL && playWaitTimer->Check ( ) )
		{
			SendPlayFailed(PLAY_ERROR_SERVER_TIMEOUT);
			safe_delete(playWaitTimer);
		}
		if(!needs_world_list && updatetimer && updatetimer->Check()){
			if(updatelisttimer && updatelisttimer->Check()){
				if(num_updates >= 180){ //30 minutes
					getConnection()->SendDisconnect();
				}
				else{					
					vector<PacketStruct*>::iterator itr;
					if(update_packets){
						for(itr = update_packets->begin(); itr != update_packets->end(); itr++){
							safe_delete(*itr);
						}
					}
					safe_delete(update_packets);
					update_packets = world_list.GetServerListUpdate(version);
				}
				num_updates++;
			}
			else{
				if(!update_packets){
					update_packets = world_list.GetServerListUpdate(version);
				}
				else{
					if(update_position < update_packets->size()){
						QueuePacket(update_packets->at(update_position)->serialize());
						update_position++;
					}
					else
						update_position = 0;
				}
			}
		}

		while(app = eqnc->PopPacket())
		{
			cout << "Received " << app->GetOpcodeName() << " Opcode" << endl;
			switch(app->GetOpcode())
			{
			case OP_LoginRequestMsg:{
//				DumpPacket(app);
				PacketStruct* packet = configReader.getStruct("LS_LoginRequest", 1);
				if(packet->LoadPacketData(app->pBuffer,app->size)){
					version = packet->getType_int16_ByName("version");

					LogWrite(LOGIN__INFO, 0, "Login", "Client Version Provided: %i", version);

					packet->PrintPacket();
					DumpPacket(app->pBuffer, app->size);

					// Image 2020 Notes
					// we keep the original version for special packets like char creation to pass to world server
					origversion = version;
					// forced version to 60085 for now since those structures seem to work best for steam AoM 12133L Aug 17 2015 11:19:13 build
					// version = 60085;

					if (EQOpcodeManager.count(GetOpcodeVersion(version)) == 0) {
						LogWrite(LOGIN__ERROR, 0, "Login", "Incompatible client version provided: %i", version);
						SendLoginDenied();
						return false;
					}
				
					if(EQOpcodeManager.count(GetOpcodeVersion(version)) > 0 && getConnection()){
						getConnection()->SetClientVersion(GetOrigVersion());
						EQ2_16BitString username = packet->getType_EQ2_16BitString_ByName("username");
						EQ2_16BitString password = packet->getType_EQ2_16BitString_ByName("password");
						LoginAccount* acct = database.LoadAccount(username.data.c_str(),password.data.c_str(), net.IsAllowingAccountCreation());
						if(acct){
							Client* otherclient = client_list.FindByLSID(acct->getLoginAccountID());
							if(otherclient)
								otherclient->getConnection()->SendDisconnect(); // This person is already logged in, we don't want them logged in twice, kick the previous client as it might be a ghost
						}
						if(acct){
							SetAccountName(username.data.c_str());
							database.UpdateAccountIPAddress(acct->getLoginAccountID(), getConnection()->GetrIP());
							LogWrite(LOGIN__INFO, 0, "Login", "%s successfully logged in.", (char*)username.data.c_str());
						}
						else
						{
							if (username.size > 0)
								LogWrite(LOGIN__ERROR, 0, "Login", "%s login failed!", (char*)username.data.c_str());
							else
								LogWrite(LOGIN__ERROR, 0, "Login", "[UNKNOWN USER] login failed!");
						}

						if(!acct)
							SendLoginDenied();
						else{
							needs_world_list = true;
							SetLoginAccount(acct);							
							SendLoginAccepted();							
						}
					}
					else{
						cout << "Error bad version: " << version << endl;
						SendLoginDeniedBadVersion();
					}
				}
				else{
					cout << "Error loading LS_LoginRequest packet: \n";
					//DumpPacket(app);
				}
				safe_delete(packet);
				break;
			}
			case OP_KeymapLoadMsg:{
			//	cout << "Received OP_KeymapNoneMsg\n";
				//dunno what this is for
				break;
								  }
			case OP_AllWSDescRequestMsg:{				
				SendWorldList();
				needs_world_list = false;
				database.LoadCharacters(GetLoginAccount(), GetOrigVersion());				
				SendCharList();				
				break;
										}
			case OP_LsClientCrashlogReplyMsg:{
//				DumpPacket(app);
				SaveErrorsToDB(app, "Crash Log");
				break;
											 }
			case OP_LsClientVerifylogReplyMsg:{
//				DumpPacket(app);
				SaveErrorsToDB(app, "Verify Log");
				break;
											  }
			case OP_LsClientAlertlogReplyMsg:{
//				DumpPacket(app);
				SaveErrorsToDB(app, "Alert Log");
				break;
											 }
			case OP_LsClientBaselogReplyMsg:{
//				DumpPacket(app);
				SaveErrorsToDB(app, "Base Log");
				break;
											}
			case OP_AllCharactersDescRequestMsg:{
				break;
			}
			case OP_CreateCharacterRequestMsg:{
				PacketStruct* packet = configReader.getStruct("CreateCharacter", GetOrigVersion());

				playWaitTimer = new Timer ( 15000 );
				playWaitTimer->Start ( );
				cout << "Char Create Request From: " << GetAccountName() << "....";
				DumpPacket(app->pBuffer, app->size);
				if(packet->LoadPacketData(app->pBuffer,app->size)){
					cout << "Loaded Successfully\n";
					//packet->setDataByName("account_id", GetAccountID());
					packet->PrintPacket();
					LWorld* world_server = world_list.FindByID(packet->getType_int32_ByName("server_id"));
					if(!world_server)
					{
						cout << GetAccountName() << " attempted creation of character with an invalid server id of: " << packet->getType_int32_ByName("server_id") << "\n";
						break;
					}
					else
					{
						ServerPacket* outpack = new ServerPacket(ServerOP_CharacterCreate, app->size+sizeof(int16)+sizeof(int32));
						int16 out_version = GetOrigVersion();
						memcpy(outpack->pBuffer, &out_version, sizeof(int16));
						int32 accountId = GetAccountID();
						memcpy(outpack->pBuffer + sizeof(int16), &accountId, sizeof(int32));
						memcpy(outpack->pBuffer + sizeof(int16) + sizeof(int32), app->pBuffer, app->size);
						world_server->SendPacket(outpack);
						safe_delete(outpack);
						createRequest = packet;
					}
				}
				else{
					cout << "Error loading Char Create Packet!!\n";
					safe_delete(packet);
				}
				//	world_list.SendWorldChanged(create.profile.server_id, false, this);
				break;
											  }
			case OP_PlayCharacterRequestMsg:{
				int32 char_id = 0;
				int32 server_id = 0;
				PacketStruct* request = configReader.getStruct("LS_PlayRequest",GetVersion());
				
				if(request && request->LoadPacketData(app->pBuffer,app->size)){
					request->PrintPacket();

					char_id = request->getType_int32_ByName("char_id");
					string name = request->getType_EQ2_16BitString_ByName("name").data.c_str();
					server_id = database.GetServerByNameCharIdAccount(&name, char_id, GetAccountID());
					LWorld* world = world_list.FindByID(server_id);
					cout << server_id << endl;
					if(world && char_id > 0){
						pending_play_char_id = char_id;
						ServerPacket* outpack = new ServerPacket(ServerOP_UsertoWorldReq, sizeof(UsertoWorldRequest_Struct));
						UsertoWorldRequest_Struct* req = (UsertoWorldRequest_Struct*)outpack->pBuffer;
						req->char_id = char_id;
						req->lsaccountid = GetAccountID();
						req->worldid = server_id;

						struct in_addr in;
						in.s_addr = GetIP();
						strcpy(req->ip_address, inet_ntoa(in));
						world->SendPacket(outpack);
						delete outpack;

						safe_delete(playWaitTimer);

						playWaitTimer = new Timer ( 5000 );
						playWaitTimer->Start ( );
					}
					else{
						cout << GetAccountName() << " sent invalid Play Request: \n";
						SendPlayFailed(PLAY_ERROR_PROBLEM);
						DumpPacket(app);
					}
				}
				safe_delete(request);
				break;
											}
			case OP_DeleteCharacterRequestMsg:{
				PacketStruct* request = configReader.getStruct("LS_DeleteCharacterRequest", GetVersion());
				PacketStruct* response = configReader.getStruct("LS_DeleteCharacterResponse", GetVersion());
				if(request && response && request->LoadPacketData(app->pBuffer,app->size)){
					EQ2_16BitString name = request->getType_EQ2_16BitString_ByName("name");
					int32 acct_id = GetAccountID();
					int32 char_id = request->getType_int32_ByName("char_id");
					int32 server_id = request->getType_int32_ByName("server_id");
					if(database.VerifyDelete(acct_id, char_id, name.data.c_str())){
						response->setDataByName("response", 1);
						GetLoginAccount()->removeCharacter((char*)name.data.c_str());
						LWorld* world_server = world_list.FindByID(server_id);
						if(world_server != NULL)
							world_server->SendDeleteCharacter ( char_id , acct_id );
					}
					else
						response->setDataByName("response", 0);
					response->setDataByName("server_id", server_id);
					response->setDataByName("char_id", char_id);
					response->setDataByName("account_id", account_id);
					response->setMediumStringByName("name", (char*)name.data.c_str());
					response->setDataByName("max_characters", 10);

					EQ2Packet* outapp = response->serialize();
					QueuePacket(outapp);

					this->SendCharList();
				}
				safe_delete(request);
				safe_delete(response);
				break;
											  }
			default: {
				char* name = (char*)app->GetOpcodeName();
				if(name)
					cout << name;
				else
					cout << "Unknown";
				cout << " Packet: OPCode: 0x" << hex << setw(2) << setfill('0') << app->GetOpcode() << dec << ", size: " << setw(5) << setfill(' ') << app->Size() << " from " << GetAccountName() << endl;
				if (app->Size() < 128)
					DumpPacket(app);
					 }
			}
			delete app;
		}
	}

	if (!eqnc->CheckActive()) {
		return false;
	}

	return true;
}

void Client::SaveErrorsToDB(EQApplicationPacket* app, char* type){
	int32 size = 0;
	memcpy(&size, app->pBuffer + sizeof(int32), sizeof(int32));
	size++;
	char* message = new char[size];
	memset(message, 0, size);
	z_stream zstream;
	int zerror = 0;
	zstream.next_in		= app->pBuffer + 8;
	zstream.avail_in	= app->size - 8;
	zstream.next_out	= (BYTE*)message;
	zstream.avail_out	= size;
	zstream.zalloc    = Z_NULL;
	zstream.zfree     = Z_NULL;
	zstream.opaque		= Z_NULL;

	zerror = inflateInit( &zstream); 
	if(zerror != Z_OK) {
		safe_delete_array(message);
		return;
	}
	zerror = inflate( &zstream, 0 );
	if(message && strlen(message) > 0)
		database.SaveClientLog(type, message, GetLoginAccount()->getLoginName(), GetVersion());
	safe_delete_array(message);
}

void Client::CharacterApproved(int32 server_id,int32 char_id)
{
	if(createRequest && server_id == createRequest->getType_int32_ByName("server_id")){
		LWorld* world_server = world_list.FindByID(server_id);
		if(!world_server)
			return;

		PacketStruct* packet = configReader.getStruct("LS_CreateCharacterReply", GetOrigVersion());
		if(packet){
			packet->setDataByName("account_id", GetAccountID());
			packet->setDataByName("unknown", 0xFFFFFFFF);
			packet->setDataByName("response", CREATESUCCESS_REPLY);
			packet->setMediumStringByName("name", (char*)createRequest->getType_EQ2_16BitString_ByName("name").data.c_str());
			EQ2Packet* outapp = packet->serialize();
			QueuePacket(outapp);
			safe_delete(packet);
			database.SaveCharacter(createRequest, GetLoginAccount(),char_id);

			// refresh characters for this account
			database.LoadCharacters(GetLoginAccount(), GetOrigVersion());
			
			SendCharList();
		}
	}
	else{
		cout << GetAccountName() << " received invalid CharacterApproval from server: " << server_id << endl;
	}
	safe_delete(createRequest);
}

void Client::CharacterRejected(int8 reason_number)
{
	PacketStruct* packet = configReader.getStruct("LS_CreateCharacterReply", GetVersion());
	if(createRequest && packet){
		packet->setDataByName("account_id", GetAccountID());
		int8 clientReasonNum = reason_number;
		// reason numbers change and instead of updating the world server
		// the login server will hold the up to date #'s
/*
		switch(reason_number)
		{
			// these error codes seem to be removed now, they shutdown the client rather immediately
			// for now we are just going to play a joke on them and say they can't create a new character.
		case INVALIDRACE_REPLY:
		case INVALIDGENDER_REPLY:
			clientReasonNum = 8;
			break;
		case BADNAMELENGTH_REPLY:
			clientReasonNum = 9;
			break;
		case NAMEINVALID_REPLY:
			clientReasonNum = 10;
			break;
		case NAMEFILTER_REPLY:
			clientReasonNum = 11;
			break;
		case NAMETAKEN_REPLY:
			clientReasonNum = 12;
			break;
		case OVERLOADEDSERVER_REPLY:
			clientReasonNum = 13;
			break;
		}
*/
		packet->setDataByName("response", clientReasonNum);
		packet->setMediumStringByName("name", "");
		EQ2Packet* outapp = packet->serialize();
		QueuePacket(outapp);
		safe_delete(packet);
	}
	/*LS_CreateCharacterReply reply(GetAccountID(), reason_number, create.profile.name.data);
	EQ2Packet* outapp = reply.serialize();
	QueuePacket(outapp);
	create.Clear();*/
}

void Client::SendCharList(){
	/*PacketStruct* packet = configReader.getStruct("LS_CreateCharacterReply");
	packet->setDataByName("account_id", GetAccountID());
	packet->setDataByName("response", reason_number);
	packet->setDataByName("name", &create.profile.name);
	EQ2Packet* outapp = packet->serialize();
	QueuePacket(outapp);
	safe_delete(packet);*/
	LogWrite(LOGIN__INFO, 0, "Login", "[%s] sending character list.", GetAccountName());
	LS_CharSelectList list;
	list.loadData(GetAccountID(), GetLoginAccount()->charlist, GetOrigVersion()); 
	EQ2Packet* outapp = list.serialize(GetOrigVersion());
	DumpPacket(outapp);
	QueuePacket(outapp);

}
void Client::SendLoginDeniedBadVersion(){
	EQ2Packet* app = new EQ2Packet(OP_LoginReplyMsg, 0, sizeof(LS_LoginResponse));
	LS_LoginResponse* ls_response = (LS_LoginResponse*)app->pBuffer;
	ls_response->reply_code = 6;
	ls_response->unknown03 = 0xFFFFFFFF;
	ls_response->unknown04 = 0xFFFFFFFF;
	QueuePacket(app);
}
void Client::SendLoginDenied(){
	EQ2Packet* app = new EQ2Packet(OP_LoginReplyMsg, 0, sizeof(LS_LoginResponse));
	LS_LoginResponse* ls_response = (LS_LoginResponse*)app->pBuffer;
	ls_response->reply_code = 1;
	ls_response->unknown03 = 0xFFFFFFFF;
	ls_response->unknown04 = 0xFFFFFFFF;
	QueuePacket(app);
}

void Client::SendLoginAccepted() {
	PacketStruct* packet = configReader.getStruct("LS_LoginReplyMsg", GetOrigVersion());
	int i = 0;
	if (packet)
	{
		packet->setDataByName("account_id", 1);
		//packet->setDataByName("login_response", 0);
		//packet->setDataByName("reset_appearance", 0);
		packet->setDataByName("do_not_force_soga", 1);
		//packet->setDataByName("race_unknown", 0x3F);
		//packet->setDataByName("unknown11", 2); // can be 7
		packet->setDataByName("sub_level", 2);
		packet->setDataByName("race_flag", 0x1FFFFF);
		packet->setDataByName("class_flag", 0x7FFFFFE);
		packet->setMediumStringByName("username", GetAccountName());
		packet->setMediumStringByName("password", GetAccountName());
		packet->setDataByName("unknown5", 0x7C);
		packet->setDataByName("unknown7", 0xFF6FFFBF);

		// Image 2020 Notes
		// Login Server only supports AoM at current time, but we will need to keep in mind the structure for 60100 or later calls for additional custom fields
		// >=60100
		packet->setDataByName("unknown11", 0x0E);

		packet->setDataByName("unknown7a", 0xFFFF);
		packet->setDataByName("unknown8", 0xFF, 1);

		EQ2Packet* outapp = packet->serialize();
		QueuePacket(outapp);
		safe_delete(packet);
	}
}

void Client::SendWorldList(){
	EQ2Packet* pack = world_list.MakeServerListPacket(lsadmin, version);
	EQ2Packet* dupe = pack->Copy();
	DumpPacket(dupe->pBuffer,dupe->size);
	QueuePacket(dupe);
	return;
}

void Client::QueuePacket(EQ2Packet* app){
	eqnc->EQ2QueuePacket(app);
}

void Client::WorldResponse(int32 worldid, int8 response, char* ip_address, int32 port, int32 access_key)
{
	LWorld* world = world_list.FindByID(worldid);
	if(world == 0) {
		FatalError(0);
		return;
	}
	if(response != 1){
		if(response == PLAY_ERROR_CHAR_NOT_LOADED){
			string pending_play_char_name = database.GetCharacterName(pending_play_char_id, worldid);
			if(database.VerifyDelete(GetAccountID(), pending_play_char_id, pending_play_char_name.c_str())){
				GetLoginAccount()->removeCharacter((char*)pending_play_char_name.c_str());
			}
		}
		FatalError(response);
		return;
	}

	PacketStruct* response_packet = configReader.getStruct("LS_PlayResponse", GetVersion());
	if(response_packet){
		safe_delete(playWaitTimer);
		response_packet->setDataByName("response", 1);
		response_packet->setSmallStringByName("server", ip_address);
		response_packet->setDataByName("port", port);
		response_packet->setDataByName("account_id", GetAccountID());
		response_packet->setDataByName("access_code", access_key);
		EQ2Packet* outapp = response_packet->serialize();
		QueuePacket(outapp);
		safe_delete(response_packet);
		disconnectTimer = new Timer(1000);
		disconnectTimer->Start();
	}
	return;
}
void Client::FatalError(int8 response) {
	safe_delete(playWaitTimer);
	SendPlayFailed(response);
}

void Client::SendPlayFailed(int8 response){
	PacketStruct* response_packet = configReader.getStruct("LS_PlayResponse", GetVersion());
	if(response_packet){
		response_packet->setDataByName("response", response);
		response_packet->setSmallStringByName("server", "");
		response_packet->setDataByName("port", 0);
		response_packet->setDataByName("account_id", GetAccountID());
		response_packet->setDataByName("access_code", 0);
		EQ2Packet* outapp = response_packet->serialize();
		QueuePacket(outapp);
		safe_delete(response_packet);
	}
}

void ClientList::Add(Client* client) {
	MClientList.writelock();
	client_list[client] = true;
	MClientList.releasewritelock();
}

Client* ClientList::Get(int32 ip, int16 port) {
	Client* ret = 0;
	map<Client*, bool>::iterator itr;
	MClientList.readlock();
	for(itr = client_list.begin(); itr != client_list.end(); itr++){
		if(itr->first->GetIP() == ip && itr->first->GetPort() == port){
			ret = itr->first;
			break;
		}
	}
	MClientList.releasereadlock();
	return ret;
}

void ClientList::FindByCreateRequest(){
	Client* client = 0;
	map<Client*, bool>::iterator itr;
	MClientList.readlock();
	for(itr = client_list.begin(); itr != client_list.end(); itr++){
		if(itr->first->AwaitingCharCreationRequest()){
			if(!client)
				client = itr->first;
			else{
				client = 0;//more than 1 character waiting, dont want to send rejection to wrong one
				break;
			}
		}
	}
	MClientList.releasereadlock();
	if(client)
		client->CharacterRejected(UNKNOWNERROR_REPLY);
}

Client* ClientList::FindByLSID(int32 lsaccountid) {
	Client* client = 0;
	map<Client*, bool>::iterator itr;
	MClientList.readlock();
	for(itr = client_list.begin(); itr != client_list.end(); itr++){
		if(itr->first->GetAccountID() == lsaccountid){
			client = itr->first;
			break;
		}
	}
	MClientList.releasereadlock();
	return client;
}
void ClientList::SendPacketToAllClients(EQ2Packet* app){
	Client* client = 0;
	map<Client*, bool>::iterator itr;
	MClientList.readlock();
	if(client_list.size() > 0){
		for(itr = client_list.begin(); itr != client_list.end(); itr++){
			itr->first->QueuePacket(app->Copy());
		}
	}
	safe_delete(app);
	MClientList.releasereadlock();
}
void ClientList::Process() {
	Client* client = 0;
	vector<Client*> erase_list;
	map<Client*, bool>::iterator itr;
	MClientList.readlock();	
	for(itr = client_list.begin(); itr != client_list.end(); itr++){		
		client = itr->first;
		if(!client->Process())
			erase_list.push_back(client);		
	}
	MClientList.releasereadlock();
	if(erase_list.size() > 0){
		vector<Client*>::iterator erase_itr;
		MClientList.writelock();
		for(erase_itr = erase_list.begin(); erase_itr != erase_list.end(); erase_itr++){
			client = *erase_itr;					
			struct in_addr	in;
			in.s_addr = client->getConnection()->GetRemoteIP();
			net.numclients--;
			cout << Timer::GetCurrentTime2() << " Removing client from ip: " << inet_ntoa(in) << " port: " << ntohs(client->getConnection()->GetRemotePort()) << " Name: " << client->GetAccountName() << endl;
			client->getConnection()->SetState(CLOSED);
			net.UpdateWindowTitle();
			client_list.erase(client);
		}
		MClientList.releasewritelock();
	}
}
