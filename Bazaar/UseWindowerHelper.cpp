#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define USEWINDOWERHELPER_CPP


#include <windows.h>
#include <tchar.h>
#include "UseWindowerHelper.h"
#include "main.h"


int InitWindowerHelper(HWND hWnd)
{
	HMODULE hModule;

	if((hModule = LoadLibrary(_T("WindowerHelper.dll"))) != NULL){
		if((DLL_CreateKeyboardHelper = (void* (__stdcall*)(char*))GetProcAddress(hModule,"CreateKeyboardHelper")) == NULL){
			MessageBox(hWnd,_T("Cant find CreateKeyboardHelper()"),_T("WindowerHelper.dll"),MB_ICONERROR);
			SendMessage(hWnd,WM_CLOSE,NULL,NULL);
			return 0;
		}
		if((DLL_DeleteKeyboardHelper = (void (__stdcall*)(void*))GetProcAddress(hModule,"DeleteKeyboardHelper")) == NULL){
			MessageBox(hWnd,_T("Cant find DeleteKeyboardHelper()"),_T("WindowerHelper.dll"),MB_ICONERROR);
			SendMessage(hWnd,WM_CLOSE,NULL,NULL);
			return 0;
		}
		if((DLL_CKHSetKey = (void (__stdcall*)(void*,unsigned char,bool))GetProcAddress(hModule,"CKHSetKey")) == NULL){
			MessageBox(hWnd,_T("Cant find CKHSetKey()"),_T("WindowerHelper.dll"),MB_ICONERROR);
			SendMessage(hWnd,WM_CLOSE,NULL,NULL);
			return 0;
		}
		if((DLL_CKHSendString = (void (__stdcall*)(void*,char*))GetProcAddress(hModule,"CKHSendString")) == NULL){
			MessageBox(hWnd,_T("Cant find CKHSendString()"),_T("WindowerHelper.dll"),MB_ICONERROR);
			SendMessage(hWnd,WM_CLOSE,NULL,NULL);
			return 0;
		}
		char szHandler[MAX_PATH];
		sprintf(szHandler,"WindowerMMFKeyboardHandler_%d",g_ProcessID);
		pKeyboardHelper = (CKeyboardHelper*)DLL_CreateKeyboardHelper(szHandler);
	}
	else{
		MessageBox(hWnd,_T("Cant find WindowerHelper.dll"),NULL,MB_ICONERROR);
		SendMessage(hWnd,WM_CLOSE,NULL,NULL);
		return 0;
	}

	return 1;
}


int CloseWindowerHelper()
{
	DLL_DeleteKeyboardHelper(pKeyboardHelper);

	return 1;
}
