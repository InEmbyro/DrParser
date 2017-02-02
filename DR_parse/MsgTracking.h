#pragma once
#include "stdafx.h"
#include "MsgHandler.h"
#include <list>

using namespace std;
using namespace CY_OWN;

class msgTracking : public MsgHandler {
public:
	msgTracking(MsgHandler*, unsigned int, unsigned int, unsigned int);
	bool HasHandler(unsigned int);
	void MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);

	unsigned int _low;
	unsigned int _high;
	unsigned int _sensorId;
};
