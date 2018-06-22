/*
 * shellCom.c
 *
 * Created: 27/01/2018 12:04:28
 *  Author: Thierry
 */
#include "appli.h"
#include "CRC16MODBUS.h"
#include <string.h>

#define USART_SERIAL					USART0
#define USART_SERIAL_ID					ID_USART0
#define USART_SERIAL_BAUDRATE			115200
#define USART_SERIAL_CHAR_LENGTH		US_MR_CHRL_8_BIT
#define USART_SERIAL_PARITY				US_MR_PAR_NO
#define USART_SERIAL_STOP_BIT			US_MR_NBSTOP_1_BIT

#define USART_SERIAL_TXD_PIN			PIN_USART0_TXD
#define USART_SERIAL_TXD_IDX			PIN_USART0_TXD_IDX
#define USART_SERIAL_TXD_FLAGS			PIN_USART0_TXD_FLAGS

#define USART_SERIAL_RXD_PIN			PIN_USART0_RXD
#define USART_SERIAL_RXD_IDX			PIN_USART0_RXD_IDX
#define USART_SERIAL_RXD_FLAGS			PIN_USART0_RXD_FLAGS

const sam_usart_opt_t usart_console_settings = {
	USART_SERIAL_BAUDRATE,
	USART_SERIAL_CHAR_LENGTH,
	USART_SERIAL_PARITY,
	USART_SERIAL_STOP_BIT,
	US_MR_CHMODE_NORMAL
};



char shellcomTxBuf[SHELLCOM_TXBUF_LEN];
char shellcomRxBuf[SHELLCOM_RXBUF_LEN+2];		//+2 pour pouvoir placer 2 caratères '\0' en fin de saisie de commande : nécessaire pour le traitement !

char shellcomEscSeqBuf[SHELLCOM_ESCSEQBUF_LEN];

#define ST_SHELL_MODE_INSERT	0x000001


typedef enum {
	IDLE	=0,
	ESCAPE,
	ESCAPE_SEQUENCE,
	EDITION,
	ED_ESCAPE,
	ED_ESCAPE_SEQUENCE,
	EXECUTE
}t_state;

struct {
	char*		pTxWrite;
	char*		pTxRead;
	char*		pEdit;
	char*		pEnd;
	char*		pEscSeq;
	t_state		state;
	uint32_t	escTimer;
	uint32_t	editTimer;
	uint16_t	crc;
	uint16_t	nbChar;
	uint16_t	nStatus;
} shellcom;


typedef void(*t_ptfVV)(void*);


typedef struct tt_cmdEntry{
	uint16_t	hashCode;
	t_ptfVV	pCmdFunc;
}t_cmdEntry;


const t_cmdEntry cmdFuncMap[]={
	{0xBF20, (t_ptfVV)_cmd_help	},
	{0x6CAB, (t_ptfVV)_cmd_set	},
	{0x3BED, (t_ptfVV)_cmd_cls	},
	{0x0000, NULL				}
};

const char separators[]=" :/;,\t.";

///////// PRIVATE SERVICES CODES /////////////////////////////////////////////////////////////////////////////////
enum{
	_SHELLCOM_KBHIT	= 1,
	_SHELLCOM_PUTHEXA,
	_SHELL_TIMER = 3,
	_SHELL_CHECK_SEPARATORS,
	_SHELL_CLEANUP_CMDLINE
};

#define _CheckSeparators(pSep,ch)		Shellcom(_SHELL_CHECK_SEPARATORS,(uint32_t)pSep,(uint32_t)ch)


uint32_t Shellcom(uint32_t sc, ...)
{
	uint32_t uK;
	char* pCh;
	char* pSt;
	
	switch(sc)
	{
	case SHELLCOM_NEW:

		shellcom.pTxWrite = shellcom.pTxRead = shellcomTxBuf;
		shellcom.pEdit = shellcom.pEnd = shellcomRxBuf;
		shellcom.pEscSeq = shellcomEscSeqBuf;
		shellcom.state = IDLE;
		shellcom.nbChar = 0;
		shellcom.editTimer = shellcom.escTimer = 0;
		shellcom.nStatus = 0;

		sysclk_enable_peripheral_clock(USART_SERIAL_ID);
		usart_init_rs232(USART_SERIAL, &usart_console_settings,	sysclk_get_cpu_hz());
		gpio_configure_pin(USART_SERIAL_TXD_IDX,USART_SERIAL_TXD_FLAGS);
		gpio_configure_pin(USART_SERIAL_RXD_IDX,USART_SERIAL_RXD_FLAGS);
		usart_enable_interrupt(USART_SERIAL, US_IER_RXRDY);
		NVIC_EnableIRQ(USART_SERIAL_ID);
		usart_enable_tx(USART_SERIAL);
		usart_enable_rx(USART_SERIAL);
		break;

	case SHELLCOM_PUTC:
#define ch	pa1
		*shellcom.pTxWrite++ =ch;
		if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
		if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL);
		usart_enable_interrupt(USART_SERIAL,US_IER_TXRDY);
#undef ch
		break;

	case SHELLCOM_PUTSTR:
#define pSrc	pa1
		while(*(char*)pSrc)
		{
			*shellcom.pTxWrite++ = *(char*)pSrc; 	pSrc++;
			if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
			if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL);
			usart_enable_interrupt(USART_SERIAL,US_IER_TXRDY);
		}
#undef pSrc
		break;

	case SHELLCOM_PUTSTRLEN:
#define pSrc	pa1
#define len		pa2
		while(len--)
		{
			*shellcom.pTxWrite++ = *(char*)pSrc; 	pSrc++;
			if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
			if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL);
			usart_enable_interrupt(USART_SERIAL,US_IER_TXRDY);
		}
#undef pSrc
#undef len
		break;

	//////////////////////////// PRIVATE SERVICES SECTION ///////////////////////////////////////////////////////////
	case _SHELLCOM_KBHIT:
#define kbHitChar	pa1
		switch(shellcom.state)
		{
		case IDLE:
			switch(kbHitChar)
			{
			case '\e':
				shellcom.crc = 0xFFFF;
				shellcom.pEscSeq = shellcomEscSeqBuf;
				*shellcom.pEscSeq++=kbHitChar;
				shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
				shellcom.escTimer=SHELL_ESC_TIMEOUT;
				shellcom.state=ESCAPE;
				break;
			case '\r':
			case ' ':
			case 0x08:	//backspace
			case 0x09:	//tabulation
				break;
			default:
				Putstr("\r\n>");
				shellcom.pEdit=shellcom.pEnd=shellcomRxBuf;
				*shellcom.pEdit++=kbHitChar; shellcom.pEnd++;
				shellcom.nbChar=1;
				Putc(kbHitChar);
				shellcom.editTimer=SHELL_EDIT_TIMEOUT;
				shellcom.state=EDITION;
				break;
			}
			break;
		case ESCAPE:
			*shellcom.pEscSeq++=kbHitChar;
			shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
			shellcom.escTimer=SHELL_ESC_TIMEOUT;
			shellcom.state=ESCAPE_SEQUENCE;
			break;
		case ESCAPE_SEQUENCE:
			*shellcom.pEscSeq++=kbHitChar;
			shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
			shellcom.escTimer=SHELL_ESC_TIMEOUT;
			break;
		case EDITION:
			shellcom.editTimer=SHELL_EDIT_TIMEOUT;
			switch(kbHitChar)
			{
			case 0x09:	//tabulation
				break;
			case 0x08:	//backspace
				if(shellcom.pEdit>shellcomRxBuf)
				{
#define pSrc	sc
					pSrc = (uint32_t)shellcom.pEdit;
					while(pSrc<(uint32_t)shellcom.pEnd)
					{
						*(char*)(pSrc-1) = *(char*)pSrc;
						pSrc++;
					}
#undef  pSrc
					shellcom.pEdit--;
					shellcom.pEnd--;
					shellcom.nbChar--;
					Putstr("\e[D");
#define k	sc
					Putstrlen(shellcom.pEdit,(k=shellcom.pEnd-shellcom.pEdit));
					Putc(0x20); k++;
					while(k--) Putstr("\e[D");
#undef k
				}
				break;
			case '\e':
				shellcom.crc = 0xFFFF;
				shellcom.pEscSeq = shellcomEscSeqBuf;
				*shellcom.pEscSeq++=kbHitChar;
				shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
				shellcom.escTimer=SHELL_ESC_TIMEOUT;
				shellcom.state=ED_ESCAPE;
				break;
			case '\r':
				//if (shellcom.nbChar)
				if(shellcom.pEnd>shellcomRxBuf)
				{
					shellcom.state=EXECUTE;
					PushTask(Shellcom,_SHELLCOM_KBHIT,0,0);
				}
				else
				{
					Putstr("\r \r");
					shellcom.state = IDLE;
				}
				break;
			default:
				//if(shellcom.pEdit<shellcomRxBuf+SHELLCOM_RXBUF_LEN)
				if(shellcom.nbChar<SHELLCOM_RXBUF_LEN)
				{
					if(mIsBitsSet(shellcom.nStatus,ST_SHELL_MODE_INSERT))
					{//Mode insertion
						if(shellcom.pEdit!=shellcom.pEnd)
						{
#define pSrc	sc
							pSrc = (uint32_t)shellcom.pEnd;
							while(pSrc>(uint32_t)shellcom.pEdit)
							{
								*(char*)pSrc = *(char*)(pSrc-1);
								pSrc--;
							}
#undef pSrc
							*shellcom.pEdit = kbHitChar;
#define k sc
							Putstrlen(shellcom.pEdit,(k=shellcom.pEnd-shellcom.pEdit+1));
							//k--; si on fait while(k--) //Il faut enlever 1 position sur k
							while(--k) Putstr("\e[D");
#undef k
							shellcom.pEdit++;
							shellcom.pEnd++;
							shellcom.nbChar++;
						}
						else
						{
							*shellcom.pEdit++=kbHitChar;
							shellcom.pEnd++;
							shellcom.nbChar++;
							Putc(kbHitChar);
						}
					}
					else
					{//Mode remplacement
						*shellcom.pEdit=kbHitChar;
						Putc(kbHitChar);
						if(shellcom.pEdit==shellcom.pEnd)
						{
							shellcom.pEnd++;
							shellcom.nbChar++;
						}
						shellcom.pEdit++;
					}
				}
				break;
			}
			break;

		case ED_ESCAPE:
			shellcom.editTimer=SHELL_EDIT_TIMEOUT;
			*shellcom.pEscSeq++=kbHitChar;
			shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
			shellcom.escTimer=SHELL_ESC_TIMEOUT;
			shellcom.state=ED_ESCAPE_SEQUENCE;
			break;
		case ED_ESCAPE_SEQUENCE:
			shellcom.editTimer=SHELL_EDIT_TIMEOUT;
			*shellcom.pEscSeq++=kbHitChar;
			shellcom.crc = CRC16MODBUS(kbHitChar,shellcom.crc);
			shellcom.escTimer=SHELL_ESC_TIMEOUT;
			break;
		case EXECUTE:
			uK=shellcom.pEnd-shellcom.pEdit;
			while(uK--) Putstr("\e[C");		//Placement du curseur en fin de ligne de la commande
			*shellcom.pEnd++='\0';	//char shellcomRxBuf[SHELLCOM_RXBUF_LEN+2];		//+2 pour pouvoir placer 2 caratères '\0' en fin de saisie de commande : nécessaire pour le traitement !
			*shellcom.pEnd++='\0';	//Pour empêcher d'aller chercher les caractères qui trainnent de la commande précédente !
#define token	sc
			//Putstr("\tEXECUTING\r\n");
			Shellcom(_SHELL_CLEANUP_CMDLINE);
			Putstr("\r\n");
			Putstr(shellcomRxBuf);
			Putstr("\r\n");

			token = strtok(shellcomRxBuf," \r\n");
			if(token)
			{
				shellcom.crc = CRC16MODBUSFRAME((unsigned char*)token,token=strlen((char*)token));
				uK=0;
				while((cmdFuncMap[uK].hashCode)&&(cmdFuncMap[uK].hashCode!=shellcom.crc)) uK++;
				if(cmdFuncMap[uK].pCmdFunc) cmdFuncMap[uK].pCmdFunc(shellcomRxBuf+token+1);
#undef token
				else
				{
					sprintf(buffer,"\tcmd:<%04X>\r\n",shellcom.crc)	;
					Putstr(buffer);
				}
			}
			shellcom.state = IDLE;
			break;
		default:
			Putstr("\r\n\t***** case _SHELLCOM_KBHIT: BAD case!!!\r\n");
			break;
		}
#undef kbHitChar
		break;

	case _SHELL_TIMER:
		if(shellcom.escTimer)
		{
			if(--shellcom.escTimer==0)		//ESCAPE hit timeout
			{
				switch(shellcom.state)
				{
				case ESCAPE:
					Putstr("ESC hit!\r\n");
					shellcom.state = IDLE;
					break;
				case ESCAPE_SEQUENCE:
					Putstr("Received ESC_SEQU: ");
					*shellcom.pEscSeq++='\0';
					shellcom.pEscSeq=shellcomEscSeqBuf;
					while(*shellcom.pEscSeq) Shellcom(_SHELLCOM_PUTHEXA,(uint32_t)*shellcom.pEscSeq++);
					sprintf(buffer," >%04X<\r\n",shellcom.crc);
					Putstr(buffer);
					shellcom.state = IDLE;
					break;
				case ED_ESCAPE:
					Putstr("\r\nESC hit! Abort edition.\r\n");
					shellcom.state = IDLE;
					break;
				case ED_ESCAPE_SEQUENCE:
					*shellcom.pEscSeq++='\0';
					switch(shellcom.crc)
					{
					case 0xC43A:	// Left arrow
						if(shellcom.pEdit>shellcomRxBuf)
						{
							Putstr(shellcomEscSeqBuf);
							shellcom.pEdit--;
						}
						break;
					case 0x067B:	// Right arrow
						//if(shellcom.pEdit<(shellcomRxBuf+shellcom.nbChar)-1)
						if(shellcom.pEdit<shellcom.pEnd)
						{
							Putstr(shellcomEscSeqBuf);
							shellcom.pEdit++;
						}
						break;
					case 0xC7FA:	// Up arrow
						break;
					case 0xC6BA:	// Down arrow
						break;
					case 0x6F0E:	// CTRL + Left arrow
#define pDst	sc
						pDst = (uint32_t)shellcom.pEdit;
						//if(pDst==(uint32_t)shellcom.pEnd) pDst--;

						//if((*(char*)pDst==0x20)||(pDst==(uint32_t)shellcom.pEnd))
						if(_CheckSeparators(separators,*(char*)pDst)||(pDst==(uint32_t)shellcom.pEnd))
						{
							//while((*(char*)pDst==0x20)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
							while(_CheckSeparators(separators,*(char*)pDst)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
						}
						else
						{
							//while((*(char*)pDst!=0x20)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
							while(!_CheckSeparators(separators,*(char*)pDst)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;

							//while((*(char*)pDst==0x20)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
							while(_CheckSeparators(separators,*(char*)pDst)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;

						}
						//while((*(char*)pDst!=0x20)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
						while(!_CheckSeparators(separators,*(char*)pDst)&&(pDst>(uint32_t)shellcomRxBuf)) pDst--;
						if(pDst>(uint32_t)shellcomRxBuf) pDst++;
						uK=(uint32_t)shellcom.pEdit-pDst;
						while(uK--) Putstr("\e[D");
						shellcom.pEdit=(char*)pDst;
#undef pDst
						break;
					case 0xAD4F:	// CTRL + Right arrow
#define pDst	sc
						pDst = (uint32_t)shellcom.pEdit;
						//while((pDst<(uint32_t)shellcom.pEnd)&&(*(char*)pDst!=0x20)) pDst++;
						while(!_CheckSeparators(separators,*(char*)pDst)&&(pDst<(uint32_t)shellcom.pEnd)) pDst++;
						//while((pDst<(uint32_t)shellcom.pEnd)&&(*(char*)pDst==0x20)) pDst++;
						while(_CheckSeparators(separators,*(char*)pDst)&&(pDst<(uint32_t)shellcom.pEnd)) pDst++;
						uK=pDst-(uint32_t)shellcom.pEdit;// + (pDst!=(uint32_t)shellcom.pEnd)?1:0;
						shellcom.pEdit+=uK;
						while(uK--) Putstr("\e[C");
#undef pDst
						break;
					case 0x6CCE:	// CTRL + Up arrow
						break;
					case 0x6DE8:	// CTRL + Down arrow
						Putstr("\r\n");			//Abort command
						shellcom.state = IDLE;
						break;
					case 0xC13A:	// Home arrow
						while(shellcom.pEdit>shellcomRxBuf)
						{
							Putc(0x08);
							shellcom.pEdit--;
						}
						break;
					case 0x6A0E:	// CTRL + Home arrow
						break;
					case 0x05BB:	// End arrow
						//while(shellcom.pEdit<(shellcomRxBuf+shellcom.nbChar)-1)
						while(shellcom.pEdit<shellcom.pEnd)
						{
							Putstr("\e[C");
							shellcom.pEdit++;
						}
						break;
					case 0xAE8F:	// CTRL + End arrow
						break;
					case 0xC3E3:	// Suppr
						//if(shellcom.pEdit!=(shellcomRxBuf+shellcom.nbChar))
						if(shellcom.pEdit!=shellcom.pEnd)
						{
#define pSrc sc
							pSrc = (uint32_t)shellcom.pEdit+1;
							//while(pSrc<(shellcomRxBuf+shellcom.nbChar))
							while(pSrc<(uint32_t)shellcom.pEnd)
							{
								*(char*)(pSrc-1)=*(char*)pSrc;
								Putc(*(char*)pSrc);
								pSrc++;
							}
							Putc(' ');
							shellcom.nbChar--;
							shellcom.pEnd--;
							while(pSrc!=(uint32_t)shellcom.pEdit)
							{
								Putc(0x08);
								pSrc--;
							}
#undef pSrc
						}
						break;
					case 0x63E0:	// PageUp
						break;
					case 0x93E0:	// PageDn
						break;
					case 0x53E2:	// INSER
						mBitsTgl(shellcom.nStatus,ST_SHELL_MODE_INSERT);
						break;
					}
					shellcom.state=EDITION;
					break;
				default:
					Putstr("\r\n_SHELL_TIMER:BAD STATE REACHED!!!\r\n");
					break;
				}
			}
		}
		if(shellcom.editTimer)
		{
			if(--shellcom.editTimer==0)
			{
				if(shellcom.state==EDITION)
				{
					Putstr("\r\n");
					shellcom.state = IDLE;
				}
			}
		}
		break;

	case _SHELL_CHECK_SEPARATORS:
#define pSeparators	pa1
#define ch	((char)pa2)
		while((*(char*)pSeparators)&&(*(char*)pSeparators!=ch)) pSeparators++;
		return (*(char*)pSeparators!='\0');
#undef pSeparators
#undef  ch
		break;

	case _SHELL_CLEANUP_CMDLINE:
#define pStr	sc
		//pStr=(uint32_t)shellcomRxBuf;
		////Removing front separators ////////////////////////////////////////////////////////////////////
		//while(*(char*)pStr)
		//{
			//while(_CheckSeparators(separators,*(char*)pStr)&&(pStr<(uint32_t)shellcom.pEnd)) pStr++;
		//}
		//pStr++; pCh=(char*)pStr;
		//uK=pStr-(uint32_t)pCh;
		//while(uK && pStr<(uint32_t)shellcom.pEnd)
		//{
			//*pCh++ = *(char*)pStr; pStr++;
		//}
		//shellcom.pEnd-=uK;
		//shellcom.nbChar-=uK;

		//Removing inside separators ///////////////////////////////////////////////////////////////////
		pStr=(uint32_t)shellcomRxBuf;
		while(*(char*)pStr)
		{
			//while(*(char*)pStr!=0x20) pStr++;
			while(!_CheckSeparators(separators,*(char*)pStr)&&(pStr<(uint32_t)shellcom.pEnd)) pStr++;
			
			pStr++; pSt=pCh=(char*)pStr;
			//while(*(char*)pStr==0x20) pStr++;
			while(_CheckSeparators(separators,*(char*)pStr)&&(pStr<(uint32_t)shellcom.pEnd)) pStr++;
			uK=pStr-(uint32_t)pCh;
			while(uK && pStr<(uint32_t)shellcom.pEnd)
			{
				*pCh++ = *(char*)pStr; pStr++;
			}
			shellcom.pEnd-=uK;
			shellcom.nbChar-=uK;
			pStr=(uint32_t)pSt;
		}
#undef pStr
		break;

	case _SHELLCOM_PUTHEXA:
	{
#define digit	((char)pa1)
		const char hexDigitAscii[]="0123456789ABCDEF";
		Putc(hexDigitAscii[digit>>4]);
		Putc(hexDigitAscii[digit&0x0F]);
		Putc(' ');
#undef digit
	}
		break;

	//////////////////////////// DEFAULT SECTION : SWITCH BAD SC ERROR THROW //////////////////////////////////////
	default:
		Error(ERROR_SHELLCOM_SWITCH_BAD_SC);
	}

	return 0;

}


void USART0_Handler()
{
	uint32_t dw_status = usart_get_status(USART_SERIAL);
	//if(usart_is_rx_ready(USART_SERIAL)) {

	if (dw_status & US_CSR_RXRDY)
	{
		PushTask(Shellcom,_SHELLCOM_KBHIT,USART_SERIAL->US_RHR & US_RHR_RXCHR_Msk,0);
	}

	if (dw_status & US_CSR_TXRDY)
	{
		if(shellcom.pTxRead!=shellcom.pTxWrite)
		{
			//usart_putchar(USART_SERIAL,*shellcom.pTxRead++);
			USART_SERIAL->US_THR = US_THR_TXCHR(*shellcom.pTxRead++);
			if(shellcom.pTxRead>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxRead=shellcomTxBuf;
		}
		else
		{
			usart_disable_interrupt(USART_SERIAL,US_IER_TXRDY);
		}
	}
}

