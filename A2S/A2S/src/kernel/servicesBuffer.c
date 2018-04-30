/*
 * frameBuffer.c
 *
 * Created: 19/04/2018 16:00:25
 *  Author: Damien
 */ 

 
 #include "kernel.h"


uint32_t requestsBufList[SERVICES_BUF_LIST_LEN];


 struct{
	 uint32_t* pPush;
	 uint32_t* pPop;
 }requestsBuffer;
 
 extern char* buf[];
 
 uint32_t Framesbuf(uint32_t sc, ...)
 {
	 switch(sc)
	 {
	 case SERVICESBUF_NEW:
		requestsBuffer.pPop = requestsBuffer.pPush = requestsBufList;
		Putstr("\r\n\tFRAMEBUF_NEW\r\n");
		break;

	case SERVICESBUF_PUSH:
#define _sc	pa1
		Putstr("\r\n\t FRME_PUSH \r\n");
		sprintf(buf,"\r\n\tSC:%X\r\n", _sc);
		Putstr(buf);
		*requestsBuffer.pPush = _sc;
		requestsBuffer.pPush++;
		if(requestsBuffer.pPush >= requestsBufList + SERVICES_BUF_LIST_LEN) requestsBuffer.pPush = requestsBufList;
		if(requestsBuffer.pPush == requestsBuffer.pPop) Error(ERROR_FRAMESBUF_LIST_FULL, 0);
#undef _sc
		break;
	case SERVICESBUF_POP:
#define _sc		sc
		Putstr("\r\n\t FRME_POP \r\n"); 
		if(requestsBuffer.pPop != requestsBuffer.pPush)
		{
			sprintf(buf, "\r\n\tpPOP: %X", *requestsBuffer.pPop);
			Putstr(buf);
			_sc = *requestsBuffer.pPop;
			requestsBuffer.pPop++;
			if(requestsBuffer.pPop >= requestsBufList + SERVICES_BUF_LIST_LEN) requestsBuffer.pPop = requestsBufList;
			sprintf(buf,"\r\n\tSC:%X\r\n", _sc);
			Putstr(buf);
			return _sc;
		}
		Putstr("\r\n\t FRME_POP EMPTY \r\n");
		return 0;
#undef _sc	

		break;
	default:
		Error(ERROR_FRAMESBUF_SWITCH_BAD_SC, sc);

	 }
 }