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
#ifndef EQ2EMU_DATABASE_H
#define EQ2EMU_DATABASE_H

#ifdef WIN32
	#include <WinSock2.h>
	#include <windows.h>
#endif
#include <mysql.h>

#include "dbcore.h"
#include "types.h"
#include "linked_list.h"
#include "EQStream.h"
#include "MiscFunctions.h"
#include "Mutex.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class Database : public DBcore
{
public:
	Database();
	~Database();
	bool Init();
	bool LoadVariables();
	void HandleMysqlError(int32 errnum);
	map<string, uint16> GetOpcodes(int16 version);
	map<int16, int16> GetVersions();

protected:
	
private:
	void InitVars();
};
class Query{
public:
	Query(){
		result = 0;
		affected_rows = 0;
		last_insert_id = 0;
		errnum = 0;
		row = 0;
		retry = true;
		escaped_name = 0;
		escaped_pass = 0;
		escaped_data1 = 0;
		multiple_results = 0;
		memset(errbuf, 0, sizeof(errbuf));
	}
	~Query(){
		if(result)
			mysql_free_result(result);
		result = 0;
		safe_delete(affected_rows);
		safe_delete(last_insert_id);
		safe_delete_array(escaped_name);
		safe_delete_array(escaped_pass);
		safe_delete_array(escaped_data1);
		if(multiple_results){
			vector<MYSQL_RES*>::iterator itr;
			for(itr = multiple_results->begin(); itr != multiple_results->end(); itr++){
				mysql_free_result(*itr);
			}
			safe_delete(multiple_results);
		}
	}
	int32		GetLastInsertedID() { return *last_insert_id; }
	int32		GetAffectedRows() { return *affected_rows; }
	MYSQL_RES*	GetResult() { return result; }
	MYSQL_RES*	RunQuery2(string in_query, QUERY_TYPE type);
	char*		GetError() { return errbuf; }
	int32		GetErrorNumber(){ return errnum; }
	const char*	GetQuery() { return query.c_str(); }
	char*		GetField(int8 field_num) {
		if(!row && result)
			*row = mysql_fetch_row(result);
		if(row && result && field_num < mysql_num_fields(result))
			return *row[field_num];
		else 
			return NULL;
	}
	void		NextRow(){
		if(result)
			*row = mysql_fetch_row(result);
	}
	MYSQL_RES*	RunQuery2(QUERY_TYPE type, const char* format, ...);
	char* escaped_name;
	char* escaped_pass;
	char* escaped_data1;
private:
	string query;
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	vector<MYSQL_RES*>* multiple_results;
	int32* affected_rows;
	int32* last_insert_id;
	int32 errnum;
	bool retry;
	MYSQL_ROW* row;
	MYSQL	mysql;
};
#endif
