#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include "process.h"
#include "..\\mta\\mta.h"

//#define STATICLINK

#ifdef STATICLINK
#pragma comment(lib,"mta.lib")
#endif

int main()
{
#ifdef STATICLINK
	//静的リンク
	MTA mta = MTA(GetPid(1),0x4FCA84,0x40C548);//初期化：プロセスID、TARGETのオフセ、NPCMAPのオフセ
	mta.SetTarget(0xB);//ID=0xBをタゲる
	printf("キー入力待ち");
	while(!_kbhit()){}//キー入力があるまで待機
	mta.UnSetTarget();//タゲ解除

#else
	//動的リンク
	void* (__stdcall *mta_Init)(DWORD,DWORD,DWORD);
	void* (__stdcall *mta_InitFromIniFile)(DWORD);
	void (__stdcall *mta_Delete)(void*);
	void (__stdcall *mta_SetTarget)(void*,DWORD);
	void (__stdcall *mta_UnSetTarget)(void*);

	HMODULE hModule;
	MTA *pMta;
	if((hModule = LoadLibrary(_T("mta.dll"))) != NULL){
		if((mta_Init = (void* (__stdcall*)(DWORD,DWORD,DWORD))GetProcAddress(hModule,"mta_Init")) == NULL){
			MessageBox(NULL,_T("Cant find mta_Init()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_InitFromIniFile = (void* (__stdcall*)(DWORD))GetProcAddress(hModule,"mta_InitFromIniFile")) == NULL){
			MessageBox(NULL,_T("Cant find mta_InitFromIniFile()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_Delete = (void (__stdcall*)(void*))GetProcAddress(hModule,"mta_Delete")) == NULL){
			MessageBox(NULL,_T("Cant find mta_Delete()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_SetTarget = (void (__stdcall*)(void*,DWORD))GetProcAddress(hModule,"mta_SetTarget")) == NULL){
			MessageBox(NULL,_T("Cant find mta_SetTarget()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_UnSetTarget = (void (__stdcall*)(void*))GetProcAddress(hModule,"mta_UnSetTarget")) == NULL){
			MessageBox(NULL,_T("Cant find mta_UnSetTarget()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		//pMta = (MTA*)mta_Init(GetPid(1),0x4FCA84,0x40C548);//初期化：プロセスID、TARGETのオフセ、NPCMAPのオフセ
		pMta = (MTA*)mta_InitFromIniFile(GetPid(1));//初期化：プロセスID、オフセ等はmta.dllから取得
	}
	else{
		MessageBox(NULL,_T("Cant find mta.dll"),_T("mta.dll"),MB_ICONERROR);
		return false;
	}
	//mta_SetTarget(pMta,0x400);//ID=0x400をタゲる
	mta_SetTarget(pMta,0xB);//ID=0xBをタゲる
	printf("キー入力待ち");
	while(!_kbhit()){}//キー入力があるまで待機
	mta_UnSetTarget(pMta);//タゲ解除
	mta_Delete(pMta);//終了
	//CloseHandle(hModule);
#endif

	return true;
}