#include "stdafx.h"
#include "MsgRaw.h"

MsgRaw::MsgRaw(MsgHandler* p, unsigned int first, unsigned int second, unsigned int sensorId)
	:MsgHandler(p)
{
	_first = first;
	_second = second;
	_sensorId = sensorId;
}

void MsgRaw::MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator obj)
{
	int temp;
	CY_OWN::RAW_DATA_OBJECT _rawObj;

	if (HasHandler(obj->sid))
	{
		if (obj->sid == _first)
		{
			_rawObj.targetNo = obj->data[0] & 0x3F;

			temp = obj->data[2] & 0x01;
			temp = (temp << 8) + obj->data[1];
			temp = (temp << 2) + ((obj->data[0] & 0xC0) >> 6);
			_rawObj.angle = (temp - 1024) * 0.16;

			temp = obj->data[4] & 0x01;
			temp = (temp << 8) + obj->data[3];
			temp = (temp << 7) + ((obj->data[2] & 0xFE) >> 1);
			_rawObj.range = temp * 0.01;

			temp = obj->data[5] & 0x03;
			temp = (temp << 7) + ((obj->data[4] & 0xFE) >> 1);
			_rawObj.AbsLevel_db = temp * 0.32;

			_rawObj.type = (obj->data[5] & 0x7C) >> 2;

			temp = obj->data[7];
			temp = (temp << 6) + ((obj->data[6] & 0xFC) >> 2);
			_rawObj.relatedSpeed = (temp - 8192) * 0.02;
			printf("%d, R%d, %d, %.3f, %.3f, %.3f, %.3f\n", obj->time / 1000, _sensorId, _rawObj.targetNo, (_rawObj.angle * 3.141593 / 180), _rawObj.angle, _rawObj.range , _rawObj.AbsLevel_db);
		}
		else
		{
			//TODO
		}
	}
	else
	{
		MsgHandler::MsgHanderProcess(obj);
	}

}

bool MsgRaw::HasHandler(unsigned int id)
{
	if (id == _first || id == _second)
	{
		return true;
	}

	return false;
}
