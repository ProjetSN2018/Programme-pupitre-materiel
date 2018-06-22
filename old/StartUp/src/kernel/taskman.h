/*
 * taskman.h
 *
 * Created: 10/03/2018 10:15:12
 *  Author: Thierry
 */ 
uint32_t Taskman(uint32_t sc, ...);

#define TASKMAN_NEW			100
#define TASKMAN_PUSH		102
#define TASKMAN_POP			103

#define PushTask(func,sc,pa,delay)		Taskman(TASKMAN_PUSH,(uint32_t)func,(uint32_t)sc,(uint32_t)pa,(uint32_t)delay)
#define PopTask()						Taskman(TASKMAN_POP)