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
#ifndef __EQ2_SKILLS_H__
#define __EQ2_SKILLS_H__

#include <map>
#include "../common/ConfigReader.h"
#include "../common/types.h"
#include "MutexMap.h"

#define SKILL_TYPE_COMBAT 1
#define SKILL_TYPE_SPELLCASTING 2
#define SKILL_TYPE_AVOIDANCE 3
#define SKILL_TYPE_HARVESTING 6
#define SKILL_TYPE_ARTISAN 7
#define SKILL_TYPE_CRAFTSMAN 8
#define SKILL_TYPE_OUTFITTER 9
#define SKILL_TYPE_SCHOLAR 10
#define SKILL_TYPE_GENERAL 12

#define SKILL_ID_SCULPTING 1039865549
#define SKILL_ID_FLETCHING 3076004370
#define SKILL_ID_ARTISTRY 3881305672
#define SKILL_ID_TAILORING 2082133324
#define SKILL_ID_METALSHAPING 3108933728
#define SKILL_ID_METALWORKING 4032608519
#define SKILL_ID_SCRIBING 773137566
#define SKILL_ID_CHEMISTRY 2557647574
#define SKILL_ID_ARTIFICING 3330500131

/* Each SkillBonus is comprised of multiple possible skill bonus values.  This is so one spell can modify
   more than one skill */
struct SkillBonusValue {
	int32 skill_id;
	float value;
};

struct SkillBonus {
	int32 spell_id;
	map<int32, SkillBonusValue*> skills;
};


class Skill{
public:
	Skill();
	Skill(Skill* skill);
	int32			skill_id;
	int16			current_val;
	int16			previous_val;
	int16			max_val;
	int32			skill_type;
	int8			display;
	EQ2_16BitString short_name;
	EQ2_16BitString name;
	EQ2_16BitString description;
	bool			save_needed;

	int			CheckDisarmSkill(int16 targetLevel, int8 chest_difficulty=0);
};

class MasterSkillList{
public:
	MasterSkillList();
	~MasterSkillList();
	void AddSkill(Skill* skill);
	int16 GetSkillCount();
	EQ2Packet* GetPopulateSkillsPacket(int16 version);
	map<int32, Skill*>* GetAllSkills();
	Skill* GetSkill(int32 skill_id);
	Skill* GetSkillByName(const char* skill_name);

private:
	map<int32, Skill*> skills;
	map<int16, EQ2Packet*> populate_packets;
};

class PlayerSkillList{
public:
	PlayerSkillList();
	~PlayerSkillList();
	void RemoveSkill(Skill* skill);
	void AddSkill(Skill* new_skill);
	bool CheckSkillIncrease(Skill* skill);	
	Skill* GetSkillByName(const char* name);
	bool HasSkill(int32 skill_id);
	Skill* GetSkill(int32 skill_id);

	void IncreaseSkill(Skill* skill, int16 amount);
	void IncreaseSkill(int32 skill_id, int16 amount);
	void DecreaseSkill(Skill* skill, int16 amount);
	void DecreaseSkill(int32 skill_id, int16 amount);
	void SetSkill(Skill* skill, int16 value);
	void SetSkill(int32 skill_id, int16 value);

	void IncreaseSkillCap(Skill* skill, int16 amount);
	void IncreaseSkillCap(int32 skill_id, int16 amount);
	void DecreaseSkillCap(Skill* skill, int16 amount);
	void DecreaseSkillCap(int32 skill_id, int16 amount);
	void SetSkillCap(Skill* skill, int16 value);
	void SetSkillCap(int32 skill_id, int16 value);
	void IncreaseAllSkillCaps(int16 value);
	void IncreaseSkillCapsByType(int8 type, int16 value);
	void SetSkillCapsByType(int8 type, int16 value);
	void AddSkillUpdateNeeded(Skill* skill);

	void AddSkillBonus(int32 spell_id, int32 skill_id, float value);
	SkillBonus*	GetSkillBonus(int32 spell_id);
	void RemoveSkillBonus(int32 spell_id);

	int16 CalculateSkillValue(int32 skill_id, int16 current_val);
	int16 CalculateSkillMaxValue(int32 skill_id, int16 max_val);
	EQ2Packet* GetSkillPacket(int16 version);
	vector<Skill*>* GetSaveNeededSkills();
	vector<Skill*>* GetSkillUpdates();
	map<int32, Skill*>* GetAllSkills();
	bool HasSkillUpdates();

private:
	volatile bool has_updates;
	Mutex MSkillUpdates;
	int16 packet_count;
	uchar* orig_packet;
	uchar* xor_packet;
	map<int32, Skill*> skills;
	map<string, Skill*> name_skill_map;
	vector<Skill*> skill_updates;
	MutexMap<int32, SkillBonus*> skill_bonus_list;
};

#endif

