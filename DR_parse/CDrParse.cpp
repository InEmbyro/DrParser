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
			if (_wfreopen_s(&m_pOut, name.data(), TEXT("w+"), stdout) != 0)
				cout << "Redirecting fail" << endl;
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


void CDrParse::ParseMasterTrackingHead(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator end)
{
	unsigned int number;
	unsigned int cycleCount;
	unsigned int temp;
	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator nextTracking;
	TRACKING_DATA_OBJECT trackObj;

	number = ite->data[0];
	cycleCount = ite->data[7];
	cycleCount = (cycleCount << 8) + ite->data[6];
	cycleCount = (cycleCount << 8) + ite->data[5];
	cycleCount = (cycleCount << 8) + ite->data[4];

	ite++;
	for (nextTracking = ite; nextTracking != end; nextTracking++)
	{
		if (nextTracking->sid == 0x605)
			break;
		if (nextTracking->sid >= 0x630 && nextTracking->sid <= 0x64F)
		{
			temp = nextTracking->data[1] & 0x1F;
			temp = (temp << 8) + nextTracking->data[0];
			trackObj.range_x = ((double)temp - 4000) * 0.032;

			temp = nextTracking->data[3] & 0x01;
			temp = (temp << 8) + nextTracking->data[2];
			temp = (temp << 3) + ((nextTracking->data[1] & 0xE0) >> 5);
			trackObj.range_y = ((double)temp - 2048) * 0.064;

			temp = nextTracking->data[4] & 0x0F;
			temp = (temp << 7) + ((nextTracking->data[3] & 0xFE) >> 1);
			trackObj.speed_x = ((double)temp - 1024) * 0.1;

			temp = nextTracking->data[5] & 0x7F;
			temp = (temp << 4) + ((nextTracking->data[4] & 0xF0) >> 4);
			trackObj.speed_y = ((double)temp - 1024) * 0.1;
			
			trackObj.number = (nextTracking->data[7] & 0xFC) >> 2;

			printf("%d, %.3f, %.3f, %.3f, %.3f, 0\n", trackObj.number, trackObj.range_x, trackObj.range_y, trackObj.speed_x, trackObj.speed_y);
		}
	}
}

void CDrParse::ParseSlaveTrackingHead(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator end)
{
	unsigned int number;
	unsigned int cycleCount;
	unsigned int temp;
	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator nextTracking;
	TRACKING_DATA_OBJECT trackObj;

	number = ite->data[0];
	cycleCount = ite->data[7];
	cycleCount = (cycleCount << 8) + ite->data[6];
	cycleCount = (cycleCount << 8) + ite->data[5];
	cycleCount = (cycleCount << 8) + ite->data[4];
	//printf("Slave No. of Tracking %d, Cycle: %d\nNo., Range X, Range Y, Speed X, Speed Y\n", number, cycleCount);

	ite++;
	for (nextTracking = ite; nextTracking != end; nextTracking++)
	{
		if (nextTracking->sid == 0x6A5)
			break;
		if (nextTracking->sid >= 0x6D0 && nextTracking->sid <= 0x6EF)
		{
			temp = nextTracking->data[1] & 0x1F;
			temp = (temp << 8) + nextTracking->data[0];
			trackObj.range_x = ((double)temp - 4000) * 0.032;

			temp = nextTracking->data[3] & 0x01;
			temp = (temp << 8) + nextTracking->data[2];
			temp = (temp << 3) + ((nextTracking->data[1] & 0xE0) >> 5);
			trackObj.range_y = (double)(temp - 2048) * 0.064;

			temp = nextTracking->data[4] & 0x0F;
			temp = (temp << 7) + ((nextTracking->data[3] & 0xFE) >> 1);
			trackObj.speed_x = ((double)temp - 1024) * 0.1;

			temp = nextTracking->data[5] & 0x7F;
			temp = (temp << 4) + ((nextTracking->data[4] & 0xF0) >> 4);
			trackObj.speed_y = ((double)temp - 1024) * 0.1;
			
			trackObj.number = (nextTracking->data[7] & 0xFC) >> 2;

			printf("%d, %.3f, %.3f, %.3f, %.3f, 1\n", trackObj.number, trackObj.range_x, trackObj.range_y, trackObj.speed_x, trackObj.speed_y);
		}
	}
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
		if (raw.targetNo == (ite402->data[0] & 0x3F) && ite402->sid == 0x405) {
			idx = ite402->data[3] & 0x03;
			idx = (idx << 7) + ((ite402->data[2] & 0xFE) >> 1);
			raw.threshold = idx * 0.32;
			m_RawObject401List.push_back(raw);
			//printf("%.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %d, 0\n", m_system.velocity, (int)raw.targetNo, raw.angle, 
			//	raw.range, raw.AbsLevel_db, raw.relatedSpeed, raw.threshold, raw.type);
			break;
		}
	}
}

void CDrParse::Parse411(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator end)
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
		if (raw.targetNo == (ite402->data[0] & 0x3F) && ite402->sid == 0x415) {
			idx = ite402->data[3] & 0x03;
			idx = (idx << 7) + ((ite402->data[2] & 0xFE) >> 1);
			raw.threshold = idx * 0.32;
			//printf("%.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %d, 1\n", m_system.velocity, (int)raw.targetNo, raw.angle, 
			//	raw.range, raw.AbsLevel_db, raw.relatedSpeed, raw.threshold, raw.type);
			m_RawObject411List.push_back(raw);
			break;
		}
	}
}

void CDrParse::ShowRawObject()
{
	if (m_RawList.size() <= 0)
		return;

	bool findHead = TRUE;
	int idx = 0;
	unsigned int previousTime = 0;
	int counter = 500000;

	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite;
	std::list<CY_OWN::RAW_DATA_OBJECT>::iterator iteRaw;
	
	//cout << "Velocity," << "Target No.," << "Angle," << "Range," << "Power," << "RelatedSpeed," << "Threshold," << "Type," << "0/1" << endl;
	printf("Tracking No., Range X, Range Y, Speed X, Speed Y, 0/1\n");
	ite = m_RawList.begin();
	while (!m_RawList.empty()) {
		ite = m_RawList.begin();
		switch (ite->sid) {
#if 0
		case 0x3F5:
			ParseCarVelocity(ite);
			break;
#endif
		case 0x605:
			ParseMasterTrackingHead(ite, m_RawList.end());
			break;
		case 0x6A5:
			ParseSlaveTrackingHead(ite, m_RawList.end());
			break;
#if 0
		case 0x403:
			Parse400(ite);
			iteRaw = m_RawObject401List.begin();
			while (iteRaw != m_RawObject401List.end()) {
				iteRaw = m_RawObject401List.begin();
				printf("%.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %d, 0\n", m_system.velocity, (int)iteRaw->targetNo, iteRaw->angle, 
					iteRaw->range, iteRaw->AbsLevel_db, iteRaw->relatedSpeed, iteRaw->threshold, iteRaw->type);
				iteRaw++;
				m_RawObject401List.pop_front();
			}
			m_RawObject401List.clear();
			break;
		case 0x413:
			//counter--;
			iteRaw = m_RawObject411List.begin();
			while (iteRaw != m_RawObject411List.end()) {
				iteRaw = m_RawObject411List.begin();
				printf("%.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %d, 1\n", m_system.velocity, (int)iteRaw->targetNo, iteRaw->angle, 
					iteRaw->range, iteRaw->AbsLevel_db, iteRaw->relatedSpeed, iteRaw->threshold, iteRaw->type);
				iteRaw++;
				m_RawObject411List.pop_front();
			}
			m_RawObject411List.clear();
			break;
		case 0x404:
			Parse401(ite, m_RawList.end());
			break;
		case 0x414:
			Parse411(ite, m_RawList.end());
			break;
#endif
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
	short idx = 0;
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
		if (pkt.sid == 0x605 || (pkt.sid >= 0x630 && pkt.sid <= 0x64F))
		{
			printf("%x,", pkt.sid);
			for (idx = 0; idx < 8; idx++)
			{
				printf("%02X,", pkt.data[idx]);
			}
			printf("\n");
		} 
		else if (pkt.sid == 0x6A5 || (pkt.sid >= 0x6D0 && pkt.sid <= 0x6EF))
		{
			printf("%x,", pkt.sid);
			for (idx = 0; idx < 8; idx++)
			{
				printf("%02X,", pkt.data[idx]);
			}
			printf("\n");
		}
#endif
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
