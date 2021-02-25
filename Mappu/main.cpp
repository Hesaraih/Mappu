#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//���j�R�[�h�ŃR���p�C��
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

///////////////////////////////////
#define MAPPU_VER _T("Mappu")//0.50����o�[�W�����͒�`���Ȃ�
///////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <commctrl.h>
#include <Mmsystem.h>
#include <locale.h>
#include <olectl.h>
#include <math.h>

#include "resource.h"
#include "process.h"
#include "header.h"
#include "draw.h"
#include "list.h"
#include "other.h"
#include "searchaddr.h"

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"version.lib")//0.50�Œǉ�

/*/////////ToDo/////////////



//////////////////////////*/

#define USE_FFHOOK 1//ffhook_Mappu.dll���g�p���邩�ǂ��� 0.51�Œǉ�
#define DF_BUFFSIZE   255
#define DF_CHECK      "CHECK"
#define DF_NOT_CHECK  "NOT CHECK"

LRESULT CALLBACK WndProcMAP(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcNPCLIST(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcSubClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCHARACTER(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcSETTING(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCOLOR(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWndProc(HWND, LPARAM);//0.52�Œǉ�
long GetTreeViewCheck(HTREEITEM);

HANDLE g_polHandle;//pol�̃n���h��
DWORD g_DllAddress;//FFXiMain.dll�̃A�h���X
DWORD g_DllSize;//DLL�̃T�C�Y
HWND g_hList;//IDC_LIST�̃n���h��
HWND g_hTree;//IDC_TREE�̃n���h��
static HMENU g_hMenuNPCLIST;//NPCLIST�̃T�u���j���[�̃n���h��
static WNDPROC WndProcMainClass;//�T�u�N���X����WNDPROC

								//�e��I�t�Z�b�g
								//MAP:AreaID v0.21����0�̎�NPCLIST����擾����悤�ɕύX
//DWORD g_OffsetMap;
//DWORD g_OffsetNpcMap;
//DWORD g_OffsetNpcList;
//DWORD g_OffsetNowSta;
//DWORD g_OffsetTarget;
//DWORD g_OffsetWeather;
DWORD g_DLLFileSize;
DWORD g_DLLFileSizeCmp;
struct FFXIOFFSET g_Offset;
struct NPC_MEM_MAP NPC_MEM;//0.49�Œǉ�
						   //map.ini�̃p�X
_TCHAR g_MapPath[MAX_PATH + 1];
//NPCLIST�̃p�X
_TCHAR g_NpcListPath[MAX_PATH + 1];
//�E�B���h�E�̈ʒu
int g_MapPosX;
int g_MapPosY;
int g_MapSizeX;
int g_MapSizeY;
int g_NpcListPosX;
int g_NpcListPosY;
int g_NpcListSizeX;
int g_NpcListSizeY;
float g_Zoom;//�\���{��
float g_ZoomMouseDelta;//�����̍����l
float g_Zoom_01;//0.1�{�}�b�v�̃f�t�H���g�{���i�t�B�[���h�j
float g_Zoom_02;//0.2�{�}�b�v�̃f�t�H���g�{���i�_���W�����j
float g_Zoom_04;//0.4�{�}�b�v�̃f�t�H���g�{���i�X�j
int g_Sort[sizeof(szColumn) / sizeof(szColumn[0])] = { DOWN,DOWN,DOWN,DOWN,DOWN,DOWN,DOWN };//�\�[�g���[�J������] 0.50��6�ɕύX 0.57��7�ɕύX
int g_Timer;//�^�C�}�[ms
_TCHAR g_WaveFileName[12][3][MAX_PATH];//0.47��3��ނɕύX
_TCHAR g_WeatherWaveFileName[MAX_PATH];
//�O���[�o���G���[�R�[�h
int g_ErrorCode = 0;
//�t�H���g
FONT g_Font;
FONT g_FontEx;
FONT g_FontSp;//0.47�Œǉ�
FONT g_FontSystem;
FONT g_FontTarget;
FONT g_FontPC;
//NPCLIST�̓G���G���AID
WORD g_ListAreaID = 0;
//�Ď�����v���Z�XID
DWORD g_ProcessID = 0;
//�F���i�[
COLORREF g_ColorVisible;
COLORREF g_ColorAbility;
COLORREF g_ColorMagic;
COLORREF g_ColorRange;
COLORREF g_ColorWindSong;
COLORREF g_ColorDetection;
COLORREF g_ColorMapName;
COLORREF g_ColorZoom;
COLORREF g_ColorMyPosition;
COLORREF g_Colorlt_Pos_gt;
COLORREF g_ColorDistance;
COLORREF g_ColorNPC_Pop[3];//0.47����Normal/Ex/Sp��3��ނɕύX
COLORREF g_ColorNPC_Share[3];//0.46�Œǉ�
COLORREF g_ColorNPC_War[3];
COLORREF g_ColorNPC_Corpse[3];
COLORREF g_ColorNPC_Out[3];
COLORREF g_ColorOWN;
COLORREF g_ColorPARTY;
COLORREF g_ColorALIANCE;//0.51�Œǉ�
COLORREF g_ColorPC;
COLORREF g_ColorPET;
COLORREF g_ColorTarget;
COLORREF g_ColorTransparent;
//�e�|�C���g�T�C�Y
float g_PointSizeNPC;
float g_PointSizeOWN;
float g_PointSizeTRACE;//0.55�Œǉ�
float g_PointSizePARTY;
float g_PointSizePC;
float g_PointSizePET;
float g_PointSizeTARGET;
int g_TargetHighlight;
//map.ini���˂��܌݊��ŗ��p����(62_c=��62_12.gif�ŊJ��)
int g_HexToDec;
//MAP�𒆐S�ɂ���
int g_CenterMAP_X = 0;
int g_CenterMAP_Z = 0;
//�V���[�g�J�b�g�L�[�̐ݒ�
int g_KeyZoomPlus;
int g_KeyZoomPlus_1;
int g_KeyZoomMinus;
int g_KeyZoomMinus_1;
//�I�����_�C�A���O�{�b�N�X��\��������
int g_ExitDialogBox;
//State��Corpse/-��Pop�ƂȂ������ATimer�̕\�����Ԃ��N���A����
int g_ClearTime;
//�͈͊O�ɂȂ������ACorpse(War)�Ȃ�͈͊O�̐F�ɂ��Ȃ�
int g_NotChangeColorCorpse;
int g_NotChangeColorWar;
//NPCLIST�̕����w�i�F��L��
int g_NPCLIST_BkColor;
//�B�����x��
int g_height_diff;

//MENU_FLAG 0.46�Œǉ�
MENU_FLAG g_Menu;
//Link 0.59�Œǉ�
LINK g_Link;
//ffhook���g��0.51�Œǉ�
int(__stdcall *FF11_Inject)(DWORD);
int(__stdcall *FF11_Eject)(int);
int(__stdcall *FFHOOK_Mappu_Command)(HWND, HWND, int, DWORD, int);
int(__stdcall *FFHOOK_Mappu_Init)(void);

_TCHAR g_szSelectItem[DF_BUFFSIZE + 1]; /* �I�����ꂽ�A�C�e��         */
_TCHAR g_szGetCheck[DF_BUFFSIZE + 1];   /* �`�F�b�N��ԕ\��           */


//WinMain
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAP), NULL, (DLGPROC)WndProcMAP);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return true;
}


//�E�B���h�E�v���V�[�W��IDD_DIALOG_MAP
LRESULT CALLBACK WndProcMAP(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HWND hNPCLIST, hCHARACTER, hSETTING;
	static HMENU hMenu, hSubMenu;
	static NOTIFYICONDATA nid;
	static int nRet;
	static int nStartX = 0, nStartZ = 0;
	static const char chFontSize[] = { 8,9,10,11,12,16,18,24 };
	static DWORD dwRButtonTimer;

	switch (msg) {
	case WM_INITDIALOG:
		//���P�[����OS�f�t�H���g�ɐݒ�
		//VC�̏ꍇ���ꂪ������IO�ŕ�����������
		_tsetlocale(LC_ALL, _T(""));
		SetWindowText(hWnd, MAPPU_VER);
		//MENU_FLAG������ 0.46�Œǉ�
		ZeroMemory(&g_Menu, sizeof(MENU_FLAG));
		//�^�X�N�g���C�A�C�R���쐬
		HICON hSmallIcon;
		hSmallIcon = (HICON)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
		memset(&nid, 0, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = ID_TASKTRAY;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.hIcon = hSmallIcon;
		nid.uCallbackMessage = WM_TASKTRAY;
		wcscpy(nid.szTip, MAPPU_VER);
		if (0 == Shell_NotifyIcon(NIM_ADD, &nid)) {
			g_ErrorCode = 1;
			ErrorNum(hWnd);
			return 0;//0.51�ŃG���[���return 0��ǉ�
		}
		//�_�C�A���O�ɏ������A�C�R����\��(�G���[�̏o�鏈��������ɏ������邱��)
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmallIcon);
		//���j���[�쐬(�I�t�Z�b�g�ǂݍ��݂���ɏ������邱��)
		_TCHAR szBuf[MAX_PATH], szFullPathName[MAX_PATH];
		if (0 == GetFullPathName(STRING_INI, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
			g_ErrorCode = 10;
			ErrorNum(hWnd);
			return 0;
		}
		GetPrivateProfileString(_T("MENU"), _T("MENU_LANG"), _T("EN"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		if (_tcscmp(szBuf, _T("JP")) == 0) {
			hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_MAP_JP));
		}
		else {
			hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_MAP));
		}
		hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu) {
			g_ErrorCode = 5;
			ErrorNum(hWnd);
			return 0;
		}
		//�_�~�[�ō�������j���[���ڂ��폜
		DeleteMenu(hMenu, IDM_SELECT_PID, FALSE);
		//FFXi�������オ���Ă��邩�m�F(�I�t�Z�b�g�ǂݍ��݂���ɏ������邱��)
		if (FindWindow(STRING_CLASS, NULL) == NULL) {
			g_ErrorCode = 4;
			ErrorNum(hWnd);
			return 0;
		}
		//FFXiMain.dll�̃A�h���X���擾(�I�t�Z�b�g�ǂݍ��݂���ɏ������邱��)
		g_DllAddress = GetDllModuleAddress(1);
		if (g_DllAddress == 0) {
			//�G���[�R�[�h��1000�ԑ�
			ErrorNum(hWnd);
			return 0;
		}
		//CHARACTER�_�C�A���O�쐬(�I�t�Z�b�g�ǂݍ��݂���ɏ������邱��)
		hCHARACTER = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_CHARACTER), hWnd, (DLGPROC)WndProcCHARACTER);
		if (!hCHARACTER) {
			g_ErrorCode = 6;
			ErrorNum(hWnd);
			return 0;
		}
		//SETTING�_�C�A���O�쐬(�I�t�Z�b�g�ǂݍ��݌�ɏ�������)
		hSETTING = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_SETTING), hWnd, (DLGPROC)WndProcSETTING);
		if (!hSETTING) {
			g_ErrorCode = 6;
			ErrorNum(hWnd);
		}
		else {
			//�����\��
			ShowWindow(hSETTING, SW_HIDE);
		}

		//ini�t�@�C������I�t�Z�b�g�ǂݍ���
		if (ReadIniFiles(hWnd, STRING_INI) != 1) {
			g_ErrorCode = 2;
			ErrorNum(hWnd);
			return 0;
		}
		//NPCLIST�_�C�A���O�쐬(�I�t�Z�b�g�ǂݍ��݌�ɏ�������)
		hNPCLIST = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_NPCLIST), hWnd, (DLGPROC)WndProcNPCLIST);
		if (!hNPCLIST) {
			g_ErrorCode = 7;
			ErrorNum(hWnd);
			return 0;
		}
		else {
			//�����\��
			if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_NPCLIST)) {
				ShowWindow(hNPCLIST, SW_SHOW);
			}
			else {
				ShowWindow(hNPCLIST, SW_HIDE);
			}
		}

		//�I�t�Z�b�g�`�F�b�N(dll�A�h���X�擾��)
		if (CheckOffset(hWnd, STRING_INI) != 1) {
			//g_ErrorCode = 3;
			//ErrorNum(hWnd);
		}
		//�E�B���h�E�ʒu���ړ�
		if (MoveWindow(hWnd, g_MapPosX, g_MapPosY, g_MapSizeX, g_MapSizeY, true) == 0) {
			g_ErrorCode = 8;
			ErrorNum(hWnd);
			return 0;
		}
		//Menu FontSize �����l
		for (int i = 0; i < sizeof(chFontSize); i++) {
			if (g_Font.nSize == chFontSize[i]) {
				if ((FontSizeSet(hSubMenu, IDM_FONT8, chFontSize, i, sizeof(chFontSize))) == 0) {
					ErrorNum(hWnd);
				}
			}
			if (g_FontEx.nSize == chFontSize[i]) {
				if ((FontSizeSet(hSubMenu, IDM_FONT_EX8, chFontSize, i, sizeof(chFontSize))) == 0) {
					ErrorNum(hWnd);
				}
			}
			if (g_FontSp.nSize == chFontSize[i]) {
				if ((FontSizeSet(hSubMenu, IDM_FONT_SP8, chFontSize, i, sizeof(chFontSize))) == 0) {
					ErrorNum(hWnd);
				}
			}
			if (g_FontPC.nSize == chFontSize[i]) {
				if ((FontSizeSet(hSubMenu, IDM_FONT_PC8, chFontSize, i, sizeof(chFontSize))) == 0) {
					ErrorNum(hWnd);
				}
			}
			if (g_FontTarget.nSize == chFontSize[i]) {
				if ((FontSizeSet(hSubMenu, IDM_FONT_TARGET8, chFontSize, i, sizeof(chFontSize))) == 0) {
					ErrorNum(hWnd);
				}
			}
		}
		if (g_FontTarget.nSize == 0)SendMessage(hWnd, WM_COMMAND, IDM_FONT_TARGET_USEDEFAULT, 0);
		//IDM_POPSOUND��IDM_STATESOUND�͂ǂ��炩�����`�F�b�N���Ȃ�
		if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND)) {
			//�����`�F�b�N��POPSOUND�𖢃`�F�b�N
			g_Menu.popsound[0] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND);//0.49�ŕύX
		}
		if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_EX)) {//0.49�Œǉ�
																											   //�����`�F�b�N��POPSOUND�𖢃`�F�b�N
			g_Menu.popsound[1] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX);
		}
		if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_SP)) {//0.49�Œǉ�
																											   //�����`�F�b�N��POPSOUND�𖢃`�F�b�N
			g_Menu.popsound[2] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP);
		}
		//�E�B���h�E�ɓ��ߐݒ��������
		if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, g_Menu.alpha_map)) {
			ErrorNum(hWnd);
			return 0;
		}

		if (g_Offset.Target == 0) {
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				//MessageBox(hWnd, _T("�I�t�Z�b�g�擾�Ɏ��s"), NULL, MB_ICONERROR);
			}
		}
		//g_OffsetTarget��0�̎����j���[���ꕔ���p�s�ɂ���
		if (g_Offset.Target == 0) {
			MENUITEMINFO menuinfo;
			menuinfo.cbSize = sizeof(MENUITEMINFO);
			menuinfo.fMask = MIIM_STATE;
			menuinfo.fState = MFS_DISABLED | MFS_GRAYED | MFS_UNCHECKED;
			SetMenuItemInfo(hSubMenu, IDM_SHOW_TARGET_NAME, FALSE, &menuinfo);
			SetMenuItemInfo(hSubMenu, IDM_SHOW_TARGET_HPP, FALSE, &menuinfo);
			SetMenuItemInfo(hSubMenu, IDM_SHOW_TARGET_ID, FALSE, &menuinfo);
		}
		if (g_Offset.Weather == 0) {
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				//MessageBox(hWnd, _T("�I�t�Z�b�g�擾�Ɏ��s"), NULL, MB_ICONERROR);
			}
		}
		//g_OffsetWeather��0�̎����j���[���ꕔ���p�s�ɂ���
		if (g_Offset.Weather == 0) {
			MENUITEMINFO menuinfo;
			menuinfo.cbSize = sizeof(MENUITEMINFO);
			menuinfo.fMask = MIIM_STATE;
			menuinfo.fState = MFS_DISABLED | MFS_GRAYED | MFS_UNCHECKED;
			SetMenuItemInfo(hSubMenu, IDM_SHOW_WEATHER_ICON, FALSE, &menuinfo);
			SetMenuItemInfo(hSubMenu, IDM_SHOW_WEATHER_SOUND, FALSE, &menuinfo);
		}
		//���j���[�̖��O��ύX
		_TCHAR szMenuName[0x20];
		MENUITEMINFO MenuInfo;
		ZeroMemory(&MenuInfo, sizeof(MENUITEMINFO));
		MenuInfo.cbSize = sizeof(MENUITEMINFO);
		MenuInfo.fMask = MIIM_TYPE;
		MenuInfo.fType = MFT_STRING;
		MenuInfo.dwTypeData = szMenuName;
		//IDM_ZOOM_PLUS
		if (g_KeyZoomPlus & 0xFF00) {
			_stprintf_s(szMenuName, _T("+0.1 [%s+%s]"), szVK[(g_KeyZoomPlus >> 8) & 0xFF], szVK[g_KeyZoomPlus & 0xFF]);
		}
		else {
			int numKeyZoom = g_KeyZoomPlus & 0xFF;
			if (numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("+0.1 [%s]"), szVK[numKeyZoom]);
			}
		}
		szMenuName[sizeof(szMenuName - 1)] = 0;
		MenuInfo.cch = _tcslen(szMenuName);
		SetMenuItemInfo(hSubMenu, IDM_ZOOM_PLUS, FALSE, &MenuInfo);
		//IDM_ZOOM_PLUS_1
		if (g_KeyZoomPlus_1 & 0xFF00) {
			_stprintf_s(szMenuName, _T("+1.0 [%s+%s]"), szVK[(g_KeyZoomPlus_1 >> 8) & 0xFF], szVK[g_KeyZoomPlus_1 & 0xFF]);
		}
		else {
			int numKeyZoom = g_KeyZoomPlus_1 & 0xFF;
			if (numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("+1.0 [%s]"), szVK[numKeyZoom]);
			}
		}
		MenuInfo.cch = _tcslen(szMenuName);
		SetMenuItemInfo(hSubMenu, IDM_ZOOM_PLUS_1, FALSE, &MenuInfo);
		//IDM_ZOOM_MINUS
		if (g_KeyZoomMinus & 0xFF00) {
			int numKeyZoom_d = (g_KeyZoomMinus >> 8) & 0xFF;
			int numKeyZoom = g_KeyZoomMinus & 0xFF;
			if (numKeyZoom_d < 144 && numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("-0.1 [%s+%s]"), szVK[numKeyZoom_d], szVK[numKeyZoom]);
			}
		}
		else {
			int numKeyZoom = g_KeyZoomMinus & 0xFF;
			if (numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("-0.1 [%s]"), szVK[numKeyZoom]);
			}
		}
		MenuInfo.cch = _tcslen(szMenuName);
		SetMenuItemInfo(hSubMenu, IDM_ZOOM_MINUS, FALSE, &MenuInfo);
		//IDM_ZOOM_MINUS_1
		if (g_KeyZoomMinus_1 & 0xFF00) {
			int numKeyZoom_d = (g_KeyZoomMinus_1 >> 8) & 0xFF;
			int numKeyZoom = g_KeyZoomMinus_1 & 0xFF;
			if (numKeyZoom_d < 144 && numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("-1.0 [%s+%s]"), szVK[numKeyZoom_d], szVK[numKeyZoom]);
			}
		}
		else {
			int numKeyZoom = g_KeyZoomMinus_1 & 0xFF;
			if (numKeyZoom < 144) {
				_stprintf_s(szMenuName, _T("-1.0 [%s]"), szVK[numKeyZoom]);
			}
		}
		MenuInfo.cch = _tcslen(szMenuName);
		SetMenuItemInfo(hSubMenu, IDM_ZOOM_MINUS_1, FALSE, &MenuInfo);

#if USE_FFHOOK == 1
		//0.51�Œǉ�
		static HMODULE hModule;
		static int index;
		if ((hModule = LoadLibrary(_T("ffhook_Mappu.dll"))) != NULL) {
			if ((FF11_Inject = (int(__stdcall*)(DWORD))GetProcAddress(hModule, "FF11_Inject")) == NULL) {
				g_ErrorCode = 20;
				ErrorNum(hWnd);
				return 0;
			}
			else if ((FF11_Eject = (int(__stdcall*)(int))GetProcAddress(hModule, "FF11_Eject")) == NULL) {
				g_ErrorCode = 21;
				ErrorNum(hWnd);
				return 0;
			}
			else if ((FFHOOK_Mappu_Command = (int(__stdcall*)(HWND, HWND, int, DWORD, int))GetProcAddress(hModule, "FFHOOK_Mappu_Command")) == NULL) {
				g_ErrorCode = 22;
				ErrorNum(hWnd);
				return 0;
			}
			else if ((FFHOOK_Mappu_Init = (int(__stdcall*)())GetProcAddress(hModule, "FFHOOK_Mappu_Init")) == NULL) {
				g_ErrorCode = 23;
				ErrorNum(hWnd);
				return 0;
			}
			FFHOOK_Mappu_Init();
			index = FF11_Inject(g_ProcessID);
			FFHOOK_Mappu_Command(hWnd, hNPCLIST, index, g_ProcessID, 1);//1�ŊJ�n0�Œ�~
		}
#endif
		//�^�C�}�[�J�n 0.51�ňړ�
		if (SetTimer(hWnd, ID_TIMER, g_Timer, NULL) == 0) {
			g_ErrorCode = 9;
			ErrorNum(hWnd);
			return 0;
		}
		break;

	case WM_TIMER:
#ifdef _DEBUG
		DWORD dwTime;
		dwTime = timeGetTime();
#endif
		if (FindWindow(STRING_CLASS, NULL) == NULL || CheckProcessID(g_ProcessID) != 0) {
			KillTimer(hWnd, ID_TIMER);
			if (FindWindow(STRING_CLASS, NULL) != NULL) {
				g_DllAddress = GetDllModuleAddress(1);
				if (g_DllAddress == 0) {//PID�؂�ւ�
																   //�G���[�R�[�h��1000�ԑ�
					ErrorNum(hWnd);
					break;
				}
				else {
					SetTimer(hWnd, ID_TIMER, g_Timer, NULL);
				}
			}
			else {
				g_ErrorCode = 4;
				ErrorNum(hWnd);
				break;
			}
		}
		else {
			KillTimer(hWnd, ID_TIMER);
			if (1 == GetMenuCheck(hWnd, g_hMenuNPCLIST, IDM_LIST_ALWAYS_UPDATE)) {
				AlwaysUpdateList(hNPCLIST);//�G���A�`�F���W���������X�g���A�b�v�f�[�g
			}
			else if (1 == GetMenuCheck(hWnd, g_hMenuNPCLIST, IDM_FILE_AUTOLOAD)) {
				FileAutoLoad(hNPCLIST);//�G���A�`�F���W���������X�g���A�b�v�f�[�g
			}
			//NPCLIST��State�ATimer�̍X�V
			nRet = List_AddState(g_hList);
			if (nRet != 0) {
				g_ErrorCode = 1200 + nRet;//List_AddState�֐����̃G���[�R�[�h��1200�ԑ�
				ErrorNum(hWnd);
			}
			//���C�_�[���[�h�̎��Ƃ���ȊO��Draw�̑�4�����ύX
			nRet = Draw(hWnd, g_hList, hCHARACTER);
			if (nRet != 0) {
				g_ErrorCode = 1100 + nRet;//DrawPos�֐����̃G���[�R�[�h��1100�ԑ�
				ErrorNum(hWnd);
			}
			SetTimer(hWnd, ID_TIMER, g_Timer, NULL);
		}
#ifdef _DEBUG
		_TCHAR szProcessTime[0x20];
		_stprintf_s(szProcessTime, _T("DEBUG - Process Time = %ldms"), timeGetTime() - dwTime);
		SetWindowText(hWnd, szProcessTime);
#endif
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
			//IDM_*�@���j���[�֘A�̃��b�Z�[�W
		case IDM_NPCLIST:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_NPCLIST)) {
				ShowWindow(hNPCLIST, SW_SHOW);
			}
			else {
				ShowWindow(hNPCLIST, SW_HIDE);
			}
			break;

		case IDM_CONTROL_ID:
			//�Ǘ�����:ID
			if (0 == GetMenuCheck(hWnd, hSubMenu, IDM_CONTROL_ID)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_CONTROL_ID);
			}
			if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_CONTROL_NAME)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_CONTROL_NAME);
			}

			g_Menu.list_control = 0;
			break;

		case IDM_CONTROL_NAME:
			//�Ǘ�����:���O
			if (0 == GetMenuCheck(hWnd, hSubMenu, IDM_CONTROL_NAME)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_CONTROL_NAME);
			}
			if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_CONTROL_ID)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_CONTROL_ID);
			}
			g_Menu.list_control = 1;
			break;

		case IDM_ZOOM_SELECT_1:
			g_Zoom = 1.0;
			float *fZoom;
			fZoom = (float*)lp;
			if (HIWORD(wp) == 1 && *fZoom >= (float)0.5) {//�{���̎w�菈�� 0.51�Œǉ�
				g_Zoom = *fZoom;
			}
			break;

		case IDM_ZOOM_SELECT_2:
			g_Zoom = 2.0;
			break;

		case IDM_ZOOM_SELECT_3:
			g_Zoom = 3.0;
			break;

		case IDM_ZOOM_SELECT_4:
			g_Zoom = 4.0;
			break;

		case IDM_ZOOM_SELECT_5:
			g_Zoom = 5.0;
			break;

		case IDM_ZOOM_SELECT_6:
			g_Zoom = 6.0;
			break;

		case IDM_ZOOM_SELECT_7:
			g_Zoom = 7.0;
			break;

		case IDM_ZOOM_SELECT_8:
			g_Zoom = 8.0;
			break;

		case IDM_ZOOM_SELECT_9:
			g_Zoom = 9.0;
			break;

		case IDM_ZOOM_SELECT_10:
			g_Zoom = 10.0;
			break;

		case IDM_ZOOM_SELECT_12:
			g_Zoom = 12.0;
			break;

		case IDM_ZOOM_SELECT_15:
			g_Zoom = 15.0;
			break;

		case IDM_ZOOM_SELECT_18:
			g_Zoom = 18.0;
			break;

		case IDM_ZOOM_SELECT_20:
			g_Zoom = 20.0;
			break;

		case IDM_ZOOM_PLUS:
			g_Zoom = (float)(g_Zoom + 0.1);
			break;

		case IDM_ZOOM_PLUS_1:
			g_Zoom = (float)(g_Zoom + 1.0);
			break;

		case IDM_ZOOM_MINUS:
			g_Zoom = (float)(g_Zoom - 0.1);
			if (g_Zoom < 0.5) {
				g_Zoom = (float)0.5;
			}
			break;

		case IDM_ZOOM_MINUS_1:
			g_Zoom = (float)(g_Zoom - 1.0);
			if (g_Zoom < 0.5) {
				g_Zoom = (float)0.5;
			}
			break;

		case IDM_ZOOM_DEFAULT:
			g_Menu.default_zoomset = ChangeMenuCheck(hWnd, hSubMenu, IDM_ZOOM_DEFAULT);
			break;

		case IDM_SHOW_TARGET_NAME:
			g_Menu.target_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_TARGET_NAME);
			break;

		case IDM_SHOW_TARGET_HPP:
			g_Menu.target_hpp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_TARGET_HPP);
			break;

		case IDM_SHOW_TARGET_ID:
			g_Menu.target_id = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_TARGET_ID);
			break;

		case IDM_FONT_TARGET_USEDEFAULT:
			if (0 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_TARGET_USEDEFAULT)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_TARGET_USEDEFAULT);
			}
			g_FontTarget.nSize = 0;
			if (FontSizeSet(hSubMenu, IDM_FONT_TARGET8, chFontSize, -1, sizeof(chFontSize)) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 350;
				ErrorNum(hWnd);
			}
			break;

		case IDM_SHOW_NPC_NAME:
			g_Menu.npc_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_NAME);
			break;

		case IDM_SHOW_NPC_HPP:
			g_Menu.npc_hpp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_HPP);
			break;

		case IDM_SHOW_NPC_ID:
			g_Menu.npc_id = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ID);
			break;

		case IDM_SHOW_NPC_TIMER:
			g_Menu.npc_timer = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_TIMER);
			break;

		case IDM_SHOW_NPC_ONLY_VISIBLE:
			g_Menu.npc_onlyvisible = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ONLY_VISIBLE);
			break;

		case IDM_SHOW_NPC_NAME_EX:
			g_Menu.npc_name_ex = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_NAME_EX);
			break;

		case IDM_SHOW_NPC_HPP_EX:
			g_Menu.npc_hpp_ex = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_HPP_EX);
			break;

		case IDM_SHOW_NPC_ID_EX:
			g_Menu.npc_id_ex = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ID_EX);
			break;

		case IDM_SHOW_NPC_TIMER_EX:
			g_Menu.npc_timer_ex = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_TIMER_EX);
			break;

		case IDM_SHOW_NPC_ONLY_VISIBLE_EX:
			g_Menu.npc_onlyvisible_ex = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ONLY_VISIBLE_EX);
			break;

		case IDM_SHOW_NPC_NAME_SP:
			g_Menu.npc_name_sp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_NAME_SP);
			break;

		case IDM_SHOW_NPC_HPP_SP:
			g_Menu.npc_hpp_sp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_HPP_SP);
			break;

		case IDM_SHOW_NPC_ID_SP:
			g_Menu.npc_id_sp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ID_SP);
			break;

		case IDM_SHOW_NPC_TIMER_SP:
			g_Menu.npc_timer_sp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_TIMER_SP);
			break;

		case IDM_SHOW_NPC_ONLY_VISIBLE_SP:
			g_Menu.npc_onlyvisible_sp = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_NPC_ONLY_VISIBLE_SP);
			break;

		case IDM_SHOW_OWN_DIRECTION:
			g_Menu.own_direction = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_OWN_DIRECTION);
			break;

		case IDM_SHOW_OWN_POSITION:
			g_Menu.own_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_OWN_POSITION);
			break;

		case IDM_SHOW_OWN_LT_POS_GT:
			g_Menu.own_lt_pos_gt = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_OWN_LT_POS_GT);
			break;

		case IDM_SHOW_OWN_TRACE_LINE:
			g_Menu.own_trace_line = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_OWN_TRACE_LINE);
			break;

		case IDM_SHOW_OWN_TRACE_POINT:
			g_Menu.own_trace_point = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_OWN_TRACE_POINT);
			break;

		case IDM_SHOW_PARTY_POSITION:
			g_Menu.party_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_POSITION);
			//�`�F�b�N���O�ꂽ��IDM_SHOW_PARTY_NAME�𖢃`�F�b�N�ɂ���
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_POSITION) == 0 && (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_NAME))) {
				g_Menu.party_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_NAME);
			}
			break;

		case IDM_SHOW_PARTY_NAME:
			//�`�F�b�N���ꂽ����IDM_SHOW_PARTY_POSITION�����`�F�b�N�Ȃ�΃`�F�b�N����
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_POSITION) == 0) {
				g_Menu.party_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_POSITION);
			}
			g_Menu.party_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PARTY_NAME);
			break;

		case IDM_SHOW_PC_POSITION:
			g_Menu.pc_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_POSITION);
			//�`�F�b�N���O�ꂽ��IDM_SHOW_PC_NAME�𖢃`�F�b�N�ɂ���
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_POSITION) == 0 && (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_NAME))) {
				g_Menu.pc_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_NAME);
			}
			break;

		case IDM_SHOW_PC_NAME:
			//�`�F�b�N���ꂽ����IDM_SHOW_PC_POSITION�����`�F�b�N�Ȃ�΃`�F�b�N����
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_POSITION) == 0) {
				g_Menu.pc_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_POSITION);
			}
			g_Menu.pc_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PC_NAME);
			break;

		case IDM_SHOW_PET_POSITION:
			g_Menu.pet_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_POSITION);
			//�`�F�b�N���O�ꂽ��IDM_SHOW_PET_NAME�𖢃`�F�b�N�ɂ���
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_POSITION) == 0 && (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_NAME))) {
				g_Menu.pet_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_NAME);
			}
			break;

		case IDM_SHOW_PET_NAME:
			//�`�F�b�N���ꂽ����IDM_SHOW_PET_POSITION�����`�F�b�N�Ȃ�΃`�F�b�N����
			if (GetMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_POSITION) == 0) {
				g_Menu.pet_position = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_POSITION);
			}
			g_Menu.pet_name = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_PET_NAME);
			break;

		case IDM_SHOW_CIRCLE_VIS:
			g_Menu.circle_visible = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_VIS);
			break;

		case IDM_SHOW_CIRCLE_ABL:
			g_Menu.circle_ability = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_ABL);
			break;

		case IDM_SHOW_CIRCLE_MGC:
			g_Menu.circle_magic = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_MGC);
			break;

		case IDM_SHOW_CIRCLE_RNG:
			g_Menu.circle_range = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_RNG);
			break;

		case IDM_SHOW_CIRCLE_WINDSONG:
			g_Menu.circle_windsong = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_WINDSONG);
			break;

		case IDM_SHOW_CIRCLE_DETECTION:
			g_Menu.circle_detection = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_CIRCLE_DETECTION);
			break;

		case IDM_SHOW_WEATHER_ICON:
			g_Menu.weather_icon = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_WEATHER_ICON);
			break;

		case IDM_SHOW_WEATHER_SOUND:
			g_Menu.weather_sound = ChangeMenuCheck(hWnd, hSubMenu, IDM_SHOW_WEATHER_SOUND);
			break;

		case IDM_FONT_TARGET8:
		case IDM_FONT_TARGET9:
		case IDM_FONT_TARGET10:
		case IDM_FONT_TARGET11:
		case IDM_FONT_TARGET12:
		case IDM_FONT_TARGET16:
		case IDM_FONT_TARGET18:
		case IDM_FONT_TARGET24:
			if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_TARGET_USEDEFAULT)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_TARGET_USEDEFAULT);
			}
			if ((g_FontTarget.nSize = FontSizeSet(hSubMenu, IDM_FONT_TARGET8, chFontSize, LOWORD(wp) - IDM_FONT_TARGET8, sizeof(chFontSize))) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 351;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD_TARGET_USEDEFAULT:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_TARGET_USEDEFAULT)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_TARGET)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_TARGET, 0);
				}
				g_FontTarget.nBold = -1;
			}
			else g_FontTarget.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 352;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD_TARGET:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_TARGET)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_TARGET_USEDEFAULT)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONTBOLD_TARGET_USEDEFAULT, 0);
				}
				g_FontTarget.nBold = 1;
			}
			else g_FontTarget.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 353;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONT_DROPSHADOW_TARGET:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_TARGET)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_TARGET)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_TARGET, 0);
				}
				g_FontTarget.nEffect = 1;
			}
			else g_FontTarget.nEffect = 0;
			break;

		case IDM_FONT_EDGE_TARGET://0.44�Œǉ�
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_TARGET)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_TARGET)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_TARGET, 0);
				}
				g_FontTarget.nEffect = 2;
			}
			else g_FontTarget.nEffect = 0;
			break;

		case IDM_FONT8:
		case IDM_FONT9:
		case IDM_FONT10:
		case IDM_FONT11:
		case IDM_FONT12:
		case IDM_FONT16:
		case IDM_FONT18:
		case IDM_FONT24:
			if ((g_Font.nSize = FontSizeSet(hSubMenu, IDM_FONT8, chFontSize, LOWORD(wp) - IDM_FONT8, sizeof(chFontSize))) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 355;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD))g_Font.nBold = 1;
			else g_Font.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 356;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONT_DROPSHADOW:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE, 0);
				}
				g_Font.nEffect = 1;
			}
			else g_Font.nEffect = 0;
			break;

		case IDM_FONT_EDGE://0.44�Œǉ�
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW, 0);
				}
				g_Font.nEffect = 2;
			}
			else g_Font.nEffect = 0;
			break;

		case IDM_FONT_EX8:
		case IDM_FONT_EX9:
		case IDM_FONT_EX10:
		case IDM_FONT_EX11:
		case IDM_FONT_EX12:
		case IDM_FONT_EX16:
		case IDM_FONT_EX18:
		case IDM_FONT_EX24:
			if ((g_FontEx.nSize = FontSizeSet(hSubMenu, IDM_FONT_EX8, chFontSize, LOWORD(wp) - IDM_FONT_EX8, sizeof(chFontSize))) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 358;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD_EX:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_EX))g_FontEx.nBold = 1;
			else g_FontEx.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 359;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONT_DROPSHADOW_EX:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_EX)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_EX)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_EX, 0);
				}
				g_FontEx.nEffect = 1;
			}
			else g_FontEx.nEffect = 0;
			break;

		case IDM_FONT_EDGE_EX://0.44�Œǉ�
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_EX)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_EX)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_EX, 0);
				}
				g_FontEx.nEffect = 2;
			}
			else g_FontEx.nEffect = 0;
			break;

		case IDM_FONT_SP8://0.47�Œǉ�
		case IDM_FONT_SP9:
		case IDM_FONT_SP10:
		case IDM_FONT_SP11:
		case IDM_FONT_SP12:
		case IDM_FONT_SP16:
		case IDM_FONT_SP18:
		case IDM_FONT_SP24:
			if ((g_FontSp.nSize = FontSizeSet(hSubMenu, IDM_FONT_SP8, chFontSize, LOWORD(wp) - IDM_FONT_SP8, sizeof(chFontSize))) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 360;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD_SP:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_SP))g_FontSp.nBold = 1;
			else g_FontSp.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 361;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONT_DROPSHADOW_SP:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_SP)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_SP)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_SP, 0);
				}
				g_FontSp.nEffect = 1;
			}
			else g_FontSp.nEffect = 0;
			break;

		case IDM_FONT_EDGE_SP:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_SP)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_SP)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_SP, 0);
				}
				g_FontSp.nEffect = 2;
			}
			else g_FontSp.nEffect = 0;
			break;

		case IDM_FONT_PC8:
		case IDM_FONT_PC9:
		case IDM_FONT_PC10:
		case IDM_FONT_PC11:
		case IDM_FONT_PC12:
		case IDM_FONT_PC16:
		case IDM_FONT_PC18:
		case IDM_FONT_PC24:
			if ((g_FontPC.nSize = FontSizeSet(hSubMenu, IDM_FONT_PC8, chFontSize, LOWORD(wp) - IDM_FONT_PC8, sizeof(chFontSize))) == 0) {
				ErrorNum(hWnd);//wp����Ώۂ̃t�H���g�T�C�Y�𒲂ׂ�
			}
			if (MakeFont(hWnd)) {
				g_ErrorCode = 365;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONTBOLD_PC:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONTBOLD_PC))g_FontPC.nBold = 1;
			else g_FontPC.nBold = 0;
			if (MakeFont(hWnd)) {
				g_ErrorCode = 366;
				ErrorNum(hWnd);
			}
			break;

		case IDM_FONT_DROPSHADOW_PC:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_PC)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_PC)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_EDGE_PC, 0);
				}
				g_FontPC.nEffect = 1;
			}
			else g_FontPC.nEffect = 0;
			break;

		case IDM_FONT_EDGE_PC://0.44�Œǉ�
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_FONT_EDGE_PC)) {
				if (1 == GetMenuCheck(hWnd, hSubMenu, IDM_FONT_DROPSHADOW_PC)) {
					SendMessage(hWnd, WM_COMMAND, IDM_FONT_DROPSHADOW_PC, 0);
				}
				g_FontPC.nEffect = 2;
			}
			else g_FontPC.nEffect = 0;
			break;

		case IDM_COLOR:
			//0.43�Œǉ�
			if (!CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_COLOR), hWnd, (DLGPROC)WndProcCOLOR)) {
				g_ErrorCode = 370;
				ErrorNum(hWnd);
			}
			break;

		case IDM_POPSOUND:
			//g_Menu.popsound = ChangeMenuCheck(hWnd,hSubMenu,IDM_POPSOUND);
			g_Menu.popsound[0] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND);//0.49�ŕύX
																			   //IDM_POPSOUND��IDM_STATESOUND�͂ǂ��炩�����`�F�b�N���Ȃ�
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND)) {
				//�����`�F�b�N��STATESOUND�𖢃`�F�b�N
				g_Menu.statesound[0] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND);
			}
			break;

		case IDM_STATESOUND:
			//g_Menu.statesound = ChangeMenuCheck(hWnd,hSubMenu,IDM_STATESOUND);
			g_Menu.statesound[0] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND);//0.49�ŕύX
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND)) {
				g_Menu.popsound[0] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND);
			}
			break;

		case IDM_POPSOUND_EX://0.49�Œǉ�
			g_Menu.popsound[1] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX);
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_EX)) {
				g_Menu.statesound[1] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_EX);
			}
			break;

		case IDM_STATESOUND_EX://0.49�Œǉ�
			g_Menu.statesound[1] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_EX);
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_EX)) {
				g_Menu.popsound[1] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_EX);
			}
			break;

		case IDM_POPSOUND_SP://0.49�Œǉ�
			g_Menu.popsound[2] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP);
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_SP)) {
				g_Menu.statesound[2] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_SP);
			}
			break;

		case IDM_STATESOUND_SP://0.49�Œǉ�
			g_Menu.statesound[2] = ChangeMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_SP);
			if (GetMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP) && GetMenuCheck(hWnd, hSubMenu, IDM_STATESOUND_SP)) {
				g_Menu.popsound[2] = ChangeMenuCheck(hWnd, hSubMenu, IDM_POPSOUND_SP);
			}
			break;

		case IDM_ALWAYS_ON_TOP:
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_ALWAYS_ON_TOP)) {
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
			}
			else {
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
			}
			break;

		case IDM_HIDE_TITLE_BAR: {
			ShowWindow(hWnd, SW_HIDE);
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_HIDE_TITLE_BAR)) {
				LONG exStyle = GetWindowLong(hWnd, GWL_STYLE);
				exStyle &= ~(WS_CAPTION | WS_THICKFRAME);
				SetWindowLong(hWnd, GWL_STYLE, exStyle);
			}
			else {
				LONG exStyle = GetWindowLong(hWnd, GWL_STYLE);
				exStyle |= (WS_CAPTION | WS_THICKFRAME);
				SetWindowLong(hWnd, GWL_STYLE, exStyle);
			}
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME | SWP_NOZORDER | SWP_FRAMECHANGED);
			ShowWindow(hWnd, SW_SHOW);
			break; }

		case IDM_ALPHA_MAP_0:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 0)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_10:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 10)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_20:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 20)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_30:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 30)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_40:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 40)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_50:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 50)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_60:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 60)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_70:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 70)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_80:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 80)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_90:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 90)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_MAP_100:
			g_Menu.transparent = 0;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 100)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_TRANSPARENT_MODE://MAP�ɓ��ߐF��ݒ�
			g_Menu.transparent = 1;
			if (-1 == AlphaValueSetMAP(hWnd, hSubMenu, 0)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_SELECT_PID1:
		case IDM_SELECT_PID2:
		case IDM_SELECT_PID3:
		case IDM_SELECT_PID4:
		case IDM_SELECT_PID5:
		case IDM_SELECT_PID6:
		case IDM_SELECT_PID7:
		case IDM_SELECT_PID8:
		case IDM_SELECT_PID9:
		case IDM_SELECT_PID10:
		case IDM_SELECT_PID11:
		case IDM_SELECT_PID12:
		case IDM_SELECT_PID13:
		case IDM_SELECT_PID14:
		case IDM_SELECT_PID15:
		case IDM_SELECT_PID16:
#if USE_FFHOOK == 1//0.51b�Œǉ�
			if (hModule) {
				FFHOOK_Mappu_Command(hWnd, hNPCLIST, index, g_ProcessID, 0);
				FF11_Eject(index);
			}
#endif
			int Num;
			Num = LOWORD(wp) - IDM_SELECT_PID;//wp����Ώ�PID�̏��Ԃ𒲂ׂ�
			if (Num <= 0) {
				g_ErrorCode = 1090;
				ErrorNum(hWnd);
			}
			KillTimer(hWnd, ID_TIMER);
			if ((g_DllAddress = GetDllModuleAddress(Num)) == 0) {
				//�G���[�R�[�h��1000�ԑ�
				ErrorNum(hWnd);
			}
#if USE_FFHOOK == 1
			else if (hModule) {//0.51�Œǉ�
				index = FF11_Inject(g_ProcessID);
				FFHOOK_Mappu_Command(hWnd, hNPCLIST, index, g_ProcessID, 1);
			}
#endif
			SetTimer(hWnd, ID_TIMER, g_Timer, NULL);
			break;

		case IDM_RADAR_MODE://���F�𓧉ߐF�ɐݒ�
		{
			MENUITEMINFO menuinfo;
			menuinfo.cbSize = sizeof(MENUITEMINFO);
			menuinfo.fMask = MIIM_STATE;
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_RADAR_MODE)) {
				g_Menu.radar = 1;
				g_Menu.center_own = 1;
				menuinfo.fState = MFS_DISABLED | MFS_GRAYED | MFS_CHECKED;
			}
			else {
				g_Menu.radar = 0;
				menuinfo.fState = MFS_CHECKED;
			}
			SetMenuItemInfo(hSubMenu, IDM_CENTER_OWN, FALSE, &menuinfo);
		}
		break;

		case IDM_CENTER_OWN:
			nStartX = 0;
			nStartZ = 0;
			g_CenterMAP_X = 0;
			g_CenterMAP_Z = 0;
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_CENTER_OWN)) {
				g_Menu.center_own = 1;
			}
			else {
				g_Menu.center_own = 0;
			}
			break;

		case IDM_HIDE_HEIGHT_DIFF://0.56�Œǉ�
			g_Menu.hide_height_diff = ChangeMenuCheck(hWnd, hSubMenu, IDM_HIDE_HEIGHT_DIFF);
			break;

		case IDM_IMAGE_REVERSE:
			g_Menu.image_reverse = ChangeMenuCheck(hWnd, hSubMenu, IDM_IMAGE_REVERSE);
			break;

		case IDM_SAVE_MENU_SETTING:
			if (IDYES == MessageBox(hWnd, _T("Save Setting?"), MAPPU_VER, MB_YESNO | MB_ICONQUESTION)) {
				if (WriteIniFiles(hWnd, hNPCLIST, hSubMenu, g_hMenuNPCLIST, STRING_INI) == 1) {
					MessageBox(hWnd, _T("Save Success"), MAPPU_VER, MB_OK);
				}
				else {
					MessageBox(hWnd, _T("Save Fails"), MAPPU_VER, MB_OK | MB_ICONERROR);
				}
			}
			break;

		case IDM_PRIORITY_NORMAL://�G���[�R�[�h1500�ԑ�
			if ((nRet = SetMenuPriority(hWnd, hSubMenu, IDM_PRIORITY_NORMAL, NORMAL_PRIORITY_CLASS)) != 0) {
				g_ErrorCode = 1500 + nRet;
				ErrorNum(hWnd);
			}
			break;

		case IDM_PRIORITY_BELOW_NORMAL:
			if ((nRet = SetMenuPriority(hWnd, hSubMenu, IDM_PRIORITY_BELOW_NORMAL, BELOW_NORMAL_PRIORITY_CLASS)) != 0) {
				g_ErrorCode = 1520 + nRet;
				ErrorNum(hWnd);
			}
			break;

		case IDM_PRIORITY_IDLE:
			if ((nRet = SetMenuPriority(hWnd, hSubMenu, IDM_PRIORITY_IDLE, IDLE_PRIORITY_CLASS)) != 0) {
				g_ErrorCode = 1540 + nRet;
				ErrorNum(hWnd);
			}
			break;

		case IDM_SETTINGS:
			if (hSETTING) {
				ShowWindow(hSETTING, SW_SHOW);
			}
			break;
		case IDM_VERSION: {
			//�o�[�W�������\�[�X������擾
			_TCHAR szFileName[MAX_PATH];
			if (GetModuleFileName(NULL, szFileName, sizeof(szFileName) / sizeof(_TCHAR) - 1) == 0) {
				g_ErrorCode = 1600;
				ErrorNum(hWnd);
			}
			DWORD dwBlockSize = GetFileVersionInfoSize(szFileName, 0);
			BYTE *pbyVersionInfo;
			pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
			if (pbyVersionInfo) {
				if (GetFileVersionInfo(szFileName, 0, dwBlockSize, (LPVOID)pbyVersionInfo) == FALSE) {
					MessageBox(hWnd, _T("Can't get VersionInfo"), MAPPU_VER, MB_ICONERROR);
				}
				else {
					struct {
						WORD wLanguage;
						WORD wCodePage;
					} *lpTranslate;
					VerQueryValue((LPVOID)pbyVersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, NULL);
					_TCHAR szDir[0x100];
					_stprintf_s(szDir, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate->wLanguage, lpTranslate->wCodePage);
					void *pvVer;
					VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvVer, NULL);
					_stprintf_s(szDir, _T("\\StringFileInfo\\%04x%04x\\LegalCopyright"), lpTranslate->wLanguage, lpTranslate->wCodePage);
					void *pvCopyright;
					VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvCopyright, NULL);

					_TCHAR szVersion[0x100];
					_stprintf_s(szVersion, _T("Version %s\n\n%s"), (_TCHAR*)pvVer, (_TCHAR*)pvCopyright);
					MessageBox(hWnd, szVersion, MAPPU_VER, MB_OK | MB_ICONINFORMATION);
				}
			}
			free(pbyVersionInfo);
			break;
		}

		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		nStartX = LOWORD(lp);
		nStartZ = HIWORD(lp);
		break;

	case WM_LBUTTONUP:
		nStartX = 0;
		nStartZ = 0;
		if (GetMenuCheck(hWnd, hSubMenu, IDM_CENTER_OWN) == 1 || g_Menu.center_own) {//0.57a�ŕύX
			g_CenterMAP_X = 0;
			g_CenterMAP_Z = 0;
		}

		break;

	case WM_MOUSEMOVE://0.52�Œǉ�
		if (wp&MK_LBUTTON) {
			g_CenterMAP_X += LOWORD(lp) - nStartX;
			g_CenterMAP_Z += HIWORD(lp) - nStartZ;
			nStartX = LOWORD(lp);
			nStartZ = HIWORD(lp);
		}
		else {
			nStartX = 0;
			nStartZ = 0;
		}
		break;

	case WM_LBUTTONDBLCLK:
		nStartX = LOWORD(lp);//0.52�Œǉ��@�_�u���N���b�N�Ńh���b�O��������Ԏ�������
		nStartZ = HIWORD(lp);//0.52�Œǉ�
		SendMessage(hWnd, WM_COMMAND, IDM_NPCLIST, 0);
		break;

	case WM_RBUTTONDOWN://0.45�Œǉ�
		dwRButtonTimer = GetTickCount64();
		break;

	case WM_RBUTTONUP:
		//�|�C���g���擾
		POINT pos;
		pos.x = LOWORD(lp);
		pos.y = HIWORD(lp);
		if (0 == ClientToScreen(hWnd, &pos)) {
			g_ErrorCode = 40;
			ErrorNum(hWnd);
		}

		//����������NPCLIST�̃��j���[��\��//0.45�Œǉ�
		dwRButtonTimer = GetTickCount64() - dwRButtonTimer;
		if (dwRButtonTimer >= 500 && dwRButtonTimer <= 10000) {
			DWORD dwPos;
			dwPos = ((pos.x) & 0xFFFF) + (((pos.y) << 16) & 0xFFFF0000);
			SendMessage(hNPCLIST, WM_RBUTTONUP, (WPARAM)hNPCLIST, dwPos);
			dwRButtonTimer = 0;
			break;
		}
		//�v���Z�X�����J�E���g
		int nCount;
		PROCESSENTRY32 pe32;
		_TCHAR szMenuStr[0x20];
		for (nCount = 0; nCount <= 0x10;) {
			if (GetProcess(&pe32, nCount + 1) != 0)break;
			else {
				nCount++;
				//���j���[�ɒǉ�
				//���j���[�𑝂₵���ꍇ�}����ɒ��ӂ��邱��
				_stprintf_s(szMenuStr, _T("PID:%X"), pe32.th32ProcessID);
				HMENU hM1 = GetSubMenu(hSubMenu, MENU_OTHER_POS);//0.50�ŕύX
				HMENU hM2 = GetSubMenu(hM1, MENU_PROCESS_ID_POS);//0.50�ŕύX
				if (FALSE == InsertMenu(hM2, nCount, MF_BYPOSITION | MF_STRING, IDM_SELECT_PID + nCount, szMenuStr)) {//0.50�ŕύX
					g_ErrorCode = 31;
					ErrorNum(hWnd);
				}
				//���݊Ď����Ă���PID�Ɠ����Ȃ�`�F�b�N����
				if (g_ProcessID == pe32.th32ProcessID) {
					CheckMenuItem(hSubMenu, IDM_SELECT_PID + nCount, MF_BYCOMMAND | MFS_CHECKED);
				}
			}
		}

		//���j���[��\��
		SetForegroundWindow(hWnd);
		if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
			g_ErrorCode = 41;
			ErrorNum(hWnd);
		}
		PostMessage(hWnd, 0, 0, 0);

		//���j���[����폜
		for (; nCount > 0; nCount--) {
			DeleteMenu(hSubMenu, IDM_SELECT_PID + nCount, FALSE);
		}
		break;

	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wp) > 0) {
			g_Zoom = (float)(g_Zoom + g_ZoomMouseDelta);
		}
		else if ((short)HIWORD(wp) < 0) {
			g_Zoom = (float)(g_Zoom - g_ZoomMouseDelta);
			if (g_Zoom < (float)0.5) {
				g_Zoom = (float)0.5;
			}
		}
		break;

	case WM_KEYUP:
		if (GetKeyState(VK_SHIFT) & 0x80 || GetKeyState(VK_CONTROL) & 0x80 || GetKeyState(VK_MENU) & 0x80) {
			if (wp == (g_KeyZoomPlus & 0xFF) && (g_KeyZoomPlus & 0xFF00) && (GetKeyState(g_KeyZoomPlus >> 8) & 0x80)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_PLUS, NULL);
			}
			if (wp == (g_KeyZoomPlus_1 & 0xFF) && (g_KeyZoomPlus_1 & 0xFF00) && (GetKeyState(g_KeyZoomPlus_1 >> 8) & 0x80)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_PLUS_1, NULL);
			}
			if (wp == (g_KeyZoomMinus & 0xFF) && (g_KeyZoomMinus & 0xFF00) && (GetKeyState(g_KeyZoomMinus >> 8) & 0x80)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_MINUS, NULL);
			}
			if (wp == (g_KeyZoomMinus_1 & 0xFF) && (g_KeyZoomMinus_1 & 0xFF00) && (GetKeyState(g_KeyZoomMinus_1 >> 8) & 0x80)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_MINUS_1, NULL);
			}
		}
		else {
			if (wp == (g_KeyZoomPlus & 0xFF) && !(g_KeyZoomPlus & 0xFF00)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_PLUS, NULL);
			}
			if (wp == (g_KeyZoomPlus_1 & 0xFF) && !(g_KeyZoomPlus_1 & 0xFF00)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_PLUS_1, NULL);
			}
			if (wp == (g_KeyZoomMinus & 0xFF) && !(g_KeyZoomMinus & 0xFF00)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_MINUS, NULL);
			}
			if (wp == (g_KeyZoomMinus_1 & 0xFF) && !(g_KeyZoomMinus_1 & 0xFF00)) {
				SendMessage(hWnd, WM_COMMAND, IDM_ZOOM_MINUS_1, NULL);
			}
		}
		break;

	case WM_TASKTRAY://�^�X�N�o�[�̃��b�Z�[�W�������Ƃ�
		if (wp == ID_TASKTRAY) {
			switch (lp) {
			case WM_LBUTTONUP:
				ShowWindow(hWnd, SW_RESTORE);
				SetForegroundWindow(hWnd);
				break;

			case WM_RBUTTONDOWN://0.45�Œǉ�
				dwRButtonTimer = GetTickCount64();
				break;

			case WM_RBUTTONUP:
				POINT pos;
				GetCursorPos(&pos);

				//����������NPCLIST�̃��j���[��\��//0.45�Œǉ�
				dwRButtonTimer = GetTickCount64() - dwRButtonTimer;
				if (dwRButtonTimer >= 500 && dwRButtonTimer <= 10000) {
					DWORD dwPos;
					dwPos = ((pos.x) & 0xFFFF) + (((pos.y) << 16) & 0xFFFF0000);
					SendMessage(hNPCLIST, WM_RBUTTONUP, (WPARAM)hNPCLIST, dwPos);
					dwRButtonTimer = 0;
					break;
				}

				int nCount;
				PROCESSENTRY32 pe32;
				_TCHAR szMenuStr[0x20];
				for (nCount = 0; nCount <= 0x10;) {
					if (GetProcess(&pe32, nCount + 1) != 0)break;
					else {
						nCount++;
						//���j���[�ɒǉ�
						//���j���[�𑝂₵���ꍇ�}����ɒ��ӂ��邱��
						_stprintf_s(szMenuStr, _T("PID:%X"), pe32.th32ProcessID);
						if (FALSE == InsertMenu(GetSubMenu(hSubMenu, MENU_PROCESS_ID_POS), nCount, MF_BYPOSITION | MF_STRING, IDM_SELECT_PID + nCount, szMenuStr)) {
							g_ErrorCode = 30;
							ErrorNum(hWnd);
						}
						//���݊Ď����Ă���PID�Ɠ����Ȃ�`�F�b�N����
						if (g_ProcessID == pe32.th32ProcessID) {
							CheckMenuItem(hSubMenu, IDM_SELECT_PID + nCount, MF_BYCOMMAND | MFS_CHECKED);
						}
					}
				}

				//���j���[��\��
				SetForegroundWindow(hWnd);
				if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
					g_ErrorCode = 31;
					ErrorNum(hWnd);
				}
				PostMessage(hWnd, 0, 0, 0);
				//���j���[����폜
				for (; nCount > 0; nCount--) {
					DeleteMenu(hSubMenu, IDM_SELECT_PID + nCount, FALSE);
				}
				break;
			}
		}
		break;

	case WM_SIZE:
		//hCHARACTER�̃T�C�Y���ύX
		if (0 == SetWindowPos(hCHARACTER, 0, 0, 0, (short)LOWORD(lp), (short)HIWORD(lp), SWP_NOOWNERZORDER | SWP_NOMOVE)) {
			g_ErrorCode = 55;
			ErrorNum(hWnd);
		}
		break;

	case WM_MOVE:
		//hCHARACTER���ړ�
		if (0 == SetWindowPos(hCHARACTER, 0, (short)LOWORD(lp), (short)HIWORD(lp), 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE)) {
			g_ErrorCode = 56;
			ErrorNum(hWnd);
		}
		break;

	case WM_CLOSE:
#ifndef DEBUG
		if (wp == 1 && lp == 1) {//0.51�Œǉ�
								 //���b�Z�[�W�{�b�N�X��\�����Ȃ�
		}
		else if (g_ExitDialogBox && IDNO == MessageBox(hWnd, _T("�I�����܂����H"), MAPPU_VER, MB_YESNO | MB_ICONQUESTION)) {
			break;
		}
#endif
		KillTimer(hWnd, ID_TIMER);
		ShowWindow(hWnd, SW_HIDE);
#if USE_FFHOOK == 1
		if (hModule) {//0.51a�Œǉ�
			FFHOOK_Mappu_Command(hWnd, hNPCLIST, index, g_ProcessID, 0);//0.51b�Œǉ�
			FF11_Eject(index);
			FreeLibrary(hModule);//0.51a�Œǉ�
		}
#endif
		SendMessage(hNPCLIST, WM_DESTROY, 0, 0);
		PostMessage(hWnd, WM_DESTROY, 0, 0);//0.50��PostMessage�ɕύX
		break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);//0.51�ňړ�
		DeleteObject(g_Font.hFont);
		DeleteObject(g_FontEx.hFont);
		DeleteObject(g_FontSp.hFont);
		DeleteObject(g_FontPC.hFont);
		DeleteObject(g_FontTarget.hFont);
		DeleteObject(g_FontSystem.hFont);
		DestroyMenu(hMenu);
		DestroyWindow(hWnd);
		break;

	default:
		break;
	}

	return 0;
}


//�E�B���h�E�v���V�[�W��IDD_DIALOG_NPCLIST
LRESULT CALLBACK WndProcNPCLIST(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HMENU hMenu, hSubMenu;
	int nItem;
	static HWND hEdit;
	static int nSearchFlag = 0;//0.48�Œǉ�
	static int nSearchSelected = -1;//0.48�Œǉ�
	static int nAutoSort = 0;//0.57�Œǉ�
	_TCHAR szName[0x20];
	_TCHAR szParam[MAX_PATH + 0x20];

	switch (msg) {
	case WM_INITDIALOG:
		//���X�g���擾
		g_hList = GetDlgItem(hWnd, IDC_LIST);
		if (!g_hList) {
			g_ErrorCode = 102;
			ErrorNum(hWnd);
		}
		//���X�g��������
		if (0 == InitList(hWnd)) {
			g_ErrorCode = 103;
			ErrorNum(hWnd);
		}
		//���j���[�쐬
		_TCHAR szBuf[MAX_PATH], szFullPathName[MAX_PATH];
		if (0 == GetFullPathName(STRING_INI, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
			g_ErrorCode = 100;
			ErrorNum(hWnd);
		}
		GetPrivateProfileString(_T("MENU"), _T("MENU_LANG"), _T("EN"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		if (_tcscmp(szBuf, _T("JP")) == 0) {
			hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_NPCLIST_JP));
		}
		else {
			hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_NPCLIST));
		}
		hSubMenu = GetSubMenu(hMenu, 0);
		g_hMenuNPCLIST = hSubMenu;
		if (!hSubMenu) {
			g_ErrorCode = 104;
			ErrorNum(hWnd);
		}
		//���X�g���T�u�N���X��
		WndProcMainClass = (WNDPROC)SetWindowLong(g_hList, GWL_WNDPROC, (LONG)WndProcSubClass);
		//NPCLIST�̃��j���[���ڂ��`�F�b�N���� 0.50��NPCLIST�̕��ֈړ�
		if (1 == g_Menu.file_autoload) {//IDM_FILE_AUTOLOAD��IDM_LIST_ALWAYS_UPDATE�̌�Ń`�F�b�N���������D��
			SendMessage(hWnd, WM_COMMAND, IDM_FILE_AUTOLOAD, 0);
		}
		if (1 == g_Menu.always_update) {
			SendMessage(hWnd, WM_COMMAND, IDM_LIST_ALWAYS_UPDATE, 0);
		}
		//0.50�Œǉ�
		if (1 == g_Menu.hide_toolbar) {
			SendMessage(hWnd, WM_COMMAND, IDM_HIDE_TOOL_BAR, 0);
		}
		if (1 == g_Menu.hide_buttonbox) {
			SendMessage(hWnd, WM_COMMAND, IDM_HIDE_BUTTONBOX, 0);
		}
		//�E�B���h�E�ɓ��ߐݒ��������
		if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, g_Menu.alpha_npclist)) {
			ErrorNum(hWnd);
		}
		//�����l����
		SetDlgItemText(hWnd, IDC_EDIT_SEARCH, _T("Search"));
		//�E�B���h�E�ړ� 0.50�ŏ������̍Ō�Ɉړ�
		if (MoveWindow(hWnd, g_NpcListPosX, g_NpcListPosY, g_NpcListSizeX, g_NpcListSizeY, true) == 0) {
			g_ErrorCode = 101;
			ErrorNum(hWnd);
		}
		//0.59�Œǉ�
		HMENU hM1, hM2;
		hM1 = GetSubMenu(hSubMenu, 8);//�ォ��8�Ԗ�
		hM2 = GetSubMenu(hM1, 3);//�ォ��4�Ԗ�
		for (int i = 0; i<4; i++) {
			DeleteMenu(hSubMenu, IDM_LINK1 + i, FALSE);
			if (wcslen(g_Link.link[i]) == 0 || wcslen(g_Link.url[i]) == 0)continue;
			if (FALSE == InsertMenu(hM2, i, MF_BYPOSITION | MF_STRING, IDM_LINK1 + i, g_Link.link[i])) {//0.50�ŕύX
				g_ErrorCode = 105;
				ErrorNum(hWnd);
			}
		}
		//0.57�Œǉ�
		SetTimer(hWnd, ID_TIMER, 1000, NULL);
		break;

	case WM_TIMER://0.57�Œǉ�
		KillTimer(hWnd, ID_TIMER);
		//�����\�[�g���� 0.57�Œǉ�
		if (nAutoSort && FALSE == ListView_SortItems(g_hList, CmpProc, nAutoSort & 0xFFFF)) {
			g_ErrorCode = 120;
			ErrorNum(hWnd);
		}
		//SetTimer(hWnd,ID_TIMER,g_Timer,NULL);
		SetTimer(hWnd, ID_TIMER, 1000, NULL);//0.57a�ŕύX
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_REFRESH:
		case IDM_LIST_UPDATE:
			//���X�g���N���A
			if (FALSE == ListView_DeleteAllItems(GetDlgItem(hWnd, IDC_LIST))) {
				g_ErrorCode = 110;
				ErrorNum(hWnd);
			}
			//���X�g���X�V
			if (0 == MakeList(hWnd)) {
				g_ErrorCode = 111;
				ErrorNum(hWnd);
			}
			break;

		case IDM_LIST_ALWAYS_UPDATE:
			if (GetMenuCheck(hWnd, hSubMenu, IDM_FILE_AUTOLOAD)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_FILE_AUTOLOAD);
			}
			ChangeMenuCheck(hWnd, hSubMenu, IDM_LIST_ALWAYS_UPDATE);
			break;

		case IDM_FILE_AUTOLOAD:
			if (GetMenuCheck(hWnd, hSubMenu, IDM_LIST_ALWAYS_UPDATE)) {
				ChangeMenuCheck(hWnd, hSubMenu, IDM_LIST_ALWAYS_UPDATE);
			}
			ChangeMenuCheck(hWnd, hSubMenu, IDM_FILE_AUTOLOAD);
			break;

		case IDM_FILE_LOAD:
			int uFlag;
			if (lp == IDM_FILE_AUTOLOAD)uFlag = 1;
			else uFlag = 0;
			if (List_ReadNPC(hWnd, g_hList, uFlag) <= 0) {
				MessageBox(hWnd, _T("NPC���X�g�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B"), MAPPU_VER, MB_OK | MB_ICONERROR);
			}
			break;

		case IDM_FILE_SAVE:
			if (List_WriteNPC(hWnd, g_hList) <= 0) {
				MessageBox(hWnd, _T("NPC���X�g�t�@�C���̕ۑ��Ɏ��s���܂����B"), MAPPU_VER, MB_OK | MB_ICONERROR);
			}
			break;

		case IDM_LIST_SET_ITEMS:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_SetCheckState(g_hList, nItem, 1);//�Z�b�g
			}
			break;

		case IDM_LIST_CLEAR_ITEMS:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_SetCheckState(g_hList, nItem, 0);//���Z�b�g
			}
			break;

		case IDM_LIST_SET_ALL:
			ListView_SetCheckState(g_hList, -1, 1);
			break;

		case IDM_LIST_CLEAR_ALL:
			ListView_SetCheckState(g_hList, -1, 0);
			break;

		case IDM_LIST_TYPE_NORMAL:
			nItem = -1;
			LVITEM item;
			item.mask = LVIF_TEXT;
			item.iSubItem = 4;//Sound
			item.pszText = _T("");//������
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				item.iItem = nItem;
				if (FALSE == ListView_SetItem(g_hList, &item)) {
					g_ErrorCode = 115;
					ErrorNum(hWnd);
				}
			}
			break;

		case IDM_LIST_TYPE_EX:
			nItem = -1;
			item.mask = LVIF_TEXT;
			item.iSubItem = 4;//Sound
			item.pszText = _T("EX");
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_SetCheckState(g_hList, nItem, 1);//�Z�b�g 0.49�Œǉ�
				if (ListView_GetCheckState(g_hList, nItem) != 0) {
					item.iItem = nItem;
					if (FALSE == ListView_SetItem(g_hList, &item)) {
						g_ErrorCode = 116;
						ErrorNum(hWnd);
					}
				}
			}
			break;

		case IDM_LIST_TYPE_SP:
			nItem = -1;
			item.mask = LVIF_TEXT;
			item.iSubItem = 4;//Sound
			item.pszText = _T("SP");
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_SetCheckState(g_hList, nItem, 1);//�Z�b�g 0.49�Œǉ�
				if (ListView_GetCheckState(g_hList, nItem) != 0) {
					item.iItem = nItem;
					if (FALSE == ListView_SetItem(g_hList, &item)) {
						g_ErrorCode = 117;
						ErrorNum(hWnd);
					}
				}
			}
			break;

		case IDM_LIST_CLOSE:
			ShowWindow(hWnd, SW_HIDE);
			break;

		case IDM_ALPHA_NPCLIST_0:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 0)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_10:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 10)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_20:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 20)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_30:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 30)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_40:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 40)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_50:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 50)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_60:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 60)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_70:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 70)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_80:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 80)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_ALPHA_NPCLIST_90:
			if (-1 == AlphaValueSetNPCLIST(hWnd, hSubMenu, 90)) {
				ErrorNum(hWnd);
			}
			break;

		case IDM_HIDE_TOOL_BAR:
			ShowWindow(hWnd, SW_HIDE);
			if (1 == ChangeMenuCheck(hWnd, hSubMenu, IDM_HIDE_TOOL_BAR)) {
				LONG exStyle = GetWindowLong(hWnd, GWL_STYLE);
				exStyle &= ~(WS_CAPTION | WS_THICKFRAME);
				SetWindowLong(hWnd, GWL_STYLE, exStyle);
			}
			else {
				LONG exStyle = GetWindowLong(hWnd, GWL_STYLE);
				exStyle |= (WS_CAPTION | WS_THICKFRAME);
				SetWindowLong(hWnd, GWL_STYLE, exStyle);
			}
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME | SWP_NOZORDER | SWP_FRAMECHANGED);
			ShowWindow(hWnd, SW_SHOW);
			break;

		case IDM_HIDE_BUTTONBOX:
			ChangeMenuCheck(hWnd, hSubMenu, IDM_HIDE_BUTTONBOX);
			if (GetMenuCheck(hWnd, hSubMenu, IDM_HIDE_BUTTONBOX)) {
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_REFRESH), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_EDIT_SEARCH), SW_HIDE);
			}
			else {
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_REFRESH), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_EDIT_SEARCH), SW_SHOW);
			}
			SendMessage(hWnd, WM_SIZE, NULL, NULL);
			break;

		case IDM_LINK1:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_GetItemText(g_hList, nItem, 1, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
				if (wcslen(szName) >= 3) {
					swprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[0], szName);
					ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
				}
				break;
			}
			break;

		case IDM_LINK2:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_GetItemText(g_hList, nItem, 1, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
				if (wcslen(szName) >= 3) {
					swprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[1], szName);
					ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
				}
				break;
			}
			break;

		case IDM_LINK3:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_GetItemText(g_hList, nItem, 1, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
				if (wcslen(szName) >= 3) {
					swprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[2], szName);
					ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
				}
				break;
			}
			break;

		case IDM_LINK4:
			nItem = -1;
			while ((nItem = ListView_GetNextItem(g_hList, nItem, LVNI_ALL | LVNI_SELECTED)) != -1) {
				ListView_GetItemText(g_hList, nItem, 1, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
				if (wcslen(szName) >= 3) {
					swprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[3], szName);
					ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
				}
				break;
			}
			break;

		case IDM_EXECUTE_EDIT:
			ShellExecute(NULL, _T("open"), _T("Edit.exe"), NULL, NULL, SW_SHOWNORMAL);
			break;

		case IDC_EDIT_SEARCH:
			if (HIWORD(wp) == EN_MAXTEXT) {
				break;
			}
			else if (HIWORD(wp) == EN_SETFOCUS) {
				SetDlgItemText(hWnd, IDC_EDIT_SEARCH, NULL);
				nSearchFlag = 1;//0.48�Œǉ�
			}
			else if (HIWORD(wp) == EN_KILLFOCUS) {
				SetDlgItemText(hWnd, IDC_EDIT_SEARCH, _T("Search"));
				nSearchFlag = 0;//0.48�Œǉ�
				nSearchSelected = -1;//0.48�Œǉ�
			}
			else if (HIWORD(wp) == EN_UPDATE) {
				_TCHAR szSearch[MAX_PATH];
				_TCHAR szName[0x20];
				int nItemCount;
				GetDlgItemText(hWnd, IDC_EDIT_SEARCH, szSearch, sizeof(szSearch) / sizeof(_TCHAR) - 1);
				if (_tcslen(szSearch) == 0)break;
				else if (isupper(szSearch[0]) == 0) {
					szSearch[0] = toupper(szSearch[0]);
				}
				//�A�C�e�������擾
				nItemCount = ListView_GetItemCount(g_hList);
				//���ׂĂ̑I��������
				ListView_SetItemState(g_hList, -1, 0, LVIS_SELECTED);
				//NPC���T�[�`
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iSubItem = 1;//��
				item.pszText = szName;//Name
				item.cchTextMax = sizeof(szName) / sizeof(_TCHAR);
				int nFlag = 0;
				for (int i = 0; i < nItemCount; i++) {
					item.iItem = i;//�s
					ListView_GetItem(g_hList, &item);
					if (_tcsncmp(szSearch, szName, _tcslen(szSearch)) == 0) {
						nSearchSelected = i;//0.48�Œǉ�
						ListView_EnsureVisible(g_hList, i, FALSE);
						nFlag = 1;
						break;
					}
				}
				if (nFlag == 0 && _tcscmp(szSearch, _T("Search")) != 0) {
					MessageBeep(MB_ICONHAND);//������Ȃ�������
				}
			}
			break;
		}
		break;

	case WM_NOTIFY:
		if ((int)wp == IDC_LIST) {
			switch (((LV_DISPINFO*)lp)->hdr.code) {
				//�J�����N���b�N��
			case LVN_COLUMNCLICK: {
				_TCHAR szWindowText[0x20];
				//�w�b�_�[�Ɂ����}�[�N��\�� 0.50�Œǉ�
				HWND hHeader = ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST));

				//���Z�b�g
				HDITEM headerItem;
				for (int i = 0; i < 7; i++) {
					headerItem.mask = HDI_FORMAT;
					Header_GetItem(hHeader, i, &headerItem);
					headerItem.fmt &= ~HDF_SORTDOWN & ~HDF_SORTUP;
					Header_SetItem(hHeader, i, &headerItem);
				}

				//Ctrl+�N���b�N�Ŏ����X�V�I�� 0.57�Œǉ�
				if (GetKeyState(VK_CONTROL) & 0x80) {
					nAutoSort = 0x80000000 + ((NM_LISTVIEW*)lp)->iSubItem;
					if (g_Sort[((NM_LISTVIEW*)lp)->iSubItem] == UP) {//0.57a�Œǉ�
						_stprintf_s(szWindowText, _T("NPCLIST AutoSort [%s] up"), szColumn[((NM_LISTVIEW*)lp)->iSubItem]);
					}
					else {
						_stprintf_s(szWindowText, _T("NPCLIST AutoSort [%s] down"), szColumn[((NM_LISTVIEW*)lp)->iSubItem]);
					}
					SetWindowText(hWnd, szWindowText);
					break;
				}
				else if (nAutoSort) {
					SetWindowText(hWnd, _T("NPCLIST"));
					nAutoSort = 0;
					break;
				}

				headerItem.mask = HDI_FORMAT;
				Header_GetItem(hHeader, ((NM_LISTVIEW*)lp)->iSubItem, &headerItem);
				headerItem.fmt &= ~HDF_SORTDOWN & ~HDF_SORTUP;

				//�\�[�g����
				if (g_Sort[((NM_LISTVIEW*)lp)->iSubItem] == UP) {
					g_Sort[((NM_LISTVIEW*)lp)->iSubItem] = DOWN;
					headerItem.fmt |= HDF_SORTDOWN | HDF_STRING;
				}
				else {
					g_Sort[((NM_LISTVIEW*)lp)->iSubItem] = UP;
					headerItem.fmt |= HDF_SORTUP | HDF_STRING;
				}
				if (FALSE == ListView_SortItems(g_hList, CmpProc, ((NM_LISTVIEW*)lp)->iSubItem)) {
					g_ErrorCode = 120;
					ErrorNum(hWnd);
				}
				Header_SetItem(hHeader, ((NM_LISTVIEW*)lp)->iSubItem, &headerItem);
				break;
			}

			case LVN_BEGINLABELEDIT:
				_TCHAR szText[MAX_PATH];
				hEdit = ListView_GetEditControl(g_hList);
				//ListView_GetItemText(g_hList,((LV_DISPINFO*)lp)->item.iItem,5,szText,sizeof(szText)/sizeof(_TCHAR));
				ListView_GetItemText(g_hList, ((LV_DISPINFO*)lp)->item.iItem, 6, szText, sizeof(szText) / sizeof(_TCHAR));//0.57�ŕύX
#ifdef _DEBUG
				DWORD dwAddress;
				int id;
				ListView_GetItemText(g_hList, ((LV_DISPINFO*)lp)->item.iItem, 0, szText, sizeof(szText) / sizeof(_TCHAR));
				id = _tcstoul(szText, NULL, 16);
				//���������[�h
				dwAddress = g_DllAddress + g_Offset.NpcMap + id * sizeof(DWORD);
				ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL);
				_stprintf_s(szText, _T("%08X"), dwAddress);
#endif
				SetWindowText(hEdit, szText);
				break;

			case LVN_ENDLABELEDIT:
				GetWindowText(hEdit, szText, sizeof(szText) / sizeof(_TCHAR) - 1);
				//ListView_SetItemText(g_hList,((LV_DISPINFO*)lp)->item.iItem,5,szText);
				ListView_SetItemText(g_hList, ((LV_DISPINFO*)lp)->item.iItem, 6, szText);//0.57�ŕύX
				break;

			default:
				break;
			}
		}

		if (((LPNMHDR)lp)->hwndFrom == GetDlgItem(hWnd, IDC_LIST)) {//IDC_LIST�̃J�X�^���h���[ 0.48�Œǉ�
			if (((LPNMLISTVIEW)lp)->hdr.code == NM_CUSTOMDRAW) {
				switch (((LPNMLVCUSTOMDRAW)lp)->nmcd.dwDrawStage) {
				case CDDS_PREPAINT:
					SetWindowLong(hWnd, DWL_MSGRESULT, (LONG)CDRF_NOTIFYITEMDRAW);
					return TRUE;
				case CDDS_ITEMPREPAINT: {
					COLORREF color = RGB(255, 255, 255);
					_TCHAR szBuf[0x20];
					ZeroMemory(szBuf, sizeof(szBuf));
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST), ((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec, 4, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);

					if (g_NPCLIST_BkColor == 1 && _tcscmp(szBuf, _T("EX")) == 0) {//EX
						ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST), ((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec, 2, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
						if (_tcscmp(szBuf, _T("Corpse")) == 0) {
							color = g_ColorNPC_Corpse[1];//�w�i�F��ύX

						}
						else if (_tcscmp(szBuf, _T("War")) == 0) {
							color = g_ColorNPC_War[1];//�w�i�F��ύX
						}
						else if (_tcscmp(szBuf, _T("Pop")) == 0) {
							color = g_ColorNPC_Pop[1];//�w�i�F��ύX
						}
						//else if(_tcscmp(szBuf,_T("-")) == 0){
						//	color = RGB(255,255,255);//�w�i�F��ύX
						//}

						((LPNMLVCUSTOMDRAW)lp)->clrTextBk = color;
						if ((int)((color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF)) < 0x180) {
							((LPNMLVCUSTOMDRAW)lp)->clrText = RGB(255, 255, 255);
						}
					}
					else if (g_NPCLIST_BkColor == 1 && _tcscmp(szBuf, _T("SP")) == 0) {//SP
						ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST), ((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec, 2, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
						if (_tcscmp(szBuf, _T("Corpse")) == 0) {
							color = g_ColorNPC_Corpse[2];//�w�i�F��ύX
						}
						else if (_tcscmp(szBuf, _T("War")) == 0) {
							color = g_ColorNPC_War[2];//�w�i�F��ύX
						}
						else if (_tcscmp(szBuf, _T("Pop")) == 0) {
							color = g_ColorNPC_Pop[2];//�w�i�F��ύX
						}
						//else if(_tcscmp(szBuf,_T("-")) == 0){
						//	color = RGB(255,255,255);//�w�i�F��ύX
						//}

						((LPNMLVCUSTOMDRAW)lp)->clrTextBk = color;
						if ((int)((color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF)) < 0x180) {
							((LPNMLVCUSTOMDRAW)lp)->clrText = RGB(255, 255, 255);
						}
					}

					if (nSearchFlag == 1)
						if (1 && ((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec == nSearchSelected) {
							((LPNMLVCUSTOMDRAW)lp)->clrTextBk = RGB(0x31, 0x6a, 0xc5);//�w�i�F��ύX
							((LPNMLVCUSTOMDRAW)lp)->clrText = RGB(0xff, 0xff, 0xff);//�����F��ύX
						}

					SetWindowLong(hWnd, DWL_MSGRESULT, (LONG)CDRF_NEWFONT);
					return TRUE; }
				default:
					SetWindowLong(hWnd, DWL_MSGRESULT, (LONG)CDRF_DODEFAULT);
					return FALSE;
				}
			}
		}
		break;

	case WM_RBUTTONUP:
		POINT pos;
		pos.x = (LONG)(SHORT)LOWORD(lp);
		pos.y = (LONG)(SHORT)HIWORD(lp);
		//���j���[�\��
		if (wp != (WPARAM)hWnd) {//MAP�E�B���h�E����̃��b�Z�[�W�ł͖�����
			if (0 == ClientToScreen(hWnd, &pos)) {
				g_ErrorCode = 130;
				ErrorNum(hWnd);
			}
		}
		SetForegroundWindow(hWnd);
		if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
			GETLASTERROR;
			g_ErrorCode = 131;
			ErrorNum(hWnd);
		}
		PostMessage(hWnd, 0, 0, 0);
		break;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		//�T�C�Y�ύX�����������A�e�R���g���[���̑傫����ύX
		if (GetMenuCheck(hWnd, hSubMenu, IDM_HIDE_BUTTONBOX)) {
			MoveWindow(GetDlgItem(hWnd, IDC_LIST), 1, 1, rect.right - 1 * 2, rect.bottom - 1 * 2, 1);
		}
		else {
			MoveWindow(GetDlgItem(hWnd, IDC_LIST), 1, 1, rect.right - 1 * 2, rect.bottom - 1 * 2 - 1 - 18, 1);
			MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_REFRESH), 1, rect.bottom - 18 - 1, rect.right - 1 * 3 - 50, 18, 1);
			MoveWindow(GetDlgItem(hWnd, IDC_EDIT_SEARCH), rect.right - 1 * 1 - 50, rect.bottom - 16 - 1, 50 - 1, 16, 1);
		}
		break;

	case WM_CLOSE:
		PostMessage(GetParent(hWnd), WM_COMMAND, IDM_NPCLIST, 0);
		break;

	case WM_DESTROY:
		DestroyMenu(hMenu);
		DestroyWindow(0);
		break;
	}
	return 0;
}

//EnumWindows�̃v���V�[�W��0.52�Œǉ�
BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lp)
{
	_TCHAR szText[MAX_PATH];
	_TCHAR szCmp[MAX_PATH];
	HWND *hRetWnd;
	hRetWnd = (HWND*)lp;

	GetWindowText(hWnd, szText, MAX_PATH);
	if (wcsncmp(szText, _T("�����݂݃n���P�[��"), wcslen(_T("�����݂݃n���P�[��"))) == 0) {//������v
		_stprintf_s(szCmp, _T("[pol.exe  (PID: %08X)]"), g_ProcessID);
		for (int i = 0; szText[i]; i++) {
			if (wcsncmp(&szText[i], szCmp, wcslen(szCmp)) == 0) {
				*hRetWnd = hWnd;
				return FALSE;//�I���
			}
		}
	}
	return TRUE;//�p��
}

//�E�B���h�E�v���V�[�W��IDD_DIALOG_CHARACTER
LRESULT CALLBACK WndProcCHARACTER(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		//���ߐݒ�
		LONG exStyle;
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//���݂�STYLE���擾
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);//�ݒ�
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), NULL, LWA_COLORKEY);//RGB(0,0,0)�͓��߂�����̂Ŏg��Ȃ�����
		break;

	case WM_CLOSE:
		break;

	}
	return 0;
}

//�E�B���h�E�v���V�[�W��IDD_DIALOG_SETTING
LRESULT CALLBACK WndProcSETTING(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMHDR lpnmh;
	LPNMTREEVIEW lpnmtv;
	TVITEM tvSelectItem;
	_TCHAR szBuffer[DF_BUFFSIZE + 1];

	switch (msg) {
	case WM_INITDIALOG:
		//�c���[���擾
		g_hTree = GetDlgItem(hWnd, IDC_TREE);
		if (!g_hTree) {
			g_ErrorCode = 102;
			ErrorNum(hWnd);
		}
		//�c���[��������
		if (0 == InitTree(g_hTree)) {
			g_ErrorCode = 103;
			ErrorNum(hWnd);
		}

		//�c���[���T�u�N���X��
		//WndProcMainClass = (WNDPROC)SetWindowLong(g_hList, GWL_WNDPROC, (LONG)WndProcSubClass);
		break;

	case WM_NOTIFY:
		if ((int)wp == IDC_TREE) {
			lpnmh = (LPNMHDR)lp;

			switch (lpnmh->code) {
				/* �A�C�e���̑I��ύX */
				case TVN_SELCHANGED:
					/* ������ */
					memset(&tvSelectItem, 0x00, sizeof(tvSelectItem));
					memset(szBuffer, 0x00, sizeof(szBuffer));

					/* NMTREEVIEW�\���̂ɏ��擾 */
					lpnmtv = (LPNMTREEVIEW)lpnmh;

					/* hItem���Z�b�g itemNew��mask,hItem,state,lParam���g�p�\ */
					tvSelectItem.hItem = lpnmtv->itemNew.hItem;
					tvSelectItem.cchTextMax = DF_BUFFSIZE;
					tvSelectItem.pszText = szBuffer;
					tvSelectItem.pszText[sizeof(szBuffer-1)] = 0;
					tvSelectItem.mask = TVIF_TEXT;

					/* �A�C�e���̎擾 */
					TreeView_GetItem(g_hTree, &tvSelectItem);
					_tcscpy(g_szSelectItem, tvSelectItem.pszText);

					/* �`�F�b�N�{�b�N�X�̏�Ԃ̎擾 */
					UINT lFlag = TreeView_GetCheckState(g_hTree, lpnmtv->itemNew.hItem);
					if (lFlag == 0) {
						_tcscpy(g_szGetCheck, _T(DF_NOT_CHECK));
					}
					else {
						_tcscpy(g_szGetCheck, _T(DF_CHECK));
					}

					GetTreeViewCheck(lpnmtv->itemNew.hItem);

					/* �ĕ`�悳���� */
					InvalidateRect(hWnd, NULL, TRUE);
					break;
			}

		}
		break;
	case WM_CLOSE:
		break;

	}
	return 0;
}

/// <summary>
/// GetTreeCheck(�`�F�b�N��Ԃ̎擾)
/// </summary>
/// <param name="hItem"></param>
/// <returns></returns>
long GetTreeViewCheck(HTREEITEM hItem) {
	TVITEM  tvItem;
	long    lFlag = 0;

	/* ������ */
	memset(&tvItem, 0x00, sizeof(tvItem));
	memset(g_szGetCheck, 0x00, sizeof(g_szGetCheck));

	/* �`�F�b�N��Ԏ擾�p���̃Z�b�g */
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	/* �A�C�e���̎擾 */
	TreeView_GetItem(g_hTree, &tvItem);

	/* �`�F�b�N�{�b�N�X�̏�Ԃ��擾     */
	/* lFlag = 1 : �`�F�b�N����Ă���   */
	/* lFlag = 0 : �`�F�b�N����Ă��Ȃ� */
	lFlag = ((BOOL)(tvItem.state >> 12) - 1);

	if (lFlag == 0) {
		_tcscpy(g_szGetCheck, _T(DF_NOT_CHECK));
	}
	else {
		_tcscpy(g_szGetCheck, _T(DF_CHECK));
	}

	return 0;
}

long SetTreeViewCheck(HWND hWnd, HTREEITEM hItem, bool isCheck){
	TVITEM  tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	tvItem.state = (isCheck ? 0x2000 : 0x1000);

	TreeView_SetCheckState(g_hTree, hItem, isCheck); //�ŏo�������ȋC������H�H
	TreeView_SetItem(g_hTree, &tvItem); //SendMessage(hWnd, TVM_SETITEM, 0, (LPARAM)&tvItem);
	return 0;
}

//�E�B���h�E�v���V�[�W��IDD_DIALOG_COLOR
LRESULT CALLBACK WndProcCOLOR(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		RECT rect;
		GetWindowRect(GetParent(g_hList), &rect);
		if (0 == SetWindowPos(hWnd, 0, rect.left, rect.top, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE)) {
			g_ErrorCode = 3000;
			ErrorNum(hWnd);
		}
		break;

	case WM_DRAWITEM:
		COLORREF color;
		color = 0xFFFFFFFF;
		switch (((LPDRAWITEMSTRUCT)(lp))->CtlID) {
		case IDC_BUTTON_NPC_POP:
			color = g_ColorNPC_Pop[0]; break;
		case IDC_BUTTON_NPC_SHARE:
			color = g_ColorNPC_Share[0]; break;
		case IDC_BUTTON_NPC_WAR:
			color = g_ColorNPC_War[0]; break;
		case IDC_BUTTON_NPC_CORPSE:
			color = g_ColorNPC_Corpse[0]; break;
		case IDC_BUTTON_NPC_OUT:
			color = g_ColorNPC_Out[0]; break;
		case IDC_BUTTON_NPC_DROPSHADOW:
			color = g_Font.ColorEffect; break;
		case IDC_BUTTON_NPC_POP_EX:
			color = g_ColorNPC_Pop[1]; break;
		case IDC_BUTTON_NPC_SHARE_EX:
			color = g_ColorNPC_Share[1]; break;
		case IDC_BUTTON_NPC_WAR_EX:
			color = g_ColorNPC_War[1]; break;
		case IDC_BUTTON_NPC_CORPSE_EX:
			color = g_ColorNPC_Corpse[1]; break;
		case IDC_BUTTON_NPC_OUT_EX:
			color = g_ColorNPC_Out[1]; break;
		case IDC_BUTTON_NPC_DROPSHADOW_EX:
			color = g_FontEx.ColorEffect; break;
		case IDC_BUTTON_NPC_POP_SP:
			color = g_ColorNPC_Pop[2]; break;
		case IDC_BUTTON_NPC_SHARE_SP:
			color = g_ColorNPC_Share[2]; break;
		case IDC_BUTTON_NPC_WAR_SP:
			color = g_ColorNPC_War[2]; break;
		case IDC_BUTTON_NPC_CORPSE_SP:
			color = g_ColorNPC_Corpse[2]; break;
		case IDC_BUTTON_NPC_OUT_SP:
			color = g_ColorNPC_Out[2]; break;
		case IDC_BUTTON_NPC_DROPSHADOW_SP:
			color = g_FontSp.ColorEffect; break;
		case IDC_BUTTON_OWN:
			color = g_ColorOWN; break;
		case IDC_BUTTON_PC:
			color = g_ColorPC; break;
		case IDC_BUTTON_PARTY:
			color = g_ColorPARTY; break;
		case IDC_BUTTON_ALIANCE:
			color = g_ColorALIANCE; break;
		case IDC_BUTTON_PET:
			color = g_ColorPET; break;
		case IDC_BUTTON_DROPSHADOW_PC:
			color = g_FontPC.ColorEffect; break;
		case IDC_BUTTON_MAPNAME:
			color = g_ColorMapName; break;
		case IDC_BUTTON_ZOOM:
			color = g_ColorZoom; break;
		case IDC_BUTTON_MYPOSITION:
			color = g_ColorMyPosition; break;
		case IDC_BUTTON_POS:
			color = g_Colorlt_Pos_gt; break;
		case IDC_BUTTON_DROPSHADOW_SYSTEM:
			color = g_FontSystem.ColorEffect; break;
		case IDC_BUTTON_TARGET:
			color = g_ColorTarget; break;
		case IDC_BUTTON_DROPSHADOW_TARGET:
			color = g_FontTarget.ColorEffect; break;
		case IDC_BUTTON_DISTANCE:
			color = g_ColorDistance; break;
		case IDC_BUTTON_CIRCLE_VISIBLE:
			color = g_ColorVisible; break;
		case IDC_BUTTON_CIRCLE_ABILITY:
			color = g_ColorAbility; break;
		case IDC_BUTTON_CIRCLE_MAGIC:
			color = g_ColorMagic; break;
		case IDC_BUTTON_CIRCLE_RANGE:
			color = g_ColorRange; break;
		case IDC_BUTTON_CIRCLE_WINDSONG:
			color = g_ColorWindSong; break;
		case IDC_BUTTON_CIRCLE_DETECTION:
			color = g_ColorDetection; break;
		}

		if (color != 0xFFFFFFFF) {
			HDC hDC = ((LPDRAWITEMSTRUCT)(lp))->hDC;
			_TCHAR szText[0x10];
			SetBkMode(hDC, TRANSPARENT);
			HPEN hPen = CreatePen(PS_INSIDEFRAME, 1, color);
			HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
			HBRUSH hBrush = CreateSolidBrush(color);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Rectangle(hDC, 0, 0, ((LPDRAWITEMSTRUCT)(lp))->rcItem.right, ((LPDRAWITEMSTRUCT)(lp))->rcItem.bottom);
			SelectObject(hDC, hOldPen);
			SelectObject(hDC, hOldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);

			if ((int)((color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF)) > 0x180) {
				color = RGB(0, 0, 0);
			}
			else {
				color = RGB(255, 255, 255);
			}
			SetTextColor(hDC, color);
			GetDlgItemText(hWnd, ((LPDRAWITEMSTRUCT)(lp))->CtlID, szText, sizeof(szText) / sizeof(_TCHAR));
			DrawText(hDC, szText, -1, &((LPDRAWITEMSTRUCT)(lp))->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_NPC_POP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Pop[0]); break;
		case IDC_BUTTON_NPC_SHARE:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Share[0]); break;
		case IDC_BUTTON_NPC_WAR:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_War[0]); break;
		case IDC_BUTTON_NPC_CORPSE:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Corpse[0]); break;
		case IDC_BUTTON_NPC_OUT:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Out[0]); break;
		case IDC_BUTTON_NPC_DROPSHADOW:
			SetColor(hWnd, LOWORD(wp), &g_Font.ColorEffect); break;
		case IDC_BUTTON_NPC_POP_EX:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Pop[1]); break;
		case IDC_BUTTON_NPC_SHARE_EX:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Share[1]); break;
		case IDC_BUTTON_NPC_WAR_EX:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_War[1]); break;
		case IDC_BUTTON_NPC_CORPSE_EX:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Corpse[1]); break;
		case IDC_BUTTON_NPC_OUT_EX:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Out[1]); break;
		case IDC_BUTTON_NPC_DROPSHADOW_EX:
			SetColor(hWnd, LOWORD(wp), &g_FontEx.ColorEffect); break;
		case IDC_BUTTON_NPC_POP_SP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Pop[2]); break;
		case IDC_BUTTON_NPC_SHARE_SP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Share[2]); break;
		case IDC_BUTTON_NPC_WAR_SP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_War[2]); break;
		case IDC_BUTTON_NPC_CORPSE_SP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Corpse[2]); break;
		case IDC_BUTTON_NPC_OUT_SP:
			SetColor(hWnd, LOWORD(wp), &g_ColorNPC_Out[2]); break;
		case IDC_BUTTON_NPC_DROPSHADOW_SP:
			SetColor(hWnd, LOWORD(wp), &g_FontSp.ColorEffect); break;
		case IDC_BUTTON_OWN:
			SetColor(hWnd, LOWORD(wp), &g_ColorOWN); break;
		case IDC_BUTTON_PC:
			SetColor(hWnd, LOWORD(wp), &g_ColorPC); break;
		case IDC_BUTTON_PARTY:
			SetColor(hWnd, LOWORD(wp), &g_ColorPARTY); break;
		case IDC_BUTTON_ALIANCE:
			SetColor(hWnd, LOWORD(wp), &g_ColorALIANCE); break;
		case IDC_BUTTON_PET:
			SetColor(hWnd, LOWORD(wp), &g_ColorPET); break;
		case IDC_BUTTON_DROPSHADOW_PC:
			SetColor(hWnd, LOWORD(wp), &g_FontPC.ColorEffect); break;
		case IDC_BUTTON_MAPNAME:
			SetColor(hWnd, LOWORD(wp), &g_ColorMapName); break;
		case IDC_BUTTON_ZOOM:
			SetColor(hWnd, LOWORD(wp), &g_ColorZoom); break;
		case IDC_BUTTON_MYPOSITION:
			SetColor(hWnd, LOWORD(wp), &g_ColorMyPosition); break;
		case IDC_BUTTON_POS:
			SetColor(hWnd, LOWORD(wp), &g_Colorlt_Pos_gt); break;
		case IDC_BUTTON_DROPSHADOW_SYSTEM:
			SetColor(hWnd, LOWORD(wp), &g_FontSystem.ColorEffect); break;
		case IDC_BUTTON_TARGET:
			SetColor(hWnd, LOWORD(wp), &g_ColorTarget); break;
		case IDC_BUTTON_DROPSHADOW_TARGET:
			SetColor(hWnd, LOWORD(wp), &g_FontTarget.ColorEffect); break;
		case IDC_BUTTON_DISTANCE:
			SetColor(hWnd, LOWORD(wp), &g_ColorDistance); break;
		case IDC_BUTTON_CIRCLE_VISIBLE:
			SetColor(hWnd, LOWORD(wp), &g_ColorVisible); break;
		case IDC_BUTTON_CIRCLE_ABILITY:
			SetColor(hWnd, LOWORD(wp), &g_ColorAbility); break;
		case IDC_BUTTON_CIRCLE_MAGIC:
			SetColor(hWnd, LOWORD(wp), &g_ColorMagic); break;
		case IDC_BUTTON_CIRCLE_RANGE:
			SetColor(hWnd, LOWORD(wp), &g_ColorRange); break;
		case IDC_BUTTON_CIRCLE_WINDSONG:
			SetColor(hWnd, LOWORD(wp), &g_ColorWindSong); break;
		case IDC_BUTTON_CIRCLE_DETECTION:
			SetColor(hWnd, LOWORD(wp), &g_ColorDetection); break;
		default:break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}


//LISTVIEW�R���g���[���̃T�u�N���X��
LRESULT CALLBACK WndProcSubClass(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_CONTEXTMENU) {//WM_RBUTTONUP��肱���炪�D��
								//�}�E�X�̃X�N���[�����W���N���C�A���g���W�ɕϊ�
		POINT Point;
		Point.x = LOWORD(lp);
		Point.y = HIWORD(lp);
		ScreenToClient(GetParent(hWnd), &Point);
		lp = Point.x + ((Point.y) << 16);
		SendMessage(GetParent(hWnd), WM_RBUTTONUP, wp, lp);
		return 0;
	}
	else if (msg == WM_KEYDOWN && wp == VK_SPACE) {
		if (ListView_GetItemCount(hWnd) == 0) {
			return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
		}
		int nItem = -1;
		while ((nItem = ListView_GetNextItem(hWnd, nItem, LVNI_SELECTED)) != -1) {
			if (ListView_GetCheckState(hWnd, nItem) != 0) {
				ListView_SetCheckState(hWnd, nItem, 0);//���Z�b�g
			}
			else {
				ListView_SetCheckState(hWnd, nItem, 1);//�Z�b�g
			}
		}
		return 0;
	}
	else if (msg == WM_KEYDOWN && wp == VK_F2) {
		_TCHAR szText[MAX_PATH];
		DWORD dwAddress;
		int id;
		int nItemNow;
		HWND hUsaWnd;
		HWND hUsaSendWnd;
		hUsaWnd = 0;

		if (EnumWindows(&EnumWndProc, (LPARAM)&hUsaWnd) == FALSE) {//���������̏���
																   //���݂̑I���ʒu���擾
			nItemNow = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);
			ListView_GetItemText(hWnd, nItemNow, 0, szText, sizeof(szText) / sizeof(_TCHAR));
			szText[sizeof(szText - 1)] = 0;
			id = _tcstoul(szText, NULL, 16);
			//���������[�h
			dwAddress = g_DllAddress + g_Offset.NpcMap + id * sizeof(DWORD);
			ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL);
			if (dwAddress == 0)dwAddress = g_DllAddress;//dwAddress��0�̎������݂݂̃A�h���X���ς��Ȃ��̂�dll�̃A�h���X�Ɉړ������� 0.53�Œǉ�
			_stprintf_s(szText, _T("%08X"), dwAddress);

			//�����݂�0.09�̕\���A�h���X��16�i���Ŏw����R���g���[�� 0.52�Œǉ�
			for (int i = 0; i<100; i++) {
				if ((hUsaSendWnd = FindWindow(NULL, _T("�\���A�h���X��16�i���Ŏw��"))) != NULL) {
					SendMessage(GetDlgItem(hUsaSendWnd, 1012), WM_SETTEXT, 0, (LPARAM)szText);//WM_SETTEXT/WM_COPYDATA�͗�O�I�ɕʃv���Z�X�ɕ�����𑗐M�ł���
					SendMessage(GetDlgItem(hUsaSendWnd, 1), WM_LBUTTONDOWN, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 1), WM_LBUTTONUP, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 2), WM_LBUTTONDOWN, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 2), WM_LBUTTONUP, 0, 0);
					break;
				}
				else if (i % 25 == 0) {
					SendMessage(hUsaWnd, WM_COMMAND, 32785, 0);
					Sleep(100);
				}
			}
		}
	}
	else if (msg == WM_KEYDOWN && wp == VK_F3) {//�^�[�Q�b�g��NPC_MEM 0.53�Œǉ�
		_TCHAR szText[MAX_PATH];
		DWORD dwAddress;
		HWND hUsaWnd;
		HWND hUsaSendWnd;
		INFORMATION targetinfo;

		hUsaWnd = 0;

		if (EnumWindows(&EnumWndProc, (LPARAM)&hUsaWnd) == FALSE) {//���������̏���
			targetinfo = GetTargetInfo();
			//���������[�h
			dwAddress = g_DllAddress + g_Offset.NpcMap + targetinfo.id * sizeof(DWORD);
			ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL);
			if (dwAddress == 0)dwAddress = g_DllAddress;
			_stprintf_s(szText, _T("%08X"), dwAddress);

			//�����݂�0.09�̕\���A�h���X��16�i���Ŏw����R���g���[�� 0.52�Œǉ�
			for (int i = 0; i<100; i++) {
				if ((hUsaSendWnd = FindWindow(NULL, _T("�\���A�h���X��16�i���Ŏw��"))) != NULL) {
					SendMessage(GetDlgItem(hUsaSendWnd, 1012), WM_SETTEXT, 0, (LPARAM)szText);//WM_SETTEXT/WM_COPYDATA�͗�O�I�ɕʃv���Z�X�ɕ�����𑗐M�ł���
					SendMessage(GetDlgItem(hUsaSendWnd, 1), WM_LBUTTONDOWN, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 1), WM_LBUTTONUP, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 2), WM_LBUTTONDOWN, 0, 0);
					SendMessage(GetDlgItem(hUsaSendWnd, 2), WM_LBUTTONUP, 0, 0);
					break;
				}
				else if (i % 25 == 0) {
					SendMessage(hUsaWnd, WM_COMMAND, 32785, 0);
					Sleep(100);
				}
			}
		}
	}
	else if (msg == WM_KEYDOWN && GetKeyState(VK_CONTROL) & 0x80 && (wp == 'N' || wp == 'E' || wp == 'S')) {
		if (wp == 'N') {
			SendMessage(GetParent(hWnd), WM_COMMAND, IDM_LIST_TYPE_NORMAL, 0);
		}
		else if (wp == 'E') {
			SendMessage(GetParent(hWnd), WM_COMMAND, IDM_LIST_TYPE_EX, 0);
		}
		else if (wp == 'S') {
			SendMessage(GetParent(hWnd), WM_COMMAND, IDM_LIST_TYPE_SP, 0);
		}
		return 0;
	}
	else if (msg == WM_CHAR) {//0.48��WM_CHAR�ɕύX
		if (wp >= 'a' && wp <= 'z' || wp >= 'A' && wp <= 'Z') {//NPCLIST��������
			_TCHAR szChar[2];
			_TCHAR szName[0x20];
			_stprintf_s(szChar, _T("%c"), toupper(wp));
			int nItemNow, nItemCount;
			//�A�C�e�������擾
			nItemCount = ListView_GetItemCount(hWnd);
			//���݂̑I���ʒu���擾
			nItemNow = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);
			//���ׂĂ̑I��������
			ListView_SetItemState(hWnd, -1, 0, LVIS_SELECTED);
			//�I���̈ʒu����szChar�Ŏn�܂�NPC���T�[�`
			LVITEM item;
			item.mask = LVIF_TEXT;
			item.iSubItem = 1;//��
			item.pszText = szName;//Name
			item.cchTextMax = sizeof(szName) / sizeof(_TCHAR);
			if (GetKeyState(VK_SHIFT) & 0x80) {//+Shift
				for (int i = nItemNow - 1; i >= 0; i--) {
					item.iItem = i;//�s
					ListView_GetItem(hWnd, &item);
					szName[sizeof(szName - 1)] = 0;
					if (_tcsncmp(szChar, szName, 1) == 0) {
						ListView_SetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
						ListView_EnsureVisible(hWnd, i, FALSE);
						break;
					}
				}
			}
			else {
				for (int i = nItemNow + 1; i<nItemCount; i++) {
					item.iItem = i;//�s
					ListView_GetItem(hWnd, &item);
					szName[sizeof(szName - 1)] = 0;
					if (_tcsncmp(szChar, szName, 1) == 0) {
						ListView_SetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
						ListView_EnsureVisible(hWnd, i, FALSE);
						break;
					}
				}
			}

			return 0;
		}
	}
	else if (msg == WM_LBUTTONDBLCLK) {
		int nItem = -1;
		nItem = ListView_GetNextItem(hWnd, nItem, LVNI_SELECTED);
		if (ListView_GetCheckState(hWnd, nItem) != 0) {
			ListView_SetCheckState(hWnd, nItem, 0);//���Z�b�g
		}
		else {
			ListView_SetCheckState(hWnd, nItem, 1);//�Z�b�g
		}
		return 0;
	}

	return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
}
