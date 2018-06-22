/*
 * modbus_private.h
 *
 * Created: 24/03/2018 14:27:02
 *  Author: Thierry
 */
////////////MOBDUS PRIVATE MODULE DEFINITIONS //////////////////////////////////////////////////////

#define MODBUS_EXCEPTION_ERROR_FRAME_LEN		5
#define MODBUS_WRITE_SINGLE_COIL_FRAME_LEN		8
#define MODBUS_NEED_NO_ACK						false

enum {
	WRITE_MULTIPLE_REGISTERS	= 0x10,
	READ_HOLDING_REGISTERS		= 0x03,
	READ_COIL					= 0x01,
	WRITE_MULTIPLE_COILS		= 0x0F,
	WRITE_SINGLE_COIL			= 0x05
};

struct{
	union{
		uint32_t reg;
		struct{
			uint8_t		status;
			uint16_t	timer;
			unsigned	:8;
		};
	};
	uint16_t	crc;
	uint8_t		functionCode;
	uint8_t		byteCount;
	uint8_t		moduleAddr;
	uint8_t		destAddr;
	uint16_t	startAddr;
	uint16_t	regQty;
	uint16_t	frameLen;
}modbus;

#define	ST_MODULE_IS_MASTER		0x01
#define IsMaster()		mIsBitsSet(modbus.status,ST_MODULE_IS_MASTER)
#define ST_FRAME_ACK_REQ		0x02
#define IsAckReq()		mIsBitsSet(modbus.status, ST_FRAME_ACK_REQ)

/// MODBUS PRIVATE SERVICE CODES //////////////////////////////////////////////
enum{
	_MODBUS_PROCESS_RECEIVED_FRAME = 1,
	_MODBUS_SLAVE_NOT_ACK,
	_MODBUS_PROCESS_ACK_FRAME,
	_MODBUS_RETURN_ERROR_CODE,
	_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_SINGLE_COIL,
	_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_MULTIPLE_REGISTER,
};

typedef struct  __attribute__ ((packed)) tt_modbusFrame{
	uint8_t		moduleAddr;
	uint8_t		functionCode;
	union{
		struct  __attribute__ ((packed)){
			uint16_t	startingAddr;
			uint16_t	registerQty;
			uint8_t		byteCount;
			uint16_t	dataField[123];
		}writeMultipleRegister_header;

		struct __attribute__ ((packed)){
			uint16_t	outputAddr;
			uint16_t	outputValue;
			uint16_t	writeSingleCoil_crc;
		}writeSingleCoil_header;

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