#pragma once
#include "stdafx.h"
#include "MsgHandler.h"

using namespace CY_OWN;

class msgRawHead : public MsgHandler {
public:
	msgRawHead(MsgHandler*, unsigned int);
	bool HasHandler(unsigned int id);
	void MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);

};
