// DR_parse.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <math.h>
#include "CDrParse.h"
#include "type.h"
#include "cy_own.h"

using namespace std;
using namespace CY_OWN;

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE *pFileName = NULL;
	CDrParse parse;
	int idx;
	list <CY_OWN::DR_FILE_CAN_PKT>::iterator ite;


	if (argc != 2) {
		cout << "Forget something?" << endl;
		return 0;
	}

	pFileName = (BYTE*) argv[1];
	parse.OperFile2Mapping(pFileName);
	if (parse.getMapAddr() == NULL)
			cout << "parse.getMapAddr() = NULL" << endl;

	parse.ParseCan();
	parse.ShowRawObject();

#if 1

#else
	printf("Timestamp, CAN ID, Target No., Angle, Range, Power, Type, RelatedSpeed\n");

	for (ite = parse.m_outputList.begin(); ite != parse.m_outputList.end(); ite++) {
		if ((opt == 1 && ite->sid == 0x401) || (opt == 2 && ite->sid == 0x411)) {
			printf("%ld, ", ite->time);

			idx = ite->data[0] & 0x3F;
			printf("%03X, %d, ", ite->sid, idx);

			idx = ite->data[2] & 0x01;
			idx = (idx << 8) + ite->data[1];
			idx = (idx << 2) + ((ite->data[0] & 0xC0) >> 6);
			printf("%f, ", (idx - 1024) * 0.16);

			idx = ite->data[4] & 0x01;
			idx = (idx << 8) + ite->data[3];
			idx = (idx << 7) + ((ite->data[2] & 0xFE) >> 1);
			printf("%f, ", idx * 0.01);

			idx = ite->data[5] & 0x03;
			idx = (idx << 7) + ((ite->data[4] & 0xFE) >> 1);
			printf("%f,", idx * 0.32);

			printf("%d, ", (ite->data[5] & 0x7C) >> 2);

			idx = ite->data[7];
			idx = (idx << 6) + ((ite->data[4] & 0xFC) >> 2);
			printf("%f\n", (idx - 8192) * 0.02);
		} else if (ite->sid == 0x740) {
			printf("%ld, %03X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, \n", ite->time, ite->sid,
				ite->data[0], ite->data[1], ite->data[2], ite->data[3], ite->data[4], ite->data[5], ite->data[6], ite->data[7]);
		}
	}
#endif
	return 0;
}
