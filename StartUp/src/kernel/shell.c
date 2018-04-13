/*
 * shell.c
 *
 * Created: 10/03/2018 11:09:26
 *  Author: Thierry
 */
#include "kernel.h"
#include "CRC16MODBUS.h"
#include "utils.h"
#include <string.h>

///////// PRIVATE SERVICES CODES //////////////////////////////////////////
enum{
	_SHELL_HEART_BEAT = 1,
	_SHELL_KBHIT = 2,
	_SHELL_TIMER = 3,
	_SHELL_PROMPT = 4,
	_SHELL_PUT_ASCII_TO_HEX,
	_SHELL_CHECK_SEPARATORS,
	_SHELL_CLEANUP_CMDLINE
};

#define _PutAscii2Hex(str,len)		Shell(_SHELL_PUT_ASCII_TO_HEX,(uint32_t)str,(uint32_t)len)
#define _IsSeparator(ch,pSep)       Shell(_SHELL_CHECK_SEPARATORS,(uint32_t)ch,(uint32_t)pSep)
#define _CleanUp_cmd_Line()         Shell(_SHELL_CLEANUP_CMDLINE)

const char separators[]=" .;:/\\\t\r\n";
const char hexDigits[]="0123456789ABCDEF";
const uint32_t heartBeatRhythm[]={ 40,254,40,1352,0 };
const char restartTimeOut[]="3";

char shellEditBuf[SHELL_EDITBUF_LEN+1];
char shellEscapeBuf[SHELL_ESCSEQBUF_LEN];
char shellTmpBuf[64];

typedef enum {
	IDLE = 0,
	ESCAPE,
	ESCAPE_SEQ,
	EDITION,
	ED_ESCAPE,
	ED_ESCAPE_SEQ,
	EXECUTION
}t_state;

struct{
	uint16_t	status;		//Must be in first position : use in shellcmd.c
	/**********************************************************************/
	char*		pEdit;
	char*		pEscbuf;
	uint16_t	nChar;
	uint16_t	nEscChar;
	uint16_t	crc;
	uint32_t	editTimer;
	uint32_t	escapeTimer;
	t_state		state;
}shell;

#define ST_SHELL_INSERT_MODE			0x000001
#define ST_SHELL_SHOW_UNKNOW_CRC		0x000002
#define ST_SHELL_PROMPT_DISABLED        0x000004

#define EnterEditMode()					mBitsSet(shell.status,ST_SHELL_PROMPT_DISABLED)
#define ExitEditMode()					mBitsClr(shell.status,ST_SHELL_PROMPT_DISABLED)

uint32_t Shell(uint32_t sc, ...)
{
	uint32_t u1,u2;
	char*pSt;

	switch(sc)
	{
	case SHELL_NEW:
		shell.editTimer=shell.escapeTimer=0;
		shell.state=IDLE;
		shell.status=ST_SHELL_SHOW_UNKNOW_CRC;
		Lcd(LCD_NEW);
		gpio_configure_pin(HEART_LED, HEART_LED_FLAGS);
		gpio_configure_pin(POWER_LED, POWER_LED_FLAGS);

#if POWER_LED_ACTIVE_LEVEL
		gpio_set_pin_low(POWER_LED);
#else
		gpio_set_pin_high(POWER_LED);
#endif
		kRtc(KRTC_NEW);
		Shellcom(SHELLCOM_NEW);
		//LcdPutstr("Hello World!",0,0);
		//LcdPutstr("Hello World!",1,1);
		//LcdPutstr("Hello World!",2,2);
		//LcdPutstr("Hello World!",3,3);
		PushTask(Shell,_SHELL_HEART_BEAT,-1,0);
		PushTask(Shell,_SHELL_PROMPT,0,0);
		break;

	////////// PRIVATE SERVICES IMPLEMENTATION //////////////////////////////////////////////////////////
	case _SHELL_HEART_BEAT:
#define k pa1
		if(heartBeatRhythm[++k]==0) k=0;
		if((k%2==0)^(!HEART_LED_ACTIVE_LEVEL)) gpio_set_pin_high(HEART_LED);
		else gpio_set_pin_low(HEART_LED);
		PushTask(Shell,_SHELL_HEART_BEAT,k,heartBeatRhythm[k]);
#undef k
		break;

	case _SHELL_PROMPT:
#define _hour	sc
#define _minute	pa1
#define _second	u1
		rtc_get_time(RTC,&_hour,&_minute,&_second);
		if(mIsBitsClr(shell.status,ST_SHELL_PROMPT_DISABLED))
		{
			sprintf(shellTmpBuf,"\r%02d:%02d:%02d",(int)_hour,(int)_minute,(int)_second);
			Putstr(shellTmpBuf);
		}
		if(IsLcdOn())
		{
			sprintf(shellTmpBuf,"%02d:%02d:%02d",(int)_hour,(int)_minute,(int)_second);
			LcdPutstr(shellTmpBuf,1,6);
#undef _hour
#undef _minute
#undef _second
#define _year	sc
#define _month	pa1
#define _date	u1
#define _day	u2
			rtc_get_date(RTC,&_year,&_month,&_date,&_day);
			sprintf(shellTmpBuf,"%02d/%02d/%04d",(int)_date,(int)_month,(int)_year);
			LcdPutstr(shellTmpBuf,0,5);
#undef _year
#undef _month
#undef _date
#undef _day			
		}
		break;

	case _SHELL_KBHIT:
#define _kbhitchar	((char)pa1)
		switch(shell.state)
		{
		case IDLE:
			switch(_kbhitchar)
			{
			case '\e':	//escape
				shell.escapeTimer=SHELL_ESC_TIMEOUT;
				shell.pEscbuf=shellEscapeBuf;
				*shell.pEscbuf++='\e';
				shell.nEscChar=1;
				shell.crc=CRC16MODBUSbyte('\e',0xFFFF);
				shell.state=ESCAPE;
				break;
			case '\b':	//backspace : 0x08
			case ' ' :	//space
			case '\t':	//tabulation : 0x09
			case '\n':
			case '\r':
				break;	//do nothing
			default:
				EnterEditMode();
				shell.editTimer=SHELL_EDIT_TIMEOUT;
				shell.pEdit=shellEditBuf;
				*shell.pEdit++=_kbhitchar;
				shell.nChar=1;
				Putstr("\r\n>"); Putch(_kbhitchar);
				shell.state=EDITION;
				break;
			}
			break;
		case ESCAPE:
				shell.state=ESCAPE_SEQ;
				//no break here to continue with ESCAPE_SEQ case !
		case ESCAPE_SEQ:
				shell.escapeTimer=SHELL_ESC_TIMEOUT;
				*shell.pEscbuf++=_kbhitchar;
				shell.nEscChar++;
				shell.crc=CRC16MODBUSbyte(_kbhitchar,shell.crc);
			break;

		case EDITION:
			shell.editTimer=SHELL_EDIT_TIMEOUT;
			switch(_kbhitchar)
			{
			case '\e':	//escape
				shell.escapeTimer=SHELL_ESC_TIMEOUT;
				shell.editTimer=0;
				shell.pEscbuf=shellEscapeBuf;
				*shell.pEscbuf++='\e';
				shell.nEscChar=1;
				shell.crc=CRC16MODBUSbyte('\e',0xFFFF);
				shell.state=ED_ESCAPE;
				break;

			case '\t':	//tabulation
			{
				const char ctrl_right_arrow[]="\e[1;5C";
				memcpy(shellEscapeBuf,ctrl_right_arrow,6);
				shell.nEscChar=6;
				shell.state=ED_ESCAPE_SEQ;
				shell.escapeTimer=1;	//Ask a ESC SEQUENCE timeout : treat as a "ctrl+right_arrow" key
			}
				break;

			case '\b':	//backspace : 0x08
                if(shell.nChar)
                {
#define pEnd    sc
#define pSrc    u1
	                pEnd = (uint32_t)shellEditBuf+shell.nChar;
	                pSrc = (uint32_t)shell.pEdit;
	                while(pSrc<pEnd)
	                {
		                *(char*)(pSrc-1)=*(char*)pSrc; pSrc++;
	                }
	                pSrc--;
	                *(char*)pSrc++=' ';
	                Putstr("\e[D");
	                shell.pEdit--; shell.nChar--;
#undef pSrc
#define k       u1
	                k=Putstrlen(shell.pEdit,shell.nChar-(shell.pEdit-shellEditBuf)+1);
	                while(k--) Putstr("\e[D");
#undef pEnd
#undef k
                }
				break;
			case '\n':
			case '\r':
				if(shell.nChar==0)
				{
					ExitEditMode();
					shell.state=IDLE;
					Putch('\r');
				}
				else
				{
					shell.editTimer=0;
					shell.state=EXECUTION;
					PushTask(Shell,_SHELL_KBHIT,'\r',0);
				}
				break;
			default:
				if(shell.nChar==0)
				{
					if(_kbhitchar!=0x20) //Excluding space char
					{
						Putch((*shell.pEdit++=_kbhitchar)); shell.nChar++;
					}
				}
				else if(shell.nChar<(SHELL_EDITBUF_LEN))
				{
					if(mIsBitsClr(shell.status,ST_SHELL_INSERT_MODE))
					{//Replace mode
						if(shell.pEdit==shellEditBuf+shell.nChar)	shell.nChar++;
						Putch((*shell.pEdit++=_kbhitchar));
					}
					else
					{//Insert mode
						if(shell.pEdit!=shellEditBuf+shell.nChar)
						{
#define pDst	sc			//Shift right chars one position on right
							pDst=(uint32_t)shellEditBuf+shell.nChar;
							while(pDst-1>=(uint32_t)shell.pEdit)
							{
								*(char*)pDst=*(char*)(pDst-1); pDst--;
							}
#undef pDst
						}
#define k sc
						Putch((*shell.pEdit++=_kbhitchar)); shell.nChar++;
						k=Putstrlen(shell.pEdit,(shellEditBuf+shell.nChar-shell.pEdit));
						while(k--) Putstr("\e[D");
#undef k
					}
				}
				break;
			}
			break;
		case EXECUTION:
#define k u1
			k=shell.nChar-(shell.pEdit-shellEditBuf);
			while(k--) Putstr("\e[C");
			shell.pEdit=shellEditBuf+shell.nChar;
			*shell.pEdit++='\0';
			_CleanUp_cmd_Line();
			shell.pEdit=shellEditBuf+shell.nChar;
			*shell.pEdit++='\0';
			Putstr("\r\n/>");
			Putstr(shellEditBuf);
#define pToken	sc
			mstrtokReset();
			pToken=mstrtok(shellEditBuf,(char*)" \r\n");
			if(pToken)
			{
				shell.crc=CRC16MODBUSFRAME((unsigned char*)pToken,strlen((char*)pToken));
				k=0;
				while((commands[k].cmdCode)&&(commands[k].cmdCode!=shell.crc)) k++;
				if(commands[k].pCmdFunc)
				{
					commands[k].pCmdFunc((uint32_t)&shell,(void*)(pToken+strlen((char*)pToken)+1));
					mstrtokRestore();
				}
				else
				{
					k=0;
                    while((cmdEntries[k].cmdCode)&&(cmdEntries[k].cmdCode!=shell.crc)) k++;
                    if(cmdEntries[k].pCmdFunc)
                    {
	                    cmdEntries[k].pCmdFunc((uint32_t)&shell,(void*)(pToken+strlen((char*)pToken)+1));
						mstrtokRestore();
						Putstr("\r\n");
                    }
                    else if(mIsBitsSet(shell.status,ST_SHELL_SHOW_UNKNOW_CRC))
                    {
	                    sprintf(shellTmpBuf,"\tunknown cmd: %04X\r\n",shell.crc);
	                    Putstr(shellTmpBuf);
						mstrtokRestore();
                    }
				}
			}
#undef k
			Putstr("\r\n");
			shell.state=IDLE;
			ExitEditMode();
			break;

		case ED_ESCAPE:
			shell.state=ED_ESCAPE_SEQ;
			//no break here to continue with ED_ESCAPE_SEQ case !
		case ED_ESCAPE_SEQ:
			shell.escapeTimer=SHELL_ESC_TIMEOUT;
			*shell.pEscbuf++=_kbhitchar;
			shell.nEscChar++;
			shell.crc=CRC16MODBUSbyte(_kbhitchar,shell.crc);
			break;

		default:
			Putstr("\r\n\t***** case _SHELL_KBHIT: BAD case! ******\r\n");
			shell.state=IDLE;
			ExitEditMode();
			break;

		}
#undef _kbhitchar
		break;

	case _SHELL_TIMER:
		if(shell.escapeTimer)
		{
			if(--shell.escapeTimer==0)
			{
				switch(shell.state)
				{
				case IDLE:
					break;
				case ESCAPE:
					shell.state=IDLE;
					break;
				case ESCAPE_SEQ:
					shell.crc=CRC16MODBUSFRAME((unsigned char*)shellEscapeBuf,shell.nEscChar);
					switch(shell.crc)
					{
					case 0xC7F5:	//ctrl+shift+up_arrow
						PushTask(_cmd_restart,&shell,restartTimeOut,0);
						//shell.state=IDLE;
						break;
					case 0xC7FA:	//up_arrow
						if(shell.nChar)
						{
							EnterEditMode();
							shell.editTimer=SHELL_EDIT_TIMEOUT;
							shell.pEdit=shellEditBuf+shell.nChar;
							Putstr("\r\n");
							Putstrlen(shellEditBuf,shell.nChar);
							shell.state=EDITION;
						}
						else
						{
							shell.state=IDLE;
						}
						break;
					default:
						if(mIsBitsSet(shell.status,ST_SHELL_SHOW_UNKNOW_CRC))
						{
							Putstr("\r\n   ESC_SEQ: ");
							_PutAscii2Hex(shellEscapeBuf,shell.nEscChar);
							sprintf(shellTmpBuf," <%04X>\r\n",shell.crc);
							Putstr(shellTmpBuf);
						}
						shell.state=IDLE;
						break;
					}//switch(shell.crc)
					break;
				case ED_ESCAPE:
					//Putstr("\r\n");
					Putstr("\e[2K\r");
					shell.state=IDLE;
					ExitEditMode();
					break;
				case ED_ESCAPE_SEQ:
					shell.crc=CRC16MODBUSFRAME((unsigned char*)shellEscapeBuf,shell.nEscChar);
					switch(shell.crc)
					{
                    case 0xC43A:    //left arrow
						if(shell.pEdit>shellEditBuf)
						{
							Putstr("\e[D");
							shell.pEdit--;
						}
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
                    case 0x067B:    //right arrow
						if(shell.pEdit<(shellEditBuf+shell.nChar))
						{
							Putstr("\e[C");
							shell.pEdit++;
						}
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
					case 0xC6BA:	//down arrow
						Putstr("\e[2K\r");
						ExitEditMode();
						shell.state=IDLE;
						break;
					case 0xC7F5:    //ctrl+shift+up_arrow
						//PushTask(_cmd_restart,&shell,0,0);
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
                    case 0xC3E3:    //suppr key
#define pSrc    sc
						pSrc=(uint32_t)shell.pEdit+1;
						while(pSrc<(uint32_t)shellEditBuf+shell.nChar)
						{
							*(char*)(pSrc-1)=*(char*)pSrc; pSrc++;
						}
						*(char*)(pSrc-1)=' ';
#define k sc
	                    k=Putstrlen(shell.pEdit,pSrc-(uint32_t)shell.pEdit);
#undef pSrc
		                while(k--) Putstr("\e[D");
#undef k
			            shell.nChar--;
						shell.editTimer=SHELL_EDIT_TIMEOUT;
				        shell.state=EDITION;
					    break;
                    case 0x53E2:    //insert key
						mBitsTgl(shell.status,ST_SHELL_INSERT_MODE);
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
                    case 0xC13A:    //home key
#define k   sc
						k=(shell.pEdit-shellEditBuf);
						while(k--) Putstr("\e[D");
						shell.pEdit=shellEditBuf;
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
                    case 0x05BB:    //end key
						k=((shellEditBuf+shell.nChar)-shell.pEdit);
						while(k--) Putstr("\e[C");
						shell.pEdit=shellEditBuf+shell.nChar;
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
#undef k
						break;
                    case 0xCCBA:    //shiht+tabulate
                    case 0x6F0E:    //ctrl+left arrow
						if((shell.nChar&&(shell.pEdit==shellEditBuf+shell.nChar)) ||
						(((shell.pEdit-1)>=shellEditBuf)&&(_IsSeparator(*(shell.pEdit-1),separators))&&(!_IsSeparator(*shell.pEdit,separators))))
						{
							shell.pEdit--; Putstr("\e[D");
						}
						if(_IsSeparator(*shell.pEdit,separators))
						{
							while((_IsSeparator(*shell.pEdit,separators))&&(shell.pEdit>shellEditBuf))
							{
								shell.pEdit--;
								Putstr("\e[D");
							}
						}
						while(((shell.pEdit-1)>=shellEditBuf)&&(!_IsSeparator(*(shell.pEdit-1),separators)))
						{
							shell.pEdit--;
							Putstr("\e[D");
						}
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
                    case 0xAD4F:    //ctrl+right arrow
						if(!_IsSeparator(*shell.pEdit,separators))
						{
							while((!_IsSeparator(*shell.pEdit,separators))&&(shell.pEdit<shellEditBuf+shell.nChar))
							{
								shell.pEdit++;
								Putstr("\e[C");
							}
						}
						while((_IsSeparator(*shell.pEdit,separators))&&(shell.pEdit<shellEditBuf+shell.nChar))
						{
							shell.pEdit++;
							Putstr("\e[C");
						}
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
					default:
						shell.editTimer=SHELL_EDIT_TIMEOUT;
						shell.state=EDITION;
						break;
					}//switch(shell.crc)
					break;

				default:
					Error(ERROR_SHELL_TIMER_ESC_BAD_SATE,shell.state);
				}
			}
		}

        if(shell.editTimer)
        {
	        if(--shell.editTimer==0)
	        {
	        }
        }
		break;

	case _SHELL_PUT_ASCII_TO_HEX:
#define str	pa1
#define len pa2
		while(len--)
		{
			Putch(hexDigits[*(char*)str>>4]);
			Putch(hexDigits[*(char*)str&0x0F]);
			Putch(' ');
			str++;
		}
#undef str
#undef len
		break;

	case _SHELL_CHECK_SEPARATORS:
#define _ch     ((char)pa1)
#define _pSep   pa2
	    while((*(char*)_pSep)&&(*(char*)_pSep!=_ch)) _pSep++;
	    return (uint32_t)*(char*)_pSep;
#undef _ch
#undef _pSep
	    break;

	case _SHELL_CLEANUP_CMDLINE:
#define pSrc       sc
#define k u1
		//Removing front separators //////////////////////////////////////////////////////////////////////
		pSrc=(uint32_t)shellEditBuf;
		while(_IsSeparator(*(char*)pSrc,separators)) pSrc++;
		k=pSrc-(uint32_t)shellEditBuf;
		if(k)
		{
			shell.pEdit=shellEditBuf;
			while(pSrc<(uint32_t)shellEditBuf+shell.nChar)
			{
				*shell.pEdit++=*(char*)pSrc++;
			}
		}
		pSrc=(uint32_t)shellEditBuf+k;
		shell.nChar-=k;

		//Removing inside separators /////////////////////////////////////////////////////////////////////
		while(pSrc<(uint32_t)shellEditBuf+shell.nChar)
		{
			while((pSrc<(uint32_t)shellEditBuf+shell.nChar)&&(!_IsSeparator(*(char*)pSrc,separators))) pSrc++;
			if(pSrc<(uint32_t)shellEditBuf+shell.nChar)
			{
				pSrc++; pSt=shell.pEdit=(char*)pSrc;
				while((pSrc<(uint32_t)shellEditBuf+shell.nChar)&&(_IsSeparator(*(char*)pSrc,separators))) pSrc++;
				k=pSrc-(uint32_t)shell.pEdit;
				while(k && pSrc<(uint32_t)shellEditBuf+shell.nChar)
				{
					*shell.pEdit++=*(char*)pSrc++;
				}
				shell.nChar-=k;
				pSrc=(uint32_t)pSt;
			}
		}
#undef pSrc
#undef k
		break;

	/////// INVALID SC CODE TRAP ERROR /////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_SHELL_SWITCH_BAD_SC,sc);
	}
	return 0;
}