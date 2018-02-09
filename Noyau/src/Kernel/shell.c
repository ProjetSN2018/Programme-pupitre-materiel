/*
 * shell.c
 *
 * Created: 06/02/2018 17:19:43
 *  Author: eleve
 */ 

#include <string.h>

#include "./Headers/appli.h"
#include "CRC16MODBUS.h"
#include "shellcmd.h"

 char shellBuf[SHELL_EDIT_BUFFER_LEN];
 char shellEscSeq[SHELL_ESCSEQ_BUFFER_LEN];
 
typedef enum { 
	IDLE = 0, 
	EDITION, 
	EXECUTION, 
	ESCAPE, 
	ESCAPE_SEQ,
	ED_ESCAPE,
	ED_ESCAPE_SEQ
} t_state;

 struct{
	t_state		state;
	uint32_t	nStatus;
	char*		pEdit;
	uint32_t	nbChar;
	uint16_t	crc;
 }shell;

const uint32_t flashSequ[]={
	50,500,50,1200,0
};
 //////Nicely shortcuts........ ///////////////////
#define nStatus		shell.nStatus
#define pEdit		shell.pEdit
#define nbChar		shell.nbChar
#define crc			shell.crc 
#define state		shell.state		


/////Shell Private services codes ////////////////////
enum{
	_SHELL_KBHIT = 1,
	_SHELL_HEARTBEAT,
};

 uint32_t Shell(uint32_t sc, ...)
 {
	 uint32_t u1;
	 
	switch(sc)
	{
	case SHELL_NEW:
		gpio_configure_pin(LED0_GPIO, LED0_FLAGS);
		gpio_set_pin_low(LED0_GPIO);
		Shellcom(SHELLCOM_NEW);
		state = IDLE;
		PushTask(Shell,_SHELL_HEARTBEAT,0,0);
		break;




	////Private services implementation ///////////////////////////////
	case _SHELL_KBHIT:
#define _kbhitChar ((char)pa1)
		switch(state)
		{
		case IDLE:
			switch(_kbhitChar)
			{
			case '\e':
				break;
			case ' ':
			case '\t':
			case '\b':
			case '\r':
			case '\n':
				break;	
			default:
				Putstr("\r\n>");
				pEdit = shellBuf;
				*pEdit++ = _kbhitChar;
				nbChar = 1;
				Putch(_kbhitChar);
				state = EDITION;
				
				break;
			}
			break;
		case ESCAPE:
			break;
		case ESCAPE_SEQ:
			break;
		case EDITION:
			switch(_kbhitChar)
			{
			case '\e':
				break;
			case '\t':
			case '\b':
			case '\n':
				break;
			case '\r':
				*pEdit++ = '\0';
				state = EXECUTION;
				PushTask(Shell, _SHELL_KBHIT, 0, 0);
				break;
			default:
				if(nbChar < SHELL_EDIT_BUFFER_LEN - 2)
				{
					*pEdit++ = _kbhitChar;
					nbChar++;
					Putch(_kbhitChar);
				}
				break;
			}
			break;
		case ED_ESCAPE:
			break;
		case ED_ESCAPE_SEQ:
			break;
		case EXECUTION:
			Putstr("\r\n>>>");
			Putstr(shellBuf);
			Putstr("\r\n");
#define pToken sc
			pToken = (uint32_t)strtok(shellBuf, " \r\n");
			if(pToken)
			{
				crc = CRC16MODBUSFRAME(pToken, strlen(pToken));

#define k u1
				k = 0;
				while(_cmdEntries[k].cmdCode && (_cmdEntries[k].cmdCode != crc)) k++;
				if(_cmdEntries[k].pCmdFunc)
				{
					_cmdEntries[k].pCmdFunc(0, shellBuf + strlen(pToken) + 1);
				}
				else
				{
					
					sprintf(buf,"\r\nunknown cmd: %04X\r\n",crc);
					Putstr(buf);
				}
#undef k
#undef pToken	
			}

			state = IDLE;
			break;
		default:
			Putch(_kbhitChar);
			break;
		}
#undef _kbhitChar
		break;
		
		

	case _SHELL_HEARTBEAT:
#define k	pa1
		k++; if(flashSequ[k]==0) k=0;
		if(k%2==0) gpio_set_pin_high(LED0_GPIO);
		else gpio_set_pin_low(LED0_GPIO);
		PushTask(Shell,_SHELL_HEARTBEAT,k,flashSequ[k]);
#undef k
		break;
		
		
	////////////////////////////////////////////////////////////////////////////////////////////////	
	default:
		Error(ERR_SHELL_SWITCH_BAD_SC,sc);
	}

	return 0;
 }
