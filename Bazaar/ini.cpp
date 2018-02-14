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


//bazaar.ini読込み
int ReadIni(HWND hWnd)
{
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szFullPathName[MAX_PATH];
	POINT pos,size;

	if(0 == GetFullPathName(_T("bazaar.ini"),sizeof(szFullPathName)/sizeof(_TCHAR),szFullPathName,NULL)){
		return 0;
	}

	//[OFFSET]
	g_AutoOffset = GetPrivateProfileInt(_T("OFFSET"),_T("AUTO"),0,szFullPathName);//0.08で追加
	GetPrivateProfileString(_T("OFFSET"),_T("NPCMAP"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_OffsetNpcMap = wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"),_T("NOWSTA"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_OffsetNowSta = wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"),_T("BAZAAR"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_OffsetBazaar = wcstoul(szBuf,NULL,16);//16進数で読み込みを行う

	//[NPC_MEM]
	GetPrivateProfileString(_T("NPC_MEM"),_T("POS_X"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	NPC_MEM.POS_X = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	//GetPrivateProfileString(_T("NPC_MEM"),_T("POS_Y"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);//0.09で削除
	//NPC_MEM.POS_Y = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"),_T("POS_Z"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	NPC_MEM.POS_Z = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	//GetPrivateProfileString(_T("NPC_MEM"),_T("CHANGEDID"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);//0.09で削除
	//NPC_MEM.CHANGEDID = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"),_T("NAME"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	NPC_MEM.Name = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	//GetPrivateProfileString(_T("NPC_MEM"),_T("DISTANCE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);//0.09で削除
	//NPC_MEM.DISTANCE = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"),_T("INRANGE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	NPC_MEM.Inrange = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"),_T("BAZAAR"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	NPC_MEM.Bazaar = (WORD)wcstoul(szBuf,NULL,16);//16進数で読み込みを行う

	//[SETTING]
	GetPrivateProfileString(_T("SETTING"),_T("USE_FILTER_PRICE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_UseFilterPrice = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("FILTER_PRICE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_FilterPrice = (int)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("AUTO_SCROLL"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_AutoScroll = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("LIST_COLOR"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ListColor = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("BLACK_LIST"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_BlackList = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("USE_FFXIAH"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_UseFFXIAH = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("FFXIAH"),NULL,g_FFXIAH,sizeof(g_FFXIAH)/sizeof(_TCHAR),szFullPathName);
	GetPrivateProfileString(_T("SETTING"),_T("TARGET"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);//0.08で追加
	g_TargetName = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("EXTRACT_SAVE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ExtractSave = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("DIR_SAVE"),NULL,g_DirSave,sizeof(g_FFXIAH)/sizeof(_TCHAR),szFullPathName);
	GetPrivateProfileString(_T("SETTING"),_T("SAVE_WINDOW"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_SaveWindow = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("SAVE_COLUMN"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_SaveColumn = (BYTE)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("KEY_DELAY"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_KeyDelay = wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("WAIT_UPDATE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_WaitUpdate = wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("SETTING"),_T("RETRY"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_Retry = wcstoul(szBuf,NULL,10);
	g_BazaarOrder = (BYTE)GetPrivateProfileInt(_T("SETTING"),_T("ORDER"),0,szFullPathName);//0.09で追加
	g_BazaarNumWait05 = (BYTE)GetPrivateProfileInt(_T("SETTING"),_T("WAIT_NUM05"),0,szFullPathName);//0.09で追加
	g_BazaarNumWait10 = (BYTE)GetPrivateProfileInt(_T("SETTING"),_T("WAIT_NUM10"),0,szFullPathName);//0.09で追加
	g_BazaarNumWait20 = (BYTE)GetPrivateProfileInt(_T("SETTING"),_T("WAIT_NUM20"),0,szFullPathName);//0.09で追加
	g_BazaarNumWait21 = (BYTE)GetPrivateProfileInt(_T("SETTING"),_T("WAIT_NUM21"),0,szFullPathName);//0.09で追加

	//[WINDOW]
	GetPrivateProfileString(_T("WINDOW"),_T("POS_X"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	pos.x = (LONG)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("POS_Y"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	pos.y = (LONG)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("SIZE_X"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	size.x = (LONG)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("SIZE_Y"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	size.y = (LONG)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("COLUMN_PC"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ColumnWidth[COLUMN_PC] = (int)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("COLUMN_NAME"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ColumnWidth[COLUMN_NAME] = (int)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("COLUMN_PRICE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ColumnWidth[COLUMN_PRICE] = (int)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("COLUMN_NUMBER"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ColumnWidth[COLUMN_NUMBER] = (int)wcstoul(szBuf,NULL,10);
	GetPrivateProfileString(_T("WINDOW"),_T("COLUMN_NOTE"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	g_ColumnWidth[COLUMN_NOTE] = (int)wcstoul(szBuf,NULL,10);

	MoveWindow(hWnd,pos.x,pos.y,size.x,size.y,TRUE);

	if(g_KeyDelay < 100){
		g_KeyDelay = 100;
	}
	if(g_WaitUpdate < 1000){
		g_WaitUpdate = 1000;
	}
	if(g_Retry < 3){
		g_Retry = 3;
	}

	
	return 1;
}

//bazaar.iniに設定を書込み
int WriteIni()
{
	_TCHAR szBuf[0x10];
	_TCHAR szFullPathName[MAX_PATH];
	int ret = 1;

	if(0 == GetFullPathName(_T("bazaar.ini"),sizeof(szFullPathName)/sizeof(_TCHAR)-1,szFullPathName,NULL)){
		return 0;
	}
	
	_swprintf(szBuf,_T("%d"),g_AutoOffset);//0.08で追加
	ret &= WritePrivateProfileString(_T("OFFSET"),_T("AUTO"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_UseFilterPrice);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("USE_FILTER_PRICE"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_FilterPrice);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("FILTER_PRICE"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_ListColor);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("LIST_COLOR"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_AutoScroll);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("AUTO_SCROLL"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_BlackList);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("BLACK_LIST"),szBuf,szFullPathName);
	//_swprintf(szBuf,_T("%d"),g_BlackList);
	_swprintf(szBuf,_T("%d"),g_UseFFXIAH);//0.08で変更
	ret &= WritePrivateProfileString(_T("SETTING"),_T("USE_FFXIAH"),szBuf,szFullPathName);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("FFXIAH"),g_FFXIAH,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_TargetName);//0.08で追加
	ret &= WritePrivateProfileString(_T("SETTING"),_T("TARGET"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_ExtractSave);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("EXTRACT_SAVE"),szBuf,szFullPathName);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("DIR_SAVE"),g_DirSave,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_SaveWindow);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("SAVE_WINDOW"),szBuf,szFullPathName);	
	_swprintf(szBuf,_T("%d"),g_SaveColumn);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("SAVE_COLUMN"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_KeyDelay);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("KEY_DELAY"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_WaitUpdate);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("WAIT_UPDATE"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_Retry);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("RETRY"),szBuf,szFullPathName);
	//0.09で追加
	_swprintf(szBuf,_T("%d"),g_BazaarOrder);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("ORDER"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_BazaarNumWait05);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("WAIT_NUM05"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_BazaarNumWait10);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("WAIT_NUM10"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_BazaarNumWait20);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("WAIT_NUM20"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_BazaarNumWait21);
	ret &= WritePrivateProfileString(_T("SETTING"),_T("WAIT_NUM21"),szBuf,szFullPathName);

	return ret;
}


//bazaar.iniの[WINDOW]セクション書込み
int WriteIniWindow(HWND hWnd)
{
	_TCHAR szBuf[0x10];
	_TCHAR szFullPathName[MAX_PATH];
	//RECT rect;
	WINDOWPLACEMENT wndPlace;//0.07で変更
	int ret = 1;

	if(0 == GetFullPathName(_T("bazaar.ini"),sizeof(szFullPathName)/sizeof(_TCHAR)-1,szFullPathName,NULL)){
		return 0;
	}
	
	//GetWindowRect(hWnd,&rect);
    wndPlace.length = sizeof(WINDOWPLACEMENT);//0.07で変更
    GetWindowPlacement(hWnd,&wndPlace);

	//_swprintf(szBuf,_T("%d"),rect.left);
	_swprintf(szBuf,_T("%d"),wndPlace.rcNormalPosition.left);//0.07で変更
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("POS_X"),szBuf,szFullPathName);	
	//_swprintf(szBuf,_T("%d"),rect.top);
	_swprintf(szBuf,_T("%d"),wndPlace.rcNormalPosition.top);//0.07で変更
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("POS_Y"),szBuf,szFullPathName);

	//_swprintf(szBuf,_T("%d"),rect.right - rect.left);
	_swprintf(szBuf,_T("%d"),wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left);//0.07で変更
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("SIZE_X"),szBuf,szFullPathName);
	//_swprintf(szBuf,_T("%d"),rect.bottom - rect.top);
	_swprintf(szBuf,_T("%d"),wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top);//0.07で変更
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("SIZE_Y"),szBuf,szFullPathName);

	return ret;
}

int WriteIniColumn()
{
	_TCHAR szBuf[0x10];
	_TCHAR szFullPathName[MAX_PATH];
	int ret = 1;

	if(0 == GetFullPathName(_T("bazaar.ini"),sizeof(szFullPathName)/sizeof(_TCHAR)-1,szFullPathName,NULL)){
		return 0;
	}

	_swprintf(szBuf,_T("%d"),g_ColumnWidth[COLUMN_PC]);
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("COLUMN_PC"),szBuf,szFullPathName);	
	_swprintf(szBuf,_T("%d"),g_ColumnWidth[COLUMN_NAME]);
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("COLUMN_NAME"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_ColumnWidth[COLUMN_PRICE]);
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("COLUMN_PRICE"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_ColumnWidth[COLUMN_NUMBER]);
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("COLUMN_NUMBER"),szBuf,szFullPathName);
	_swprintf(szBuf,_T("%d"),g_ColumnWidth[COLUMN_NOTE]);
	ret &= WritePrivateProfileString(_T("WINDOW"),_T("COLUMN_NOTE"),szBuf,szFullPathName);


	return ret;
}


//wantedを文字列から取得
int ReadWantedFromString(_TCHAR *szIniFile)
{
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szFullPathName[MAX_PATH];
	_TCHAR szSection[0x20];
	int r,g,b;

	if(0 == GetFullPathName(szIniFile,sizeof(szFullPathName)/sizeof(_TCHAR),szFullPathName,NULL)){
		return 0;
	}

	GetPrivateProfileString(_T("CALL"),_T("SOUND"),NULL,g_szSound,sizeof(g_szSound)/sizeof(_TCHAR),szFullPathName);
	
	GetPrivateProfileString(_T("COLOR"),_T("PRICE_LOW"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	swscanf(szBuf,_T("RGB(%i,%i,%i)"),&r,&g,&b);
	g_ColorLow = RGB(r,g,b);
	GetPrivateProfileString(_T("COLOR"),_T("PRICE_HIGH"),NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
	swscanf(szBuf,_T("RGB(%i,%i,%i)"),&r,&g,&b);
	g_ColorHigh = RGB(r,g,b);

	for(int i=0;i<100;i++){
		_swprintf(szSection,_T("%02d_USE"),i);
		GetPrivateProfileString(_T("WANTED"),szSection,_T("0"),szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
		g_Wanted[i].use = (BYTE)wcstoul(szBuf,NULL,10);
		_swprintf(szSection,_T("%02d_NAME"),i);
		GetPrivateProfileString(_T("WANTED"),szSection,NULL,g_Wanted[i].szName,sizeof(g_Wanted[1].szName)/sizeof(_TCHAR),szFullPathName);
		_swprintf(szSection,_T("%02d_PRICE"),i);
		GetPrivateProfileString(_T("WANTED"),szSection,NULL,szBuf,sizeof(szBuf)/sizeof(_TCHAR),szFullPathName);
		g_Wanted[i].price = wcstoul(szBuf,NULL,10);
	}
	
	return 1;
}


//blist.ini
int ReadBlackList()
{
	_TCHAR szFullPathName[MAX_PATH];
	_TCHAR szSection[0x20];

	if(0 == GetFullPathName(_T("blist.ini"),sizeof(szFullPathName)/sizeof(_TCHAR),szFullPathName,NULL)){
		return 0;
	}

	for(int i=0;i<100;i++){
		_swprintf(szSection,_T("%02d_NAME"),i);
		GetPrivateProfileString(_T("BLIST"),szSection,NULL,g_szBlackListName[i],sizeof(g_szBlackListName[0])/sizeof(_TCHAR),szFullPathName);
	}
	
	return 1;
}

