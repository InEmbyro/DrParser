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


//void CDrParse::ParseMasterTrackingHead(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator ite, std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator end)
//{
//	unsigned int number;
//	unsigned int cycleCount;
//	unsigned int temp;
//	std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator nextTracking;
//	TRACKING_DATA_OBJECT trackObj;
//
//	number = ite->data[0];
//	cycleCount = ite->data[7];
//	cycleCount = (cycleCount << 8) + ite->data[6];
//	cycleCount = (cycleCount << 8) + ite->data[5];
//	cycleCount = (cycleCount << 8) + ite->data[4];
//
//	ite++;
//	for (nextTracking = ite; nextTracking != end; nextTracking++)
//	{
//		if (nextTracking->sid == 0x605)
//			break;
//		if (nextTracking->sid >= 0x630 && nextTracking->sid <= 0x64F)
//		{
//			temp = nextTracking->data[1] & 0x1F;
//			temp = (temp << 8) + nextTracking->data[0];
//			trackObj.range_x = ((double)temp - 4000) * 0.032;
//
//			temp = nextTracking->data[3] & 0x01;
//			temp = (temp << 8) + nextTracking->data[2];
//			temp = (temp << 3) + ((nextTracking->data[1] & 0xE0) >> 5);
//			trackObj.range_y = ((double)temp - 2048) * 0.064;
//
//			temp = nextTracking->data[4] & 0x0F;
//			temp = (temp << 7) + ((nextTracking->data[3] & 0xFE) >> 1);
//			trackObj.speed_x = ((double)temp - 1024) * 0.1;
//
//			temp = nextTracking->data[5] & 0x7F;
//			temp = (temp << 4) + ((nextTracking->data[4] & 0xF0) >> 4);
//			trackObj.speed_y = ((double)temp - 1024) * 0.1;
//			
//			trackObj.number = (nextTracking->data[7] & 0xFC) >> 2;
//
//			printf("%u, %d, %.3f, %.3f, %.3f, %.3f, 0\n", nextTracking->time, trackObj.number, trackObj.range_x, trackObj.range_y, trackObj.speed_x, trackObj.speed_y);
//		}
//	}
//}

void CDrParse::ParseTrace(std::list<CY_OWN::DR_FILE_CAN_PKT>::iterator data)
{
	unsigned int	temp;
	struct BROOK_CAN_TRACE trace;
#if 0
	data->data[0] = 0x11;
	data->data[1] = 0x0a;
	data->data[2] = 0x4e;
	data->data[3] = 0x8b;
	data->data[4] = 0xbf;
	data->data[5] = 0x0f;
	data->data[6] = 0x0f;
	data->data[7] = 0;
#endif
	trace.traceId = data->data[0] & 0x07;
	
	temp = ((data->data[0] & 0xF8) >> 3) + ((data->data[1] & 0x03) << 5);
	trace.radius = static_cast<double>(temp) *0.25;

	temp = ((data->data[1] & 0xFC) >> 2);
	trace.angle = (static_cast<double>(temp) - 12) * 5;

	temp = data->data[2];
	trace.coordinate_x = static_cast<double>(temp) * 0.25;

	temp = (data->data[3] & 0x7F);
	trace.coordinate_y = (static_cast<double>(temp) - 64.0f ) * 0.25;

	temp = (data->data[4] & 0x7F) << 1;
	temp = temp + ((data->data[3] & 0x80) >> 7);
	trace.relativeSpeed = (static_cast<double>(temp) - 128)* 0.4;

	temp = (data->data[5] & 0x7F) << 1;
	temp = temp + ((data->data[4] & 0x80) >> 7);
	trace.objectSignalLevel = static_cast<double>(temp) * 0.96;

	temp = data->data[5] & 0x80;
	trace.objectAppearanceStatus = temp!=0?1:0;

	temp = data->data[6] & 0x01;
	trace.errorFlag = temp!=0?1:0;

	temp = (data->data[6] & 0x06) >> 1;
	trace.triggerFlag = temp;

	temp = data->data[6] & 0x08;
	trace.detectionFlag = temp!=0?1:0;

	printf("%u, %x, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %d, %d, %d, %d\n", 
		data->time, data->sid, trace.traceId, trace.radius, trace.angle, trace.coordinate_x, trace.coordinate_y,
		trace.relativeSpeed, trace.objectSignalLevel,trace.objectAppearanceStatus, trace.errorFlag, trace.triggerFlag,
		trace.detectionFlag);

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
	
	cout << "Timestamp," << "CAN ID," << "Trace ID," << "Radius," << "Angle," << "Coordinate X," << "Coordinate Y," << "Relative Speed," \
		<< "Signal Level," << "Appearance Status," << " Error flag," << "Trigger flag," << "Detection Flag," << endl;
	ite = m_RawList.begin();
	while (!m_RawList.empty()) {
		ite = m_RawList.begin();
		switch (ite->sid) {
		case 0:
		default:
			if (ite->sid >= 0x510 && ite->sid <= 0x597)
			{
				ParseTrace(ite);
			}
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
