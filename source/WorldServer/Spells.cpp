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
#include "Spells.h"
#include "../common/ConfigReader.h"
#include "WorldDatabase.h"
#include "../common/Log.h"
#include "Traits/Traits.h"
#include "AltAdvancement/AltAdvancement.h"
#include <cmath>

extern ConfigReader configReader;
extern WorldDatabase database;
extern MasterTraitList master_trait_list;
extern MasterAAList master_aa_list;
extern MasterSpellList master_spell_list;

Spell::Spell(){
	spell = new SpellData;
	heal_spell = false;
	buff_spell = false;
	damage_spell = false;
	control_spell = false;
	offense_spell = false;
	MSpellInfo.SetName("Spell::MSpellInfo");
}

Spell::Spell(SpellData* in_spell){
	spell = in_spell;
	heal_spell = false;
	buff_spell = false;
	damage_spell = false;
	control_spell = false;
	offense_spell = false;
	MSpellInfo.SetName("Spell::MSpellInfo");
}

Spell::~Spell(){
	vector<LevelArray*>::iterator itr1;
	for(itr1=levels.begin();itr1!=levels.end();itr1++) {
		safe_delete(*itr1);
	}
	vector<SpellDisplayEffect*>::iterator itr2;
	for(itr2=effects.begin();itr2!=effects.end();itr2++) {
		safe_delete(*itr2);
	}
	vector<LUAData*>::iterator itr3;
	for(itr3=lua_data.begin();itr3!=lua_data.end();itr3++) {
		safe_delete(*itr3);
	}
	safe_delete(spell);
}

void Spell::AddSpellLuaData(int8 type, int int_value, int int_value2, float float_value, float float_value2, bool bool_value, string string_value, string string_value2, string helper){
	LUAData* data = new LUAData;
	data->type = type;
	data->int_value = int_value;
	data->int_value2 = int_value2;
	data->float_value = float_value;
	data->float_value2 = float_value2;
	data->bool_value = bool_value;
	data->string_value = string_value;
	data->string_value2 = string_value2;
	data->string_helper = helper;

	MSpellInfo.lock();
	lua_data.push_back(data);
	MSpellInfo.unlock();
}

void Spell::AddSpellLuaDataInt(int value, int value2, string helper) {
	LUAData *data = new LUAData;

	data->type = 0;
	data->int_value = value;
	data->int_value2 = value2;
	data->float_value = 0;
	data->float_value2 = 0;
	data->bool_value = false;
	data->string_helper = helper;

	MSpellInfo.lock();
	lua_data.push_back(data);
	MSpellInfo.unlock();
}

void Spell::AddSpellLuaDataFloat(float value, float value2, string helper) {
	LUAData *data = new LUAData;

	data->type = 1;
	data->int_value = 0;
	data->int_value2 = 0;
	data->float_value = value;
	data->float_value2 = value2;
	data->bool_value = false;
	data->string_helper = helper;

	MSpellInfo.lock();
	lua_data.push_back(data);
	MSpellInfo.unlock();
}

void Spell::AddSpellLuaDataBool(bool value, string helper) {
	LUAData *data = new LUAData;

	data->type = 2;
	data->int_value = 0;
	data->float_value = 0;
	data->bool_value = value;
	data->string_helper = helper;

	MSpellInfo.lock();
	lua_data.push_back(data);
	MSpellInfo.unlock();
}

void Spell::AddSpellLuaDataString(string value, string value2,string helper) {
	LUAData *data = new LUAData;

	data->type = 3;
	data->int_value = 0;
	data->int_value2 = 0;
	data->float_value = 0;
	data->float_value2 = 0;
	data->bool_value = false;
	data->string_value = value;
	data->string_value2 = value2;
	data->string_helper = helper;

	MSpellInfo.lock();
	lua_data.push_back(data);
	MSpellInfo.unlock();
}

int16 Spell::GetLevelRequired(Client* client){
	int16 ret = 0xFFFF;
	if(!client)
		return ret;
	LevelArray* level = 0;
	vector<LevelArray*>::iterator itr;
	for(itr = levels.begin(); itr != levels.end(); itr++){
		level = *itr;
		if(level && level->adventure_class == client->GetPlayer()->GetAdventureClass()){
			ret = level->spell_level/10;
			break;
		}
	}
	return ret;
}
void Spell::SetAAPacketInformation(PacketStruct* packet, AltAdvanceData* data, Client* client, bool display_tier) {
	int8 current_tier = client->GetPlayer()->GetSpellTier(spell->id);
	Spell* next_spell;
	SpellData* spell2;
	if (data->maxRank > current_tier) {
		next_spell = master_spell_list.GetSpell(spell->id, current_tier + 1);
		spell2 = next_spell->GetSpellData();
	}
	SpellDisplayEffect* effect2;

	//next_spell->effects[1]->description;


	int xxx = 0;

	int16 hp_req = 0;
	int16 power_req = 0;

	if (current_tier > 0) {
		packet->setSubstructDataByName("spell_info", "current_id", spell->id);
		packet->setSubstructDataByName("spell_info", "current_icon", spell->icon);
		packet->setSubstructDataByName("spell_info", "current_icon2", spell->icon_heroic_op);	// fix struct element name eventually
		packet->setSubstructDataByName("spell_info", "current_icontype", spell->icon_backdrop);	// fix struct element name eventually

		if (packet->GetVersion() >= 63119) {
			packet->setSubstructDataByName("spell_info", "current_version", 0x04);
			packet->setSubstructDataByName("spell_info", "current_sub_version", 0x24);
		}
		else if (packet->GetVersion() >= 58617) {
			packet->setSubstructDataByName("spell_info", "current_version", 0x03);
			packet->setSubstructDataByName("spell_info", "current_sub_version", 0x131A);
		}
		else {
			packet->setSubstructDataByName("spell_info", "current_version", 0x00);
			packet->setSubstructDataByName("spell_info", "current_sub_version", 0xD9);
		}

		packet->setSubstructDataByName("spell_info", "current_type", spell->type);
		packet->setSubstructDataByName("spell_info", "unknown_MJ1d", 1); //63119 test
		packet->setSubstructDataByName("spell_info", "current_class_skill", spell->class_skill);
		packet->setSubstructDataByName("spell_info", "current_mastery_skill", spell->mastery_skill);
		packet->setSubstructDataByName("spell_info", "duration_flag", spell->duration_until_cancel);


		if (client && spell->type != 2) {
			sint8 spell_text_color = client->GetPlayer()->GetArrowColor(GetLevelRequired(client));
			if (spell_text_color != ARROW_COLOR_WHITE && spell_text_color != ARROW_COLOR_RED && spell_text_color != ARROW_COLOR_GRAY)
				spell_text_color = ARROW_COLOR_WHITE;
			spell_text_color -= 6;
			if (spell_text_color < 0)
				spell_text_color *= -1;
			packet->setSubstructDataByName("spell_info", "current_spell_text_color", (xxx == 1 ? 0xFFFFFFFF : spell_text_color));
		}
		else {
			packet->setSubstructDataByName("spell_info", "current_spell_text_color", (xxx == 1 ? 0xFFFFFFFF : 3));
		}
		packet->setSubstructDataByName("spell_info", "current_spell_text_color", (xxx == 1 ? 0xFFFFFFFF : 3));
		packet->setSubstructDataByName("spell_info", "current_tier", (spell->tier));

		if (spell->type != 2) {
			packet->setArrayLengthByName("current_num_levels", 0);
			for (int32 i = 0; i < levels.size(); i++) {
				packet->setArrayDataByName("spell_info_aa_adventure_class", levels[i]->adventure_class, i);
				packet->setArrayDataByName("spell_info_aa_tradeskill_class", levels[i]->tradeskill_class, i);
				packet->setArrayDataByName("spell_info_aa_spell_level", levels[i]->spell_level, i);
			}
		}
		//packet->setSubstructDataByName("spell_info","unknown9", 20);

		if (client) {
			hp_req = GetHPRequired(client->GetPlayer());
			power_req = GetPowerRequired(client->GetPlayer());

			// might need version checks around these?
			if (client->GetVersion() >= 1193)
			{
				int16 savagery_req = GetSavageryRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "current_savagery_req", savagery_req);
				packet->setSubstructDataByName("spell_info", "current_savagery_upkeep", spell->savagery_upkeep);
			}
			if (client->GetVersion() >= 57048)
			{
				int16 dissonance_req = GetDissonanceRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "dissonance_req", dissonance_req);
				packet->setSubstructDataByName("spell_info", "dissonance_upkeep", spell->dissonance_upkeep);
			}
		}
		packet->setSubstructDataByName("spell_info", "current_health_req", hp_req);
		packet->setSubstructDataByName("spell_info", "current_health_upkeep", spell->hp_upkeep);
		packet->setSubstructDataByName("spell_info", "current_power_req", power_req);
		packet->setSubstructDataByName("spell_info", "current_power_upkeep", spell->power_upkeep);
		packet->setSubstructDataByName("spell_info", "current_req_concentration", spell->req_concentration);
		//unknown1 savagery???
		packet->setSubstructDataByName("spell_info", "current_cast_time", spell->cast_time);
		packet->setSubstructDataByName("spell_info", "current_recovery", spell->recovery);
		packet->setSubstructDataByName("spell_info", "current_recast", spell->recast);
		packet->setSubstructDataByName("spell_info", "current_radius", spell->radius);
		packet->setSubstructDataByName("spell_info", "current_max_aoe_targets", spell->max_aoe_targets);
		packet->setSubstructDataByName("spell_info", "current_friendly_spell", spell->friendly_spell);
		// rumber of reagents with array









		packet->setSubstructArrayLengthByName("spell_info", "current_num_effects", (xxx == 1 ? 0 : effects.size()));
		for (int32 i = 0; i < effects.size(); i++) {
			packet->setArrayDataByName("current_subbulletflag", effects[i]->subbullet, i);
			string effect_message;
			if (effects[i]->description.length() > 0) {
				effect_message = effects[i]->description;
				if (effect_message.find("%LM") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LM");
					int data_index = stoi(effect_message.substr(string_index + 3, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					string strValue = to_string(value);
					strValue.erase(strValue.find_last_not_of('0') + 1, std::string::npos);
					effect_message.replace(effect_message.find("%LM"), 5, strValue);
				}
				// Magic damage min
				if (effect_message.find("%DML") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%DML");
					int data_index = stoi(effect_message.substr(string_index+4, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%DML"), 6, damage);
				}
				// Magic damage max
				if (effect_message.find("%DMH") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%DMH");
					int data_index = stoi(effect_message.substr(string_index+4, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%DMH"), 6, damage);
				}
				// level based Magic damage min
				if (effect_message.find("%LDML") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LDML");
					int data_index = stoi(effect_message.substr(string_index+5, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%LDML"), 7, damage);
				}
				// level based Magic damage max
				if (effect_message.find("%LDMH") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LDMH");
					int data_index = stoi(effect_message.substr(string_index+5, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%LDMH"), 7, damage);
				}
				//GetZone()->SimpleMessage(CHANNEL_COLOR_SPELL_EFFECT, effect_message.c_str(), victim, 50);
				packet->setArrayDataByName("current_effect", effect_message.c_str(), i);
			}
			packet->setArrayDataByName("current_percentage", effects[i]->percentage, i);
		}
		if (display_tier == true)
			packet->setSubstructDataByName("spell_info", "current_display_spell_tier", 1);// spell2->display_spell_tier);
		else
			packet->setSubstructDataByName("spell_info", "current_display_spell_tier", 1);// 0);

		packet->setSubstructDataByName("spell_info", "current_unknown_1", 1);// 0);
		//unkown1_1
		packet->setSubstructDataByName("spell_info", "current_minimum_range", spell->min_range);
		packet->setSubstructDataByName("spell_info", "current_range", spell->range);
		packet->setSubstructDataByName("spell_info", "current_duration_1", spell->duration1);
		packet->setSubstructDataByName("spell_info", "current_duration_2", spell->duration2);

		packet->setSubstructDataByName("spell_info", "current_duration_flag", spell->duration_until_cancel);
		packet->setSubstructDataByName("spell_info", "current_target", spell->target_type);





		packet->setSubstructDataByName("spell_info", "current_can_effect_raid", spell->can_effect_raid);
		packet->setSubstructDataByName("spell_info", "current_affect_only_group_members", spell->affect_only_group_members);
		packet->setSubstructDataByName("spell_info", "current_group_spell", spell->group_spell);
		packet->setSubstructDataByName("spell_info", "current_resistibility", spell->resistibility);
		packet->setSubstructDataByName("spell_info", "current_name", &(spell->name));
		packet->setSubstructDataByName("spell_info", "current_description", &(spell->description));

	}

	if (current_tier + 1 <= data->maxRank) {
		packet->setSubstructDataByName("spell_info", "next_id", spell2->id);
		packet->setSubstructDataByName("spell_info", "next_icon", spell2->icon);
		packet->setSubstructDataByName("spell_info", "next_icon2", spell2->icon_heroic_op);	// fix struct element name eventually
		packet->setSubstructDataByName("spell_info", "next_icontype", spell2->icon_backdrop);	// fix struct element name eventually

		if (packet->GetVersion() >= 63119) {
			packet->setSubstructDataByName("spell_info", "next_aa_spell_info2", "version", 0x04);
			packet->setSubstructDataByName("spell_info", "next_aa_spell_info2", "sub_version", 0x24);
		}
		else if (packet->GetVersion() >= 58617) {
			packet->setSubstructDataByName("spell_info", "next_version", 0x03);
			packet->setSubstructDataByName("spell_info", "next_sub_version", 0x131A);
		}
		else {
			packet->setSubstructDataByName("spell_info", "next_version", 0x00);
			packet->setSubstructDataByName("spell_info", "next_sub_version", 0xD9);
		}

		packet->setSubstructDataByName("spell_info", "next_type", spell2->type);
		packet->setSubstructDataByName("spell_info", "next_unknown_MJ1d", 1); //63119 test
		packet->setSubstructDataByName("spell_info", "next_class_skill", spell2->class_skill);
		packet->setSubstructDataByName("spell_info", "next_mastery_skill", spell2->mastery_skill);
		packet->setSubstructDataByName("spell_info", "next_duration_flag", spell2->duration_until_cancel);
		if (client && spell->type != 2) {
			sint8 spell_text_color = client->GetPlayer()->GetArrowColor(GetLevelRequired(client));
			if (spell_text_color != ARROW_COLOR_WHITE && spell_text_color != ARROW_COLOR_RED && spell_text_color != ARROW_COLOR_GRAY)
				spell_text_color = ARROW_COLOR_WHITE;
			spell_text_color -= 6;
			if (spell_text_color < 0)
				spell_text_color *= -1;
			packet->setSubstructDataByName("spell_info", "next_spell_text_color", spell_text_color);
		}
		else
			packet->setSubstructDataByName("spell_info", "next_spell_text_color", 3);
		if (spell->type != 2) {
			packet->setArrayLengthByName("num_levels", levels.size());
			for (int32 i = 0; i < levels.size(); i++) {
				packet->setArrayDataByName("spell_info_aa_adventure_class2", levels[i]->adventure_class, i);
				packet->setArrayDataByName("spell_info_aa_tradeskill_class2", levels[i]->tradeskill_class, i);
				packet->setArrayDataByName("spell_info_aa_spell_level2", levels[i]->spell_level, i);
			}
		}
		//packet->setSubstructDataByName("spell_info","unknown9", 20);
		hp_req = 0;
		power_req = 0;
		if (client) {
			hp_req = GetHPRequired(client->GetPlayer());
			power_req = GetPowerRequired(client->GetPlayer());

			// might need version checks around these?
			if (client->GetVersion() >= 1193)
			{
				int16 savagery_req = GetSavageryRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "next_savagery_req", savagery_req);
				packet->setSubstructDataByName("spell_info", "next_savagery_upkeep", spell->savagery_upkeep);
			}
			if (client->GetVersion() >= 57048)
			{
				int16 dissonance_req = GetDissonanceRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "next_dissonance_req", dissonance_req);
				packet->setSubstructDataByName("spell_info", "next_dissonance_upkeep", spell->dissonance_upkeep);
			}
		}
		packet->setSubstructDataByName("spell_info", "next_target", spell->target_type);
		packet->setSubstructDataByName("spell_info", "next_recovery", spell->recovery);
		packet->setSubstructDataByName("spell_info", "next_health_upkeep", spell->hp_upkeep);
		packet->setSubstructDataByName("spell_info", "next_health_req", hp_req);
		packet->setSubstructDataByName("spell_info", "next_tier", spell->tier);
		packet->setSubstructDataByName("spell_info", "next_power_req", power_req);
		packet->setSubstructDataByName("spell_info", "next_power_upkeep", spell->power_upkeep);

		packet->setSubstructDataByName("spell_info", "next_cast_time", spell->cast_time);
		packet->setSubstructDataByName("spell_info", "next_recast", spell->recast);
		packet->setSubstructDataByName("spell_info", "next_radius", spell->radius);
		packet->setSubstructDataByName("spell_info", "next_req_concentration", spell->req_concentration);
		//packet->setSubstructDataByName("spell_info","req_concentration2", 2);
		packet->setSubstructDataByName("spell_info", "next_max_aoe_targets", spell->max_aoe_targets);
		packet->setSubstructDataByName("spell_info", "next_friendly_spell", spell->friendly_spell);
		packet->setSubstructArrayLengthByName("spell_info", "next_num_effects", next_spell->effects.size());
		for (int32 i = 0; i < next_spell->effects.size(); i++) {
			packet->setArrayDataByName("next_subbulletflag", next_spell->effects[i]->subbullet, i);
			string effect_message;
			if (next_spell->effects[i]->description.length() > 0) {
				effect_message = next_spell->effects[i]->description;
				if (effect_message.find("%LM") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LM");
					int data_index = stoi(effect_message.substr(string_index + 3, 2));
					float value;
					if (next_spell->lua_data[data_index]->type == 1)
						value = next_spell->lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = next_spell->lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					string strValue = to_string(value);
					strValue.erase(strValue.find_last_not_of('0') + 1, std::string::npos);
					effect_message.replace(effect_message.find("%LM"), 5, strValue);
				}
				// Magic damage min
				if (effect_message.find("%DML") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%DML");
					int data_index = stoi(effect_message.substr(string_index + 4, 2));
					float value;
					if (next_spell->lua_data[data_index]->type == 1)
						value = next_spell->lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = next_spell->lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					damage.erase(damage.find_last_not_of('0') + 1, std::string::npos);
					effect_message.replace(effect_message.find("%DML"), 6, damage);
				}
					// Magic damage max
					if (effect_message.find("%DMH") < 0xFFFFFFFF) {
						int string_index = effect_message.find("%DMH");
						int data_index = stoi(effect_message.substr(string_index + 4, 2));
						float value;
						if (next_spell->lua_data[data_index]->type == 1)
							value = next_spell->lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
						else
							value = next_spell->lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
						value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
						int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
						value += mod;
						string damage = to_string((int)round(value));
						damage.erase(damage.find_last_not_of('0') + 1, std::string::npos);
						effect_message.replace(effect_message.find("%DMH"), 6, damage);
					}
					// level based Magic damage min
					if (effect_message.find("%LDML") < 0xFFFFFFFF) {
						int string_index = effect_message.find("%LDML");
						int data_index = stoi(effect_message.substr(string_index + 5, 2));
						float value;
						if (next_spell->lua_data[data_index]->type == 1)
							value = next_spell->lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
						else
							value = next_spell->lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
						value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
						int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
						value += mod;
						string damage = to_string((int)round(value));
						effect_message.replace(effect_message.find("%LDML"), 7, damage);
					}
					// level based Magic damage max
					if (effect_message.find("%LDMH") < 0xFFFFFFFF) {
						int string_index = effect_message.find("%LDMH");
						int data_index = stoi(effect_message.substr(string_index + 5, 2));
						float value;
						if (next_spell->lua_data[data_index]->type == 1)
							value = next_spell->lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
						else
							value = next_spell->lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
						value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
						int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
						value += mod;
						string damage = to_string((int)round(value));
						effect_message.replace(effect_message.find("%LDMH"), 7, damage);
					}
					//GetZone()->SimpleMessage(CHANNEL_COLOR_SPELL_EFFECT, effect_message.c_str(), victim, 50);
					packet->setArrayDataByName("next_effect", effect_message.c_str(), i);
				}
					   			 		
			packet->setArrayDataByName("next_percentage", next_spell->effects[i]->percentage, i);

		}
		if (display_tier == true)
			packet->setSubstructDataByName("spell_info", "next_display_spell_tier", 1);// spell->display_spell_tier);
		else
			packet->setSubstructDataByName("spell_info", "next_display_spell_tier", 1);//0
		packet->setSubstructDataByName("spell_info", "next_unknown_1", 1);//0
		packet->setSubstructDataByName("spell_info", "next_range", spell2->range);
		packet->setSubstructDataByName("spell_info", "next_duration_1", spell2->duration1);
		packet->setSubstructDataByName("spell_info", "next_duration_2", spell2->duration2);

		packet->setSubstructDataByName("spell_info", "next_can_effect_raid", spell2->can_effect_raid);
		packet->setSubstructDataByName("spell_info", "next_affect_only_group_members", spell2->affect_only_group_members);
		packet->setSubstructDataByName("spell_info", "next_group_spell", spell2->group_spell);
		packet->setSubstructDataByName("spell_info", "next_resistibility", spell2->resistibility);
		packet->setSubstructDataByName("spell_info", "next_name", &(spell2->name));
		packet->setSubstructDataByName("spell_info", "next_description", &(spell2->description));
	
	}
}
	void Spell::SetPacketInformation(PacketStruct* packet, Client* client, bool display_tier) {
		packet->setSubstructDataByName("spell_info", "id", spell->id);
		packet->setSubstructDataByName("spell_info", "icon", spell->icon);
		packet->setSubstructDataByName("spell_info", "icon2", spell->icon_heroic_op);	// fix struct element name eventually
		packet->setSubstructDataByName("spell_info", "icontype", spell->icon_backdrop);	// fix struct element name eventually

		if (packet->GetVersion() >= 63119) {
			packet->setSubstructDataByName("spell_info", "version", 0x04);
			packet->setSubstructDataByName("spell_info", "sub_version", 0x24);
		}
		else if (packet->GetVersion() >= 60114) {
			packet->setSubstructDataByName("spell_info", "version", 0x03);
			packet->setSubstructDataByName("spell_info", "sub_version", 4890);
		}
		else {
			packet->setSubstructDataByName("spell_info", "version", 0x11);
			packet->setSubstructDataByName("spell_info", "sub_version", 0x14);
		}

		packet->setSubstructDataByName("spell_info", "type", spell->type);
		packet->setSubstructDataByName("spell_info", "unknown_MJ1d", 1); //63119 test
		packet->setSubstructDataByName("spell_info", "class_skill", spell->class_skill);
		packet->setSubstructDataByName("spell_info", "mastery_skill", spell->mastery_skill);
		packet->setSubstructDataByName("spell_info", "duration_flag", spell->duration_until_cancel);
		if (client && spell->type != 2) {
			sint8 spell_text_color = client->GetPlayer()->GetArrowColor(GetLevelRequired(client));
			if (spell_text_color != ARROW_COLOR_WHITE && spell_text_color != ARROW_COLOR_RED && spell_text_color != ARROW_COLOR_GRAY)
				spell_text_color = ARROW_COLOR_WHITE;
			spell_text_color -= 6;
			if (spell_text_color < 0)
				spell_text_color *= -1;
			packet->setSubstructDataByName("spell_info", "spell_text_color", spell_text_color);
		}
		else
			packet->setSubstructDataByName("spell_info", "spell_text_color", 3);
		if (spell->type != 2) {
			packet->setSubstructArrayLengthByName("spell_info", "num_levels", levels.size());
			for (int32 i = 0; i < levels.size(); i++) {
				packet->setArrayDataByName("adventure_class", levels[i]->adventure_class, i);
				packet->setArrayDataByName("tradeskill_class", levels[i]->tradeskill_class, i);
				packet->setArrayDataByName("spell_level", levels[i]->spell_level, i);
			}
		}
		packet->setSubstructDataByName("spell_info", "unknown9", 20);
		int16 hp_req = 0;
		int16 power_req = 0;
		if (client) {
			hp_req = GetHPRequired(client->GetPlayer());
			power_req = GetPowerRequired(client->GetPlayer());

			// might need version checks around these?
			if (client->GetVersion() >= 1193)
			{
				int16 savagery_req = GetSavageryRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "savagery_req", savagery_req);
				packet->setSubstructDataByName("spell_info", "savagery_upkeep", spell->savagery_upkeep);
			}
			if (client->GetVersion() >= 57048)
			{
				int16 dissonance_req = GetDissonanceRequired(client->GetPlayer()); // dunno why we need to do this
				packet->setSubstructDataByName("spell_info", "dissonance_req", dissonance_req);
				packet->setSubstructDataByName("spell_info", "dissonance_upkeep", spell->dissonance_upkeep);
			}
		}
		packet->setSubstructDataByName("spell_info", "target", spell->target_type);
		packet->setSubstructDataByName("spell_info", "recovery", spell->recovery);
		packet->setSubstructDataByName("spell_info", "health_upkeep", spell->hp_upkeep);
		packet->setSubstructDataByName("spell_info", "health_req", hp_req);
		packet->setSubstructDataByName("spell_info", "tier", spell->tier);
		packet->setSubstructDataByName("spell_info", "power_req", power_req);
		packet->setSubstructDataByName("spell_info", "power_upkeep", spell->power_upkeep);

		packet->setSubstructDataByName("spell_info", "cast_time", spell->cast_time);
		packet->setSubstructDataByName("spell_info", "recast", spell->recast);
		packet->setSubstructDataByName("spell_info", "radius", spell->radius);
		packet->setSubstructDataByName("spell_info", "req_concentration", spell->req_concentration);
		//packet->setSubstructDataByName("spell_info","req_concentration2", 2);
		packet->setSubstructDataByName("spell_info", "max_aoe_targets", spell->max_aoe_targets);
		packet->setSubstructDataByName("spell_info", "friendly_spell", spell->friendly_spell);
		packet->setSubstructArrayLengthByName("spell_info", "num_effects", effects.size());
		for (int32 i = 0; i < effects.size(); i++) {
			
			packet->setArrayDataByName("subbulletflag", effects[i]->subbullet, i);
			string effect_message;
			if (effects[i]->description.length() > 0) {
				effect_message = effects[i]->description;
				if (effect_message.find("%LM") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LM");
					int data_index = stoi(effect_message.substr(string_index + 3, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					string strValue = to_string(value);
					strValue.erase(strValue.find_last_not_of('0') + 1, std::string::npos);
					effect_message.replace(effect_message.find("%LM"), 5, strValue);
				}
				// Magic damage min
				if (effect_message.find("%DML") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%DML");
					int data_index = stoi(effect_message.substr(string_index + 4, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%DML"), 6, damage);
				}
				// Magic damage max
				if (effect_message.find("%DMH") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%DMH");
					int data_index = stoi(effect_message.substr(string_index + 4, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%DMH"), 6, damage);
				}
				// level based Magic damage min
				if (effect_message.find("%LDML") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LDML");
					int data_index = stoi(effect_message.substr(string_index + 5, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%LDML"), 7, damage);
				}
				// level based Magic damage max
				if (effect_message.find("%LDMH") < 0xFFFFFFFF) {
					int string_index = effect_message.find("%LDMH");
					int data_index = stoi(effect_message.substr(string_index + 5, 2));
					float value;
					if (lua_data[data_index]->type == 1)
						value = lua_data[data_index]->float_value * client->GetPlayer()->GetLevel();
					else
						value = lua_data[data_index]->int_value * client->GetPlayer()->GetLevel();
					value *= ((client->GetPlayer()->GetInfoStruct()->potency / 100) + 1);
					int32 mod = (int32)min(client->GetPlayer()->GetInfoStruct()->ability_modifier, (float)(value / 2));
					value += mod;
					string damage = to_string((int)round(value));
					effect_message.replace(effect_message.find("%LDMH"), 7, damage);
				}
				//GetZone()->SimpleMessage(CHANNEL_COLOR_SPELL_EFFECT, effect_message.c_str(), victim, 50);
			}
			packet->setArrayDataByName("effect", effect_message.c_str(), i);
			packet->setArrayDataByName("percentage", effects[i]->percentage, i);
		}
		if (display_tier == true)
			packet->setSubstructDataByName("spell_info", "display_spell_tier", spell->display_spell_tier);
		else
			packet->setSubstructDataByName("spell_info", "display_spell_tier", 0);
		packet->setSubstructDataByName("spell_info", "range", spell->range);
		packet->setSubstructDataByName("spell_info", "duration1", spell->duration1);
		packet->setSubstructDataByName("spell_info", "duration2", spell->duration2);

		packet->setSubstructDataByName("spell_info", "can_effect_raid", spell->can_effect_raid);
		packet->setSubstructDataByName("spell_info", "affect_only_group_members", spell->affect_only_group_members);
		packet->setSubstructDataByName("spell_info", "group_spell", spell->group_spell);
		packet->setSubstructDataByName("spell_info", "resistibility", spell->resistibility);
		packet->setSubstructDataByName("spell_info", "name", &(spell->name));
		packet->setSubstructDataByName("spell_info", "description", &(spell->description));
		//packet->PrintPacket();
}
EQ2Packet* Spell::SerializeSpecialSpell(Client* client, bool display, int8 packet_type, int8 sub_packet_type){
	return SerializeSpell(client, display, false, packet_type, sub_packet_type, "WS_ExamineSpecialSpellInfo");
}

EQ2Packet* Spell::SerializeAASpell(Client* client,int8 tier, AltAdvanceData* data, bool display, bool trait_display, int8 packet_type, int8 sub_packet_type, const char* struct_name){
	if (!client)
		return 0;
	int16 version = 1;
	if (client)
		version = client->GetVersion();
	if (!struct_name)
		struct_name = "WS_ExamineAASpellInfo";
	PacketStruct* packet = configReader.getStruct(struct_name, version);
	if (display)
		packet->setSubstructDataByName("info_header", "show_name", 1);//1
	else
		if (!trait_display)
			packet->setSubstructDataByName("info_header", "show_popup", 1);//1
		else
			packet->setSubstructDataByName("info_header", "show_popup", 0);

	if (packet_type > 0)
		packet->setSubstructDataByName("info_header", "packettype", packet_type * 256 + 0xFE);
	else
		packet->setSubstructDataByName("info_header", "packettype", 0x4FFE);// 0x45FE    GetItemPacketType(version));
	//packet->setDataByName("unknown2",5);
	//packet->setDataByName("unknown7", 1);
	//packet->setDataByName("unknown9", 20);
	//packet->setDataByName("unknown10", 1, 2);
	if (sub_packet_type == 0)
		sub_packet_type = 0x83;
	packet->setSubstructDataByName("info_header", "packetsubtype", 4);// sub_packet_type);
	packet->setSubstructDataByName("spell_info", "aa_id", data->spellID);
	packet->setSubstructDataByName("spell_info", "aa_tab_id", data->group);
	packet->setSubstructDataByName("spell_info", "aa_icon", data->icon);
	packet->setSubstructDataByName("spell_info", "aa_icon2", data->icon2);
	packet->setSubstructDataByName("spell_info", "aa_current_rank", tier); // how to get this info to here?
	packet->setSubstructDataByName("spell_info", "aa_max_rank", data->maxRank);
	packet->setSubstructDataByName("spell_info", "aa_rank_cost", data->rankCost);
	packet->setSubstructDataByName("spell_info", "aa_unknown_2", 20);
	packet->setSubstructDataByName("spell_info", "aa_name", &(spell->name));
	packet->setSubstructDataByName("spell_info", "aa_description", &(spell->description));




	//packet->setDataByName("unknown3",2);
	//packet->setDataByName("unknown7", 50);
	if (sub_packet_type == 0x81)
		SetAAPacketInformation(packet, data, client);
	else
		SetAAPacketInformation(packet,data, client, true);
	packet->setSubstructDataByName("spell_info", "uses_remaining", 0xFFFF);
	packet->setSubstructDataByName("spell_info", "damage_remaining", 0xFFFF);
	//packet->PrintPacket();
	// This adds the second portion to the spell packet. Could be used for bonuses etc.?
	string* data1 = packet->serializeString();
	uchar*  data2 = (uchar*)data1->c_str();
	uchar*  ptr2 = data2;
	int32 size = data1->length();// *2;
	////uchar* data3 = new uchar[size];
	////memcpy(data3, data2, data1->length());
	////uchar* ptr = data3;
	////size -= 17;
	////memcpy(ptr, &size, sizeof(int32));
	////size += 3;
	////ptr += data1->length();
	////ptr2 += 14;
	////memcpy(ptr, ptr2, data1->length() - 14);

	EQ2Packet* outapp = new EQ2Packet(OP_ClientCmdMsg, data2, size);
	//DumpPacket(outapp);
	//safe_delete_array(data3);
	safe_delete(packet);
	return outapp;
	/*PacketStruct* packet = configReader.getStruct("WS_ExamineAASpellInfo", client->GetVersion());
	packet->setSubstructDataByName("info_header", "show_name", 0);
	packet->setSubstructDataByName("info_header", "show_popup", 0);
	packet->setSubstructDataByName("info_header", "packettype", packet_type);
	packet->setSubstructDataByName("info_header", "packetsubtype", sub_packet_type);
	packet->setSubstructDataByName("Spell_info", "aa_id", data->spellID);
	packet->setSubstructDataByName("Spell_info", "aa_tab_id", data->group);
	packet->setSubstructDataByName("Spell_info", "aa_icon", data->icon);
	packet->setSubstructDataByName("Spell_info", "aa_icon2", data->icon2);
	packet->setSubstructDataByName("Spell_info", "current_rank", 0); // how to get this info to here?
	packet->setSubstructDataByName("Spell_info", "max_rank", data->maxRank);
	packet->setSubstructDataByName("Spell_info", "rank_cost", data->rankCost);
	packet->setSubstructDataByName("spell_info", "unknown2", 20);
	// Spell info
	packet->setSubstructDataByName("spell_info", "id", spell->id);
	packet->setSubstructDataByName("spell_info", "icon", spell->icon);
	packet->setSubstructDataByName("spell_info", "icon2", spell->icon_heroic_op);	// fix struct element name eventually
	packet->setSubstructDataByName("spell_info", "icontype", spell->icon_backdrop);	// fix struct element name eventually

	if (packet->GetVersion() >= 63119) {
		packet->setSubstructDataByName("spell_info", "version", 0x04);
		packet->setSubstructDataByName("spell_info", "sub_version", 0x24);
	}
	else if (packet->GetVersion() >= 1193) {
		packet->setSubstructDataByName("spell_info", "version", 0x00);
		packet->setSubstructDataByName("spell_info", "sub_version", 0xD9);
	}
	else {
		packet->setSubstructDataByName("spell_info", "version", 0x11);
		packet->setSubstructDataByName("spell_info", "sub_version", 0x14);
	}

	packet->setSubstructDataByName("spell_info", "type", spell->type);
	packet->setSubstructDataByName("spell_info", "unknown_MJ1d", 1); //63119 test
	packet->setSubstructDataByName("spell_info", "class_skill", spell->class_skill);
	packet->setSubstructDataByName("spell_info", "mastery_skill", spell->mastery_skill);
	packet->setSubstructDataByName("spell_info", "duration_flag", spell->duration_until_cancel);
	if (client && spell->type != 2) {
		sint8 spell_text_color = client->GetPlayer()->GetArrowColor(GetLevelRequired(client));
		if (spell_text_color != ARROW_COLOR_WHITE && spell_text_color != ARROW_COLOR_RED && spell_text_color != ARROW_COLOR_GRAY)
			spell_text_color = ARROW_COLOR_WHITE;
		spell_text_color -= 6;
		if (spell_text_color < 0)
			spell_text_color *= -1;
		packet->setSubstructDataByName("spell_info", "spell_text_color", spell_text_color);
	}
	else
		packet->setSubstructDataByName("spell_info", "spell_text_color", 3);
	if (spell->type != 2) {
		packet->setSubstructArrayLengthByName("spell_info", "num_levels", levels.size());
		for (int32 i = 0; i < levels.size(); i++) {
			packet->setArrayDataByName("adventure_class", levels[i]->adventure_class, i);
			packet->setArrayDataByName("tradeskill_class", levels[i]->tradeskill_class, i);
			packet->setArrayDataByName("spell_level", levels[i]->spell_level, i);
		}
	}
	//packet->setSubstructDataByName("spell_info", "unknown9", 20);
	int16 hp_req = 0;
	int16 power_req = 0;
	if (client) {
		hp_req = GetHPRequired(client->GetPlayer());
		power_req = GetPowerRequired(client->GetPlayer());

		// might need version checks around these?
		if (client->GetVersion() >= 1193)
		{
			int16 savagery_req = GetSavageryRequired(client->GetPlayer()); // dunno why we need to do this
			packet->setSubstructDataByName("spell_info", "savagery_req", savagery_req);
			packet->setSubstructDataByName("spell_info", "savagery_upkeep", spell->savagery_upkeep);
		}
		if (client->GetVersion() >= 57048)
		{
			int16 dissonance_req = GetDissonanceRequired(client->GetPlayer()); // dunno why we need to do this
			packet->setSubstructDataByName("spell_info", "dissonance_req", dissonance_req);
			packet->setSubstructDataByName("spell_info", "dissonance_upkeep", spell->dissonance_upkeep);
		}
	}
	packet->setSubstructDataByName("spell_info", "tier", spell->tier);
	packet->setSubstructDataByName("spell_info", "health_req", hp_req);
	packet->setSubstructDataByName("spell_info", "health_upkeep", spell->hp_upkeep);
	packet->setSubstructDataByName("spell_info", "power_req", power_req);
	packet->setSubstructDataByName("spell_info", "power_upkeep", spell->power_upkeep);
	packet->setSubstructDataByName("spell_info", "req_concentration", spell->req_concentration);
	//packet->setDataByName("req_concentration2", 2);
	packet->setSubstructDataByName("spell_info", "cast_time", spell->cast_time);
	packet->setSubstructDataByName("spell_info", "recovery", spell->recovery);
	packet->setSubstructDataByName("spell_info", "recast", spell->recast);
	packet->setSubstructDataByName("spell_info", "radius", spell->radius);
	packet->setSubstructDataByName("spell_info", "max_aoe_targets", spell->max_aoe_targets);
	packet->setSubstructDataByName("spell_info", "friendly_spell", spell->friendly_spell);
	//reageants??
	packet->setSubstructArrayLengthByName("spell_info", "num_effects", effects.size());
	for (int32 i = 0; i < effects.size(); i++) {
		packet->setArrayDataByName("subbulletflag", effects[i]->subbullet, i);
		packet->setArrayDataByName("effect", effects[i]->description.c_str(), i);
		packet->setArrayDataByName("percentage", effects[i]->percentage, i);		
	}
	//if (display_tier == true)
	packet->setSubstructDataByName("spell_info", "display_spell_tier", spell->display_spell_tier);
	//else
	//	packet->setSubstructDataByName("spell_info", "display_spell_tier", 0);

	// minimum range??
	packet->setSubstructDataByName("spell_info", "range", spell->range);
	packet->setSubstructDataByName("spell_info", "duration1", spell->duration1);
	packet->setSubstructDataByName("spell_info", "duration2", spell->duration2);
	//unknown9 ??
	//duration flag??
	packet->setSubstructDataByName("spell_info", "target", spell->target_type);
	packet->setSubstructDataByName("spell_info", "can_effect_raid", spell->can_effect_raid);
	packet->setSubstructDataByName("spell_info", "affect_only_group_members", spell->affect_only_group_members);
	packet->setSubstructDataByName("spell_info", "group_spell", spell->group_spell);
	packet->setSubstructDataByName("spell_info", "resistibility", spell->resistibility);
	//unknown11 ??
	//hit_bonus ??
	//unknown12 ??
	packet->setSubstructDataByName("spell_info", "name", &(spell->name));
	packet->setSubstructDataByName("spell_info", "description", &(spell->description));
	EQ2Packet* packetdata = packet->serialize();
	//EQ2Packet* app = new EQ2Packet(OP_AdventureList, packetdata->pBuffer, packetdata->size);
	EQ2Packet* app = new EQ2Packet(OP_ClientCmdMsg, packetdata->pBuffer, packetdata->size);
	packet->PrintPacket();
	//DumpPacket(app);
	safe_delete(packet);
	safe_delete(data);
	return app;
	*/
}

EQ2Packet* Spell::SerializeSpell(Client* client, bool display, bool trait_display, int8 packet_type, int8 sub_packet_type, const char* struct_name){
	int16 version = 1;
	if(client)
		version = client->GetVersion();
	if(!struct_name)
		struct_name = "WS_ExamineSpellInfo";
	PacketStruct* packet = configReader.getStruct(struct_name, version);
	if(display)
		packet->setSubstructDataByName("info_header", "show_name", 1);
	else
		if (!trait_display)
			packet->setSubstructDataByName("info_header", "show_popup", 1);
		else
			packet->setSubstructDataByName("info_header", "show_popup", 0);
	
	if(packet_type > 0)
		packet->setSubstructDataByName("info_header", "packettype", packet_type*256 + 0xFE);
	else
		packet->setSubstructDataByName("info_header", "packettype", GetItemPacketType(version));
	//packet->setDataByName("unknown2",5);
	//packet->setDataByName("unknown7", 1);
	//packet->setDataByName("unknown9", 20);
	//packet->setDataByName("unknown10", 1, 2);
	if(sub_packet_type == 0)
		sub_packet_type = 0x83;
	packet->setSubstructDataByName("info_header", "packetsubtype", sub_packet_type);
	//packet->setDataByName("unknown3",2);
	//packet->setDataByName("unknown7", 50);
	if(sub_packet_type == 0x81)
		SetPacketInformation(packet, client);
	else
		SetPacketInformation(packet, client, true);
	packet->setSubstructDataByName("spell_info", "uses_remaining", 0xFFFF);
	packet->setSubstructDataByName("spell_info", "damage_remaining", 0xFFFF);
	//packet->PrintPacket();
	// This adds the second portion to the spell packet. Could be used for bonuses etc.?
	int8 offset = 0;
	if (packet->GetVersion() == 60114) {
		offset = 28;
	}
	else {
		offset = 14;
	}
	string* data1 = packet->serializeString();
	uchar*  data2 = (uchar*)data1->c_str();
	uchar*  ptr2 = data2;
	int32 size = data1->length() * 2;
	uchar* data3 = new uchar[size];
	memcpy(data3, data2, data1->length());
	uchar* ptr = data3;
	size -=offset+3;
	memcpy(ptr, &size, sizeof(int32)); 
	size +=3;
	ptr += data1->length();
	ptr2 += offset;
	memcpy(ptr, ptr2, data1->length() - offset);

	EQ2Packet* outapp = new EQ2Packet(OP_ClientCmdMsg, data3, size);
	//DumpPacket(outapp);
	safe_delete_array(data3);
	safe_delete(packet);
	return outapp;
}

void Spell::AddSpellEffect(int8 percentage, int8 subbullet, string description){
	SpellDisplayEffect* effect = new SpellDisplayEffect;
	effect->description = description;
	effect->subbullet = subbullet;
	effect->percentage = percentage;
	MSpellInfo.lock();
	effects.push_back(effect);
	MSpellInfo.unlock();
}

int16 Spell::GetHPRequired(Spawn* spawn){
	int16 hp_req = spell->hp_req;
	if(spawn && spell->hp_req_percent > 0){
		double result = ((double)spell->hp_req_percent/100)*spawn->GetTotalHP();
		if(result >= (((int16)result) + .5))
			result++;
		hp_req = (int16)result;
	}
	return hp_req;
}

int16 Spell::GetPowerRequired(Spawn* spawn){
	int16 power_req;
	if (spell->power_by_level == true) {
		power_req =round( (spell->power_req) * spawn->GetLevel());
	}
	else {
		power_req = round(spell->power_req);
	}
	if(spawn && spell->power_req_percent > 0){
		double result = ((double)spell->power_req_percent/100)*spawn->GetTotalPower();
		if(result >= (((int16)result) + .5))
			result++;
		power_req = (int16)result;
	}
	return power_req;
}

int16 Spell::GetSavageryRequired(Spawn* spawn){
	int16 savagery_req = spell->savagery_req;
	if(spawn && spell->savagery_req_percent > 0){
		double result = ((double)spell->savagery_req_percent/100)*spawn->GetTotalSavagery();
		if(result >= (((int16)result) + .5))
			result++;
		savagery_req = (int16)result;
	}
	return savagery_req;
}

int16 Spell::GetDissonanceRequired(Spawn* spawn){
	int16 dissonance_req = spell->dissonance_req;
	if(spawn && spell->dissonance_req_percent > 0){
		double result = ((double)spell->dissonance_req_percent/100)*spawn->GetTotalDissonance();
		if(result >= (((int16)result) + .5))
			result++;
		dissonance_req = (int16)result;
	}
	return dissonance_req;
}

int32 Spell::GetSpellDuration(){
	if(spell->duration1 == spell->duration2)
		return spell->duration1;

	int32 difference = 0;
	int32 lower = 0;
	if(spell->duration2 > spell->duration1){
		difference = spell->duration2 - spell->duration1;
		lower = spell->duration1;
	}
	else{
		difference = spell->duration1 - spell->duration2;
		lower = spell->duration2;
	}
	int32 duration = (rand()%difference) + lower;
	return duration;
}

const char* Spell::GetName(){
	return spell->name.data.c_str();
}

const char* Spell::GetDescription(){
	return spell->description.data.c_str();
}

void Spell::AddSpellLevel(int8 adventure_class, int8 tradeskill_class, int16 level){
	LevelArray* lvl = new LevelArray;
	lvl->adventure_class = adventure_class;
	lvl->tradeskill_class = tradeskill_class;
	lvl->spell_level = level;
	MSpellInfo.lock();
	levels.push_back(lvl);
	MSpellInfo.unlock();
}

int32 Spell::GetSpellID(){
	if (spell)
		return spell->id;
	return 0;
}

int8 Spell::GetSpellTier(){
	if (spell)
		return spell->tier;
	return 0;
}

vector<LUAData*>* Spell::GetLUAData(){
	return &lua_data;
}
SpellData* Spell::GetSpellData(){
	return spell;
}

int16 Spell::GetSpellIcon(){
	if (spell)
		return spell->icon;
	return 0;
}

int16 Spell::GetSpellIconBackdrop(){
	if (spell)
		return spell->icon_backdrop;
	return 0;
}

int16 Spell::GetSpellIconHeroicOp(){
	if (spell)
		return spell->icon_heroic_op;
	return 0;
}

bool Spell::IsHealSpell(){
	return heal_spell;
}

bool Spell::IsBuffSpell(){
	return buff_spell;
}

bool Spell::IsDamageSpell(){
	return damage_spell;
}
bool Spell::IsControlSpell(){
	return control_spell;
}

bool Spell::IsOffenseSpell(){
	return offense_spell;
}

void Spell::ModifyCastTime(Entity* caster){
	int16 cast_time = spell->cast_time;
	float cast_speed = caster->GetInfoStruct()->casting_speed;
	if (cast_time > 0){
		if (cast_speed > 0) // casting speed can only reduce up to half a cast time
			spell->cast_time *= max((float) 0.5, (float) (1 / (1 + (cast_speed * .01))));
		else if (cast_speed < 0) // not sure if casting speed debuff is capped on live or not, capping at 1.5 * the normal rate for now
			spell->cast_time *= min((float) 1.5, (float) (1 + (1 - (1 / (1 + (cast_speed * -.01))))));
	}
}

vector <SpellDisplayEffect*>* Spell::GetSpellEffects(){
	MSpellInfo.lock();
	vector <SpellDisplayEffect*>* ret = &effects;
	MSpellInfo.unlock();
	return ret;
}

vector <LevelArray*>* Spell::GetSpellLevels(){
	MSpellInfo.lock();
	vector <LevelArray*>* ret = &levels;
	MSpellInfo.unlock();
	return ret;
}

bool Spell::ScribeAllowed(Player* player){
	bool ret = false;
	if(player){
		MSpellInfo.lock();
		for(int32 i=0;!ret && i<levels.size();i++){
			int16 mylevel = player->GetLevel();
			int16 spelllevels = levels[i]->spell_level;
			bool advlev = player->GetAdventureClass() == levels[i]->adventure_class;
			bool tslev = player->GetTradeskillClass() == levels[i]->tradeskill_class;
			bool levelmatch = player->GetLevel() >= levels[i]->spell_level;
			if((player->GetAdventureClass() == levels[i]->adventure_class || player->GetTradeskillClass() == levels[i]->tradeskill_class) && player->GetLevel() >= levels[i]->spell_level/10)
				ret = true;
		}
		MSpellInfo.unlock();
	}
	return ret;
}

MasterSpellList::MasterSpellList(){
	MMasterSpellList.SetName("MasterSpellList::MMasterSpellList");
}

MasterSpellList::~MasterSpellList(){
	DestroySpells();
}
void MasterSpellList::DestroySpells(){

	spell_errors.clear();

	MMasterSpellList.lock();
	map<int32, map<int32, Spell*> >::iterator iter;
	map<int32, Spell*>::iterator iter2;
	for(iter = spell_list.begin();iter != spell_list.end(); iter++){
		for(iter2 = iter->second.begin();iter2 != iter->second.end(); iter2++){
			safe_delete(iter2->second);
		}
	}
	spell_list.clear();
	MMasterSpellList.unlock();
}
void MasterSpellList::AddSpell(int32 id, int8 tier, Spell* spell){
	MMasterSpellList.lock();
	spell_list[id][tier] = spell;
	spell_name_map[spell->GetName()] = spell;
	spell_soecrc_map[spell->GetSpellData()->soe_spell_crc] = spell;
	MMasterSpellList.unlock();
}

Spell* MasterSpellList::GetSpell(int32 id, int8 tier){
	if (spell_list.count(id) > 0 && spell_list[id].count(tier) > 0)
		return spell_list[id][tier];
	return 0;
}

Spell* MasterSpellList::GetSpellByName(const char* name){
	if(spell_name_map.count(name) > 0)
		return spell_name_map[name];
	return 0;
}

Spell* MasterSpellList::GetSpellByCRC(int32 spell_crc){
	if(spell_soecrc_map.count(spell_crc) > 0)
		return spell_soecrc_map[spell_crc];
	return 0;
}

EQ2Packet* MasterSpellList::GetSpellPacket(int32 id, int8 tier, Client* client, bool display, int8 packet_type){
	Spell* spell = GetSpell(id, tier);
	if(spell)
		return spell->SerializeSpell(client, display, packet_type);
	return 0;
}
EQ2Packet* MasterSpellList::GetAASpellPacket(int32 id, int8 tier, Client* client, bool display, int8 packet_type) {
	Spell* spell = GetSpell(id, (tier == 0 ? 1 : tier));
	//Spell* spell2= GetSpell(id, (tier +1));
	AltAdvanceData* data = master_aa_list.GetAltAdvancement(id);
	if (spell)
		return spell->SerializeAASpell(client,tier, data, display,false, packet_type);
	return 0;
}

EQ2Packet* MasterSpellList::GetSpecialSpellPacket(int32 id, int8 tier, Client* client, bool display, int8 packet_type){
	Spell* spell = GetSpell(id, tier);
	if(spell)
		return spell->SerializeSpecialSpell(client, display, packet_type, 0x81);
	return 0;
}

vector<Spell*>* MasterSpellList::GetSpellListByAdventureClass(int8 class_id, int16 max_level, int8 max_tier){
	vector<Spell*>* ret = new vector<Spell*>;
	Spell* spell = 0;
	vector<LevelArray*>* levels = 0;
	LevelArray* level = 0;
	vector<LevelArray*>::iterator level_itr;
	MMasterSpellList.lock();
	map<int32, map<int32, Spell*> >::iterator iter;
	map<int32, Spell*>::iterator iter2;
	max_level *= 10; //convert to client level format, which is 10 times higher
	for(iter = spell_list.begin();iter != spell_list.end(); iter++){
		for(iter2 = iter->second.begin();iter2 != iter->second.end(); iter2++){
			spell = iter2->second;
			if(iter2->first <= max_tier && spell){
				levels = spell->GetSpellLevels();
				for(level_itr = levels->begin(); level_itr != levels->end(); level_itr++){
					level = *level_itr;
					if(level->spell_level <= max_level && level->adventure_class == class_id){
						ret->push_back(spell);
						break;
					}
				}
			}
		}
	}
	MMasterSpellList.unlock();
	return ret;
}

vector<Spell*>* MasterSpellList::GetSpellListByTradeskillClass(int8 class_id, int16 max_level, int8 max_tier){
	vector<Spell*>* ret = new vector<Spell*>;
	Spell* spell = 0;
	vector<LevelArray*>* levels = 0;
	LevelArray* level = 0;
	vector<LevelArray*>::iterator level_itr;
	MMasterSpellList.lock();
	map<int32, map<int32, Spell*> >::iterator iter;
	map<int32, Spell*>::iterator iter2;
	for(iter = spell_list.begin();iter != spell_list.end(); iter++){
		for(iter2 = iter->second.begin();iter2 != iter->second.end(); iter2++){
			spell = iter2->second;
			if(iter2->first <= max_tier && spell){
				levels = spell->GetSpellLevels();
				for(level_itr = levels->begin(); level_itr != levels->end(); level_itr++){
					level = *level_itr;
					if(level->spell_level <= max_level && level->tradeskill_class == class_id){
						ret->push_back(spell);
						break;
					}
				}
			}
		}
	}
	MMasterSpellList.unlock();
	return ret;
}

void MasterSpellList::Reload(){
	master_trait_list.DestroyTraits();
	DestroySpells();
	database.LoadSpells();
	database.LoadSpellErrors();
	database.LoadTraits();
}

int16 MasterSpellList::GetSpellErrorValue(int16 version, int8 error_index) {
	version = GetClosestVersion(version);

	if (spell_errors[version].count(error_index) == 0) {
		LogWrite(SPELL__ERROR, 0, "Spells", "No spell error entry. (version = %i, error_index = %i)", version, error_index);
		// 1 will give the client a pop up message of "Cannot cast" and a chat message of "[BUG] Cannot cast. Unknown failure casting spell."
		return 1;
	}
	return spell_errors[version][error_index];
}

void MasterSpellList::AddSpellError(int16 version, int8 error_index, int16 error_value) {
	if (spell_errors[version].count(error_index) == 0)
		spell_errors[version][error_index] = error_value;
}

int16 MasterSpellList::GetClosestVersion(int16 version) {
	int16 ret = 0;
	map<int16, map<int8, int16> >::iterator itr;
	// Get the closest version in the list that is less then or equal to the given version
	for (itr = spell_errors.begin(); itr != spell_errors.end(); itr++) {
		if (itr->first <= version) {
			if (itr->first > ret)
				ret = itr->first;
		}
	}

	return ret;
}

bool Spell::CastWhileStunned(){
	return (spell->casting_flags & CASTING_FLAG_STUNNED) == CASTING_FLAG_STUNNED;
}

bool Spell::CastWhileMezzed(){
	return (spell->casting_flags & CASTING_FLAG_MEZZED) == CASTING_FLAG_MEZZED;
}

bool Spell::CastWhileStifled(){
	return (spell->casting_flags & CASTING_FLAG_STIFLED) == CASTING_FLAG_STIFLED;
}

bool Spell::CastWhileFeared(){
	return (spell->casting_flags & CASTING_FLAG_FEARED) == CASTING_FLAG_FEARED;
}
