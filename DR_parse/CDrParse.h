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
	std::list<CY_OWN::RAW_DATA_OBJECT> m_RawObject401List;
	std::list<CY_OWN::RAW_DATA_OBJECT> m_RawObject411List;

private:
	LPVOID	m_lpMapAddress;
	HANDLE	m_FileHnd;
	FILE*	m_pOut;
	HANDLE	m_MappingHnd;
	DWORD	m_FielSizeLow;
	DWORD	filesizeHigh;
	unsigned int m_cycle;
	void	ParseCarVelocity(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	void	Parse400(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	void	Parse401(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	void	Parse411(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator);
	CY_OWN::SYSTEM_STATUS	m_system;
};
