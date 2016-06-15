/*
 * =====================================================================================
 *
 *       Filename:  ProcSpawn.cpp
 *
 *    Description:  Spawns process on new thread
 *
 *        Version:  1.0
 *        Created:  06/15/2016 01:44:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rice Shelley
 *   Organization:  Cobweb dev team 
 *
 * =====================================================================================
 */

#include "ProcSpawn.h"

ProcSpawn::ProcSpawn(char* cmd) : cmd(cmd)
{
	pthread_t thread;
	pthread_create(&thread, NULL, ProcSpawn::threadEntry, this);
}

void ProcSpawn::executeTask()
{
	//execute cmd
	system(cmd);	
}

void* ProcSpawn::threadEntry(void* p)
{
	((ProcSpawn*) p)->executeTask();
	return NULL;
}
