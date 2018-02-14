//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "main.h"
#include "process.h"


//モジュール関連の取得
//Num番目のプロセスの情報を取得(通常1、複数プロセスのある場合のみ2以上)
DWORD GetDllModuleAddress(int Num,int *pId,HANDLE *polHandle,_TCHAR *sztDllFullPath)//0.05で変更
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int nRet;

	if(Num < 1){
		return 0;
	}
	if(*polHandle)CloseHandle(*polHandle);

	nRet = GetProcess(&pe32,Num);
	if(nRet != 0){
		return nRet;
	}
	
	//polのプロセスハンドルを取得
	*polHandle = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);
	if(*polHandle == NULL){
		return 0;
	}

	nRet = GetModule(&mo32,pe32);
	if(nRet != 0){
		return nRet;
	}

	*pId = pe32.th32ProcessID;

	if(sztDllFullPath != NULL){//0.05で追加
		_tcscpy_s(sztDllFullPath,MAX_PATH,(_TCHAR*)mo32.szExePath);
	}

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

	if(hModuleSnapshot == INVALID_HANDLE_VALUE){//0.5で修正
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
