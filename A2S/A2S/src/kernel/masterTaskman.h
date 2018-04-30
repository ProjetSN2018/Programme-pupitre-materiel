/*
 * masterTaskBuf.h
 *
 * Created: 28/04/2018 12:35:59
 *  Author: Damien
 */ 

#define MASTER_TASKMAN_NEW		101
#define MASTER_TASKMAN_PUSH		103
#define MASTER_TASKMAN_POP		104


#define ST_SAS_BUSY				0x01

#define ST_EMERGENCY			0x02

typedef struct tt_masterTask{
	uint32_t	sc;
	uint8_t		slaveAddr;		
	uint8_t		priority;
	}t_masterTask;
	
uint32_t MasterTaskman(uint32_t sc,...);