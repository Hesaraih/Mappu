#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//ユニコードでコンパイル
#ifndef _UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "process.h"
#include "resource.h"
#include "memorymap.h"//0.37で追加
#include "searchaddr.h"//0.37で追加

#define STRING_POL	_T("pol.exe")
#define STRING_DLL	_T("FFXiMain.dll")

extern volatile HANDLE g_polHandle;//プレイオンラインのハンドル
extern volatile DWORD g_ProcessID;//監視するプロセスID
extern volatile DWORD g_DllAddress;//FFXiMain.dllのアドレス
extern volatile DWORD g_DllSize;//FFXiMAin.dllのサイズ
extern FFXIOFFSET g_Offset;//0.37で追加


//モジュール関連の取得
//Num番目のプロセスの情報を取得(通常1、複数プロセスのある場合のみ2以上)
DWORD GetDllModuleAddress(int Num)
{
	HANDLE hPol;//0.37で追加
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int iRet;

	if(Num < 1){
		return -1;
	}
	if(g_polHandle)CloseHandle(g_polHandle);

	iRet = GetProcess(&pe32,Num);
	if(iRet != 0){
		return -2;
	}
	
	//polのプロセスハンドルを取得
	hPol = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);//0.37で変更

	//if(g_polHandle == NULL){
	if(hPol == NULL){//0.37で変更
		return -3;
	}

	iRet = GetModule(&mo32,pe32);
	if(iRet != 0){
		return -4;
	}

	g_polHandle = hPol;
	g_ProcessID = pe32.th32ProcessID;
	g_DllSize = mo32.modBaseSize;

	return (DWORD)mo32.modBaseAddr;
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
			if(_tcscmp (STRING_POL,pe32->szExeFile) == 0){
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

//複数プロセス対応モジュールを取得
int GetModule(MODULEENTRY32 *mo32,PROCESSENTRY32 pe32)
{
	int iFlag = 0;
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
			if(_tcscmp (STRING_DLL,mo32->szModule) == 0){
				iFlag = 1;
				break;
			}
		}while(Module32Next(hModuleSnapshot,mo32));
	}
	CloseHandle(hModuleSnapshot);

	if(iFlag == 0){
		return 2;
	}

	return 0;
}

//プロセスIDが存在するか確認
int CheckProcessID(DWORD pID)
{
	int iFlag = 0;
	//プロセスのスナップショット一覧取得
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32,sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)return 0;
	
	//pol.exeのプロセスを取得
	if(Process32First(hSnapshot,&pe32)){
		do{
			if(pID == pe32.th32ProcessID){
				iFlag = 1;
				break;
			}
		}while(Process32Next(hSnapshot,&pe32));
	}
	CloseHandle(hSnapshot);

	if(iFlag == 0){
		return 2;
	}

	return 0;
}

//PID一覧を作成する
//HWNDにはコンボボックスのハンドル
int MakeListPID(HWND hWnd)
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;//0.37で追加
	_TCHAR str[0x20];
	int i;

	//コンボボックスを初期化
	while(SendMessage(hWnd,CB_GETCOUNT,0,0) != 0){
		SendMessage(hWnd,CB_DELETESTRING,0,0);
	}


	//PIDがあればコンボボックスに追加
	for(i=1;i<=0x10;i++){
		if(GetProcess(&pe32,i) == 0 && GetModule(&mo32,pe32) == 0){//0.37で変更
			if(GetPCNameFromPid(pe32.th32ProcessID,str,sizeof(str)) != 0){//0.37で追加
				_stprintf(str,_T("PID:%03X"),pe32.th32ProcessID);
			}
			SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)str);
			if(g_ProcessID == pe32.th32ProcessID){//すでに選択されているPIDにカーソルをセット
				SendMessage(hWnd,CB_SETCURSEL,i-1,0);
			}
		}
		else break;
	}

	if(i == 1){//一つもPIDが見つからない場合
		return 0;
	}

	//コンボボックスのカーソルがセットされていない場合
	if(SendMessage(hWnd,CB_GETCURSEL,0,0) == -1){
		if(SendMessage(hWnd,CB_GETLBTEXT,0,(LPARAM)str) != CB_ERR){
			GetProcess(&pe32,1);//0.37で変更
			g_ProcessID = pe32.th32ProcessID;

			SendMessage(hWnd,CB_SETCURSEL,0,0);//一番上をセット
		}
	}

	return 1;
}


//PIDからウィンドウハンドルを取得　0.18で復活
HWND GetHWNDFromPID(DWORD dwPID)
{
	DWORD ProcessID;//0.35で変更
	HWND hWnd = GetTopWindow(NULL);

	do{
		if(GetWindowLong(hWnd,GWL_HWNDPARENT) != 0 || !IsWindowVisible(hWnd)){
			continue;
		}
		GetWindowThreadProcessId(hWnd,&ProcessID);
		if(dwPID == ProcessID){
			return hWnd;
		}
	}while((hWnd = GetNextWindow(hWnd,GW_HWNDNEXT)) != NULL);

	return NULL;
}

//PIDからキャラ名を取得 0.37で追加
int GetPCNameFromPid(DWORD dwPID,_TCHAR *szPCName,int nSize)
{
	HANDLE hPol;
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;

	ZeroMemory(&pe32,sizeof(PROCESSENTRY32));
	pe32.th32ProcessID = dwPID;
	//polのプロセスハンドルを取得
	hPol = OpenProcess(PROCESS_VM_READ,false,dwPID);


	if(GetModule(&mo32,pe32) != 0){
		return -1;
	}

	if(0 != GetPCNameFromHandle(hPol,(DWORD)mo32.modBaseAddr,0,szPCName,nSize)){
		return -2;
	}

	if(!(_tcslen(szPCName) >= 3 && szPCName[0] >= 'A' && szPCName[0] <= 'Z')){
		return -3;
	}

	return 0;
}

//0.37で追加
int GetPCNameFromHandle(HANDLE hPol,DWORD dwDllAddr,int pos,_TCHAR* szPCName,int nSize)
{
	DWORD dwAddress;
	char dump[0x210];

	//構造体初期化
	ZeroMemory(szPCName,sizeof(szPCName));
	
	//メモリリード
	dwAddress = dwDllAddr + g_Offset.NowSta + NOWSTA_HOSEI + pos * NOWSTA_SIZE;//スタート位置の補正NOWSTA_HOSEI
	if(0 == ReadProcessMemory(hPol,(const void *)dwAddress,dump,sizeof(dump),NULL)){
		return -1;
	}

	//NOWSTA_MEM_PARTYが0の時すでに脱退
	if(dump[NOWSTA_MEM_PARTY] == 0x01){
		mbstowcs(szPCName,&dump[NOWSTA_MEM_NAME],nSize-1);
	}
	else{
		return -2;
	}

	return 0;
}

//初期PIDを選択 0.43で追加
int SetInitPID(HWND hWnd,_TCHAR* str)
{
	if(_tcslen(str) == 0)return -1;

	int nPIDcount = SendMessage(hWnd,CB_GETCOUNT,0,0);
	_TCHAR szText[0x20];

	if(nPIDcount <= 0)return -1;

	for(int i=0;i<nPIDcount;i++){
		SendMessage(hWnd,CB_GETLBTEXT,i,(LPARAM)szText);
		if(_tcscmp (str,szText) == 0){
			SendMessage(hWnd,CB_SETCURSEL,i,0);
			SendMessage(GetParent(hWnd),WM_COMMAND,IDC_COMBO_PID|(CBN_SELCHANGE<<16),0);
			break;
		}
	}

	return 0;
}