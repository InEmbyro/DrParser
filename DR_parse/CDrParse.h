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

	void	OperFile2Mapping(BYTE* pName);
	HANDLE	getFileHnd(void);
	HANDLE	getMappingHnd(void);
	LPVOID	getMapAddr(void);
	void	ParseCan(void);
	void	ShowRawObject(void);

	std::list<CY_OWN::DR_FILE_CAN_PKT> m_RawList;

private:
	LPVOID	m_lpMapAddress;
	HANDLE	m_FileHnd;
	HANDLE	m_MappingHnd;
	DWORD	m_FielSizeLow;
	DWORD	filesizeHigh;
	unsigned int m_cycle;
};
