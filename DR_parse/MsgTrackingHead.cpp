#include "stdafx.h"
#include "MsgTrackingHead.h"
#include <iostream>

using namespace std;


msgTrackingHead::msgTrackingHead(MsgHandler* p, unsigned int first)
	:MsgHandler(p)
{
	_canId = first;
}

void msgTrackingHead::MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator obj)
{
	if (HasHandler(obj->sid))
	{
		if (obj->data[0] != 0)
		{
			cout << "Timestamp [ms], Object_Number," << "Sensor_ID," << "X_point1[m]," << "y_point1[m]," << "speed_X[m/s]," \
				<< "speed_y[m/s]," << endl;
		}
	}
	else
	{
		MsgHandler::MsgHanderProcess(obj);
	}
}

bool msgTrackingHead::HasHandler(unsigned int id)
{
	if (id == _canId)
	{
		return true;
	}

	return false;
}
