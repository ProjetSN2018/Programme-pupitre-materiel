/*
 * shellcomm.c
 *
 * Created: 06/02/2018 14:11:02
 *  Author: eleve
 */ 
 #include "./Headers/appli.h"



//PARAMETRE DE COMMUNICATION DE L'UART///////////////////////////////////////////////////////////
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
 
 ///////////////////////////////////////////////////////////////////////////////////////////////////
 char shellcomTxBuf[SHELLCOM_TXBUF_LEN];
 


 ///////// Shell object model /////////////////////////////////
 struct {
	char		*pTxWrite;
	char		*pTxRead;
}shellcom;		//Unique shell object


 ////// Nicely shortcuts .............. ///////////////////////
 #define pTxWrite		shellcom.pTxWrite
 #define pTxRead		shellcom.pTxRead

 uint32_t Shellcom(uint32_t sc , ...)
 {
	switch(sc)
	{
	//ON CREE LE NOYAU////////////////////////////////////////////
	case SHELLCOM_NEW:
		//ON ATTIRBUT LES PARAMETRES DU NOYAU/////////////////////
		pTxWrite = pTxRead = shellcomTxBuf;
		/////////////////////////////////////////////////////////
		//ON CONFIGURE LE PORT SERIE/////////////////////////////
		sysclk_enable_peripheral_clock(USART_SERIAL_ID);
		usart_init_rs232(USART_SERIAL, &usart_console_settings,	sysclk_get_cpu_hz());
		gpio_configure_pin(USART_SERIAL_TXD_IDX,USART_SERIAL_TXD_FLAGS);
		gpio_configure_pin(USART_SERIAL_RXD_IDX,USART_SERIAL_RXD_FLAGS);
		usart_enable_interrupt(USART_SERIAL, US_IER_RXRDY);
		NVIC_EnableIRQ(USART_SERIAL_ID);
		usart_enable_tx(USART_SERIAL);
		usart_enable_rx(USART_SERIAL);
		/////////////////////////////////////////////////////////


		
		break;
	case SHELLCOM_PUTC:
#define _ch ((char)pa1)
		*pTxWrite++ = _ch;
		if(pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) pTxWrite=shellcomTxBuf;
		if(pTxWrite==pTxRead) Error(ERR_SHELCOM_TXBUF_FULL,SHELLCOM_PUTC);
		usart_enable_interrupt(USART_SERIAL, US_IER_TXRDY);
#undef _ch
		break;

	case SHELLCOM_PUTSTR:
#define _pStr pa1
		while(*(char*)_pStr)
		{
			*pTxWrite++ = *(char*)_pStr++;
			if(pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) pTxWrite=shellcomTxBuf;
			if(pTxWrite==pTxRead) Error(ERR_SHELCOM_TXBUF_FULL,SHELLCOM_PUTSTR);
			usart_enable_interrupt(USART_SERIAL, US_IER_TXRDY);
		}
#undef _pStr
		break;
	case SHELLCOM_PUTSTRLEN:
#define _pStr pa1
#define _len  pa2
	while(_len)
	{
		*pTxWrite++ = *(char*)_pStr++; _len--;
		if(pTxWrite>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) pTxWrite=shellcomTxBuf;
		if(pTxWrite==pTxRead) Error(ERR_SHELCOM_TXBUF_FULL,SHELLCOM_PUTSTR);
		usart_enable_interrupt(USART_SERIAL, US_IER_TXRDY);
	}
#undef _pStr
#undef _len
	break;

	///////////Shellcom private services implementation /////////////////////////////
	
	////////////////// STALL APPLICATION IF NO CASE HIT /////////////////////////////
	default:
		Error(ERR_SHELLCOM_SWITCH_BAD_SC,sc);
	}

	return 0;
 }



 enum{
	 _SHELL_KBHIT = 1
 };

 
 void USART0_Handler()
 {
	uint32_t dw_status = usart_get_status(USART_SERIAL);

	if(dw_status & US_CSR_RXRDY)
	{
		PushTask(Shell, _SHELL_KBHIT, USART_SERIAL->US_RHR & US_RHR_RXCHR_Msk, 0);
	}

	if(dw_status & US_CSR_TXRDY)
	{
				
		if(pTxRead!=pTxWrite)
		{
			USART_SERIAL->US_THR = US_THR_TXCHR(*pTxRead++);
			if(pTxRead>=shellcomTxBuf+SHELLCOM_TXBUF_LEN) pTxRead = shellcomTxBuf;
		}
		else
		{
			usart_disable_interrupt(USART_SERIAL, US_IER_TXRDY);
		}
	}
 }