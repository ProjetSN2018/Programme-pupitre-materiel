/*
 * modbus_private.h
 *
 * Created: 24/03/2018 14:27:02
 *  Author: Thierry
 */

////////////MOBDUS PRIVATE MODULE DEFINITIONS //////////////////////////////////////////////////////

#define MODBUS_EXCEPTION_ERROR_FRAME_LEN		5
#define MODBUS_WRITE_SINGLE_COIL_FRAME_LEN		8
#define MODBUS_READ_COILS_FRAME_LEN				8
#define MODBUS_ATTACH_FRAME_LEN					6
#define MODBUS_SLAVE_SEND_FRAME_LEN				5
#define MODBUS_NEED_NO_ACK						false

enum {
	WRITE_MULTIPLE_REGISTERS	= 0x10,
	READ_HOLDING_REGISTERS		= 0x03,
	READ_COILS					= 0x01,
	WRITE_MULTIPLE_COILS		= 0x0F,
	WRITE_SINGLE_COIL			= 0x05,
	NETWORK_ATTACH				= 0x64,
	SLAVE_SEND					= 0x65,
	SLAVE_TEST					= 0x66,
};

struct{
	union{
		uint32_t reg;
		struct{
			uint16_t		status;
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
	uint8_t		IDSlave;

}modbus;

#define	ST_MODULE_IS_MASTER		0x01
#define IsMaster()			mIsBitsSet(modbus.status,ST_MODULE_IS_MASTER)
#define ST_FRAME_ACK_REQ		0x02
#define IsAckReq()			mIsBitsSet(modbus.status, ST_FRAME_ACK_REQ)
#define ST_REQEST				0x04
#define IsRequestON()		mIsBitsSet(modbus.status, ST_REQEST)
#define ST_SLAVE_ASK			0x08
#define IsMasterAsking()	mIsBitsSet(modbus.status, ST_SLAVE_ASK)


/// MODBUS PRIVATE SERVICE CODES //////////////////////////////////////////////
enum{
	_MODBUS_PROCESS_RECEIVED_FRAME = 1,
	_MODBUS_SLAVE_NOT_ACK,
	_MODBUS_PROCESS_ACK_FRAME,
	_MODBUS_RETURN_ERROR_CODE,
	_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_SINGLE_COIL,
	_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_MULTIPLE_REGISTER,
	_MODBUS_COMMAND_ACKNOWNLEDGING_READ_COILS,
};





