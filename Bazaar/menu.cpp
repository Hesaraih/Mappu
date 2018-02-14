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
#include "main.h"
#include "menu.h"
#include "process.h"
#include "ffxiinfo.h"
#include "resource.h"


//メニューを変更 falseでDisable trueでEnable
int MenuItemEnable(HMENU hMenu,DWORD dwID,BOOL bFlag)
{
	MENUITEMINFO mii;

	//メニューの一部をグレーに
	ZeroMemory(&mii,sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(bFlag == (BOOL)true){
		mii.fState = MFS_ENABLED;
	}
	else{
		mii.fState = MFS_DISABLED;
	}
	SetMenuItemInfo(hMenu,dwID,false,&mii);

	return 1;
}


//PID選択メニューの作成
int MenuMakePID(HMENU hMenu)
{
	PROCESSENTRY32 pe32;
	_TCHAR szMenuStr[0x20];
	HMENU hPIDMenu;

	for(int nCount=1;nCount<=MAX_PID_COUNT;nCount++){//作ったメニュー項目を削除
		if(DeleteMenu(hMenu,IDM_PID + nCount,FALSE) == 0){//失敗した時
			break;
		}
	}

	hPIDMenu = GetSubMenu(hMenu,1);

	//プロセス数をカウント
	for(int nCount=1;nCount<=MAX_PID_COUNT;nCount++){
		if(GetProcess(&pe32,nCount) != 0)break;
		else{
			//メニューに追加
			//メニューを増やした場合挿入先に注意すること
			_swprintf(szMenuStr,_T("PID:%X"),pe32.th32ProcessID);
			InsertMenu(hPIDMenu,nCount,MF_BYPOSITION|MF_STRING,IDM_PID + nCount,szMenuStr);
			//現在監視しているPIDと同じならチェックする
			if(g_ProcessID == pe32.th32ProcessID){
				CheckMenuItem(hPIDMenu,IDM_PID + nCount,MF_BYCOMMAND|MFS_CHECKED);
			}
		}
	}

	return 0;
}


//メニューのチェック⇔アンチェック切り替え
BYTE MenuChangeCheck(HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	BYTE byRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	mii.fMask = MIIM_STATE;
	if(mii.fState == MFS_CHECKED){//チェックされている場合
		mii.fState = MFS_UNCHECKED;
		byRet = 0;
	}
	else{//チェックされていない場合
		mii.fState = MFS_CHECKED;
		byRet = 1;
	}
	//チェックボックス更新
	if(SetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	
	return byRet;
}

//メニューの状態を確認チェック時1、未チェック時0
UINT MenuGetCheck(HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	UINT uRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	if(mii.fState == MFS_CHECKED){//チェックされている場合
		uRet = 1;
	}
	else{//チェックされていない場合
		uRet = 0;
	}
	
	return uRet;
}


//wanted\*.iniをサーチして一覧作成
int MenuMakeFindWantedIni(HMENU hMenu)
{
	WIN32_FIND_DATA wfd;
	int nFindNum = 0;
	HANDLE hFind;
	HMENU hSubMenu;
	HMENU hWantedMenu;
	HANDLE hFile;
	_TCHAR szPath[MAX_PATH];

	hSubMenu = GetSubMenu(hMenu,3);
	hWantedMenu = GetSubMenu(hSubMenu,2);

	
	//カレントディレクトリのwanted.iniをサーチ
	hFile = CreateFile(_T("wanted.ini"),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		MenuItemEnable(hWantedMenu,IDM_LOAD_WANTED,true);
		nFindNum++;
		//現在使用しているiniと同じならチェックする
		if(wcscmp(g_WantedIniFileName,_T("wanted.ini")) == 0){
			CheckMenuItem(hWantedMenu,IDM_LOAD_WANTED,MF_BYCOMMAND|MFS_CHECKED);
		}
	}
	else{
		if(wcscmp(g_WantedIniFileName,_T("wanted.ini")) == 0){//wanted.iniが見つからない且つ読込みファイルがwanted.iniの時(初期化)
			_swprintf(g_WantedIniFileName,_T(""));
		}
		MenuItemEnable(hWantedMenu,IDM_LOAD_WANTED,false);
	}
	CloseHandle(hFile);

	for(int nCount=1;nCount<g_WantedIni;nCount++){//作ったメニュー項目を削除
		if(DeleteMenu(hWantedMenu,IDM_LOAD_WANTED + nCount,MF_BYCOMMAND) == 0){//失敗した時
			g_WantedIni = nFindNum;
			return 0;
		}
	}

	hFind = FindFirstFile(_T("wanted\\*.ini"),&wfd);
	if(hFind == INVALID_HANDLE_VALUE){
		g_WantedIni = nFindNum;
		return 0;
	}

	//wantedディレクトリをサーチ
	do{
		_swprintf(szPath,_T("wanted\\%s"),wfd.cFileName);
		InsertMenu(hWantedMenu,IDM_LOAD_WANTED + nFindNum + 1,MF_BYCOMMAND|MF_STRING,IDM_LOAD_WANTED + nFindNum,szPath);
		//現在使用しているiniと同じならチェックする
		if(wcscmp(g_WantedIniFileName,szPath) == 0){
			CheckMenuItem(hWantedMenu,IDM_LOAD_WANTED + nFindNum,MF_BYCOMMAND|MFS_CHECKED);
		}
		nFindNum++;
	}while(FindNextFile(hFind,&wfd));//失敗又は終了で0
	
	if(GetLastError() == ERROR_NO_MORE_FILES){//列挙終了
	}
	else{//失敗
		nFindNum = 0;
	}

	FindClose(hFind);

	g_WantedIni = nFindNum;

	return nFindNum;
}