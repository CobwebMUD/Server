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
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <sqlite3.h>
#include <ctime>

class Character {
	public:
		Character(const char * accountName, const char * name, std::vector<std::string> keywords, const char * shortDesc, const char * mainDesc);
		Character(int ID);
		Character(const char * accountName);
		int getID();
		int getLoc();
		std::string getAccountName();
		std::string getName();
		std::vector<std::string> getKeywords();
		std::string getSDesc();
		std::string getMDesc();
		struct CharInd {
			int ID;
			std::string name;
			int ind;
		};
		std::vector<Character::CharInd> getAccountChars();
	private:
		char const* dbFile = "data/game.db";

		int charID;
		int charLocation;
		std::string charAccountName;
		std::string charName;
		std::vector<std::string> charKeywords;
		std::string charSDesc;
		std::string charMDesc;
		std::string timeCreated;
		std::vector<CharInd> accountChars;

		int createCharTable();
		int storeCharInDB();
		int gatherDetailsByID();
		int gatherDetailsByCharName();
		std::string joinVectorString(std::vector<std::string> v);
		int gatherAccountCharsFromDB();
};
