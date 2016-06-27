/*
 * =====================================================================================
 *
 *       Filename:  Account.cpp
 *
 *    Description:  Implementation of Account class
 *
 *        Version:  1.0
 *        Created:  06/19/2016 08:11:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  CobwebMUD 
 *
 * =====================================================================================
 */

#include "Account.h"

// The constructor for a new account.
Account::Account(std::string userName, std::string pass, std::string email) : accountName(userName), accountPass(pass), accountEmail(email) {
	// Set dateCreated to the current time based on GMT timezone formatted as a string.
	time_t secs;
	struct tm * timeCreated;
	char dateStr[80];

	time(&secs);
	timeCreated = gmtime(&secs);
	strftime(dateStr, 80, "%F %X", timeCreated);
	dateCreated = dateStr;
	std::cout << "Account object initialized at " << dateCreated << std::endl;
	std::cout << "accountName: " << accountName << ", " << " accountPass: " << accountPass << ", " << " accountEmail: " << accountEmail << std::endl;
	// Create Account table if it doesn't exist.
	createAccountTable();

	userExists = exists();
	if (!userExists) {
		std::cout << "User does not exist!" << std::endl;
		storeAccount();
		// Assuming no errors comes up during the storing of the account details...
		std::cout << "Account created." << std::endl;
	} else {
		std::cout << "Failure creating account, username already exists." << std::endl;
	}
}


// The constructor for logging into an existing account.
Account::Account(std::string userName, std::string pass) : accountName(userName), accountPass(pass) {
	createAccountTable();

	// Check if username exists
	userExists = exists();
	std::cout << "User exists: " << userExists << std::endl;
	if (userExists) {
		// Gather info from username into struct 'details'
		findDetailsByUsername();		

		if (details.pass == accountPass) {
			std::cout << "Logging into account: " << details.name << ", " << details.pass << ", " << details.email << ", " << details.date << std::endl;
			loggedIn = true;
		}
		else {
			loggedIn = false;
		}
	} else {
		loggedIn = false;
	}
}

void Account::createAccountTable() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
	}

	// Create Account table in database.
	char const* sql =
		"CREATE TABLE IF NOT EXISTS Account ("
		"ID INTEGER PRIMARY KEY," 
		"Username TEXT NOT NULL," 
		"Password TEXT NOT NULL," 
		"Email TEXT,"
		"DateCreated TEXT NOT NULL);";
	char* errMsg;
	rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error creating table Account: %s\n", errMsg);
		sqlite3_free(errMsg);
		exit(-1);
	}
	sqlite3_close(db);
}

// Function to store a new account details in database.
void Account::storeAccount() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_stmt *stmt;
	// Prepare statement of inserting row into Account table. 
	sqlite3_prepare_v2(db, "INSERT INTO Account (Username,Password,Email,DateCreated) VALUES (?1,?2,?3,?4);", -1, &stmt, NULL);
		// Bind new account variables to statement placeholders.
	sqlite3_bind_text(stmt, 1, accountName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, accountPass.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, accountEmail.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, dateCreated.c_str(), -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr,"ERROR executing statement: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
};

bool Account::exists() {
	std::cout << "Called exists() function." << std::endl;
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_stmt *stmt;
	// Check if account name already exists.
	char sqlString[] = "SELECT COUNT(*) FROM Account WHERE Username = '";
	strcat(sqlString, accountName.c_str());
	strcat(sqlString, "';");

	rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "ERROR preparing database for exists() function: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}
	// Execute statement
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
		fprintf(stderr,"ERROR executing statement: %s\n", sqlite3_errmsg(db));
	}
	int count = sqlite3_column_int(stmt, 0);
	std::cout << "Number of users with same username existing: " << count << std::endl;
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return (count > 0);
};

// Function that uses the account's username and returns account info.
void Account::findDetailsByUsername() {
	sqlite3 *db;
	int rc = sqlite3_open(dbFile, &db);
	if (rc) {
		fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_stmt *stmt;

	char sqlString[] = "SELECT * FROM Account WHERE Username = '";
	strcat(sqlString, accountName.c_str());
	strcat(sqlString, "';'");

	rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "ERROR preparing database for finding details: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}
	rc = sqlite3_step(stmt);
	sqlite3_close(db);

	// Cast text returned by sqlite3_column_text to strings.
	std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
	std::string pass(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
	std::string email(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
	std::string date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
	
	details.name = name;
	details.pass = pass;
	details.email = email;
	details.date = date;
}
