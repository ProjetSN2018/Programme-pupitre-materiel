/*
 * shellcomm.h
 *
 * Created: 06/02/2018 14:11:22
 *  Author: eleve
 */ 


#define SHELL_ESC_TIMEOUT		10
#define SHELL_EDIT_TIMEOUT		0

#define SHELLCOM_TXBUF_LEN		256

uint32_t Shellcom(uint32_t sc, ...);

#define SHELLCOM_NEW			100
#define SHELLCOM_PUTC			102
#define SHELLCOM_PUTSTR			103
#define SHELLCOM_PUTSTRLEN		104




