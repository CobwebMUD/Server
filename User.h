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
#include <boost/algorithm/string.hpp>
#include <regex>
#include "ProcSpawn.h"
#include "Account.h"

class User {
	public:
		User(int clientID);
		void service();
		bool getState();
    Account* account;
	private:
		void uListen();
		void startGame();
		void clearBuff(char* buff, int size);
		static void* threadEntry(void* p);	
		bool connected;
		int clientID;
};
