/*
 * modbus.c
 *
 * Created: 23/03/2018 22:16:15
 *  Author: Thierry
 */

//#include "kernel.h"
#include "appli.h"
#include "CRC16MODBUS.h"

#include "modbus_private.h"

char buf[512];
 uint32_t*	const registerMap[]={
	&RTC->RTC_TIMR,		//RTC Time Register
	&RTC->RTC_CALR,		//RTC Calendar Register
};

t_modbusFrame tempFrame;

uint32_t Modbus(uint32_t sc,...)
{
	switch(sc)
	{
	case MODBUS_NEW:
#define _moduleAddr		((char)pa1)
		modbus.reg=0;
		
		if((modbus.moduleAddr=_moduleAddr)==0) 
		{
			mBitsSet(modbus.status,ST_MODULE_IS_MASTER);
			LcdPutstr("MASTER", 3, 6);
		}
		else 
		{
			sprintf(buf,"Slave ADD:%x", _moduleAddr);
			LcdPutstr(buf, 3, 3);
		}
#undef _moduleAddr
		break;

	case MODBUS_WRITE_MULTIPLE_REGISTERS: //FOR MASTER ASKING////
	
#define _slaveAddr			((char)pa1)
#define _startingAddr		((uint16_t)pa2)
#define _registerQty		((uint16_t)pa3)
#define _dataPtr			((uint16_t*)pa4)
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sc=rs485_GetBufPtr();
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->moduleAddr=_slaveAddr),0xFFFF);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=_startingAddr),modbus.crc);
		modbus.crc=CRC16MODBUSword((_pModbusTxFramePtr->writeMultipleRegister_header.startingAddr=swap16(_startingAddr)),modbus.crc);
		modbus.crc=CRC16MODBUSword((_pModbusTxFramePtr->writeMultipleRegister_header.registerQty=swap16(_registerQty)),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->writeMultipleRegister_header.byteCount=_registerQty*2),modbus.crc);
#undef _startingAddr
#define _k	pa2
		for (_k=0;_k<_registerQty;_k++)
		{
			modbus.crc=CRC16MODBUSword((_pModbusTxFramePtr->writeMultipleRegister_header.dataField[_k]=swap16(_dataPtr[_k])),modbus.crc);
		}

		_pModbusTxFramePtr->writeMultipleRegister_header.dataField[_k]=swap16(modbus.crc);
		(_slaveAddr!=MODBUS_BROADCAST_ADDRESS)?	mBitsSet(modbus.status, ST_FRAME_ACK_REQ) : mBitsClr(modbus.status, ST_FRAME_ACK_REQ);
		if(IsAckReq())
		{
			sprintf(buf, "\r\nSlave com: %x			Slave add:%x\r\n", _pModbusTxFramePtr->moduleAddr, modbus.moduleAddr);
			Putstr(buf);
			sprintf(buf, "\r\nFunc: %x\r\n", _pModbusTxFramePtr->functionCode);
			Putstr(buf);
			sprintf(buf, "\r\nIsAckReq: %d\r\n", IsAckReq());
			Putstr(buf);
		}
		rs485_Send((modbus.frameLen = _registerQty*2+9), IsAckReq());
#undef _slaveAddr
#undef _registerQty
#undef _dataPtr
#undef _pModbusTxFramePtr
#undef _k
		break;

	case MODBUS_WRITE_SINGLE_COIL:
#define _slaveAddr			((char)pa1)
#define _outputAddr			((uint16_t)pa2)
#define _outputValue		((uint16_t)pa3)
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
#define _modbusOutputAddr	modbus.startAddr
#define _modbusOutputValue	modbus.regQty
		sc=rs485_GetBufPtr();
		modbus.crc=CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr=_slaveAddr),0xFFFF);
		modbus.crc=CRC16MODBUSbyte((modbus.functionCode = _pModbusTxFramePtr->functionCode = WRITE_SINGLE_COIL), modbus.crc);
		modbus.crc=CRC16MODBUSword((_modbusOutputAddr = _pModbusTxFramePtr->writeSingleCoil_header.outputAddr= swap16(_outputAddr)),modbus.crc);
		modbus.crc=CRC16MODBUSword((_modbusOutputValue = _pModbusTxFramePtr->writeSingleCoil_header.outputValue=swap16(_outputValue)),modbus.crc);
		_pModbusTxFramePtr->writeSingleCoil_header.writeSingleCoil_crc=swap16(modbus.crc);
		mBitsSet(modbus.status, ST_FRAME_ACK_REQ);
		rs485_Send((modbus.frameLen = MODBUS_WRITE_SINGLE_COIL_FRAME_LEN),(_slaveAddr!=MODBUS_BROADCAST_ADDRESS));
#undef _slaveAddr
#undef _outputAddr
#undef _outputValue
#undef _pModbusTxFramePtr
#undef _modbusOutputAddr
#undef _modbusOutputValue
		break;

	///PRIVATE SERVICE IMPLEMENTATION //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case _MODBUS_PROCESS_RECEIVED_FRAME:
#define _pModbusRxFramePtr	((t_modbusFrame*)sc)
		if(IsMaster())
		{//MASTER
			Putstr("\r\n\tIs MASTER\r\n");
			sprintf(buf, "\r\n\tACK REQUIRED: %d", IsAckReq());
			Putstr(buf);
			if(IsAckReq())
			{
				sc=rs485_GetBufPtr();
				switch(_pModbusRxFramePtr->functionCode)
				{
				case WRITE_SINGLE_COIL:
					Putstr("\r\n\tWRITE_SINGLE_COIL_ACK_OK\r\n");
					if(_pModbusRxFramePtr->moduleAddr == 0x16)
					{
						WriteSingleCoil(0x20, 0x01, 0x01);
					}
					break;

				default:
					Error(ERROR_MODBUS_PROCESS_ACK_FRAME_SWITCH_BAD_SC, _pModbusRxFramePtr->functionCode);
				}
				mBitsClr(modbus.status, ST_FRAME_ACK_REQ);
			}
		}
		else
		{//SLAVE
			//Check the slave address or the broadcast address ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			sc=rs485_GetBufPtr();
			if((_pModbusRxFramePtr->moduleAddr!=modbus.moduleAddr)&&(_pModbusRxFramePtr->moduleAddr!=MODBUS_BROADCAST_ADDRESS))
			{
				sprintf(buf, "\r\nSlave com: %x			Slave add:%x\r\n", _pModbusRxFramePtr->moduleAddr, modbus.moduleAddr);
				Putstr(buf);
				return 0;
			}
			sprintf(buf, "\r\nSlave com: %x			Slave add:%x\r\n", _pModbusRxFramePtr->moduleAddr, modbus.moduleAddr);
			Putstr(buf);
			sprintf(buf, "\r\nFunc: %x\r\n", _pModbusRxFramePtr->functionCode);
			Putstr(buf);
			//Check the acknowledge required//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(_pModbusRxFramePtr->moduleAddr!=MODBUS_BROADCAST_ADDRESS) mIsBitsSet(modbus.status,ST_FRAME_ACK_REQ);

			//if((_pModbusRxFramePtr->writeMultipleRegister_header.registerQty < 0x0001) || (_pModbusRxFramePtr->writeMultipleRegister_header.registerQty > 0x0007)) return 03;
			///////////FUNCTION CODE PROCESSING//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			switch(_pModbusRxFramePtr->functionCode)
			{
			case WRITE_MULTIPLE_REGISTERS:
				switch(swap16(_pModbusRxFramePtr->writeMultipleRegister_header.startingAddr))
				{
				case MODBUS_SYNCRONIZATION_TOKEN:		//STARTING ADDRESS FOR MODBUS_SYNCRONIZATION_TOKEN
					//Check register addresses ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					if((_pModbusRxFramePtr->writeMultipleRegister_header.startingAddr == modbus.startAddr) &&
						((_pModbusRxFramePtr->writeMultipleRegister_header.startingAddr + _pModbusRxFramePtr->writeMultipleRegister_header.registerQty) >= modbus.startAddr + modbus.regQty))
					{
						PushTask(Modbus,_MODBUS_RETURN_ERROR_CODE,2,0);
						return 0;
					}
					//Processing the command /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					while ((RTC->RTC_SR & RTC_SR_SEC) != RTC_SR_SEC);
					RTC->RTC_CR |= (RTC_CR_UPDTIM | RTC_CR_UPDCAL) ;
					while ((RTC->RTC_SR & RTC_SR_ACKUPD) != RTC_SR_ACKUPD);
					RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
					_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]=swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]);
					_pModbusRxFramePtr->writeMultipleRegister_header.dataField[1]=swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[1]);
					_pModbusRxFramePtr->writeMultipleRegister_header.dataField[2]=swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[2]);
					_pModbusRxFramePtr->writeMultipleRegister_header.dataField[3]=swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[3]);
					*registerMap[0]=*(uint32_t*)&_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0];
					*registerMap[1]=*(uint32_t*)&_pModbusRxFramePtr->writeMultipleRegister_header.dataField[2];
					RTC->RTC_CR &= ((~RTC_CR_UPDTIM)&(~RTC_CR_UPDCAL));
					if(mIsBitsSet(modbus.status,ST_FRAME_ACK_REQ))
					{
						//PushTask(Modbus,_MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_MULTIPLE_REGISTER,0,0);
						return 0;
					}
					//_rs485_Flush();
					break;
				
				default: //BAD REGISTER ADDRESSES OR BAD REGISTER QUANTITY ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					PushTask(Modbus,_MODBUS_RETURN_ERROR_CODE,2,0);
					break;
				}
				break;

			case READ_HOLDING_REGISTERS:
				break;	
			case WRITE_SINGLE_COIL:
				gpio_toggle_pin(CMD_MOSFET);
				gpio_toggle_pin(CMD_MOT_SERRURE);
				sprintf(buf, "\r\nWRITE SINGLE COIL\r\nFunctionCode: %X\r\nOUTPUT ADDR: %X OUTPUT VALUE: %X",_pModbusRxFramePtr->functionCode, 
					swap16(_pModbusRxFramePtr->writeSingleCoil_header.outputAddr), swap16(_pModbusRxFramePtr->writeSingleCoil_header.outputValue));
				Putstr(buf);
				WriteSingleCoilAck(_pModbusRxFramePtr->functionCode, swap16(_pModbusRxFramePtr->writeSingleCoil_header.outputAddr), swap16(_pModbusRxFramePtr->writeSingleCoil_header.outputValue));
				//_rs485_Flush();
				break;
			case READ_COIL:
				//WriteMultipleRegisters(0x16, READ_COILS, 6, "Hello there!");
				_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]=swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]);
				gpio_toggle_pin(CMD_MOSFET);
				gpio_toggle_pin(CMD_MOT_SERRURE);
				sprintf(buf, "\r\n%c%c\r\n",
				(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]), swap16(_pModbusRxFramePtr->writeMultipleRegister_header.dataField[0]));
				Putstr(buf);
				PushTask(Modbus, _MODBUS_RETURN_ERROR_CODE,0 , 0);
				//_rs485_Flush();
				break;

			default:
				//FUNCTION CODE NOT SUPPORTED : RETURN EXCEPTION CODE 01 //////////////////////////////////////////////
				PushTask(Modbus,_MODBUS_RETURN_ERROR_CODE,1,0);
				break;
			}//switch(_pModbusRxFramePtr->functionCode)

#undef _pModbusRxFramePtr
		}
		break;

	case _MODBUS_RETURN_ERROR_CODE:
#define _exceptionCode pa1
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sc=rs485_GetBufPtr();
		modbus.crc=CRC16MODBUSbyte(_pModbusTxFramePtr->moduleAddr,0xFFFF);
		modbus.crc=CRC16MODBUSbyte(_pModbusTxFramePtr->functionCode|0x80,modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->errorFrame.exceptionCode=_exceptionCode),modbus.crc);
		_pModbusTxFramePtr->errorFrame.exceptionCode_crc=swap16(modbus.crc);
		rs485_Send(MODBUS_EXCEPTION_ERROR_FRAME_LEN, MODBUS_NEED_NO_ACK);
#undef _exceptionCode
		break;
	case _MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_MULTIPLE_REGISTER:
		//rs485_Send();
		break;
	case _MODBUS_COMMAND_ACKNOWNLEDGING_WRITE_SINGLE_COIL:
		Putstr("\r\n\nWRITE SINGLE COIL ACK!!\r\n");
		rs485_Send(modbus.frameLen = MODBUS_WRITE_SINGLE_COIL_FRAME_LEN, 0);
		break;
		
#undef _pModbusRxFramePtr;
		break;
	case _MODBUS_SLAVE_NOT_ACK:
#define _slaveAddr		pa1
		Putstr("\r\n\t***** MODBUS_SLAVE_NOT_ACK FROM SLAVE #");
		sprintf(buf, "%02x *****\r\n", _slaveAddr);
		Putstr(buf);
#undef _slaveAddr
		break;

	///// DEFAULT SWITCH BAD SC ERROR TRAP ////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_MODBUS_SWITCH_BAD_SC,sc);
	}
	return 0;
}