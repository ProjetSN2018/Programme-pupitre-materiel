/*
 * shellcom.h
 *
 * Created: 10/03/2018 07:12:57
 *  Author: Thierry
 */

#define SHELLCOM_TXBUF_LEN		SHELL_TXBUF_LEN

uint32_t Shellcom(uint32_t sc, ...);

#define SHELLCOM_NEW				100

#define SHELLCOM_PUTC				102
#define SHELLCOM_PUTSTR				103
#define SHELLCOM_PUTSTRLEN			104

#define Putc(c)						Shellcom(SHELLCOM_PUTC,(uint32_t)c)
#define Putstr(str)					Shellcom(SHELLCOM_PUTSTR,(uint32_t)str)
#define Putstrlen(str,len)			Shellcom(SHELLCOM_PUTSTRLEN,(uint32_t)str,(uint32_t)len)

