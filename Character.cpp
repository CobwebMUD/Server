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

Character::Character(std::string accountName, std::string name, std::vector<std::string> keywords, std::string shortDesc, std::string mainDesc) : charName(name), charKeywords(keywords), charShort(shortDesc), charMain(mainDesc), charAccountName(accountName) {
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

Character::Character(std::string accountName, int ID, std::string name) : charAccountName(accountName), charID(ID), charName(name) {
	// Load details of existing character from db.
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
		"LastLocation	INTEGER);";
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
	std::cout << "Keywords: " << keywordsString << std::endl;

	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "INSERT INTO Character (AccountName,Name,Keywords,ShortDesc,MainDesc,LastLocation) VALUES (?1,?2,?3,?4,?5,?6);", -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, charAccountName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, charName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, keywordsString.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, charShort.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, charMain.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 6, (charLocation || -1));

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

std::string Character::joinVectorString(std::vector<std::string> v) {
	std::stringstream ss;
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(ss, " "));
	std::string result = ss.str();
	if (!result.empty()) {
		result.resize(result.length() - 1);
	}
	return result;
}
