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
 *       Compiler:  GCC *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team 
 *
 * =====================================================================================
 */
#include "User.h"

User::User(int clientID) : clientID(clientID) 
{
	connected = true;
	loggedIn = false; 
}

User::~User()
{
	// on object deconstruction delete account object 
	delete account;
}

void User::uListen() 
{
	char fromUser[120];
	
	// if client is not yet logged on 
	write(clientID, "LOGIN\n", 7); 
	read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));	
	
	if (strcmp(fromUser, "NEW") == 0)
	{
		// create new account 
		// get credentials from user
		char const* prompt;

		// get user name 
		prompt = "Enter username: \n";
		write(clientID, prompt, strlen(prompt));
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		if (strcmp(fromUser, "") == 0)
		{
			return;
		}	
		std::string userName;
		userName = fromUser;

		// get password 
		prompt = "Enter password: \n";
		write(clientID, prompt, strlen(prompt));
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		if (strcmp(fromUser, "") == 0)
		{
			return;
		}
		std::string pass;
		pass = fromUser;

		// get email
		prompt = "Enter email: \n";
		write(clientID, prompt, strlen(prompt));
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		if (strcmp(fromUser, "") == 0)
		{
			return;
		}
		std::string email;
		email = fromUser;

		// create new acount
		account = new Account(userName, pass, email);
		loggedIn = account->loggedIn;
		std::cout << loggedIn << std::endl;
	}
	else if (strcmp(fromUser, "EXISTING") == 0) 
	{
		// log into existing account 
		// get username from user
		char const* prompt;
		prompt = "What is your username? \n";
		write(clientID, prompt, strlen(prompt));
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		if (strcmp(fromUser, "") == 0)
		{
			return;
		}
		std::string userName = std::string(fromUser);

		// get password from user
		prompt = "Password: \n";
		write(clientID, prompt, strlen(prompt));
		read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
		if (strcmp(fromUser, "") == 0)
		{
			return;
		}
		std::string userPass = std::string(fromUser);

		// create account object 
		account = new Account(userName, userPass);

		// verify that account exists
		if (account->userExists) 
			loggedIn = account->loggedIn;
		else
			std::cout << "Woops! User does not exist." << std::endl;
	}
	if (loggedIn)
	{
		while (true)
		{
			clearBuff(fromUser, (sizeof(fromUser) / sizeof(char)));
			// listen for intput from User
			read(clientID, fromUser, (sizeof(fromUser) / sizeof(char)));
			// Remove trailing whitespace from input
			std::string userInput = std::string(fromUser);
			trimStr(&userInput);
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
			// "" means client has lost connection with server 
			if (strcmp(recv, "") == 0)
			{
				write(gameInst, "EXIT", 4);
				break;
			}
			// If message from game script is prefixed with "RSVP" then request input from user.
			else if (strncmp(recv, "RSVP", 4) == 0) 
			{
				// wait for response from user
				clearBuff(recv, rChunkS);
				read(clientID, recv, rChunkS);
				std::cout << "from user: " << recv << std::endl;
				// "" means client has lost connection with server 
				if (strcmp(recv, "") == 0)
				{
					write(gameInst, "EXIT", 4);
					break;
				}
				// send users response to gamescript
				write(gameInst, recv, strlen(recv));
			}
		} 
		else 
		{
			std::cout << "User " << clientID << "'s game script exiting" << std::endl;
			break;
		}
	}
	close(sock);
}

void User::trimStr(std::string* str)
{
	int len = strlen(str->c_str());
	if (len > 0) 
	{
		char cStr[len];
		strcpy(cStr, str->c_str());
		int cIndex = (len - 1);
		while(cStr[cIndex] == ' ') 
		{
			cIndex--;
		}
		*str = str->substr(0, (cIndex + 1));
		cIndex = 0;
		while(cStr[cIndex] == ' ')
		{
			cIndex++;
		}
		*str = str->substr(cIndex, strlen(str->c_str()));
	}
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


