#include "stdafx.h"
#include "MsgHandler.h"

MsgHandler::MsgHandler(MsgHandler* p)
	:_successor(p), _canId(0)
{

}

MsgHandler::~MsgHandler()
{
	if (_successor != 0)
	{
		delete _successor;
		_successor = 0;
	}
}

bool MsgHandler::HasHandler(unsigned int id)
{
	if (id == _canId)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MsgHandler::SetCanId(unsigned int id)
{
	_canId = id;
}

unsigned int MsgHandler::GetCandId()
{
	return _canId;
}

void MsgHandler::MsgHanderProcess(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite)
{
	if (_successor != 0)
	{
		_successor->MsgHanderProcess(ite);
	}
}