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
// This class will split up a string smartly at the div character (default is space and tab)
// Seperator.arg[i] is a copy of the string chopped at the divs
// Seperator.argplus[i] is a pointer to the original string so it doesnt end at the div

// Written by Quagmire
#ifndef SEPERATOR_H
#define SEPERATOR_H

#include <string.h>
#include <stdlib.h>

class Seperator
{
public:
	Seperator(const char* message, char div = ' ', int16 in_maxargnum = 10, int16 arglen = 100, bool iObeyQuotes = false, char div2 = '\t', char div3 = 0, bool iSkipEmpty = true) {
		int i;
		argnum = 0;
		msg = strdup(message);
		this->maxargnum = in_maxargnum;
		argplus = new const char *[maxargnum+1];
		arg = new char *[maxargnum+1];
		for (i=0; i<=maxargnum; i++) {
			argplus[i]=arg[i] = new char[arglen+1];
			memset(arg[i], 0, arglen+1);
		}

		int len = strlen(message);
		int s = 0, l = 0;
		bool inarg = (!iSkipEmpty || !(message[0] == div || message[0] == div2 || message[0] == div3));
		bool inquote = (iObeyQuotes && (message[0] == '\"' || message[0] == '\''));
		argplus[0] = message;
		if (len == 0)
			return;

		for (i=0; i<len; i++) {
//			cout << i << ": 0x" << hex << (int) message[i] << dec << " " << message[i] << endl;
			if (inarg) {
				if ((inquote == false && (message[i] == div || message[i] == div2 || message[i] == div3)) || (inquote && (message[i] == '\'' || message[i] == '\"') && (message[i+1] == div || message[i+1] == div2 || message[i+1] == div3 || message[i+1] == 0))) {
					inquote = false;					
					l = i-s;					
					if (l >= arglen)
						l = arglen;
					if (l){
						if(l > 1 && (argplus[argnum][0] == '\'' || argplus[argnum][0] == '\"')){
							l--;
							memcpy(arg[argnum], argplus[argnum]+1, l);
						}
						else
							memcpy(arg[argnum], argplus[argnum], l);
					}
					arg[argnum][l] = 0;
					argnum++;
					if (iSkipEmpty)
						inarg = false;
					else {
						s=i+1;
						argplus[argnum] = &message[s];
					}
				}
			}
			else if (iObeyQuotes && (message[i] == '\"' || message[i] == '\'')) {
				inquote = true;
			}
			else {
				s = i;
				argplus[argnum] = &message[s];
				if (!(message[i] == div || message[i] == div2 || message[i] == div3)) {
					inarg = true;
				}
			}
			if (argnum > maxargnum)
				break;
		}
		if (inarg && argnum <= maxargnum) {
			l = i-s;
			if (l >= arglen)
				l = arglen;
			if (l)
				memcpy(arg[argnum], argplus[argnum], l);
		}
	}
	~Seperator() {
		for (int i=0; i<=maxargnum; i++)
			safe_delete_array(arg[i]);
		safe_delete_array(arg);
		safe_delete_array(argplus);
		if (msg)
			free(msg);
	}
	int16 argnum;
	char** arg;
	const char** argplus;
	char * msg;
	bool IsSet(int num) const {
		return IsSet(arg[num]);
	}
	bool IsNumber(int num) const {
		return IsNumber(arg[num]);
	}
	bool IsHexNumber(int num) const {
		return IsHexNumber(arg[num]);
	}
	static bool IsSet(const char *check) {
		return check[0] != '\0';
	}
	static bool IsNumber(const char* check) {
		bool SeenDec = false;
		int len = strlen(check);
		if (len == 0) {
			return false;
		}
		int i;
		for (i = 0; i < len; i++) {
			if (check[i] < '0' || check[i] > '9') {
				if (check[i] == '.' && !SeenDec) {
					SeenDec = true;
				}
				else if (i == 0 && (check[i] == '-' || check[i] == '+') && !check[i+1] == 0) {
					// this is ok, do nothin
				}
				else {
					return false;
				}
			}
		}
		return true;
	}
	static bool IsHexNumber(char* check) {
		int len = strlen(check);
		if (len < 3)
			return false;
		if (check[0] != '0' || (check[1] != 'x' && check[1] != 'X'))
			return false;
		for (int i=2; i<len; i++) {
			if ((check[i] < '0' || check[i] > '9') && (check[i] < 'A' || check[i] > 'F') && (check[i] < 'a' || check[i] > 'f'))
				return false;
		}
		return true;
	}
	inline int16 GetMaxArgNum() const { return maxargnum; }
	inline int16 GetArgNumber() const { return argnum; }
private:
	int16 maxargnum;
};

#endif
