/*
 * =====================================================================================
 *
 *       Filename:  chatRoom.h
 *
 *    Description:  chat room server
 *
 *        Version:  1.0
 *        Created:  07/27/2016 11:19:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  cobweb team
 *
 * =====================================================================================
 */
#ifndef CHATROOM_H
#define CHATROOM_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <vector>

// max len of chat room messages
#define MSG_MAXLEN 128

class ChatRoom {
	
	public:
		ChatRoom(const char* name, int port);
	private:
		std::vector<int> clients;
		struct clientInfo {
			int clientID;
			std::vector<int>* clientsList;
		};
		void startServer();
		static void* handleClient(void* p);
		const char* CRname;
		int portNum;

};
#endif
