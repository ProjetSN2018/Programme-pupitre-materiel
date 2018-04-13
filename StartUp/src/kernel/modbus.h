/*
 * modbus.h
 *
 * Created: 23/03/2018 22:16:25
 *  Author: Thierry
 */
#define MODBUS_BROADCAST_ADDRESS				(0x7F)

#define MODBUS_SYNCRONIZATION_TOKEN				0x1000

uint32_t Modbus(uint32_t sc,...);

#define MODBUS_NEW								100
#define MODBUS_WRITE_MULTIPLE_REGISTERS			102
#define	MODBUS_WRITE_SINGLE_COIL				103

#define ModbusNew(moduleAddr)		Modbus(MODBUS_NEW,(uint32_t)moduleAddr)

#define WriteMultipleRegisters(slvAddr,regAddr,regQty,dataPtr)		Modbus(MODBUS_WRITE_MULTIPLE_REGISTERS,(uint32_t)slvAddr,(uint32_t)regAddr,(uint32_t)regQty,(uint32_t)dataPtr)

#define WriteSingleCoil(slvAddr, outputAddr, outputValue)			Modbus(MODBUS_WRITE_SINGLE_COIL, (uint32_t)slvAddr, (uint32_t)outputAddr, (uint32_t)outputValue)

#define WriteSingleCoilAck(functionCode, outputAddr, outputValue)	Modbus(_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_SINGLE_COIL, (uint32_t)functionCode, (uint32_t)outputAddr, (uint32_t)outputValue)