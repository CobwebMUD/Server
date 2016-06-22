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

// Initialize the Account constructor
Account::Account(std::string userName, std::string pass, std::string email) : accountName(userName), accountPass(pass), accountEmail(email)
{
  // Set dateCreated to a time string based on GMT timezone.
  time_t secs;
  struct tm * timeCreated;
  char dateStr[80];

  time(&secs);
  timeCreated = gmtime(&secs);
  strftime(dateStr, 80, "%F %X", timeCreated);
  dateCreated = dateStr;
  std::cout << "Account object initialized at " << dateCreated << std::endl;
  std::cout << "accountName: " << accountName << ", " << " accountPass: " << accountPass << ", " << " accountEmail: " << accountEmail << std::endl;
  
  // Call function storeAccount and store boolean.
  exists = storeAccount();
  std::cout << (exists ? "Failure creating account, username already exists." : "Account created.") << "\n";
}

// Function to store account details in database.
int Account::storeAccount() {
  sqlite3 *db;
  // Create or open game database
  char const* dbFile = "game.db";
  int rc;
  rc = sqlite3_open(dbFile, &db);
  if (rc) {
    fprintf(stderr, "Error when opening database: %s\n", sqlite3_errmsg(db));
    return(0);
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
  }

  sqlite3_stmt *stmt;
  // Check if account name already exists.
  // Prepare statement
  char sqlString[] = "SELECT COUNT(*) FROM Account WHERE Username = '";
  strcat(sqlString, accountName.c_str());
  strcat(sqlString, "';");
  rc = sqlite3_prepare(db, sqlString, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    std::cout << "ERROR preparing database: " << rc << std::endl;
    exit(-1);
  }
  // Execute statement
  rc = sqlite3_step(stmt);
  int count = sqlite3_column_int(stmt, 0);

  if (count == 0) {
    sqlite3_reset(stmt);
    // Prepare statement of inserting row into Account table. 
    sqlite3_prepare_v2(db, "INSERT INTO Account (Username,Password,Email,DateCreated) VALUES (?1,?2,?3,?4);", -1, &stmt, NULL);
    // Bind new account variables to statement placeholders.
    sqlite3_bind_text(stmt, 1, accountName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accountPass.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, accountEmail.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, dateCreated.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      std::cout << "ERROR executing statement: " << sqlite3_errmsg(db) << std::endl;
    }
  }
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return (count > 0);
};
