/* 
 * File:   CRC16MODBUS.h
 * Author: eleve
 *
 * Created on 14 avril 2016, 15:40
 */

unsigned int CRC16MODBUSFRAME (unsigned char *nData, unsigned int wLength);
unsigned int CRC16MODBUS (unsigned char nData, unsigned int wCRCWord);
