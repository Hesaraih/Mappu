#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#include "process.h"
#include "ffxihook_log.h"
#include "madCHook.h"

#pragma comment(lib,"madCHook")

int main()
{
	int pId = 0;
	HANDLE polHandle = NULL;
	HINSTANCE hDll;

	_tprintf(_T("FFXiMain.dllの情報を取得\n"));
	DWORD dwDllAddress = GetDllModuleAddress(1,&pId,&polHandle);
	_tprintf(_T("    pId = 0x%08X\n"),pId);
	_tprintf(_T("    polHandle = 0x%08X\n"), (DWORD)polHandle);
	_tprintf(_T("    dwDllAddress = 0x%08X\n"), (DWORD)dwDllAddress);
	
	/*
	InjectLibrary
	ハンドルを指定する場合はOpenProcessでPROCESS_ALL_ACCESSを指定
	*/
	_tprintf(_T("madCHookをインジェクト\n"));
	InjectLibrary((DWORD)polHandle,_T("madCHook.dll"));

	/*
	ffxihook_logは動的リンクで読込む
	*/
	_tprintf(_T("ffxihook_logをロード\n"));
	hDll = LoadLibrary(_T("ffxihook_log.dll"));
	_tprintf(_T("    ffxihook_log.dll = 0x%08X\n"), (DWORD)hDll);
	if(hDll == NULL){
		_tprintf(_T("  ロードに失敗\n"));
	}

	_tprintf(_T("関数をロード\n"));
	bool (__stdcall *FFXiHookLog_Inject)(DWORD);
	FFXiHookLog_Inject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Inject");
	if(FFXiHookLog_Inject){
		_tprintf(_T("    FFXiHookLog_Inject = 0x%08X\n"), (DWORD)FFXiHookLog_Inject);
	}
	bool (__stdcall *FFXiHookLog_Eject)(DWORD);
	FFXiHookLog_Eject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Eject");
	if(FFXiHookLog_Eject){
		_tprintf(_T("    FFXiHookLog_Eject = 0x%08X\n"), (DWORD)FFXiHookLog_Eject);
	}
	IFFXiHooks* (__stdcall *FFXiHookLog_Interface)(void);
	FFXiHookLog_Interface = (IFFXiHooks* (__stdcall *)(void))GetProcAddress(hDll,"FFXiHookLog_Interface");
	if(FFXiHookLog_Interface){
		_tprintf(_T("    FFXiHookLog_Interface = 0x%08X\n"), (DWORD)FFXiHookLog_Interface);
	}
	if(FFXiHookLog_Inject == NULL || FFXiHookLog_Eject == NULL || FFXiHookLog_Interface == NULL){
		_tprintf(_T("    ロードに失敗\n"));
	}
	
	/*
	ffxihook_logをインジェクト
	madCHookでやっても良いかも
	*/
	_tprintf(_T("ffxihook_logをインジェクト\n"));
	if(FFXiHookLog_Inject(pId) == FALSE){
		_tprintf(_T("    失敗\n"));
	}
	
	/*
	ffxihook_logの初期化
	*/
	_tprintf(_T("ffxihook_logの初期化\n"));
	IFFXiHooks* pHooks = FFXiHookLog_Interface();
	if(pHooks->Open() == FALSE){
		_tprintf(_T("    pHooks->Open()失敗\n"));
	}

	/*
	ログを取得してみる
	プロンプトでどれかキーを押すとループ終了
	*/
	_tprintf(_T("------ループ開始------\n"));
	BYTE attr;
	size_t index = 0;
	_TCHAR cstr[0x200];
	while(!_kbhit()){
		attr = 0;
		_stprintf_s(cstr, _T("%s"),(LPCTSTR)pHooks->Get(attr));
		if(attr)_tprintf(_T("%02X : %s\n"),attr, (LPCTSTR)cstr);
		Sleep(10);
	}
	_tprintf(_T("------ループ終了------\n"));
	
	/*
	ffxihook_logの終了処理
	*/
	_tprintf(_T("ffxihook_logの終了処理\n"));
	pHooks->Close();
	pHooks->Destroy();
	
	/*
	埋め込んだffxihook_log.dllをアンロード
	*/
	_tprintf(_T("ffxihook_logをエジェクト\n"));
	if(FFXiHookLog_Eject(pId) == 0){
		printf("    失敗\n");
	}
	
	/*
	埋め込んだmadCHook.dllをアンロード
	*/
	_tprintf(_T("madCHookをエジェクト\n"));
	UninjectLibrary((DWORD)polHandle,_T("madCHook.dll"));
	
	/*
	解放
	*/
	FreeLibrary(hDll);
	CloseHandle(polHandle);

	_getch();

	return 0;
}