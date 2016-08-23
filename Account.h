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
#include <cstring>
#include <string>
#include <ctime>

class Account {
	public:
		Account(std::string userName, std::string pass, std::string email);
		Account(std::string userName, std::string pass);
		bool exists();
		bool loggedIn;
		struct accountDetails {
			std::string name;
			std::string pass;
			std::string email;
			std::string date;
		} details;
	private:
		char const* dbFile = "data/game.db";
		std::string accountName;
		std::string accountPass;
		std::string accountEmail;
		std::string timeCreated;
		int storeAccount();
		int findDetailsByUsername(std::string user);
		int createAccountTable();
};
