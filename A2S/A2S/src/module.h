/*
 * module.h
 *
 * Created: 09/04/2018 16:33:38
 *  Author: eleve
 */ 
 #include <asf.h>
#define MASTER
#define MODBUS_PUPITRE_ADDRESS			(0x16)
#define MODULE_SERIAL_NUMBER_LEN		1
extern struct moduleIdentification;

uint32_t Module(uint32_t sc,...);

#define MODULE_NEW						100
#define MODULE_DEL						101	
#define MODULE_IS_MODULE_IDENTIFIED		102
#define MODULE_GET_MODBUS_ADDR			103
#define MODULE_GET_SERIAl_NUMBER		104
#define MODULE_ATTACH					105

#define ModuleIdentificationNew(modbusModuleAddr, pSerialNumber)\
			Module(MODULE_ATTACH, (uint32_t)modbusModuleAddr, (uint32_t)pSerialNumber)

#define ModuleDel(modbusModuleAddr, pSerialNumber)\
			Module(MODULE_DEL, (uint32_t)modbusModuleAddr, (uint32_t)pSerialNumber)

#define IsModuleIdentified()	Module(MODULE_IS_MODULE_IDENTIFIED)

#define GetModuleAddr()			Module(MODULE_GET_MODBUS_ADDR)

#define GetModuleSerialNumber()	Module(MODULE_GET_SERIAl_NUMBER)
