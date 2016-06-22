/*
 * =====================================================================================
 *
 *       Filename:  Main.cpp
 *
 *    Description:  Cobweb MUD server 
 *
 *        Version:  1.0
 *        Created:  06/12/2016 08:49:01 PM
 *       Revision:  none
 *       Compiler:  GCC
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team 
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>
#include "User.h"

// array of users logged on
std::vector<User> users;

int main() 
{
	int port = 9876;

	//server socket creation
	struct sockaddr_in socketDef;
	socketDef.sin_family = AF_INET;
	socketDef.sin_port = htons(port);
	socketDef.sin_addr.s_addr = INADDR_ANY;

	int servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//bind server socket to port
	if (bind(servSocket, (struct sockaddr*) &socketDef, sizeof(socketDef)) < 0) 
	{
		std::cout << "Error binding socket to port: " << port << std::endl;
		return -1;
	}

	//allow socket to start listening for connections 	
	listen(servSocket, 5);

	while (true)	
	{
		struct sockaddr_in clientDef;
		socklen_t clientDefLn = sizeof(clientDef);
		//block and wait to accept connection from clients
		int clientID = accept(servSocket, (struct sockaddr*) &clientDef, &clientDefLn);
		std::cout << "CLIENT CONNECTED!!! ID: " << clientID << std::endl;
		//create user object to maintain clients connection 
		User* user = new User(clientID);
		user->service();
		users.push_back(*user);
		std::cout << "Users online: " << users.size() << std::endl;
		//loop through and remove disconnected clients from users vector 
		for (int i = 0; i < users.size(); i++)
		{
			if (!users.at(i).getState())
			{
				//remove client
				//User* user = &users.at(i);
				//users.erase(users.begin() + i);
				//delete user;
			}
		}
	}
}

