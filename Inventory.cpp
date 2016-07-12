/*
 * =====================================================================================
 *
 *       Filename:  Inventory.cpp
 *
 *    Description:  Inventory class for user items
 *
 *        Version:  1.0
 *        Created:  07/09/2016 03:42:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb MUD team 
 *
 * =====================================================================================
 */

#include "Inventory.h"

Inventory::Inventory(std::string username) : username(username)
{
	// Load in players inventory	
	loadInventory();
}

void Inventory::addItem(int id, int amt)
{
	// If item ID is already in player inventory increase the ids amt value
	for (int i = 0; i < items.size(); i++)
	{
		if (items.at(i).id == id)
		{
			items.at(i).amt += amt;
			return;
		}
	}
	// If item ID is not in player inventory add it
	Item newItem;
	newItem.id = id;
	newItem.amt = amt;
	items.push_back(newItem);
}

std::string Inventory::identifyItem(int itemID)
{
	sqlite3 *db;
	if (sqlite3_open("itemTable.db", &db))
	{
		std::cout << "Error opening database \"itemTable.db\"" << std::endl;
		return "ERROR";
	}
	sqlite3_stmt *stmt;
	std::string sqlString = "SELECT * FROM Items WHERE ID = '";
	sqlString += std::to_string(itemID);
	sqlString += "';";
	if (sqlite3_prepare_v2(db, sqlString.c_str(), -1, &stmt, NULL) != SQLITE_OK)
	{
		std::cout << "Error preparing database \"itemtable.db\"" << std::endl;
		sqlite3_close(db);
		return "ERROR";
	} 
	sqlite3_step(stmt);
	sqlite3_close(db);

	std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
	std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
	std::string damage = std::to_string(sqlite3_column_int(stmt, 3));
	std::string defense = std::to_string(sqlite3_column_int(stmt, 4));
	std::string effect = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
	std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

	return name + " /" + "Item type: " + type + " /" + "damage: " + damage + " /" + "defense: " + defense + " /" + "effect: " + effect + " /" + "description: " + description + " //";
}

void Inventory::saveInventory() {
	std::string path("userData/" + username + ".inv");
	std::ofstream inv(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	// loop through vector of items structs and write each one to its .inv file
	for (int i = 0; i < items.size(); i++)
	{
		inv.write((const char*)&items.at(i), sizeof(Item));
	}
	inv.close();
}

void Inventory::loadInventory()
{
	std::string path("userData/" + username + ".inv");
	std::ifstream inv(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	std::streampos size;
	size = inv.tellg();
	// invLen = amount of structs saved in file
	int invLen = (size / sizeof(Item));
	inv.seekg(0, std::ios::beg);
	// read each struct from file and load back into vector
	for (int i = 0; i < invLen; i++)
	{
		Item newItem;
		inv.read((char*)&newItem, sizeof(Item));
		items.push_back(newItem);
	}
	inv.close();
}

std::vector<Inventory::Item>* Inventory::getItems()
{
	return &items;
}

