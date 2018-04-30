/*
 * module.c
 *
 * Created: 10/04/2018 16:39:03
 *  Author: eleve
 */ 

 #include "appli.h"
 #include "./kernel/CRC16MODBUS.h"
 #include "./kernel/lcd.h"
 #include <string.h>
 extern char* buf[];
struct {
	uint8_t		modbusModuleAddr;
	//uint8_t		moduleSerialNumber[MODULE_SERIAL_NUMBER_LEN];
	uint8_t		moduleSerialNumber;
	uint16_t	moduleCRC;
}moduleIdentification = {
	0xFF,
	//{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0xFF,
	0xFFFF
};

uint32_t Module(uint32_t sc,...)
{
	switch(sc)
	{
	case MODULE_NEW:
		Putstr("\r\n\t MODULE NEW!!!\r\n");
#define _crc					sc
#ifdef MASTER
		Putstr("\r\n\t IFDEF MASTER!!!\r\n");
		_crc = CRC16MODBUSbyte((moduleIdentification.modbusModuleAddr = 0x00), 0xFFFF);
		//if((defaultList = (malloc(sizeof(t_module*)))) == NULL) { return - 1;}
		//defaultList[0].modbusModuleAddr		= NULL;
		//defaultList[0].moduleSerialNumber	= NULL;
		//defaultList[0].moduleCRC			= NULL;
		//moduleRegister.listModule = defaultList;
		//sprintf(buf, "\r\n\t MODULE REGISTER.LIST MODULE->ID: %X", moduleRegister.listModule[moduleRegister.nbModule]);
		//Putstr(buf);
#else
		Putstr("\r\n\t ELSE!!!\r\n");
		_crc = CRC16MODBUSbyte((moduleIdentification.modbusModuleAddr = 0x20), 0xFFFF);
#endif
		_crc = CRC16MODBUSbyte((moduleIdentification.moduleSerialNumber =  0xFF), _crc);
		moduleIdentification.moduleCRC = _crc;
#undef _crc
	break;

	case MODULE_ATTACH:
#define _modbusModuleAddr		((uint8_t)pa1)
#define _pModuleSerialNumber	((uint8_t)pa2)
#define _crc					sc
		_crc = CRC16MODBUSbyte((moduleIdentification.modbusModuleAddr = _modbusModuleAddr), 0xFFFF);
#undef _modbusModuleAddr
#define _k						pa1
		//for(_k = 0; _k <= MODULE_SERIAL_NUMBER_LEN; _k++)
		//{
		//_crc = CRC16MODBUSbyte((moduleIdentification.moduleSerialNumber[_k] = _pModuleSerialNumber[_k]), _crc);
		//}
		_crc = CRC16MODBUSbyte((moduleIdentification.moduleSerialNumber =  _pModuleSerialNumber), _crc);
		moduleIdentification.moduleCRC = _crc;
		sprintf(buf, "\r\n\tMODULE ADRESSE: %X\tSERIAL NUMBER: %X", moduleIdentification.modbusModuleAddr, moduleIdentification.moduleSerialNumber);
		Putstr(buf);
		sprintf(buf, "MODULE ADRESSE: %X", moduleIdentification.modbusModuleAddr);
		LcdPutstr("                    ", 3, 0);
		LcdPutstr(buf, 3, LcdFindHalf(strlen(buf)));
#undef _pModuleSerialNumber
#undef _k
		break;
	case MODULE_IS_MODULE_IDENTIFIED:
		return (moduleIdentification.moduleCRC != 0xFFFF);
		//no break;

	case MODULE_GET_MODBUS_ADDR:
		return (uint32_t)moduleIdentification.modbusModuleAddr;
		//no break;

	case MODULE_GET_SERIAl_NUMBER:
		return (uint32_t)&moduleIdentification.moduleSerialNumber;
		//no break;

	default:
		Error(ERROR_MODULE_SWITCH_BAD_SC, sc);
	}

	return 0;
} 