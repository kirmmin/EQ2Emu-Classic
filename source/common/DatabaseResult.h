#ifndef COMMON_DATABASERESULT_H_
#define COMMON_DATABASERESULT_H_

#include "types.h"
#ifdef _WIN32
#include <WinSock2.h>	//#include <my_global.h> when we/if we go to winsock2 :/
#endif
#include <mysql.h>

class DatabaseResult {
public:
	DatabaseResult();
	virtual ~DatabaseResult();

	bool StoreResult(MYSQL_RES *res);
	bool Next();

	bool IsNull(unsigned int index);
	bool IsNullStr(const char *field_name);
	int8 GetInt8(unsigned int index);
	int8 GetInt8Str(const char *field_name);
	sint8 GetSInt8(unsigned int index);
	sint8 GetSInt8Str(const char *field_name);
	int16 GetInt16(unsigned int index);
	int16 GetInt16Str(const char *field_name);
	sint16 GetSInt16(unsigned int index);
	sint16 GetSInt16Str(const char *field_name);
	int32 GetInt32(unsigned int index);
	int32 GetInt32Str(const char *field_name);
	sint32 GetSInt32(unsigned int index);
	sint32 GetSInt32Str(const char *field_name);
	int64 GetInt64(unsigned int index);
	int64 GetInt64Str(const char *field_name);
	sint64 GetSInt64(unsigned int index);
	sint64 GetSInt64Str(const char *field_name);
	float GetFloat(unsigned int index);
	float GetFloatStr(const char *field_name);
	char GetChar(unsigned int index);
	char GetCharStr(const char *field_name);
	const char * GetString(unsigned int index);
	const char * GetStringStr(const char *field_name);

	unsigned int GetNumRows() {return result == NULL ? 0 : (unsigned int)mysql_num_rows(result);}
	
private:
	MYSQL_RES *result;
	MYSQL_ROW row;
	char **field_names;
	unsigned int num_fields;

	const char * GetFieldValue(unsigned int index);
	const char * GetFieldValueStr(const char *field_name);
};

#endif
