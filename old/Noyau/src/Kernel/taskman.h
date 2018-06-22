/*
 * taskman.h
 *
 * Created: 31/01/2018 17:42:40
 *  Author: eleve
 */ 


#ifndef TASKMAN_H_
#define TASKMAN_H_

#define TASKMAN_NEW                     100
//#define TASKMAN_DEL                   101 //UNUSED, real time application
#define TASKMAN_PUSH_TASK               102
#define TASKMAN_POP_TASK                103
#define TASKMAN_DELAYED_TASK_PROCESS    104


//Taskman's settings
//Instant task buffer's length 
#define TASKMAN_TASKS_LIST_LEN          10
//Delayed task buffer's length
#define TASKMAN_DELAYED_TASKS_LIST_LEN  10




//Task structure definition
typedef struct tt_task{
	t_ptf		pFunc;
	uint32_t	sc;
	void*		pParam;
	uint32_t	delay;
}t_task;

//Concerned function prototype
uint32_t Taskman(uint32_t sc,...);

#define PushTask(pFunc,sc,param,delay)			Taskman(TASKMAN_PUSH_TASK,(uint32_t)pFunc,(uint32_t)sc,(uint32_t)param,(uint32_t)delay)
#define PopTask()								Taskman(TASKMAN_POP_TASK)

#endif /* TASKMAN_H_ */