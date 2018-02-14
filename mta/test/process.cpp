#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "process.h"

#define STRING_POL	_T("pol.exe")
#define STRING_DLL	_T("FFXiMain.dll")


DWORD GetPid(int Num)
{
	PROCESSENTRY32 pe32;
	int iRet;

	if(Num < 1){
		return 0;
	}
	iRet = GetProcess(&pe32,Num);
	if(iRet != 0){
		return 0;
	}
	return pe32.th32ProcessID;
}

//複数プロセス対応プロセスIDを取得
int GetProcess(PROCESSENTRY32 *pe32,int iProcessCounter)
{
	int iRet = 1;
	//プロセスのスナップショット一覧取得
	HANDLE hSnapshot;
	ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	pe32->dwSize = sizeof(PROCESSENTRY32);

	if(iProcessCounter < 1){
		return 1;
	}

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)return 0;
	
	//pol.exeのプロセスを取得
	if(Process32First(hSnapshot,pe32)){
		do{
			if(_tcscmp(STRING_POL,pe32->szExeFile) == 0){
				iProcessCounter--;
				if(iProcessCounter <=0){
					iRet = 0;
					break;
				}
			}
		}while(Process32Next(hSnapshot,pe32));
	}
	CloseHandle(hSnapshot);

	if(iRet == 1){
		ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	}

	return iRet;
}
