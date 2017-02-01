#pragma once 
#include "stdafx.h"
#include "cy_own.h"
#include <list>
using namespace std;

class MsgHandler {
public:
	MsgHandler(MsgHandler* = 0);
	~MsgHandler();
	virtual void MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	void SetCanId(unsigned int);
	unsigned int GetCandId();
	virtual bool HasHandler(unsigned int = 0);

protected:
	unsigned int _canId;
	MsgHandler* _successor;
};

