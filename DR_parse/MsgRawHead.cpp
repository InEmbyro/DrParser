#include "stdafx.h"
#include "MsgRawHead.h"
#include <iostream>

using namespace std;


msgRawHead::msgRawHead(MsgHandler* p, unsigned int first)
	:MsgHandler(p)
{
	_canId = first;
}

void msgRawHead::MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator obj)
{
	if (HasHandler(obj->sid))
	{
		if (obj->dlc == 6 && (obj->data[3] & 0xFC) != 0)
		{
			//printf("%d, Object_Number, Azimuth1[rad],AZWinke1[¢X], Range1[m]\n", obj->time);
			printf("Timestamp [ms], Object_Number, Azimuth1[rad],AZWinke1[¢X], Range1[m], AbsLevel_db [dB]\n");
		}
	}
	else
	{
		MsgHandler::MsgHanderProcess(obj);
	}
}

bool msgRawHead::HasHandler(unsigned int id)
{
	if (id == _canId)
	{
		return true;
	}

	return false;
}
