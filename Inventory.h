/*
 * =====================================================================================
 *
 *       Filename:  Inventory.h
 *
 *    Description:  Inventory class for user iteams
 *
 *        Version:  1.0
 *        Created:  07/09/2016 03:19:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb MUD team 
 *
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sqlite3.h>

class Inventory {
	
	public:
		// Param -> player username
		Inventory(std::string username);

		// Item struct <- id of item, amount of item
		struct Item {
			int id;
			int amt;
		};

		// Identify item via item database
		std::string identifyItem(int itemID);

		// Add item to user inventory 
		void addItem(int id, int amt);

		// Save inventory to file
		void saveInventory();

		// Get player inventory 
		std::vector<Item>* getItems();

	private:
		// Players unique username used as name for players inventory save file "playersname.inv"
		std::string username;

		// Function to load in inventory from player inventory binary save file
		void loadInventory();

		// Player items 
		std::vector<Item> items;
};
