#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "process.h"
#include "main.h"
#include "ffxiinfo.h"

#pragma comment(lib,"psapi.lib")


//モジュール関連の取得
//Num番目のプロセスの情報を取得(通常1、複数プロセスのある場合のみ2以上)
DWORD GetDllModuleAddress(int Num)
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int nRet;

	g_ProcessID = 0;

	if(Num < 1){
		return 0;
	}
	if(g_polHandle)CloseHandle(g_polHandle);

	nRet = GetProcess(&pe32,Num);
	if(nRet != 0){
		return nRet;
	}
	
	//polのプロセスハンドルを取得
	g_polHandle = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);
	if(g_polHandle == NULL){
		return 0;
	}

	nRet = GetModule(&mo32,pe32);
	if(nRet != 0){
		return nRet;
	}

	g_ProcessID = pe32.th32ProcessID;

	return (DWORD)mo32.modBaseAddr;
}

//複数プロセス対応プロセスIDを取得
int GetProcess(PROCESSENTRY32 *pe32,int iProcessCounter)
{
	int nRet = 1;
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
			if(wcscmp(_T("pol.exe"),pe32->szExeFile) == 0){
				iProcessCounter--;
				if(iProcessCounter <=0){
					nRet = 0;
					break;
				}
			}
		}while(Process32Next(hSnapshot,pe32));
	}
	CloseHandle(hSnapshot);

	if(nRet == 1){
		ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	}

	return nRet;
}

//複数プロセス対応モジュールを取得
int GetModule(MODULEENTRY32 *mo32,PROCESSENTRY32 pe32)
{
	int nFlag = 0;
	//モジュールのスナップショットを作成
	HANDLE hModuleSnapshot;
	hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe32.th32ProcessID);

	if(hModuleSnapshot == NULL){
		return 1;
	}

	//FFXiMain.dllのモジュールの情報を取得
	ZeroMemory(mo32,sizeof(MODULEENTRY32));
	mo32->dwSize = sizeof(MODULEENTRY32);

	if(Module32First(hModuleSnapshot,mo32)){
		do{
			if(wcscmp(_T("FFXiMain.dll"),mo32->szModule) == 0){
				nFlag = 1;
				break;
			}
		}while(Module32Next(hModuleSnapshot,mo32));
	}
	CloseHandle(hModuleSnapshot);

	if(nFlag == 0){
		return 2;
	}

	return 0;
}



//PID切替え
int ChangePID(HWND hWnd,int nCount)
{
	_TCHAR szTitle[MAX_PATH];
	
	if((g_DllAddress = GetDllModuleAddress(nCount)) != 0 && g_ProcessID){
		_swprintf(szTitle,_T("%s - %s"),BAZAAR_STRING,GetMyInfo().name);
		SetWindowText(hWnd,szTitle);
	}
	else{//失敗した時
		_swprintf(szTitle,BAZAAR_STRING);
		SetWindowText(hWnd,szTitle);
		return -1;
	}

	return 0;
}