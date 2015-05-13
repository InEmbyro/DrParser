#include "stdafx.h"
#include "CDrParse.h"
#include "type.h"
#include <iostream>
#include <io.h>
#include <ctime>
#include <math.h>
#include <list>
#include <iostream>
#include <vector>
#include <map>
#include "cy_own.h"

using namespace std;
using namespace CY_OWN;

#define PI	3.1415926
#define RAD_CONVER	(PI / 180)
#define GROUP_NO	2

std::list<CAN_MSG> outputList;
std::list<CAN_MSG>* pOutputList;
//std::list<RAW_OBJECT_STRUCT>** ppRawList;
//std::list<RAW_OBJECT_STRUCT>* pRawTempList;
//
//std::list<std::list<RAW_OBJECT_STRUCT>*> rawHead;
//std::list<std::list<RAW_OBJECT_STRUCT>*>::iterator iteRawHead;

//std::list<RAW_OBJECT_STRUCT> rawTempList;

CDrParse::CDrParse()
	:m_FileHnd(INVALID_HANDLE_VALUE), m_MappingHnd(INVALID_HANDLE_VALUE), m_lpMapAddress(NULL)
{
	m_pOut = NULL;
	memset((void*)&m_system, 0x00, sizeof(m_system));
}

CDrParse::~CDrParse()
{
	if (m_FileHnd != INVALID_HANDLE_VALUE)
		CloseHandle(m_FileHnd);
	if (m_MappingHnd != INVALID_HANDLE_VALUE)
		CloseHandle(m_MappingHnd);
	FlushViewOfFile(m_lpMapAddress, 0);
}

HANDLE CDrParse::getFileHnd(void)
{
	return m_FileHnd;
}

void CDrParse::Close()
{
	UnmapViewOfFile(m_lpMapAddress);
	CloseHandle(m_MappingHnd);
	CloseHandle(m_FileHnd);
	fclose(m_pOut);

	m_lpMapAddress = NULL;
	m_FileHnd = INVALID_HANDLE_VALUE;
	m_MappingHnd = INVALID_HANDLE_VALUE;
}

void CDrParse::OperFile2Mapping(std::basic_string<WCHAR> name)
{
	std::basic_string<WCHAR> csv(TEXT("csv"));

	m_FileHnd = CreateFile(name.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	m_FielSizeLow = GetFileSize(m_FileHnd, &filesizeHigh);
	m_MappingHnd = CreateFileMapping(m_FileHnd, NULL, PAGE_READONLY, 0, 0, NULL);
	m_lpMapAddress = MapViewOfFile(m_MappingHnd, FILE_MAP_READ, 0, 0, 0);

	if (m_lpMapAddress != NULL) {
		size_t idx = name.find_last_of('.');
		if (idx == std::string::npos) {
			//nothing to do
		} else {
			name.replace(idx+1, 3, csv);
			m_pOut = _wfreopen(name.data(), TEXT("w+"), stdout);
		}
	}

	return;
}

HANDLE CDrParse::getMappingHnd(void)
{
	return m_MappingHnd;
}

LPVOID CDrParse::getMapAddr(void)
{
	return m_lpMapAddress;
}

void CDrParse::ParseCarVelocity(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite)
{
	int idx;

	idx = ite->data[1] & 0x7F;
	idx = (idx << 8) + ite->data[0];
	m_system.velocity = idx * 0.01;
}

void CDrParse::Parse400(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite)
{
	if (ite->dlc != 6)
		return;
}
void CDrParse::Parse401(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator end)
{
	int idx;
	CY_OWN::RAW_DATA_OBJECT raw;
	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite402;

	raw.targetNo = ite->data[0] & 0x3F;

	idx = ite->data[2] & 0x01;
	idx = (idx << 8) + ite->data[1];
	idx = (idx << 2) + ((ite->data[0] & 0xC0) >> 6);
	raw.angle =((idx - 1024) * 0.16);

	idx = ite->data[4] & 0x01;
	idx = (idx << 8) + ite->data[3];
	idx = (idx << 7) + ((ite->data[2] & 0xFE) >> 1);
	raw.range = idx * 0.01;

	idx = ite->data[5] & 0x03;
	idx = (idx << 7) + ((ite->data[4] & 0xFE) >> 1);
	raw.AbsLevel_db = idx * 0.32;

	raw.type = ((ite->data[5] & 0x7C) >> 2);

	idx = ite->data[7];
	idx = (idx << 6) + ((ite->data[6] & 0xFC) >> 2);
	raw.relatedSpeed = ((idx - 8192) * 0.02);

	for (ite402 = ite; ite402 != end; ite402++) {
		if (raw.targetNo == ite402->data[0] & 0x3F) {
			idx = ite402->data[3] & 0x01;
			idx = (idx << 8) + ite402->data[2];
			raw.threshold = idx * 0.32;
			printf("%.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %d\n", m_system.velocity, (int)raw.targetNo, raw.angle, 
				raw.range, raw.AbsLevel_db, raw.relatedSpeed, raw.threshold, raw.type);
			break;
		}
	}
}

void CDrParse::ShowRawObject()
{
	double velocity;

	if (m_RawList.size() <= 0)
		return;

	bool findHead = TRUE;
	int idx = 0;
	unsigned int previousTime = 0;

	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite;
	

	cout << "Velocity," << "Target No.," << "Angle," << "Range," << "Power," << "RelatedSpeed," << "Threshold," << "Type" << endl;
	ite = m_RawList.begin();
	while (!m_RawList.empty()) {
		ite = m_RawList.begin();
		switch (ite->sid) {
		case 0x3F5:
			ParseCarVelocity(ite);
			break;
		case 0x400:
			Parse400(ite);
			break;
		case 0x401:
			Parse401(ite, m_RawList.end());
			break;
		default:
			break;
		}
		ite++;
		m_RawList.pop_front();
	}
}

void CDrParse::ParseCan()
{
	BYTE* pS = (BYTE*)m_lpMapAddress;
	BYTE* pE = NULL;
	BYTE* ptr = NULL;
	BYTE* ptrRaw = NULL;
	BYTE buf[BUFFER_SIZE];
	short idx = 0;
	CAN_MSG msg;
	CY_OWN::DR_FILE_CAN_PKT pkt;
	
	pE = (BYTE*)m_lpMapAddress + m_FielSizeLow;
	ptr = pS + 16;
	idx = 0;

	do {
		memset((void*)&pkt, 0x00, sizeof(pkt));
		memcpy((void*)&pkt, ptr, sizeof(pkt));
		ptr += BUFFER_SIZE;
		m_RawList.push_back(pkt);

#if 0
		//byte[7:4]
		idx = 7;
		msg.time = buf[idx--];
		msg.time = (msg.time << 8) + buf[idx--];
		msg.time = (msg.time << 8) + buf[idx--];
		msg.time = (msg.time << 8) + buf[idx--];

		//byte[13:12]
		idx = 13;
		msg.sid = buf[idx--];
		msg.sid = (msg.sid << 8) + buf[idx];

		//byte[16];	
		idx = 16;
		msg.dlc = buf[idx];

		//byte[27:20]
		idx = 20;
		msg.data[0] = buf[idx++];
		msg.data[1] = buf[idx++];
		msg.data[2] = buf[idx++];
		msg.data[3] = buf[idx++];
		msg.data[4] = buf[idx++];
		msg.data[5] = buf[idx++];
		msg.data[6] = buf[idx++];
		msg.data[7] = buf[idx++];

		//byte[36:32]
		idx = 36;
		msg.time2 = buf[idx--];
		msg.time2 = (msg.time2 << 8) + buf[idx--];
		msg.time2 = (msg.time2 << 8) + buf[idx--];
		msg.time2 = (msg.time2 << 8) + buf[idx--];
		msg.time2 = (msg.time2 << 8) + buf[idx--];

		idx = 0;
#endif
	} while ((pE - ptr) > 8);
}
