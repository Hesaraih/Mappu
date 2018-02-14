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
#include <stdio.h>
#include <math.h>
#include <olectl.h>
#include <commctrl.h>
#include "resource.h"
#include "header.h"
#include "main.h"
#include "draw.h"
#include "list.h"
#include "searchaddr.h"
#include "other.h"

#pragma comment(lib,"Comctl32.lib")

//エラーコード表示
void ErrorNum(HWND hWnd)
{
	_TCHAR szErrorMessage[MAX_PATH];
	sprintf(szErrorMessage, _T("Error Code = %d"), g_ErrorCode);
	MessageBox(hWnd, szErrorMessage, _T("Mappu"), MB_OK | MB_ICONERROR);
	PostMessage(hWnd, WM_CLOSE, 1, 1);//0.51で変更
}

//ini読み込み
int ReadIniFiles(HWND hWnd, const _TCHAR* szFileName)
{
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szFullPathName[MAX_PATH];
	int r, g, b;

	if (0 == GetFullPathName(szFileName, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
		return 0;
	}

	//[OFFSET]
	GetPrivateProfileString(_T("OFFSET"), _T("NPCMAP"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Offset.NpcMap = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"), _T("NPCLIST"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Offset.NpcList = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"), _T("NOWSTA"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Offset.NowSta = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"), _T("TARGET"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Offset.Target = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OFFSET"), _T("WEATHER"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Offset.Weather = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う

											   //[NPC_MEM]
	GetPrivateProfileString(_T("NPC_MEM"), _T("POS_X"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.POS_X = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("POS_Y"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.POS_Y = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("POS_Z"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.POS_Z = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("RADIAN"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.Radian = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("CHANGEDID"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.ChangedId = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("FIXEDID"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.FixedId = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("NAME"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.Name = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("VIEW_ADDRESS"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.ViewAddress = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("DISTANCE"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.Distance = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("HP"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.Hpp = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("NPCTYPE"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);//0.53で追加
	NPC_MEM.NpcType = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("VIEW"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.View = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("INRANGE"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.Inrange = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("STATE"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.State = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("NPC_MEM"), _T("LASTHATEID"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	NPC_MEM.LastHateId = (WORD)strtoul(szBuf, NULL, 16);//16進数で読み込みを行う

														//[OTHER]
	g_Timer = GetPrivateProfileInt(_T("OTHER"), _T("TIMER"), 500, szFullPathName);
	GetPrivateProfileString(_T("OTHER"), _T("KEY_ZOOM_+0.1"), _T("0021"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_KeyZoomPlus = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OTHER"), _T("KEY_ZOOM_+1.0"), _T("1021"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_KeyZoomPlus_1 = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OTHER"), _T("KEY_ZOOM_-0.1"), _T("0022"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_KeyZoomMinus = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	GetPrivateProfileString(_T("OTHER"), _T("KEY_ZOOM_-1.0"), _T("1022"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_KeyZoomMinus_1 = strtoul(szBuf, NULL, 16);//16進数で読み込みを行う
	g_DLLFileSizeCmp = GetPrivateProfileInt(_T("OTHER"), _T("DLLFILESIZE"), 0, szFullPathName);
	GetPrivateProfileString(_T("OTHER"), _T("EXIT_DIALOGBOX"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_ExitDialogBox = 1;
	}
	GetPrivateProfileString(_T("OTHER"), _T("CLEAR_TIME"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_ClearTime = 1;
	}
	GetPrivateProfileString(_T("OTHER"), _T("NOTCHANGECOLOR_CORPSE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_NotChangeColorCorpse = 1;
	}
	GetPrivateProfileString(_T("OTHER"), _T("NOTCHANGECOLOR_WAR"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_NotChangeColorWar = 1;
	}
	GetPrivateProfileString(_T("OTHER"), _T("NPCLIST_BKCOLOR"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_NPCLIST_BkColor = 1;
	}


	//[POS]
	g_MapPosX = GetPrivateProfileInt(_T("POS"), _T("MAP_POS_X"), 0, szFullPathName);
	g_MapPosY = GetPrivateProfileInt(_T("POS"), _T("MAP_POS_Y"), 0, szFullPathName);
	g_MapSizeX = GetPrivateProfileInt(_T("POS"), _T("MAP_SIZE_X"), 400, szFullPathName);
	g_MapSizeY = GetPrivateProfileInt(_T("POS"), _T("MAP_SIZE_Y"), 400, szFullPathName);
	g_NpcListPosX = GetPrivateProfileInt(_T("POS"), _T("NPCLIST_POS_X"), 0, szFullPathName);
	g_NpcListPosY = GetPrivateProfileInt(_T("POS"), _T("NPCLIST_POS_Y"), 400, szFullPathName);
	g_NpcListSizeX = GetPrivateProfileInt(_T("POS"), _T("NPCLIST_SIZE_X"), 400, szFullPathName);
	g_NpcListSizeY = GetPrivateProfileInt(_T("POS"), _T("NPCLIST_SIZE_Y"), 400, szFullPathName);

	//[MAP]
	GetPrivateProfileString(_T("MAP"), _T("MAPPATH"), _T(".\\map"), g_MapPath, sizeof(g_MapPath) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("MAP"), _T("HEX_TO_DEC"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_HexToDec = 1;
	}
	GetPrivateProfileString(_T("MAP"), _T("IMAGE_REVERSE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_IMAGE_REVERSE, 0);
	}
	GetPrivateProfileString(_T("MAP"), _T("DEFAULT_ZOOM_SETTING"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_DEFAULT, 0);
	}
	GetPrivateProfileString(_T("MAP"), _T("DEFAULT_ZOOM_01"), _T("12.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Zoom_01 = (float)atof(szBuf);
	GetPrivateProfileString(_T("MAP"), _T("DEFAULT_ZOOM_02"), _T("6.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Zoom_02 = (float)atof(szBuf);
	GetPrivateProfileString(_T("MAP"), _T("DEFAULT_ZOOM_04"), _T("3.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Zoom_04 = (float)atof(szBuf);
	GetPrivateProfileString(_T("MAP"), _T("ZOOM"), _T("3.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_Zoom = (float)atof(szBuf);
	GetPrivateProfileString(_T("MAP"), _T("ZOOM_MOUSE_DELTA"), _T("0.5"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_ZoomMouseDelta = (float)fabs(atof(szBuf));

	//[FONT]
	g_Font.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE"), 11, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD, 0);
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE"), _T(""), g_Font.szType, sizeof(g_Font.szType) / sizeof(_TCHAR), szFullPathName);
	g_FontEx.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE_EX"), 11, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD_EX"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_EX, 0);
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE_EX"), _T(""), g_FontEx.szType, sizeof(g_FontEx.szType) / sizeof(_TCHAR), szFullPathName);
	g_FontSp.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE_SP"), 11, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD_SP"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_SP, 0);
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE_SP"), _T(""), g_FontSp.szType, sizeof(g_FontEx.szType) / sizeof(_TCHAR), szFullPathName);
	g_FontPC.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE_PC"), 9, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD_PC"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_PC, 0);
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE_PC"), _T(""), g_FontPC.szType, sizeof(g_Font.szType) / sizeof(_TCHAR), szFullPathName);
	g_FontTarget.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE_TARGET"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD_TARGET"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_TARGET, 0);
	}
	else if (_tcscmp(szBuf, _T("")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_TARGET_USEDEFAULT, 0);
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE_TARGET"), _T(""), g_FontTarget.szType, sizeof(g_FontTarget.szType) / sizeof(_TCHAR), szFullPathName);
	g_FontSystem.nSize = GetPrivateProfileInt(_T("FONT"), _T("FONTSIZE_SYSTEM"), 11, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTBOLD_SYSTEM"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_FontSystem.nBold = 1;
	}
	GetPrivateProfileString(_T("FONT"), _T("FONTTYPE_SYSTEM"), _T(""), g_FontSystem.szType, sizeof(g_FontSystem.szType) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW, 0);
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE, 0);
	}
	g_Font.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X"), 0, szFullPathName);
	g_Font.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR"), _T("RGB(100,100,100)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_Font.ColorEffect = RGB(r, g, b);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_EX, 0);
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_EX, 0);
	}
	g_FontEx.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X_EX"), 0, szFullPathName);
	g_FontEx.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z_EX"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_EX"), _T("RGB(100,100,100)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_FontEx.ColorEffect = RGB(r, g, b);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_SP, 0);
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_SP, 0);
	}
	g_FontSp.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X_SP"), 0, szFullPathName);
	g_FontSp.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z_SP"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_SP"), _T("RGB(100,100,100)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_FontSp.ColorEffect = RGB(r, g, b);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_PC"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_PC, 0);
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_PC, 0);
	}
	g_FontPC.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X_PC"), 0, szFullPathName);
	g_FontPC.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z_PC"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_PC"), _T("RGB(100,100,100)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_FontPC.ColorEffect = RGB(r, g, b);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_TARGET"), _T("EDGE"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_TARGET, 0);
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_TARGET, 0);
	}
	g_FontTarget.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X_TARGET"), 0, szFullPathName);
	g_FontTarget.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z_TARGET"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_TARGET"), _T("RGB(0,64,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_FontTarget.ColorEffect = RGB(r, g, b);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_SYSTEM"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("DROPSHADOW")) == 0) {
		g_FontSystem.nEffect = 1;
	}
	else if (_tcscmp(szBuf, _T("EDGE")) == 0) {
		g_FontSystem.nEffect = 2;
	}
	g_FontSystem.nDS_x = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_X_SYSTEM"), 0, szFullPathName);
	g_FontSystem.nDS_z = GetPrivateProfileInt(_T("FONT"), _T("DROPSHADOW_POS_Z_SYSTEM"), 0, szFullPathName);
	GetPrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_SYSTEM"), _T("RGB(100,100,100)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_FontSystem.ColorEffect = RGB(r, g, b);

	//[MENU]
	GetPrivateProfileString(_T("MENU"), _T("CONTROL"), _T("0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("1")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_CONTROL_NAME, 0);
	}
	else {
		SendMessage(hWnd, WM_COMMAND, IDM_CONTROL_ID, 0);
	}

	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPCLIST"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_NPCLIST, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_TARGET_NAME"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_TARGET_NAME, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_TARGET_HPP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_TARGET_HPP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_TARGET_ID"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_TARGET_ID, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_NAME, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_HPP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ID, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_TIMER, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ONLY_VISIBLE, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME_EX"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_NAME_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_HPP_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ID_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_TIMER_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ONLY_VISIBLE_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME_SP"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_NAME_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_HPP_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ID_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_TIMER_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_NPC_ONLY_VISIBLE_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_OWN_DIRECTION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_OWN_DIRECTION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_OWN_POSITION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_OWN_POSITION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_OWN_LT_POS_GT"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_OWN_LT_POS_GT, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_OWN_TRACE_LINE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_OWN_TRACE_LINE, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_OWN_TRACE_POINT"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_OWN_TRACE_POINT, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PARTY_POSITION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PARTY_POSITION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PARTY_NAME"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PARTY_NAME, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PC_POSITION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PC_POSITION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PC_NAME"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PC_NAME, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PET_POSITION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PET_POSITION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_PET_NAME"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_PET_NAME, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_VISIBLE"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_VIS, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_ABILITY"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_ABL, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_MAGIC"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_MGC, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_RANGE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_RNG, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_WINDSONG"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_WINDSONG, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_DETECTION"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_CIRCLE_DETECTION, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_WEATHER_ICON"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_WEATHER_ICON, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("SHOW_WEATHER_SOUND"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_SHOW_WEATHER_SOUND, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("ALWAYS_ON_TOP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_ALWAYS_ON_TOP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("HIDE_TITLE_BAR"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_HIDE_TITLE_BAR, 0);
	}
	g_Menu.alpha_map = GetPrivateProfileInt(_T("MENU"), _T("ALPHA_VALUE_MAP"), 0, szFullPathName);
	GetPrivateProfileString(_T("MENU"), _T("TRANSPARENT_COLOR_MODE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_Menu.transparent = 1;
		g_Menu.alpha_map = 0;
	}
	g_Menu.alpha_npclist = GetPrivateProfileInt(_T("MENU"), _T("ALPHA_VALUE_NPCLIST"), 0, szFullPathName);
	GetPrivateProfileString(_T("MENU"), _T("POPSOUND"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_POPSOUND, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("STATESOUND"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_STATESOUND, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("POPSOUND_EX"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);//0.49で追加
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_POPSOUND_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("STATESOUND_EX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_STATESOUND_EX, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("POPSOUND_SP"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);//0.49で追加
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_POPSOUND_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("STATESOUND_SP"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_STATESOUND_SP, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("RADAR_MODE"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_RADAR_MODE, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("CENTER_OWN"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_CENTER_OWN, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("HIDE_HEIGHT_DIFF"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_HIDE_HEIGHT_DIFF, 0);
	}
	g_height_diff = GetPrivateProfileInt(_T("MENU"), _T("HEIGHT_DIFF_VALUE"), 8, szFullPathName);
	GetPrivateProfileString(_T("MENU"), _T("ALWAYS_UPDATE_AND_SET_ALL_ITEMS"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_Menu.always_update = 1;
	}
	GetPrivateProfileString(_T("MENU"), _T("FILE_AUTOLOAD"), _T("ON"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_Menu.file_autoload = 1;
	}
	GetPrivateProfileString(_T("MENU"), _T("NPCLISTPATH"), _T(".\\npclist"), g_NpcListPath, sizeof(g_NpcListPath) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("MENU"), _T("PRIORITY"), _T("BELOW_NORMAL"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("BELOW_NORMAL")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_PRIORITY_BELOW_NORMAL, 0);
	}
	else if (_tcscmp(szBuf, _T("IDLE")) == 0) {
		SendMessage(hWnd, WM_COMMAND, IDM_PRIORITY_IDLE, 0);
	}
	else {
		SendMessage(hWnd, WM_COMMAND, IDM_PRIORITY_NORMAL, 0);
	}
	GetPrivateProfileString(_T("MENU"), _T("HIDE_TOOL_BAR"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_Menu.hide_toolbar = 1;
	}
	GetPrivateProfileString(_T("MENU"), _T("HIDE_BUTTONBOX"), _T("OFF"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcscmp(szBuf, _T("ON")) == 0) {
		g_Menu.hide_buttonbox = 1;
	}
	g_Menu.column.id = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_ID"), 50, szFullPathName);
	g_Menu.column.name = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_NAME"), 147, szFullPathName);
	g_Menu.column.state = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_STATE"), 50, szFullPathName);
	g_Menu.column.timer = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_TIMER"), 50, szFullPathName);
	g_Menu.column.type = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_TYPE"), 40, szFullPathName);
	g_Menu.column.ym = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_YM"), 30, szFullPathName);//0.57で追加
	g_Menu.column.note = GetPrivateProfileInt(_T("MENU"), _T("COLUMN_NOTE"), 100, szFullPathName);

	//[LINK] 0.59で追加
	GetPrivateProfileString(_T("LINK"), _T("EXE"), _T("iexplore.exe"), g_Link.exe, sizeof(g_Link.exe) / sizeof(_TCHAR) - 1, szFullPathName);
	GetPrivateProfileString(_T("LINK"), _T("CMD"), NULL, g_Link.cmd, sizeof(g_Link.cmd) / sizeof(_TCHAR) - 1, szFullPathName);
	for (int i = 0; i < 4; i++) {
		swprintf(szBuf, _T("LINK%d"), i + 1);
		GetPrivateProfileString(_T("LINK"), szBuf, NULL, g_Link.link[i], sizeof(g_Link.link[0]) / sizeof(_TCHAR) - 1, szFullPathName);
		swprintf(szBuf, _T("URL%d"), i + 1);
		GetPrivateProfileString(_T("LINK"), szBuf, NULL, g_Link.url[i], sizeof(g_Link.url[0]) / sizeof(_TCHAR) - 1, szFullPathName);
	}

	//[SOUND]
	GetPrivateProfileString(_T("SOUND"), _T("SOUND1"), _T(""), g_WaveFileName[0][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND2"), _T(""), g_WaveFileName[1][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND3"), _T(""), g_WaveFileName[2][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND4"), _T(""), g_WaveFileName[3][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND5"), _T(""), g_WaveFileName[4][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND6"), _T(""), g_WaveFileName[5][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND7"), _T(""), g_WaveFileName[6][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND8"), _T(""), g_WaveFileName[7][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND9"), _T(""), g_WaveFileName[8][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND10"), _T(""), g_WaveFileName[9][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND11"), _T(""), g_WaveFileName[10][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("SOUND12"), _T(""), g_WaveFileName[11][0], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND"), _T("WEATHER_SOUND"), _T(""), g_WeatherWaveFileName, sizeof(g_WeatherWaveFileName) / sizeof(_TCHAR), szFullPathName);

	//[SOUND_EX]
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND1"), _T(""), g_WaveFileName[0][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND2"), _T(""), g_WaveFileName[1][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND3"), _T(""), g_WaveFileName[2][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND4"), _T(""), g_WaveFileName[3][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND5"), _T(""), g_WaveFileName[4][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND6"), _T(""), g_WaveFileName[5][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND7"), _T(""), g_WaveFileName[6][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND8"), _T(""), g_WaveFileName[7][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND9"), _T(""), g_WaveFileName[8][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND10"), _T(""), g_WaveFileName[9][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND11"), _T(""), g_WaveFileName[10][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_EX"), _T("SOUND12"), _T(""), g_WaveFileName[11][1], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);

	//[SOUND_SP]
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND1"), _T(""), g_WaveFileName[0][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND2"), _T(""), g_WaveFileName[1][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND3"), _T(""), g_WaveFileName[2][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND4"), _T(""), g_WaveFileName[3][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND5"), _T(""), g_WaveFileName[4][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND6"), _T(""), g_WaveFileName[5][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND7"), _T(""), g_WaveFileName[6][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND8"), _T(""), g_WaveFileName[7][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND9"), _T(""), g_WaveFileName[8][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND10"), _T(""), g_WaveFileName[9][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND11"), _T(""), g_WaveFileName[10][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);
	GetPrivateProfileString(_T("SOUND_SP"), _T("SOUND12"), _T(""), g_WaveFileName[11][2], sizeof(g_WaveFileName[0][0]) / sizeof(_TCHAR), szFullPathName);

	//[COLOR]
	GetPrivateProfileString(_T("COLOR"), _T("VISIBLE"), _T("RGB(0,0,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorVisible = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("ABILITY"), _T("RGB(255,0,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorAbility = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("MAGIC"), _T("RGB(0,255,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorMagic = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("RANGE"), _T("RGB(153,0,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorRange = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("WINDSONG"), _T("RGB(0,0,128)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorWindSong = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("DETECTION"), _T("RGB(0,128,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorDetection = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("MAPNAME"), _T("RGB(204,153,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorMapName = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("ZOOM"), _T("RGB(204,153,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorZoom = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("MYPOSITION"), _T("RGB(204,153,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorMyPosition = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("LT_POS_GT"), _T("RGB(204,153,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_Colorlt_Pos_gt = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("DISTANCE"), _T("RGB(204,153,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorDistance = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("POP"), _T("RGB(255,0,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorNPC_Pop[0] = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("SHARE"), _T("RGB(255,128,128)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorNPC_Share[0] = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("WAR"), _T("RGB(255,0,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorNPC_War[0] = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("CORPSE"), _T("RGB(80,80,80)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorNPC_Corpse[0] = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("OUT"), _T("RGB(0,255,0)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorNPC_Out[0] = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("OWN"), _T("RGB(0,0,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorOWN = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("PARTY"), _T("RGB(0,204,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorPARTY = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("ALIANCE"), _T("RGB(0,204,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorALIANCE = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("PC"), _T("RGB(0,0,255)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorPC = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("PET"), _T("RGB(0,100,200)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorPET = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("TARGET"), _T("RGB(0,255,102)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorTarget = RGB(r, g, b);
	GetPrivateProfileString(_T("COLOR"), _T("TRANSPARENT"), _T("RGB(1,1,1)"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
	g_ColorTransparent = RGB(r, g, b);


	//[COLOR_EX]
	GetPrivateProfileString(_T("COLOR_EX"), _T("POP"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Pop[1] = g_ColorNPC_Pop[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Pop[1] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_EX"), _T("SHARE"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Share[1] = g_ColorNPC_Share[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Share[1] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_EX"), _T("WAR"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_War[1] = g_ColorNPC_War[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_War[1] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_EX"), _T("CORPSE"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Corpse[1] = g_ColorNPC_Corpse[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Corpse[1] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_EX"), _T("OUT"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Out[1] = g_ColorNPC_Out[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Out[1] = RGB(r, g, b);
	}

	//[COLOR_SP]
	GetPrivateProfileString(_T("COLOR_SP"), _T("POP"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Pop[2] = g_ColorNPC_Pop[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Pop[2] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_SP"), _T("SHARE"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Share[2] = g_ColorNPC_Share[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Share[2] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_SP"), _T("WAR"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_War[2] = g_ColorNPC_War[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_War[2] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_SP"), _T("CORPSE"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Corpse[2] = g_ColorNPC_Corpse[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Corpse[2] = RGB(r, g, b);
	}
	GetPrivateProfileString(_T("COLOR_SP"), _T("OUT"), _T(""), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	if (_tcslen(szBuf) == 0) {
		g_ColorNPC_Out[2] = g_ColorNPC_Out[0];
	}
	else {
		_tscanf(szBuf, _T("RGB(%i,%i,%i)"), &r, &g, &b);
		g_ColorNPC_Out[2] = RGB(r, g, b);
	}

	//[POINT_SIZE]
	GetPrivateProfileString(_T("POINT_SIZE"), _T("OWN"), _T("4.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizeOWN = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("TRACE"), _T("3.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizeTRACE = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("NPC"), _T("4.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizeNPC = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("PARTY"), _T("3.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizePARTY = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("PC"), _T("3.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizePC = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("PET"), _T("2.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizePET = (float)atof(szBuf);
	GetPrivateProfileString(_T("POINT_SIZE"), _T("TARGET"), _T("4.0"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
	g_PointSizeTARGET = (float)atof(szBuf);
	g_TargetHighlight = GetPrivateProfileInt(_T("POINT_SIZE"), _T("TARGET_HIGHLIGHT"), 15, szFullPathName);


	return 1;
}

/// <summary>
/// ini書き込み
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hNPCList"></param>
/// <param name="hMenu"></param>
/// <param name="hMenuNPCList"></param>
/// <param name="filename"></param>
/// <returns></returns>
int WriteIniFiles(HWND hWnd, HWND hNPCList, HMENU hMenu, HMENU hMenuNPCList, const _TCHAR* filename)
{
	const _TCHAR OffOn[2][4] = { _T("OFF"),_T("ON") };
	_TCHAR szBuf[0x20];
	_TCHAR szFullPathName[MAX_PATH];
	int nRet = 1;

	if (0 == GetFullPathName(filename, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
		return 0;
	}

	//[POS]
	RECT rect_MAP, rect_LIST;
	if (GetWindowRect(hWnd, &rect_MAP) == 0) {
		return -1;
	}
	wsprintf(szBuf, _T("%d"), rect_MAP.left);
	nRet &= WritePrivateProfileString(_T("POS"), _T("MAP_POS_X"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_MAP.top);
	nRet &= WritePrivateProfileString(_T("POS"), _T("MAP_POS_Y"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_MAP.right - rect_MAP.left);
	nRet &= WritePrivateProfileString(_T("POS"), _T("MAP_SIZE_X"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_MAP.bottom - rect_MAP.top);
	nRet &= WritePrivateProfileString(_T("POS"), _T("MAP_SIZE_Y"), szBuf, szFullPathName);

	if (GetWindowRect(hNPCList, &rect_LIST) == 0) {
		return -2;
	}
	wsprintf(szBuf, _T("%d"), rect_LIST.left);
	nRet &= WritePrivateProfileString(_T("POS"), _T("NPCLIST_POS_X"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_LIST.top);
	nRet &= WritePrivateProfileString(_T("POS"), _T("NPCLIST_POS_Y"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_LIST.right - rect_LIST.left);
	nRet &= WritePrivateProfileString(_T("POS"), _T("NPCLIST_SIZE_X"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), rect_LIST.bottom - rect_LIST.top);
	nRet &= WritePrivateProfileString(_T("POS"), _T("NPCLIST_SIZE_Y"), szBuf, szFullPathName);

	//[MAP]
	nRet &= WritePrivateProfileString(_T("MAP"), _T("IMAGE_REVERSE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_IMAGE_REVERSE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MAP"), _T("DEFAULT_ZOOM_SETTING"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_ZOOM_DEFAULT)], szFullPathName);

	//[FONT]
	wsprintf(szBuf, _T("%d"), g_Font.nSize);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTSIZE"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD)], szFullPathName);
	if (GetMenuCheck(hWnd, hMenu, IDM_FONT_DROPSHADOW)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT"), _T("DROPSHADOW"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONT_EDGE)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT"), _T("EDGE"), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT"), _T("OFF"), szFullPathName);
	}
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_Font.ColorEffect & 0xFF, (g_Font.ColorEffect >> 8) & 0xFF, (g_Font.ColorEffect >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), g_FontEx.nSize);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTSIZE_EX"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD_EX)], szFullPathName);
	if (GetMenuCheck(hWnd, hMenu, IDM_FONT_DROPSHADOW_EX)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_EX"), _T("DROPSHADOW"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONT_EDGE_EX)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_EX"), _T("EDGE"), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_EX"), _T("OFF"), szFullPathName);
	}
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_FontEx.ColorEffect & 0xFF, (g_FontEx.ColorEffect >> 8) & 0xFF, (g_FontEx.ColorEffect >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_EX"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), g_FontSp.nSize);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTSIZE_SP"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD_SP)], szFullPathName);
	if (GetMenuCheck(hWnd, hMenu, IDM_FONT_DROPSHADOW_SP)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_SP"), _T("DROPSHADOW"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONT_EDGE_SP)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_SP"), _T("EDGE"), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_SP"), _T("OFF"), szFullPathName);
	}
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_FontSp.ColorEffect & 0xFF, (g_FontSp.ColorEffect >> 8) & 0xFF, (g_FontSp.ColorEffect >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_SP"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), g_FontPC.nSize);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTSIZE_PC"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_PC"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD_PC)], szFullPathName);
	if (GetMenuCheck(hWnd, hMenu, IDM_FONT_DROPSHADOW_PC)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_PC"), _T("DROPSHADOW"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONT_EDGE_PC)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_PC"), _T("EDGE"), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_PC"), _T("OFF"), szFullPathName);
	}
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_FontPC.ColorEffect & 0xFF, (g_FontPC.ColorEffect >> 8) & 0xFF, (g_FontPC.ColorEffect >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_PC"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), g_FontTarget.nSize);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTSIZE_TARGET"), szBuf, szFullPathName);
	if (GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD_TARGET)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_TARGET"), _T("ON"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONTBOLD_TARGET_USEDEFAULT)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_TARGET"), _T(""), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTBOLD_TARGET"), _T("OFF"), szFullPathName);
	}
	if (GetMenuCheck(hWnd, hMenu, IDM_FONT_DROPSHADOW_TARGET)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_TARGET"), _T("DROPSHADOW"), szFullPathName);
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_FONT_EDGE_TARGET)) {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_TARGET"), _T("EDGE"), szFullPathName);
	}
	else {
		nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_TARGET"), _T("OFF"), szFullPathName);
	}
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_FontTarget.ColorEffect & 0xFF, (g_FontTarget.ColorEffect >> 8) & 0xFF, (g_FontTarget.ColorEffect >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("FONT"), _T("FONTEFFECT_COLOR_TARGET"), szBuf, szFullPathName);


	//[MENU]
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPCLIST"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_NPCLIST)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_TARGET_NAME"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_TARGET_NAME)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_TARGET_HPP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_TARGET_HPP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_TARGET_ID"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_TARGET_ID)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_NAME)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_HPP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ID)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_TIMER)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ONLY_VISIBLE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_NAME_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_HPP_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ID_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_TIMER_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ONLY_VISIBLE_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_NAME_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_NAME_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_HPP_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_HPP_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ID_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ID_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_TIMER_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_TIMER_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_NPC_ONLYVISIBLE_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_NPC_ONLY_VISIBLE_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_OWN_DIRECTION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_OWN_DIRECTION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_OWN_POSITION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_OWN_POSITION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_OWN_LT_POS_GT"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_OWN_LT_POS_GT)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_OWN_TRACE_LINE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_OWN_TRACE_LINE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_OWN_TRACE_POINT"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_OWN_TRACE_POINT)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PARTY_POSITION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PARTY_POSITION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PARTY_NAME"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PARTY_NAME)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PC_POSITION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PC_POSITION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PC_NAME"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PC_NAME)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PET_POSITION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PET_POSITION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_PET_NAME"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_PET_NAME)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_VISIBLE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_VIS)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_ABILITY"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_ABL)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_MAGIC"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_MGC)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_RANGE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_RNG)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_WINDSONG"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_WINDSONG)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_CIRCLE_DETECTION"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_CIRCLE_DETECTION)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_WEATHER_ICON"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_WEATHER_ICON)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("SHOW_WEATHER_SOUND"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_SHOW_WEATHER_SOUND)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("ALWAYS_ON_TOP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_ALWAYS_ON_TOP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("HIDE_TITLE_BAR"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_HIDE_TITLE_BAR)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("RADAR_MODE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_RADAR_MODE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("CENTER_OWN"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_CENTER_OWN)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("HIDE_HEIGHT_DIFF"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_HIDE_HEIGHT_DIFF)], szFullPathName);//0.56で追加
	if (GetMenuCheck(hWnd, hMenu, IDM_PRIORITY_BELOW_NORMAL)) {
		wsprintf(szBuf, _T("%s"), _T("BELOW_NORMAL"));
	}
	else if (GetMenuCheck(hWnd, hMenu, IDM_PRIORITY_IDLE)) {
		wsprintf(szBuf, _T("%s"), _T("IDLE"));
	}
	else {
		wsprintf(szBuf, _T("%s"), _T("NORMAL"));
	}
	nRet &= WritePrivateProfileString(_T("MENU"), _T("PRIORITY"), szBuf, szFullPathName);
	wsprintf(szBuf, _T("%d"), g_Menu.alpha_map);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("ALPHA_VALUE_MAP"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("TRANSPARENT_COLOR_MODE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_TRANSPARENT_MODE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("RADAR_MODE"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_RADAR_MODE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("POPSOUND"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_POPSOUND)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("STATESOUND"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_STATESOUND)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("POPSOUND_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_POPSOUND_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("STATESOUND_EX"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_STATESOUND_EX)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("POPSOUND_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_POPSOUND_SP)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("STATESOUND_SP"), OffOn[GetMenuCheck(hWnd, hMenu, IDM_STATESOUND_SP)], szFullPathName);
	wsprintf(szBuf, _T("%d"), g_Menu.alpha_npclist);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("ALPHA_VALUE_NPCLIST"), szBuf, szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("ALWAYS_UPDATE_AND_SET_ALL_ITEMS"), OffOn[GetMenuCheck(hWnd, hMenuNPCList, IDM_LIST_ALWAYS_UPDATE)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("FILE_AUTOLOAD"), OffOn[GetMenuCheck(hWnd, hMenuNPCList, IDM_FILE_AUTOLOAD)], szFullPathName);
	nRet &= WritePrivateProfileString(_T("MENU"), _T("HIDE_TOOL_BAR"), OffOn[GetMenuCheck(hWnd, hMenuNPCList, IDM_HIDE_TOOL_BAR)], szFullPathName);//0.50で追加
	nRet &= WritePrivateProfileString(_T("MENU"), _T("HIDE_BUTTONBOX"), OffOn[GetMenuCheck(hWnd, hMenuNPCList, IDM_HIDE_BUTTONBOX)], szFullPathName);//0.50で追加
																																					 //カラムの幅　0.50で追加
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 0));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_ID"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 1));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_NAME"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 2));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_STATE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 3));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_TIMER"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 4));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_TYPE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 5));
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_YM"), szBuf, szFullPathName);//0.59で追加
	_stprintf(szBuf, _T("%d"), ListView_GetColumnWidth(g_hList, 6));//0.59で追加
	nRet &= WritePrivateProfileString(_T("MENU"), _T("COLUMN_NOTE"), szBuf, szFullPathName);


	//[COLOR]
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorVisible & 0xFF, (g_ColorVisible >> 8) & 0xFF, (g_ColorVisible >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("VISIBLE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorAbility & 0xFF, (g_ColorAbility >> 8) & 0xFF, (g_ColorAbility >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("ABILITY"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorMagic & 0xFF, (g_ColorMagic >> 8) & 0xFF, (g_ColorMagic >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("MAGIC"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorRange & 0xFF, (g_ColorRange >> 8) & 0xFF, (g_ColorRange >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("RANGE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorWindSong & 0xFF, (g_ColorWindSong >> 8) & 0xFF, (g_ColorWindSong >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("WINDSONG"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorDetection & 0xFF, (g_ColorDetection >> 8) & 0xFF, (g_ColorDetection >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("DETECTION"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorMapName & 0xFF, (g_ColorMapName >> 8) & 0xFF, (g_ColorMapName >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("MAPNAME"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorZoom & 0xFF, (g_ColorZoom >> 8) & 0xFF, (g_ColorZoom >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("ZOOM"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorMyPosition & 0xFF, (g_ColorMyPosition >> 8) & 0xFF, (g_ColorMyPosition >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("MYPOSITION"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_Colorlt_Pos_gt & 0xFF, (g_Colorlt_Pos_gt >> 8) & 0xFF, (g_Colorlt_Pos_gt >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("LT_POS_GT"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorDistance & 0xFF, (g_ColorDistance >> 8) & 0xFF, (g_ColorDistance >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("DISTANCE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Pop[0] & 0xFF, (g_ColorNPC_Pop[0] >> 8) & 0xFF, (g_ColorNPC_Pop[0] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("POP"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Share[0] & 0xFF, (g_ColorNPC_Share[0] >> 8) & 0xFF, (g_ColorNPC_Share[0] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("SHARE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_War[0] & 0xFF, (g_ColorNPC_War[0] >> 8) & 0xFF, (g_ColorNPC_War[0] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("WAR"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Corpse[0] & 0xFF, (g_ColorNPC_Corpse[0] >> 8) & 0xFF, (g_ColorNPC_Corpse[0] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("CORPSE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Out[0] & 0xFF, (g_ColorNPC_Out[0] >> 8) & 0xFF, (g_ColorNPC_Out[0] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("OUT"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorOWN & 0xFF, (g_ColorOWN >> 8) & 0xFF, (g_ColorOWN >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("OWN"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorPARTY & 0xFF, (g_ColorPARTY >> 8) & 0xFF, (g_ColorPARTY >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("PARTY"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorALIANCE & 0xFF, (g_ColorALIANCE >> 8) & 0xFF, (g_ColorALIANCE >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("ALIANCE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorPC & 0xFF, (g_ColorPC >> 8) & 0xFF, (g_ColorPC >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("PC"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorPET & 0xFF, (g_ColorPET >> 8) & 0xFF, (g_ColorPET >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("PET"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorTarget & 0xFF, (g_ColorTarget >> 8) & 0xFF, (g_ColorTarget >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR"), _T("TARGET"), szBuf, szFullPathName);

	//[COLOR_EX]
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Pop[1] & 0xFF, (g_ColorNPC_Pop[1] >> 8) & 0xFF, (g_ColorNPC_Pop[1] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_EX"), _T("POP"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Share[1] & 0xFF, (g_ColorNPC_Share[1] >> 8) & 0xFF, (g_ColorNPC_Share[1] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_EX"), _T("SHARE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_War[1] & 0xFF, (g_ColorNPC_War[1] >> 8) & 0xFF, (g_ColorNPC_War[1] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_EX"), _T("WAR"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Corpse[1] & 0xFF, (g_ColorNPC_Corpse[1] >> 8) & 0xFF, (g_ColorNPC_Corpse[1] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_EX"), _T("CORPSE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Out[1] & 0xFF, (g_ColorNPC_Out[1] >> 8) & 0xFF, (g_ColorNPC_Out[1] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_EX"), _T("OUT"), szBuf, szFullPathName);

	//[COLOR_SP]
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Pop[2] & 0xFF, (g_ColorNPC_Pop[2] >> 8) & 0xFF, (g_ColorNPC_Pop[2] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_SP"), _T("POP"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Share[2] & 0xFF, (g_ColorNPC_Share[2] >> 8) & 0xFF, (g_ColorNPC_Share[2] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_SP"), _T("SHARE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_War[2] & 0xFF, (g_ColorNPC_War[2] >> 8) & 0xFF, (g_ColorNPC_War[2] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_SP"), _T("WAR"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Corpse[2] & 0xFF, (g_ColorNPC_Corpse[2] >> 8) & 0xFF, (g_ColorNPC_Corpse[2] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_SP"), _T("CORPSE"), szBuf, szFullPathName);
	_stprintf(szBuf, _T("RGB(%i,%i,%i)"), g_ColorNPC_Out[2] & 0xFF, (g_ColorNPC_Out[2] >> 8) & 0xFF, (g_ColorNPC_Out[2] >> 16) & 0xFF);
	nRet &= WritePrivateProfileString(_T("COLOR_SP"), _T("OUT"), szBuf, szFullPathName);

	return nRet;
}

//オフセット値の簡易チェック
int CheckOffset(HWND hWnd,const _TCHAR* filename)
{
	int nFlag = 0;
	_TCHAR szText[0x50];
	_TCHAR szNpcmap[0x10] = _T("");
	_TCHAR szNowsta[0x10] = _T("");
	_TCHAR szNpclist[0x10] = _T("");

	//NPCMAP
	if(g_Offset.NpcMap == 0 || g_Offset.NpcMap >= g_DllSize){
		_stprintf(szNpcmap, _T("\nNPCMAP=?"));
		nFlag = 1;
	}

	//NOWSTA
	if(g_Offset.NowSta == 0 || g_Offset.NowSta >= g_DllSize){
		_stprintf(szNowsta, _T("\nNOWSTA=?"));
		nFlag = 2;
	}
	else{
		INFORMATION mypos = GetMyInfo();
		if(fabs(mypos.x) > 10000 || fabs(mypos.y) > 10000 || fabs(mypos.z) > 10000){
			nFlag = 3;
		}
	}

	//NPCLIST
	if(g_Offset.NpcList == 0 || g_Offset.NpcList >= g_DllSize){
		_stprintf(szNpclist, _T("\nNPCLIST=?"));
		nFlag = 4;
	}


	//TIMER
	if(g_Timer < 100){
		g_Timer = 500;
	}

	//zoom
	if(g_Zoom < 0.5){
		g_Zoom = (float)3.0;
	}

	//DefaultZoom
	if(g_Zoom_01 < 0.1){
		g_Zoom_01 = (float)12.0;
	}
	if(g_Zoom_02 < 0.1){
		g_Zoom_02 = (float)6.0;
	}
	if(g_Zoom_04 < 0.1){
		g_Zoom_04 = (float)3.0;
	}

	//Alpha
	if(g_Menu.alpha_map < 0 || g_Menu.alpha_map > 100){
		g_Menu.alpha_map = 0;
	}
	if(g_Menu.alpha_npclist < 0 || g_Menu.alpha_npclist > 90){
		g_Menu.alpha_npclist = 0;
	}

	//height_diff 0.56で追加
	if(g_height_diff == 0)g_height_diff = 8;
	else if(g_height_diff < 0)g_height_diff = -g_height_diff;

	//ショートカットキーの設定
	if((g_KeyZoomPlus>>8) != 0x00 && (g_KeyZoomPlus>>8) != VK_SHIFT && (g_KeyZoomPlus>>8) != VK_CONTROL && (g_KeyZoomPlus>>8) != VK_MENU){
		g_KeyZoomPlus = g_KeyZoomPlus&0xFF;
	}
	if((g_KeyZoomPlus_1>>8) != 0x00 && (g_KeyZoomPlus_1>>8) != VK_SHIFT && (g_KeyZoomPlus_1>>8) != VK_CONTROL && (g_KeyZoomPlus_1>>8) != VK_MENU){
		g_KeyZoomPlus_1 = g_KeyZoomPlus_1&0xFF;
	}
	if((g_KeyZoomMinus>>8) != 0x00 && (g_KeyZoomMinus>>8) != VK_SHIFT && (g_KeyZoomMinus>>8) != VK_CONTROL && (g_KeyZoomMinus>>8) != VK_MENU){
		g_KeyZoomMinus = g_KeyZoomMinus&0xFF;
	}
	if((g_KeyZoomMinus_1>>8) != 0x00 && (g_KeyZoomMinus_1>>8) != VK_SHIFT && (g_KeyZoomMinus_1>>8) != VK_CONTROL && (g_KeyZoomMinus_1>>8) != VK_MENU){
		g_KeyZoomMinus_1 = g_KeyZoomMinus_1&0xFF;
	}
	
	if((g_KeyZoomPlus&0xFF) >= 0x90 || (g_KeyZoomPlus_1&0xFF) >= 0x90 || (g_KeyZoomMinus&0xFF) >= 0x90 || (g_KeyZoomMinus_1&0xFF) >= 0x90){
		g_KeyZoomPlus = 0x0021;
		g_KeyZoomPlus_1 = 0x1021;
		g_KeyZoomMinus = 0x0022;
		g_KeyZoomMinus_1 = 0x1022;
	}

	//Mappu.iniのフルパスを取得
	_TCHAR szFullPathName[MAX_PATH];
	GetFullPathName(filename, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL);
	if (g_DLLFileSizeCmp != 0 && g_DLLFileSize != g_DLLFileSizeCmp) {
		_TCHAR szBuf[0x10];
		if (IDYES == MessageBox(hWnd, _T("FFXiMain.dll更新\n[OFFSET]アドレスを検索してみますか？"), _T("Mappu"), MB_YESNO | MB_ICONQUESTION)) {
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				MessageBox(hWnd, _T("オフセット取得に失敗"), NULL, MB_ICONERROR);
			}
			_TCHAR szBuf[MAX_PATH];
			_stprintf(szBuf, _T("Mappu.iniを上書きしますか？\n[OFFSET]\nNPCMAP = %X\nNPCLIST = %X\nNOWSTA = %X\nTARGET = %X\nWEATHER = %X"), g_Offset.NpcMap, g_Offset.NpcList, g_Offset.NowSta, g_Offset.Target, g_Offset.Weather);
			if (g_Offset.NpcMap == 0 || g_Offset.NowSta == 0 || g_Offset.NpcList == 0 || g_Offset.Target == 0 || g_Offset.Weather == 0) {
				_stprintf(szBuf, _T("オフセット取得に失敗!\n[OFFSET]\nNPCMAP = %X\nNPCLIST = %X\nNOWSTA = %X\nTARGET = %X\nWEATHER = %X"), g_Offset.NpcMap, g_Offset.NpcList, g_Offset.NowSta, g_Offset.Target, g_Offset.Weather);
				MessageBox(hWnd, szBuf, _T("Mappu"), MB_OK | MB_ICONERROR);
				DestroyWindow(hWnd);
			}
			else if (IDYES == MessageBox(hWnd, szBuf, _T("Mappu"), MB_YESNO | MB_ICONQUESTION)) {
				int nRet = 1;

				if (0 == GetFullPathName(filename, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
					return 0;
				}
				wsprintf(szBuf, _T("%X"), g_Offset.NpcMap);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NPCMAP"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.NpcList);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NPCLIST"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.NowSta);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NOWSTA"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.Target);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("TARGET"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.Weather);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("WEATHER"), szBuf, szFullPathName);

				if (nRet == 0) {
					MessageBox(hWnd, _T("書き込みエラー!"), _T("Mappu"), MB_OK | MB_ICONERROR);
				}
			}
		}
		wsprintf(szBuf, _T("%d"), g_DLLFileSize);
		WritePrivateProfileString(_T("OTHER"), _T("DLLFILESIZE"), szBuf, szFullPathName);
		nFlag = 0;
	}

	if (nFlag) {
		_stprintf(szText, _T("[OFFSET]アドレスを検索しますか？%s%s%s"), szNpcmap, szNowsta, szNpclist);
		if (IDYES == MessageBox(hWnd, szText, _T("Mappu.ini [OFFSET] error"), MB_YESNO | MB_ICONQUESTION)) {
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				MessageBox(hWnd, _T("オフセット取得に失敗"), NULL, MB_ICONERROR);
			}
			_TCHAR szBuf[MAX_PATH];
			_stprintf(szBuf, _T("Mappu.iniを上書きしますか？\n[OFFSET]\nNPCMAP = %X\nNPCLIST = %X\nNOWSTA = %X\nTARGET = %X\nWEATHER = %X"), g_Offset.NpcMap, g_Offset.NpcList, g_Offset.NowSta, g_Offset.Target, g_Offset.Weather);
			if (g_Offset.NpcMap == 0 || g_Offset.NowSta == 0 || g_Offset.NpcList == 0 || g_Offset.Target == 0 || g_Offset.Weather == 0) {
				_stprintf(szBuf, _T("オフセット取得に失敗!\n[OFFSET]\nNPCMAP = %X\nNPCLIST = %X\nNOWSTA = %X\nTARGET = %X\nWEATHER = %X"), g_Offset.NpcMap, g_Offset.NpcList, g_Offset.NowSta, g_Offset.Target, g_Offset.Weather);
				MessageBox(hWnd, szBuf, _T("Mappu"), MB_OK | MB_ICONERROR);
				DestroyWindow(hWnd);
			}
			else if (IDYES == MessageBox(hWnd, szBuf, _T("Mappu"), MB_YESNO | MB_ICONQUESTION)) {
				int nRet = 1;

				if (0 == GetFullPathName(filename, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
					return 0;
				}
				wsprintf(szBuf, _T("%X"), g_Offset.NpcMap);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NPCMAP"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.NpcList);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NPCLIST"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.NowSta);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("NOWSTA"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.Target);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("TARGET"), szBuf, szFullPathName);
				wsprintf(szBuf, _T("%X"), g_Offset.Weather);
				nRet &= WritePrivateProfileString(_T("OFFSET"), _T("WEATHER"), szBuf, szFullPathName);

				if (nRet == 0) {
					MessageBox(hWnd, _T("書き込みエラー!"), _T("Mappu"), MB_OK | MB_ICONERROR);
				}
			}
		}
		else {
			DestroyWindow(hWnd);
		}
	}

	return 1;
}

/// <summary>
/// メニューのチェック⇔アンチェック切り替え
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hMenu"></param>
/// <param name="uID"></param>
/// <returns></returns>
BYTE ChangeMenuCheck(HWND hWnd,HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	BYTE byRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		g_ErrorCode = 201;
		ErrorNum(hWnd);
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
		g_ErrorCode = 202;
		ErrorNum(hWnd);
	}
	
	return byRet;
}

/// <summary>
/// メニューの状態を確認
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hMenu"></param>
/// <param name="uID"></param>
/// <returns>チェック時:1、未チェック時:0</returns>
UINT GetMenuCheck(HWND hWnd,HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	UINT uRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		g_ErrorCode = 211;
		ErrorNum(hWnd);
	}
	if(mii.fState == MFS_CHECKED){//チェックされている場合
		uRet = 1;
	}
	else{//チェックされていない場合
		uRet = 0;
	}
	
	return uRet;
}

/// <summary>
/// メニューからフォント情報を取得
/// </summary>
/// <param name="hMenu">メニューハンドラ</param>
/// <param name="dwStartID">開始ID</param>
/// <param name="chFontSize"></param>
/// <param name="nNum"></param>
/// <param name="nSizeOfchFontSize"></param>
/// <returns></returns>
int FontSizeSet(HMENU hMenu, DWORD dwStartID, const char* chFontSize, int nNum, int nSizeOfchFontSize)
{
	MENUITEMINFO mii;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	//uncheck
	mii.fState = MFS_UNCHECKED;
	for (int i = 0; i < nSizeOfchFontSize; i++) {
		if (SetMenuItemInfo(hMenu, dwStartID + i, FALSE, &mii) == 0) {
			g_ErrorCode = 300;
			return 0;
		}
	}

	if (nNum < 0) {
		return -1;
	}

	//check
	mii.fState = MFS_CHECKED;
	if (SetMenuItemInfo(hMenu, dwStartID + nNum, FALSE, &mii) == 0) {
		g_ErrorCode = 301;
		return 0;
	}

	return chFontSize[nNum];
}

//フォントを作成
int MakeFont(HWND hWnd) {
	HDC hDC;
	LOGFONT logFont;

	//開放
	DeleteObject(g_Font.hFont);
	DeleteObject(g_FontEx.hFont);
	DeleteObject(g_FontSystem.hFont);

	hDC = GetDC(hWnd);
	//Normal
	ZeroMemory(&logFont, sizeof(logFont));
	logFont.lfCharSet = SHIFTJIS_CHARSET;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	logFont.lfHeight = -MulDiv(g_Font.nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	strcpy(logFont.lfFaceName, g_Font.szType);
	if (g_Font.nBold == 1) {
		logFont.lfWeight = FW_BOLD;
	}
	g_Font.hFont = CreateFontIndirect(&logFont);

	//Ex
	ZeroMemory(&logFont, sizeof(logFont));
	logFont.lfCharSet = SHIFTJIS_CHARSET;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	logFont.lfHeight = -MulDiv(g_FontEx.nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	strcpy(logFont.lfFaceName, g_FontEx.szType);
	if (g_FontEx.nBold == 1) {
		logFont.lfWeight = FW_BOLD;
	}
	g_FontEx.hFont = CreateFontIndirect(&logFont);

	//Sp
	ZeroMemory(&logFont, sizeof(logFont));
	logFont.lfCharSet = SHIFTJIS_CHARSET;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	logFont.lfHeight = -MulDiv(g_FontSp.nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	strcpy(logFont.lfFaceName, g_FontSp.szType);
	if (g_FontSp.nBold == 1) {
		logFont.lfWeight = FW_BOLD;
	}
	g_FontSp.hFont = CreateFontIndirect(&logFont);

	//System
	ZeroMemory(&logFont, sizeof(logFont));
	logFont.lfHeight = -MulDiv(g_FontSystem.nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	logFont.lfCharSet = SHIFTJIS_CHARSET;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	strcpy(logFont.lfFaceName, g_FontSystem.szType);
	if (g_FontSystem.nBold == 1) {
		logFont.lfWeight = FW_BOLD;
	}
	g_FontSystem.hFont = CreateFontIndirect(&logFont);

	//PC
	ZeroMemory(&logFont, sizeof(logFont));
	logFont.lfHeight = -MulDiv(g_FontPC.nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	logFont.lfCharSet = SHIFTJIS_CHARSET;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	strcpy(logFont.lfFaceName, g_FontPC.szType);
	if (g_FontPC.nBold == 1) {
		logFont.lfWeight = FW_BOLD;
	}
	g_FontPC.hFont = CreateFontIndirect(&logFont);


	DeleteDC(hDC);
	if (g_Font.hFont == 0 || g_FontEx.hFont == 0 || g_FontSystem.hFont == 0 || g_FontPC.hFont == 0) {
		return -1;
	}

	return 0;
}

//優先度の設定
int SetMenuPriority(HWND hWnd, HMENU hMenu, int id, DWORD dwPriorityClass)
{
	MENUITEMINFO mii;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	//uncheck
	mii.fState = MFS_UNCHECKED;
	if (SetMenuItemInfo(hMenu, IDM_PRIORITY_NORMAL, FALSE, &mii) == 0) {
		return 1;
	}
	if (SetMenuItemInfo(hMenu, IDM_PRIORITY_BELOW_NORMAL, FALSE, &mii) == 0) {
		return 2;
	}
	if (SetMenuItemInfo(hMenu, IDM_PRIORITY_IDLE, FALSE, &mii) == 0) {
		return 3;
	}

	//check
	mii.fState = MFS_CHECKED;
	if (SetMenuItemInfo(hMenu, id, FALSE, &mii) == 0) {
		return 5;
	}

	//優先度
	if (SetPriorityClass(GetCurrentProcess(), dwPriorityClass) == 0) {
		return 10;
	}

	return 0;
}

int AlphaValueSetMAP(HWND hWnd, HMENU hMenu, int iAlpha)
{
	MENUITEMINFO mii;

	if (iAlpha < 0 || iAlpha > 100) {
		g_ErrorCode = 400;
		return -1;
	}

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	//uncheck
	mii.fState = MFS_UNCHECKED;
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_0, FALSE, &mii) == 0) {
		g_ErrorCode = 401;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_10, FALSE, &mii) == 0) {
		g_ErrorCode = 402;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_20, FALSE, &mii) == 0) {
		g_ErrorCode = 403;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_30, FALSE, &mii) == 0) {
		g_ErrorCode = 404;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_40, FALSE, &mii) == 0) {
		g_ErrorCode = 405;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_50, FALSE, &mii) == 0) {
		g_ErrorCode = 406;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_60, FALSE, &mii) == 0) {
		g_ErrorCode = 407;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_70, FALSE, &mii) == 0) {
		g_ErrorCode = 408;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_80, FALSE, &mii) == 0) {
		g_ErrorCode = 409;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_90, FALSE, &mii) == 0) {
		g_ErrorCode = 410;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_100, FALSE, &mii) == 0) {
		g_ErrorCode = 411;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_TRANSPARENT_MODE, FALSE, &mii) == 0) {
		g_ErrorCode = 412;
		return -1;
	}

	//check
	mii.fState = MFS_CHECKED;

	if (g_Menu.transparent) {
		if (SetMenuItemInfo(hMenu, IDM_TRANSPARENT_MODE, FALSE, &mii) == 0) {
			g_ErrorCode = 421;
			return -1;
		}
	}
	else {
		switch (iAlpha) {
		case 0:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_0, FALSE, &mii) == 0) {
				g_ErrorCode = 422;
				return -1;
			}
			break;
		case 10:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_10, FALSE, &mii) == 0) {
				g_ErrorCode = 423;
				return -1;
			}
			break;
		case 20:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_20, FALSE, &mii) == 0) {
				g_ErrorCode = 424;
				return -1;
			}
			break;
		case 30:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_30, FALSE, &mii) == 0) {
				g_ErrorCode = 425;
				return -1;
			}
			break;
		case 40:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_40, FALSE, &mii) == 0) {
				g_ErrorCode = 426;
				return -1;
			}
			break;
		case 50:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_50, FALSE, &mii) == 0) {
				g_ErrorCode = 427;
				return -1;
			}
			break;
		case 60:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_60, FALSE, &mii) == 0) {
				g_ErrorCode = 428;
				return -1;
			}
			break;
		case 70:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_70, FALSE, &mii) == 0) {
				g_ErrorCode = 429;
				return -1;
			}
			break;
		case 80:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_80, FALSE, &mii) == 0) {
				g_ErrorCode = 430;
				return -1;
			}
			break;
		case 90:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_90, FALSE, &mii) == 0) {
				g_ErrorCode = 431;
				return -1;
			}
			break;
		case 100:
			if (SetMenuItemInfo(hMenu, IDM_ALPHA_MAP_100, FALSE, &mii) == 0) {
				g_ErrorCode = 432;
				return -1;
			}
			break;
		}
	}

	LONG exStyle;
	if (g_Menu.transparent) {
		//マップに透過色を設定する
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//現在のSTYLEを取得
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);//設定
		SetLayeredWindowAttributes(hWnd, g_ColorTransparent, NULL, LWA_COLORKEY);
	}
	else if (iAlpha == 0) {//透過モードはメニュー表示の遅延につながるのでAlpha==0の時はスタイルを変更
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//現在のSTYLEを取得
		exStyle &= ~WS_EX_LAYERED;//レイヤーモードを削除
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);//設定
	}
	else {
		//透過率を設定する
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//現在のSTYLEを取得
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);//設定
		SetLayeredWindowAttributes(hWnd, NULL, (BYTE)(0xFF * (100 - iAlpha) / 100), LWA_ALPHA);
	}

	g_Menu.alpha_map = iAlpha;

	return iAlpha;
}


int AlphaValueSetNPCLIST(HWND hWnd, HMENU hMenu, int iAlpha)
{
	MENUITEMINFO mii;

	if (iAlpha < 0 || iAlpha > 90) {
		g_ErrorCode = 450;
		return -1;
	}

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	//uncheck
	mii.fState = MFS_UNCHECKED;
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_0, FALSE, &mii) == 0) {
		g_ErrorCode = 451;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_10, FALSE, &mii) == 0) {
		g_ErrorCode = 452;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_20, FALSE, &mii) == 0) {
		g_ErrorCode = 453;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_30, FALSE, &mii) == 0) {
		g_ErrorCode = 454;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_40, FALSE, &mii) == 0) {
		g_ErrorCode = 455;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_50, FALSE, &mii) == 0) {
		g_ErrorCode = 456;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_60, FALSE, &mii) == 0) {
		g_ErrorCode = 457;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_70, FALSE, &mii) == 0) {
		g_ErrorCode = 458;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_80, FALSE, &mii) == 0) {
		g_ErrorCode = 459;
		return -1;
	}
	if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_90, FALSE, &mii) == 0) {
		g_ErrorCode = 460;
		return -1;
	}

	//check
	mii.fState = MFS_CHECKED;
	switch (iAlpha) {
	case 0:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_0, FALSE, &mii) == 0) {
			g_ErrorCode = 471;
			return -1;
		}
		break;
	case 10:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_10, FALSE, &mii) == 0) {
			g_ErrorCode = 472;
			return -1;
		}
		break;
	case 20:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_20, FALSE, &mii) == 0) {
			g_ErrorCode = 473;
			return -1;
		}
		break;
	case 30:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_30, FALSE, &mii) == 0) {
			g_ErrorCode = 474;
			return -1;
		}
		break;
	case 40:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_40, FALSE, &mii) == 0) {
			g_ErrorCode = 475;
			return -1;
		}
		break;
	case 50:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_50, FALSE, &mii) == 0) {
			g_ErrorCode = 476;
			return -1;
		}
		break;
	case 60:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_60, FALSE, &mii) == 0) {
			g_ErrorCode = 477;
			return -1;
		}
		break;
	case 70:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_70, FALSE, &mii) == 0) {
			g_ErrorCode = 478;
			return -1;
		}
		break;
	case 80:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_80, FALSE, &mii) == 0) {
			g_ErrorCode = 479;
			return -1;
		}
		break;
	case 90:
		if (SetMenuItemInfo(hMenu, IDM_ALPHA_NPCLIST_90, FALSE, &mii) == 0) {
			g_ErrorCode = 480;
			return -1;
		}
		break;
	}

	LONG exStyle;
	if (iAlpha == 0) {//透過モードはメニュー表示の遅延につながるのでAlpha==0の時はスタイルを変更
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//現在のSTYLEを取得
		exStyle &= ~WS_EX_LAYERED;//レイヤーモードを削除
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);//設定
	}
	else {
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//現在のSTYLEを取得
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);//設定
		SetLayeredWindowAttributes(hWnd, NULL, (BYTE)(0xFF * (100 - iAlpha) / 100), LWA_ALPHA);
	}

	g_Menu.alpha_npclist = iAlpha;

	return iAlpha;
}

//エリア変更があった時NPCLISTにIDM_LIST_UPDATEとIDM_LIST_CHECKを送信
int AlwaysUpdateList(HWND hNPCLIST)
{
	static WORD pastAreaID = 0;
	WORD nowAreaID;

	//エリアID取得
	nowAreaID = GetAreaID();

	//エリアが変わった時
	if (nowAreaID != pastAreaID) {
		SendMessage(hNPCLIST, WM_COMMAND, IDM_LIST_UPDATE, 0);
		SendMessage(hNPCLIST, WM_COMMAND, IDM_LIST_SET_ALL, 0);
	}
	else if (0 == ListView_GetItemCount(GetDlgItem(hNPCLIST, IDC_LIST))) {//NPCLISTの作成が早すぎてリスト数0の時
		SendMessage(hNPCLIST, WM_COMMAND, IDM_LIST_UPDATE, 0);
		SendMessage(hNPCLIST, WM_COMMAND, IDM_LIST_SET_ALL, 0);
	}

	pastAreaID = nowAreaID;

	return 1;
}

//エリア変更があった時NPCLISTにIDM_FILE_LOADを送信
int FileAutoLoad(HWND hNPCLIST)
{
	static WORD pastAreaID = 0;
	WORD nowAreaID;

	//エリアID取得
	nowAreaID = GetAreaID();

	//エリアが変わった時
	if (nowAreaID != pastAreaID) {
		SendMessage(hNPCLIST, WM_COMMAND, IDM_FILE_LOAD, IDM_FILE_AUTOLOAD);
	}
	else if (0 == ListView_GetItemCount(GetDlgItem(hNPCLIST, IDC_LIST))) {//NPCLISTの作成が早すぎてリスト数0の時
		SendMessage(hNPCLIST, WM_COMMAND, IDM_FILE_LOAD, IDM_FILE_AUTOLOAD);
	}

	pastAreaID = nowAreaID;

	return 1;
}

struct _TREEITEM
{
	//これがないとなぜかリンカーがエラーを吐く
};

int InitTree(HWND hTreeWnd)
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrls.dwICC = ICC_TREEVIEW_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	//ツリービューにEXスタイル付与
	TreeView_SetExtendedStyle(GetDlgItem(hTreeWnd, IDC_TREE),
		TVS_CHECKBOXES | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_SINGLEEXPAND | TVS_TRACKSELECT,
		TVS_CHECKBOXES | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_SINGLEEXPAND | TVS_TRACKSELECT);

	HTREEITEM hNPCLIST, hControl, hZoom, hShow, hSound, hOther, hSelect, hTarget, hNPC, hFont,
		hTarget_Font, hNPC_N_Font, hNPC_E_Font, hNPC_S_Font, hPlayer_Font;
	TVINSERTSTRUCT tvis;

	//[+]
	/* │  */tvis.hParent = TVI_ROOT;
	/* │  */tvis.hInsertAfter = TVI_LAST; // TVI_FIRST, TVI_LAST, TVI_SORT
	/* │  */tvis.item.mask = TVIF_TEXT;
	/* ├─*/tvis.item.pszText = _T("NPCLIST表示");
	/* │  */hNPCLIST = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │  */
	/* ├─*/tvis.item.pszText = _T("管理方式");
	/* │  */hControl = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    │  */tvis.hParent = hControl;
	/* │    ├─*/tvis.item.pszText = _T("ID");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    └─*/tvis.item.pszText = _T("名前");
	/* │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │        */
	/* │  */tvis.hParent = TVI_ROOT;
	/* ├─*/tvis.item.pszText = _T("倍率");
	/* │  */hZoom = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    │  */tvis.hParent = hZoom;
	/* │    ├─*/tvis.item.pszText = _T("選択");
	/* │    │  */hSelect = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x1.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x2.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x3.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x4.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x5.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x6.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x7.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x8.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x9.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x10.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x12.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x15.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("x18.0");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("x20.0");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hZoom;
	/* │    ├─*/tvis.item.pszText = _T("+0.1");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("+1.0");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("-0.1");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("-1.0");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    └─*/tvis.item.pszText = _T("デフォルトズーム機能");
	/* │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │        */
	/* │  */tvis.hParent = TVI_ROOT;
	/* ├─*/tvis.item.pszText = _T("表示");
	/* │  */hShow = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("ターゲット");
	/* │    │  */hTarget = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    │  */tvis.hParent = hTarget;
	/* │    │    ├─*/tvis.item.pszText = _T("名前");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("HP%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("ID");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("NPC");
	/* │    │  */hNPC = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    │  */tvis.hParent = hNPC;
	/* │    │    ├─*/tvis.item.pszText = _T("Normal");
	/* │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("名前");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("HP%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("ID");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("タイマー");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("可視範囲のみ");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hNPC;
	/* │    ├─*/tvis.item.pszText = _T("Ex");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("名前");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("HP%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("ID");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("タイマー");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("可視範囲のみ");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hNPC;
	/* │    ├─*/tvis.item.pszText = _T("Sp");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("名前");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("HP%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("ID");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("タイマー");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("可視範囲のみ");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("自分");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("方向");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("座標");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("<pos>(不正確)");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("軌跡　線");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("軌跡　点");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("プレイヤー");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("位置");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("名前");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("パーティ/アラ");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("位置");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("名前");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("ペット");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("位置");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("名前");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("円");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("可視範囲");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("アビリティ");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("魔法");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("遠隔攻撃");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("管楽器強化歌");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("感知範囲");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("天候");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("アイコン");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("音");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hShow;
	/* │    ├─*/tvis.item.pszText = _T("フォント");
	/* │    │  */hFont = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    │  */tvis.hParent = hFont;
	/* │    │    ├─*/tvis.item.pszText = _T("ターゲット");
	/* │    │    │  */hTarget_Font = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    │  */tvis.hParent = hTarget_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("大きさ");
	/* │    │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("Normal/Ex/PCの値を使用");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("8");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("9");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("10");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("11");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("12");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("16");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("18");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    └─*/tvis.item.pszText = _T("24");
	/* │    │    │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │        */
	/* │    │    │    │  */tvis.hParent = hTarget_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("太字");
	/* │    │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("Normal/Ex/PCの値を使用");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    └─*/tvis.item.pszText = _T("太字");
	/* │    │    │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │        */
	/* │    │    │    │  */tvis.hParent = hTarget_Font;
	/* │    │    │    └─*/tvis.item.pszText = _T("効果");
	/* │    │    │        */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │         │  */
	/* │    │    │         ├─*/tvis.item.pszText = _T("影");
	/* │    │    │         │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │         │  */
	/* │    │    │         └─*/tvis.item.pszText = _T("縁");
	/* │    │    │             */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │             */
	/* │    │    │  */tvis.hParent = hFont;
	/* │    │    ├─*/tvis.item.pszText = _T("NPC Normal");
	/* │    │    │  */hNPC_N_Font = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    │  */tvis.hParent = hNPC_N_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("大きさ");
	/* │    │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("8");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("9");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("10");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("11");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("12");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("16");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("18");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    └─*/tvis.item.pszText = _T("24");
	/* │    │    │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │        */
	/* │    │    │    │  */tvis.hParent = hNPC_N_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("太字");
	/* │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    └─*/tvis.item.pszText = _T("効果");
	/* │    │    │        */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          ├─*/tvis.item.pszText = _T("影");
	/* │    │    │          │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          └─*/tvis.item.pszText = _T("縁");
	/* │    │    │              */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │              */
	/* │    │    │  */tvis.hParent = hFont;
	/* │    │    ├─*/tvis.item.pszText = _T("NPC Ex");
	/* │    │    │  */hNPC_E_Font = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    │  */tvis.hParent = hNPC_E_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("大きさ");
	/* │    │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("8");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("9");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("10");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("11");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("12");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("16");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("18");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    └─*/tvis.item.pszText = _T("24");
	/* │    │    │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │        */
	/* │    │    │    │  */tvis.hParent = hNPC_E_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("太字");
	/* │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    └─*/tvis.item.pszText = _T("効果");
	/* │    │    │        */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          ├─*/tvis.item.pszText = _T("影");
	/* │    │    │          │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          └─*/tvis.item.pszText = _T("縁");
	/* │    │    │              */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │              */
	/* │    │    │  */tvis.hParent = hFont;
	/* │    │    ├─*/tvis.item.pszText = _T("NPC Sp");
	/* │    │    │  */hNPC_S_Font = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    │  */tvis.hParent = hNPC_S_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("大きさ");
	/* │    │    │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("8");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("9");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("10");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("11");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("12");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("16");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    ├─*/tvis.item.pszText = _T("18");
	/* │    │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │    │  */
	/* │    │    │    │    └─*/tvis.item.pszText = _T("24");
	/* │    │    │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │        */
	/* │    │    │    │  */tvis.hParent = hNPC_S_Font;
	/* │    │    │    ├─*/tvis.item.pszText = _T("太字");
	/* │    │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │    │  */
	/* │    │    │    └─*/tvis.item.pszText = _T("効果");
	/* │    │    │        */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          ├─*/tvis.item.pszText = _T("影");
	/* │    │    │          │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │          │  */
	/* │    │    │          └─*/tvis.item.pszText = _T("縁");
	/* │    │    │              */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │              */
	/* │    │    │  */tvis.hParent = hFont;
	/* │    │    └─*/tvis.item.pszText = _T("プレイヤー");
	/* │    │        */hPlayer_Font = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │  */
	/* │    │         │  */tvis.hParent = hPlayer_Font;
	/* │    │         ├─*/tvis.item.pszText = _T("大きさ");
	/* │    │         │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("8");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("9");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("10");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("11");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("12");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("16");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    ├─*/tvis.item.pszText = _T("18");
	/* │    │         │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │    │  */
	/* │    │         │    └─*/tvis.item.pszText = _T("24");
	/* │    │         │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │        */
	/* │    │         │  */tvis.hParent = hPlayer_Font;
	/* │    │         ├─*/tvis.item.pszText = _T("太字");
	/* │    │         │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │         │  */
	/* │    │         └─*/tvis.item.pszText = _T("効果");
	/* │    │             */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │               │  */
	/* │    │               ├─*/tvis.item.pszText = _T("影");
	/* │    │               │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │               │  */
	/* │    │               └─*/tvis.item.pszText = _T("縁");
	/* │    │                   */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │                   */
	/* │    │  */tvis.hParent = hShow;
	/* │    └─*/tvis.item.pszText = _T("色");
	/* │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │        */
	/* │  */tvis.hParent = TVI_ROOT;
	/* ├─*/tvis.item.pszText = _T("音");
	/* │  */hSound = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("沸き通知 Normal");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("状態変化通知 Normal");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("沸き通知 Ex");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("状態変化通知 Ex");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("沸き通知 Sp");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    └─*/tvis.item.pszText = _T("状態変化通知 Sp");
	/* │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │        */
	/* │  */tvis.hParent = TVI_ROOT;
	/* ├─*/tvis.item.pszText = _T("他");
	/* │  */hOther = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    │  */tvis.hParent = hOther;
	/* │    ├─*/tvis.item.pszText = _T("常に最前面");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("タイトルバーを隠す");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("透過度/透過色");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("0%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("10%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("20%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("30%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("40%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("50%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("60%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("70%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("80%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("90%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("100%");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("透過色モード");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hOther;
	/* │    ├─*/tvis.item.pszText = _T("監視対象PID");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/
	/* │    │        */
	/* │    │  */tvis.hParent = hOther;
	/* │    ├─*/tvis.item.pszText = _T("レーダー表示");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("自分中心で表示");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("高度差のある物を隠す");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("地図画像反転");
	/* │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │  */
	/* │    ├─*/tvis.item.pszText = _T("プログラム優先度");
	/* │    │  */tvis.hParent = TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("通常");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    ├─*/tvis.item.pszText = _T("通常以下");
	/* │    │    │  */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │    │  */
	/* │    │    └─*/tvis.item.pszText = _T("アイドル");
	/* │    │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │    │        */
	/* │    │  */tvis.hParent = hOther;
	/* │    └─*/tvis.item.pszText = _T("設定の保存");
	/* │        */TreeView_InsertItem(hTreeWnd, &tvis);
	/* │        */
	/* │  */tvis.hParent = TVI_ROOT;
	/* └─*/tvis.item.pszText = _T("バージョン情報");
	/*     */TreeView_InsertItem(hTreeWnd, &tvis);

	return 1;
}

//SetColor用のフック関数
UINT CALLBACK CCHookProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowText(hWnd, (_TCHAR*)((CHOOSECOLOR*)lp)->lCustData);
		return TRUE;
	}
	return FALSE;
}

int SetColor(HWND hWnd, DWORD dwId, COLORREF *Color)
{
	CHOOSECOLOR cc;
	COLORREF colorPallet[16];
	colorPallet[0] = g_ColorNPC_Pop[0];
	colorPallet[1] = g_ColorNPC_Share[0];
	colorPallet[2] = g_ColorNPC_War[0];
	colorPallet[3] = g_ColorNPC_Corpse[0];
	colorPallet[4] = g_ColorNPC_Out[0];
	colorPallet[5] = g_ColorNPC_Pop[1];
	colorPallet[6] = g_ColorNPC_Share[1];
	colorPallet[7] = g_ColorNPC_War[1];
	colorPallet[8] = g_ColorNPC_Corpse[1];
	colorPallet[9] = g_ColorNPC_Out[1];
	colorPallet[10] = g_ColorNPC_Pop[2];
	colorPallet[11] = g_ColorOWN;
	colorPallet[12] = g_ColorPC;
	colorPallet[13] = g_ColorPARTY;
	colorPallet[14] = g_ColorALIANCE;
	colorPallet[15] = g_ColorPET;

	_TCHAR szBuf[0x10], szText[0x20];
	GetDlgItemText(hWnd, dwId, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
	sprintf(szText, _T("%s \"%s\""), _T("Color"), szBuf);
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = colorPallet;
	cc.lpfnHook = CCHookProc;
	cc.lCustData = (LPARAM)szText;
	cc.Flags = CC_RGBINIT | CC_ENABLEHOOK;
	if (ChooseColor(&cc)) {
		*Color = cc.rgbResult;
		//再描画
		InvalidateRect(hWnd, NULL, false);
	}

	return 0;
}
