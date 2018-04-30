/*
 * modbus.h
 *
 * Created: 23/03/2018 22:16:25
 *  Author: Thierry
 */

#define MODBUS_BROADCAST_ADDRESS		(0x7F)

#define MODBUS_SYNCRONIZATION_TOKEN		0x1000


typedef struct  __attribute__ ((packed)) tt_modbusFrame{
	uint8_t		moduleAddr;
	uint8_t		functionCode;
	union{
		struct  __attribute__ ((packed)){
			uint8_t	startingAddr;
			uint16_t	registerQty;
			uint8_t		byteCount;
			uint16_t	dataField[123];
		}writeMultipleRegister_header;

		struct __attribute__ ((packed)){
			uint8_t		slaveFunc;
			uint16_t	slaveSend_crc;
			uint16_t	;
			uint8_t		;
		}slaveSend_headers;
		struct __attribute__ ((packed)){
			uint16_t	outputAddr;
			uint16_t	outputValue;
			uint16_t	writeSingleCoil_crc;
		}writeSingleCoil_header;

		struct __attribute__ ((packed)){
			uint16_t		startingAddr;
			uint16_t		coilsQty;
			uint16_t		readCoils_crc;

		}readCoils_header;

		struct __attribute__ ((packed)){
			uint8_t			slaveID;
			uint8_t			slaveSerialNumber;
			uint16_t		attach_crc;
			uint16_t		;
		}attach_header;

		struct  __attribute__ ((packed)){
			uint8_t		exceptionCode;
			uint16_t	exceptionCode_crc;
			uint8_t		;
			uint16_t	dummy[123];
		}errorFrame;

		struct __attribute__ ((packed)){
			uint16_t	ack_crc;
		};
	};
}t_modbusFrame;


uint32_t Modbus(uint32_t sc,...);

#define MODBUS_NEW								100
#define MODBUS_WRITE_MULTIPLE_REGISTERS			102
#define	MODBUS_WRITE_SINGLE_COIL				103
#define MODBUS_READ_COILS						104
#define MODBUS_TEST								105

#define MODBUS_SLAVE_SEND						110




#define MODBUS_ATTACH							201



#define ModbusNew()		Modbus(MODBUS_NEW)

#define WriteMultipleRegisters(slvAddr,regAddr,regQty,dataPtr)\
		Modbus(MODBUS_WRITE_MULTIPLE_REGISTERS,(uint32_t)slvAddr,(uint32_t)regAddr,(uint32_t)regQty,(uint32_t)dataPtr)

#define WriteSingleCoil(slvAddr, outputAddr, outputValue)\
		Modbus(MODBUS_WRITE_SINGLE_COIL, (uint32_t)slvAddr, (uint32_t)outputAddr, (uint32_t)outputValue)

#define WriteSingleCoilAck(functionCode, outputAddr, outputValue)\
		Modbus(_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_SINGLE_COIL, (uint32_t)functionCode, (uint32_t)outputAddr, (uint32_t)outputValue)

#define ReadCoils(slvAddr, startingAddr, CoilsQty)\
		Modbus(MODBUS_READ_COILS, (uint32_t)slvAddr, (uint32_t)startingAddr, (uint32_t)CoilsQty)

#define ReadCoilsAck(byteCount, coilsStatus)\
		Modbus(_MODBUS_COMMAND_ACKNOWNLEDGING_READ_COILS, (uint32_t)byteCount, (uint32_t)coilsStatus)

#define ModbusAtach()\
		Modbus(MODBUS_ATTACH)










