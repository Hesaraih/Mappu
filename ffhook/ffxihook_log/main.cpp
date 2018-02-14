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

	printf("FFXiMain.dllの情報を取得\n");
	DWORD dwDllAddress = GetDllModuleAddress(1,&pId,&polHandle);
	printf("    pId = 0x%08X\n",pId);
	printf("    polHandle = 0x%08X\n",polHandle);
	printf("    dwDllAddress = 0x%08X\n",dwDllAddress);
	
	/*
	InjectLibrary
	ハンドルを指定する場合はOpenProcessでPROCESS_ALL_ACCESSを指定
	*/
	printf("madCHookをインジェクト\n");
	InjectLibrary((DWORD)polHandle,_T("madCHook.dll"));

	/*
	ffxihook_logは動的リンクで読込む
	*/
	printf("ffxihook_logをロード\n");
	hDll = LoadLibrary(_T("ffxihook_log.dll"));
	printf("    ffxihook_log.dll = 0x%08X\n",hDll);
	if(hDll == NULL){
		printf("  ロードに失敗\n");
	}

	printf("関数をロード\n");
	bool (__stdcall *FFXiHookLog_Inject)(DWORD);
	FFXiHookLog_Inject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Inject");
	if(FFXiHookLog_Inject){
		printf("    FFXiHookLog_Inject = 0x%08X\n",FFXiHookLog_Inject);
	}
	bool (__stdcall *FFXiHookLog_Eject)(DWORD);
	FFXiHookLog_Eject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Eject");
	if(FFXiHookLog_Eject){
		printf("    FFXiHookLog_Eject = 0x%08X\n",FFXiHookLog_Eject);
	}
	IFFXiHooks* (__stdcall *FFXiHookLog_Interface)(void);
	FFXiHookLog_Interface = (IFFXiHooks* (__stdcall *)(void))GetProcAddress(hDll,"FFXiHookLog_Interface");
	if(FFXiHookLog_Interface){
		printf("    FFXiHookLog_Interface = 0x%08X\n",FFXiHookLog_Interface);
	}
	if(FFXiHookLog_Inject == NULL || FFXiHookLog_Eject == NULL || FFXiHookLog_Interface == NULL){
		printf("    ロードに失敗\n");
	}
	
	/*
	ffxihook_logをインジェクト
	madCHookでやっても良いかも
	*/
	printf("ffxihook_logをインジェクト\n");
	if(FFXiHookLog_Inject(pId) == FALSE){
		printf("    失敗\n");
	}
	
	/*
	ffxihook_logの初期化
	*/
	printf("ffxihook_logの初期化\n");
	IFFXiHooks* pHooks = FFXiHookLog_Interface();
	if(pHooks->Open() == FALSE){
		printf("    pHooks->Open()失敗\n");
	}

	/*
	ログを取得してみる
	プロンプトでどれかキーを押すとループ終了
	*/
	printf("------ループ開始------\n");
	BYTE attr;
	size_t index = 0;
	_TCHAR cstr[0x200];
	while(!_kbhit()){
		attr = 0;
		_stprintf_s(cstr, _T("%s"),pHooks->Get(attr));
		if(attr)printf("%02X : %s\n",attr,cstr);
		Sleep(10);
	}
	printf("------ループ終了------\n");
	
	/*
	ffxihook_logの終了処理
	*/
	printf("ffxihook_logの終了処理\n");
	pHooks->Close();
	pHooks->Destroy();
	
	/*
	埋め込んだffxihook_log.dllをアンロード
	*/
	printf("ffxihook_logをエジェクト\n");
	if(FFXiHookLog_Eject(pId) == 0){
		printf("    失敗\n");
	}
	
	/*
	埋め込んだmadCHook.dllをアンロード
	*/
	printf("madCHookをエジェクト\n");
	UninjectLibrary((DWORD)polHandle,_T("madCHook.dll"));
	
	/*
	解放
	*/
	FreeLibrary(hDll);
	CloseHandle(polHandle);

	_getch();

	return 0;
}