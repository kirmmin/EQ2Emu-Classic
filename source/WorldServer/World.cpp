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
#include <assert.h>
#include "World.h"
#include "Items/Items.h"
#include "Items/Items_ToV.h"
#include "Items/Items_DoV.h"
#include "Spells.h"
#include "client.h"
#include "WorldDatabase.h"
#include "../common/debug.h"
#include "races.h"
#include "classes.h"
#include "VisualStates.h"
#include "Appearances.h"
#include "Skills.h"
#include "LoginServer.h"
#include "Quests.h"
#include "Factions.h"
#include "Guilds/Guild.h"
#include "Collections/Collections.h"
#include "Achievements/Achievements.h"
#include "Recipes/Recipe.h"
#include "Rules/Rules.h"
#include "IRC/IRC.h"
#include "../common/Log.h"
#include "Traits/Traits.h"
#include "Chat/Chat.h"
#include "Tradeskills/Tradeskills.h"
#include "AltAdvancement/AltAdvancement.h"
#include "LuaInterface.h"
#include "HeroicOp/HeroicOp.h"
#include "RaceTypes/RaceTypes.h"

MasterQuestList master_quest_list;
MasterItemList master_item_list;
MasterSpellList master_spell_list;
MasterTraitList master_trait_list;
MasterHeroicOPList master_ho_list;
MasterSkillList master_skill_list;
MasterFactionList master_faction_list;
MasterCollectionList master_collection_list;
MasterAchievementList master_achievement_list;
MasterRecipeList master_recipe_list;
MasterRecipeBookList master_recipebook_list;
MasterTradeskillEventsList master_tradeskillevent_list;
MasterAAList master_aa_list;
MasterRaceTypeList race_types_list;
MasterAANodeList master_tree_nodes;
ClientList	client_list;
ZoneList	zone_list;
ZoneAuth	zone_auth;
int32 Spawn::next_id = 1;
int32 WorldDatabase::next_id = 0;
Commands commands;
Variables variables;
VisualStates visual_states;
Appearances master_appearance_list;
Classes classes;
Races races;
map<int16,OpcodeManager*>EQOpcodeManager;
map<int16, int16> EQOpcodeVersions;
WorldDatabase database;
GuildList guild_list;
IRC irc;
Chat chat;

extern ConfigReader configReader;
extern LoginServer loginserver;
extern World world;
extern RuleManager rule_manager;

World::World() : save_time_timer(300000), time_tick_timer(3000), vitality_timer(3600000), player_stats_timer(60000), server_stats_timer(60000), /*remove_grouped_player(30000),*/ guilds_timer(60000), lotto_players_timer(500) {
	save_time_timer.Start();
	time_tick_timer.Start();
	vitality_timer.Start();
	player_stats_timer.Start();
	server_stats_timer.Start();
	//remove_grouped_player.Start();
	guilds_timer.Start();
	lotto_players_timer.Start();
	xp_rate = -1;
	ts_xp_rate = -1;
	vitality_frequency = 0xFFFFFFFF;
	vitality_amount = -1;
	last_checked_time = 0;
	items_loaded = false;
	spells_loaded = false;
	achievments_loaded = false;
	merchant_inventory_items.clear();
	MHouseZones.SetName("World::m_houseZones");
	MPlayerHouses.SetName("World::m_playerHouses");
}

World::~World(){
	// At this point the log system is already shut down so no calls to LogWrite are allowed in any of the functions called by this deconstructor
	DeleteSpawns();
	if(database.GetStatus() == database.Connected)
		WriteServerStatistics();
	RemoveServerStatistics();
	DeleteMerchantsInfo();
	MutexMap<int32, LottoPlayer*>::iterator itr = lotto_players.begin();
	while (itr.Next())
		safe_delete(itr->second);
	map<int32, HouseZone*>::iterator itr2;
	for (itr2 = m_houseZones.begin(); itr2 != m_houseZones.end(); itr2++)
		safe_delete(itr2->second);
	m_houseZones.clear();

	tov_itemstat_conversion.clear();
}

void World::init(){
	WorldDatabase::next_id = database.GetMaxHotBarID();

	LogWrite(COMMAND__DEBUG, 1, "Command", "-Loading Commands...");
	database.LoadCommandList();
	LogWrite(COMMAND__DEBUG, 1, "Command", "-Load Commands complete!");

	LogWrite(FACTION__DEBUG, 1, "Faction", "-Loading Factions...");
	database.LoadFactionList();
	LogWrite(FACTION__DEBUG, 1, "Faction", "-Load Factions complete!...");

	LogWrite(SKILL__DEBUG, 1, "Skill", "-Loading Skills...");
	database.LoadSkills();
	LogWrite(SKILL__DEBUG, 1, "Skill", "-Load Skills complete...");

	LogWrite(WORLD__DEBUG, 1, "World", "-Loading `variables`...");
	database.LoadGlobalVariables();
	LogWrite(WORLD__DEBUG, 1, "World", "-Load `variables` complete!");
	
	LogWrite(WORLD__DEBUG, 1, "World", "-Loading `appearances`...");
	database.LoadAppearanceMasterList();
	LogWrite(WORLD__DEBUG, 1, "World", "-Load `appearances` complete!");

	LogWrite(WORLD__DEBUG, 1, "World", "-Loading `visual_states`...");
	database.LoadVisualStates();
	LogWrite(WORLD__DEBUG, 1, "World", "-Load `visual states` complete!");

	LogWrite(WORLD__DEBUG, 1, "World", "-Setting system parameters...");
	Variable* var = variables.FindVariable("gametime");
	const char* time_string = 0;
	char default_time[] = "0/0/3800 8:30";

	if(var)
		time_string = var->GetValue();

	if(!time_string)
		time_string = default_time;
	int	year, month, day, hour, minute;
	sscanf (time_string, "%d/%d/%d %d:%d", &month, &day, &year, &hour, &minute);
	LogWrite(WORLD__DEBUG, 3, "World", "--Setting World Time to %s...", time_string);
	world_time.month = month;
	world_time.day = day;
	world_time.year = year;
	world_time.hour = hour;
	world_time.minute = minute;

	LogWrite(WORLD__DEBUG, 3, "World", "--Loading Vitality Information...");
	LoadVitalityInformation();

	LogWrite(WORLD__DEBUG, 3, "World", "--Loading Server Statistics...");
	database.LoadServerStatistics();

	LogWrite(WORLD__DEBUG, 3, "World", "--Setting Server Start Time...");
	UpdateServerStatistic(STAT_SERVER_START_TIME, Timer::GetUnixTimeStamp(), true);

	LogWrite(WORLD__DEBUG, 3, "World", "--Resetting Accepted Connections to 0...");
	UpdateServerStatistic(STAT_SERVER_ACCEPTED_CONNECTION, 0, true);

	LogWrite(WORLD__DEBUG, 3, "World", "--Resetting Active Zones to 0...");
	UpdateServerStatistic(STAT_SERVER_NUM_ACTIVE_ZONES, 0, true);

	// Clear all online players at server startup
	LogWrite(WORLD__DEBUG, 3, "World", "--Resetting characters online flags...");
	database.ToggleCharacterOnline();
	LogWrite(WORLD__DEBUG, 1, "World", "-Set system parameters complete!");

	LogWrite(RULESYS__DEBUG, 1, "Rules", "-Loading Rule Sets...");
	database.LoadRuleSets();
	LogWrite(RULESYS__DEBUG, 1, "Rules", "-Load Rule Sets complete!");

	LogWrite(CHAT__DEBUG, 1, "IRC", "-Starting IRC thread...");
	LoadItemBlueStats();
	//PopulateTOVStatMap();
	group_buff_updates.Start(rule_manager.GetGlobalRule(R_Client, GroupSpellsTimer)->GetInt32());
	irc.Start();
}





PacketStruct* World::GetWorldTime(int16 version){
	PacketStruct* packet = configReader.getStruct("WS_GameWorldTime", version);
	if(packet){
		packet->setDataByName("year", world_time.year);
		packet->setDataByName("month", world_time.month);
		packet->setDataByName("day", world_time.day);
		packet->setDataByName("hour", world_time.hour);
		packet->setDataByName("minute", world_time.minute);
		packet->setDataByName("unknown", 250);
		packet->setDataByName("unix_time", Timer::GetUnixTimeStamp());
		packet->setDataByName("unknown2", 1);
	}
	return packet;
}

float World::GetXPRate(){
	if(xp_rate >= 0)
		return xp_rate;

	xp_rate = rule_manager.GetGlobalRule(R_Player, XPMultiplier)->GetFloat();
	LogWrite(WORLD__DEBUG, 0, "World", "Setting Global XP Rate to: %.2f", xp_rate);
	return xp_rate;
}

float World::GetTSXPRate()
{
	if(ts_xp_rate >= 0)
		return ts_xp_rate;

	ts_xp_rate = rule_manager.GetGlobalRule(R_Player, TSXPMultiplier)->GetFloat();
	LogWrite(WORLD__DEBUG, 0, "World", "Setting Global Tradeskill XP Rate to: %.2f", ts_xp_rate);
	return ts_xp_rate;
}

void World::Process(){
	if(last_checked_time > Timer::GetCurrentTime2())
		return;
	last_checked_time = Timer::GetCurrentTime2() + 1000;
	if(save_time_timer.Check())
		database.SaveWorldTime(&world_time);
	if(time_tick_timer.Check())
		WorldTimeTick();
	if(vitality_timer.Check())
		UpdateVitality();
	if (player_stats_timer.Check())
		WritePlayerStatistics();
	if (server_stats_timer.Check())
		WriteServerStatistics();
	/*if(remove_grouped_player.Check())
		CheckRemoveGroupedPlayer();*/
	if (group_buff_updates.Check())
		GetGroupManager()->UpdateGroupBuffs();
	if (guilds_timer.Check())
		SaveGuilds();
	if (lotto_players_timer.Check())
		CheckLottoPlayers();
}

vector<Variable*>* World::GetClientVariables(){
	return variables.GetVariables("cl_");
}

void World::LoadVitalityInformation()
{
	int32 timestamp = Timer::GetUnixTimeStamp();
	int32 diff = 0;

	// fetch vitalitytimer value from `variables` table
	Variable* timer_var = variables.FindVariable("vitalitytimer");

	if(timer_var)
	{
		try
		{
			diff = timestamp - atoul(timer_var->GetValue());
			diff *= 1000; //convert seconds to milliseconds
		}
		catch(...)
		{
			LogWrite(WORLD__ERROR, 0, "World", "Error parsing vitalitytimer, value: %s", timer_var->GetValue());
		}
	}

	// Now using Rules System to set vitality parameters
	vitality_amount		= rule_manager.GetGlobalRule(R_Player, VitalityAmount)->GetFloat();
	vitality_frequency	= rule_manager.GetGlobalRule(R_Player, VitalityFrequency)->GetInt32();

	vitality_frequency *= 1000; //convert seconds to milliseconds

	if(diff >= vitality_frequency)
		UpdateVitality(); //update now
	else
		vitality_timer.SetTimer(vitality_frequency - diff);
}

void World::UpdateVitality()
{
	// push new vitalitytimer to `variables` table
	database.UpdateVitality(Timer::GetUnixTimeStamp(), vitality_amount);

	if(vitality_timer.GetDuration() != vitality_frequency)
		vitality_timer.SetTimer(vitality_frequency);

	zone_list.UpdateVitality(vitality_amount);
}


void ZoneList::UpdateVitality(float amount)
{
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);

	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++)
	{
		tmp = *zone_iter;
		if(tmp)
			tmp->UpdateVitality(amount);
	}

	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}


void World::WorldTimeTick(){
	world_time.minute++;
	//I know it looks complicated, but the nested ifs are to avoid checking all of them every 3 seconds
	if(world_time.minute >= 60){ // >= in case of bad time from db
		world_time.minute = 0;
		world_time.hour++;
		if(world_time.hour >= 24){
			world_time.hour = 0;
			world_time.day++;
			if(world_time.day>=30){
				world_time.day = 0;
				world_time.month++;
				if(world_time.month >= 12){
					world_time.month = 0;
					world_time.year++;
				}
			}
		}
	}
}


ZoneList::ZoneList() {
	MZoneList.SetName("ZoneList::MZoneList");
}

ZoneList::~ZoneList() {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* zs = 0;
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end();){
		zs = *zone_iter;
		zone_iter = zlist.erase(zone_iter);
		safe_delete(zs);
	}
}

void ZoneList::CheckFriendList(Client* client) {
	LogWrite(WORLD__DEBUG, 0, "World", "Sending FriendList...");
	MClientList.lock();
	map<string,Client*>::iterator itr;
	for(itr = client_map.begin(); itr != client_map.end(); itr++){
		if(itr->second != client && itr->second){
			if(itr->second->GetPlayer()->IsFriend(client->GetPlayer()->GetName())){
				itr->second->SendFriendList();
				itr->second->Message(CHANNEL_COLOR_CHAT_RELATIONSHIP, "Friend: %s has logged in.", client->GetPlayer()->GetName());
			}
		}
	}
	MClientList.unlock();
}

void ZoneList::CheckFriendZoned(Client* client){
	MClientList.lock();
	map<string,Client*>::iterator itr;
	for(itr = client_map.begin(); itr != client_map.end(); itr++){
		if(itr->second != client && itr->second){
			if(itr->second->GetPlayer()->IsFriend(client->GetPlayer()->GetName())){
				itr->second->SendFriendList();
			}
		}
	}
	MClientList.unlock();
}

bool ZoneList::HandleGlobalChatMessage(Client* from, char* to, int16 channel, const char* message, const char* channel_name){
	if (!from) {
		LogWrite(WORLD__ERROR, 0, "World", "HandleGlobalChatMessage() called with an invalid client");
		return false;
	}

	if(channel == CHANNEL_TELL){
		Client* find_client = zone_list.GetClientByCharName(to);
		if(!find_client || find_client->GetPlayer()->IsIgnored(from->GetPlayer()->GetName()))
			return false;
		else if(find_client == from)
		{
			from->Message(CHANNEL_COLOR_RED,"You must be very lonely...(ERROR: Cannot send tell to self)");
		}
		else
		{
			PacketStruct* packet = configReader.getStruct("WS_HearChat", from->GetVersion());
			if(packet){
				packet->setMediumStringByName("from", from->GetPlayer()->GetName());
				packet->setMediumStringByName("to", find_client->GetPlayer()->GetName());
				packet->setDataByName("channel", CHANNEL_TELL);
				packet->setDataByName("from_spawn_id", 0xFFFFFFFF);
				packet->setDataByName("to_spawn_id", 0xFFFFFFFF);
				packet->setDataByName("unknown2", 1, 1);
				packet->setDataByName("show_bubble", 1);
				packet->setDataByName("understood", 1);
				packet->setDataByName("time", 2); 
				packet->setMediumStringByName("message", message);
				if(channel_name)
					packet->setMediumStringByName("channel_name", channel_name);
				EQ2Packet* outpacket = packet->serialize();
				//DumpPacket(outpacket);
				find_client->QueuePacket(outpacket->Copy());
				from->QueuePacket(outpacket);
				safe_delete(packet);
			}
			if (find_client->GetPlayer()->get_character_flag(CF_AFK)) {
				PacketStruct* packet2 = configReader.getStruct("WS_HearChat", from->GetVersion());
				if (packet2) {
					packet2->setMediumStringByName("from", find_client->GetPlayer()->GetName());
					packet2->setMediumStringByName("to", from->GetPlayer()->GetName());
					packet2->setDataByName("channel", CHANNEL_TELL);
					packet2->setDataByName("from_spawn_id", 0xFFFFFFFF);
					packet2->setDataByName("to_spawn_id", 0xFFFFFFFF);
					packet2->setDataByName("unknown2", 1, 1);
					packet2->setDataByName("show_bubble", 1);
					packet2->setDataByName("understood", 1);
					packet2->setDataByName("time", 2);
					packet2->setMediumStringByName("message", find_client->GetPlayer()->GetAwayMessage().c_str());
					if (channel_name)
						packet2->setMediumStringByName("channel_name", channel_name);
					EQ2Packet* outpacket = packet2->serialize();
					from->QueuePacket(outpacket->Copy());
					find_client->QueuePacket(outpacket);
					safe_delete(packet2);
				}
			}
		}
	}
	else if(channel == CHANNEL_GROUP) {
		GroupMemberInfo* gmi = from->GetPlayer()->GetGroupMemberInfo();
		if(gmi)
			world.GetGroupManager()->GroupMessage(gmi->group_id, message);
	}
	else{
		list<ZoneServer*>::iterator zone_iter;
		ZoneServer* zs = 0;
		MZoneList.readlock(__FUNCTION__, __LINE__);
		for(zone_iter=zlist.begin(); zone_iter!=zlist.end();zone_iter++){
			zs = *zone_iter;
			if(zs)
				zs->HandleChatMessage(from->GetPlayer(), to, channel, message, 0, channel_name);
		}
		MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	}
	return true;
}

void ZoneList::LoadSpellProcess(){
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* zone = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_iter=zlist.begin(); zone_iter!=zlist.end();zone_iter++){
		zone = *zone_iter;
		if (zone)
			zone->LoadSpellProcess();
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::DeleteSpellProcess(){
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* zone = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_iter=zlist.begin(); zone_iter!=zlist.end();zone_iter++){
		zone = *zone_iter;
		if (zone)
			zone->DeleteSpellProcess();
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::HandleGlobalBroadcast(const char* message) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* zone = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_iter=zlist.begin(); zone_iter!=zlist.end();zone_iter++){
		zone = *zone_iter;
		if (zone)
			zone->HandleBroadcast(message);
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::HandleGlobalAnnouncement(const char* message) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* zone = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_iter=zlist.begin(); zone_iter!=zlist.end();zone_iter++){
		zone = *zone_iter;
		if (zone)
			zone->HandleAnnouncement(message);
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

int32 ZoneList::Count(){
	return zlist.size();
}

void ZoneList::Add(ZoneServer* zone) {
	MZoneList.writelock(__FUNCTION__, __LINE__);
	zlist.push_back(zone);
	MZoneList.releasewritelock(__FUNCTION__, __LINE__);
}
void ZoneList::Remove(ZoneServer* zone) {
	MZoneList.writelock(__FUNCTION__, __LINE__);
	zlist.remove(zone);
	MZoneList.releasewritelock(__FUNCTION__, __LINE__);
}
ZoneServer* ZoneList::Get(const char* zone_name, bool loadZone) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	ZoneServer* ret = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		tmp = *zone_iter;
		if (!tmp->isZoneShuttingDown() && !tmp->IsInstanceZone() && strlen(zone_name) == strlen(tmp->GetZoneName()) && 
			strncasecmp(tmp->GetZoneName(), zone_name, strlen(zone_name))==0){
			if(tmp->NumPlayers() < 30 || tmp->IsCityZone())
				ret = tmp;
			break;
		}
	}

	MZoneList.releasereadlock(__FUNCTION__, __LINE__);

	if(!ret )
	{
		if ( loadZone )
		{
			ret = new ZoneServer(zone_name);
			database.LoadZoneInfo(ret);
			ret->Init();
		}
	}
	return ret;
}

ZoneServer* ZoneList::Get(int32 id, bool loadZone) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	ZoneServer* ret = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		tmp = *zone_iter;
		if(!tmp->isZoneShuttingDown() && !tmp->IsInstanceZone() && tmp->GetZoneID() == id){
			if(tmp->NumPlayers() < 30 || tmp->IsCityZone())
				ret = tmp;
			break;
		}
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	if(ret)
		tmp = ret;
	else if (loadZone) {
		string* zonename = database.GetZoneName(id);
		if(zonename){
			tmp = new ZoneServer(zonename->c_str());
			database.LoadZoneInfo(tmp);
			tmp->Init();
			safe_delete(zonename);
		}
	}
	return tmp;
}


void ZoneList::SendZoneList(Client* client) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	int zonesListed = 0;
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		tmp = *zone_iter;
		if ( zonesListed > 20 )
		{
			client->Message(CHANNEL_COLOR_YELLOW,"Reached max zone list of 20.");
			break;
		}
		zonesListed++;
		client->Message(CHANNEL_COLOR_YELLOW,"Zone(ID): %s(%i), Instance ID: %i, Description: %s.",tmp->GetZoneName(),tmp->GetZoneID(),
			tmp->GetInstanceID(),tmp->GetZoneDescription());
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

ZoneServer* ZoneList::GetByInstanceID(int32 id, int32 zone_id) {
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	ZoneServer* ret = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	if ( id > 0 )
	{
		for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
			tmp = *zone_iter;
			if(tmp->GetInstanceID() == id){
					ret = tmp;
					break;
			}
		}
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	if(ret)
		tmp = ret;
	else if ( zone_id > 0 ){
		string* zonename = database.GetZoneName(zone_id);
		if(zonename){
			tmp = new ZoneServer(zonename->c_str());

			// the player is trying to preload an already existing instance but it isn't loaded
			if ( id > 0 )
				tmp->SetupInstance(id);

			database.LoadZoneInfo(tmp);
			tmp->Init();
			safe_delete(zonename);
		}
	}
	return tmp;
}

ZoneServer* ZoneList::GetByLowestPopulation(int32 zone_id) {
	ZoneServer* ret = 0;
	ZoneServer* zone = 0;
	int32 clients = 0;
	list<ZoneServer*>::iterator itr;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	if (zone_id) {
		for (itr = zlist.begin(); itr != zlist.end(); itr++) {
			zone = *itr;
			if (zone) {
				// check the zone id's
				if (zone->GetZoneID() == zone_id) {
					// check this zones client count
					if (clients == 0 || zone->GetClientCount() < clients) {
						ret = zone;
						clients = zone->GetClientCount();
					}
				}
			}
		}
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

bool ZoneList::ClientConnected(int32 account_id){
	bool ret = false;
	map<string, Client*>::iterator itr;
	MClientList.lock();
	for(itr=client_map.begin(); itr != client_map.end(); itr++){
		if(itr->second && itr->second->GetAccountID() == account_id && (itr->second->GetPlayer()->GetActivityStatus() & ACTIVITY_STATUS_LINKDEAD) == 0){
			ret = true;
			break;
		}
		else if(!itr->second){
			client_map.erase(itr);
			if(client_map.size() > 0){
				itr=client_map.begin();
				if(itr == client_map.end()){
					if(itr->second && itr->second->GetAccountID() == account_id && (itr->second->GetPlayer()->GetActivityStatus() & ACTIVITY_STATUS_LINKDEAD) == 0)
						ret = true;
					break;
				}
			}
			else
				break;
		}
	}
	MClientList.unlock();
	return ret;
}

void ZoneList::ReloadClientQuests(){
	list<ZoneServer*>::iterator zone_iter;
	ZoneServer* tmp = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		tmp = *zone_iter;
		if(tmp)
			tmp->ReloadClientQuests();
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::ProcessWhoQuery(vector<string>* queries, ZoneServer* zone, vector<Entity*>* players, bool isGM){
	Entity* player = 0;
	bool add_player = true;
	bool found_match = false;
	int8 lower = 0;
	int8 upper = 0;
	vector<Entity*> tmpPlayers;
	vector<Entity*>::iterator spawn_iter;
	if(!zone->isZoneShuttingDown()){
		tmpPlayers = zone->GetPlayers();
		for(spawn_iter = tmpPlayers.begin(); spawn_iter!=tmpPlayers.end(); spawn_iter++){
			player = *spawn_iter;
			add_player = true;
			Client* find_client = zone_list.GetClientByCharName(player->GetName());
			if (find_client == NULL) continue;
			int flags = find_client->GetPlayer()->GetInfoStruct()->flags;
			int flags2 = find_client->GetPlayer()->GetInfoStruct()->flags2;
			for(int32 i=0;add_player && queries && i<queries->size();i++){
				found_match = false;
				if(queries->at(i) == "ALL")
					continue;
				if(queries->at(i).length() > 3 && classes.GetClassID(queries->at(i).c_str()) > 0){
					if(player->GetAdventureClass() != classes.GetClassID(queries->at(i).c_str()))
						add_player = false;
					found_match = true;
				}
				else if(queries->at(i).length() > 2 && races.GetRaceID(queries->at(i).c_str()) > 0){
					if(player->GetRace() != races.GetRaceID(queries->at(i).c_str()))
						add_player = false;
					found_match = true;
				}
				if(!found_match && queries->at(i) == "GOOD"){
					if(player->GetDeity() != 1)
						add_player = false;
					found_match = true;
				}
				else if(!found_match && queries->at(i) == "EVIL"){
					if(player->GetDeity() == 1)
						add_player = false;
					found_match = true;
				}
				if((queries->at(i) == "GUIDE") && (find_client->GetAdminStatus() > 0) && ((find_client->GetAdminStatus() >> 4) < 5))
					found_match = true;
				else if((queries->at(i) == "GM") && ((find_client->GetAdminStatus() >> 4) > 4))
					found_match = true;
				else if((queries->at(i) == "LFG") && (flags & (1 << CF_LFG)))
					found_match = true;
				else if((queries->at(i) == "LFW") && (flags & (1 << CF_LFW)))
					found_match = true;
				else if((queries->at(i) == "ROLEPLAYING") && (flags & (1 << CF_ROLEPLAYING)))
					found_match = true;
				else if(strspn(queries->at(i).c_str(),"0123456789") == queries->at(i).length()){
					try{
						if(lower == 0)
							lower = atoi(queries->at(i).c_str());
						else
							upper = atoi(queries->at(i).c_str());
					}
					catch(...){}
					found_match = true;
				}
				if(!found_match){
					string name = string(player->GetName());
					name = ToUpper(name);
					if(name.find(queries->at(i)) == name.npos)
						add_player = false;
				}
			}
			if(lower > 0 && upper > 0){
				if(player->GetLevel() < lower || player->GetLevel() > upper)
					add_player = false;
			}
			else if(lower > 0){
				if(player->GetLevel() != lower)
					add_player = false;
			}
			if((flags2 & (1 << (CF_GM_HIDDEN - 32))) && !isGM) {
				add_player = false;
				found_match = true;
			}
			if(add_player)
				players->push_back(player);
		}
	}
}

void ZoneList::ProcessWhoQuery(const char* query, Client* client){
	list<ZoneServer*>::iterator zone_iter;
	vector<Entity*> players;
	vector<Entity*>::iterator spawn_iter;
	Entity* player = 0;
	//for now display all clients
	bool all = false;
	vector<string>* queries = 0;
	bool isGM = ((client->GetAdminStatus() >> 4) > 4);
	if(query){
		string query_string = string(query);
		query_string = ToUpper(query_string);
		queries = SplitString(query_string, ' ');
	}
	if(queries && queries->size() > 0 && queries->at(0) == "ALL")
		all = true;
	if(all){
		MZoneList.readlock(__FUNCTION__, __LINE__);
		for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
			ZoneServer* tmp = *zone_iter;
			ProcessWhoQuery(queries, tmp, &players, isGM);
		}
		MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	}
	else{
		ProcessWhoQuery(queries, client->GetCurrentZone(), &players, isGM);
	}
	
	PacketStruct* packet = configReader.getStruct("WS_WhoQueryReply", client->GetVersion());
	if(packet){
		packet->setDataByName("account_id", client->GetAccountID());
		packet->setDataByName("unknown", 0xFFFFFFFF);
		int8 num_characters = players.size();
		int8 max_who_results = 10;
		int8 max_who_results_status_override = 100;

		Variable* var = variables.FindVariable("max_who_results_status_override");
		if ( var ){
			max_who_results_status_override = atoi(var->GetValue());
		}

		// AdnaeDMorte
		if ( client->GetAdminStatus() >= max_who_results_status_override ){
			client->Message(CHANNEL_COLOR_RED, "** ADMIN-MODE ** ");
		}

		Variable* var1 = variables.FindVariable("max_who_results");
		if ( var1 ){
			max_who_results = atoi(var1->GetValue());
		}

		if(num_characters > max_who_results && client->GetAdminStatus() < max_who_results_status_override){
			num_characters = max_who_results;
			packet->setDataByName("response", 3);  //response 1 = error message, 3 == capped
		}
		else
			packet->setDataByName("response", 2);
		packet->setArrayLengthByName("num_characters", num_characters);
		packet->setDataByName("unknown10", 1);
		int i=0;
		for(spawn_iter = players.begin(); spawn_iter!=players.end(); spawn_iter++, i++){
			if(i == num_characters)
				break;
			player = *spawn_iter;
			Client* find_client = zone_list.GetClientByCharName(player->GetName());
			int flags = find_client->GetPlayer()->GetInfoStruct()->flags;
			int flags2 = find_client->GetPlayer()->GetInfoStruct()->flags2;
			packet->setArrayDataByName("char_name", player->GetName(), i);
			packet->setArrayDataByName("level", player->GetLevel(), i);
			packet->setArrayDataByName("admin_level", ((flags2 & (1 << (CF_HIDE_STATUS - 32))) && !isGM)?0:(find_client->GetAdminStatus() >> 4), i);
			packet->setArrayDataByName("class", player->GetAdventureClass(), i);
			packet->setArrayDataByName("unknown4", 0xFF, i); //probably tradeskill class
			packet->setArrayDataByName("flags", (((flags >> CF_ANONYMOUS) & 1) << 0 ) |
									(((flags >> CF_LFG) & 1) << 1 ) |
									(((flags >> CF_ANONYMOUS) & 1) << 2 ) |
									/*(((flags >> CF_HIDDEN) & 1) << 3 ) |*/
									(((flags >> CF_ROLEPLAYING) & 1) << 4 ) |
									(((flags >> CF_AFK) & 1) << 5 ) |
									(((flags >> CF_LFW) & 1) << 6 ) /*|
									(((flags >> CF_NOTA) & 1) << 7 )*/, i);
			packet->setArrayDataByName("race", player->GetRace(), i);
			if(player->GetZone() && player->GetZone()->GetZoneDescription())
				packet->setArrayDataByName("zone", player->GetZone()->GetZoneDescription(), i);
			if(player->appearance.sub_title) {
				int32 sub_title_length = strlen(player->appearance.sub_title);
				char tmp_title[255];
				int32 index = 0;
				int32 index_tmp = 0;
				while (index < sub_title_length) {
					if (player->appearance.sub_title[index] != '<' && player->appearance.sub_title[index] != '>') {
						memcpy(tmp_title + index_tmp, player->appearance.sub_title + index, 1);
						index_tmp++;
					}
					index++;
				}
				tmp_title[index_tmp] = '\0';
				packet->setArrayDataByName("guild", tmp_title, i);
			}
		}
		client->QueuePacket(packet->serialize());
		safe_delete(packet);
	}
}

bool ZoneList::DepopFinished(){
	list<ZoneServer*>::iterator zone_iter;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	bool finished_depop = true;
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		if(!(*zone_iter)->FinishedDepop())
			finished_depop = false;
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	return finished_depop;
}

void ZoneList::Depop(){
	list<ZoneServer*>::iterator zone_iter;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		(*zone_iter)->Depop();
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::Repop(){
	list<ZoneServer*>::iterator zone_iter;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for(zone_iter=zlist.begin(); zone_iter!=zlist.end(); zone_iter++){
		(*zone_iter)->Depop(false, true);
	}
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::ReloadSpawns() {
	MZoneList.readlock(__FUNCTION__, __LINE__);

	list<ZoneServer*>::iterator itr;
	for (itr = zlist.begin(); itr != zlist.end(); itr++)
		(*itr)->ReloadSpawns();

	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

bool World::ReportBug(string data, char* player_name, int32 account_id, const char* spawn_name, int32 spawn_id, int32 zone_id){
	//loginserver
	vector<string> list;
	int32 offset = 0;
	int32 old_offset = 0;
	while((offset = data.find(7, old_offset+1)) < 0xFFFFFFFF){
		if(old_offset > 0)
			list.push_back(data.substr(old_offset+1, offset-old_offset-1));
		else
			list.push_back(data.substr(old_offset, offset));
		old_offset = offset;
	}
	if(list.size() < 7){
		string output = "Invalid bug list:\n";
		for(int32 i=0;i<list.size();i++)
			output = output.append("\t").append(list[i]).append("\n");
		LogWrite(WORLD__ERROR, 0, "World", "%s", output.c_str());
		return false;
	}
	ServerPacket* outpack = new ServerPacket(ServerOP_BugReport, sizeof(BugReport));
	BugReport* report = (BugReport*)outpack->pBuffer;
	strncpy(report->category, list[0].c_str(), list[0].length() > 63 ? 63 : list[0].length());
	strncpy(report->subcategory, list[1].c_str(), list[1].length() > 63 ? 63 : list[1].length());
	strncpy(report->causes_crash, list[2].c_str(), list[2].length() > 63 ? 63 : list[2].length());
	strncpy(report->reproducible, list[3].c_str(), list[3].length() > 63 ? 63 : list[3].length());
	strncpy(report->summary, list[4].c_str(), list[4].length() > 127 ? 127 : list[4].length());
	strncpy(report->description, list[5].c_str(), list[5].length() > 1999 ? 1999 : list[5].length());
	strncpy(report->version, list[6].c_str(), list[6].length() > 31 ? 31 : list[6].length());
	strncpy(report->player, player_name, strlen(player_name) > 63 ? 63 : strlen(player_name));
	strncpy(report->spawn_name, spawn_name, strlen(spawn_name) > 63 ? 63 : strlen(spawn_name));
	report->spawn_id = spawn_id;
	report->account_id = account_id;
	report->zone_id = zone_id;
	loginserver.SendPacket(outpack);
	database.SaveBugReport(report->category, report->subcategory, report->causes_crash, report->reproducible, report->summary, report->description, report->version, report->player, account_id, spawn_name, spawn_id, zone_id);
	safe_delete(outpack);
	return true;
}

void ZoneList::WritePlayerStatistics() {
	list<ZoneServer*>::iterator zone_itr;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_itr = zlist.begin(); zone_itr != zlist.end(); zone_itr++)
		(*zone_itr)->WritePlayerStatistics();
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
}

void ZoneList::ShutDownZones(){
	LogWrite(WORLD__INFO, 0, "World", "Shutting down all zones, please wait...");
	list<ZoneServer*>::iterator zone_itr;
	int32 size = 0;
	MZoneList.readlock(__FUNCTION__, __LINE__);
	for (zone_itr = zlist.begin(); zone_itr != zlist.end(); zone_itr++){
		(*zone_itr)->Shutdown();
	}
	size = zlist.size();
	MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	while(size > 0){
		Sleep(10);
		MZoneList.readlock(__FUNCTION__, __LINE__);
		size = zlist.size();
		MZoneList.releasereadlock(__FUNCTION__, __LINE__);
	}
	LogWrite(WORLD__INFO, 0, "World", "Zone shutdown complete");
}

void ZoneList::ReloadMail() {
	map<string, Client*>::iterator itr;
	MClientList.writelock(__FUNCTION__, __LINE__);
	for (itr = client_map.begin(); itr != client_map.end(); itr++) {
		itr->second->GetPlayer()->DeleteMail();
		database.LoadPlayerMail(itr->second);
	}
	MClientList.releasewritelock(__FUNCTION__, __LINE__);
}

void World::AddSpawnScript(int32 id, const char* name){
	MSpawnScripts.lock();
	if(name)
		spawn_scripts[id] = string(name);
	MSpawnScripts.unlock();
}

void World::AddSpawnEntryScript(int32 id, const char* name){
	MSpawnScripts.lock();
	if(name)
		spawnentry_scripts[id] = string(name);
	MSpawnScripts.unlock();
}

void World::AddSpawnLocationScript(int32 id, const char* name){
	MSpawnScripts.lock();
	if(name)
		spawnlocation_scripts[id] = string(name);
	MSpawnScripts.unlock();
}

void World::AddZoneScript(int32 id, const char* name) {
	MZoneScripts.lock();
	if (name)
		zone_scripts[id] = string(name);
	MZoneScripts.unlock();
}

const char* World::GetSpawnScript(int32 id){
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Enter %s", __FUNCTION__);
	const char* ret = 0;
	MSpawnScripts.lock();
	if(spawn_scripts.count(id) > 0)
		ret = spawn_scripts[id].c_str();
	MSpawnScripts.unlock();
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Exit %s", __FUNCTION__);
	return ret;
}

const char* World::GetSpawnEntryScript(int32 id){
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Enter %s", __FUNCTION__);
	const char* ret = 0;
	MSpawnScripts.lock();
	if(spawnentry_scripts.count(id) > 0)
		ret = spawnentry_scripts[id].c_str();
	MSpawnScripts.unlock();
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Exit %s", __FUNCTION__);
	return ret;
}

const char* World::GetSpawnLocationScript(int32 id){
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Enter %s", __FUNCTION__);
	const char* ret = 0;
	MSpawnScripts.lock();
	if(spawnlocation_scripts.count(id) > 0)
		ret = spawnlocation_scripts[id].c_str();
	MSpawnScripts.unlock();
	LogWrite(SPAWN__TRACE, 1, "Spawn", "Exit %s", __FUNCTION__);
	return ret;
}

const char* World::GetZoneScript(int32 id) {
	const char* ret = 0;
	MZoneScripts.lock();
	if (zone_scripts.count(id) > 0)
		ret = zone_scripts[id].c_str();
	MZoneScripts.unlock();
	return ret;
}

void World::ResetSpawnScripts(){
	MSpawnScripts.lock();
	spawn_scripts.clear();
	spawnentry_scripts.clear();
	spawnlocation_scripts.clear();
	MSpawnScripts.unlock();
}

void World::ResetZoneScripts() {
	MZoneScripts.lock();
	zone_scripts.clear();
	MZoneScripts.unlock();
}



vector<MerchantItemInfo>* World::GetMerchantItemList(int32 merchant_id, int8 merchant_type, Player* player)
{
	vector<MerchantItemInfo>* ret = 0;
	MMerchantList.lock();

	if(merchant_info.count(merchant_id) > 0)
	{
		MerchantInfo* info = merchant_info[merchant_id];
		vector<MerchantItemInfo>::iterator itr;
		int32 inventory_id = 0;
		Item* item = 0;

		for(int i=info->inventory_ids.size()-1;i>=0;i--)
		{
			inventory_id = info->inventory_ids[i];

			if(merchant_inventory_items.count(inventory_id) > 0)
			{
				for(itr = merchant_inventory_items[inventory_id].begin(); itr != merchant_inventory_items[inventory_id].end(); itr++)
				{
					if(!ret)
						ret = new vector<MerchantItemInfo>;

					item = master_item_list.GetItem((*itr).item_id);

					// if NOT spell merchant, OR 
					// skill req is any skill, OR player has the skill, AND 
					// skill req2 is any skill, OR player has the skill2
					if(item && ( (merchant_type & MERCHANT_TYPE_SPELLS) == 0 || ( (item->generic_info.skill_req1 == 0xFFFFFFFF || player->GetSkills()->HasSkill(item->generic_info.skill_req1)) && (item->generic_info.skill_req2 == 0xFFFFFFFF || player->GetSkills()->HasSkill(item->generic_info.skill_req2)) ) ) )
						(*ret).push_back(*itr);
				}
			}
		}
	}
	MMerchantList.unlock();
	return ret;
}

vector<MerchantItemInfo>* World::GetMerchantList(int32 merchant_id){
	vector<MerchantItemInfo>* ret = 0;
	MMerchantList.lock();
	if(merchant_info.count(merchant_id) > 0){
		MerchantInfo* info = merchant_info[merchant_id];
		map<int32, int16>::iterator itr;
		int32 inventory_id = 0;
		for(int i=info->inventory_ids.size()-1;i>=0;i--){
			inventory_id = info->inventory_ids[i];
			if(merchant_inventory_items.count(inventory_id) > 0){
				ret = &merchant_inventory_items[inventory_id];
			}
		}
	}
	MMerchantList.unlock();
	return ret;
}

void World::AddMerchantItem(int32 inventory_id, MerchantItemInfo ItemInfo){
	MMerchantList.lock();
	merchant_inventory_items[inventory_id].push_back(ItemInfo);
	MMerchantList.unlock();
}

void World::DeleteMerchantItems(){
	MMerchantList.lock();
	merchant_inventory_items.clear();
	MMerchantList.unlock();
}

void World::RemoveMerchantItem(int32 inventory_id, int32 item_id){
	MMerchantList.lock();
	if(merchant_inventory_items.count(inventory_id) > 0) {
		vector<MerchantItemInfo>::iterator itr;
		for(itr = merchant_inventory_items[inventory_id].begin(); itr != merchant_inventory_items[inventory_id].end(); itr++){
			if ((*itr).item_id == item_id) {
				merchant_inventory_items[inventory_id].erase(itr);
				break;
			}
		}
	}
	MMerchantList.unlock();
}

int16 World::GetMerchantItemQuantity(int32 merchant_id, int32 item_id){
	int16 amount = 0;
	int32 inventory_id = GetInventoryID(merchant_id, item_id);
	if(inventory_id > 0){
		MMerchantList.lock();
		vector<MerchantItemInfo>::iterator itr;
		for(itr = merchant_inventory_items[inventory_id].begin(); itr != merchant_inventory_items[inventory_id].end(); itr++){
			if ((*itr).item_id == item_id)
				amount = (*itr).quantity;
		}
		MMerchantList.unlock();
	}
	return amount;
}

int32 World::GetInventoryID(int32 merchant_id, int32 item_id){
	int32 ret = 0;
	MMerchantList.lock();
	if(merchant_info.count(merchant_id) > 0){
		MerchantInfo* info = merchant_info[merchant_id];
		vector<MerchantItemInfo>::iterator itr;
		int32 inventory_id = 0;
		for(int i=info->inventory_ids.size()-1;i>=0;i--){
			inventory_id = info->inventory_ids[i];
			if(merchant_inventory_items.count(inventory_id) > 0){
				for(itr = merchant_inventory_items[inventory_id].begin(); itr != merchant_inventory_items[inventory_id].end(); itr++){
					if((*itr).item_id == item_id){
						ret = inventory_id;
						break;
					}
				}
				if(ret > 0)
					break;
			}
		}
	}
	MMerchantList.unlock();
	return ret;
}

void World::DecreaseMerchantQuantity(int32 merchant_id, int32 item_id, int16 amount){
	int16 total_left = GetMerchantItemQuantity(merchant_id, item_id);
	if(total_left > 0 && total_left < 0xFFFF){
		int32 inventory_id = GetInventoryID(merchant_id, item_id);
		if(inventory_id > 0){
			MMerchantList.lock();
			vector<MerchantItemInfo>::iterator itr;
			for(itr = merchant_inventory_items[inventory_id].begin(); itr != merchant_inventory_items[inventory_id].end(); itr++){
				if ((*itr).item_id == item_id) {
					if(total_left <= amount) {
						merchant_inventory_items[inventory_id].erase(itr);
						amount = 0;
						break;
					}
					else
						(*itr).quantity -= amount;
					amount = (*itr).quantity;
				}
			}

			MMerchantList.unlock();
		}
	}
}

MerchantInfo* World::GetMerchantInfo(int32 merchant_id){
	MerchantInfo* ret = 0;
	MMerchantList.lock();
	if(merchant_info.count(merchant_id) > 0)
		ret = merchant_info[merchant_id];
	MMerchantList.unlock();
	return ret;
}

void World::AddMerchantInfo(int32 merchant_id, MerchantInfo* info){
	MMerchantList.lock();
	if(merchant_info.count(merchant_id) > 0){
		safe_delete(merchant_info[merchant_id]);
	}
	merchant_info[merchant_id] = info;
	MMerchantList.unlock();
}

map<int32, MerchantInfo*>* World::GetMerchantInfo() {
	return &merchant_info;
}

void World::DeleteMerchantsInfo(){
	MMerchantList.lock();
	map<int32, MerchantInfo*>::iterator itr;
	for(itr = merchant_info.begin(); itr != merchant_info.end(); itr++){
		safe_delete(itr->second);
	}
	merchant_info.clear();
	MMerchantList.unlock();
}





void World::DeleteSpawns(){
	//reloading = true;
	//ClearLootTables();
	/*
	map<int32, NPC*>::iterator npc_list_iter;
	for(npc_list_iter=npc_list.begin();npc_list_iter!=npc_list.end();npc_list_iter++) {
		safe_delete(npc_list_iter->second);
	} 
	npc_list.clear();
	map<int32, Object*>::iterator object_list_iter;
	for(object_list_iter=object_list.begin();object_list_iter!=object_list.end();object_list_iter++) {
		safe_delete(object_list_iter->second);
	}
	object_list.clear();
	map<int32, GroundSpawn*>::iterator groundspawn_list_iter;
	for(groundspawn_list_iter=groundspawn_list.begin();groundspawn_list_iter!=groundspawn_list.end();groundspawn_list_iter++) {
		safe_delete(groundspawn_list_iter->second);
	}
	groundspawn_list.clear();
	map<int32, Widget*>::iterator widget_list_iter;
	for(widget_list_iter=widget_list.begin();widget_list_iter!=widget_list.end();widget_list_iter++) {
		safe_delete(widget_list_iter->second);
	}
	widget_list.clear();
	map<int32, Sign*>::iterator sign_list_iter;
	for(sign_list_iter=sign_list.begin();sign_list_iter!=sign_list.end();sign_list_iter++) {
		safe_delete(sign_list_iter->second);
	}
	sign_list.clear();*/
	map<int32, AppearanceData*>::iterator appearance_list_iter;
	for(appearance_list_iter=npc_appearance_list.begin();appearance_list_iter!=npc_appearance_list.end();appearance_list_iter++) {
		safe_delete(appearance_list_iter->second);
	}
	npc_appearance_list.clear();

	/*
	map<int32, vector<EntityCommand*>* >::iterator command_list_iter;
	for(command_list_iter=entity_command_list.begin();command_list_iter!=entity_command_list.end();command_list_iter++) {
		vector<EntityCommand*>* v = command_list_iter->second;
		if(v){
			for(int32 i=0;i<v->size();i++){
				safe_delete(v->at(i));
			}
			safe_delete(v);
		}
	}
	entity_command_list.clear();
	*/

	//DeleteGroundSpawnItems();
	//DeleteTransporters();
	//DeleteTransporterMaps();
}

void World::ReloadGuilds() {
	guild_list.GetGuilds()->clear(true);
	database.LoadGuilds();
}

int8 World::GetClassID(const char* name){
	return classes.GetClassID(name);
}

void World::WritePlayerStatistics() {
	zone_list.WritePlayerStatistics();
}

void World::WriteServerStatistics() {
	map<int32, Statistic*>::iterator itr;
	Statistic* stat = 0;
	for (itr = server_statistics.begin(); itr != server_statistics.end(); itr++) {
		stat = itr->second;
		if (stat->save_needed) {
			stat->save_needed = false;
			database.WriteServerStatistic(stat);
		}
	}
	database.WriteServerStatisticsNeededQueries();
}

void World::AddServerStatistic(int32 stat_id, sint32 stat_value, int32 stat_date) {
	if (server_statistics.count(stat_id) == 0) {
		Statistic* stat = new Statistic;
		stat->stat_id = stat_id;
		stat->stat_value = stat_value;
		stat->stat_date = stat_date;
		stat->save_needed = false;
		server_statistics[stat_id] = stat;
	}
}
	
void World::UpdateServerStatistic(int32 stat_id, sint32 stat_value, bool overwrite) {
	if (server_statistics.count(stat_id) == 0)
		AddServerStatistic(stat_id, stat_value, 0);
	Statistic* stat = server_statistics[stat_id];
	overwrite == true ? stat->stat_value = stat_value : stat->stat_value += stat_value;
	stat->stat_date = Timer::GetUnixTimeStamp();
	stat->save_needed = true;
}

sint32 World::GetServerStatisticValue(int32 stat_id) {
	if (server_statistics.count(stat_id) > 0)
		return server_statistics[stat_id]->stat_value;
	return 0;
}

void World::RemoveServerStatistics() {
	map<int32, Statistic*>::iterator stat_itr;
	for (stat_itr = server_statistics.begin(); stat_itr != server_statistics.end(); stat_itr++)
		safe_delete(stat_itr->second);
	server_statistics.clear();
}

void World::SendGroupQuests(PlayerGroup* group, Client* client){
	return;
	/*if(!group)
		return;
	GroupMemberInfo* info = 0;
	MGroups.readlock(__FUNCTION__, __LINE__);
	deque<GroupMemberInfo*>::iterator itr;
	for(itr = group->members.begin(); itr != group->members.end(); itr++){
		info = *itr;
		if(info->client){
			LogWrite(PLAYER__DEBUG, 0, "Player", "Send Quest Journal...");
			info->client->SendQuestJournal(false, client);
			client->SendQuestJournal(false, info->client);
		}
	}
	MGroups.releasereadlock(__FUNCTION__, __LINE__);*/
}

/*void World::CheckRemoveGroupedPlayer(){
	map<GroupMemberInfo*, int32>::iterator itr;
	GroupMemberInfo* found = 0;
	MGroups.readlock(__FUNCTION__, __LINE__);
	for(itr = group_removal_pending.begin(); itr != group_removal_pending.end(); itr++){
		if(itr->second < Timer::GetCurrentTime2()){
			found = itr->first;
			break;
		}
	}
	MGroups.releasereadlock(__FUNCTION__, __LINE__);
	if(found){
		if(!found->client || (found->client && found->client->IsConnected() == false))
			DeleteGroupMember(found);
		else{
			MGroups.writelock(__FUNCTION__, __LINE__);
			group_removal_pending.erase(found);
			MGroups.releasewritelock(__FUNCTION__, __LINE__);
		}
	}
}*/

void World::RejoinGroup(Client* client){
	/*map<GroupMemberInfo*, int32>::iterator itr;
	GroupMemberInfo* found = 0;
	string name = string(client->GetPlayer()->GetName());
	MGroups.readlock(__FUNCTION__, __LINE__);
	PlayerGroup* group = 0;
	for(int i = player_groups.size()-1;!found && i >= 0;i--){
		group = player_groups[i];
		for(int x=group->members.size()-1;x>=0; x--){
			if(group->members[x]->name == name){
				found = group->members[x];
				break;
			}
		}
	}
	MGroups.releasereadlock(__FUNCTION__, __LINE__);
	if(found){
		found->client = client;
		client->GetPlayer()->SetGroup(found->group);
		client->GetPlayer()->SetGroupMemberInfo(found);
	}*/
}


void World::AddBonuses(ItemStatsValues* values, int16 type, sint32 value, Entity* entity){
	if(values){
		switch(type){
			case ITEM_STAT_STR:{
				values->str += value;
				break;
			}
			case ITEM_STAT_STA:{
				values->sta += value;
				break;
			}
			case ITEM_STAT_AGI:{
				values->agi += value;
				break;
			}
			case ITEM_STAT_WIS:{
				values->wis += value;
				break;
			}
			case ITEM_STAT_INT:{
				values->int_ += value;
				break;
			}
			case ITEM_STAT_VS_SLASH:{
				values->vs_slash += value;
				break;
			}
			case ITEM_STAT_VS_CRUSH:{
				values->vs_crush += value;
				break;
			}
			case ITEM_STAT_VS_PIERCE:{
				values->vs_pierce += value;
				break;
			}
			case ITEM_STAT_VS_HEAT:{
				values->vs_heat += value;
				break;
			}
			case ITEM_STAT_VS_COLD:{
				values->vs_cold += value;
				break;
			}
			case ITEM_STAT_VS_MAGIC:{
				values->vs_magic += value;
				break;
			}
			case ITEM_STAT_VS_MENTAL:{
				values->vs_mental += value;
				break;
			}
			case ITEM_STAT_VS_DIVINE:{
				values->vs_divine += value;
				break;
			}
			case ITEM_STAT_VS_DISEASE:{
				values->vs_disease += value;
				break;
			}
			case ITEM_STAT_VS_POISON:{
				values->vs_poison += value;
				break;
			}
			case ITEM_STAT_HEALTH:{
				values->health += value;
				break;
			}
			case ITEM_STAT_POWER:{
				values->power += value;
				break;
			}
			case ITEM_STAT_CONCENTRATION:{
				values->concentration += value;
				break;
			}
			case ITEM_STAT_ABILITY_MODIFIER:{
				values->ability_modifier += value;
				break;
			}
			case ITEM_STAT_CRITICALMITIGATION:{
				values->criticalmitigation += value;
				break;
			}
			case ITEM_STAT_EXTRASHIELDBLOCKCHANCE:{
				values->extrashieldblockchance += value;
				break;
			}
			case ITEM_STAT_BENEFICIALCRITCHANCE:{
				values->beneficialcritchance += value;
				break;
			}
			case ITEM_STAT_CRITBONUS:{
				values->critbonus += value;
				break;
			}
			/*case ITEM_STAT_POTENCY:{
				values->potency += value;
				break;
			}*/
			case ITEM_STAT_HATEGAINMOD:{
				values->hategainmod += value;
				break;
			}
			case ITEM_STAT_ABILITYREUSESPEED:{
				values->abilityreusespeed += value;
				break;
			}
       		case ITEM_STAT_ABILITYCASTINGSPEED:{
				values->abilitycastingspeed += value;
				break;
			}
			case ITEM_STAT_ABILITYRECOVERYSPEED:{
				values->abilityrecoveryspeed += value;
				break;
			}
			case ITEM_STAT_SPELLREUSESPEED:{
				values->spellreusespeed += value;
				break;
			}
			case ITEM_STAT_SPELLMULTIATTACKCHANCE:{
				values->spellmultiattackchance += value;
				break;
			}
			case ITEM_STAT_DPS:{
				values->dps += value;
				break;
			}
			case ITEM_STAT_ATTACKSPEED:{
				values->attackspeed += value;
				break;
			}
		    case ITEM_STAT_MULTIATTACKCHANCE:{
				values->multiattackchance += value;
				break;
			}
			case ITEM_STAT_AEAUTOATTACKCHANCE:{
				values->aeautoattackchance += value;
				break;
			}
			case ITEM_STAT_STRIKETHROUGH:{
				values->strikethrough += value;
				break;
			}
			case ITEM_STAT_ACCURACY:{
				values->accuracy += value;
				break;
			}
        	/*case ITEM_STAT_OFFENSIVESPEED:{
				values->offensivespeed += value;
				break;
			}*/
			default: {
				if (entity)
					entity->stats[type] += value;
				break;
					 }
		}
	}
}

void World::CreateGuild(const char* guild_name, Client* leader, int32 group_id) {
	deque<GroupMemberInfo*>::iterator itr;
	GroupMemberInfo* gmi;
	Guild *guild;

	assert(guild_name);

	guild = new Guild();
	guild->SetName(guild_name);
	guild->SetFormedDate(Timer::GetUnixTimeStamp());
	database.LoadGuildDefaultRanks(guild);
	database.LoadGuildDefaultEventFilters(guild);
	database.SaveGuild(guild, true);
	database.SaveGuildEvents(guild);
	database.SaveGuildRanks(guild);
	database.SaveGuildEventFilters(guild);
	database.SaveGuildRecruiting(guild);
	guild_list.AddGuild(guild);
	if (leader && !leader->GetPlayer()->GetGuild())
		guild->AddNewGuildMember(leader, 0, GUILD_RANK_LEADER);
	database.SaveGuildMembers(guild);
	if (leader && group_id > 0) {
		GetGroupManager()->GroupLock(__FUNCTION__, __LINE__);

		deque<GroupMemberInfo*>* members = GetGroupManager()->GetGroupMembers(group_id);
		for (itr = members->begin(); itr != members->end(); itr++) {
			gmi = *itr;
			if (gmi->client && gmi->client != leader && !gmi->client->GetPlayer()->GetGuild())
				guild->InvitePlayer(gmi->client, leader->GetPlayer()->GetName());
		}

		GetGroupManager()->ReleaseGroupLock(__FUNCTION__, __LINE__);
	}
}

void World::SaveGuilds() {
	MutexMap<int32, Guild*>* guilds = guild_list.GetGuilds();
	MutexMap<int32, Guild*>::iterator itr = guilds->begin();
	while (itr.Next()) {
		Guild* guild = itr.second;
		if (guild->GetSaveNeeded())
			database.SaveGuild(guild);
		if (guild->GetMemberSaveNeeded())
			database.SaveGuildMembers(guild);
		if (guild->GetEventsSaveNeeded())
			database.SaveGuildEvents(guild);
		if (guild->GetRanksSaveNeeded())
			database.SaveGuildRanks(guild);
		if (guild->GetEventFiltersSaveNeeded())
			database.SaveGuildEventFilters(guild);
		if (guild->GetPointsHistorySaveNeeded())
			database.SaveGuildPointsHistory(guild);
		if (guild->GetRecruitingSaveNeeded())
			database.SaveGuildRecruiting(guild);
	}
}

void World::PickRandomLottoDigits(int32* digits) {
	if (digits) {
		for (int32 i = 0; i < 6; i++) {
			bool found = true;
			int32 num = 0;
			while (found) {
				num = ((int32)rand() % 36) + 1;
				for (int32 j = 0; j < 6; j++) {
					if (digits[j] == num)
						break;
					if (j == 5)
						found = false;
				}
			}
			digits[i] = num;
		}
	}
}

void World::AddLottoPlayer(int32 character_id, int32 end_time) {
	LottoPlayer* lp;
	if (lotto_players.count(character_id) == 0) {
		lp = new LottoPlayer;
		lotto_players.Put(character_id, lp);
	}
	else
		lp = lotto_players.Get(character_id);
	lp->end_time = end_time;
	lp->num_matches = 0;
	lp->set = false;
}

void World::RemoveLottoPlayer(int32 character_id) {
	if (lotto_players.count(character_id) > 0)
		lotto_players.erase(character_id, false, true);
}

void World::SetLottoPlayerNumMatches(int32 character_id, int8 num_matches) {
	if (lotto_players.count(character_id) > 0) {
		lotto_players.Get(character_id)->num_matches = num_matches;
		lotto_players.Get(character_id)->set = true;
	}
}

void World::CheckLottoPlayers() {
	MutexMap<int32, LottoPlayer*>::iterator itr = lotto_players.begin();
	while (itr.Next()) {
		LottoPlayer* lp = itr->second;
		if (Timer::GetCurrentTime2() >= lp->end_time && lp->set) {
			int8 num_matches = lp->num_matches;
			LogWrite(PLAYER__DEBUG, 0, "Player", "Num matches: %u", lp->num_matches);
			Client* client = zone_list.GetClientByCharID(itr->first);
			if (client && num_matches >= 2) {
				if (num_matches == 2) {
					client->SimpleMessage(CHANNEL_COLOR_YELLOW, "You receive 10 silver.");
					client->SendPopupMessage(0, "Congratulations! You have won 10 silver!", "", 2, 0xFF, 0xFF, 0x99);
					client->GetPlayer()->AddCoins(1000);
					client->GetPlayer()->GetZone()->SendCastSpellPacket(869, client->GetPlayer());
				}
				else if (num_matches == 3) {
					client->SimpleMessage(CHANNEL_COLOR_YELLOW, "You receive 50 silver.");
					client->SendPopupMessage(0, "Congratulations! You have won 50 silver!", "", 2, 0xFF, 0xFF, 0x99);
					client->GetPlayer()->AddCoins(5000);
					client->GetPlayer()->GetZone()->SendCastSpellPacket(870, client->GetPlayer());
				}
				else if (num_matches == 4) {
					client->SimpleMessage(CHANNEL_COLOR_YELLOW, "You receive 2 gold 50 silver.");
					client->SendPopupMessage(0, "Congratulations! You have won 2 gold 50 silver!", "", 2, 0xFF, 0xFF, 0x99);
					client->GetPlayer()->AddCoins(25000);
					client->GetPlayer()->GetZone()->SendCastSpellPacket(871, client->GetPlayer());
				}
				else if (num_matches == 5) {
					client->SimpleMessage(CHANNEL_COLOR_YELLOW, "You receive 25 gold.");
					client->SendPopupMessage(0, "Congratulations! You have won 25 gold!", "", 2, 0xFF, 0xFF, 0x99);
					client->GetPlayer()->AddCoins(250000);
					client->GetPlayer()->GetZone()->SendCastSpellPacket(872, client->GetPlayer());
				}
				else if (num_matches == 6) {
					Variable* var = variables.FindVariable("gambling_current_jackpot");
					if (var) {
						int64 jackpot = 0;
						try {
							jackpot = atoul(var->GetValue());
						}
						catch (...) {
							jackpot = 10000;
						}
						char coin_message[128];
						char message[512];
						char announcement[512];
						memset(coin_message, 0, sizeof(coin_message));
						memset(message, 0, sizeof(message));
						memset(announcement, 0, sizeof(announcement));
						sprintf(coin_message, "%s", client->GetCoinMessage(jackpot).c_str());
						sprintf(message, "Congratulations! You have won %s!", coin_message);
						sprintf(announcement, "%s as won the jackpot containing a total of %s!", client->GetPlayer()->GetName(), coin_message);
						client->Message(CHANNEL_COLOR_YELLOW, "You receive %s.", coin_message);
						client->SendPopupMessage(0, message, "", 2, 0xFF, 0xFF, 0x99);
						zone_list.HandleGlobalAnnouncement(announcement);
						client->GetPlayer()->AddCoins(jackpot);
						client->GetPlayer()->GetZone()->SendCastSpellPacket(843, client->GetPlayer());
						client->GetPlayer()->GetZone()->SendCastSpellPacket(1413, client->GetPlayer());
					}
				}
			}
			RemoveLottoPlayer(itr->first);
		}
	}
}

void World::AddHouseZone(int32 id, string name, int64 cost_coins, int32 cost_status, int64 upkeep_coins, int32 upkeep_status, int8 vault_slots, int8 alignment, int8 guild_level, int32 zone_id, int32 exit_zone_id, float exit_x, float exit_y, float exit_z, float exit_heading) {
	MHouseZones.writelock(__FUNCTION__, __LINE__);
	if (m_houseZones.count(id) == 0) {
		HouseZone* hz = new HouseZone;
		//ZeroMemory(hz, sizeof(HouseZone));
		hz->id = id;
		hz->name = name;
		hz->cost_coin = cost_coins;
		hz->cost_status = cost_status;
		hz->upkeep_coin = upkeep_coins;
		hz->upkeep_status = upkeep_status;
		hz->vault_slots = vault_slots;
		hz->alignment = alignment;
		hz->guild_level = guild_level;
		hz->zone_id = zone_id;
		hz->exit_zone_id = exit_zone_id;
		hz->exit_x = exit_x;
		hz->exit_y = exit_y;
		hz->exit_z = exit_z;
		hz->exit_heading = exit_heading;
		m_houseZones[id] = hz;
	}
	else {
		LogWrite(WORLD__ERROR, 0, "Housing", "Duplicate house id (%u) for %s", id, name.c_str());
	}
	MHouseZones.releasewritelock(__FUNCTION__, __LINE__);
}

HouseZone* World::GetHouseZone(int32 id) {
	HouseZone* ret = 0;

	MHouseZones.readlock(__FUNCTION__, __LINE__);
	if (m_houseZones.count(id) > 0)
		ret = m_houseZones[id];
	MHouseZones.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

void World::AddPlayerHouse(int32 char_id, int32 house_id, int64 unique_id, int32 instance_id, int32 upkeep_due, int64 escrow_coins, int32 escrow_status, string player_name) {
	MPlayerHouses.writelock(__FUNCTION__, __LINE__);
	if (m_playerHouses.count(house_id) == 0 || m_playerHouses[house_id].count(char_id) == 0) {
		PlayerHouse* ph = new PlayerHouse;
		ph->house_id = house_id;
		ph->unique_id = unique_id;
		ph->instance_id = instance_id;
		ph->escrow_coins = escrow_coins;
		ph->escrow_status = escrow_status;
		ph->upkeep_due = upkeep_due;
		ph->player_name = player_name;

		m_playerHouses[house_id][char_id] = ph;
	}
	MPlayerHouses.releasewritelock(__FUNCTION__, __LINE__);
}

PlayerHouse* World::GetPlayerHouseByHouseID(int32 char_id, int32 house_id) {
	PlayerHouse* ret = 0;

	MPlayerHouses.readlock(__FUNCTION__, __LINE__);
	if (m_playerHouses.count(house_id) > 0 && m_playerHouses[house_id].count(char_id) > 0)
		ret = m_playerHouses[house_id][char_id];
	MPlayerHouses.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

PlayerHouse* World::GetPlayerHouseByUniqueID(int64 unique_id) {
	PlayerHouse* ret = 0;

	MPlayerHouses.readlock(__FUNCTION__, __LINE__);
	map<int32, map<int32, PlayerHouse*> >::iterator itr;
	for (itr = m_playerHouses.begin(); itr != m_playerHouses.end(); itr++) {
		map<int32, PlayerHouse*>::iterator itr2;
		for (itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++) {
			if (itr2->second->unique_id == unique_id) {
				ret = itr2->second;
				break;
			}
		}
		if (ret)
			break;
	}
	MPlayerHouses.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

PlayerHouse* World::GetPlayerHouseByInstanceID(int32 instance_id) {
	PlayerHouse* ret = 0;

	MPlayerHouses.readlock(__FUNCTION__, __LINE__);
	map<int32, map<int32, PlayerHouse*> >::iterator itr;
	for (itr = m_playerHouses.begin(); itr != m_playerHouses.end(); itr++) {
		map<int32, PlayerHouse*>::iterator itr2;
		for (itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++) {
			if (itr2->second->instance_id == instance_id) {
				ret = itr2->second;
				break;
			}
		}
		if (ret)
			break;
	}
	MPlayerHouses.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

vector<PlayerHouse*> World::GetAllPlayerHouses(int32 char_id) {
	vector<PlayerHouse*> ret;

	MPlayerHouses.readlock(__FUNCTION__, __LINE__);
	map<int32, map<int32, PlayerHouse*> >::iterator itr;
	for (itr = m_playerHouses.begin(); itr != m_playerHouses.end(); itr++) {
		if (itr->second.count(char_id) > 0)
			ret.push_back(itr->second[char_id]);
	}
	MPlayerHouses.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}

vector<PlayerHouse*> World::GetAllPlayerHousesByHouseID(int32 house_id) {
	vector<PlayerHouse*> ret;

	MPlayerHouses.readlock(__FUNCTION__, __LINE__);
	if (m_houseZones.count(house_id) > 0) {
		map<int32, PlayerHouse*>::iterator itr;
		for (itr = m_playerHouses[house_id].begin(); itr != m_playerHouses[house_id].end(); itr++)
			ret.push_back(itr->second);
	}
	MPlayerHouses.releasereadlock(__FUNCTION__, __LINE__);

	return ret;
}



void World::PopulateTOVStatMap() {
	//This function populates a map that converts changed CoE to ToV stats
	tov_itemstat_conversion[0] = TOV_ITEM_STAT_HPREGEN;
	tov_itemstat_conversion[1] = TOV_ITEM_STAT_MANAREGEN;
	tov_itemstat_conversion[2] = TOV_ITEM_STAT_HPREGENPPT;
	tov_itemstat_conversion[3] = TOV_ITEM_STAT_MPREGENPPT;
	tov_itemstat_conversion[4] = TOV_ITEM_STAT_COMBATHPREGENPPT;
	tov_itemstat_conversion[5] = TOV_ITEM_STAT_COMBATMPREGENPPT;
	tov_itemstat_conversion[6] = TOV_ITEM_STAT_MAXHP;
	tov_itemstat_conversion[7] = TOV_ITEM_STAT_MAXHPPERC;
	tov_itemstat_conversion[8] = TOV_ITEM_STAT_MAXHPPERCFINAL;
	tov_itemstat_conversion[9] = TOV_ITEM_STAT_SPEED;
	tov_itemstat_conversion[10] = TOV_ITEM_STAT_SLOW;
	tov_itemstat_conversion[11] = TOV_ITEM_STAT_MOUNTSPEED;
	tov_itemstat_conversion[12] = TOV_ITEM_STAT_MOUNTAIRSPEED;
	tov_itemstat_conversion[13] = TOV_ITEM_STAT_LEAPSPEED;
	tov_itemstat_conversion[14] = TOV_ITEM_STAT_LEAPTIME;
	tov_itemstat_conversion[15] = TOV_ITEM_STAT_GLIDEEFFICIENCY;
	tov_itemstat_conversion[16] = TOV_ITEM_STAT_OFFENSIVESPEED;
	tov_itemstat_conversion[17] = TOV_ITEM_STAT_ATTACKSPEED;
	tov_itemstat_conversion[18] = 698;
	tov_itemstat_conversion[19] = TOV_ITEM_STAT_MAXMANA;
	tov_itemstat_conversion[20] = TOV_ITEM_STAT_MAXMANAPERC;
	tov_itemstat_conversion[21] = TOV_ITEM_STAT_MAXATTPERC;
	tov_itemstat_conversion[22] = TOV_ITEM_STAT_BLURVISION;
	tov_itemstat_conversion[23] = TOV_ITEM_STAT_MAGICLEVELIMMUNITY;
	tov_itemstat_conversion[24] = TOV_ITEM_STAT_HATEGAINMOD;
	tov_itemstat_conversion[25] = TOV_ITEM_STAT_COMBATEXPMOD;
	tov_itemstat_conversion[26] = TOV_ITEM_STAT_TRADESKILLEXPMOD;
	tov_itemstat_conversion[27] = TOV_ITEM_STAT_ACHIEVEMENTEXPMOD	;
	tov_itemstat_conversion[28] = TOV_ITEM_STAT_SIZEMOD;
	tov_itemstat_conversion[29] = TOV_ITEM_STAT_DPS;
	tov_itemstat_conversion[30] = 698;
	tov_itemstat_conversion[31] = TOV_ITEM_STAT_STEALTH;
	tov_itemstat_conversion[32] = TOV_ITEM_STAT_INVIS;
	tov_itemstat_conversion[33] = TOV_ITEM_STAT_SEESTEALTH;
	tov_itemstat_conversion[34] = TOV_ITEM_STAT_SEEINVIS;
	tov_itemstat_conversion[35] = TOV_ITEM_STAT_EFFECTIVELEVELMOD;
	tov_itemstat_conversion[36] = TOV_ITEM_STAT_RIPOSTECHANCE;
	tov_itemstat_conversion[37] = TOV_ITEM_STAT_PARRYCHANCE;
	tov_itemstat_conversion[38] = TOV_ITEM_STAT_DODGECHANCE;
	tov_itemstat_conversion[39] = TOV_ITEM_STAT_AEAUTOATTACKCHANCE;
	tov_itemstat_conversion[40] = 698;
	tov_itemstat_conversion[41] = TOV_ITEM_STAT_MULTIATTACKCHANCE;
	tov_itemstat_conversion[42] = 698;
	tov_itemstat_conversion[43] = 698;
	tov_itemstat_conversion[44] = 698;
	tov_itemstat_conversion[45] = TOV_ITEM_STAT_SPELLMULTIATTACKCHANCE;
	tov_itemstat_conversion[46] = 698;
	tov_itemstat_conversion[47] = TOV_ITEM_STAT_FLURRY;
	tov_itemstat_conversion[48] = 698;
	tov_itemstat_conversion[49] = TOV_ITEM_STAT_MELEEDAMAGEMULTIPLIER;
	tov_itemstat_conversion[50] = TOV_ITEM_STAT_EXTRAHARVESTCHANCE;
	tov_itemstat_conversion[51] = TOV_ITEM_STAT_EXTRASHIELDBLOCKCHANCE;
	tov_itemstat_conversion[52] = TOV_ITEM_STAT_ITEMHPREGENPPT;
	tov_itemstat_conversion[53] = TOV_ITEM_STAT_ITEMPPREGENPPT;
	tov_itemstat_conversion[54] = TOV_ITEM_STAT_MELEECRITCHANCE;
	tov_itemstat_conversion[55] = TOV_ITEM_STAT_CRITAVOIDANCE;
	tov_itemstat_conversion[56] = TOV_ITEM_STAT_BENEFICIALCRITCHANCE;
	tov_itemstat_conversion[57] = TOV_ITEM_STAT_CRITBONUS;
	tov_itemstat_conversion[58] = 698;
	tov_itemstat_conversion[59] = TOV_ITEM_STAT_POTENCY;
	tov_itemstat_conversion[60] = 698;
	tov_itemstat_conversion[61] = TOV_ITEM_STAT_UNCONSCIOUSHPMOD;
	tov_itemstat_conversion[62] = TOV_ITEM_STAT_ABILITYREUSESPEED;
	tov_itemstat_conversion[63] = TOV_ITEM_STAT_ABILITYRECOVERYSPEED;
	tov_itemstat_conversion[64] = TOV_ITEM_STAT_ABILITYCASTINGSPEED;
	tov_itemstat_conversion[65] = TOV_ITEM_STAT_SPELLREUSESPEED;
	tov_itemstat_conversion[66] = TOV_ITEM_STAT_MELEEWEAPONRANGE;
	tov_itemstat_conversion[67] = TOV_ITEM_STAT_RANGEDWEAPONRANGE;
	tov_itemstat_conversion[68] = TOV_ITEM_STAT_FALLINGDAMAGEREDUCTION;
	tov_itemstat_conversion[69] = TOV_ITEM_STAT_RIPOSTEDAMAGE;
	tov_itemstat_conversion[70] = TOV_ITEM_STAT_MINIMUMDEFLECTIONCHANCE;
	tov_itemstat_conversion[71] = TOV_ITEM_STAT_MOVEMENTWEAVE;
	tov_itemstat_conversion[72] = TOV_ITEM_STAT_COMBATHPREGEN;
	tov_itemstat_conversion[73] = TOV_ITEM_STAT_COMBATMANAREGEN;
	tov_itemstat_conversion[74] = TOV_ITEM_STAT_CONTESTSPEEDBOOST;
	tov_itemstat_conversion[75] = TOV_ITEM_STAT_TRACKINGAVOIDANCE;
	tov_itemstat_conversion[76] = TOV_ITEM_STAT_STEALTHINVISSPEEDMOD;
	tov_itemstat_conversion[77] = TOV_ITEM_STAT_LOOT_COIN;
	tov_itemstat_conversion[78] = TOV_ITEM_STAT_ARMORMITIGATIONINCREASE;
	tov_itemstat_conversion[79] = TOV_ITEM_STAT_AMMOCONSERVATION;
	tov_itemstat_conversion[80] = TOV_ITEM_STAT_STRIKETHROUGH;
	tov_itemstat_conversion[81] = TOV_ITEM_STAT_STATUSBONUS;
	tov_itemstat_conversion[82] = TOV_ITEM_STAT_ACCURACY;
	tov_itemstat_conversion[83] = TOV_ITEM_STAT_COUNTERSTRIKE;
	tov_itemstat_conversion[84] = TOV_ITEM_STAT_SHIELDBASH;
	tov_itemstat_conversion[85] = TOV_ITEM_STAT_WEAPONDAMAGEBONUS;
	tov_itemstat_conversion[86] = 698;
	tov_itemstat_conversion[87] = TOV_ITEM_STAT_WEAPONDAMAGEBONUSMELEEONLY;
	tov_itemstat_conversion[88] = TOV_ITEM_STAT_ADDITIONALRIPOSTECHANCE;
	tov_itemstat_conversion[89] = TOV_ITEM_STAT_PVPTOUGHNESS;
	tov_itemstat_conversion[90] = TOV_ITEM_STAT_PVPLETHALITY;
	tov_itemstat_conversion[91] = TOV_ITEM_STAT_STAMINABONUS;
	tov_itemstat_conversion[92] = TOV_ITEM_STAT_WISDOMMITBONUS;
	tov_itemstat_conversion[93] = TOV_ITEM_STAT_HEALRECEIVE;
	tov_itemstat_conversion[94] = TOV_ITEM_STAT_HEALRECEIVEPERC;
	tov_itemstat_conversion[95] = TOV_ITEM_STAT_PVPCRITICALMITIGATION;
	tov_itemstat_conversion[96] = TOV_ITEM_STAT_BASEAVOIDANCEBONUS;
	tov_itemstat_conversion[97] = TOV_ITEM_STAT_INCOMBATSAVAGERYREGEN;
	tov_itemstat_conversion[98] = TOV_ITEM_STAT_OUTOFCOMBATSAVAGERYREGEN;
	tov_itemstat_conversion[99] = TOV_ITEM_STAT_SAVAGERYREGEN;
	tov_itemstat_conversion[100] = TOV_ITEM_STAT_SAVAGERYGAINMOD;
	tov_itemstat_conversion[101] = TOV_ITEM_STAT_MAXSAVAGERYLEVEL;
}

int32 World::LoadItemBlueStats() {
	Query query;
	MYSQL_ROW row;
	int32 count = 0;
	MYSQL_RES* result = query.RunQuery2(Q_SELECT, "SELECT version_range1,version_range2,emu_stat,name,stat from itemstats");

	if (result && mysql_num_rows(result) > 0) {
		while (result && (row = mysql_fetch_row(result))) {
			count++;

			if (atoi(row[0]) >= 63119) //KA
				ka_itemstat_conversion[atoi(row[2])] = atoi(row[4]);
			else if (atoi(row[0]) >= 57101) // ToV
				tov_itemstat_conversion[atoi(row[2])] = atoi(row[4]);
			else if (atoi(row[0]) >= 1193) // CoE
				coe_itemstat_conversion[atoi(row[2])] = atoi(row[4]);
			else if (atoi(row[0]) >= 1096) // DoV
				dov_itemstat_conversion[atoi(row[2])] = atoi(row[4]);
		}
	}
	return count;
}

sint16 World::GetItemStatTOVValue(sint16 subtype) {
	return (tov_itemstat_conversion[subtype] - 600);
}
sint16 World::GetItemStatDOVValue(sint16 subtype) {
	return (dov_itemstat_conversion[subtype] - 600);
}
sint16 World::GetItemStatCOEValue(sint16 subtype) {
	return (coe_itemstat_conversion[subtype] - 600);
}
sint16 World::GetItemStatKAValue(sint16 subtype) {
	return (ka_itemstat_conversion[subtype] - 600);
}


#ifdef WIN32
ulong World::GetCurrentThreadID(){
	return GetCurrentThreadId();
}

int64 World::GetThreadUsageCPUTime(){
	HANDLE handle = GetCurrentThread();
	int64 lpCreationTime;
	int64 lpExitTime;
	int64 lpKernelTime;
	int64 lpUserTime;
	if(GetThreadTimes(handle, (FILETIME*)&lpCreationTime, (FILETIME*)&lpExitTime, (FILETIME*)&lpKernelTime, (FILETIME*)&lpUserTime))
		return lpKernelTime + lpUserTime;
	return 0;
}
#else

#endif
