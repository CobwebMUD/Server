/*
 * =====================================================================================
 *
 *       Filename:  Account.cpp
 * *    Description:  Implementation of Account class
 *
 *        Version:  1.0
 *        Created:  06/19/2016 08:11:38 PM
 *       Revision:  none
 *       Compiler:  gcc *
 *         Author:  
 *   Organization:  CobwebMUD 
 *
 * ===================================================================================== */

#include "Account.h"

// The constructor for a new account.  
Account::Account(std::string userName, std::string pass, std::string email) : accountName(userName), accountPass(pass), accountEmail(email) 
{ 
	// Set timeCreated to a unix timestamp. 
	time_t timestamp; 
	struct tm * timeGMT;
	char dateStr[80];
	
	time(&timestamp);
	timeGMT = gmtime(&timestamp);
	strftime(dateStr, 80, "%F %X", timeGMT);
	timeCreated = dateStr;
	std::cout << " User name: " << accountName << std::endl;
	std::cout << " pass: " << accountPass << std::endl;
	std::cout << " email: " << accountEmail << std::endl;
	std::cout << " date created: " << timeCreated << std::endl;
	// Create account 
	if (createAccountTable() == 0)
	{
		if (!exists()) 
		{
			if (storeAccount() == 0)
			{
				std::cout << "Account created." << std::endl;
				loggedIn = true;
				
				details.name = accountName;
				details.pass = accountPass;
				details.email = accountEmail;
				details.date = timeCreated;
			}
			else 
				std::cout << "failed to store account info" << std::endl;
		} 
		else 
			std::cout << "failed to creating account, username already exists." << std::endl;
	} 
	else 
		std::cout << "Unable to create account table" << std::endl;
}

// The constructor for logging into an existing account.
Account::Account(std::string userName, std::string pass) : accountName(userName), accountPass(pass) 
{
	if (createAccountTable() == 0)
	{
		// Check if username exists
		if (exists()) 
		{
			// Gather info from username into struct 'details'
			if (findDetailsByUsername() == 0)
				loggedIn = (details.pass == accountPass);
			else 
				std::cout << "failed to get them juicy details" << std::endl;
		} 
		else 
			loggedIn = false;
	}
	else 
		std::cout << "Unable to create account table" << std::endl;
}

int Account::createAccountTable() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	// Create Account table in database.
	char const* sql =
		"CREATE TABLE IF NOT EXISTS Account ("
		"ID INTEGER PRIMARY KEY," 
		"UserName TEXT NOT NULL," 
		"Password TEXT NOT NULL," 
		"Email TEXT NOT NULL,"
		"TimeCreated TEXT NOT NULL);";
	char* errMsg;
	rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error creating table Account: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_close(db);
	return 0;
}

// Function to store a new account details in database.
int Account::storeAccount() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	// Prepare statement of inserting row into Account table. 
	sqlite3_prepare_v2(db, "INSERT INTO Account (Username,Password,Email,TimeCreated) VALUES (?1,?2,?3,?4);", -1, &stmt, NULL);
		// Bind new account variables to statement placeholders.
	sqlite3_bind_text(stmt, 1, accountName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, accountPass.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, accountEmail.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, timeCreated.c_str(), -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr,"ERROR executing statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

bool Account::exists() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) 
	{
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}
	sqlite3_stmt *stmt;
	// Check if account name already exists.
	char sqlString[] = "SELECT COUNT(*) FROM Account WHERE Username = '";
	strcat(sqlString, accountName.c_str());
	strcat(sqlString, "';");
	rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "ERROR preparing database for exists() function: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return false;
	}
	// Execute statement
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr, "ERROR executing statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return false;
	}
	int count = sqlite3_column_int(stmt, 0);
	std::cout << "Number of users with same username existing: " << count << std::endl;
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return (count > 0);
}

// Function that uses the account's username and returns account info.
int Account::findDetailsByUsername() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc != SQLITE_OK) 
	{
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	
	sqlite3_prepare_v2(db, "SELECT * FROM Account WHERE UserName = ?;", -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, accountName.c_str(), sizeof(accountName.c_str()), 0);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr, "ERROR preparing database for finding details: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	if (rc == SQLITE_ROW) {
		details.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		details.pass = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		details.email = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
		details.date = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
	}
	
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	return 0;
}
