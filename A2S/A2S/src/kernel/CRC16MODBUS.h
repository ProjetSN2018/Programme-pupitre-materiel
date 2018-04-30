/* 
 * File:   CRC16MODBUSbyte.h
 * Author: eleve
 *
 * Created on 14 avril 2016, 15:40
 */

unsigned int CRC16MODBUSFRAME	(unsigned char *nData, unsigned int wLength);
unsigned int CRC16MODBUSbyte		(unsigned char nData, unsigned int wCRCWord);
unsigned int CRC16MODBUSword	(unsigned int wData, unsigned int wCRCWord);

