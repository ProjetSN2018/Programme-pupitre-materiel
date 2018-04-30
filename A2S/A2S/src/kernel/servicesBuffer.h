/*
 * frameBuffer.h
 *
 * Created: 19/04/2018 16:00:38
 *  Author: Damien
 */ 


 #define SERVICES_BUF_LIST_LEN		512

 #define SERVICESBUF_NEW		101
 #define SERVICESBUF_PUSH		102
 #define SERVICESBUF_POP		103
 

uint32_t Framesbuf(uint32_t sc, ...);

#define ServicesBufferNew()	Framesbuf(SERVICESBUF_NEW)
#define ServicesBufferPop()	Framesbuf(SERVICESBUF_POP)
#define ServicesBufferPush(sc)	Framesbuf(SERVICESBUF_PUSH, (uint32_t)sc)

