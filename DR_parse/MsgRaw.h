#pragma once
#include "stdafx.h"
#include "MsgHandler.h"

using namespace std;
using namespace CY_OWN;

class MsgRaw : public MsgHandler {
public:
	MsgRaw(MsgHandler*, unsigned int, unsigned int, unsigned int);
	bool HasHandler(unsigned int id);
	void MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);

private:
	unsigned int _first;
	unsigned int _second;
	unsigned int _sensorId;
};
