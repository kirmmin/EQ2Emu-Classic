/*  
	EQ2Emulator:  Everquest II Server Emulator
	Copyright (C) 2007  EQ2EMulator Development Team (http://www.eq2emulator.net)

	This file is part of EQ2Emulator.
*/
#ifndef LOGIN_STRUCTS_H
#define LOGIN_STRUCTS_H

#include "../common/types.h"
#include "PacketHeaders.h"

#pragma pack(1)
struct LS_LoginRequest{
	EQ2_16BitString AccessCode;
	EQ2_16BitString unknown1;
	EQ2_16BitString username;
	EQ2_16BitString password;
	EQ2_16BitString unknown2[4];
	int16			unknown3;
	int32			unknown4[2];
};
struct LS_WorldStatusChanged{
	int32	server_id;
	int8	up;
	int8	locked;
	int8	hidden;
};
struct LS_PlayCharacterRequest{
	int32	character_id;
	int32	server_id;
	int16	unknown1;
};
struct LS_OLDPlayCharacterRequest{
	int32	character_id;
	EQ2_16BitString name;
};
struct LS_CharListAccountInfo{
	int32	account_id;
	int32	unknown1;
	int16	unknown2;
	int32	unknown3;
	int8	unknown4;
	int32	unknown5[4];
	int16	unknown6;
//	int8	unknown7; // adds 'free' option..
};
#pragma pack()

#endif
