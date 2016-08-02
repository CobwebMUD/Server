/*
 * =====================================================================================
 *
 *       Filename:  chatRoom.cpp
 *
 *    Description:  chat room server
 *
 *        Version:  1.0
 *        Created:  07/27/2016 11:30:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  cobweb team 
 *
 * =====================================================================================
 */
#include "ChatRoom.h"

ChatRoom::ChatRoom(const char* name, int port) : CRname(name), portNum(port)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		startServer();
	}	
}

void ChatRoom::startServer()
{
	// Start server
	struct sockaddr_in socketDef;
	socketDef.sin_family = AF_INET;
	socketDef.sin_port = htons(portNum);
	socketDef.sin_addr.s_addr = INADDR_ANY;

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Bind server to socket
	if (bind(sock, (struct sockaddr*) &socketDef, sizeof(socketDef)) < 0)
	{
		std::cout << "chat room '" << CRname << "' failed to bind socket" << std::endl;
	}

	listen(sock, 5);

	while(true)
	{
		struct sockaddr_in clientDef;
		socklen_t clientDefLen = sizeof(clientDef);
		int client = accept(sock, (struct sockaddr*) &clientDef, &clientDefLen);
		// Create new client thead here
		std::cout << "client connected" << std::endl;
		// Add client id to clients
		clients.push_back(client);
		clientInfo cI;
		cI.clientID = client;
		cI.clientsList = &clients;
		pthread_t thread;
		pthread_create(&thread, NULL, ChatRoom::handleClient, &cI);
	}
}

void* ChatRoom::handleClient(void* p)
{
	// get client info 
	clientInfo cI = *((clientInfo*) p);
	// Get handle name for chat room
	char name[20];
	memset(name, 0, 20);
	send(cI.clientID, "ENTER NAME", 10, 0); 
	read(cI.clientID, name, 18);
	if (strcmp(name, "") == 0)
	{
		std::cout << "client dissconnect" << std::endl;
		return NULL;	
	}
	// Add ": " to end of c string for aesthetic reasons
	strcat(name, ": ");
	// Handle client
	while (true)
	{
		// Max len of msg is macro in ChatRoom.h
		char fromUser[MSG_MAXLEN];
		memset(fromUser, 0, MSG_MAXLEN); 
		read(cI.clientID, fromUser, MSG_MAXLEN - 20);
		// If client sends "" then client disconnect
		if (strcmp(fromUser, "") == 0)
		{ 
			std::cout << "client dissconnect" << std::endl;
			break;
		}
		for (int i = 0; i < cI.clientsList->size(); i++)
		{
			// Send msg to all clients except the one who it was received from
			if (cI.clientsList->at(i) != cI.clientID)
			{ 
				// Format msg with name at beginning
				char temp[MSG_MAXLEN];
				memset(temp, 0, MSG_MAXLEN);
				strcpy(temp, name);
				strcat(temp, fromUser);
				memset(fromUser, 0, MSG_MAXLEN);
				strncpy(fromUser, temp, MSG_MAXLEN);
				send(cI.clientsList->at(i), fromUser, MSG_MAXLEN, 0);
			}
		}
	}
	// Remove client from clientsList vector
	for (int i = 0; i < cI.clientsList->size(); i++)
	{
		if (cI.clientsList->at(i) == cI.clientID)
		{
			cI.clientsList->erase(cI.clientsList->begin() + i);
		}
	}
	return NULL;
}

