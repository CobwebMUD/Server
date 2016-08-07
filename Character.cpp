/*
 * =====================================================================================
 *
 *       Filename:  Character.cpp
 *
 *    Description:  Implementation of Character class
 *
 *        Version:  1.0
 *        Created:  07/21/2016 10:37:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  CobwebMUD
 *
 * =====================================================================================
 */

#include "Character.h"

// Constructor for a new character

Character::Character(const char * accountName, const char * name, std::vector<std::string> keywords, const char * shortDesc, const char * mainDesc) : charName(name), charKeywords(keywords), charSDesc(shortDesc), charMDesc(mainDesc), charAccountName(accountName) {
	// Set dateCreated to current time based on GMT with format 'YYYY-MM-DD hh:mm:ss'.
	time_t timestamp; 
	struct tm * timeGMT;
	char dateStr[80];
	
	time(&timestamp);
	timeGMT = gmtime(&timestamp);
	strftime(dateStr, 80, "%F %X", timeGMT);
	timeCreated = dateStr;
	// Store character in the database.	
	if (!createCharTable()) {
		if (!storeCharInDB()) {
			std::cout << "Stored character in database." << std::endl;
		} else {
			std::cout << "Error storing character in database." << std::endl;
		}
	} else {
		std::cout << "Error creating character table." << std::endl;
	}
}

// Constructor for an existing character
Character::Character(int ID) : charID(ID) {
	// Load details of existing character from db.
	if (gatherDetailsByID()) {
		std::cout << "Error getting details of character." << std::endl;
	}
}

// Constructor for getting existing characters of an account
Character::Character(const char * accountName) : charAccountName(accountName) {
	std::cout << charAccountName << std::endl;
	if (gatherAccountCharsFromDB())
		std::cout << "Failed to load characters from DB." << std::endl;
	else {
		std::cout << "Loaded characters from DB." << std::endl;
	}
}

int Character::createCharTable() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	// Create Character table in database.
	char const* sql =
		"CREATE TABLE IF NOT EXISTS Character ("
		"ID				INTEGER PRIMARY KEY,"
		"AccountName	TEXT					NOT NULL,"
		"Name			TEXT					NOT NULL,"
		"Keywords		TEXT					NOT NULL,"
		"ShortDesc		TEXT					NOT NULL,"
		"MainDesc		TEXT					NOT NULL,"
		"LastLocation	INTEGER,"
		"TimeCreated	TEXT					NOT NULL);";
	char* errMsg;
	rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error creating table Character: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_close(db);
	return 0;
}

int Character::storeCharInDB() {
	std::string keywordsString = joinVectorString(charKeywords);

	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "INSERT INTO Character (AccountName,Name,Keywords,ShortDesc,MainDesc,LastLocation,TimeCreated) VALUES (?1,?2,?3,?4,?5,?6,?7);", -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, charAccountName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, charName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, keywordsString.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, charSDesc.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, charMDesc.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 6, (charLocation || -1));
	sqlite3_bind_text(stmt, 7, timeCreated.c_str(), -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

// Gather details by ID alone
int Character::gatherDetailsByID() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;

	// prepare statement
	sqlite3_prepare_v2(db, "SELECT * FROM Character WHERE ID = '?';", -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, charID);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		fprintf(stderr, "Error preparing database for func gatherDetailsByID: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
	}
	
	int gatheredID = sqlite3_column_int(stmt, 0);
	std::string gatheredAccountName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
	std::string gatheredName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
	std::string gatheredKeywordStr(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
	std::string gatheredSDesc(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
	std::string gatheredMDesc(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
	int gatheredLoc = sqlite3_column_int(stmt, 6);
	std::string gatheredTimeCreated(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));

	// Iterate through keyword string and split by space into vector.
	std::istringstream iss(gatheredKeywordStr);
	std::vector<std::string> gatheredKeywords{
		std::istream_iterator<std::string>(iss), {}
	};

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	charAccountName = gatheredAccountName;
	charName = gatheredName;
	charSDesc = gatheredSDesc;
	charMDesc = gatheredMDesc;
	charLocation = gatheredLoc;
	charKeywords = gatheredKeywords;
	timeCreated = gatheredTimeCreated;

	return 0;
}

std::string Character::joinVectorString(std::vector<std::string> v) {
	std::stringstream ss;
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(ss, " "));
	std::string result = ss.str();
	if (!result.empty()) {
		result.resize(result.length() - 1);
	}
	return result;
}

std::string Character::getAccountName() {
	return charAccountName;
}

int Character::getID() {
	return charID;
}

std::string Character::getName() {
	return charName;
}

std::vector<std::string> Character::getKeywords() {
	return charKeywords;
}

std::string Character::getSDesc() {
	return charSDesc;
}

std::string Character::getMDesc() {
	return charMDesc;
}

int Character::getLoc() {
	return charLocation;
}

std::vector<Character::CharInd> Character::getAccountChars() {
	return accountChars;
}

int Character::gatherAccountCharsFromDB() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, "SELECT * FROM Character Where AccountName = ?;", -1, &stmt, 0);
	sqlite3_bind_text(stmt, 1, charAccountName.data(), charAccountName.size(), 0);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	CharInd accountChar;
	int ind = 1;

	while (rc == SQLITE_ROW) {
		accountChar.ID = sqlite3_column_int(stmt, 0);
		accountChar.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		accountChar.ind = ind;
		accountChars.push_back(accountChar);
		rc = sqlite3_step(stmt);
		ind++;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	for (auto it = accountChars.begin(); it != accountChars.end(); ++it) {
		std::cout << "(" << it->ind << ") ID: " << it->ID << ", " << "Name: " << it->name << std::endl;
	}

	return 0;
}
