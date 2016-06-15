/*
 * =====================================================================================
 *
 *       Filename:  ProcSpawn.h
 *
 *    Description: Spawns process on new thread
 *
 *        Version:  1.0
 *        Created:  06/15/2016 01:38:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team
 *
 * =====================================================================================
 */

#include <iostream>
#include <pthread.h>

class ProcSpawn 
{
	public:
		ProcSpawn(char* cmd);
	private:
		char* cmd;	
		void executeTask();
		static void* threadEntry(void* p);
		
};
