#include "stdafx.h"
#include "MsgHandler.h"
#include "MsgTracking.h"

msgTracking::msgTracking(MsgHandler* p, unsigned int low, unsigned int high)
	:_low(low), _high(high), MsgHandler(p)
{
}

bool msgTracking::HasHandler(unsigned int id)
{
	if (id < _low || id > _high)
	{
		return false;
	}
	return true;
}

void msgTracking::MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite)
{
	int temp;

	if (HasHandler((ite->sid)))
	{
		TRACKING_DATA_OBJECT trace;
		
		temp = ite->data[1] & 0x1F;
		temp = (temp << 8) + ite->data[0];
		trace.range_x = ((double)temp - 7500) * 0.016;

		temp = ite->data[3] & 0x01;
		temp = (temp << 8) + ite->data[2];
		temp = (temp << 3) + ((ite->data[1] & 0xE0) >> 5);
		trace.range_y = ((double)temp - 2048) * 0.032;

		temp = ite->data[4] & 0x0F;
		temp = (temp << 7) + ((ite->data[3] & 0xFE) >> 1);
		trace.speed_x = ((double)temp - 1024) * 0.1;

		temp = ite->data[5] & 0x7F;
		temp = (temp << 4) + ((ite->data[4] & 0xF0) >> 4);
		trace.speed_y = ((double)temp - 1024) * 0.1;
			
		trace.number = (ite->data[7] & 0xFC) >> 2;

		printf("%d, %d, %.3f, %.3f, %.3f, %.3f\n", ite->time / 1000, trace.number, trace.range_x, trace.range_y, trace.speed_x, trace.speed_y);
	}
	else
	{
		MsgHandler::MsgHanderProcess(ite);
	}
}
