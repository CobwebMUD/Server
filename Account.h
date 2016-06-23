/*
 * =====================================================================================
 *
 *       Filename:  Account.h
 *
 *    Description:  Account class for maintaining accounts. 
 *
 *        Version:  1.0
 *        Created:  06/19/2016 03:41:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: 
 *   Organization:  CobwebMUD 
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
#include <string>
#include <ctime>

class Account {
	public:
		Account(std::string userName, std::string pass, std::string email);
		Account(std::string userName, std::string pass);
		bool userExists;
		bool loggedIn;
		struct accountDetails {
			std::string name;
			std::string pass;
			std::string email;
			std::string date;
		} details;
	private:
		bool exists(sqlite3 *db);
		char const* dbFile = "game.db";
		std::string accountName;
		std::string accountPass;
		std::string accountEmail;
		std::string dateCreated;
		void storeAccount(sqlite3 *db);
		void findDetailsByUsername(sqlite3 *db);
};
