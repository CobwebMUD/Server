/*
 * =====================================================================================
 *
 *       Filename:  User.h
 *
 *    Description:  User class def for maintaining clients 
 *
 *        Version:  1.0
 *        Created:  06/12/2016 09:15:18 PM
 *       Revision:  none
 *       Compiler:  GCC
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team 
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <sstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include "ProcSpawn.h"
#include "Account.h"

class User {
	public:
		User(int clientID);
		~User();
		void service();
		bool getState();
	private:
		void uListen();
		void startGame();
		void clearBuff(char* buff, int size);
		void trimStr(std::string* str);
		static void* threadEntry(void* p);	
		Account* account;
		bool loggedIn;
		bool connected;
		int clientID;
};
