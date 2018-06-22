/*
 * module.h
 *
 * Created: 09/04/2018 16:33:38
 *  Author: eleve  
 */ 
 #include <asf.h>
//#define MASTER
#define MODBUS_PUPITRE_ADDRESS			(0x20)
#define MODULE_SERIAL_NUMBER_LEN		1
#define ST_MODULE_IS_PLATINE			0x01
#define ST_MODULE_IS_DOOR_OPEN			0x02

extern struct moduleIdentification;

uint32_t Module(uint32_t sc,...);

#define MODULE_NEW						100
#define MODULE_DEL						101	
#define MODULE_IS_MODULE_IDENTIFIED		102
#define MODULE_GET_MODBUS_ADDR			103
#define MODULE_GET_SERIAl_NUMBER		104
#define MODULE_ATTACH					105
#define MODULE_INIT						106
#define MODULE_IS_PLATINE				107
#define MODULE_GET_SLAVE_SERVED			108
#define MODULE_IS_DOOR_READY			109
	

//#define	READ_COILS						 0x01

#define MODULE_INIT_DELAY			5000

#define ModuleIdentificationNew(modbusModuleAddr, pSerialNumber)\
			Module(MODULE_ATTACH, (uint32_t)modbusModuleAddr, (uint32_t)pSerialNumber)

#define ModuleDel(modbusModuleAddr, pSerialNumber)\
			Module(MODULE_DEL, (uint32_t)modbusModuleAddr, (uint32_t)pSerialNumber)

#define IsModuleIdentified()	Module(MODULE_IS_MODULE_IDENTIFIED)
#define IsModulePlatine()		Module(MODULE_IS_PLATINE)
#define GetModuleAddr()			Module(MODULE_GET_MODBUS_ADDR)
#define GetModuleServed()		Module(MODULE_GET_SLAVE_SERVED)

#define GetModuleSerialNumber()	Module(MODULE_GET_SERIAl_NUMBER)
