/*
 * =====================================================================================
 *
 *       Filename:  User.cpp
 *
 *    Description:  class for maintain clients 	 
 *
 *        Version:  1.0 
 *        Created:  06/12/2016 10:03:35 PM
 *       Revision:  none
 *       Compiler:  GCC
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team 
 *
 * =====================================================================================
 */
#include "User.h"

User::User(int clientID) : clientID(clientID) 
{
	connected = true;
	testAccount = new Account("user", "pass", "email");
}

void User::uListen() 
{
	char fromUser[120];
	while (true)
	{
		clearBuff(fromUser, 120);
		//listen for intput from User
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		// Remove trailing whitespace from input
		std::string userInput = std::string(fromUser);
		boost::trim(userInput);

		if (userInput == "GAMESTART") 
		{
			startGame(); 
		}
		else if (userInput == "EXIT")
		{
			std::cout << "User logged off!!! ClientID: " << clientID << std::endl;
			break;
		}
		else if (strcmp(fromUser, "") == 0)
		{	
			std::cout << "Server lost connection with User!!! ClientID: " << clientID << std::endl;
			break;
		}
	}	
}

// start python game script and handle communication with UDS / IPC
void User::startGame()
{
	// socket domain ".sock + clientID"
	char socketP[50];
	char cClientID[10];
	sprintf(cClientID, "%d", clientID);
	clearBuff(socketP, 50);
	strcat(socketP, "gameScripts/.sock");
	strcat(socketP, cClientID);
	strcat(socketP, "\0");
	// UD socket creation
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	std::cout << socketP << std::endl;
	strncpy(addr.sun_path, socketP, strlen(socketP));
	unlink(socketP);
	// bind socket to domain
	if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		std::cout << "failed to bind game server to ./sock" << clientID << std::endl;
	} 
	// listen for incoming connections from newly spawned game script
	listen(sock, 1);
	// start game script
	char* sGCMD = new char[100];
	strcpy(sGCMD, "python gameScripts/test.py ");
	strcat(sGCMD, cClientID);
	ProcSpawn pSpawn(sGCMD);
	// accept connection
	int gameInst = accept(sock, NULL, NULL);
	int rChunkS = 1000;
	char recv[rChunkS];
	while (true)
	{
		clearBuff(recv, rChunkS);
		// read data from game script 
		read(gameInst, recv, rChunkS);
		if (strcmp(recv, "<EXIT>") != 0)
		{	
			// send the output from the game script to the user
			std::cout << "from gameScript: " << recv << std::endl;
			write(clientID, recv, strlen(recv));
			// wait for response from user
			clearBuff(recv, rChunkS);
			read(clientID, recv, rChunkS);
			std::cout << "from user: " << recv << std::endl;
			// send users response to gamescript
			write(gameInst, recv, strlen(recv));
		} 
		else 
		{
			std::cout << "User " << clientID << "'s game script exiting" << std::endl;
			break;
		}
	}
	close(sock);
}

void User::clearBuff(char* buff, int size)
{
	memset(buff, 0, size);
}

void User::service()
{
	pthread_t thread;
	pthread_create(&thread, NULL, User::threadEntry, this);
}

void* User::threadEntry(void* p)
{
	// cast void pointer to pointer of type User
	User* parent;
	/*
	 * note this keyword returns pointer to instance of object so *(&p) <- '*' is dereferencing 
	 * this before casting to pointer of type User
	 */
	parent = ((User*) *(&p));
	// start listening loop on this glorious new thread
	parent->uListen();
	/*
	 * when uListen() returns client must have exited or lost connection
	 * so set connection to false so server can know to delete this object <- so sad :( 
	 */
	parent->connected = false;
	return NULL;
}

bool User::getState()
{
	return connected;
}
