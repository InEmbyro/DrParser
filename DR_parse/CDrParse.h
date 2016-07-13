#pragma once

#include <Windows.h>
#include <list>
#include "type.h"
#include "cy_own.h"
#define	BUFFER_SIZE	40

using namespace CY_OWN;

class CDrParse {

public:
	CDrParse();
	~CDrParse();

	void	OperFile2Mapping(std::basic_string<WCHAR>);
	void	Close(void);
	HANDLE	getFileHnd(void);
	HANDLE	getMappingHnd(void);
	LPVOID	getMapAddr(void);
	void	ParseCan(void);
	void	ShowRawObject(void);

	std::list<CY_OWN::DR_FILE_CAN_PKT> m_RawList;


private:
	LPVOID	m_lpMapAddress;
	HANDLE	m_FileHnd;
	FILE*	m_pOut;
	HANDLE	m_MappingHnd;
	DWORD	m_FielSizeLow;
	DWORD	filesizeHigh;
	unsigned int m_cycle;
	void	ParseCarVelocity(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	void	ParseTrace(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	CY_OWN::SYSTEM_STATUS	m_system;
};
