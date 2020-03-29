#include "Trade.h"
#include "Items/Items.h"
#include "Entity.h"
#include "Bots/Bot.h"
#include "../common/Log.h"

extern ConfigReader configReader;
extern MasterItemList master_item_list;

Trade::Trade(Entity* trader1, Entity* trader2) {
	this->trader1 = trader1;
	this->trader2 = trader2;

	trader1_accepted = false;
	trader2_accepted = false;

	trader1_coins = 0;
	trader2_coins = 0;


	OpenTradeWindow();
}

Trade::~Trade() {

}

int8 Trade::AddItemToTrade(Entity* character, Item* item, int8 quantity, int8 slot) {
	LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) adding item (%u) to slot %u of the trade window", character->GetName(), item->details.item_id, slot);
	if (slot == 255)
		slot = GetNextFreeSlot(character);

	if (slot < 0 || slot > 11) {
		LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) tried to add an item to an invalid trade slot (%u)", character->GetName(), slot);
		return 255;
	}

	Entity* other = GetTradee(character);
	int8 result = CheckItem(character, item, other->IsBot());

	if (result == 0) {
		if (character == trader1) {
			Trader1ItemAdd(item, quantity, slot);
			// Only trader2 can be a bot so only
			// need to do the bot check here
			if (trader2->IsBot()) {
				((Bot*)trader2)->TradeItemAdded(item);
			}
		}
		else if (character == trader2)
			Trader2ItemAdd(item, quantity, slot);
		else {
			LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) tried to add an item to a trade but was neither trader1 or trader2", character->GetName());
			return 255;
		}
	}

	SendTradePacket();
	return result;
}

int8 Trade::CheckItem(Entity* trader, Item* item, bool other_is_bot) {
	int8 ret = 0;
	map<int8, TradeItemInfo>* list = 0;
	map<int8, TradeItemInfo>::iterator itr;

	if (trader == trader1)
		list = &trader1_items;
	else if (trader == trader2)
		list = &trader2_items;

	if (list) {
		if (trader->IsPlayer()) {
			// Check to see if the item is already in the trade
			for (itr = list->begin(); itr != list->end(); itr++) {
				if (itr->second.item->details.unique_id == item->details.unique_id) {
					ret = 1;
					break;
				}
			}

			// Only allow heirloom and no-trade items to be traded with a bot
			if (!other_is_bot) {
				if (item->CheckFlag(NO_TRADE))
					ret = 2;
				if (item->CheckFlag2(HEIRLOOM))
					ret = 3;
			}
		}
	}

	return ret;
}

void Trade::RemoveItemFromTrade(Entity* character, int8 slot) {
	map<int8, TradeItemInfo>* list = 0;

	if (character == trader1)
		list = &trader1_items;
	else if (character == trader2)
		list = &trader2_items;

	if (list) {
		if (list->count(slot) > 0) {
			list->erase(slot);
			SendTradePacket();
		}
		else
			LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) tried to remove an item from a trade slot that was empty ", character->GetName());
	}
}

void Trade::AddCoinToTrade(Entity* character, int64 amount) {
	if (!character->IsPlayer())
		return;

	if (character == trader1) {
		trader1_coins += amount;
		if (!((Player*)character)->HasCoins(trader1_coins)) {
			trader1_coins -= amount;
			LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) tried to add more coins then they had ", character->GetName());
		}
	}
	else if (character == trader2) {
		trader2_coins += amount;
		if (!((Player*)character)->HasCoins(trader2_coins)) {
			trader2_coins -= amount;
			LogWrite(PLAYER__ERROR, 0, "Trade", "Player (%s) tried to add more coins then they had ", character->GetName());
		}
	}

	SendTradePacket();
}

void Trade::RemoveCoinFromTrade(Entity* character, int64 amount) {
	if (character == trader1)
		trader1_coins = (amount >= trader1_coins) ? 0 : trader1_coins - amount;
	else if (character == trader2)
		trader2_coins = (amount >= trader2_coins) ? 0 : trader2_coins - amount;

	SendTradePacket();
}

Entity* Trade::GetTradee(Entity* character) {
	if (character == trader1)
		return trader2;
	else if (character == trader2)
		return trader1;

	return 0;
}

bool Trade::SetTradeAccepted(Entity* character) {
	if (character == trader1)
		trader1_accepted = true;
	else if (character == trader2)
		trader2_accepted = true;
	else
		return false;

	Entity* other = GetTradee(character);
	if (other) {
		if (other->IsPlayer()) {
			Client* client = other->GetZone()->GetClientBySpawn(other);
			PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
			if (packet) {
				packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(character));
				packet->setDataByName("type", 16);
				client->QueuePacket(packet->serialize());
				safe_delete(packet);
			}
		}
		else if (other->IsBot()) {
			Client* client = character->GetZone()->GetClientBySpawn(character);
			if (trader1_coins > 0) {
				CancelTrade(other);
				if (client)
					client->SimpleMessage(CHANNEL_ERROR, "Bots can't take coins so the trade was canceled.");

				return true;
			}
			else {
				if (!((Bot*)other)->CheckTradeItems(&trader1_items)) {
					CancelTrade(other);
					if (client)
						client->SimpleMessage(CHANNEL_ERROR, "There was an item the bot could not equip so the trade was canceled.");

					return true;
				}
				else
					trader2_accepted = true;
			}
		}

		if (HasAcceptedTrade(other)) {
			CompleteTrade();
			return true;
		}
	}

	return false;
}

bool Trade::HasAcceptedTrade(Entity* character) {
	if (character == trader1)
		return trader1_accepted;
	else if (character == trader2)
		return trader2_accepted;

	return false;
}

void Trade::CancelTrade(Entity* character) {
	Entity* other = GetTradee(character);
	if (other){
		if (other->IsPlayer()) {
			Client* client = other->GetZone()->GetClientBySpawn(other);
			PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
			if (packet) {
				packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(character));
				packet->setDataByName("type", 2);
				client->QueuePacket(packet->serialize());
				safe_delete(packet);
			}
		}
		else if (other->IsBot())
			((Bot*)other)->FinishTrade();
	}

	trader1->trade = 0;
	trader2->trade = 0;
}

void Trade::Trader1ItemAdd(Item* item, int8 quantity, int8 slot) {
	trader1_items[slot].item = item;
	trader1_items[slot].quantity = quantity;
}

void Trade::Trader2ItemAdd(Item* item, int8 quantity, int8 slot) {
	trader2_items[slot].item = item;
	trader2_items[slot].quantity = quantity;
}

void Trade::CompleteTrade() {
	map<int8, TradeItemInfo>::iterator itr;
	map<int32, int8> trader1_item_ids;
	map<int32, int8>::iterator itr2;
	string log_string = "TradeComplete:\n";

	if (trader1->IsPlayer()) {
		Player* player = (Player*)trader1;
		Client* client = player->GetZone()->GetClientBySpawn(player);
		if (client) {
			log_string += "Trader1 = ";
			log_string += trader1->GetName();
			log_string += "(" + to_string(client->GetCharacterID()) + ")\n";
			log_string += "Coins: " + to_string(trader1_coins) + "\n";
			log_string += "Items:\n";
			player->RemoveCoins(trader1_coins);
			for (itr = trader1_items.begin(); itr != trader1_items.end(); itr++) {
				// client->RemoveItem can delete the item so we need to store the item id's and quantity to give to trader2
				trader1_item_ids[itr->second.item->details.item_id] = itr->second.quantity;
				log_string += itr->second.item->name + " (" + to_string(itr->second.item->details.item_id) + ") x" + to_string(itr->second.quantity) + "\n";
				client->RemoveItem(itr->second.item, itr->second.quantity);
			}

			player->AddCoins(trader2_coins);
			for (itr = trader2_items.begin(); itr != trader2_items.end(); itr++) {
				client->AddItem(itr->second.item->details.item_id, itr->second.quantity);
			}

			PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
			if (packet) {
				packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader2));
				packet->setDataByName("type", 24);
				client->QueuePacket(packet->serialize());
				safe_delete(packet);
			}
		}
	}
	// trader1 is the player who starts the trade, will never be a bot, if trader1 is not a player something went horribly wrong.


	if (trader2->IsPlayer()) {
		Player* player = (Player*)trader2;
		Client* client = player->GetZone()->GetClientBySpawn(player);
		if (client) {
			log_string += "Trader2 = ";
			log_string += trader2->GetName();
			log_string += "(" + to_string(client->GetCharacterID()) + ")\n";
			log_string += "Coins: " + to_string(trader2_coins) + "\n";
			log_string += "Items:\n";
			player->RemoveCoins(trader2_coins);
			for (itr = trader2_items.begin(); itr != trader2_items.end(); itr++) {
				log_string += itr->second.item->name + " (" + to_string(itr->second.item->details.item_id) + ") x" + to_string(itr->second.quantity) + "\n";
				client->RemoveItem(itr->second.item, itr->second.quantity);
			}

			player->AddCoins(trader1_coins);
			for (itr2 = trader1_item_ids.begin(); itr2 != trader1_item_ids.end(); itr2++) {
				client->AddItem(itr2->first, itr2->second);
			}

			PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
			if (packet) {
				packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader1));
				packet->setDataByName("type", 24);
				client->QueuePacket(packet->serialize());
				safe_delete(packet);
			}
		}
	}
	else if (trader2->IsBot()) {
		Bot* bot = (Bot*)trader2;
		log_string += "Trader2 is a bot";
		for (itr = trader2_items.begin(); itr != trader2_items.end(); itr++) {
			bot->RemoveItem(itr->second.item);
		}

		for (itr2 = trader1_item_ids.begin(); itr2 != trader1_item_ids.end(); itr2++) {
			bot->GiveItem(itr2->first);
		}
		bot->FinishTrade();
	}

	LogWrite(PLAYER__INFO, 0, "Trade", log_string.c_str());

	trader1->trade = 0;
	trader2->trade = 0;
}

void Trade::OpenTradeWindow() {
	if (trader1->IsPlayer()) {
		Client* client = trader1->GetZone()->GetClientBySpawn(trader1);
		PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
		if (packet) {
			packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader2));
			packet->setDataByName("type", 1);
			client->QueuePacket(packet->serialize());
			safe_delete(packet);
		}
	}

	if (trader2->IsPlayer()) {
		Client* client = trader2->GetZone()->GetClientBySpawn(trader2);
		PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
		if (packet) {
			packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader1));
			packet->setDataByName("type", 1);
			client->QueuePacket(packet->serialize());
			safe_delete(packet);
		}
	}
}

void Trade::SendTradePacket() {
	if (trader1->IsPlayer()) {
		Client* client = trader1->GetZone()->GetClientBySpawn(trader1);

		PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
		if (packet) {
			packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader2));
			packet->setDataByName("type", 1);

			int8 size = (int8)(trader1_items.size());
			int8 i = 0;
			map<int8, TradeItemInfo>::iterator itr;

			packet->setArrayLengthByName("your_item_count", size);
			for (itr = trader1_items.begin(); itr != trader1_items.end(); itr++) {
				packet->setArrayDataByName("your_item_unknown1", 1, i);
				packet->setArrayDataByName("your_item_unknown2", 1, i);
				packet->setArrayDataByName("your_item_slot", itr->first, i);
				packet->setArrayDataByName("your_item_id", itr->second.item->details.item_id, i);
				packet->setArrayDataByName("your_item_quantity", itr->second.quantity, i);
				packet->setArrayDataByName("your_item_icon", itr->second.item->details.icon, i);
				packet->setArrayDataByName("your_item_background", 0, i); // No clue on this value yet
				i++;
			}
			int32 plat = 0;
			int32 gold = 0;
			int32 silver = 0;
			int32 copper = 0;

			CalculateCoins(trader1_coins, plat, gold, silver, copper);
			packet->setDataByName("your_copper", copper);
			packet->setDataByName("your_silver", silver);
			packet->setDataByName("your_gold", gold);
			packet->setDataByName("your_plat", plat);
			

			size = (int8)(trader2_items.size());
			i = 0;

			packet->setArrayLengthByName("their_item_count", size);
			for (itr = trader2_items.begin(); itr != trader2_items.end(); itr++) {
				packet->setArrayDataByName("their_item_unknown1", 1, i);
				packet->setArrayDataByName("their_item_unknown2", 1, i);
				packet->setArrayDataByName("their_item_slot", itr->first, i);
				packet->setArrayDataByName("their_item_id", itr->second.item->details.item_id, i);
				packet->setArrayDataByName("their_item_quantity", itr->second.quantity, i);
				packet->setArrayDataByName("their_item_icon", itr->second.item->details.icon, i);
				packet->setArrayDataByName("their_item_background", 0, i); // No clue on this value yet
				i++;
			}

			plat = 0;
			gold = 0;
			silver = 0;
			copper = 0;

			CalculateCoins(trader2_coins, plat, gold, silver, copper);
			packet->setDataByName("their_copper", copper);
			packet->setDataByName("their_silver", silver);
			packet->setDataByName("their_gold", gold);
			packet->setDataByName("their_plat", plat);

			LogWrite(PLAYER__ERROR, 0, "Trade", "packet sent");
			client->QueuePacket(packet->serialize());
			safe_delete(packet);
		}
	}

	if (trader2->IsPlayer()) {
		Client* client = trader2->GetZone()->GetClientBySpawn(trader2);
		PacketStruct* packet = configReader.getStruct("WS_PlayerTrade", client->GetVersion());
		if (packet) {
			packet->setDataByName("spawn_id", client->GetPlayer()->GetIDWithPlayerSpawn(trader1));
			packet->setDataByName("type", 1);

			int8 size = (int8)(trader2_items.size());
			int8 i = 0;
			map<int8, TradeItemInfo>::iterator itr;

			packet->setArrayLengthByName("your_item_count", size);
			for (itr = trader2_items.begin(); itr != trader2_items.end(); itr++) {
				packet->setArrayDataByName("your_item_unknown1", 1, i);
				packet->setArrayDataByName("your_item_unknown2", 1, i);
				packet->setArrayDataByName("your_item_slot", itr->first, i);
				packet->setArrayDataByName("your_item_id", itr->second.item->details.item_id, i);
				packet->setArrayDataByName("your_item_quantity", itr->second.quantity, i);
				packet->setArrayDataByName("your_item_icon", itr->second.item->details.icon, i);
				packet->setArrayDataByName("your_item_background", 0, i); // No clue on this value yet
				i++;
			}
			int32 plat = 0;
			int32 gold = 0;
			int32 silver = 0;
			int32 copper = 0;

			CalculateCoins(trader2_coins, plat, gold, silver, copper);
			packet->setDataByName("your_copper", copper);
			packet->setDataByName("your_silver", silver);
			packet->setDataByName("your_gold", gold);
			packet->setDataByName("your_plat", plat);

			size = (int8)(trader1_items.size());
			i = 0;

			packet->setArrayLengthByName("their_item_count", size);
			for (itr = trader1_items.begin(); itr != trader1_items.end(); itr++) {
				packet->setArrayDataByName("their_item_unknown1", 1, i);
				packet->setArrayDataByName("their_item_unknown2", 1, i);
				packet->setArrayDataByName("their_item_slot", itr->first, i);
				packet->setArrayDataByName("their_item_id", itr->second.item->details.item_id, i);
				packet->setArrayDataByName("their_item_quantity", itr->second.quantity, i);
				packet->setArrayDataByName("their_item_icon", itr->second.item->details.icon, i);
				packet->setArrayDataByName("their_item_background", 0, i); // No clue on this value yet
				i++;
			}

			plat = 0;
			gold = 0;
			silver = 0;
			copper = 0;

			CalculateCoins(trader1_coins, plat, gold, silver, copper);
			packet->setDataByName("their_copper", copper);
			packet->setDataByName("their_silver", silver);
			packet->setDataByName("their_gold", gold);
			packet->setDataByName("their_plat", plat);

			client->QueuePacket(packet->serialize());
			safe_delete(packet);
		}
	}
}

void Trade::CalculateCoins(int64 val, int32& plat, int32& gold, int32& silver, int32& copper) {
	int32 tmp = 0;
	if (val >= 1000000) {
		tmp = val / 1000000;
		val -= tmp * 1000000;
		plat = tmp;
	}
	if (val >= 10000) {
		tmp = val / 10000;
		val -= tmp * 10000;
		gold = tmp;
	}
	if (val >= 100) {
		tmp = val / 100;
		val -= tmp * 100;
		silver = tmp;
	}
	if (val > 0) {
		copper = val;
	}
}

int8 Trade::GetNextFreeSlot(Entity* character) {
	map<int8, TradeItemInfo>* list = 0;

	if (character == trader1)
		list = &trader1_items;
	else if (character == trader2)
		list = &trader2_items;
	else
		return 255;

	int8 ret = 255;
	for (int8 index = 0; index < 12; index++) {
		if (list->count(index) == 0) {
			ret = index;
			break;
		}
	}

	return ret;
}