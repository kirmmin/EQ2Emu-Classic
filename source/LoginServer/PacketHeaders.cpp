/*  
	EQ2Emulator:  Everquest II Server Emulator
	Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

	This file is part of EQ2Emulator.
*/
#include "PacketHeaders.h"
#include "../common/MiscFunctions.h"
#include "LWorld.h"

extern LWorldList	world_list;

void LS_DeleteCharacterRequest::loadData(EQApplicationPacket* packet){
	InitializeLoadData(packet->pBuffer, packet->size);
	LoadData(character_number);
	LoadData(server_id);
	LoadData(spacer);
	LoadDataString(name);
}

EQ2Packet* LS_CharSelectList::serialize(int16 version){
	LS_CharListAccountInfo account_info;
	account_info.account_id = account_id;
	account_info.unknown1 = 0xFFFFFFFF;
	account_info.unknown2 = 0;
	account_info.unknown3 = 10;
	account_info.unknown4 = 0;
	for(int i=0;i<3;i++)
		account_info.unknown5[i] = 0xFFFFFFFF;
	account_info.unknown5[3] = 0;

	Clear();
	AddData(num_characters);
	AddData(char_data);
	AddData(account_info);
	return new EQ2Packet(OP_AllCharactersDescReplyMsg, getData(), getDataSize());
}

void LS_CharSelectList::addChar(uchar* data, int16 size){
	char_data.append((char*)data, size);
}

void LS_CharSelectList::loadData(int32 account, vector<CharSelectProfile*> charlist, int16 version){
	vector<CharSelectProfile*>::iterator itr;
	account_id = account;
	num_characters = 0;
	char_data = "";
	CharSelectProfile* character = 0;
	for(itr = charlist.begin();itr != charlist.end();itr++){
		character = *itr;
		int32 serverID = character->packet->getType_int32_ByName("server_id");
		if(serverID == 0 || !world_list.FindByID(serverID))
			continue;
		num_characters++;
		character->SaveData(version);
		addChar(character->getData(), character->getDataSize());
	}
}

void CharSelectProfile::SaveData(int16 in_version){
	Clear();
	AddData(*packet->serializeString());
}