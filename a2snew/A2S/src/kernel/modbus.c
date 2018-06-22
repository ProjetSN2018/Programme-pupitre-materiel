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

extern uint32_t * flashLoadBuffer[];
char buf[512];
 uint32_t*	const registerMap[]={
	&RTC->RTC_TIMR,		//RTC Time Register
	&RTC->RTC_CALR,		//RTC Calendar Register
};

uint32_t slaves[] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	///////////////////////////////////////////////////////////
	NULL
};
enum{
	_COMRS485_TIMER = 1,
	_COMRS485_FLUSH
};


uint32_t Modbus(uint32_t sc,...)
{
	switch(sc)
	{
	case MODBUS_NEW:
		
		ServicesBufferNew();
		modbus.addIndex = 0;
		modbus.comIndex = 0;
		modbus.reg=0;
		modbus.IDSlave = 0x01;
		modbus.networkReady = 0;
		modbus.iIndex = 1;
		modbus.doorServed = 0x00;
		
		//if((modbus.moduleAddr=(uint8_t)GetModuleAddr())==0) 
		//{
			//mBitsSet(modbus.status,ST_MODULE_IS_MASTER);
			//
			//str("MASTER", 3, 6);
		//}
		//else 
		//{
			//sprintf(buf,"Slave ADD:%x", 0xFF);
			//LcdPutstr(buf, 3, 3);
		//}
		//if(IsMaster())
		//{
		////Modbus(MODBUS_SLAVE_SEND);
		//}
		
		
#ifdef MASTER
		if(Flash(FLASH_LOAD) == 1)
		{
			for(int i = 0; i < flashLoadBuffer[1]; i++)
			{
				Putstr("\r\n\t!\r\n");
				slaves[i] = flashLoadBuffer[START_ID_SLAVE_REGISTER + i];
				sprintf(buf, "\r\n\tslves[%d] = %d\r\n", i, slaves[i]);
				Putstr(buf);
			}
			PushTask(Modbus,MODBUS_NETWORK_VALIDATION, 0, 5000);
		}
		else
		{
			Modbus(MODBUS_ATTACH);
		}	

#else
		if(Flash(FLASH_LOAD) == 1)
		{
			Module(MODULE_ATTACH, flashLoadBuffer[START_ID_SLAVE_REGISTER], 0);
		
		}
#endif
		sprintf(buf, "\r\n\tATTACH READY: %d\r\n", mIsBitsSet(modbus.status, ST_ATTACH_READY));
		Putstr(buf);
		break;
	
	case MODBUS_COM_RESTOR:
		if(mIsBitsSet(modbus.status, ST_MASTER_ASK))
		{
			Putstr("\r\n\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
			mBitsClr(modbus.status, ST_MASTER_ASK);
		}
		mBitsClr(modbus.status, ST_REQEST);

		break;

	case SLAVE_ADD_REQUEST:
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sc = rs485_GetBufPtr();
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->moduleAddr=00),0xFFFF);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=NETWORK_ATTACH),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.slaveID = 0),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.doorServed=0),modbus.crc);
		_pModbusTxFramePtr->attach_header.attach_crc = swap16(modbus.crc);
		mBitsSet(modbus.status, ST_REQEST);
		rs485_Send((modbus.frameLen = MODBUS_ATTACH_FRAME_LEN), 0);
#undef _pModbusTxFramePtr
		break;

	case MODBUS_ATTACH:
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sprintf(buf, "\r\n\tIsRequestON:%d \r\n\tIsNetworkInitDone:%d\r\n", IsRequestON(), IsNetworkInit());
		Putstr(buf);
		if((!IsRequestON()) && (!IsNetworkInit()))
		{
			sc=rs485_GetBufPtr();
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->moduleAddr=MODBUS_BROADCAST_ADDRESS),0xFFFF);
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=NETWORK_ATTACH),modbus.crc);
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.slaveID = 0),modbus.crc);
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.doorServed=0),modbus.crc);
			_pModbusTxFramePtr->attach_header.attach_crc = swap16(modbus.crc);
			rs485_Send((modbus.frameLen = MODBUS_ATTACH_FRAME_LEN), 0);

			PushTask(Modbus, MODBUS_ATTACH, 0, 5000);
		}
		else if((IsRequestON()) && (!IsNetworkInit()))
		{
			PushTask(Modbus, MODBUS_ATTACH, 0, 500);
		}
#undef _pModbusTxFramePtr
		break;

	case MODBUS_NETWORK_VALIDATION:
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		
		sprintf(buf,"\r\n\t\t\tNETWORK VALIDATION INDEX: %d", modbus.comIndex);
		Putstr(buf);
		sc=rs485_GetBufPtr();
		modbus.crc=CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr = MODBUS_BROADCAST_ADDRESS),0xFFFF);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=NETWORK_VALIDATION),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.slaveID = 0),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.doorServed = 0), modbus.crc);
		_pModbusTxFramePtr->attach_header.attach_crc = swap16(modbus.crc);
		rs485_Send((modbus.frameLen = MODBUS_ATTACH_FRAME_LEN), 0);
		sprintf(buf, "\r\n\tVALIDATION:::::IsRequestON:%d \r\n\tIsNetworkInitDone:%d\r\n", IsRequestON(), IsNetworkInit());
		Putstr(buf);
		//mBitsClr(modbus.status,ST_REQEST);
		//mBitsClr(modbus.status, ST_MASTER_ASK);
		NetworkInitDone();
		sprintf(buf, "\r\n\tVALIDATION:::::IsRequestON:%d \r\n\tIsNetworkInitDone:%d\r\n", IsRequestON(), IsNetworkInit());
		Putstr(buf);
		PushTask(Modbus, MODBUS_SLAVE_SEND, 0, 1500);
#undef _pModbusTxFramePtr
		
		break;

	case MODBUS_SLAVE_SEND:
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)

			sprintf(buf, "\r\n\tST_MASTER_ASKING: %d \r\n\tST_REQUEST_ON:%d\r\n", IsMasterAsking(), IsRequestON());
			Putstr(buf);
			if(!IsRequestON() && !IsMasterAsking())
			{
				Putstr("\r\n\there\r\n");
				mBitsSet(modbus.status, ST_MASTER_ASK);
				sc=rs485_GetBufPtr();
				modbus.crc=CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr=slaves[modbus.comIndex]),0xFFFF);
				modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=SLAVE_SEND),modbus.crc);
				modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->slaveSend_headers.slaveFunc=0x00),modbus.crc);
				modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->slaveSend_headers.doorserved=0x00),modbus.crc);
				_pModbusTxFramePtr->slaveSend_headers.slaveSend_crc = swap16(modbus.crc);
				mBitsSet(modbus.status, ST_FRAME_ACK_REQ);
				if(slaves[modbus.comIndex] == 00) 
				{
					modbus.comIndex = 0;
				}
				else 
				{ 
					modbus.comIndex++;
				}

				if(modbus.destAddr == 0x00)
				{
					modbus.sc = ServicesBufferPop();
					mBitsClr(modbus.status, ST_MASTER_ASK);
					mBitsClr(modbus.status, ST_FRAME_ACK_REQ);
					Modbus(_MODBUS_COMMAND_INTERPRETER, modbus.sc, modbus.destAddr);
				}
				else
				{
					rs485_Send((modbus.frameLen = MODBUS_SLAVE_SEND_FRAME_LEN),IsAckReq());
				}
			}
			PushTask(Modbus, MODBUS_SLAVE_SEND, 0, 1000);
#undef _pModbusTxFramePtr
		break;

	case MODBUS_ADD_DISTRIBUTION:
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sc=rs485_GetBufPtr();
		modbus.destAddr = modbus.IDSlave;
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->moduleAddr=MODBUS_BROADCAST_ADDRESS),0xFFFF);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->functionCode=MODBUS_ADD_DISTRIBUTION),modbus.crc);
		modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.slaveID = modbus.IDSlave),modbus.crc);
		if(modbus.iIndex == 0)
		{
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.doorServed=MODBUS_IS_DOOR),modbus.crc);
			modbus.doorServed = modbus.destAddr;
		}
		else 
		{
			modbus.crc=CRC16MODBUSbyte((_pModbusTxFramePtr->attach_header.doorServed=modbus.doorServed),modbus.crc);
		}
		_pModbusTxFramePtr->attach_header.attach_crc = swap16(modbus.crc);
		rs485_Send((modbus.frameLen = MODBUS_ATTACH_FRAME_LEN), 0);
		modbus.iIndex++;
		if(modbus.iIndex > MODBUS_PLATINE_PER_DOOR)
		{
			modbus.iIndex = 0;

		}
#undef _pModbusTxFramePtr
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

	case MODBUS_READ_COILS:
#define _slaveAddr			((char)pa1)
#define _startingAddr		((uint16_t)pa2)
#define _coilsQty			((uint16_t)pa3)
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
		sc=rs485_GetBufPtr();
		modbus.crc = CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr = _slaveAddr), 0xFFFF);
		modbus.crc = CRC16MODBUSbyte((modbus.functionCode = _pModbusTxFramePtr->functionCode = READ_COILS), modbus.crc);
		modbus.crc = CRC16MODBUSword((modbus.startAddr = _pModbusTxFramePtr->readCoils_header.startingAddr = swap16(_startingAddr)), modbus.crc);
		modbus.crc = CRC16MODBUSword((modbus.regQty = _pModbusTxFramePtr->readCoils_header.coilsQty = swap16(_coilsQty)), modbus.crc);
		_pModbusTxFramePtr->readCoils_header.readCoils_crc = swap16(modbus.crc);
		mBitsSet(modbus.status, ST_FRAME_ACK_REQ);
		rs485_Send((modbus.frameLen = MODBUS_READ_COILS_FRAME_LEN),(_slaveAddr!=MODBUS_BROADCAST_ADDRESS));
#undef _pModbusTxFramePtr
#undef _slaveAddr
#undef _startingAddr
#undef	_coilsQty
		break;


	case MODBUS_WRITE_SINGLE_COIL:
#define _slaveAddr			((char)pa1)
#define _outputAddr			((uint16_t)pa2)
#define _outputValue		((uint16_t)pa3)
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
#define _modbusOutputAddr	modbus.startAddr
#define _modbusOutputValue	modbus.regQty
		if(_slaveAddr == 0x00)
		{
			if(Module(MODULE_IS_DOOR_READY))
			{
				Putstr("\r\n\tDOOR OPEN\r\n");
				gpio_toggle_pin(CMD_RELAIS_1);

			}
			else
			{
				Putstr("\r\n\tDOOR NOT READY");
			}
			mBitsClr(modbus.status, ST_REQEST);
		}
		else
		{
			sc=rs485_GetBufPtr();
			modbus.crc=CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr=_slaveAddr),0xFFFF);
			modbus.crc=CRC16MODBUSbyte((modbus.functionCode = _pModbusTxFramePtr->functionCode = WRITE_SINGLE_COIL), modbus.crc);
			modbus.crc=CRC16MODBUSword((_modbusOutputAddr = _pModbusTxFramePtr->writeSingleCoil_header.outputAddr= swap16(0)),modbus.crc);
			modbus.crc=CRC16MODBUSword((_modbusOutputValue = _pModbusTxFramePtr->writeSingleCoil_header.outputValue=swap16(0)),modbus.crc);
			_pModbusTxFramePtr->writeSingleCoil_header.writeSingleCoil_crc=swap16(modbus.crc);
			mBitsSet(modbus.status, ST_FRAME_ACK_REQ);
			rs485_Send((modbus.frameLen = MODBUS_WRITE_SINGLE_COIL_FRAME_LEN),(_slaveAddr!=MODBUS_BROADCAST_ADDRESS));
		}
		
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
		sc=rs485_GetBufPtr();
		//sprintf(buf, "\r\n\t\t SC:%X\r\n", _pModbusRxFramePtr->functionCode);
		//Putstr(buf);
		//sprintf(buf, "\r\nIS MASTER: %d", IsMaster());
		//Putstr(buf);
		if(IsMaster())
		{//MASTER
			Putstr("\r\n\tIs MASTER\r\n");
			sprintf(buf, "\r\n\tACK REQUIRED: %d\r\n", IsAckReq());
			Putstr(buf);
			if(_pModbusRxFramePtr->functionCode == NETWORK_ATTACH) 
			{
				mBitsSet(modbus.status, ST_REQEST);
				Modbus(MODBUS_ADD_DISTRIBUTION);
				
			}
			else if(_pModbusRxFramePtr->functionCode == MODBUS_ADD_DISTRIBUTION)
			{
				if(_pModbusRxFramePtr->attach_header.slaveID == modbus.IDSlave)
				{
					Putstr("\r\n\t\tINIT SUCCESFUL!!!!!!!!!\r\n");
					slaves[modbus.addIndex] = modbus.IDSlave;
					modbus.IDSlave++;
					modbus.addIndex++;
				}
				mBitsClr(modbus.status, ST_REQEST);
			}
			
			if(IsAckReq())
			{
				
				switch(_pModbusRxFramePtr->functionCode)
				{
				case WRITE_SINGLE_COIL:
					mBitsClr(modbus.status, ST_REQEST);
					if(_pModbusRxFramePtr->writeSingleCoil_header.outputValue == 0)
					{
						Putstr("\r\n\tDOOR SLAVE NOT READY\r\n");
					}
					else
					{
						Putstr("\r\n\tDOOR SLAVE READY\r\n");
						
					}
					Putstr("\r\n\tWRITE_SINGLE_COIL_ACK_OK\r\n");
					//Modbus(MODBUS_SLAVE_SEND, i);
					break;
				
				case READ_COILS:
					mBitsClr(modbus.status, ST_REQEST);
					
					Putstr("\r\n\tREAD_COILS_ACK_OK\r\n");
					//Modbus(MODBUS_SLAVE_SEND, i);
					break;

				case NETWORK_ATTACH:
					sprintf(buf, "\r\n\tNew slave ID: %X\r\n", _pModbusRxFramePtr->attach_header.slaveID);
					Putstr(buf);
					ComRS485(_COMRS485_FLUSH);
					//ModbusAtach();
					break;
				case SLAVE_SEND:
					//sprintf(buf, "\r\nJE DEMANDE LA FONCTION: %X \r\n", _pModbusRxFramePtr->slaveSend_headers.slaveFunc);
					//Putstr(buf);
					Modbus(_MODBUS_COMMAND_INTERPRETER, _pModbusRxFramePtr->slaveSend_headers.slaveFunc, _pModbusRxFramePtr->slaveSend_headers.doorserved);
					break;
				default:
					Error(ERROR_MODBUS_PROCESS_ACK_FRAME_SWITCH_BAD_SC, _pModbusRxFramePtr->functionCode);
				}
						
			}
		}
		else
		{//SLAVE
			//Check the slave address or the broadcast address ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			sc=rs485_GetBufPtr();
			if((_pModbusRxFramePtr->moduleAddr!=GetModuleAddr())&&(_pModbusRxFramePtr->moduleAddr!=MODBUS_BROADCAST_ADDRESS))
			{
				//sprintf(buf, "\r\nSlave com: %x			Slave add:%x\r\n", _pModbusRxFramePtr->moduleAddr, GetModuleAddr());
				//Putstr(buf);
				return 0;
			}
			//sprintf(buf, "\r\nSlave com: %x			Slave add:%x\r\n", _pModbusRxFramePtr->moduleAddr, GetModuleAddr());
			//Putstr(buf);
			//sprintf(buf, "\r\nFunc: %x\r\n", _pModbusRxFramePtr->functionCode);
			//Putstr(buf);
			//Check the acknowledge required//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(_pModbusRxFramePtr->moduleAddr!=MODBUS_BROADCAST_ADDRESS) mBitsSet(modbus.status,ST_FRAME_ACK_REQ);

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
			case NETWORK_ATTACH:
				modbus.timer = MODBUS_TIMER_VALUE;
				Modbus(_MODBUS_TIMER);
				
				
				break;
				
			case MODBUS_ADD_DISTRIBUTION:
				if(IsRequestON())
				{
					Module(MODULE_ATTACH, _pModbusRxFramePtr->attach_header.slaveID, _pModbusRxFramePtr->attach_header.doorServed);
					gpio_set_pin_low(POWER_LED);
					mBitsClr(modbus.status, ST_ATTACH_READY);
					modbus.timer = 0;
					modbus.IDSlave = _pModbusRxFramePtr->attach_header.slaveID;
					modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->moduleAddr=0x00),0xFFFF);
					modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->functionCode=MODBUS_ADD_DISTRIBUTION),modbus.crc);
					modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->attach_header.slaveID = GetModuleAddr()),modbus.crc);
					modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->attach_header.doorServed),modbus.crc);
					_pModbusRxFramePtr->attach_header.attach_crc = swap16(modbus.crc);
					mBitsClr(modbus.status, ST_REQEST);

					rs485_Send(modbus.frameLen = MODBUS_ATTACH_FRAME_LEN, 0);
				}
				break;

			case NETWORK_VALIDATION:
				NetworkInitDone();
				break;


			case SLAVE_SEND:
				//Putstr("\r\n\tSLAVE SNED!!!\r\n\n");
				modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->moduleAddr),0xFFFF);
				modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->functionCode=SLAVE_SEND),modbus.crc);
				modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->slaveSend_headers.slaveFunc=ServicesBufferPop()),modbus.crc);
				modbus.crc=CRC16MODBUSbyte((_pModbusRxFramePtr->slaveSend_headers.doorserved = GetModuleServed()),modbus.crc);
				_pModbusRxFramePtr->slaveSend_headers.slaveSend_crc = swap16(modbus.crc);
				rs485_Send((modbus.frameLen = MODBUS_SLAVE_SEND_FRAME_LEN), 0);
				break;


			case READ_HOLDING_REGISTERS:
				break;
				
					
			case WRITE_SINGLE_COIL:
#define _slaveAddr			((char)pa1)
#define _outputAddr			((uint16_t)pa2)
#define _outputValue		((uint16_t)pa3)
#define _pModbusTxFramePtr	((t_modbusFrame*)sc)
#define _modbusOutputAddr	modbus.startAddr
#define _modbusOutputValue	modbus.regQty
				sc=rs485_GetBufPtr();
				modbus.crc=CRC16MODBUSbyte((modbus.destAddr = _pModbusTxFramePtr->moduleAddr=_slaveAddr),0xFFFF);
				modbus.crc=CRC16MODBUSbyte((modbus.functionCode = _pModbusTxFramePtr->functionCode = WRITE_SINGLE_COIL), modbus.crc);
				modbus.crc=CRC16MODBUSword((_modbusOutputAddr = _pModbusTxFramePtr->writeSingleCoil_header.outputAddr= swap16(0)),modbus.crc);
				if(Module(MODULE_IS_DOOR_READY))
				{
					gpio_toggle_pin(CMD_RELAIS_1);
					modbus.crc=CRC16MODBUSword((_modbusOutputValue = _pModbusTxFramePtr->writeSingleCoil_header.outputValue=swap16(1)),modbus.crc);
				}
				else
				{
					modbus.crc=CRC16MODBUSword((_modbusOutputValue = _pModbusTxFramePtr->writeSingleCoil_header.outputValue=swap16(0)),modbus.crc);
				}

				_pModbusTxFramePtr->writeSingleCoil_header.writeSingleCoil_crc=swap16(modbus.crc);
				rs485_Send(modbus.frameLen = MODBUS_WRITE_SINGLE_COIL_FRAME_LEN, 0);
				//_rs485_Flush();
#undef _slaveAddr
#undef _outputAddr
#undef _outputValue
#undef _pModbusTxFramePtr
#undef _modbusOutputAddr
#undef _modbusOutputValue
				break;
			case READ_COILS:
				
				//sprintf(buf, "\r\nREAD COILS\r\nFunctionCode: %X\r\nSTARTING ADDR: %X QUANTITY OF COILS: %X",_pModbusRxFramePtr->functionCode,
					//swap16(_pModbusRxFramePtr->readCoils_header.byteCount), swap16(_pModbusRxFramePtr->readCoils_header.coilsQty));
				Putstr(buf);
				ReadCoilsAck(1,1);
				break;

			default:
				//FUNCTION CODE NOT SUPPORTED : RETURN EXCEPTION CODE 01 //////////////////////////////////////////////
				PushTask(Modbus,_MODBUS_RETURN_ERROR_CODE,1,0);
				
				break;
			}

#undef _pModbusRxFramePtr
		}
		break;


	case _MODBUS_COMMAND_INTERPRETER:
#define	_sc			pa1
#define _slvAddr	pa2
		switch(_sc)
		{
		case READ_COILS:
			Putstr("\r\n\tSLAVE REQ: READ COILS\r\n");
			mBitsSet(modbus.status, ST_REQEST);
			ReadCoils(modbus.destAddr, 0x01, 1);
			break;
		case WRITE_SINGLE_COIL:

			Putstr("\r\n\tSLAVE REQ: WRITE SINGLE COILS\r\n");
			mBitsSet(modbus.status, ST_REQEST);
			WriteSingleCoil(_slvAddr);
		
			break;
		case 0:
			Putstr("\r\n\t NO REQUEST");
			//mBitsClr(modbus.status, ST_FRAME_ACK_REQ);
			break;
		default:
			Error(ERROR_MODBUS_PROCESS_ACK_FRAME_SLAVE_SEND_BAD_SC, _sc);
		}
		mBitsClr(modbus.status, ST_MASTER_ASK);
		mBitsClr(modbus.status, ST_FRAME_ACK_REQ);
#undef _sc
#undef slvAddr

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
	case _MODBUS_COMMAND_ACKNOWNLEDGING_READ_COILS:
		Putstr("\r\n\nREAD COILS ACK!!\r\n");
		rs485_Send(modbus.frameLen = MODBUS_READ_COILS_FRAME_LEN, 0);
		break;
	case _MODBUS_SLAVE_NOT_ACK:
#define _slaveAddr		pa1
		Putstr("\r\n\t***** MODBUS_SLAVE_NOT_ACK FROM SLAVE #");
		sprintf(buf, "%02x *****\r\n", _slaveAddr);
		Putstr(buf);
		mBitsClr(modbus.status, ST_MASTER_ASK);
		mBitsClr(modbus.status, ST_REQEST);

		#undef _slaveAddr
		break;

	case _MODBUS_TIMER:
		if(!IsModuleIdentified() && modbus.timer)
		{
			
			gpio_set_pin_high(POWER_LED);
			mBitsSet(modbus.status, ST_ATTACH_READY);
			//sprintf(buf, "\r\n\tATTACH READY Firsr: %d\r\n", mIsBitsSet(modbus.status, ST_ATTACH_READY));
			//Putstr(buf);
			modbus.timer--;
			PushTask(Modbus, _MODBUS_TIMER, 0, 500);
		}
		else if (modbus.timer == 0)
		{
			gpio_set_pin_low(POWER_LED);	
			//mBitsClr(modbus.status, ST_ATTACH_READY);
			//sprintf(buf, "\r\n\tATTACH READY SCOND: %d\r\n", mIsBitsSet(modbus.status, ST_ATTACH_READY));
			//Putstr(buf);
		}
		break;
		
	case MODBUS_GET_NB_SLAVES:
		return (uint32_t)modbus.addIndex;
		break;
	case MODBUS_GET_ID_SLAVE:
#define nbSlave	pa1
		return (uint32_t)slaves[nbSlave];
#undef nbSlave
		break;
	///// DEFAULT SWITCH BAD SC ERROR TRAP ////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_MODBUS_SWITCH_BAD_SC,sc);
	}


	return 0;
}

