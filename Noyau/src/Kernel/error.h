/*
 * error.h
 *
 * Created: 29/01/2018 17:26:45
 *  Author: eleve
 */ 


#ifndef ERROR_H_
#define ERROR_H_

//Application's error codes
#define ERR_TASKMAN_SWITCH_BAD_SC                   11
#define ERR_SABLIER_SWITCH_BAD_SC                   12
#define ERR_SHELLCOM_SWITCH_BAD_SC					13
#define ERR_SHELL_SWITCH_BAD_SC						14


#define ERR_TASKMAN_TASKLIST_FULL			        21
#define ERR_TASKMAN_DELAYED_TASKLIST_FULL			22
#define ERR_SHELCOM_TXBUF_FULL						23

//Concerned function prototype
uint32_t ErrorLed(uint32_t err, ...);
uint32_t ErrorUART(uint32_t err, ...);


extern t_ptf32 pErrorFunc;

#define Error(errc, sc)		pErrorFunc(errc, sc)

#endif /* ERROR_H_ */