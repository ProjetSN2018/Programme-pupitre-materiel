/*
 * shellCom.h
 *
 * Created: 27/01/2018 12:04:05
 *  Author: Thierry
 */


#define SHELL_ESC_TIMEOUT	100
#define SHELL_EDIT_TIMEOUT	0

#define SHELLCOM_TXBUF_LEN					256
#define SHELLCOM_RXBUF_LEN					256
#define SHELLCOM_ESCSEQBUF_LEN				128


uint32_t Shellcom(uint32_t sc, ...);

#define SHELLCOM_NEW			100
#define SHELLCOM_PUTC			102
#define SHELLCOM_PUTSTR			103
#define SHELLCOM_PUTSTRLEN		104

#define Putc(c)					Shellcom(SHELLCOM_PUTC,(uint32_t)c)
#define Putstr(str)				Shellcom(SHELLCOM_PUTSTR,(uint32_t)str)
#define Putstrlen(str,len)		Shellcom(SHELLCOM_PUTSTRLEN,(uint32_t)str,(uint32_t)len)

