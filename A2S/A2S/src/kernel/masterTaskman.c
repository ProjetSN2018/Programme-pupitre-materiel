/*
 * masterTaskBuf.c
 *
 * Created: 28/04/2018 12:35:39
 *  Author: Damien
 */ 

#include "kernel.h"

#define MASTER_TASK_BUF_LEN		20


t_masterTask* masterTaskBuf[MASTER_TASK_BUF_LEN];
 
struct{
	t_masterTask*	pPop;
	t_masterTask*	pPush;
	t_masterTask*	pScan;
	t_masterTask*	pIndex;
	uint32_t		status;
}masterTaskman;

#define IsSasBusy()				mIsBitsSet(masterTaskman.status, ST_SAS_BUSY)

uint32_t MasterTaskman(uint32_t sc,...)
{
	switch(sc)
	{
	case MASTER_TASKMAN_NEW:
		masterTaskman.pPop = masterTaskBuf;
		masterTaskman.pPush = masterTaskBuf;
		break;
	case MASTER_TASKMAN_PUSH:
#define _sc			pa1
#define _slaveAddr	pa2
#define _priority	pa3

		masterTaskman.pScan = masterTaskman.pPush + 1;
		while((masterTaskman.pScan != masterTaskman.pPush) || (masterTaskman.pScan->sc != 0))
		{
			masterTaskman.pScan++;
			if(masterTaskman.pScan == masterTaskBuf + MASTER_TASK_BUF_LEN) { masterTaskman.pScan = masterTaskBuf ;}
		}
		if(masterTaskman.pScan == masterTaskman.pPush) Error(ERROR_MASTER_TASKMAN_BUFFER_FULL, sc);
		masterTaskman.pPush = masterTaskman.pScan + 1;
		masterTaskman.pPush->sc = _sc;
		masterTaskman.pPush->slaveAddr = _slaveAddr;
		masterTaskman.pPush->priority = _priority;
		
#undef _sc
#undef _slaveAddr
#undef _priority
		break;
		
	case MASTER_TASKMAN_POP:
		masterTaskman.pScan = masterTaskman.pPop + 1;
		while(masterTaskman.pScan != masterTaskman.pPop)
		{
			if(masterTaskman.pScan->priority == 0)
			{
				masterTaskman.pIndex == masterTaskman.pScan;
			}
			masterTaskman.pScan++;
			//return t_masterTask;
			
		}
		
		if(!IsSasBusy())
		{
			masterTaskman.pScan = masterTaskman.pPop + 1;
			while(masterTaskman.pScan != masterTaskman.pPop)
			{
				//if(masterTaskman.pScan->sc != 0) 
			}
		}
	
		break;
	default:
		Error(ERROR_MASTER_TASKMAN_SWITCH_BAD_SC, sc);
		//no break;
	}
}

