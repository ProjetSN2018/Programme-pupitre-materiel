/*
 * shellcom.c
 *
 * Created: 10/03/2018 07:13:14
 *  Author: Thierry
 */

#include "kernel.h"

//#define USART_SERIAL					USART0
//#define USART_SERIAL_ID					ID_USART0
//#define USART_SERIAL_BAUDRATE			115200
//#define USART_SERIAL_CHAR_LENGTH		US_MR_CHRL_8_BIT
//#define USART_SERIAL_PARITY				US_MR_PAR_NO
//#define USART_SERIAL_STOP_BIT			US_MR_NBSTOP_1_BIT
//
//#define USART_SERIAL_TXD_PIN			PIN_USART0_TXD
//#define USART_SERIAL_TXD_IDX			PIN_USART0_TXD_IDX
//#define USART_SERIAL_TXD_FLAGS			PIN_USART0_TXD_FLAGS
//
//#define USART_SERIAL_RXD_PIN			PIN_USART0_RXD
//#define USART_SERIAL_RXD_IDX			PIN_USART0_RXD_IDX
//#define USART_SERIAL_RXD_FLAGS			PIN_USART0_RXD_FLAGS

enum{
	PAR_EVEN = 0,
	PAR_ODD,
	PAR_SPACE,
	PAR_MARK,
	PAR_NO
};
enum{
	CHMODE_NORMAL = 0,
	CHMODE_AUTOMATIC,
	CHMODE_LOCAL_LOOPBACK,
	CHMODE_REMOTE_LOOPBACK
};

#define UART_MR_PAR_POS			9
#define UART_MR_CHMODE_POS		14

const sam_uart_opt_t uart_opt={
	BOARD_FREQ_MAINCK_XTAL*8,
	UART_BAUDRATE,
	PAR_NO<<UART_MR_PAR_POS | CHMODE_NORMAL<<UART_MR_CHMODE_POS
	//US_MR_PAR(US_MR_PAR_NO) //| US_MR_CHMODE(US_MR_CHMODE_NORMAL)
};

#define UART_TX_PIN					(PIO_PA12_IDX)
#define UART_TX_PIN_FLAGS			(PIO_TYPE_PIO_PERIPH_A | PIO_DEFAULT)

#define UART_RX_PIN					(PIO_PA11_IDX)
#define UART_RX_PIN_FLAGS			(PIO_TYPE_PIO_PERIPH_A | PIO_PULLUP)

extern const char hexDigits[];
char shellcomTxBuf[SHELLCOM_TXBUF_LEN];

struct{
	char	*pTxWrite;
	char	*pTxRead;
}shellcom;

uint32_t Shellcom(uint32_t sc, ...)
{
	switch(sc)
	{
	case SHELLCOM_NEW:
		shellcom.pTxRead=shellcom.pTxWrite=shellcomTxBuf;
		pmc_enable_periph_clk(ID_UART);
		pio_configure_pin(UART_TX_PIN,UART_TX_PIN_FLAGS);
		pio_configure_pin(UART_RX_PIN,UART_RX_PIN_FLAGS);
		if(uart_init(UART,&uart_opt)) Error(ERROR_SHELLCOM_UART_INIT_FAILED);
		NVIC_EnableIRQ(ID_UART);
		while(uart_is_rx_ready(UART)) uart_read(UART,(uint8_t*)&sc);
		uart_get_status(UART);	
		uart_enable_interrupt(UART,US_IER_RXRDY);
		break;

	case SHELLCOM_PUTC:
#define _charData	((char)pa1)
		*shellcom.pTxWrite++=_charData;
		if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
		if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL, (uint32_t)0);
		uart_enable_interrupt(UART,US_IER_TXEMPTY);
		return 1;
#undef _charData
		//no break;
	case SHELLCOM_PUTSTR:
#define _pStr	pa1
#define _nChar	sc
		_nChar=0;
		while(*(char*)_pStr)
		{
			*shellcom.pTxWrite++=*(char*)_pStr++; _nChar++;
			if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
			if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL, (uint32_t)0);
			uart_enable_interrupt(UART,US_IER_TXEMPTY);
		}
		return _nChar;
#undef _pStr
#undef _nChar
		//no break;
	case SHELLCOM_PUTSTRLEN:
#define _pStr	pa1
#define _strLen	pa2
#define _nChar	sc
		_nChar=0;
		while(_strLen)
		{
			*shellcom.pTxWrite++=*(char*)_pStr++; _strLen--; _nChar++;
			if(shellcom.pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxWrite=shellcomTxBuf;
			if(shellcom.pTxWrite==shellcom.pTxRead) Error(ERROR_SHELLCOM_TX_BUFFER_FULL, (uint32_t)0);
			uart_enable_interrupt(UART,US_IER_TXEMPTY);
		}
		return _nChar;
#undef _pStr
#undef _strLen
#undef _nChar
		//no break;

	case SHELLCOM_PUTHEXACODE:
#define _pStr	pa1
#define _strLen	pa2
#define _nChar	sc
		_nChar=0;
		while(_strLen)
		{
			Putch(hexDigits[(*(char*)_pStr)>>4]);
			Putch(hexDigits[(*(char*)_pStr)&0x0F]);
			Putch(' ');
			 _pStr++; _strLen--; _nChar++;
		}
		return _nChar;
#undef _pStr
#undef _strLen
#undef _nChar
		//no break;
	/////// INVALID SC CODE TRAP ERROR /////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_SHELLCOM_SWITCH_BAD_SC, (uint32_t)sc);
	}
	return 0;
}

enum{
	_SHELL_KBHIT = 2
};

void UART_Handler()
{
	uint32_t dw_status = uart_get_status(UART);
	//TX section //////////////////////////////////////////////////////////////////////////////////
	if(mIsBitsSet(dw_status,US_CSR_TXEMPTY))
	{
		if(shellcom.pTxRead!=shellcom.pTxWrite)
		{
			UART->UART_THR = *shellcom.pTxRead++;
			if(shellcom.pTxRead>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) shellcom.pTxRead=shellcomTxBuf;
		}
		else
		{
			uart_disable_interrupt(UART,US_IER_TXEMPTY);
		}
	}

	//RX section //////////////////////////////////////////////////////////////////////////////////
	if(mIsBitsSet(dw_status,US_CSR_RXRDY))
	{
		PushTask(Shell,_SHELL_KBHIT,UART->UART_RHR,0);
	}
}