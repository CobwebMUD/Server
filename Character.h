/*
 * =====================================================================================
 *
 *       Filename:  Character.h
 *
 *    Description:  Character class for maintaining characters of an account.
 *
 *        Version:  1.0
 *        Created:  07/21/2016 10:36:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   
 *   Organization:	CobwebMUD 
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <sqlite3.h>
#include <ctime>

class Character {
	public:
		Character(std::string accountName, std::string name, std::vector<std::string> keywords, std::string shortDesc, std::string mainDesc);
		Character(std::string accountName, int ID, std::string name);
	private:
		char const* dbFile = "data/game.db";
		std::string charAccountName;
		int charID;
		std::string charName;
		std::vector<std::string> charKeywords;
		std::string charShort;
		std::string charMain;
		int charLocation;
		int createCharTable();
		int storeCharInDB();
		std::string joinVectorString(std::vector<std::string> v);
};
