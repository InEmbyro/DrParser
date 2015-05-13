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

void FineDrFile(void);

std::list<std::basic_string<WCHAR>> fileList;

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE *pFileName = NULL;
	CDrParse parse;
	int idx;
	std::list<std::basic_string<WCHAR>>::iterator ite;
	std::basic_string<WCHAR> name;


	FineDrFile();
	if (fileList.size() == 0) {
		cout << "No DR file" << endl;
		return 0;
	}

	for (ite = fileList.begin(); ite != fileList.end(); ite++) {
		name = ite->data();
		parse.OperFile2Mapping(name);
		if (parse.getMapAddr() == NULL) {
			cout << "parse.getMapAddr() = NULL" << endl;
		} else {
			parse.ParseCan();
			parse.ShowRawObject();
		}
		parse.Close();
	}
	return 0;
}

void FineDrFile()
{
	WIN32_FIND_DATA ffd;
	HANDLE hnd;
	//std::basic_string<WCHAR> cvs(TEXT("cvs"));

	hnd = FindFirstFile(_T(".\\*.dr2"), &ffd);
	if (hnd == INVALID_HANDLE_VALUE) {
		//cout << "No any DR file!!" << endl;
		return;
	}

	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//nothing to do
		} else {
			//_tprintf(TEXT("%s\n"), ffd.cFileName);
			std::basic_string<WCHAR> s = ffd.cFileName;
			size_t idx = s.find_last_of('.');
			if (idx == std::string::npos) {
				//nothing to do
			} else {
				//s.replace(idx+1, 3, cvs);
				fileList.push_back(s);
				//_tprintf(TEXT("%s.csv\n"), s.substr(0, idx));
			}
		}
	} while (FindNextFile(hnd, &ffd) != 0);
	FindClose(hnd);

	return;
}