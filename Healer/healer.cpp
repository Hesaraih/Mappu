#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING__stprintfS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//���j�R�[�h�ŃR���p�C��
#ifndef _UNICODE
#define _UNICODE
#endif

//swprintf�y��_stprintf��_stprintf��0.34�ŕύX

/*ToDo
*/
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC//0.40�Œǉ�
#include <stdlib.h>
#include <crtdbg.h>
#define UPLOAD
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <commctrl.h>
#include <locale.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <math.h>
#include <Mmsystem.h>

#include "memorymap.h"
#include "process.h"
#include "cmd.h"
#include "resource.h"
#include "WindowerHelper.h"
#include "searchaddr.h"//0.36�Œǉ�

void* (__stdcall *DLL_CreateKeyboardHelper)(char*);//0.1.1.0�Œǉ�
void(__stdcall *DLL_DeleteKeyboardHelper)(void*);//0.1.1.0�Œǉ�
void(__stdcall *DLL_CKHSetKey)(void*, unsigned char, bool);//0.1.1.0�Œǉ�


														   //0.02�Œǉ�
#include <uxtheme.h>//Theme�n�֐��ɕK�v
#include <vssym32.h>//DrawTheme�n�֐��Ŏg�p����񋓌^������`����Ă���
#pragma comment(lib,"uxtheme.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"version.lib")//0.14�Œǉ�


#if defined(UPLOAD) || defined(_DEBUG)
#define HAKOSIRO	_T("���Y���� - UPLOAD��")
#else
#define HAKOSIRO	_T("���Y����")
#endif

#define EVENT_NAME	_T("ADD_QUEUE")

#define TIMER_ID	1000
#define TIMER_ID_LOG	1010
#define TIMER_ID_FOLLOW	1020
#define TIMER_ID_TARGET	1030
#define TIMER_ID_RESERVED	1040
#define TIMER_ID_COMBAT	1050
#define TIMER_ID_FELLOW	1060//0.34�Œǉ�
#define	ID_STATUS	1001

#define WM_SETTING_DONE	(WM_USER + 1)
#define WM_CHANGE_HEADER_NAME	(WM_USER + 2)//0.41�Œǉ�

														   //0.22�ŕύX
#define COLUMN_COUNT	14
#define COLUMN_NAME	0
#define COLUMN_HASTE	1
#define COLUMN_REFRESH	2
#define COLUMN_USER	3
														   //0.22�ŕύX
#define COLUMN_USER2	4
#define COLUMN_USER3	5
#define COLUMN_HP	6
#define COLUMN_HPP	7
#define COLUMN_MP	8
#define COLUMN_MPP	9
#define COLUMN_TP	10
#define COLUMN_PRIORITY	11
#define COLUMN_DISTANCE	12
#define COLUMN_MAXHP	13

#define COLUMN_ATTR	0
#define COLUMN_TIME	1
#define COLUMN_LOG	2
#define COLUMN_RECASTNO	3

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcLog(HWND, UINT, WPARAM, LPARAM);//0.10�Œǉ�
LRESULT CALLBACK WndProcSubClass(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcFollow(HWND, UINT, WPARAM, LPARAM);//0.10�Œǉ�
LRESULT CALLBACK WndProcSetting(HWND, UINT, WPARAM, LPARAM);//0.13�Œǉ�
static WNDPROC WndProcMainClass;//�T�u�N���X����WNDPROC
LRESULT CALLBACK WndProcReserved(HWND, UINT, WPARAM, LPARAM);//0.12�Œǉ�
LRESULT CALLBACK WndProcTarget(HWND, UINT, WPARAM, LPARAM);//0.17�Œǉ�
LRESULT CALLBACK WndProcCombat(HWND, UINT, WPARAM, LPARAM);//0.28�Œǉ�
LRESULT CALLBACK WndProcSubClass_Combat(HWND, UINT, WPARAM, LPARAM);//0.29�Œǉ�
LRESULT CALLBACK WndProcFellow(HWND, UINT, WPARAM, LPARAM);//0.34�Œǉ�

void DrawItem(LPDRAWITEMSTRUCT);//0.02�Œǉ�
void MeasureItem(LPMEASUREITEMSTRUCT);//0.02�Œǉ�
int UpdateList(HWND, int);
short GetRecast(DWORD);
int Casting();//0.10�Œǉ�
int SearchRecastNo(int);//���C���p
int SearchRecastNo_Log(int);//Log�p
PARTYINFORMATION GetPartyInfo(int);
PARTYINFORMATION GetNPCInfo(int);
int ReadINI(HWND, LPCTSTR);
int MakeComboFromSection(HWND);
int UpdateList_Reserved(HWND, int);//0.12�Œǉ�
PARTYINFORMATION GetCharInfo(_TCHAR*);//0.12�Œǉ�
int exec_queue();//0.13�Œǉ�
int PutCmdQueue(HWND);//0.13�Œǉ�
int UpdateTarget(HWND);//0.17�Œǉ�
WORD GetTargetID();//0.17�Œǉ�
WORD GetArea2ByteID();//0.21�Œǉ�
int MakeList(HWND);//0.28�Œǉ�
int GetList(int, _TCHAR*, _TCHAR*);//0.28�Œǉ�
WORD GetActivityInfo();//0.28�Œǉ�
int CombatLoop(HWND, CKeyboardHelper*, INPUTKEYSTATE*);//0.28�Œǉ�
int AutoCombat(HWND, CKeyboardHelper*, INPUTKEYSTATE*);//0.28�Œǉ�
int BackToCamp(HWND, CKeyboardHelper*, INPUTKEYSTATE*);//0.28�Œǉ�
int MoveToNPC(HWND, CKeyboardHelper*, INPUTKEYSTATE*);//0.28�Œǉ�
int SetCamp(HWND);//0.28�Œǉ�
void Follow(_TCHAR*, int, HWND);//0.29�ŕύX
void Follow_Helper(_TCHAR*, int, CKeyboardHelper*, HWND);//0.29�ŕύX
BYTE GetLockOn();//0.29�Œǉ�
BYTE GetSubTarget();//0.30�Œǉ�
HWND MakeToolTips(HWND, LONG);//0.30�Œǉ�
void UpdateToolTips(HWND, HWND, LONG);//0.30�Œǉ�
int AutoCombatFellow(HWND hWnd, CKeyboardHelper*, INPUTKEYSTATE*, PARTYINFORMATION, int);//0.34�Œǉ�
int ListChangeHeaderWidth(HWND, int, int);//0.34�Œǉ�
int ListGetHeaderWidth(HWND, int);//0.34�Œǉ�
int GetFileName(HWND, _TCHAR*, int);//0.41�Œǉ�


volatile HANDLE g_polHandle = 0;//�v���C�I�����C���̃n���h��
volatile DWORD g_ProcessID = 0;//�Ď�����v���Z�XID
volatile DWORD g_DllAddress = 0;//FFXiMain.dll�̃A�h���X
volatile DWORD g_DllSize = 0;//FFXiMAin.dll�̃T�C�Y

FFXIOFFSET g_Offset;//0.36�ŕύX
int g_DefaultPriority;
BYTE g_DeltaHppPriority[6];//0.25�Œǉ�
double g_SpellDistance;
DWORD g_CastWaitTime;//0.14�Œǉ�
DWORD g_CmdInterval;
DWORD g_CmdWaitTime;
DWORD g_MoveWaitTime;
DWORD g_AreaWaitTime;//0.20�Œǉ�
DWORD g_CureWaitTime;//0.40�Œǉ�
DWORD g_StopTime;//0.40�Œǉ�
BYTE g_AreaStop;//0.42�Œǉ�
BYTE g_HinshiDeltaHp;//0.21�Œǉ�
_TCHAR g_DefaultFollower[0x20];//0.23�Œǉ�
int g_ColumnCx[COLUMN_COUNT];
int g_ColumnCxLog[4];
_TCHAR g_SectionName[20][0x20];//0.26�ŕύX
struct {//0.17�Œǉ�
	BYTE open;
	BYTE add;//0.18�Œǉ�
	_TCHAR exe[MAX_PATH];
	_TCHAR cmd[0x20];
	_TCHAR link[4][0x20];
	_TCHAR url[4][MAX_PATH];
}g_Link;
struct {
	_TCHAR szCmd[0x20];
	_TCHAR szPreJa[0x20];//0.31�Œǉ�
	_TCHAR szEquip[9][0x30];//0.19�Œǉ�
	_TCHAR szEquipAfter[9][0x30];//0.20�Œǉ�
	short recast_no;
	int threshold;
	int time;
	int mode;//0.32�Œǉ�
	DWORD mp;//0.40�ŕύX
}g_CureSetting[6], g_HasteSetting, g_RefreshSetting, g_UserSetting[3];//0.22�ŕύX

struct {
	int x;
	int y;
	int cx;
	int cy;
	int default_open;//0.15�Œǉ�
}g_Log, g_Follow, g_Reserved, g_Target, g_Target_Add, g_Combat, g_Fellow;//0.28��g_Combat�ǉ� 0.34��g_Fellow�ǉ�
int g_UseWindowerHelper = 0;
int g_SavePosition = 0;//0.15�Œǉ�
struct CMD_QUEUE {//0.13�Œǉ�
	CMD_QUEUE *nextpointer;
	_TCHAR szCmd[0x40];
}*cqueue;
_TCHAR g_szStopWav[MAX_PATH];//0.41�Œǉ�
_TCHAR g_szDefaultPIDName[0x20];//0.43�Œǉ�


#ifndef UPLOAD
int g_SkillUpMode = 0;//0.26�Œǉ�
#endif


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//���P�[����OS�f�t�H���g�ɐݒ�VC�̏ꍇ���ꂪ������IO�ŕ�����������
	setlocale(LC_ALL, "");//0.13b�Œǉ�

	if (strlen(lpCmdLine)) {//0.13�ŃR�}���h���C����ǉ�
		_TCHAR *szCmd;
		szCmd = (_TCHAR*)malloc((strlen(lpCmdLine) + 1) * sizeof(_TCHAR));

		mbstowcs(szCmd, lpCmdLine, strlen(lpCmdLine) + 1);

		HWND hTargetWnd = FindWindow(NULL, HAKOSIRO);
		if (hTargetWnd != NULL) {
			COPYDATASTRUCT cp;
			ZeroMemory(&cp, sizeof(COPYDATASTRUCT));
			cp.lpData = szCmd;
			cp.cbData = (_tcslen(szCmd) + 1) * sizeof(_TCHAR);
#ifdef _DEBUG
			//MessageBox(NULL,szCmd,_T("wcs"),MB_OK);
			//MessageBoxA(NULL,lpCmdLine,"mbs",MB_OK);
#endif
			SendMessage(hTargetWnd, WM_COPYDATA, NULL, (LPARAM)&cp);
		}

		free(szCmd);
	}
	else {
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)WndProc);
	}

	return true;
}



//�E�B���h�E�v���V�[�W��IDD_DIALOG
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static int nStart = 0;
	static HWND hEdit = 0;
	static HWND hLog;
	static HWND hFollow;
	static HWND hReserved;//0.12�Œǉ�
	static HWND hSetting;//0.13�Œǉ�
	static HWND hTarget;//0.17�Œǉ�
	static HWND hCombat;//0.28�Œǉ�
	static HWND hTool;//0.30�Œǉ�
	static HWND hFellow;//0.34�Œǉ�
	static HMENU hMenu, hSubMenu;
	static HANDLE hEvent;
	_TCHAR szPos[0x10];//0.15�Œǉ�
	static _TCHAR szFullPathName[MAX_PATH];//0.15�Œǉ�
	static WORD stwAreaID = 0;//0.42�Œǉ�

	static DWORD dwStartTime = -1;

	switch (msg) {
	case WM_INITDIALOG: {
		//���P�[����OS�f�t�H���g�ɐݒ� VC�̏ꍇ���ꂪ������IO�ŕ�����������
		_tsetlocale(LC_ALL, _T(""));
		//ini�ǂݍ���
		if (ReadINI(hWnd, _T("")) != 0) {
			MessageBox(hWnd, _T("Healer.ini �G���["), _T("Error"), MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		//���X�g�r���[�̏�����
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&InitCtrls);
		//���X�g���T�u�N���X��
		WndProcMainClass = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDC_LIST_PC), GWL_WNDPROC, (LONG)WndProcSubClass);
		//���X�g�r���[��EX�X�^�C���t�^
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_PC),
			LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
			LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
		);
		//�J�����쐬
		LVCOLUMN col;
		ZeroMemory(&col, sizeof(col));

		//�J�����쐬
		const _TCHAR szColumn[][0x10] = { _T("���O"),_T("Haste"),_T("Refresh"),_T("User"),_T("User2"),_T("User3"),_T("HP"),_T("HP%"),_T("MP"),_T("MP%"),_T("TP"),_T("�D��x"),_T("����") };//0.22�ŕύX
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
		col.fmt = LVCFMT_LEFT;
		for (int i = 0; i < sizeof(szColumn) / sizeof(szColumn[0]); i++) {//0.15�ŕύX
			col.cx = g_ColumnCx[i];
			col.fmt = LVCFMT_RIGHT;
			col.iSubItem = i;
			col.pszText = (_TCHAR*)szColumn[i];
			if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_PC), i, &col)) {
				return 0;
			}
		}
		//POLID�������ݒ�
		g_DllAddress = GetDllModuleAddress(1);//0.37�ŕύX
		if (g_Offset.Auto) {//0.36a�ŕύX
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				MessageBox(hWnd, _T("�I�t�Z�b�g�擾�Ɏ��s"), NULL, MB_ICONERROR);
			}
		}
		//PID���X�g��ݒ�
		MakeListPID(GetDlgItem(hWnd, IDC_COMBO_PID));

		//PID�̏����I�� 0.43�Œǉ�
		SetInitPID(GetDlgItem(hWnd, IDC_COMBO_PID), g_szDefaultPIDName);

		if (ffhook_init(g_ProcessID) != 0) {
			MessageBox(hWnd, _T("ffhook_init"), _T("error"), MB_OK | MB_ICONERROR);
		}
		//Section�ݒ�
		MakeComboFromSection(hWnd);
		//��ԏ��Ǎ��� 0.33�Œǉ�
		SendMessage(hWnd, WM_COMMAND, IDC_COMBO_JOB | CBN_SELCHANGE << 16, 0);
		//�A�C�R���̐ݒ�
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED));
		//���j���[�쐬
		hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_DIALOG));
		hSubMenu = GetSubMenu(hMenu, 0);
		//�^�C�}�[�Z�b�g
		SetTimer(hWnd, TIMER_ID, 1000, NULL);
		SetWindowText(hWnd, HAKOSIRO);
		cqueue = (CMD_QUEUE*)malloc(sizeof(CMD_QUEUE));
		ZeroMemory(cqueue, sizeof(CMD_QUEUE));
		//0.13b�Œǉ�
		hEvent = CreateEvent(NULL, TRUE, TRUE, EVENT_NAME);
		if (hEvent == NULL) {
			MessageBox(hWnd, _T("CreateEvent"), _T("error"), MB_OK | MB_ICONERROR);
		}
		GetFullPathName(_T("healer.ini"), sizeof(szFullPathName) / sizeof(_TCHAR) - 1, szFullPathName, NULL);
		//�N�����\�� 0.15�Œǉ�
		if (g_Log.default_open)SendMessage(hWnd, WM_COMMAND, IDM_LOG_OPEN, 0);//0.15�Œǉ�
		if (g_Follow.default_open)SendMessage(hWnd, WM_COMMAND, IDM_FOLLOW_OPEN, 0);//0.15�Œǉ�
		if (g_Reserved.default_open)SendMessage(hWnd, WM_COMMAND, IDM_RESERVED_OPEN, 0);//0.15�Œǉ�
		if (g_Target.default_open)SendMessage(hWnd, WM_COMMAND, IDM_TARGET_OPEN, 0);

		//�c�[���`�b�v
		hTool = MakeToolTips(hWnd, (LONG)IDC_LIST_PC);

#ifdef UPLOAD//0.28�ő啝�ύX
		ShowWindow(hWnd, SW_HIDE);
		//���j���[��Combat�����O���[��
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_DISABLED;
		SetMenuItemInfo(hSubMenu, IDM_COMBAT_OPEN, false, &mii);

		//���k�������I��
		_TCHAR szFileName[MAX_PATH];
		HANDLE hFile;
		GetModuleFileName(NULL, szFileName, sizeof(szFileName) / sizeof(_TCHAR) - 1);
		if (_tcslen(szFileName) <= 5) {
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		else {
			hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
#ifndef	_DEBUG
			else if (102400 <= GetFileSize(hFile, NULL)) {//100KB�ȏ�
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
#endif
		}
		ShowWindow(hWnd, SW_NORMAL);
#endif

		break; }

	case WM_TIMER:
		DWORD time;//0.13c�Œǉ�
		KillTimer(hWnd, TIMER_ID);
		if (UpdateList(hWnd, nStart) == 1) {
			//time = 400;
			time = g_CmdWaitTime;//0.14�ŕύX
		}
		else time = 200;

		if (!IsWindow(hReserved)) {
			int ret;//0.38�ŕύX
			ret = PutCmdQueue(hWnd);
			if (ret == 2 && nStart == 0) {//�J�n
				SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, NULL);
			}
			else if (ret == 3 && nStart == 1) {
				SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, NULL);
			}
		}

		DWORD dwStopTimer;
		dwStopTimer = 0;

		if (nStart == 1 && g_StopTime && (int)(dwStartTime - timeGetTime()) < -(int)g_StopTime * 60 * 1000) {//0.40�ŕύX
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 0);
			PlaySound(g_szStopWav, NULL, SND_ASYNC | SND_FILENAME);//0.41�ŕύX
		}
		else if (nStart == 1 && g_StopTime) {
			DWORD dwStopTime = (DWORD)g_StopTime * 60 * 1000;
			DWORD dwTimer = dwStopTime - (timeGetTime() - dwStartTime);
			_TCHAR szTimer[0x20];
			if (nStart == 3)_stprintf(szTimer, _T("�ꎞ��~��(%01d��)"), dwTimer / 60 / 1000);//0.38�Œǉ�
			else _stprintf(szTimer, _T("��~(%01d��)"), dwTimer / 60 / 1000);
			SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_START), szTimer);
		}

		//�G���A�X�g�b�v 0.42�Œǉ�
		if (nStart == 1 && g_AreaStop && stwAreaID != GetArea2ByteID()) {
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 0);
			PlaySound(g_szStopWav, NULL, SND_ASYNC | SND_FILENAME);
		}
		else {
			stwAreaID = GetArea2ByteID();
		}

		SetTimer(hWnd, TIMER_ID, time, NULL);//0.13c�ŕύX
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_COMBO_PID:
			if (HIWORD(wp) == CBN_DROPDOWN) {//�h���b�v�_�E��������PID�ꗗ���X�V����
				if (MakeListPID(GetDlgItem(hWnd, IDC_COMBO_PID)) == 0) {
					//MessageBox(hWnd,_T("FF11���N�����ĂȂ��H"),PROJECT_VER,MB_ICONERROR|MB_OK);
				}
			}
			else if (HIWORD(wp) == CBN_SELCHANGE) {
				g_DllAddress = GetDllModuleAddress(SendMessage(GetDlgItem(hWnd, IDC_COMBO_PID), CB_GETCURSEL, 0, 0) + 1);
				if (g_DllAddress) {//0.16�Œǉ�
					WINDOWPLACEMENT wndPlace;//0.34�ŕύX
					wndPlace.length = sizeof(WINDOWPLACEMENT);

					int nLog = IsWindow(hLog);
					int nFollow = IsWindow(hFollow);
					int nReserve = IsWindow(hReserved);
					int nTarget = IsWindow(hTarget);
					int nCombat = IsWindow(hCombat);//0.28�Œǉ�
					int nFellow = IsWindow(hFellow);//0.34�Œǉ�

					if (nLog) {
						GetWindowPlacement(hLog, &wndPlace);//0.34�ŕύX
						g_Log.x = wndPlace.rcNormalPosition.left;
						g_Log.y = wndPlace.rcNormalPosition.top;
						g_Log.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
						g_Log.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_LOG_OPEN, 1);
					}
					if (nFollow) {
						GetWindowPlacement(hFollow, &wndPlace);//0.34�ŕύX
						g_Follow.x = wndPlace.rcNormalPosition.left;
						g_Follow.y = wndPlace.rcNormalPosition.top;
						g_Follow.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
						g_Follow.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_FOLLOW_OPEN, 1);
					}
					if (nReserve) {
						GetWindowPlacement(hReserved, &wndPlace);//0.34�ŕύX
						g_Reserved.x = wndPlace.rcNormalPosition.left;
						g_Reserved.y = wndPlace.rcNormalPosition.top;
						g_Reserved.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
						g_Reserved.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_RESERVED_OPEN, 1);
					}
					if (nTarget) {//0.17�Œǉ�
						GetWindowPlacement(hTarget, &wndPlace);//0.34�ŕύX
						g_Target.x = wndPlace.rcNormalPosition.left;
						g_Target.y = wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_TARGET_OPEN, 1);
					}
					if (nCombat) {//0.28�Œǉ�
						GetWindowPlacement(hCombat, &wndPlace);//0.34�ŕύX
						g_Combat.x = wndPlace.rcNormalPosition.left;
						g_Combat.y = wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_COMBAT_OPEN, 1);
					}
					if (nFellow) {//0.34�Œǉ�
						GetWindowPlacement(hFellow, &wndPlace);
						g_Fellow.x = wndPlace.rcNormalPosition.left;
						g_Fellow.y = wndPlace.rcNormalPosition.top;
						SendMessage(hWnd, WM_COMMAND, IDM_FELLOW_OPEN, 1);
					}

					if (ffhook_end() == 0 && ffhook_init(g_ProcessID) != 0) {//0.1.1.1�ŏC��
						MessageBox(hWnd, _T("ffhook_init"), _T("error"), MB_OK | MB_ICONERROR);
					}
					else {
						if (nLog)SendMessage(hWnd, WM_COMMAND, IDM_LOG_OPEN, 0);
						if (nFollow)SendMessage(hWnd, WM_COMMAND, IDM_FOLLOW_OPEN, 0);
						if (nReserve) {
							SendMessage(hWnd, WM_COMMAND, IDM_RESERVED_OPEN, 0);
							DWORD dwPos = (USHORT)g_Reserved.x | (((USHORT)g_Reserved.y) << 16);
							SendMessage(hReserved, WM_MOVE, dwPos, NULL);
						}
						if (nTarget)SendMessage(hWnd, WM_COMMAND, IDM_TARGET_OPEN, 0);//0.17�Œǉ�
						if (nCombat)SendMessage(hWnd, WM_COMMAND, IDM_COMBAT_OPEN, 0);//0.28�Œǉ�
						if (nFellow)SendMessage(hWnd, WM_COMMAND, IDM_FELLOW_OPEN, 0);//0.34�Œǉ�
					}

					if (g_Offset.Auto) {//0.36a�ŕύX
						if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
							MessageBox(hWnd, _T("�I�t�Z�b�g�擾�Ɏ��s"), NULL, MB_ICONERROR);
						}
					}
				}
			}
			break;

		case IDC_COMBO_JOB:
			if (HIWORD(wp) == CBN_SELCHANGE) {
				_TCHAR szText[0x20];
				ZeroMemory(szText, sizeof(szText));
				int nCurSel = SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_GETCURSEL, 0, 0);
				SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_GETLBTEXT, nCurSel, (LPARAM)szText);
				if (ReadINI(hWnd, szText) != 0) {
					MessageBox(hWnd, _T("Healer.ini �G���["), _T("Error"), MB_OK | MB_ICONERROR);
					SendMessage(hWnd, WM_DESTROY, 0, 0);
				}
				UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_PC);//0.30�Œǉ�
				if (IsWindow(hReserved)) {
					//�c�[���`�b�v�X�V�p 0.30�Œǉ�
					SendMessage(hReserved, WM_SIZE, 0, 0);
				}
			}
			else if (HIWORD(wp) == CBN_DROPDOWN) {
				//�h���b�v�_�E��������SECTION�ꗗ���X�V���� 0.26�Œǉ�
				if (ReadINI(hWnd, _T("SECTION")) != 0) {
					MessageBox(hWnd, _T("Healer.ini �G���["), _T("Error"), MB_OK | MB_ICONERROR);
					SendMessage(hWnd, WM_DESTROY, 0, 0);
				}
				MakeComboFromSection(hWnd);
			}
			break;

		case IDC_BUTTON_START:
			if (nStart == 0) {
				if (lp == 2) {//2��stop 0.38�Œǉ�
					break;
				}

				//10���ŏI��������
				dwStartTime = timeGetTime();

				nStart = 1;
				SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_START), _T("��~"));
			}
			else {
				if (lp == 1) {//1��run 0.38�Œǉ�
					break;
				}
				nStart = 0;
				SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_START), _T("�J�n"));
			}
			break;

		case IDM_LOG_OPEN://0.10�Œǉ�
			if (lp == 1) {
				WINDOWPLACEMENT wndPlace;//0.34�ŕύX
				wndPlace.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement(hLog, &wndPlace)) {
					g_Log.x = wndPlace.rcNormalPosition.left;
					g_Log.y = wndPlace.rcNormalPosition.top;
					g_Log.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
					g_Log.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
				}

				EndDialog(hLog, 0);//0.15�Œǉ�
				DestroyWindow(hLog);
			}
			else {
				if (IsWindow(hLog) == 0) {
					hLog = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_LOG), hWnd, (DLGPROC)WndProcLog);
				}
			}
			break;

		case IDM_FOLLOW_OPEN://0.10�Œǉ�
			if (lp == 1) {
				WINDOWPLACEMENT wndPlace;//0.34�ŕύX
				wndPlace.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement(hFollow, &wndPlace)) {
					g_Follow.x = wndPlace.rcNormalPosition.left;
					g_Follow.y = wndPlace.rcNormalPosition.top;
					g_Follow.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
					g_Follow.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
				}
				EndDialog(hFollow, 0);//0.15�Œǉ�
				DestroyWindow(hFollow);
			}
			else {
				if (IsWindow(hFollow) == 0) {
					if (IsWindow(hCombat)) {
						MessageBox(hWnd, _T("���̋@�\��Combat���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else if (IsWindow(hFellow)) {//0.34�Œǉ�
						MessageBox(hWnd, _T("���̋@�\��Fellow���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else {
						hFollow = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_FOLLOW), hWnd, (DLGPROC)WndProcFollow);
					}
				}
			}
			break;

		case IDM_RESERVED_OPEN://0.12�Œǉ�
			if (lp == 1) {
				EndDialog(hReserved, 0);//0.15�Œǉ�
				DestroyWindow(hReserved);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), true);
			}
			else {
				if (nStart == 1) {
					MessageBox(hWnd, _T("���̋@�\�͓����ɗ��p�ł��܂���\n��~���Ă��炲���p��������"), NULL, MB_ICONERROR);
				}
				else if (IsWindow(hReserved) == 0) {
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), false);
					hReserved = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_RESERVED), hWnd, (DLGPROC)WndProcReserved);
				}
			}
			break;

		case IDM_TARGET_OPEN://0.17�Œǉ�
			if (lp == 1) {
				WINDOWPLACEMENT wndPlace;//0.34�ŕύX
				wndPlace.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement(hTarget, &wndPlace)) {
					g_Target.x = wndPlace.rcNormalPosition.left;
					g_Target.y = wndPlace.rcNormalPosition.top;
					g_Target.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
					g_Target.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
				}
				EndDialog(hTarget, 0);//0.15�Œǉ�
				DestroyWindow(hTarget);
			}
			else {
				if (IsWindow(hTarget) == 0) {
					hTarget = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_TARGET), hWnd, (DLGPROC)WndProcTarget);
				}
			}
			break;

		case IDM_COMBAT_OPEN://0.28�Œǉ�
			if (lp == 1) {
				WINDOWPLACEMENT wndPlace;//0.34�ŕύX
				wndPlace.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement(hCombat, &wndPlace)) {
					g_Combat.x = wndPlace.rcNormalPosition.left;
					g_Combat.y = wndPlace.rcNormalPosition.top;
					g_Combat.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
					g_Combat.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
				}
				EndDialog(hCombat, 0);
				DestroyWindow(hCombat);
			}
			else {
				if (IsWindow(hCombat) == 0) {
					if (IsWindow(hFollow)) {
						MessageBox(hWnd, _T("���̋@�\��Follow���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else if (IsWindow(hFellow)) {
						MessageBox(hWnd, _T("���̋@�\��Fellow���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else {
						hCombat = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_COMBAT), hWnd, (DLGPROC)WndProcCombat);
					}
				}
			}
			break;

		case IDM_FELLOW_OPEN://0.34�Œǉ�
			if (lp == 1) {
				WINDOWPLACEMENT wndPlace;//0.34�ŕύX
				wndPlace.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement(hFellow, &wndPlace)) {
					g_Fellow.x = wndPlace.rcNormalPosition.left;
					g_Fellow.y = wndPlace.rcNormalPosition.top;
					g_Fellow.cx = wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left;
					g_Fellow.cy = wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top;
				}
				EndDialog(hFellow, 0);
				DestroyWindow(hFellow);
			}
			else {
				if (IsWindow(hFellow) == 0) {
					if (IsWindow(hFollow)) {
						MessageBox(hWnd, _T("���̋@�\��Follow���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else if (IsWindow(hCombat)) {
						MessageBox(hWnd, _T("���̋@�\��Combat���Ɠ����ɗ��p�ł��܂���"), NULL, MB_ICONERROR);
					}
					else {
						hFellow = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_FELLOW), hWnd, (DLGPROC)WndProcFellow);
					}
				}
			}
			break;

		case IDM_SETTING_OPEN://0.13�Œǉ�
			if (lp == 1) {
				EndDialog(hSetting, 0);//0.15�Œǉ�
				DestroyWindow(hSetting);
			}
			else {
				if (IsWindow(hSetting) == 0) {
					hSetting = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING), hWnd, (DLGPROC)WndProcSetting);
				}
			}
			break;

		case IDM_EDIT_HEALER_INI://0.26�Œǉ�
			ShellExecute(NULL, _T("open"), _T("Healer.ini"), NULL, NULL, SW_SHOWDEFAULT);
			break;

		case IDM_EDIT_CHARACTER_INI://0.26�Œǉ�
			ShellExecute(NULL, _T("open"), _T("Character.ini"), NULL, NULL, SW_SHOWDEFAULT);
			break;

		case IDM_VERSION://0.14�Œǉ�
						 //�o�[�W�������\�[�X������擾
			_TCHAR szFileName[MAX_PATH];
			DWORD dwBlockSize;
			BYTE *pbyVersionInfo;

			if (GetModuleFileName(NULL, szFileName, sizeof(szFileName) / sizeof(_TCHAR) - 1) == 0) {
				MessageBox(hWnd, _T("Cant get VersionInfo"), HAKOSIRO, MB_ICONERROR);
			}
			else {
				dwBlockSize = GetFileVersionInfoSize(szFileName, 0);
				pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
				if (pbyVersionInfo) {
					if (GetFileVersionInfo(szFileName, 0, dwBlockSize, (LPVOID)pbyVersionInfo) == FALSE) {
						MessageBox(hWnd, _T("Cant get VersionInfo"), HAKOSIRO, MB_ICONERROR);
					}
					else {
						struct {
							WORD wLanguage;
							WORD wCodePage;
						} *lpTranslate;
						VerQueryValue((LPVOID)pbyVersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, NULL);
						_TCHAR szDir[0x100];
						_stprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate->wLanguage, lpTranslate->wCodePage);
						void *pvVer;
						VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvVer, NULL);
						_stprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\LegalCopyright"), lpTranslate->wLanguage, lpTranslate->wCodePage);
						void *pvCopyright;
						VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvCopyright, NULL);
						//0.43d�Œǉ�
						_stprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate->wLanguage, lpTranslate->wCodePage);
						void *pvComments;
						VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvComments, NULL);

						_TCHAR szVersion[0x100];
#ifdef _DEBUG
						_stprintf(szVersion, _T("Version %s(Debug)\n%s\n%s"), (TCHAR*)&pvVer, (TCHAR*)&pvComments, (TCHAR*)&pvCopyright);//0.43d�ŕύX
#else
#ifdef UPLOAD
						_stprintf(szVersion, _T("Version %s\n\n�]�ڋ֎~��Ĕz�z�֎~\n%s\n%s"), (TCHAR*)&pvVer, (TCHAR*)&pvComments, (TCHAR*)&pvCopyright);//0.43d�ŕύX
#else
						_stprintf(szVersion, _T("Version %s\n\n�]�ڋ֎~��Ĕz�z�֎~\n%s\n%s"), (TCHAR*)&pvVer, (TCHAR*)&pvComments, (TCHAR*)&pvCopyright);//0.43d�ŕύX
#endif
#endif
						MessageBox(hWnd, szVersion, HAKOSIRO, MB_OK | MB_ICONINFORMATION);
					}
				}
				free(pbyVersionInfo);
			}
			break;

		case IDM_END:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		default:
			break;
		}
		break;

		//0.13�Œǉ�
	case WM_COPYDATA://WM_SETTEXT/WM_COPYDATA�͗�O�I�ɕʃv���Z�X�ɕ�����𑗐M�ł���
		COPYDATASTRUCT * copydata;
		if (lp == NULL)return 0;
		copydata = (COPYDATASTRUCT*)lp;
		CMD_QUEUE *buf_que, *new_que;
		_TCHAR *szSrcCmd;
		szSrcCmd = (_TCHAR*)copydata->lpData;
		if (_tcslen(szSrcCmd) != 0 && *szSrcCmd == '/') {
			WaitForSingleObject(hEvent, 1000);//0.142b�ŕύX
			ResetEvent(hEvent);//0.13b�Œǉ�
			buf_que = cqueue;
			new_que = (CMD_QUEUE*)malloc(sizeof(CMD_QUEUE));
			new_que->nextpointer = NULL;
			wcscpy(new_que->szCmd, szSrcCmd);
			while (buf_que->nextpointer != NULL) {
				buf_que = buf_que->nextpointer;
				if (_tcscmp (buf_que->szCmd, new_que->szCmd) == 0) {//0.13b�Œǉ�
					SetEvent(hEvent);//0.13b�Œǉ�
					free(new_que);//0.40�Œǉ�
					return 0;
				}
			}
			buf_que->nextpointer = new_que;
			SetEvent(hEvent);//0.13b�Œǉ�
		}
		else if (_tcslen(szSrcCmd) != 0 && _tcscmp (szSrcCmd, _T("run")) == 0) {//0.38�Œǉ�
			if (!IsWindow(hReserved)) {
				SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 1);//1��run
			}
			else {
				SendMessage(hReserved, WM_COMMAND, IDC_BUTTON_RESERVED_START, 1);
			}
		}
		else if (_tcslen(szSrcCmd) != 0 && _tcscmp (szSrcCmd, _T("stop")) == 0) {//0.38�Œǉ�
			if (!IsWindow(hReserved)) {
				SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 2);//2��stop
			}
			else {
				SendMessage(hReserved, WM_COMMAND, IDC_BUTTON_RESERVED_START, 2);
			}
		}
		break;

	case WM_DRAWITEM:
		DrawItem((LPDRAWITEMSTRUCT)lp);
		break;

	case WM_MEASUREITEM:
		MeasureItem((LPMEASUREITEMSTRUCT)lp);
		break;

	case WM_MOUSEWHEEL:
		//�}�E�X�z�C�[������
		if (IsWindow(hReserved)) {
			break;//0.12�Œǉ�
		}
		if ((short)HIWORD(wp) > 0) {//�J�n
			nStart = 0;//�J�n���邽�߂̃t���O
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 0);
		}
		else if ((short)HIWORD(wp) < 0) {//��~
			nStart = 1;//��~���邽�߂̃t���O
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_START, 0);
		}
		break;

	case WM_RBUTTONUP:
		//���j���[��\��
		POINT pos;
		pos.x = LOWORD(lp);
		pos.y = HIWORD(lp);
		if (0 == ClientToScreen(hWnd, &pos)) {
		}
		SetForegroundWindow(hWnd);
		if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
		}
		PostMessage(hWnd, 0, 0, 0);
		break;

	case WM_NOTIFY://�J�������ύX�ɂ��c�[���`�b�v�X�V 0.30�Œǉ�
		switch (((NMHDR*)lp)->code) {//0.34�ŕύX
		case HDN_ENDTRACK:
		case HDN_DIVIDERDBLCLICK://�w�b�_�[�_�u���N���b�N
			g_ColumnCx[((NMHEADER*)lp)->iItem] = ListGetHeaderWidth(((LPNMHDR)lp)->hwndFrom, ((NMHEADER*)lp)->iItem);
			ListChangeHeaderWidth(hWnd, ((NMHEADER*)lp)->iItem, g_ColumnCx[((NMHEADER*)lp)->iItem]);
			UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_PC);
			if (IsWindow(hReserved)) {
				ListChangeHeaderWidth(hReserved, ((NMHEADER*)lp)->iItem, g_ColumnCx[((NMHEADER*)lp)->iItem]);
				SendMessage(hReserved, TTM_NEWTOOLRECT, 0, 0);
			}
			break;
		}
		break;

	case TTM_NEWTOOLRECT://�c�[���`�b�v�̕����Ē���(hReserved���甭�s�����) 0.34�Œǉ�
		UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_PC);
		break;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		//�T�C�Y�ύX�����������A�e�R���g���[���̑傫����ύX
		MoveWindow(GetDlgItem(hWnd, IDC_LIST_PC), 1, 1, rect.right - 1 * 2, rect.bottom - 1 * 3 - 20, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_COMBO_PID), 1, rect.bottom - 21, 78, 100, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_COMBO_JOB), 80, rect.bottom - 21, 120, 100, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), 201, rect.bottom - 21, 30, 100, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_START), 232, rect.bottom - 21, rect.right - 232, 20, 1);
		UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_PC);//�c�[���`�b�v���X�V 0.30�Œǉ�
		break;

	case WM_CHANGE_HEADER_NAME://0.41�Œǉ�
		if (_tcslen((_TCHAR*)wp) != 0) {
			LVCOLUMN col;
			ZeroMemory(&col, sizeof(col));
			col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
			col.iSubItem = (int)lp;
			col.pszText = (_TCHAR*)wp;
			ListView_SetColumn(GetDlgItem(hWnd, IDC_LIST_PC), col.iSubItem, &col);
			if (IsWindow(hReserved))ListView_SetColumn(GetDlgItem(hReserved, IDC_LIST_RESERVED), col.iSubItem, &col);
		}
		break;

	case WM_CLOSE:
		if (hEvent)CloseHandle(hEvent);
		SendMessage(hWnd, WM_COMMAND, IDM_LOG_OPEN, 1);//0.15�ŕύX
		SendMessage(hWnd, WM_COMMAND, IDM_FOLLOW_OPEN, 1);//0.15�Œǉ�
		SendMessage(hWnd, WM_COMMAND, IDM_RESERVED_OPEN, 1);//0.15�Œǉ�
		SendMessage(hWnd, WM_COMMAND, IDM_SETTING_OPEN, 1);//0.15�Œǉ�
		if (IsWindow(hCombat))SendMessage(hCombat, WM_CLOSE, 0, 0);//0.34�Œǉ�
		if (IsWindow(hFellow))SendMessage(hFellow, WM_CLOSE, 0, 0);//0.34�Œǉ�
		if (IsWindow(hTarget))SendMessage(hTarget, WM_CLOSE, NULL, NULL);//0.20�ŕύX �ʒu�ۑ��̊֌W�Ń^�[�Q�b�g���ɏI�������𓊂���
		CMD_QUEUE *next_que, qbuf;
		next_que = cqueue;
		while (1) {
			qbuf = *next_que;
			free(next_que);
			if (qbuf.nextpointer != NULL) {
				next_que = qbuf.nextpointer;
			}
			else break;
		}
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;

	case WM_DESTROY:
		if (g_SavePosition) {//0.15�Œǉ�
			WINDOWPLACEMENT wndPlace;//0.34�ŕύX
			wndPlace.length = sizeof(WINDOWPLACEMENT);
			if (GetWindowPlacement(hWnd, &wndPlace)) {
				_stprintf(szPos, _T("%d"), wndPlace.rcNormalPosition.left);
				WritePrivateProfileString(_T("INIT"), _T("X"), szPos, szFullPathName);
				_stprintf(szPos, _T("%d"), wndPlace.rcNormalPosition.top);
				WritePrivateProfileString(_T("INIT"), _T("Y"), szPos, szFullPathName);
				_stprintf(szPos, _T("%d"), wndPlace.rcNormalPosition.right - wndPlace.rcNormalPosition.left);
				WritePrivateProfileString(_T("INIT"), _T("SIZEX"), szPos, szFullPathName);
				_stprintf(szPos, _T("%d"), wndPlace.rcNormalPosition.bottom - wndPlace.rcNormalPosition.top);
				WritePrivateProfileString(_T("INIT"), _T("SIZEY"), szPos, szFullPathName);
			}
			//0.34�ŏI�����Ɉړ�
			_stprintf(szPos, _T("%d"), g_Log.x);
			WritePrivateProfileString(_T("INIT"), _T("LOG_X"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Log.y);
			WritePrivateProfileString(_T("INIT"), _T("LOG_Y"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Log.cx);
			WritePrivateProfileString(_T("INIT"), _T("LOG_SIZEX"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Log.cy);
			WritePrivateProfileString(_T("INIT"), _T("LOG_SIZEY"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Follow.x);
			WritePrivateProfileString(_T("INIT"), _T("FOLLOW_X"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Follow.y);
			WritePrivateProfileString(_T("INIT"), _T("FOLLOW_Y"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Target.x);
			WritePrivateProfileString(_T("INIT"), _T("TARGET_X"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Target.y);
			WritePrivateProfileString(_T("INIT"), _T("TARGET_Y"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Combat.x);
			WritePrivateProfileString(_T("INIT"), _T("COMBAT_X"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Combat.y);
			WritePrivateProfileString(_T("INIT"), _T("COMBAT_Y"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Fellow.x);
			WritePrivateProfileString(_T("INIT"), _T("FELLOW_X"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_Fellow.y);
			WritePrivateProfileString(_T("INIT"), _T("FELLOW_Y"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[0]);
			WritePrivateProfileString(_T("COLUMN"), _T("NAME"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[1]);
			WritePrivateProfileString(_T("COLUMN"), _T("HASTE"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[2]);
			WritePrivateProfileString(_T("COLUMN"), _T("REFRESH"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[3]);
			WritePrivateProfileString(_T("COLUMN"), _T("USER"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[4]);
			WritePrivateProfileString(_T("COLUMN"), _T("USER2"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[5]);
			WritePrivateProfileString(_T("COLUMN"), _T("USER3"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[6]);
			WritePrivateProfileString(_T("COLUMN"), _T("HP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[7]);
			WritePrivateProfileString(_T("COLUMN"), _T("HPP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[8]);
			WritePrivateProfileString(_T("COLUMN"), _T("MP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[9]);
			WritePrivateProfileString(_T("COLUMN"), _T("MPP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[10]);
			WritePrivateProfileString(_T("COLUMN"), _T("TP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[11]);
			WritePrivateProfileString(_T("COLUMN"), _T("PRIORITY"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[12]);
			WritePrivateProfileString(_T("COLUMN"), _T("DISTANCE"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCx[13]);
			WritePrivateProfileString(_T("COLUMN"), _T("MAXHP"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCxLog[0]);
			WritePrivateProfileString(_T("COLUMN"), _T("ATTR"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCxLog[1]);
			WritePrivateProfileString(_T("COLUMN"), _T("TIME"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCxLog[2]);
			WritePrivateProfileString(_T("COLUMN"), _T("LOG"), szPos, szFullPathName);
			_stprintf(szPos, _T("%d"), g_ColumnCxLog[3]);
			WritePrivateProfileString(_T("COLUMN"), _T("RECASTNO"), szPos, szFullPathName);

			g_SavePosition = 0;//WM_DESTROY�͉��x�������Ă���ۂ� 0.38�Œǉ�
		}
		EndDialog(hWnd, 0);
		DestroyWindow(hWnd);
		break;

	default:
		break;
	}

	return 0;
}

//�c�[���`�b�v�쐬 0.30�Œǉ�
HWND MakeToolTips(HWND hWnd, LONG ID)
{
	TOOLINFO ti;
	HDITEM hdi;
	LONG lWidth = 0;
	HWND hTool;
	_TCHAR szText[MAX_PATH];//0.31�Œǉ�
	_TCHAR szBuf[MAX_PATH];//0.32�Œǉ�

	hTool = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, ListView_GetHeader(GetDlgItem(hWnd, ID)), NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

	SendMessage(hTool, TTM_SETMAXTIPWIDTH, 0, 250);//���s������ׂɃe�L�X�g����ݒ� 0.31�Œǉ�

	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = ListView_GetHeader(GetDlgItem(hWnd, ID));
	hdi.mask = HDI_WIDTH;
	Header_GetItem(ListView_GetHeader(GetDlgItem(hWnd, ID)), 0, &hdi);
	GetClientRect(ListView_GetHeader(GetDlgItem(hWnd, ID)), &ti.rect);
	for (int i = 1; i <= 5; i++) {
		ZeroMemory(szBuf, sizeof(szBuf));//0.32�Œǉ�
		ti.uId = i;
		lWidth += hdi.cxy;
		ti.rect.left = lWidth;
		Header_GetItem(ListView_GetHeader(GetDlgItem(hWnd, ID)), i, &hdi);
		ti.rect.right = lWidth + hdi.cxy;
		_stprintf(ti.lpszText, _T("-"));
		if (i == 1 && _tcslen(g_HasteSetting.szCmd)) {//0.32�ŕύX
			if (g_HasteSetting.mode == 0x01) {//0.33�ŕύX
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_HasteSetting.szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_HasteSetting.szPreJa, g_HasteSetting.szCmd);
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_HasteSetting.szCmd);
				ti.lpszText = szText;
			}
		}
		else if (i == 2 && _tcslen(g_RefreshSetting.szCmd)) {//0.32�ŕύX
			if (g_RefreshSetting.mode == 0x01) {//0.33�ŕύX
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_RefreshSetting.szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_RefreshSetting.szPreJa, g_RefreshSetting.szCmd);
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_RefreshSetting.szCmd);
				ti.lpszText = szText;
			}
		}
		else if (i == 3 && _tcslen(g_UserSetting[0].szCmd)) {//0.31�ŕύX
			if (g_UserSetting[0].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[0].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[0].szPreJa, g_UserSetting[0].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[0].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}
		else if (i == 4 && _tcslen(g_UserSetting[1].szCmd)) {
			if (g_UserSetting[1].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[1].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[1].szPreJa, g_UserSetting[1].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[1].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}
		else if (i == 5 && _tcslen(g_UserSetting[2].szCmd)) {
			if (g_UserSetting[2].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[2].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[2].szPreJa, g_UserSetting[2].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[2].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}
		SendMessage(hTool, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}

	return hTool;
}

//�c�[���`�b�v�X�V 0.30�Œǉ�
void UpdateToolTips(HWND hWnd, HWND hTool, LONG ID)
{
	TOOLINFO ti;
	HDITEM hdi;
	LONG lWidth = 0;
	_TCHAR szText[MAX_PATH];
	_TCHAR szBuf[MAX_PATH];//0.32�Œǉ�

	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = ListView_GetHeader(GetDlgItem(hWnd, ID));
	hdi.mask = HDI_WIDTH;
	Header_GetItem(ListView_GetHeader(GetDlgItem(hWnd, ID)), 0, &hdi);
	GetClientRect(ListView_GetHeader(GetDlgItem(hWnd, ID)), &ti.rect);
	for (int i = 1; i <= 5; i++) {
		ZeroMemory(szBuf, sizeof(szBuf));//0.32�Œǉ�
		ti.uId = i;
		lWidth += hdi.cxy;
		ti.rect.left = lWidth;
		Header_GetItem(ListView_GetHeader(GetDlgItem(hWnd, ID)), i, &hdi);
		ti.rect.right = lWidth + hdi.cxy;
		_stprintf(ti.lpszText, _T("-"));
		if (i == 1 && _tcslen(g_HasteSetting.szCmd)) {//0.32�ŕύX
			if (g_HasteSetting.mode == 0x01) {
				_stprintf(szText, _T("�Ώۂ��퓬���̎��������s\n%s"), g_HasteSetting.szCmd);
			}
			else {
				_stprintf(szText, _T("%s"), g_HasteSetting.szCmd);
			}
			ti.lpszText = szText;
		}
		else if (i == 2 && _tcslen(g_RefreshSetting.szCmd)) {//0.32�ŕύX
			if (g_RefreshSetting.mode == 0x01) {
				_stprintf(szText, _T("�Ώۂ��퓬���̎��������s\n%s"), g_RefreshSetting.szCmd);
			}
			else {
				_stprintf(szText, _T("%s"), g_RefreshSetting.szCmd);
			}
			ti.lpszText = szText;
		}
		else if (i == 3 && _tcslen(g_UserSetting[0].szCmd)) {//0.31�ŕύX
			if (g_UserSetting[0].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[0].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[0].szPreJa, g_UserSetting[0].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[0].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}
		else if (i == 4 && _tcslen(g_UserSetting[1].szCmd)) {
			if (g_UserSetting[1].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[1].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[1].szPreJa, g_UserSetting[1].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[1].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}
		else if (i == 5 && _tcslen(g_UserSetting[2].szCmd)) {
			if (g_UserSetting[2].mode == 0x01) {//0.32�Œǉ�
				_stprintf(szBuf, _T("�Ώۂ��퓬���̎��������s\n"));
			}

			if (_tcslen(g_UserSetting[2].szPreJa)) {
				_stprintf(szText, _T("%s%s\n%s"), szBuf, g_UserSetting[2].szPreJa, g_UserSetting[2].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
			else {
				_stprintf(szText, _T("%s%s"), szBuf, g_UserSetting[2].szCmd);//0.32�ŕύX
				ti.lpszText = szText;
			}
		}

		SendMessage(hTool, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
		SendMessage(hTool, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
	}
}

//LISTVIEW�̃I�[�i�[�h���[ 0.02�Œǉ�
void DrawItem(LPDRAWITEMSTRUCT lp)
{
	HDC hDC = lp->hDC;
	HWND hList = lp->hwndItem;
	RECT rcItem[COLUMN_COUNT];
	RECT rcFullSelect;
	_TCHAR szItem[COLUMN_COUNT][0x20];
	HBRUSH hBrush;
	int nColumnCount;

	//hDC��ۑ�
	SaveDC(hDC);

	ZeroMemory(&rcFullSelect, sizeof(rcFullSelect));
	nColumnCount = Header_GetItemCount(ListView_GetHeader(hList));
	//�e�A�C�e���̈ʒu�A��������擾
	for (int i = 0; i<nColumnCount; i++) {//0.15�ŕύX
		rcFullSelect.right += ListView_GetColumnWidth(hList, i);
		ListView_GetSubItemRect(hList, lp->itemID, i, LVIR_LABEL, &rcItem[i]);
		ListView_GetItemText(hList, lp->itemID, i, szItem[i], sizeof(szItem[0]) / sizeof(_TCHAR));
	}
	rcFullSelect.top = rcItem[0].top;
	rcFullSelect.bottom = rcItem[0].bottom;
	rcFullSelect.left = rcItem[0].left;

	//�I�����ꂽ�s�̔w�i�F����
	if (lp->itemState&ODS_SELECTED && lp->hwndItem == GetFocus()) {
		SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		hBrush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
		FillRect(hDC, &rcFullSelect, hBrush);
		DeleteObject(hBrush);
	}
	else {
		if (_tcslen(szItem[COLUMN_DISTANCE]) >= 3 && wcstod(szItem[COLUMN_DISTANCE], NULL) <= (double)g_SpellDistance) {//���@�˒������� 0.13�ŕύX
			hBrush = CreateSolidBrush(RGB(255, 255, 192));
			FillRect(hDC, &rcItem[COLUMN_DISTANCE], hBrush);//���� 0.20�ŕύX
			DeleteObject(hBrush);//0.20�ŕύX
		}

		//MP�}�X��MPP�̊����œh��
		RECT rcMP;
		rcMP.bottom = rcItem[COLUMN_MP].bottom;
		rcMP.top = rcItem[COLUMN_MP].top;
		rcMP.left = rcItem[COLUMN_MP].left;
		rcMP.right = rcItem[COLUMN_MP].left + (rcItem[COLUMN_MP].right - rcItem[COLUMN_MP].left) * _wtoi(szItem[COLUMN_MPP]) / 100;
		if (_wtoi(szItem[COLUMN_MPP]) < 50 && _wtoi(szItem[COLUMN_MPP]) != 0) {//MP��50%����
			hBrush = CreateSolidBrush(RGB(255, 204, 153));
		}
		else {
			hBrush = CreateSolidBrush(RGB(153, 255, 255));
		}
		FillRect(hDC, &rcMP, hBrush);//MP
		if (_tcslen(szItem[COLUMN_DISTANCE]) >= 3 && wcstod(szItem[COLUMN_DISTANCE], NULL) <= (double)g_SpellDistance) {//���@�˒������� 0.13�ŕύX
			rcMP.bottom = rcItem[COLUMN_NAME].bottom;
			rcMP.top = rcItem[COLUMN_NAME].top + (rcItem[COLUMN_NAME].bottom - rcItem[COLUMN_NAME].top) / 2;
			rcMP.left = rcItem[COLUMN_NAME].left;
			rcMP.right = rcItem[COLUMN_NAME].left + (rcItem[COLUMN_NAME].right - rcItem[COLUMN_NAME].left) * _wtoi(szItem[COLUMN_MPP]) / 100;
			FillRect(hDC, &rcMP, hBrush);//Name
		}
		DeleteObject(hBrush);

		//HP�}�X��HPP�̊����œh��
		RECT rcHP;
		rcHP.bottom = rcItem[COLUMN_HP].bottom;
		rcHP.top = rcItem[COLUMN_HP].top;
		rcHP.left = rcItem[COLUMN_HP].left;
		rcHP.right = rcItem[COLUMN_HP].left + (rcItem[COLUMN_HP].right - rcItem[COLUMN_HP].left) * _wtoi(szItem[COLUMN_HPP]) / 100;
		if (_wtoi(szItem[COLUMN_HPP]) < g_DeltaHppPriority[_wtoi(szItem[COLUMN_PRIORITY])]) {//HP��g_DeltaHpp���� //0.13�ŕύX 0.25�ŕύX
			hBrush = CreateSolidBrush(RGB(255, 145, 145));
		}
		else {
			hBrush = CreateSolidBrush(RGB(1, 255, 31));
		}
		FillRect(hDC, &rcHP, hBrush);//HP
		if (_tcslen(szItem[COLUMN_DISTANCE]) >= 3 && wcstod(szItem[COLUMN_DISTANCE], NULL) <= (double)g_SpellDistance) {//���@�˒������� 0.13�ŕύX
			rcHP.bottom = rcItem[COLUMN_NAME].top + (rcItem[COLUMN_NAME].bottom - rcItem[COLUMN_NAME].top) * 3 / 4;
			rcHP.top = rcItem[COLUMN_NAME].top;
			rcHP.left = rcItem[COLUMN_NAME].left;
			rcHP.right = rcItem[COLUMN_NAME].left + (rcItem[COLUMN_NAME].right - rcItem[COLUMN_NAME].left) * _wtoi(szItem[COLUMN_HPP]) / 100;
			FillRect(hDC, &rcHP, hBrush);//Name
		}
		DeleteObject(hBrush);

		if (_wtoi(szItem[COLUMN_TP]) >= 100) {//TP��100�ȏ�
			hBrush = CreateSolidBrush(RGB(171, 145, 255));
			FillRect(hDC, &rcItem[COLUMN_TP], hBrush);//TP
			DeleteObject(hBrush);
		}
	}

	//�w�i�𓧉�
	SetBkMode(hDC, TRANSPARENT);
	//�e�L�X�g�`��
	DRAWTEXTPARAMS dp;
	dp.cbSize = sizeof(DRAWTEXTPARAMS);
	dp.iLeftMargin = 3;
	dp.iRightMargin = 5;
	DrawTextEx(hDC, szItem[0], -1, &rcItem[0], DT_LEFT | DT_VCENTER | DT_SINGLELINE, &dp);

	for (int i = COLUMN_HASTE; i <= COLUMN_USER3; i++) {//0.43e�ŏC��
		DrawText(hDC, szItem[i], -1, &rcItem[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	for (int i = COLUMN_HP; i<nColumnCount; i++) {
		DrawTextEx(hDC, szItem[i], -1, &rcItem[i], DT_RIGHT | DT_VCENTER | DT_SINGLELINE, &dp);
	}

	//�R���g���[����`��
	HTHEME hTheme = OpenThemeData(hList, _T("BUTTON"));
	if (hTheme) {
		if (_tcscmp (szItem[COLUMN_HASTE], _T("1")) == 0) {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_DEFAULTED, &rcItem[COLUMN_HASTE], NULL);
		}
		else {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_NORMAL, &rcItem[COLUMN_HASTE], NULL);
		}
		if (_tcscmp (szItem[COLUMN_REFRESH], _T("1")) == 0) {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_DEFAULTED, &rcItem[COLUMN_REFRESH], NULL);
		}
		else {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_NORMAL, &rcItem[COLUMN_REFRESH], NULL);
		}
		if (_tcscmp (szItem[COLUMN_USER], _T("1")) == 0) {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_DEFAULTED, &rcItem[COLUMN_USER], NULL);
		}
		else {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_NORMAL, &rcItem[COLUMN_USER], NULL);
		}
		//0.22�Œǉ�
		if (_tcscmp (szItem[COLUMN_USER2], _T("1")) == 0) {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_DEFAULTED, &rcItem[COLUMN_USER2], NULL);
		}
		else {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_NORMAL, &rcItem[COLUMN_USER2], NULL);
		}
		if (_tcscmp (szItem[COLUMN_USER3], _T("1")) == 0) {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_DEFAULTED, &rcItem[COLUMN_USER3], NULL);
		}
		else {
			DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, PBS_NORMAL, &rcItem[COLUMN_USER3], NULL);
		}
	}
	else {
		//�r�W���A���X�^�C�����g�p���Ȃ��`��
	}

	//�t�H�[�J�X�̏���
	if (lp->itemState&ODS_FOCUS && lp->hwndItem == GetFocus()) {
		DrawFocusRect(hDC, &rcFullSelect);
	}

	//hDC�����X�g�A
	RestoreDC(hDC, -1);
}

//MeasureItem�֐��̎���
//���X�g�r���[�̍��������ύX
void MeasureItem(LPMEASUREITEMSTRUCT lp)
{
	lp->itemHeight = 15;
}

//LISTVIEW�R���g���[���̃T�u�N���X��
LRESULT CALLBACK WndProcSubClass(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_CONTEXTMENU) {
		//WM_RBUTTONUP��肱���炪�D��
		//�}�E�X�̃X�N���[�����W���N���C�A���g���W�ɕϊ�
		POINT Point;
		Point.x = LOWORD(lp);
		Point.y = HIWORD(lp);
		ScreenToClient(GetParent(hWnd), &Point);
		lp = Point.x + ((Point.y) << 16);
		SendMessage(GetParent(hWnd), WM_RBUTTONUP, wp, lp);
		return 0;
	}
	else if (msg == WM_CHAR) {
		int nItem = -1;
		_TCHAR szText[0x10];
		LVITEM item;

		nItem = ListView_GetNextItem(hWnd, nItem, LVNI_SELECTED);

		item.mask = LVIF_TEXT;
		_stprintf(item.pszText, _T(""));//������


		if (wp == 'h' || wp == 'H') {
			ListView_GetItemText(hWnd, nItem, COLUMN_HASTE, szText, sizeof(szText) / sizeof(_TCHAR));
			if (_tcscmp (szText, _T("0")) == 0) {
				ListView_SetItemText(hWnd, nItem, COLUMN_HASTE, _T("1"));
			}
			else {
				_stprintf(szText, _T("0"));
				ListView_SetItemText(hWnd, nItem, COLUMN_HASTE, _T("0"));
			}
		}
		else if (wp == 'r' || wp == 'R') {
			ListView_GetItemText(hWnd, nItem, COLUMN_REFRESH, szText, sizeof(szText) / sizeof(_TCHAR));
			if (_tcscmp (szText, _T("0")) == 0) {
				ListView_SetItemText(hWnd, nItem, COLUMN_REFRESH, _T("1"));
			}
			else {
				ListView_SetItemText(hWnd, nItem, COLUMN_REFRESH, _T("0"));
			}
		}
		else if (wp == 'u' || wp == 'U') {
			ListView_GetItemText(hWnd, nItem, COLUMN_USER, szText, sizeof(szText) / sizeof(_TCHAR));
			if (_tcscmp (szText, _T("0")) == 0) {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER, _T("1"));
			}
			else {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER, _T("0"));
			}
		}
		else if (wp == 'i' || wp == 'I') {//0.22�Œǉ�
			ListView_GetItemText(hWnd, nItem, COLUMN_USER2, szText, sizeof(szText) / sizeof(_TCHAR));
			if (_tcscmp (szText, _T("0")) == 0) {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER2, _T("1"));
			}
			else {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER2, _T("0"));
			}
		}
		else if (wp == 'o' || wp == 'O') {//0.22�Œǉ�
			ListView_GetItemText(hWnd, nItem, COLUMN_USER3, szText, sizeof(szText) / sizeof(_TCHAR));
			if (_tcscmp (szText, _T("0")) == 0) {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER3, _T("1"));
			}
			else {
				ListView_SetItemText(hWnd, nItem, COLUMN_USER3, _T("0"));
			}
		}
		else if (wp >= '0' && wp <= '5') {
			_stprintf(szText, _T("%d"), wp - '0');
			ListView_SetItemText(hWnd, nItem, COLUMN_PRIORITY, szText);
		}
		else if ((wp == '+' || wp == '-')) {
			ListView_GetItemText(hWnd, nItem, COLUMN_PRIORITY, szText, sizeof(szText) / sizeof(_TCHAR) - 1);
			char chPriority = _wtoi(szText);
			if (wp == '+')chPriority = (++chPriority) % 6;
			else {
				if (chPriority == 0)chPriority = 5;
				else chPriority--;
			}
			_stprintf(szText, _T("%d"), chPriority);
			ListView_SetItemText(hWnd, nItem, COLUMN_PRIORITY, szText);
		}

		return 0;
	}
	else if (msg == WM_LBUTTONDOWN) {//0.02�Œǉ�
		LV_HITTESTINFO lvinfo;
		GetCursorPos((LPPOINT)&lvinfo.pt);
		ScreenToClient(hWnd, &lvinfo.pt);
		ListView_SubItemHitTest(hWnd, &lvinfo);
		if ((lvinfo.flags & LVHT_ONITEM) != 0 && (lvinfo.iSubItem >= COLUMN_HASTE && lvinfo.iSubItem <= COLUMN_USER3)) {//0.22�ŕύX
			_TCHAR szItem[0x20];
			ListView_GetItemText(hWnd, lvinfo.iItem, lvinfo.iSubItem, szItem, sizeof(szItem) / sizeof(_TCHAR));
			if (_tcscmp (szItem, _T("0")) == 0) {
				ListView_SetItemText(hWnd, lvinfo.iItem, lvinfo.iSubItem, _T("1"));
			}
			else if (_tcscmp (szItem, _T("1")) == 0) {
				ListView_SetItemText(hWnd, lvinfo.iItem, lvinfo.iSubItem, _T("0"));
			}
			else {
				ListView_SetItemText(hWnd, lvinfo.iItem, lvinfo.iSubItem, _T("1"));
			}
		}
		//�ʏ폈�����s��
	}
	else if (msg == WM_LBUTTONDBLCLK) {//0.15�Œǉ�
		if (COLUMN_COUNT == Header_GetItemCount(ListView_GetHeader(hWnd))) {//�L�����w�葋
			SendMessage(GetParent(hWnd), WM_COMMAND, IDM_CHANGE_MAXHP, NULL);
		}
	}
	else if (msg == WM_KEYDOWN) {//0.15�Œǉ�
		if (wp == VK_DELETE && COLUMN_COUNT == Header_GetItemCount(ListView_GetHeader(hWnd))) {//�L�����w�葋
			SendMessage(GetParent(hWnd), WM_COMMAND, IDM_DELETE_CHAR, NULL);
		}
	}

	return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
}

//LISTVIEW�R���g���[���̃T�u�N���X��
//Log���p 0.23�Œǉ�
LRESULT CALLBACK WndProcSubClass_Log(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_KEYUP) {
		int nItem = -1;
		_TCHAR szText[MAX_PATH];

		nItem = ListView_GetNextItem(hWnd, nItem, LVNI_SELECTED);

		if (nItem != -1 && wp == 'C' && GetKeyState(VK_CONTROL) & 0x8000) {
			ListView_GetItemText(hWnd, nItem, COLUMN_LOG, szText, sizeof(szText) / sizeof(_TCHAR) - 1);
			//�N���b�v�{�[�h�ɃR�s�[
			if (OpenClipboard(hWnd)) {
				HGLOBAL hg;
				_TCHAR *Mem;

				EmptyClipboard();
				hg = GlobalAlloc(GHND | GMEM_SHARE, MAX_PATH);
				Mem = (_TCHAR*)GlobalLock(hg);
				wcscpy(Mem, szText);
				GlobalUnlock(hg);
				SetClipboardData(CF_UNICODETEXT, hg);
				CloseClipboard();
			}
		}
	}
	else if (msg == WM_CONTEXTMENU) {
		//WM_RBUTTONUP��肱���炪�D�� 0.30�Œǉ�
		//�}�E�X�̃X�N���[�����W���N���C�A���g���W�ɕϊ�
		POINT Point;
		Point.x = LOWORD(lp);
		Point.y = HIWORD(lp);
		ScreenToClient(GetParent(hWnd), &Point);
		lp = Point.x + ((Point.y) << 16);
		SendMessage(GetParent(hWnd), WM_RBUTTONUP, wp, lp);
		return 0;
	}

	return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
}


//���X�g�X�V ���@�r��
int UpdateList(HWND hWnd, int nStart) {
#define FLAG_HASTE	1
#define FLAG_REFRESH	2
#define FLAG_USER	4
#define FLAG_USER2	8//0.22�Œǉ�
#define FLAG_USER3	16//0.22�Œǉ�
	LVITEM item;
	PARTYINFORMATION pInfo[18];
	int nItemCount;
	_TCHAR szName[0x20];
	_TCHAR szHP[0x10];
	_TCHAR szMP[0x10];
	_TCHAR szTP[0x10];
	_TCHAR szPriority[0x10];
	_TCHAR szDistance[0x10];
	_TCHAR szCmd[0x40];
	struct {
		char Player;//�����D��x�ň�ԒႢHPP�̃v���C���[ID
		char HPP;//�����D��x�ň�ԒႢHPP
		int DeltaHP;//�����D��x�ň�ԒႢHPP��HP
	}priority[6];
	char out_range;
	static DWORD dwTime = 0;//�r���\����
	DWORD dwNowTime;
	static DWORD dwRecastTime[11] = { 0,0,0,0,0,0,0,0,0,0,0 };//0-5�̓P�A���A6�̓w�C�X�g�A7�̓��t���A8�̓��[�U�A9�̓��[�U2�A10�̓��[�U3 0.22�ŕύX
	char chHasteRefreshFlag[18];
	static DWORD dwEffectTime[5][18];//�w�C�X�g���t�����[�U�̌��ʎ��� 0.22�ŕύX
	static PARTYINFORMATION myInfo;
	static BYTE byteTargetPlayerNo = -1;
	static char stcDeleteflag[18] = {};//0.24�ŃX�^�e�B�b�N�ɕύX
	static BYTE byteCasting = 0;//0.10�Œǉ�
	static short shRecastNo = 0;//0.10�Œǉ�
	static BYTE byteMagicNo = -1;//0.10�Œǉ� -1�Ń��L���X�g�ԍ����T�[�`���Ȃ� 0-5�P�A�� 6�w�C�X�g 7���t�� 8���[�U
	float fDistance;
	static DWORD stTimer = 0;//0.20�Œǉ�
	static int stnPTmems = 1;//0.20�Œǉ�
	int nPTmems = 0;//0.20�Œǉ�
	static WORD stwAreaID = 0;//0.21�Œǉ�
	static BYTE stbNoRecastMode = 0;//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
	static DWORD stdwFlagCure[18] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//0.40�Œǉ�

	ZeroMemory(&pInfo, sizeof(pInfo));
	ZeroMemory(priority, sizeof(priority));
	for (int i = 0; i<6; i++) {//������
		priority[i].Player = -1;
		priority[i].HPP = 100;
	}
	ZeroMemory(chHasteRefreshFlag, sizeof(chHasteRefreshFlag));

	for (int nPos = 0; nPos<18; nPos++) {
		pInfo[nPos] = GetPartyInfo(nPos);
		if (pInfo[nPos].code == 0)nPTmems++;//0.20�Œǉ�
	}
#ifdef _DEBUG
	_stprintf(pInfo[1].name, _T("_DebugName"));
#endif

	//�G���A�`�F���W���Ńp�[�e�B�����o�[�̏�񂪔��ł鎞 0.20�Œǉ�
	if (stTimer && nPTmems > 1) {//�ҋ@�҂����Ƀp�[�e�B�����o�[�̍X�V���I����� 0.21�Œǉ� 0.24a�ŕύX
		stTimer -= 1000;
		if (stTimer <= 0)stTimer = 1;
	}

	if (stTimer == 0 && nPTmems == 1 && stnPTmems != 1) {
		stTimer = timeGetTime() + g_AreaWaitTime;
		EnableWindow(GetDlgItem(hWnd, IDC_LIST_PC), false);
		stwAreaID = GetArea2ByteID();//0.21�Œǉ�
		for (int i = 0; i<18; i++)stcDeleteflag[i] = 0;//�t���O���Z�b�g 0.24a�Œǉ�
		return 0;
	}
	else if (stTimer && stTimer >= timeGetTime()) {
		if (stwAreaID != GetArea2ByteID()) {//0.21�Œǉ�
			stwAreaID = GetArea2ByteID();
			stTimer = timeGetTime() + g_AreaWaitTime;
		}
		return 0;
	}
	else if (stTimer) {
		stTimer = 0;
		EnableWindow(GetDlgItem(hWnd, IDC_LIST_PC), true);
		stnPTmems = nPTmems;
	}
	else if (nPTmems != 0) {//0.25�Œǉ�
		stnPTmems = nPTmems;
	}

	//�H�Ɏ����̏�񂪖�����������
	if (_tcslen(pInfo[0].name) == 0)return 0;

	//���X�g�̐����擾
	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd, IDC_LIST_PC));

	//���\�������Ԃ��擾
	for (int i = 0; i < nItemCount; i++) {
		stcDeleteflag[i]++;//�폜�t���O 0.24�ŕύX �J�E���g�A�b�v���Ĉ��̐��l�ȏ�ō폜(����Ƀ��X�g���Z�b�g�΍�)
		ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), i, COLUMN_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
		for (int nPos = 0; nPos<18; nPos++) {
			if (_tcscmp (szName, pInfo[nPos].name) == 0 && pInfo[nPos].listnum >= 100) {
				//�܂����X�g���X�V����ĂȂ�PC
				stcDeleteflag[i] = 0;//�폜���Ȃ�
				pInfo[nPos].listnum = i;

				//xz��2��������
				if (g_Offset.Distance) {//0.17�Œǉ�
					fDistance = (float)sqrt(pInfo[nPos].distance);
				}
				else {
					fDistance = (float)sqrt(pow(pInfo[nPos].x - pInfo[0].x, 2) + pow(pInfo[nPos].z - pInfo[0].z, 2));
				}

				item.mask = LVIF_TEXT;
				item.iItem = pInfo[nPos].listnum;//�s
				item.iSubItem = COLUMN_DISTANCE;//��
				if (pInfo[nPos].areaid != pInfo[0].areaid || (pInfo[nPos].inrange == 0 && nPos != 0)) {//0.16�ŕύX
					_stprintf(szDistance, _T("-"));
					out_range = 2;
				}
				else if (fDistance <= (float)g_SpellDistance) {//0.13�ŕύX
					_swprintf(szDistance, _T("%0.2f"), fDistance);
					out_range = 0;
				}
				else {
					_swprintf(szDistance, _T("%0.2f"), fDistance);
					out_range = 1;
				}
				item.pszText = szDistance;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				//HP MP TP���X�V(�V�����o�[�͍X�V���Ȃ�)
				item.mask = LVIF_TEXT;
				item.iItem = pInfo[nPos].listnum;//�s
				item.iSubItem = COLUMN_HP;//��
				if (out_range == 2)_stprintf(szHP, _T(""));
				else _stprintf(szHP, _T("%d"), pInfo[nPos].hp);
				item.pszText = szHP;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				item.iSubItem = COLUMN_HPP;//��
				if (out_range == 2)_stprintf(szHP, _T(""));
				else _stprintf(szHP, _T("%d"), pInfo[nPos].hpp);
				item.pszText = szHP;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				item.iSubItem = COLUMN_MP;//��
				if (out_range == 2)_stprintf(szMP, _T(""));
				else _stprintf(szMP, _T("%d"), pInfo[nPos].mp);
				item.pszText = szMP;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				item.iSubItem = COLUMN_MPP;//��
				if (out_range == 2)_stprintf(szMP, _T(""));
				else _stprintf(szMP, _T("%d"), pInfo[nPos].mpp);
				item.pszText = szMP;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				item.iSubItem = COLUMN_TP;//��
				if (out_range == 2)_stprintf(szTP, _T(""));
				else _stprintf(szTP, _T("%d%%"), pInfo[nPos].tp);
				item.pszText = szTP;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
				}

				//�D��x���擾&�D��x���Ƃ̍ŏ�HPP�L�����N�^�[�̏����擾
				ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_PRIORITY, szPriority, sizeof(szPriority) / sizeof(_TCHAR) - 1);
				if (_tcslen(szPriority) == 0) {
					//�V�����ǉ����ꂽPC
					//�����Z�b�g 0.20�Œǉ�
					_TCHAR szFullPathName[MAX_PATH];
					int nCurSel = SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_GETCURSEL, 0, 0);
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_GETLBTEXT, nCurSel, (LPARAM)szPriority);


					GetFullPathName(_T("character.ini"), sizeof(szFullPathName) / sizeof(_TCHAR) - 1, szFullPathName, NULL);
					if (GetPrivateProfileInt(pInfo[nPos].name, _T("USE"), 0, szFullPathName)) {
						GetPrivateProfileString(pInfo[nPos].name, _T("PRIORITY"), szPriority, szPriority, sizeof(szPriority) / sizeof(_TCHAR), szFullPathName);
						item.mask = LVIF_TEXT;
						item.iItem = i;//�s
						item.iSubItem = COLUMN_PRIORITY;//��
						item.pszText = szPriority;
						ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);

						if (GetPrivateProfileInt(pInfo[nPos].name, _T("HASTE"), 0, szFullPathName)) {
							item.iSubItem = COLUMN_HASTE;//��
							item.pszText = _T("1");
							ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
						}
						if (GetPrivateProfileInt(pInfo[nPos].name, _T("REFRESH"), 0, szFullPathName)) {
							item.iSubItem = COLUMN_REFRESH;//��
							item.pszText = _T("1");
							ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
						}
						if (GetPrivateProfileInt(pInfo[nPos].name, _T("USER"), 0, szFullPathName)) {
							item.iSubItem = COLUMN_USER;//��
							item.pszText = _T("1");
							ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
						}
						//0.22�Œǉ�
						if (GetPrivateProfileInt(pInfo[nPos].name, _T("USER2"), 0, szFullPathName)) {
							item.iSubItem = COLUMN_USER2;//��
							item.pszText = _T("1");
							ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
						}
						if (GetPrivateProfileInt(pInfo[nPos].name, _T("USER3"), 0, szFullPathName)) {
							item.iSubItem = COLUMN_USER3;//��
							item.pszText = _T("1");
							ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
						}
					}
					else {
						item.mask = LVIF_TEXT;
						item.iItem = i;//�s
						item.iSubItem = COLUMN_PRIORITY;//��
						item.pszText = szPriority;
						ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_PC), &item);
					}
				}
				else {
					int nPri = _wtoi(szPriority);
					pInfo[nPos].priority = nPri;
					//�񕜍s���̃f�B���C 0.40a�ŕύX
					if (pInfo[nPos].hpp != 0 && g_DeltaHppPriority[nPri] >= pInfo[nPos].hpp) {
						if (stdwFlagCure[nPos] == 0) {
							stdwFlagCure[nPos] = timeGetTime();
						}
					}
					else {
						stdwFlagCure[nPos] = 0;
					}

					if (out_range == 0 && pInfo[nPos].hpp != 0 && priority[nPri].HPP >= pInfo[nPos].hpp && stdwFlagCure[nPos] && timeGetTime() - stdwFlagCure[nPos] >= g_CureWaitTime) {//0.40�Œǉ�
						priority[nPri].Player = nPos;
						int max_hp = pInfo[nPos].hp * 100 / pInfo[nPos].hpp;
						priority[nPri].HPP = pInfo[nPos].hpp;
						if (g_HinshiDeltaHp && pInfo[nPos].hpp == 1 && pInfo[nPos].hp <= 10) {//0.21�Œǉ�
							priority[nPri].DeltaHP = 2000;
						}
						else priority[nPri].DeltaHP = max_hp - pInfo[nPos].hp;
					}
				}

				//�w�C�X�g�p�`�F�b�N����Ă��邩�擾
				if (out_range == 0) {
					_TCHAR szBuf[0x10];
					char cFlag = 0;
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_HASTE, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
					if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_HASTE;
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_REFRESH, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
					if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_REFRESH;
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_USER, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
					if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER;
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_USER2, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);//0.22�Œǉ�
					if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER2;//0.22�Œǉ�
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_PC), pInfo[nPos].listnum, COLUMN_USER3, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);//0.22�Œǉ�
					if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER3;//0.22�Œǉ�
					chHasteRefreshFlag[nPos] = cFlag;
				}
			}
		}
	}

	//�p�[�e�B�[�����o�[���폜
	for (int i = nItemCount - 1; i >= 0; i--) {
		if (stcDeleteflag[i] >= 5) {//���̐��l�ȏ�ō폜 0.24�ŕύX
			ListView_DeleteItem(GetDlgItem(hWnd, IDC_LIST_PC), i);
		}
	}

	//�p�[�e�B�����o�[���ǉ����ꂽ�ꍇ�͍X�V��ɒǉ�
	char nowlistnum = 0;//���݂̃��X�g�ԍ�
	for (int nPos = 0; nPos<18; nPos++) {
		if (pInfo[nPos].code == 0 && pInfo[nPos].listnum >= 100 && _tcslen(pInfo[nPos].name) >= 3) {
			item.mask = LVIF_TEXT;
			item.iItem = nowlistnum;//�s
			item.iSubItem = 0;//��
			item.pszText = pInfo[nPos].name;
			if (-1 == ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST_PC), &item)) {
			}
		}
		if (pInfo[nPos].code == 0)nowlistnum++;
	}

	//���X�g���ĕ`��
	InvalidateRect(GetDlgItem(hWnd, IDC_LIST_PC), 0, TRUE);

	//���@�r��
	dwNowTime = timeGetTime();
	int nCasting = Casting();

	if (nCasting > 0) {//�r���� 0.10�Œǉ�
		byteCasting = 1;
		return 0;
	}
	else if (byteCasting == 1) {//�r���I����̏���
		byteCasting = 0;
		if (nCasting != -1 && byteMagicNo != -1) {//�r������
			int nSearchRecastNo = 0;//���L���X�g�ԍ����T�[�`���邩 0.10�Œǉ�
			if (shRecastNo == 0) {//���L���X�g�ԍ����ݒ肳��Ė�����
				shRecastNo = SearchRecastNo(1);
				nSearchRecastNo = 1;
			}
			dwRecastTime[byteMagicNo] = dwNowTime + GetRecast(shRecastNo) * 1000 / 60;//���L���X�g���X�V
			if (byteTargetPlayerNo != -1 && (nSearchRecastNo == 0 && GetRecast(shRecastNo) > 0 || nSearchRecastNo != 0)) {//���L���X�g�ԍ����ݒ肵�Ă��鎞�̓��L���X�g���m�F 0.10�ŕύX
				switch (byteMagicNo) {//�w�C�X�g���t�����[�U�̏ꍇ���ʎ��Ԃ��X�V
				case 6://�w�C�X�g
					dwEffectTime[0][byteTargetPlayerNo] = dwNowTime + g_HasteSetting.time;
					break;
				case 7://���t��
					dwEffectTime[1][byteTargetPlayerNo] = dwNowTime + g_RefreshSetting.time;
					break;
				case 8://���[�U
					dwEffectTime[2][byteTargetPlayerNo] = dwNowTime + g_UserSetting[0].time;//0.22�ŕύX
					break;
				case 9://���[�U2 0.22�ŕύX
					dwEffectTime[3][byteTargetPlayerNo] = dwNowTime + g_UserSetting[1].time;
					break;
				case 10://���[�U3 0.22�ŕύX
					dwEffectTime[4][byteTargetPlayerNo] = dwNowTime + g_UserSetting[2].time;
					break;
				default:
					break;
				}
				byteTargetPlayerNo = -1;
			}
		}
		//�r����̒��ւ� 0.20�Œǉ�
		if (byteMagicNo != -1) {
			if (byteMagicNo >= 0 && byteMagicNo <= 5) {
				//�P�A��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_CureSetting[byteMagicNo].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_CureSetting[byteMagicNo].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 6) {
				//�w�C�X�g
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_HasteSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_HasteSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 7) {
				//���t��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_RefreshSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_RefreshSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 8) {
				//���[�U
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[0].szEquipAfter[nEquip])) {//0.22�ŕύX
						ffhook_putcmd(g_UserSetting[0].szEquipAfter[nEquip]);//0.22�ŕύX
					}
					else break;
				}
			}
			else if (byteMagicNo == 9) {
				//���[�U2 0.22�Œǉ�
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[1].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[1].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 10) {
				//���[�U3 0.22�Œǉ�
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[2].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[2].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
		}

		byteMagicNo = -1;
		dwTime = dwNowTime + g_CastWaitTime;//�L���X�g��̃f�B���C 0.14�ŕύX

		return 0;
	}
	else if (stbNoRecastMode) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
		stbNoRecastMode = 0;
		if (byteTargetPlayerNo != -1) {
			switch (byteMagicNo) {//�w�C�X�g���t�����[�U�̏ꍇ���ʎ��Ԃ��X�V
			case 6://�w�C�X�g
				dwEffectTime[0][byteTargetPlayerNo] = dwNowTime + g_HasteSetting.time;
				break;
			case 7://���t��
				dwEffectTime[1][byteTargetPlayerNo] = dwNowTime + g_RefreshSetting.time;
				break;
			case 8://���[�U
				dwEffectTime[2][byteTargetPlayerNo] = dwNowTime + g_UserSetting[0].time;//0.22�ŕύX
				break;
			case 9://���[�U2 0.22�ŕύX
				dwEffectTime[3][byteTargetPlayerNo] = dwNowTime + g_UserSetting[1].time;
				break;
			case 10://���[�U3 0.22�ŕύX
				dwEffectTime[4][byteTargetPlayerNo] = dwNowTime + g_UserSetting[2].time;
				break;
			default:
				break;
			}
			byteTargetPlayerNo = -1;
		}
		if (byteMagicNo != -1) {
			if (byteMagicNo >= 0 && byteMagicNo <= 5) {
				//�P�A��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_CureSetting[byteMagicNo].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_CureSetting[byteMagicNo].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 6) {
				//�w�C�X�g
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_HasteSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_HasteSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 7) {
				//���t��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_RefreshSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_RefreshSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 8) {
				//���[�U
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[0].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[0].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 9) {
				//���[�U2
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[1].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[1].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 10) {
				//���[�U3
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[2].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[2].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
		}

		byteMagicNo = -1;
		dwTime = dwNowTime + g_CastWaitTime;//�L���X�g��̃f�B���C

		return 0;
	}

#ifndef UPLOAD//0.26�Œǉ�
	if (g_SkillUpMode) {
		int nPCCounter = 0;
		for (int i = 0x400; i<0x700; i++) {
			if (GetNPCInfo(i).inrange && GetNPCInfo(i).id != pInfo[0].id) {
				nPCCounter++;
				if (nPCCounter >= 1)return 0;
			}
		}
	}
#endif

	//���S���͉������Ȃ�
	if (pInfo[0].hpp == 0)return 0;

	//Activity���`�F�b�N 0.28�Œǉ�
	WORD wActivity = GetActivityInfo();
	if (wActivity != 0 && wActivity != 1) {
		return 0;
	}

	//���L���X�g�ԍ��̃t���O��������
	shRecastNo = 0;
	byteMagicNo = -1;

	if (nStart == 1 && myInfo.x == pInfo[0].x && myInfo.y == pInfo[0].y && myInfo.z == pInfo[0].z) {//�ʒu�������Ă��Ȃ��ꍇ
		memcpy(&myInfo, &pInfo[0], sizeof(PARTYINFORMATION));//pInfo[0]�͎���
		if (dwTime <= dwNowTime) {
			dwTime = dwNowTime;
			//�����ŃR�}���h�𑗂��Ă݂�
			for (int j = 5; j>0; j--) {//�D��x0�͉񕜂��Ȃ�
				if (priority[j].HPP < g_DeltaHppPriority[j]) {
					//HP��g_DeltaHppPriority�����̎� 0.13�ŕύX 0.25�ŕύX
					//���@��I��
					for (int curenum = 5; curenum >= 0; curenum--) {
						if (priority[j].DeltaHP >= g_CureSetting[curenum].threshold && _tcslen(g_CureSetting[curenum].szCmd) != 0 && dwRecastTime[curenum] <= dwNowTime && GetRecast(g_CureSetting[curenum].recast_no) == 0 && myInfo.mp >= g_CureSetting[curenum].mp) {//0.39�ŕύX
							ZeroMemory(szCmd, sizeof(szCmd));
							if (_tcslen(pInfo[priority[j].Player].name) == 0)break;
							_stprintf(szCmd, _T("%s %s"), g_CureSetting[curenum].szCmd, pInfo[priority[j].Player].name);
							if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
								ffhook_putcmd(szCmd);
								dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
								shRecastNo = g_CureSetting[curenum].recast_no;
								byteMagicNo = curenum;
								if (shRecastNo == 0) {//0.10�Œǉ�
									SearchRecastNo(0);
								}
								for (int nEquip = 0; nEquip<9; nEquip++) {//0.19�Œǉ� 0.43g�ŕύX
									if (_tcslen(g_CureSetting[curenum].szEquip[nEquip])) {
										ffhook_putcmd(g_CureSetting[curenum].szEquip[nEquip]);
									}
									else break;
								}
								return 1;
							}
						}
					}
				}
			}

			//�w�C�X�g�@H�A�C�R����PC
			if (dwRecastTime[6] <= dwNowTime && _tcslen(g_HasteSetting.szCmd) != 0) {
				//�w�C�X�g��6��
				for (int i = 0; i<18; i++) {
					if (chHasteRefreshFlag[i] & FLAG_HASTE && dwEffectTime[0][i] <= dwNowTime && GetRecast(g_HasteSetting.recast_no) == 0 && myInfo.mp >= g_HasteSetting.mp) {//0.39�ŕύX
						if (g_HasteSetting.mode == 1) {//Haste_MODE���`�F�b�N 0.32�Œǉ�
							if (pInfo[i].state == 0x01) {
							}
							else {
								continue;
							}
						}
						if (pInfo[i].hpp == 0) {//0.32�Œǉ�
							continue;
						}

						ZeroMemory(szCmd, sizeof(szCmd));
						_stprintf(szCmd, _T("%s %s"), g_HasteSetting.szCmd, pInfo[i].name);
						if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
							if (wcsncmp(g_HasteSetting.szPreJa, _T("/"), 1) == 0) {
								//�v���W���u�A�r���e�B 0.33�Œǉ�
								ffhook_putcmd(g_HasteSetting.szPreJa);
								Sleep(1500);
							}
							ffhook_putcmd(szCmd);
							dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
							shRecastNo = g_HasteSetting.recast_no;
							byteTargetPlayerNo = i;
							byteMagicNo = 6;
							if (shRecastNo == 0) {//0.10�Œǉ�
								SearchRecastNo(0);
							}
							else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
								stbNoRecastMode = 1;
							}

							for (int nEquip = 0; nEquip<9; nEquip++) {//0.19�Œǉ� 0.43g�ŕύX
								if (_tcslen(g_HasteSetting.szEquip[nEquip])) {
									ffhook_putcmd(g_HasteSetting.szEquip[nEquip]);
								}
								else break;
							}
							return 1;
						}
						break;
					}
				}
			}

			//���t���@R�A�C�R����PC
			if (dwRecastTime[7] <= dwNowTime && _tcslen(g_RefreshSetting.szCmd) != 0) {
				//���t����7��
				for (int i = 0; i<18; i++) {
					if (chHasteRefreshFlag[i] & FLAG_REFRESH && dwEffectTime[1][i] <= dwNowTime && GetRecast(g_RefreshSetting.recast_no) == 0 && myInfo.mp >= g_RefreshSetting.mp) {//0.39�ŕύX
						if (g_RefreshSetting.mode == 1) {//Refresh_MODE���`�F�b�N 0.32�Œǉ�
							if (pInfo[i].state == 0x01) {
							}
							else {
								continue;
							}
						}
						if (pInfo[i].hpp == 0) {//0.32�Œǉ�
							continue;
						}

						ZeroMemory(szCmd, sizeof(szCmd));
						_stprintf(szCmd, _T("%s %s"), g_RefreshSetting.szCmd, pInfo[i].name);
						if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
							if (wcsncmp(g_RefreshSetting.szPreJa, _T("/"), 1) == 0) {
								//�v���W���u�A�r���e�B 0.33�Œǉ�
								ffhook_putcmd(g_RefreshSetting.szPreJa);
								Sleep(1500);
							}
							ffhook_putcmd(szCmd);
							dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
							shRecastNo = g_RefreshSetting.recast_no;
							byteTargetPlayerNo = i;
							byteMagicNo = 7;
							if (shRecastNo == 0) {
								SearchRecastNo(0);
							}
							else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
								stbNoRecastMode = 1;
							}
							for (int nEquip = 0; nEquip<9; nEquip++) {//0.19�Œǉ� 0.43g�ŕύX
								if (_tcslen(g_RefreshSetting.szEquip[nEquip])) {
									ffhook_putcmd(g_RefreshSetting.szEquip[nEquip]);
								}
								else break;
							}
							return 1;
						}
						break;
					}
				}
			}

			for (int nUserNum = 0; nUserNum <= 2; nUserNum++) {
				//0:USER�A1:USER2,2:USER3 0.22�ŕύX
				if (dwRecastTime[nUserNum + 8] <= dwNowTime && _tcslen(g_UserSetting[nUserNum].szCmd) != 0) {//���[�U��8��
					for (int i = 0; i < 18; i++) {
						if (chHasteRefreshFlag[i] & (FLAG_USER << nUserNum) && dwEffectTime[nUserNum + 2][i] <= dwNowTime && GetRecast(g_UserSetting[nUserNum].recast_no) == 0 && myInfo.mp >= g_UserSetting[nUserNum].mp) {//0.39�ŕύX
							if (g_UserSetting[nUserNum].mode == 1) {//User*_MODE���`�F�b�N 0.32�Œǉ�
								if (pInfo[i].state == 0x01) {
								}
								else {
									continue;
								}
							}
							if (pInfo[i].hpp == 0) {//0.32�Œǉ�
								continue;
							}

							ZeroMemory(szCmd, sizeof(szCmd));
							_stprintf(szCmd, _T("%s %s"), g_UserSetting[nUserNum].szCmd, pInfo[i].name);
							if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
								if (wcsncmp(g_UserSetting[nUserNum].szPreJa, _T("/"), 1) == 0) {//�v���W���u�A�r���e�B 0.31�Œǉ�
									ffhook_putcmd(g_UserSetting[nUserNum].szPreJa);
									Sleep(1500);
								}
								ffhook_putcmd(szCmd);
								dwTime += g_CmdInterval;
								shRecastNo = g_UserSetting[nUserNum].recast_no;
								byteTargetPlayerNo = i;
								byteMagicNo = nUserNum + 8;
								if (shRecastNo == 0) {
									SearchRecastNo(0);
								}
								else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
									stbNoRecastMode = 1;
								}
								for (int nEquip = 0; nEquip < 9; nEquip++) {//0.43g�ŕύX
									if (_tcslen(g_UserSetting[nUserNum].szEquip[nEquip])) {
										ffhook_putcmd(g_UserSetting[nUserNum].szEquip[nEquip]);
									}
									else break;
								}
								return 1;
							}
							break;
						}
					}
				}
			}
			if (exec_queue() == 1) {//0.13a�ŕύX
				dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
				return 1;
			}
		}
	}
	else {//�������ꍇ�͈ʒu����ۑ�
		memcpy(&myInfo, &pInfo[0], sizeof(PARTYINFORMATION));//pInfo[0]�͎���
		dwTime = dwNowTime + g_MoveWaitTime;//0.14�ŕύX
	}

	return 0;
}


//���������烊�L���X�g���擾
short GetRecast(DWORD pos)
{
	DWORD dwAddress;
	short shRet;

	if (pos == 0xFFFFFFFF) {//recast_no=-1�̎����L���X�g�����Ȃ� 0.43e�Œǉ�
		return 0;
	}

	dwAddress = g_DllAddress + g_Offset.Recast + pos;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &shRet, sizeof(short), NULL)) {
		return -1;
	}

	return shRet;
}

//���������烊�L���X�g�ԍ����T�[�`
//0.10�Œǉ�
int SearchRecastNo(int ON)
{
	int RecastNo = -1;
	DWORD dwAddress;
#define RECAST_MEM_SIZE	1000
	static short shRecastPast[RECAST_MEM_SIZE];
	static short shRecastNow[RECAST_MEM_SIZE];


	if (ON == 0) {//���݂̏�Ԃ�ۑ�
		dwAddress = g_DllAddress + g_Offset.Recast;
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, shRecastPast, RECAST_MEM_SIZE * sizeof(short), NULL)) {//0.50d�ŕύX
			return -12;
		}
	}
	else if (ON == 1) {
		dwAddress = g_DllAddress + g_Offset.Recast;
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, shRecastNow, RECAST_MEM_SIZE * sizeof(short), NULL)) {//0.50d�ŕύX
			return -14;
		}
		for (int i = 1; i < RECAST_MEM_SIZE; i++) {
			//shRecast��short�Ŋi�[����Ă���̂Ń��[�v�񐔂�short�Ŋ������l
			if (shRecastPast[i] == 0 && shRecastNow[i] > 0) {
				//���L���X�g�ԍ��̓I�t�Z�b�g�A�h���X����̐��l�Ȃ̂�short���|����
				RecastNo = i * sizeof(short);
				break;
			}
		}
	}

	return RecastNo;
}

//���������烊�L���X�g�ԍ����T�[�`
//0.10�Œǉ�
int SearchRecastNo_Log(int ON)
{
	int RecastNo = -1;
	DWORD dwAddress;
	static short shRecastPast[RECAST_MEM_SIZE];
	static short shRecastNow[RECAST_MEM_SIZE];


	if (ON == 0) {//���݂̏�Ԃ�ۑ�
		dwAddress = g_DllAddress + g_Offset.Recast;
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, shRecastPast, RECAST_MEM_SIZE * sizeof(short), NULL)) {//0.50d�ŕύX
			return -12;
		}
	}
	else if (ON == 1) {
		dwAddress = g_DllAddress + g_Offset.Recast;
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, shRecastNow, RECAST_MEM_SIZE * sizeof(short), NULL)) {//0.50d�ŕύX
			return -14;
		}
		for (int i = 1; i < RECAST_MEM_SIZE; i++) {//shRecast��short�Ŋi�[����Ă���̂Ń��[�v�񐔂�short�Ŋ������l
			if (shRecastPast[i] == 0 && shRecastNow[i] > 0) {
				RecastNo = i * sizeof(short);//���L���X�g�ԍ��̓I�t�Z�b�g�A�h���X����̐��l�Ȃ̂�short���|����
				break;
			}
		}
	}

	return RecastNo;
}


//����������L���X�g�����ǂ�������
//�L���X�g��̓��͎�t�ҋ@���Ԃ͕ʓr�������邱��
//0.10�Œǉ�
//�P�A��1�r���������00002043000000000000803F�Ō���
//���������A�h���X-0��14�̃A�h���X���G���f�B�A���������̂�����
//���������A�h���X-�x�[�X�A�h���X���I�t�Z�b�g
int Casting()
{
	DWORD dwAddress;
	struct {
		DWORD unk1;//�s���A�h���X
		DWORD unk2;//0?
		DWORD nowCasting;//�r�����̂݃A�h���X�炵�����l�A���r����0
		DWORD unk3;//0x0100FFFF?
		DWORD unk4;//0x01000000?
		float cast_frame;//�r�����Ă��閂�@�̃L���X�g����(�P�ʂ̓t���[��)
		float cast_left;//�r���I���܂ł̎���(�P�ʂ̓t���[��)���f���ꂽ�ꍇ�͒��f���ꂽ���̎���
		float cast_gage;//�L���X�g�Q�[�W0�`1.0�B���f���ꂽ���͒��f���ꂽ���̒l�B�r����������0x0000803F=1.0
	}mem_cast;

	dwAddress = g_DllAddress + g_Offset.Cast;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &mem_cast, sizeof(mem_cast), NULL)) {
		return -1;
	}

	if (mem_cast.nowCasting)return (WORD)mem_cast.cast_left;//�c��L���X�g�t���[��
	else if (!mem_cast.nowCasting && mem_cast.cast_left)return -1;//�L���X�g���s

	return 0;
}

//PARTY���擾
PARTYINFORMATION GetPartyInfo(int pos)
{
	DWORD dwAddress;
	PARTYINFORMATION info;
	char dump[0x210];

	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

	//���������[�h
	dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI + pos * NOWSTA_SIZE;//�X�^�[�g�ʒu�̕␳NOWSTA_HOSEI
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
		info.code = -1;
		return info;
	}

	//NOWSTA_MEM_PARTY��0�̎����łɒE��
	if (dump[NOWSTA_MEM_PARTY] == 0x01) {//0.35�ŕύX
		memcpy(&(info.id), &dump[NOWSTA_MEM_CHANGEID], sizeof(WORD));

		info = GetNPCInfo(info.id);
		info.areaid = dump[NOWSTA_MEM_AREAID];
		info.hpp = dump[NOWSTA_MEM_HPP];
		info.mpp = dump[NOWSTA_MEM_MPP];
		memcpy(&(info.hp), &dump[NOWSTA_MEM_HP], sizeof(DWORD));
		memcpy(&(info.mp), &dump[NOWSTA_MEM_MP], sizeof(DWORD));
		memcpy(&(info.tp), &dump[NOWSTA_MEM_TP], sizeof(DWORD));
		mbstowcs(info.name, &dump[NOWSTA_MEM_NAME], sizeof(info.name) / sizeof(_TCHAR) - 1);
		info.listnum = 100 + pos;//���X�g�ǉ��p�V���A���i���o�[
		info.code = 0;//�G���A�O��PC�������ꍇ-3�ƂȂ�̂�0�ɒu������
	}
	else {
		info.code = -4;
	}

	return info;
}

//NPC���擾
PARTYINFORMATION GetNPCInfo(int id)
{
	DWORD dwAddress;
	PARTYINFORMATION info;
	char dump[NPC_MEM_SIZE];

	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

	//���������[�h
	dwAddress = g_DllAddress + g_Offset.NpcMap + id * sizeof(DWORD);
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		info.code = -1;
		return info;
	}
	if (dwAddress != 0) {
		//���A�h���X�����݂��鎞�̏���
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
			info.code = -2;
			return info;
		}
		memcpy(&(info.x), &dump[g_Offset.PosX], sizeof(float));//0.43h�ŕύX
		memcpy(&(info.y), &dump[g_Offset.PosY], sizeof(float));//0.43h�ŕύX
		memcpy(&(info.z), &dump[g_Offset.PosZ], sizeof(float));//0.43h�ŕύX
		memcpy(&(info.hpp), &dump[g_Offset.Hpp], sizeof(BYTE));//0.43h�ŕύX
		memcpy(&(info.radian), &dump[g_Offset.Radian], sizeof(float));//0.43h�ŕύX
		mbstowcs(info.name, &dump[g_Offset.Name], sizeof(info.name) / sizeof(_TCHAR) - 1);//0.43h�ŕύX
		memcpy(&(info.inrange), &dump[g_Offset.Inrange], sizeof(BYTE));//0.16�ŕύX
		memcpy(&(info.distance), &dump[g_Offset.Distance], sizeof(float));//0.17�ŕύX
		memcpy(&(info.state), &dump[g_Offset.State], sizeof(BYTE));//0.28�ŕύX
		info.id = (WORD)id;
		memcpy(&(info.fixedid), &dump[g_Offset.FixedId], sizeof(DWORD));//0.43h�ŕύX
	}
	else {
		//dwAddress�ɃA�h���X�������ꍇ
		info.code = -3;
	}

	return info;
}

int ReadINI(HWND hWnd, LPCTSTR szSection)
{
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szFullPathName[MAX_PATH];

	if (0 == GetFullPathName(_T("healer.ini"), sizeof(szFullPathName) / sizeof(_TCHAR) - 1, szFullPathName, NULL)) {
		return 0;
	}

	if (_tcslen(szSection) == 0) {
		//[OFFSET]
		g_Offset.Auto = GetPrivateProfileInt(_T("OFFSET"), _T("AUTO"), 0, szFullPathName);//0.36�Œǉ�
		GetPrivateProfileString(_T("OFFSET"), _T("NPCMAP"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);
		g_Offset.NpcMap = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("MAP"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.21�Œǉ�
		g_Offset.Map = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("NOWSTA"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);
		g_Offset.NowSta = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("RECAST"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);
		g_Offset.Recast = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("CAST"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);
		g_Offset.Cast = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("VIEWSTYLE"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);
		g_Offset.ViewPoint = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("TARGET"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.17�Œǉ�
		g_Offset.Target = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("NPCLIST"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.28�Œǉ�
		g_Offset.NpcList = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("OFFSET"), _T("ACTIVITY_INFO"), NULL, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.28�Œǉ�
		g_Offset.ActivityInfo = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��

		GetPrivateProfileString(_T("NPC_MEM"), _T("POS_X"), _T("04"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.PosX = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("POS_Y"), _T("08"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.PosY = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("POS_Z"), _T("0C"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.PosZ = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("RADIAN"), _T("18"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.Radian = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("FIXEDID"), _T("78"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.FixedId = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("NAME"), _T("7C"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.Name = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("HP"), _T("CC"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�Œǉ�
		g_Offset.Hpp = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��

		GetPrivateProfileString(_T("NPC_MEM"), _T("INRANGE"), _T("119"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�ŕύX
		g_Offset.Inrange = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("DISTANCE"), _T("B4"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�ŕύX
		g_Offset.Distance = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��
		GetPrivateProfileString(_T("NPC_MEM"), _T("STATE"), _T("144"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.43h�ŕύX
		g_Offset.State = _tcstoul(szBuf, NULL, 16);//16�i���œǂݍ��݂��s��

												  //INIT
		GetPrivateProfileString(_T("INIT"), _T("DEFAULT_PIDNAME"), _T(""), g_szDefaultPIDName, sizeof(g_szDefaultPIDName) / sizeof(_TCHAR) - 1, szFullPathName);//0.43�Œǉ�
		g_UseWindowerHelper = GetPrivateProfileInt(_T("INIT"), _T("USE_WINDOWERHELPER"), 0, szFullPathName);//0.1.1.0�Œǉ�
		g_SavePosition = GetPrivateProfileInt(_T("INIT"), _T("SAVE_POSITION"), 0, szFullPathName);//0.15�Œǉ�
		int x = GetPrivateProfileInt(_T("INIT"), _T("X"), 100, szFullPathName);
		int y = GetPrivateProfileInt(_T("INIT"), _T("Y"), 0, szFullPathName);
		int cx = GetPrivateProfileInt(_T("INIT"), _T("SIZEX"), 400, szFullPathName);
		int cy = GetPrivateProfileInt(_T("INIT"), _T("SIZEY"), 360, szFullPathName);

		g_DefaultPriority = GetPrivateProfileInt(_T("INIT"), _T("PRIORITY"), 3, szFullPathName);//0.13�ŕύX
		g_Log.x = GetPrivateProfileInt(_T("INIT"), _T("LOG_X"), 100, szFullPathName);
		g_Log.y = GetPrivateProfileInt(_T("INIT"), _T("LOG_Y"), 360, szFullPathName);
		g_Log.cx = GetPrivateProfileInt(_T("INIT"), _T("LOG_SIZEX"), 400, szFullPathName);
		g_Log.cy = GetPrivateProfileInt(_T("INIT"), _T("LOG_SIZEY"), 360, szFullPathName);
		g_Follow.x = GetPrivateProfileInt(_T("INIT"), _T("FOLLOW_X"), 100, szFullPathName);
		g_Follow.y = GetPrivateProfileInt(_T("INIT"), _T("FOLLOW_Y"), 360, szFullPathName);
		g_Target.x = GetPrivateProfileInt(_T("INIT"), _T("TARGET_X"), 0, szFullPathName);//0.17�Œǉ�
		g_Target.y = GetPrivateProfileInt(_T("INIT"), _T("TARGET_Y"), 0, szFullPathName);//0.17�Œǉ�
		g_Target_Add.x = GetPrivateProfileInt(_T("INIT"), _T("TARGET_ADD_X"), 200, szFullPathName);//0.18�Œǉ�
		g_Target_Add.y = GetPrivateProfileInt(_T("INIT"), _T("TARGET_ADD_Y"), 5, szFullPathName);//0.18�Œǉ�
		g_Combat.x = GetPrivateProfileInt(_T("INIT"), _T("COMBAT_X"), 0, szFullPathName);//0.28�Œǉ�
		g_Combat.y = GetPrivateProfileInt(_T("INIT"), _T("COMBAT_Y"), 0, szFullPathName);//0.28�Œǉ�
		g_Fellow.x = GetPrivateProfileInt(_T("INIT"), _T("FELLOW_X"), 0, szFullPathName);//0.34�Œǉ�
		g_Fellow.y = GetPrivateProfileInt(_T("INIT"), _T("FELLOW_Y"), 0, szFullPathName);//0.34�Œǉ�
		g_DeltaHppPriority[1] = GetPrivateProfileInt(_T("INIT"), _T("DELTA_HPP_PRIORITY1"), 90, szFullPathName);//0.25�Œǉ�
		g_DeltaHppPriority[2] = GetPrivateProfileInt(_T("INIT"), _T("DELTA_HPP_PRIORITY2"), 90, szFullPathName);//0.25�Œǉ�
		g_DeltaHppPriority[3] = GetPrivateProfileInt(_T("INIT"), _T("DELTA_HPP_PRIORITY3"), 90, szFullPathName);//0.25�Œǉ�
		g_DeltaHppPriority[4] = GetPrivateProfileInt(_T("INIT"), _T("DELTA_HPP_PRIORITY4"), 90, szFullPathName);//0.25�Œǉ�
		g_DeltaHppPriority[5] = GetPrivateProfileInt(_T("INIT"), _T("DELTA_HPP_PRIORITY5"), 90, szFullPathName);//0.25�Œǉ�
		g_HinshiDeltaHp = GetPrivateProfileInt(_T("INIT"), _T("HINSHI_DELTA_HP"), 1, szFullPathName);//0.21�Œǉ�
		GetPrivateProfileString(_T("INIT"), _T("DEFAULT_FOLLOWER"), NULL, g_DefaultFollower, sizeof(g_DefaultFollower) / sizeof(_TCHAR) - 1, szFullPathName);//0.23�Œǉ�
		GetPrivateProfileString(_T("INIT"), _T("SPELL_DISTANCE"), _T("20.50"), szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1, szFullPathName);//0.13�Œǉ�
		g_SpellDistance = _wtof(szBuf);
		g_CastWaitTime = GetPrivateProfileInt(_T("INIT"), _T("CAST_WAIT_TIME"), 2000, szFullPathName);//0.14�Œǉ�
		g_CmdInterval = GetPrivateProfileInt(_T("INIT"), _T("CMD_INTERVAL"), 1000, szFullPathName);//0.14�Œǉ�
		g_CmdWaitTime = GetPrivateProfileInt(_T("INIT"), _T("CMD_WAIT_TIME"), 800, szFullPathName);//0.14�Œǉ�
		g_MoveWaitTime = GetPrivateProfileInt(_T("INIT"), _T("MOVE_WAIT_TIME"), 2000, szFullPathName);//0.14�Œǉ�
		g_AreaWaitTime = GetPrivateProfileInt(_T("INIT"), _T("AREA_WAIT_TIME"), 10000, szFullPathName);//0.20�Œǉ�
		g_CureWaitTime = GetPrivateProfileInt(_T("INIT"), _T("CURE_WAIT_TIME"), 0, szFullPathName);//0.40�Œǉ�
		g_StopTime = GetPrivateProfileInt(_T("INIT"), _T("STOP_TIME"), 30, szFullPathName);//0.40�Œǉ�
		GetPrivateProfileString(_T("INIT"), _T("STOP_WAV"), _T("stop.wav"), g_szStopWav, sizeof(g_szStopWav) / sizeof(_TCHAR) - 1, szFullPathName);//0.41�Œǉ�
		g_AreaStop = GetPrivateProfileInt(_T("INIT"), _T("AREA_STOP"), 0, szFullPathName);//0.42�Œǉ�
		g_Log.default_open = GetPrivateProfileInt(_T("INIT"), _T("WINDOW_LOG"), 0, szFullPathName);//0.15�Œǉ�
		g_Follow.default_open = GetPrivateProfileInt(_T("INIT"), _T("WINDOW_FOLLOW"), 0, szFullPathName);//0.15�Œǉ�
		g_Reserved.default_open = GetPrivateProfileInt(_T("INIT"), _T("WINDOW_RESERVED"), 0, szFullPathName);//0.15�Œǉ�
		g_Target.default_open = GetPrivateProfileInt(_T("INIT"), _T("WINDOW_TARGET"), 0, szFullPathName);//0.17�Œǉ�
		g_Target_Add.default_open = GetPrivateProfileInt(_T("INIT"), _T("WINDOW_TARGET_ADD"), 0, szFullPathName);//0.18�Œǉ�

																												 //�f�t�H���g�D��x�̐ݒ�
		if (g_DefaultPriority > 5 || g_DefaultPriority < 0) {//0.13�ŕύX
			g_DefaultPriority = 3;
		}
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("0"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("1"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("2"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("3"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("4"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("5"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_PRIORITY), CB_SETCURSEL, g_DefaultPriority, 0);

		//DeltaHpp�̐ݒ�
		for (int i = 5; i > 0; i--) {//0.25�Œǉ�
			if (g_DeltaHppPriority[i] > 95 || g_DeltaHppPriority[i] < 10) {
				g_DeltaHppPriority[i] = 90;
			}
		}

		//SpellDistance�̐ݒ� 0.13�Œǉ�
		if (g_SpellDistance > 30.0 || g_SpellDistance < 5.0) {
			g_SpellDistance = 20.5;
		}

		//0.14�Œǉ�
		if (g_CastWaitTime > 5000 || g_CastWaitTime < 1000) {
			g_CastWaitTime = 2000;
		}
		if (g_CmdInterval > 2000 || g_CmdInterval < 400) {
			g_CmdInterval = 400;
		}
		if (g_CmdWaitTime > 1000 || g_CmdWaitTime < 400) {
			g_CmdWaitTime = 800;
		}
		if (g_MoveWaitTime > 9500 || g_MoveWaitTime < 1000) {
			g_MoveWaitTime = 2000;
		}
		//0.40�Œǉ�
		if (g_CureWaitTime > 10000) {
			g_CureWaitTime = 0;
		}
		if (g_StopTime > 300) {
			g_StopTime = 0;
		}

		//�e�R���g���[���̑傫����ύX
		MoveWindow(hWnd, x, y, cx, cy, TRUE);
		SendMessage(hWnd, WM_SIZE, 0, 0);

		//COLUMN
		g_ColumnCx[0] = GetPrivateProfileInt(_T("COLUMN"), _T("NAME"), 115, szFullPathName);
		g_ColumnCx[1] = GetPrivateProfileInt(_T("COLUMN"), _T("HASTE"), 23, szFullPathName);
		g_ColumnCx[2] = GetPrivateProfileInt(_T("COLUMN"), _T("REFRESH"), 23, szFullPathName);
		g_ColumnCx[3] = GetPrivateProfileInt(_T("COLUMN"), _T("USER"), 23, szFullPathName);
		//0.22�ŕύX
		g_ColumnCx[4] = GetPrivateProfileInt(_T("COLUMN"), _T("USER2"), 0, szFullPathName);
		g_ColumnCx[5] = GetPrivateProfileInt(_T("COLUMN"), _T("USER3"), 0, szFullPathName);
		g_ColumnCx[6] = GetPrivateProfileInt(_T("COLUMN"), _T("HP"), 40, szFullPathName);
		g_ColumnCx[7] = GetPrivateProfileInt(_T("COLUMN"), _T("HPP"), 0, szFullPathName);
		g_ColumnCx[8] = GetPrivateProfileInt(_T("COLUMN"), _T("MP"), 40, szFullPathName);
		g_ColumnCx[9] = GetPrivateProfileInt(_T("COLUMN"), _T("MPP"), 0, szFullPathName);
		g_ColumnCx[10] = GetPrivateProfileInt(_T("COLUMN"), _T("TP"), 40, szFullPathName);
		g_ColumnCx[11] = GetPrivateProfileInt(_T("COLUMN"), _T("PRIORITY"), 30, szFullPathName);
		g_ColumnCx[12] = GetPrivateProfileInt(_T("COLUMN"), _T("DISTANCE"), 50, szFullPathName);
		g_ColumnCx[13] = GetPrivateProfileInt(_T("COLUMN"), _T("MAXHP"), 55, szFullPathName);

		g_ColumnCxLog[0] = GetPrivateProfileInt(_T("COLUMN"), _T("ATTR"), 45, szFullPathName);
		g_ColumnCxLog[1] = GetPrivateProfileInt(_T("COLUMN"), _T("TIME"), 0, szFullPathName);
		g_ColumnCxLog[2] = GetPrivateProfileInt(_T("COLUMN"), _T("LOG"), 370, szFullPathName);
		g_ColumnCxLog[3] = GetPrivateProfileInt(_T("COLUMN"), _T("RECASTNO"), 50, szFullPathName);

		//[SECTION]
		for (int i = 0; i<20; i++) {//0.26�ŕύX
			_stprintf(szBuf, _T("Section%d"), i + 1);
			GetPrivateProfileString(_T("SECTION"), szBuf, NULL, g_SectionName[i], sizeof(g_SectionName[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}

		//[LINK]0.17�Œǉ�
		g_Link.open = GetPrivateProfileInt(_T("LINK"), _T("LINK_OPEN"), 1, szFullPathName);
		GetPrivateProfileString(_T("LINK"), _T("EXE"), _T("iexplore.exe"), g_Link.exe, sizeof(g_Link.exe) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(_T("LINK"), _T("CMD"), NULL, g_Link.cmd, sizeof(g_Link.cmd) / sizeof(_TCHAR) - 1, szFullPathName);
		for (int i = 0; i<4; i++) {
			_stprintf(szBuf, _T("LINK%d"), i + 1);
			GetPrivateProfileString(_T("LINK"), szBuf, NULL, g_Link.link[i], sizeof(g_Link.link[0]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("URL%d"), i + 1);
			GetPrivateProfileString(_T("LINK"), szBuf, NULL, g_Link.url[i], sizeof(g_Link.url[0]) / sizeof(_TCHAR) - 1, szFullPathName);
		}

	}
	else if (_tcscmp (szSection, _T("SECTION")) == 0) {//0.26�Œǉ�
		for (int i = 0; i<20; i++) {
			_stprintf(szBuf, _T("Section%d"), i + 1);
			GetPrivateProfileString(_T("SECTION"), szBuf, NULL, g_SectionName[i], sizeof(g_SectionName[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}
	}
	else {//[*] �Z�N�V�����ݒ莞
		for (int i = 0; i <= 5; i++) {//0.19�ŕύX
			_stprintf(szBuf, _T("Cure_%d_CMD"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_CureSetting[i].szCmd, sizeof(g_CureSetting[i].szCmd) / sizeof(_TCHAR) - 1, szFullPathName);

			_stprintf(szBuf, _T("Cure_%d_RECAST_NO"), i + 1);
			g_CureSetting[i].recast_no = GetPrivateProfileInt(szSection, szBuf, 0, szFullPathName);

			_stprintf(szBuf, _T("Cure_%d_THRESHOLD"), i + 1);
			g_CureSetting[i].threshold = GetPrivateProfileInt(szSection, szBuf, 0, szFullPathName);

			_stprintf(szBuf, _T("Cure_%d_MP"), i + 1);//0.39�Œǉ�
													  //g_CureSetting[i].mp = GetPrivateProfileInt(szSection,szBuf,0,szFullPathName);//0.39�Œǉ�
			g_CureSetting[i].mp = (DWORD)GetPrivateProfileInt(szSection, szBuf, 0, szFullPathName);//0.40�ŕύX

																								   //���ւ��p
			for (int j = 0; j<9; j++) {//0.19�Œǉ� 0.20�ŕύX
				_stprintf(szBuf, _T("Cure_%d_EQUIP%d"), i + 1, j + 1);
				GetPrivateProfileString(szSection, szBuf, NULL, g_CureSetting[i].szEquip[j], sizeof(g_CureSetting[i].szEquip[j]) / sizeof(_TCHAR) - 1, szFullPathName);
				_stprintf(szBuf, _T("Cure_%d_EQUIP_AFTER%d"), i + 1, j + 1);
				GetPrivateProfileString(szSection, szBuf, NULL, g_CureSetting[i].szEquipAfter[j], sizeof(g_CureSetting[i].szEquipAfter[j]) / sizeof(_TCHAR) - 1, szFullPathName);
			}
		}
		//0.41�Œǉ�
		GetPrivateProfileString(szSection, _T("Haste_HEADER"), _T("Haste"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		SendMessage(hWnd, WM_CHANGE_HEADER_NAME, (WPARAM)szBuf, (LPARAM)1);
		GetPrivateProfileString(szSection, _T("Haste_CMD"), NULL, g_HasteSetting.szCmd, sizeof(g_HasteSetting.szCmd) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(szSection, _T("Haste_PREJA"), NULL, g_HasteSetting.szPreJa, sizeof(g_HasteSetting.szPreJa) / sizeof(_TCHAR) - 1, szFullPathName);//0.33�Œǉ�
		g_HasteSetting.recast_no = GetPrivateProfileInt(szSection, _T("Haste_RECAST_NO"), 0, szFullPathName);
		g_HasteSetting.time = GetPrivateProfileInt(szSection, _T("Haste_TIME"), 0, szFullPathName);
		g_HasteSetting.mode = GetPrivateProfileInt(szSection, _T("Haste_MODE"), 0, szFullPathName);//0.32�Œǉ�
																								   //g_HasteSetting.mp = GetPrivateProfileInt(szSection,_T("Haste_MP"),0,szFullPathName);//0.39�Œǉ�
		g_HasteSetting.mp = (DWORD)GetPrivateProfileInt(szSection, _T("Haste_MP"), 0, szFullPathName);//0.40�ŕύX
		for (int i = 0; i<9; i++) {//���ւ��p 0.19�Œǉ� 0.20��AFTER��ǉ�
			_stprintf(szBuf, _T("Haste_EQUIP%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_HasteSetting.szEquip[i], sizeof(g_HasteSetting.szEquip[i]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("Haste_EQUIP_AFTER%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_HasteSetting.szEquipAfter[i], sizeof(g_HasteSetting.szEquipAfter[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}

		//0.41�Œǉ�
		GetPrivateProfileString(szSection, _T("Refresh_HEADER"), _T("Refresh"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		SendMessage(hWnd, WM_CHANGE_HEADER_NAME, (WPARAM)szBuf, (LPARAM)2);
		GetPrivateProfileString(szSection, _T("Refresh_CMD"), NULL, g_RefreshSetting.szCmd, sizeof(g_RefreshSetting.szCmd) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(szSection, _T("Refresh_PREJA"), NULL, g_RefreshSetting.szPreJa, sizeof(g_RefreshSetting.szPreJa) / sizeof(_TCHAR) - 1, szFullPathName);//0.33�Œǉ�
		g_RefreshSetting.recast_no = GetPrivateProfileInt(szSection, _T("Refresh_RECAST_NO"), 0, szFullPathName);
		g_RefreshSetting.time = GetPrivateProfileInt(szSection, _T("Refresh_TIME"), 0, szFullPathName);
		g_RefreshSetting.mode = GetPrivateProfileInt(szSection, _T("Refresh_MODE"), 0, szFullPathName);//0.32�Œǉ�
																									   //g_RefreshSetting.mp = GetPrivateProfileInt(szSection,_T("Refresh_MP"),0,szFullPathName);//0.39�Œǉ�
		g_RefreshSetting.mp = (DWORD)GetPrivateProfileInt(szSection, _T("Refresh_MP"), 0, szFullPathName);//0.40�ŕύX
		for (int i = 0; i<9; i++) {//���ւ��p 0.19�Œǉ� 0.20��AFTER��ǉ�
			_stprintf(szBuf, _T("Refresh_EQUIP%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_RefreshSetting.szEquip[i], sizeof(g_RefreshSetting.szEquip[i]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("Refresh_EQUIP_AFTER%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_RefreshSetting.szEquipAfter[i], sizeof(g_RefreshSetting.szEquipAfter[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}

		//0.41�Œǉ�
		GetPrivateProfileString(szSection, _T("User_HEADER"), _T("User"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		SendMessage(hWnd, WM_CHANGE_HEADER_NAME, (WPARAM)szBuf, (LPARAM)3);
		//0.22�ŕύX
		GetPrivateProfileString(szSection, _T("User_CMD"), NULL, g_UserSetting[0].szCmd, sizeof(g_UserSetting[0].szCmd) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(szSection, _T("User_PREJA"), NULL, g_UserSetting[0].szPreJa, sizeof(g_UserSetting[0].szPreJa) / sizeof(_TCHAR) - 1, szFullPathName);//0.31�Œǉ�
		g_UserSetting[0].recast_no = GetPrivateProfileInt(szSection, _T("User_RECAST_NO"), 0, szFullPathName);
		g_UserSetting[0].time = GetPrivateProfileInt(szSection, _T("User_TIME"), 0, szFullPathName);
		g_UserSetting[0].mode = GetPrivateProfileInt(szSection, _T("User_MODE"), 0, szFullPathName);//0.32�Œǉ�
																									//g_UserSetting[0].mp = GetPrivateProfileInt(szSection,_T("User_MP"),0,szFullPathName);//0.39�Œǉ�
		g_UserSetting[0].mp = (DWORD)GetPrivateProfileInt(szSection, _T("User_MP"), 0, szFullPathName);//0.40�ŕύX
		for (int i = 0; i<9; i++) {//���ւ��p 0.19�Œǉ� 0.20��AFTER��ǉ�
			_stprintf(szBuf, _T("User_EQUIP%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[0].szEquip[i], sizeof(g_UserSetting[0].szEquip[i]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("User_EQUIP_AFTER%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[0].szEquipAfter[i], sizeof(g_UserSetting[0].szEquipAfter[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}
		//0.41�Œǉ�
		GetPrivateProfileString(szSection, _T("User2_HEADER"), _T("User2"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		SendMessage(hWnd, WM_CHANGE_HEADER_NAME, (WPARAM)szBuf, (LPARAM)4);
		GetPrivateProfileString(szSection, _T("User2_CMD"), NULL, g_UserSetting[1].szCmd, sizeof(g_UserSetting[1].szCmd) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(szSection, _T("User2_PREJA"), NULL, g_UserSetting[1].szPreJa, sizeof(g_UserSetting[1].szPreJa) / sizeof(_TCHAR) - 1, szFullPathName);//0.31�Œǉ�
		g_UserSetting[1].recast_no = GetPrivateProfileInt(szSection, _T("User2_RECAST_NO"), 0, szFullPathName);
		g_UserSetting[1].time = GetPrivateProfileInt(szSection, _T("User2_TIME"), 0, szFullPathName);
		g_UserSetting[1].mode = GetPrivateProfileInt(szSection, _T("User2_MODE"), 0, szFullPathName);//0.32�Œǉ�
																									 //g_UserSetting[1].mp = GetPrivateProfileInt(szSection,_T("User2_MP"),0,szFullPathName);//0.39�Œǉ�
		g_UserSetting[1].mp = (DWORD)GetPrivateProfileInt(szSection, _T("User2_MP"), 0, szFullPathName);//0.40�ŕύX
		for (int i = 0; i<9; i++) {
			_stprintf(szBuf, _T("User2_EQUIP%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[1].szEquip[i], sizeof(g_UserSetting[1].szEquip[i]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("User2_EQUIP_AFTER%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[1].szEquipAfter[i], sizeof(g_UserSetting[1].szEquipAfter[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}
		//0.41�Œǉ�
		GetPrivateProfileString(szSection, _T("User3_HEADER"), _T("User3"), szBuf, sizeof(szBuf) / sizeof(_TCHAR), szFullPathName);
		SendMessage(hWnd, WM_CHANGE_HEADER_NAME, (WPARAM)szBuf, (LPARAM)5);
		GetPrivateProfileString(szSection, _T("User3_CMD"), NULL, g_UserSetting[2].szCmd, sizeof(g_UserSetting[2].szCmd) / sizeof(_TCHAR) - 1, szFullPathName);
		GetPrivateProfileString(szSection, _T("User3_PREJA"), NULL, g_UserSetting[2].szPreJa, sizeof(g_UserSetting[2].szPreJa) / sizeof(_TCHAR) - 1, szFullPathName);//0.31�Œǉ�
		g_UserSetting[2].recast_no = GetPrivateProfileInt(szSection, _T("User3_RECAST_NO"), 0, szFullPathName);
		g_UserSetting[2].time = GetPrivateProfileInt(szSection, _T("User3_TIME"), 0, szFullPathName);
		g_UserSetting[2].mode = GetPrivateProfileInt(szSection, _T("User3_MODE"), 0, szFullPathName);//0.32�Œǉ�
																									 //g_UserSetting[2].mp = GetPrivateProfileInt(szSection,_T("User3_MP"),0,szFullPathName);//0.39�Œǉ�
		g_UserSetting[2].mp = (DWORD)GetPrivateProfileInt(szSection, _T("User3_MP"), 0, szFullPathName);//0.40�ŕύX
		for (int i = 0; i<9; i++) {
			_stprintf(szBuf, _T("User3_EQUIP%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[2].szEquip[i], sizeof(g_UserSetting[2].szEquip[i]) / sizeof(_TCHAR) - 1, szFullPathName);
			_stprintf(szBuf, _T("User3_EQUIP_AFTER%d"), i + 1);
			GetPrivateProfileString(szSection, szBuf, NULL, g_UserSetting[2].szEquipAfter[i], sizeof(g_UserSetting[2].szEquipAfter[i]) / sizeof(_TCHAR) - 1, szFullPathName);
		}

#ifndef UPLOAD//0.26�Œǉ�
		g_SkillUpMode = GetPrivateProfileInt(szSection, _T("SKILL_UP_MODE"), 0, szFullPathName);
		if (g_SkillUpMode)MessageBox(hWnd, _T("SKILL UP MODE"), HAKOSIRO, MB_OK);
#endif
	}

	for (int i = 0; i<6; i++) {
		if (wcsncmp(g_CureSetting[i].szCmd, _T("/sh"), 3) == 0)return -1;
		if (wcsncmp(g_CureSetting[i].szCmd, _T("/say"), 4) == 0)return -1;
		if (wcsncmp(g_CureSetting[i].szCmd, _T("/s "), 3) == 0)return -1;
		if (wcsncmp(g_CureSetting[i].szCmd, _T("/y"), 2) == 0)return -1;
	}
	for (int i = 0; i<6; i++) {//0.19�Œǉ� 0.20�ŕύX
		for (int j = 0; j<9; j++) {
			if (_tcslen(g_CureSetting[i].szEquip[j]) && wcsncmp(g_CureSetting[i].szEquip[j], _T("/equip "), 7))return -1;
			if (_tcslen(g_CureSetting[i].szEquipAfter[j]) && wcsncmp(g_CureSetting[i].szEquipAfter[j], _T("/equip "), 7))return -1;
			if (_tcslen(g_HasteSetting.szEquip[j]) && wcsncmp(g_HasteSetting.szEquip[j], _T("/equip "), 7))return -1;
			if (_tcslen(g_HasteSetting.szEquipAfter[j]) && wcsncmp(g_HasteSetting.szEquipAfter[j], _T("/equip "), 7))return -1;
			if (_tcslen(g_RefreshSetting.szEquip[j]) && wcsncmp(g_RefreshSetting.szEquip[j], _T("/equip "), 7))return -1;
			if (_tcslen(g_RefreshSetting.szEquipAfter[j]) && wcsncmp(g_RefreshSetting.szEquipAfter[j], _T("/equip "), 7))return -1;
			//0.22�ŕύX
			for (int k = 0; k <= 2; k++) {
				if (_tcslen(g_UserSetting[k].szEquip[j]) && wcsncmp(g_UserSetting[k].szEquip[j], _T("/equip "), 7))return -1;
				if (_tcslen(g_UserSetting[k].szEquipAfter[j]) && wcsncmp(g_UserSetting[k].szEquipAfter[j], _T("/equip "), 7))return -1;
			}
			//
		}
	}
	if (wcsncmp(g_HasteSetting.szCmd, _T("/sh"), 3) == 0)return -1;
	if (wcsncmp(g_HasteSetting.szCmd, _T("/say"), 4) == 0)return -1;
	if (wcsncmp(g_HasteSetting.szCmd, _T("/s "), 3) == 0)return -1;
	if (wcsncmp(g_HasteSetting.szCmd, _T("/y"), 2) == 0)return -1;
	if (wcsncmp(g_RefreshSetting.szCmd, _T("/sh"), 3) == 0)return -1;
	if (wcsncmp(g_RefreshSetting.szCmd, _T("/say"), 4) == 0)return -1;
	if (wcsncmp(g_RefreshSetting.szCmd, _T("/s "), 3) == 0)return -1;
	if (wcsncmp(g_RefreshSetting.szCmd, _T("/y"), 2) == 0)return -1;
	//0.22�ŕύX
	for (int i = 0; i <= 2; i++) {
		if (wcsncmp(g_UserSetting[i].szCmd, _T("/sh"), 3) == 0)return -1;
		if (wcsncmp(g_UserSetting[i].szCmd, _T("/say"), 4) == 0)return -1;
		if (wcsncmp(g_UserSetting[i].szCmd, _T("/s "), 3) == 0)return -1;
		if (wcsncmp(g_UserSetting[i].szCmd, _T("/y"), 2) == 0)return -1;

		if (wcsncmp(g_UserSetting[i].szPreJa, _T("/sh"), 3) == 0)return -1;//0.31�Œǉ�
		if (wcsncmp(g_UserSetting[i].szPreJa, _T("/say"), 4) == 0)return -1;
		if (wcsncmp(g_UserSetting[i].szPreJa, _T("/s "), 3) == 0)return -1;
		if (wcsncmp(g_UserSetting[i].szPreJa, _T("/y"), 2) == 0)return -1;
	}

	return 0;
}

int MakeComboFromSection(HWND hWnd)
{
	_TCHAR szText[MAX_PATH];//0.33�Œǉ�
	int nSelectedRow = 0;//0.33�Œǉ�

	GetDlgItemText(hWnd, IDC_COMBO_JOB, szText, sizeof(szText) / sizeof(_TCHAR));//�I������Ă��镶������擾 0.33�Œǉ�

																				 //�R���{�{�b�N�X��������
	while (SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_GETCOUNT, 0, 0) != 0) {
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_DELETESTRING, 0, 0);
	}

	//�����񂪂���΃R���{�{�b�N�X�ɒǉ�
	for (int i = 0, nRow = 0; i<20; i++) {//0.26�ŕύX 0.33�ŕύX
		if (_tcslen(g_SectionName[i]) != 0) {
			SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_ADDSTRING, 0, (LPARAM)g_SectionName[i]);
			if (_tcscmp (g_SectionName[i], szText) == 0) {//0.33�Œǉ�
				nSelectedRow = nRow;
			}
			nRow++;//0.33�Œǉ�
		}
	}

	//�I������Ă����s���Z�b�g
	SendMessage(GetDlgItem(hWnd, IDC_COMBO_JOB), CB_SETCURSEL, nSelectedRow, 0);

	return 0;
}


//���O�̃E�B���h�v���V�[�W�� 0.10�Œǉ�
LRESULT CALLBACK WndProcLog(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static int line = 0;
	static int line_party, line_linkshell;//0.30�Œǉ�
	static int cast_line = 0;
	static BYTE byCastMagic = 0;
	static HMENU hMenu, hSubMenu;

	switch (msg) {
	case WM_INITDIALOG: {
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_LOG),
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
		);
		SetWindowLong(GetDlgItem(hWnd, IDC_LIST_LOG), GWL_WNDPROC, (LONG)WndProcSubClass_Log);//0.23�Œǉ�

																							  //0.30�Œǉ�
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY),
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
		);
		SetWindowLong(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), GWL_WNDPROC, (LONG)WndProcSubClass_Log);

		//0.30�Œǉ�
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL),
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
		);
		SetWindowLong(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), GWL_WNDPROC, (LONG)WndProcSubClass_Log);

		//�J�����쐬
		LVCOLUMN col;
		ZeroMemory(&col, sizeof(col));
		const _TCHAR szColumn[][0x10] = { _T("attr"),_T("Time"),_T("Log"),_T("RecastNo") };
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
		col.fmt = LVCFMT_LEFT;
		for (int i = 0; i<sizeof(szColumn) / sizeof(szColumn[0]); i++) {
			col.cx = g_ColumnCxLog[i];
			col.iSubItem = i;
			col.pszText = (_TCHAR*)szColumn[i];
			if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_LOG), i, &col)) {
				return 0;
			}
			if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), i, &col)) {//�p�[�e�B�p 0.30�Œǉ�
				return 0;
			}
			if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), i, &col)) {//�����N�V�F���p 0.30�Œǉ�
				return 0;
			}
		}

		MoveWindow(hWnd, g_Log.x, g_Log.y, g_Log.cx, g_Log.cy, TRUE);
		PostMessage(hWnd, WM_SIZE, NULL, NULL);
		//������
		line = 0;
		line_party = 0;
		line_linkshell = 0;
		cast_line = 0;
		BYTE byCastMagic = 0;
		//�o�b�t�@�Ɏc���Ă郍�O�����폜 0.10�Œǉ�
		BYTE attr;
		char buf[MAX_PATH];
		while (ffhook_getlog(&attr, buf, sizeof(buf))) {
			Sleep(1);
		};

		//���j���[�쐬 0.30�Œǉ�
		hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_DIALOG));
		hSubMenu = GetSubMenu(hMenu, 2);
		SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_LOG_ALL, 0);

		//�^�C�}�[�J�n
		SetTimer(hWnd, TIMER_ID_LOG, 1000, NULL);
		break; }

	case WM_COMMAND:
		//0.30�Œǉ�
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;

		switch (LOWORD(wp)) {
		case IDM_LOG_ALL:
			mii.fState = MFS_CHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_ALL, FALSE, &mii);
			mii.fState = MFS_UNCHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_PARTY, FALSE, &mii);
			SetMenuItemInfo(hSubMenu, IDM_LOG_LINKSHELL, FALSE, &mii);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG), true);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), false);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), false);
			SetWindowText(hWnd, _T("Log"));
			break;
		case IDM_LOG_PARTY:
			mii.fState = MFS_CHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_PARTY, FALSE, &mii);
			mii.fState = MFS_UNCHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_ALL, FALSE, &mii);
			SetMenuItemInfo(hSubMenu, IDM_LOG_LINKSHELL, FALSE, &mii);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG), false);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), true);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), false);
			SetWindowText(hWnd, _T("Log - �p�[�e�B��b"));
			break;
		case IDM_LOG_LINKSHELL:
			mii.fState = MFS_CHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_LINKSHELL, FALSE, &mii);
			mii.fState = MFS_UNCHECKED;
			SetMenuItemInfo(hSubMenu, IDM_LOG_ALL, FALSE, &mii);
			SetMenuItemInfo(hSubMenu, IDM_LOG_PARTY, FALSE, &mii);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG), false);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), false);
			ShowWindow(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), true);
			SetWindowText(hWnd, _T("Log - �����N�V�F����b"));
			break;
		}
		break;

	case WM_TIMER: {
		KillTimer(hWnd, TIMER_ID_LOG);
		BYTE attr;
		char szGetLog[0x100];
		LVITEMA item;
		ZeroMemory(&item, sizeof(item));
		if (byCastMagic == 1 && Casting() <= 0) {//�r������
			byCastMagic = 0;
			char szRecastNo[0x10];
			int RecastNo = SearchRecastNo_Log(1);//�ۑ��������Ɣ�r���ăT�[�`
												 //���L���X�g�ԍ����L��
			item.mask = LVIF_TEXT;
			item.iItem = cast_line;//�s
			item.iSubItem = COLUMN_RECASTNO;//��
			item.pszText = szRecastNo;
			if (RecastNo == -1) {
				_stprintf(szRecastNo, _T("-"));//0.43i�ŕύX
			}
			else {
				_stprintf(szRecastNo, _T("%d"), RecastNo, RecastNo);//0.43i�ŕύX
			}
			SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG), LVM_SETITEMA, NULL, (LPARAM)&item);//mbs�œ]������̂�SendMessage���g��
		}
		if (ffhook_getlog(&attr, szGetLog, sizeof(szGetLog))) {
			char szAttr[0x10];
			_stprintf(szAttr, _T("%02X"), attr);//0.43i�ŕύX
			item.mask = LVIF_TEXT;
			item.iItem = line;//�s
			item.iSubItem = COLUMN_ATTR;//��
			item.pszText = szAttr;
			SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG), LVM_INSERTITEMA, NULL, (LPARAM)&item);//mbs�œ]������̂�SendMessage���g��
			if (GetFocus() != GetDlgItem(hWnd, IDC_LIST_LOG))ListView_EnsureVisible(GetDlgItem(hWnd, IDC_LIST_LOG), line, FALSE);//���C�����ړ�
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);
			char szTime[0x10];
			_stprintf(szTime, _T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);//0.43i�ŕύX
			item.iSubItem = COLUMN_TIME;//��
			item.pszText = szTime;
			SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG), LVM_SETITEMA, NULL, (LPARAM)&item);//mbs�œ]������̂�SendMessage���g��
			item.iSubItem = COLUMN_LOG;//��
			item.pszText = szGetLog;
			SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG), LVM_SETITEMA, NULL, (LPARAM)&item);//mbs�œ]������̂�SendMessage���g��
			if (byCastMagic == 0 && (attr == 0x34 || attr == 0x32)) {//���@�r���J�n
				byCastMagic = 1;
				cast_line = line;
				SearchRecastNo_Log(0);//���O�Ƀ��L���X�g����ۑ�
			}
			line++;

			//�p�[�e�B��b 0.30�Œǉ�
			if (attr == 0x05 || attr == 0x0D) {
				item.iItem = line_party;//�s
				item.iSubItem = COLUMN_ATTR;//��
				item.pszText = szAttr;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), LVM_INSERTITEMA, NULL, (LPARAM)&item);
				if (GetFocus() != GetDlgItem(hWnd, IDC_LIST_LOG_PARTY))ListView_EnsureVisible(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), line_party, FALSE);
				item.iSubItem = COLUMN_TIME;//��
				item.pszText = szTime;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), LVM_SETITEMA, NULL, (LPARAM)&item);
				item.iSubItem = COLUMN_LOG;//��
				item.pszText = szGetLog;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), LVM_SETITEMA, NULL, (LPARAM)&item);
				line_party++;
			}
			//�����N�V�F����b 0.30�Œǉ�
			if (attr == 0x06 || attr == 0x0E) {
				item.iItem = line_linkshell;//�s
				item.iSubItem = COLUMN_ATTR;//��
				item.pszText = szAttr;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), LVM_INSERTITEMA, NULL, (LPARAM)&item);
				if (GetFocus() != GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL))ListView_EnsureVisible(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), line_linkshell, FALSE);
				item.iSubItem = COLUMN_TIME;//��
				item.pszText = szTime;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), LVM_SETITEMA, NULL, (LPARAM)&item);
				item.iSubItem = COLUMN_LOG;//��
				item.pszText = szGetLog;
				SendMessage(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), LVM_SETITEMA, NULL, (LPARAM)&item);
				line_linkshell++;
			}

		}
		SetTimer(hWnd, TIMER_ID_LOG, 10, NULL);
		break; }

	case WM_RBUTTONUP://0.30�Œǉ�
					  //���j���[��\��
		POINT pos;
		pos.x = LOWORD(lp);
		pos.y = HIWORD(lp);
		if (0 == ClientToScreen(hWnd, &pos)) {
		}
		SetForegroundWindow(hWnd);
		if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
		}
		PostMessage(hWnd, 0, 0, 0);
		break;

	case WM_NOTIFY://�J�������A�� 0.30�Œǉ�
		switch (((NMHDR*)lp)->code) {
		case HDN_ENDTRACK:
			HDITEM hdi;
			ZeroMemory(&hdi, sizeof(HDITEM));
			hdi.mask = HDI_WIDTH;
			hdi.cxy = ((NMHEADER*)lp)->pitem->cxy;
			g_ColumnCxLog[((NMHEADER*)lp)->iItem] = ((NMHEADER*)lp)->pitem->cxy;//0.34�Œǉ�
			Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST_LOG)), ((NMHEADER*)lp)->iItem, &hdi);
			Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY)), ((NMHEADER*)lp)->iItem, &hdi);
			Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL)), ((NMHEADER*)lp)->iItem, &hdi);
			break;
		}
		break;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		MoveWindow(GetDlgItem(hWnd, IDC_LIST_LOG), 1, 1, rect.right - 2, rect.bottom - 2, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_LIST_LOG_PARTY), 1, 1, rect.right - 2, rect.bottom - 2, 1);//0.30�Œǉ�
		MoveWindow(GetDlgItem(hWnd, IDC_LIST_LOG_LINKSHELL), 1, 1, rect.right - 2, rect.bottom - 2, 1);//0.30�Œǉ�
		break;

	case WM_CLOSE:
		//0.17�Œǉ�
		for (int i = 0; i<sizeof(g_ColumnCxLog) / sizeof(g_ColumnCxLog[0]); i++) {//MAXHP�͓Ǎ��܂Ȃ�
			g_ColumnCxLog[i] = ListView_GetColumnWidth(GetDlgItem(hWnd, IDC_LIST_LOG), i);
		}
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_LOG_OPEN, 1);
		break;
	}

	return 0;
}

LRESULT CALLBACK WndProcFollow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static int nStart = 0;
	static CKeyboardHelper* pKeyboardHelper;
	static HMODULE hModule;

	switch (msg) {
	case WM_INITDIALOG:
		if (g_UseWindowerHelper && (hModule = LoadLibrary(_T("WindowerHelper.dll"))) != NULL) {//0.1.1.0�Œǉ�
			if ((DLL_CreateKeyboardHelper = (void* (__stdcall*)(char*))GetProcAddress(hModule, "CreateKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find CreateKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_DeleteKeyboardHelper = (void(__stdcall*)(void*))GetProcAddress(hModule, "DeleteKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find DeleteKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_CKHSetKey = (void(__stdcall*)(void*, unsigned char, bool))GetProcAddress(hModule, "CKHSetKey")) == NULL) {
				MessageBox(hWnd, _T("Cant find CKHSetKey()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			SetWindowText(hWnd, _T("Follow - WindowerHelper.dll"));
			char szHandler[MAX_PATH];
			_stprintf(szHandler, _T("WindowerMMFKeyboardHandler_%d"), g_ProcessID);//0.43i�ŕύX
			pKeyboardHelper = (CKeyboardHelper*)DLL_CreateKeyboardHelper(szHandler);//0.1.1.0�Œǉ�
		}
		else if (g_UseWindowerHelper) {
			MessageBox(hWnd, _T("Cant find WindowerHelper.dll"), NULL, MB_ICONERROR);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			return 0;
		}

		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("1Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("3Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("5Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("10Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("15Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("20Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("25Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_ADDSTRING, 0, (LPARAM)_T("30Ym"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), CB_SETCURSEL, 0, 0);
		SetWindowPos(hWnd, 0, g_Follow.x, g_Follow.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		SetDlgItemText(hWnd, IDC_COMBO_FNAME, g_DefaultFollower);//0.23�Œǉ�
																 //�^�C�}�[�J�n
		SetTimer(hWnd, TIMER_ID_FOLLOW, 1000, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_COMBO_FNAME:
			PARTYINFORMATION pInfo;
			if (HIWORD(wp) == CBN_DROPDOWN) {
				while (SendMessage(GetDlgItem(hWnd, IDC_COMBO_FNAME), CB_GETCOUNT, 0, 0) != 0) {//�R���{�{�b�N�X��������
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_FNAME), CB_DELETESTRING, 0, 0);
				}
				for (int i = 0x400; i<0x700; i++) {
					pInfo = GetNPCInfo(i);
					if (pInfo.inrange && _tcslen((const _TCHAR*)pInfo.name) >= 3 && pInfo.inrange) {//0.12a�ŕύX
						SendMessage(GetDlgItem(hWnd, IDC_COMBO_FNAME), CB_ADDSTRING, 0, (LPARAM)pInfo.name);
					}
				}
			}
			break;

		case IDC_BUTTON_FSTART:
			if (nStart == 0) {
				SetDlgItemText(hWnd, IDC_BUTTON_FSTART, _T("��~"));
				nStart = 1;
			}
			else {
				SetDlgItemText(hWnd, IDC_BUTTON_FSTART, _T("�J�n"));
				nStart = 0;
			}
		}
		break;

	case WM_TIMER:
		KillTimer(hWnd, TIMER_ID_FOLLOW);
		if (nStart) {
			_TCHAR szText[MAX_PATH];
			_TCHAR szTargetName[0x20];
			GetWindowText(GetDlgItem(hWnd, IDC_COMBO_FDISTANCE), szText, MAX_PATH - 1);
			int nCmpDistance = _wtoi(szText);
			if (nCmpDistance == 0)nCmpDistance = 1;

			GetWindowText(GetDlgItem(hWnd, IDC_COMBO_FNAME), szTargetName, 0x20 - 1);
			if (_tcslen(szTargetName)) {
				//(�r������Follow���Ȃ��Ƀ`�F�b�N���Ă��� && �r�����ł͂Ȃ�) || �r������Follow���Ȃ��Ƀ`�F�b�N���Ă��Ȃ�
				if ((IsDlgButtonChecked(hWnd, IDC_CHECKBOX_CASTING) && Casting() <= 0) || IsDlgButtonChecked(hWnd, IDC_CHECKBOX_CASTING) == 0) {//0.1.1.0�Œǉ�
																																				//if(g_UseWindowerHelper){//0.28�ŕύX
					if (g_UseWindowerHelper && pKeyboardHelper) {
						Follow_Helper(szTargetName, nCmpDistance, pKeyboardHelper, hWnd);//0.29�ŕύX
					}
					else {
						Follow(szTargetName, nCmpDistance, hWnd);//0.29�ŕύX
					}
				}
			}
		}
		SetTimer(hWnd, TIMER_ID_FOLLOW, 100, NULL);
		break;

	case WM_CLOSE:
		if (g_UseWindowerHelper && pKeyboardHelper)DLL_DeleteKeyboardHelper(pKeyboardHelper);//0.1.1.0�Œǉ�
		FreeLibrary(hModule);
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_FOLLOW_OPEN, 1);
		break;
	}

	return 0;
}


//���b�N�I���̏�Ԃ�����ׂ� 0.29�Œǉ�
BYTE GetLockOn()
{
	DWORD dwAddress;
	BYTE byRet;

	dwAddress = g_DllAddress + g_Offset.Target;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		return -1;
	}
	//���b�N�I������0x29�o�C�g��
	//���b�N�I������0x31�o�C�g�� //0.50e�ŕύX
	//���b�N�I������0x51�o�C�g�� //0.50f�ŕύX
	//0:���b�N�I�����Ă��Ȃ� 1:���b�N�I�����Ă���
	dwAddress += 0x51;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &byRet, sizeof(byRet), NULL)) {
		return -1;
	}

	return byRet;
}

//�T�u�^�[�Q�b�g���o�Ă��邩������ׂ� 0.30�Œǉ�
BYTE GetSubTarget()
{
	DWORD dwAddress;
	BYTE byRet;

	dwAddress = g_DllAddress + g_Offset.Target;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		return -1;
	}
	//�T�u�^�[�Q�b�g����0x2B�o�C�g�� 0:�T�u�^�[�Q�b�g���o�Ă��Ȃ� 1:�o�Ă���
	dwAddress += 0x2B;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &byRet, sizeof(byRet), NULL)) {
		return -1;
	}

	return byRet;
}


//���݂̎��_���
int GetViewPoint()
{
	DWORD dwAddress;
	BYTE byRet;

	//���������[�h
	dwAddress = g_DllAddress + g_Offset.ViewPoint;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &byRet, sizeof(byRet), NULL)) {
		return -1;
	}

	return byRet;
}

void Follow(_TCHAR *szName, int nCmpDistance, HWND hWnd)
{
	PARTYINFORMATION pInfo;
	PARTYINFORMATION myInfo;
	INPUT InKey[10];
	static BYTE byNowKeyDown[10];

#ifdef _DEBUG
	int i = 0x000;
#else
	int i = 0x400;
#endif
	for (i; i < 0x700; i++) {
		pInfo = GetNPCInfo(i);
		if (_tcscmp (pInfo.name, szName) == 0) {
			break;
		}
	}
	if (_tcscmp (pInfo.name, szName) != 0)return;

	myInfo = GetPartyInfo(0);

	int nDistance = (int)sqrt(pow(myInfo.x - pInfo.x, 2) + pow(myInfo.z - pInfo.z, 2));

	if (nDistance >= nCmpDistance && GetForegroundWindow() == FindWindow(_T("FFXiClass"), NULL) && GetViewPoint() == 1) {
		//0.29�Œǉ�
		if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_LOCKON), BM_GETCHECK, 0, 0)) {
			_TCHAR szTargetCmd[MAX_PATH];
			if (GetSubTarget() == 0 && _tcscmp (szName, GetNPCInfo(GetTargetID()).name) != 0) {
				//�t�H���[�ΏۈȊO�����b�N�I������Ă��鎞 0.30�ŃT�u�^�[�Q�b�g������
				_stprintf(szTargetCmd, _T("/ta %s"), szName);
				ffhook_putcmd(szTargetCmd);
			}
			if (GetSubTarget() == 0 && GetLockOn() == 0) {//0.30�ŃT�u�^�[�Q�b�g������
				ffhook_putcmd(_T("/lockon"));
			}
		}

		//5�ŉ�ʂ̐؂�ւ����s���Ă�����
		//4��6�Ŋp�x��+-30�x�ɂ��킹��
		double dKakudo = 180 / 3.141592*(atan2(pInfo.z - myInfo.z, pInfo.x - myInfo.x) + myInfo.radian);
		if (dKakudo < 0)dKakudo += 360;

		if (dKakudo <= 330 && dKakudo >= 180) {//�E�ɉ�]
			InKey[0].type = INPUT_KEYBOARD;
			InKey[0].ki.wVk = VK_NUMPAD6;
			InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD6, 0);
			InKey[0].ki.dwFlags = 0;
			byNowKeyDown[6] = 1;
			SendInput(1, InKey, sizeof(INPUT));
		}
		else if (dKakudo >= 30 && dKakudo <= 180) {//���ɉ�]
			InKey[0].type = INPUT_KEYBOARD;
			InKey[0].ki.wVk = VK_NUMPAD4;
			InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD4, 0);
			InKey[0].ki.dwFlags = 0;
			byNowKeyDown[4] = 1;
			SendInput(1, InKey, sizeof(INPUT));
		}
		else {//8�őO�i
			int i = 0;
			//15�x�ȏ�̎���]
			if (byNowKeyDown[6] == 0 && dKakudo <= 345 && dKakudo >= 180) {//�E�ɉ�]
				InKey[i].type = INPUT_KEYBOARD;
				InKey[i].ki.wVk = VK_NUMPAD6;
				InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD6, 0);
				InKey[i].ki.dwFlags = 0;
				byNowKeyDown[6] = 1;
				i++;
			}
			else {
				InKey[i].type = INPUT_KEYBOARD;
				InKey[i].ki.wVk = VK_NUMPAD6;
				InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD6, 0);
				InKey[i].ki.dwFlags = KEYEVENTF_KEYUP;
				byNowKeyDown[6] = 0;
				i++;
			}
			if (byNowKeyDown[4] == 0 && dKakudo >= 15 && dKakudo <= 180) {//���ɉ�]
				InKey[i].type = INPUT_KEYBOARD;
				InKey[i].ki.wVk = VK_NUMPAD4;
				InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD4, 0);
				InKey[i].ki.dwFlags = 0;
				byNowKeyDown[4] = 1;
				i++;
			}
			else {
				InKey[i].type = INPUT_KEYBOARD;
				InKey[i].ki.wVk = VK_NUMPAD4;
				InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD4, 0);
				InKey[i].ki.dwFlags = KEYEVENTF_KEYUP;
				byNowKeyDown[4] = 0;
				i++;
			}
			//���i
			InKey[i].type = INPUT_KEYBOARD;
			InKey[i].ki.wVk = VK_NUMPAD8;
			InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD8, 0);
			InKey[i].ki.dwFlags = 0;
			byNowKeyDown[8] = 1;
			SendInput(++i, InKey, sizeof(INPUT));
		}
	}
	else {
		int i = 0;
		//�L�[��������Ă��������
		if (byNowKeyDown[6] == 1) {
			InKey[i].type = INPUT_KEYBOARD;
			InKey[i].ki.wVk = VK_NUMPAD6;
			InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD6, 0);
			InKey[i].ki.dwFlags = KEYEVENTF_KEYUP;
			byNowKeyDown[6] = 0;
			i++;
		}
		if (byNowKeyDown[4] == 1) {
			InKey[i].type = INPUT_KEYBOARD;
			InKey[i].ki.wVk = VK_NUMPAD4;
			InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD4, 0);
			InKey[i].ki.dwFlags = KEYEVENTF_KEYUP;
			byNowKeyDown[4] = 0;
			i++;
		}
		if (byNowKeyDown[8] == 1) {
			InKey[i].type = INPUT_KEYBOARD;
			InKey[i].ki.wVk = VK_NUMPAD8;
			InKey[i].ki.wScan = MapVirtualKey(VK_NUMPAD8, 0);
			InKey[i].ki.dwFlags = KEYEVENTF_KEYUP;
			byNowKeyDown[8] = 0;
			i++;
		}
		if (i) {
			SendInput(i, InKey, sizeof(INPUT));
		}
	}
}

void Follow_Helper(_TCHAR* szName, int nCmpDistance, CKeyboardHelper* pKeyHelper, HWND hWnd)
{
	PARTYINFORMATION pInfo;
	PARTYINFORMATION myInfo;
	static BYTE byNowKeyDown[10];

#ifdef _DEBUG
	int i = 0x000;
#else
	int i = 0x400;
#endif
	for (i; i < 0x700; i++) {
		pInfo = GetNPCInfo(i);
		if (_tcscmp (pInfo.name, szName) == 0) {
			break;
		}
	}
	if (_tcscmp (pInfo.name, szName) != 0)return;

	myInfo = GetPartyInfo(0);

	int nDistance = (int)sqrt(pow(myInfo.x - pInfo.x, 2) + pow(myInfo.z - pInfo.z, 2));

	if (nDistance >= nCmpDistance && GetViewPoint() == 1) {
		if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_LOCKON), BM_GETCHECK, 0, 0)) {
			_TCHAR szTargetCmd[MAX_PATH];
			if (GetSubTarget() == 0 && _tcscmp (szName, GetNPCInfo(GetTargetID()).name) != 0) {
				//�t�H���[�ΏۈȊO�����b�N�I������Ă��鎞 0.30�ŃT�u�^�[�Q�b�g������
				_stprintf(szTargetCmd, _T("/ta %s"), szName);
				ffhook_putcmd(szTargetCmd);
			}

			if (GetSubTarget() == 0 && GetLockOn() == 0) {//0.30�ŃT�u�^�[�Q�b�g������
				ffhook_putcmd(_T("/lockon"));
			}
		}

		//5�ŉ�ʂ̐؂�ւ����s���Ă�����
		//4��6�Ŋp�x��+-30�x�ɂ��킹��
		double dKakudo = 180 / 3.141592 * (atan2(pInfo.z - myInfo.z, pInfo.x - myInfo.x) + myInfo.radian);
		if (dKakudo < 0)dKakudo += 360;

		if (dKakudo <= 330 && dKakudo >= 180) {//�E�ɉ�]
			byNowKeyDown[6] = 1;
			DLL_CKHSetKey(pKeyHelper, NPNUM6, true);
		}
		else if (dKakudo >= 30 && dKakudo <= 180) {//���ɉ�]
			byNowKeyDown[4] = 1;
			DLL_CKHSetKey(pKeyHelper, NPNUM4, true);
		}
		else {//8�őO�i
			int i = 0;
			//15�x�ȏ�̎���]
			if (byNowKeyDown[6] == 0 && dKakudo <= 345 && dKakudo >= 180) {//�E�ɉ�]
				DLL_CKHSetKey(pKeyHelper, NPNUM6, true);
				byNowKeyDown[6] = 1;
			}
			else {
				DLL_CKHSetKey(pKeyHelper, NPNUM6, false);
				byNowKeyDown[6] = 0;
			}
			if (byNowKeyDown[4] == 0 && dKakudo >= 15 && dKakudo <= 180) {//���ɉ�]
				DLL_CKHSetKey(pKeyHelper, NPNUM4, true);
				byNowKeyDown[4] = 1;
			}
			else {
				DLL_CKHSetKey(pKeyHelper, NPNUM4, false);
				byNowKeyDown[4] = 0;
			}
			//���i
			byNowKeyDown[8] = 1;
			DLL_CKHSetKey(pKeyHelper, NPNUM8, true);
		}
	}
	else {
		int i = 0;
		//�L�[��������Ă��������
		if (byNowKeyDown[6] == 1) {
			DLL_CKHSetKey(pKeyHelper, NPNUM6, false);
			byNowKeyDown[6] = 0;
		}
		if (byNowKeyDown[4] == 1) {
			DLL_CKHSetKey(pKeyHelper, NPNUM4, false);
			byNowKeyDown[4] = 0;
		}
		if (byNowKeyDown[8] == 1) {
			DLL_CKHSetKey(pKeyHelper, NPNUM8, false);
			byNowKeyDown[8] = 0;
		}
	}
}


//Reserved�̃E�B���h�v���V�[�W�� 0.12�Œǉ�
LRESULT CALLBACK WndProcReserved(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static int nStart = 0;
	static HWND hTool;//0.30�Œǉ�
	static HMENU hMenu, hSubMenu;

	static DWORD dwStartTime;
	static WORD stwAreaID;//0.42�Œǉ�

	switch (msg) {
	case WM_INITDIALOG: {
		//���X�g�r���[��EX�X�^�C���t�^
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_RESERVED),
			LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
			LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
		);
		//���X�g���T�u�N���X��
		SetWindowLong(GetDlgItem(hWnd, IDC_LIST_RESERVED), GWL_WNDPROC, (LONG)WndProcSubClass);//0.23�ŕύX
																							   //�J�����쐬
		LVCOLUMN col;
		ZeroMemory(&col, sizeof(col));

		//�J�����쐬
		_TCHAR szBuf[20];
		for (int i = 0; i < 14; i++) {
			col.mask = LVCF_TEXT;
			col.pszText = szBuf;
			col.cchTextMax = sizeof(szBuf) / sizeof(_TCHAR) - 1;
			if (i == 13) {
				col.pszText = _T("MaxHP");
			}
			else {
				ListView_GetColumn(GetDlgItem(GetParent(hWnd), IDC_LIST_PC), i, &col);
				col.pszText = szBuf;
			}
			col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			col.fmt = LVCFMT_RIGHT;
			col.cx = g_ColumnCx[i];
			col.iSubItem = i;
			if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, &col)) {
				return 0;
			}
		}

		//HP�R���{���X�g�쐬
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("50"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("100"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("200"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("300"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("400"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("500"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("600"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("700"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("800"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("900"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("1000"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("1200"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("1500"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("2000"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("2500"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), CB_ADDSTRING, 0, (LPARAM)_T("3000"));
		SetDlgItemText(hWnd, IDC_COMBO_RESERVED_CHARHP, _T("MaxHP"));
		//�E�B���h�E�T�C�Y�ʒu�̐ݒ�
		RECT rect, rect_list_pc;
		GetWindowRect(GetParent(hWnd), &rect);
		GetWindowRect(GetDlgItem(GetParent(hWnd), IDC_LIST_PC), &rect_list_pc);
		SetWindowPos(hWnd, 0, rect.left, rect_list_pc.top, rect.right - rect.left, rect_list_pc.bottom - rect_list_pc.top, SWP_SHOWWINDOW);
		PostMessage(hWnd, WM_SIZE, NULL, NULL);

		//���j���[�쐬 0.15�Œǉ�
		hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_DIALOG));
		hSubMenu = GetSubMenu(hMenu, 1);

		//�c�[���`�b�v 0.30�Œǉ�
		hTool = MakeToolTips(hWnd, (LONG)IDC_LIST_RESERVED);

		//�^�C�}�[�J�n
		SetTimer(hWnd, TIMER_ID_RESERVED, 1000, NULL);
		break; }

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_COMBO_RESERVED_CHARNAME:
			PARTYINFORMATION pInfo;
			if (HIWORD(wp) == CBN_DROPDOWN) {
				while (SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARNAME), CB_GETCOUNT, 0, 0) != 0) {//�R���{�{�b�N�X��������
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARNAME), CB_DELETESTRING, 0, 0);
				}
				for (int i = 0x400; i<0x700; i++) {
					pInfo = GetNPCInfo(i);
					if (pInfo.inrange && _tcslen((const _TCHAR*)pInfo.name) >= 3 && pInfo.inrange) {//0.12a�ŕύX
						SendMessage(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARNAME), CB_ADDSTRING, 0, (LPARAM)pInfo.name);
					}
				}
			}
			break;

		case IDC_BUTTON_RESERVED_ADD:
			_TCHAR szTname[0x20];
			_TCHAR szCmpTname[0x20];
			_TCHAR szPriority[0x10];
			_TCHAR szHp[0x10];
			GetDlgItemText(hWnd, IDC_COMBO_RESERVED_CHARNAME, szTname, sizeof(szTname) / sizeof(_TCHAR) - 1);
			GetDlgItemText(hWnd, IDC_COMBO_RESERVED_CHARHP, szHp, sizeof(szHp) / sizeof(_TCHAR) - 1);
			if (_tcslen(szTname) >= 3 && _wtoi(szHp) > 10) {
				int nItemCount = ListView_GetItemCount(GetDlgItem(hWnd, IDC_LIST_RESERVED));
				if (nItemCount > 18) {
					MessageBox(hWnd, _T("�o�^���̏���ɒB���Ă��܂�"), NULL, MB_ICONERROR);
					return 0;
				}
				for (int i = 0; i < nItemCount; i++) {
					ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_NAME, szCmpTname, sizeof(szCmpTname) / sizeof(_TCHAR) - 1);
					if (_tcscmp (szTname, szCmpTname) == 0) {
						ListView_SetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_MAXHP, szHp);
						return 0;
					}
				}
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = nItemCount;//�s
				item.iSubItem = 0;//��
				item.pszText = szTname;
				if (-1 == ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
					return 0;
				}
				item.iSubItem = COLUMN_PRIORITY;
				GetDlgItemText(GetParent(hWnd), IDC_COMBO_PRIORITY, szPriority, sizeof(szPriority) / sizeof(_TCHAR) - 1);
				item.pszText = szPriority;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
					return 0;
				}
				item.iSubItem = COLUMN_MAXHP;
				item.pszText = szHp;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
					return 0;
				}
			}
			else {
				MessageBox(hWnd, _T("�L�����N�^�[����HP��I�����Ă�������"), NULL, MB_ICONERROR);
			}
			break;

		case IDC_BUTTON_RESERVED_START:
			if (nStart == 0) {

				dwStartTime = timeGetTime();

				SetDlgItemText(hWnd, IDC_BUTTON_RESERVED_START, _T("��~"));
				nStart = 1;
			}
			else {
				SetDlgItemText(hWnd, IDC_BUTTON_RESERVED_START, _T("�J�n"));
				nStart = 0;
			}
			break;

		case IDM_CHANGE_MAXHP://0.15�Œǉ�
			int nTarget;
			_TCHAR szText[0x20];

			nTarget = ListView_GetNextItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), -1, LVNI_ALL | LVNI_SELECTED);
			if (nTarget >= 0) {
				ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), nTarget, COLUMN_NAME, szText, sizeof(szText) / sizeof(_TCHAR));
				SetWindowText(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARNAME), szText);

				ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), nTarget, COLUMN_MAXHP, szText, sizeof(szText) / sizeof(_TCHAR));
				SetWindowText(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), szText);

				SetFocus(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP));
			}
			break;

		case IDM_DELETE_CHAR://0.15�Œǉ�
			nTarget = ListView_GetNextItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), -1, LVNI_ALL | LVNI_SELECTED);
			if (nTarget) {
				SendMessage(GetDlgItem(hWnd, IDC_LIST_RESERVED), LVM_DELETEITEM, nTarget, NULL);
			}
			break;
		}
		break;

	case WM_TIMER:
		DWORD time;
		KillTimer(hWnd, TIMER_ID_RESERVED);
		if (UpdateList_Reserved(hWnd, nStart) == 1) {//0.14�ŕύX
			time = g_CmdWaitTime;//0.14�ŕύX
		}
		else time = 200;

		int ret;//0.38�Œǉ�
		ret = PutCmdQueue(GetParent(hWnd));
		if (ret == 2 && nStart == 0) {//�J�n
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_RESERVED_START, NULL);
		}
		else if (ret == 3 && nStart == 1) {//��~
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_RESERVED_START, NULL);
		}

		DWORD dwStopTimer;
		dwStopTimer = 0;

		if (nStart == 1 && g_StopTime && (int)(dwStartTime - timeGetTime()) < -(int)g_StopTime * 60 * 1000) {//0.40�ŕύX
																											 //SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_START,0);
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_RESERVED_START, 0);
			PlaySound(g_szStopWav, NULL, SND_ASYNC | SND_FILENAME);//0.41�ŕύX
		}
		else if (nStart == 1 && g_StopTime) {
			DWORD dwStopTime = g_StopTime * 60 * 1000;
			DWORD dwTimer = dwStopTime - (timeGetTime() - dwStartTime);
			_TCHAR szTimer[0x20];
			if (nStart == 3)_stprintf(szTimer, _T("�ꎞ��~��(%01d��)"), dwTimer / 60 / 1000);//0.38�Œǉ�
			else _stprintf(szTimer, _T("��~(%01d��)"), dwTimer / 60 / 1000);
			SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_RESERVED_START), szTimer);//0.38�ŕύX
		}

		//�G���A�X�g�b�v 0.42�Œǉ�
		if (nStart == 1 && g_AreaStop && stwAreaID != GetArea2ByteID()) {
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_RESERVED_START, 0);
			PlaySound(g_szStopWav, NULL, SND_ASYNC | SND_FILENAME);
		}
		else {
			stwAreaID = GetArea2ByteID();
		}

		SetTimer(hWnd, TIMER_ID_RESERVED, time, NULL);//0.14�ŕύX
		break;

	case WM_DRAWITEM:
		DrawItem((LPDRAWITEMSTRUCT)lp);
		break;

	case WM_MEASUREITEM:
		MeasureItem((LPMEASUREITEMSTRUCT)lp);
		break;

	case WM_RBUTTONUP:
		//���j���[��\�� 0.15�Œǉ�
		POINT pos;
		pos.x = LOWORD(lp);
		pos.y = HIWORD(lp);
		if (0 == ClientToScreen(hWnd, &pos)) {
		}
		SetForegroundWindow(hWnd);
		if (0 == TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, hWnd, NULL)) {
		}
		PostMessage(hWnd, 0, 0, 0);
		break;

	case WM_NOTIFY://�J�������ύX�ɂ��c�[���`�b�v�X�V 0.30�Œǉ�
		switch (((NMHDR*)lp)->code) {//0.34�ŕύX
		case HDN_ENDTRACK:
		case HDN_DIVIDERDBLCLICK://�w�b�_�[�_�u���N���b�N
			g_ColumnCx[((NMHEADER*)lp)->iItem] = ListGetHeaderWidth(((LPNMHDR)lp)->hwndFrom, ((NMHEADER*)lp)->iItem);
			ListChangeHeaderWidth(hWnd, ((NMHEADER*)lp)->iItem, g_ColumnCx[((NMHEADER*)lp)->iItem]);
			UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_RESERVED);
			ListChangeHeaderWidth(GetParent(hWnd), ((NMHEADER*)lp)->iItem, g_ColumnCx[((NMHEADER*)lp)->iItem]);
			SendMessage(GetParent(hWnd), TTM_NEWTOOLRECT, 0, 0);
			break;
		}
		break;

	case TTM_NEWTOOLRECT:
		//�c�[���`�b�v�̕����Ē���(���Y�������C���E�B���h�E���甭�s�����) 0.34�Œǉ�
		UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_RESERVED);
		break;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		//�T�C�Y�ύX�����������A�e�R���g���[���̑傫����ύX
		MoveWindow(GetDlgItem(hWnd, IDC_LIST_RESERVED), 1, 1, rect.right - 1 * 2, rect.bottom - 1 * 3 - 20, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARNAME), 1, rect.bottom - 21, 120, 100, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_COMBO_RESERVED_CHARHP), 122, rect.bottom - 21, 60, 100, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_RESERVED_ADD), 183, rect.bottom - 21, 68, 20, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_RESERVED_START), 252, rect.bottom - 21, rect.right - 252, 20, 1);
		UpdateToolTips(hWnd, hTool, (LONG)IDC_LIST_RESERVED);//0.30�Œǉ�
		break;

	case WM_MOVE:
		if (wp)SetWindowPos(hWnd, NULL, LOWORD(wp), HIWORD(wp), 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		break;

	case WM_CLOSE:
		g_ColumnCx[COLUMN_MAXHP] = ListView_GetColumnWidth(GetDlgItem(hWnd, IDC_LIST_RESERVED), 11);//0.17�Œǉ� 0.22�ŕύX
		if (nStart)SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_RESERVED_START, NULL);
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_RESERVED_OPEN, 1);
		break;
	}

	return 0;
}


//���X�g�X�V ���@�r��
int UpdateList_Reserved(HWND hWnd, int nStart) {
#define FLAG_HASTE	1
#define FLAG_REFRESH	2
#define FLAG_USER	4
	LVITEM item;
	PARTYINFORMATION pInfo[20];
	int nItemCount;
	_TCHAR szName[0x20];
	_TCHAR szHP[0x10];
	_TCHAR szMP[0x10];
	_TCHAR szTP[0x10];
	_TCHAR szPriority[0x10];
	_TCHAR szDistance[0x10];
	_TCHAR szCmd[0x40];
	struct {
		char Player;//�����D��x�ň�ԒႢHPP�̃v���C���[ID
		char HPP;//�����D��x�ň�ԒႢHPP
		int DeltaHP;//�����D��x�ň�ԒႢHPP��HP
	}priority[6];
	char out_range;
	static DWORD dwTime = 0;//�r���\����
	DWORD dwNowTime;
	static DWORD dwRecastTime[11] = { 0,0,0,0,0,0,0,0,0,0,0 };
	char chHasteRefreshFlag[18];
	static DWORD dwEffectTime[5][18];//�w�C�X�g���t�����[�U�̌��ʎ���
	static PARTYINFORMATION myInfo;
	static BYTE byteTargetPlayerNo = -1;
	static BYTE byteCasting = 0;
	static short shRecastNo = 0;
	static BYTE byteMagicNo = -1;//-1�Ń��L���X�g�ԍ����T�[�`���Ȃ� 0-5�P�A�� 6�w�C�X�g 7���t�� 8���[�U
	static BYTE stbNoRecastMode = 0;//���L���X�g�����Ȃ����[�h
	static DWORD stdwFlagCure[18] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//0.41�Œǉ�

	ZeroMemory(priority, sizeof(priority));
	for (int i = 0; i<6; i++) {//������
		priority[i].Player = -1;
		priority[i].HPP = 100;
	}
	ZeroMemory(chHasteRefreshFlag, sizeof(chHasteRefreshFlag));

	pInfo[0] = GetPartyInfo(0);

	//�H�Ɏ����̏�񂪖�����������
	if (_tcslen(pInfo[0].name) == 0)return 0;

	//���X�g�̐����擾
	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd, IDC_LIST_RESERVED));

	if (nItemCount == 0) {//List�Ɏ�����ǉ�
		item.mask = LVIF_TEXT;
		item.iItem = 0;//�s
		item.iSubItem = 0;//��
		item.pszText = pInfo[0].name;
		if (-1 == ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
			return 0;
		}
		item.iSubItem = COLUMN_PRIORITY;
		GetDlgItemText(GetParent(hWnd), IDC_COMBO_PRIORITY, szPriority, sizeof(szPriority) / sizeof(_TCHAR) - 1);
		item.pszText = szPriority;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
			return 0;
		}
		nItemCount = 1;
	}

	//���\�������Ԃ��擾
	for (int i = 0; i < nItemCount; i++) {
		if (i != 0) {
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
			pInfo[i] = GetCharInfo(szName);
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_MAXHP, szHP, sizeof(szHP) / sizeof(_TCHAR) - 1);
			pInfo[i].hp = _wtoi(szHP) * pInfo[i].hpp / 100;
			if (pInfo[i].code != 0) {
				item.mask = LVIF_TEXT;
				item.iItem = i;//�s
				item.iSubItem = COLUMN_DISTANCE;//��
				_stprintf(szDistance, _T("-"));
				out_range = 1;
				item.pszText = szDistance;
				if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
				}
				continue;
			}
		}
		//xz��2��������
		float fDistance = (float)sqrt(pow(pInfo[i].x - pInfo[0].x, 2) + pow(pInfo[i].z - pInfo[0].z, 2));

		item.mask = LVIF_TEXT;
		item.iItem = i;//�s
		item.iSubItem = COLUMN_DISTANCE;//��
		if (pInfo[i].inrange == 0 && i != 0) {//0.16�ŕύX
			_stprintf(szDistance, _T("-"));
			out_range = 2;
		}
		else if (fDistance <= (float)g_SpellDistance) {//0.13�ŕύX 0.16�ŕύX
			_stprintf(szDistance, _T("%0.2f"), fDistance);
			out_range = 0;
		}
		else {
			_stprintf(szDistance, _T("%0.2f"), fDistance);
			out_range = 1;
		}
		item.pszText = szDistance;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		//HP MP TP���X�V
		item.mask = LVIF_TEXT;
		item.iItem = i;//�s
		item.iSubItem = COLUMN_HP;//��
		_stprintf(szHP, _T("%d"), pInfo[i].hp);
		item.pszText = szHP;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		item.iSubItem = COLUMN_HPP;//��
		if (out_range == 2)_stprintf(szHP, _T(""));
		else _stprintf(szHP, _T("%d"), pInfo[i].hpp);
		item.pszText = szHP;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		item.iSubItem = COLUMN_MP;//��
		if (out_range == 2)_stprintf(szMP, _T(""));
		else _stprintf(szMP, _T("%d"), pInfo[i].mp);
		item.pszText = szMP;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		item.iSubItem = COLUMN_MPP;//��
		if (out_range == 2)_stprintf(szMP, _T(""));
		else _stprintf(szMP, _T("%d"), pInfo[i].mpp);
		item.pszText = szMP;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		item.iSubItem = COLUMN_TP;//��
		if (out_range == 2)_stprintf(szTP, _T(""));
		else _stprintf(szTP, _T("%d%%"), pInfo[i].tp);
		item.pszText = szTP;
		if (-1 == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_RESERVED), &item)) {
		}

		//�D��x���擾&�D��x���Ƃ̍ŏ�HPP�L�����N�^�[�̏����擾
		ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_PRIORITY, szPriority, sizeof(szPriority) / sizeof(_TCHAR) - 1);
		int nPri = _wtoi(szPriority);
		pInfo[i].priority = nPri;
		//�񕜍s���̃f�B���C 0.41�Œǉ�
		if (pInfo[i].hpp != 0 && g_DeltaHppPriority[nPri] >= pInfo[i].hpp) {
			if (stdwFlagCure[i] == 0) {
				stdwFlagCure[i] = timeGetTime();
			}
		}
		else {
			stdwFlagCure[i] = 0;
		}
		if (out_range == 0 && pInfo[i].hpp != 0 && priority[nPri].HPP >= pInfo[i].hpp && stdwFlagCure[i] && timeGetTime() - stdwFlagCure[i] >= g_CureWaitTime) {
			priority[nPri].Player = i;
			int max_hp = pInfo[i].hp * 100 / pInfo[i].hpp;
			priority[nPri].HPP = pInfo[i].hpp;
			priority[nPri].DeltaHP = max_hp - pInfo[i].hp;
		}

		//�w�C�X�g�p�`�F�b�N����Ă��邩�擾
		if (out_range == 0) {
			_TCHAR szBuf[0x10];
			char cFlag = 0;
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_HASTE, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_HASTE;
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_REFRESH, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_REFRESH;
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_USER, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER;
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_USER2, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER2;
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_RESERVED), i, COLUMN_USER3, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szBuf, _T("0")) == 1)cFlag |= FLAG_USER3;
			chHasteRefreshFlag[i] = cFlag;
		}
	}

	//���X�g���ĕ`��
	InvalidateRect(GetDlgItem(hWnd, IDC_LIST_RESERVED), 0, TRUE);

	//���@�r��
	dwNowTime = timeGetTime();
	int nCasting = Casting();

	if (nCasting > 0) {//�r���� 0.10�Œǉ�
		byteCasting = 1;
		return 0;
	}
	else if (byteCasting == 1) {//�r���I����̏���
		byteCasting = 0;
		if (nCasting != -1 && byteMagicNo != -1) {//�r������
			int nSearchRecastNo = 0;//���L���X�g�ԍ����T�[�`���邩 0.10�Œǉ�
			if (shRecastNo == 0) {//���L���X�g�ԍ����ݒ肳��Ė�����
				shRecastNo = SearchRecastNo(1);
				nSearchRecastNo = 1;
			}
			dwRecastTime[byteMagicNo] = dwNowTime + GetRecast(shRecastNo) * 1000 / 60;//���L���X�g���X�V
			if (byteTargetPlayerNo != -1 && (nSearchRecastNo == 0 && GetRecast(shRecastNo) > 0 || nSearchRecastNo != 0)) {//���L���X�g�ԍ����ݒ肵�Ă��鎞�̓��L���X�g���m�F 0.10�ŕύX
				switch (byteMagicNo) {//�w�C�X�g���t�����[�U�̏ꍇ���ʎ��Ԃ��X�V
				case 6://�w�C�X�g
					dwEffectTime[0][byteTargetPlayerNo] = dwNowTime + g_HasteSetting.time;
					break;
				case 7://���t��
					dwEffectTime[1][byteTargetPlayerNo] = dwNowTime + g_RefreshSetting.time;
					break;
				case 8://���[�U
					dwEffectTime[2][byteTargetPlayerNo] = dwNowTime + g_UserSetting[0].time;
					break;
				case 9://���[�U
					dwEffectTime[3][byteTargetPlayerNo] = dwNowTime + g_UserSetting[1].time;
					break;
				case 10://���[�U
					dwEffectTime[4][byteTargetPlayerNo] = dwNowTime + g_UserSetting[2].time;
					break;
				default:
					break;
				}
				byteTargetPlayerNo = -1;
			}

		}
		//�r����̒��ւ� 0.20�Œǉ�
		if (byteMagicNo != -1) {
			if (byteMagicNo >= 0 && byteMagicNo <= 5) {//�P�A��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_CureSetting[byteMagicNo].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_CureSetting[byteMagicNo].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 6) {//�w�C�X�g
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_HasteSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_HasteSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 7) {//���t��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_RefreshSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_RefreshSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 8) {//���[�U
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[0].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[0].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 9) {//���[�U2
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[1].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[1].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 10) {//���[�U3
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[2].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[2].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
		}
		byteMagicNo = -1;
		//�L���X�g��̃f�B���C 0.14�ŕύX
		dwTime = dwNowTime + g_CastWaitTime;

		return 0;
	}
	else if (stbNoRecastMode) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
		stbNoRecastMode = 0;
		if (byteTargetPlayerNo != -1) {
			switch (byteMagicNo) {//�w�C�X�g���t�����[�U�̏ꍇ���ʎ��Ԃ��X�V
			case 6://�w�C�X�g
				dwEffectTime[0][byteTargetPlayerNo] = dwNowTime + g_HasteSetting.time;
				break;
			case 7://���t��
				dwEffectTime[1][byteTargetPlayerNo] = dwNowTime + g_RefreshSetting.time;
				break;
			case 8://���[�U
				dwEffectTime[2][byteTargetPlayerNo] = dwNowTime + g_UserSetting[0].time;//0.22�ŕύX
				break;
			case 9://���[�U2 0.22�ŕύX
				dwEffectTime[3][byteTargetPlayerNo] = dwNowTime + g_UserSetting[1].time;
				break;
			case 10://���[�U3 0.22�ŕύX
				dwEffectTime[4][byteTargetPlayerNo] = dwNowTime + g_UserSetting[2].time;
				break;
			default:
				break;
			}
			byteTargetPlayerNo = -1;
		}
		if (byteMagicNo != -1) {
			if (byteMagicNo >= 0 && byteMagicNo <= 5) {//�P�A��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_CureSetting[byteMagicNo].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_CureSetting[byteMagicNo].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 6) {//�w�C�X�g
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_HasteSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_HasteSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 7) {//���t��
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_RefreshSetting.szEquipAfter[nEquip])) {
						ffhook_putcmd(g_RefreshSetting.szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 8) {//���[�U
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[0].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[0].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 9) {//���[�U2
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[1].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[1].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
			else if (byteMagicNo == 10) {//���[�U3
				for (int nEquip = 0; nEquip<9; nEquip++) {//0.43g�ŕύX
					if (_tcslen(g_UserSetting[2].szEquipAfter[nEquip])) {
						ffhook_putcmd(g_UserSetting[2].szEquipAfter[nEquip]);
					}
					else break;
				}
			}
		}

		byteMagicNo = -1;
		//�L���X�g��̃f�B���C
		dwTime = dwNowTime + g_CastWaitTime;

		return 0;
	}

	//���S���͉������Ȃ�
	if (pInfo[0].hpp == 0)return 0;

	//Activity���`�F�b�N 0.28�Œǉ�
	WORD wActivity = GetActivityInfo();
	if (wActivity != 0 && wActivity != 1) {
		return 0;
	}

	//���L���X�g�ԍ��̃t���O��������
	shRecastNo = 0;
	byteMagicNo = -1;

	if (nStart == 1 && myInfo.x == pInfo[0].x && myInfo.y == pInfo[0].y && myInfo.z == pInfo[0].z) {//�ʒu�������Ă��Ȃ��ꍇ
		memcpy(&myInfo, &pInfo[0], sizeof(PARTYINFORMATION));//pInfo[0]�͎���
		if (dwTime <= dwNowTime) {
			dwTime = dwNowTime;
			//�����ŃR�}���h�𑗂��Ă݂�
			for (int j = 5; j > 0; j--) {//�D��x0�͉񕜂��Ȃ�
				if (priority[j].HPP < g_DeltaHppPriority[j]) {
					//HP��g_DeltaHppPriority�����̎�
					//���@��I��
					for (int curenum = 5; curenum >= 0; curenum--) {
						if (priority[j].DeltaHP >= g_CureSetting[curenum].threshold && _tcslen(g_CureSetting[curenum].szCmd) != 0 && dwRecastTime[curenum] <= dwNowTime && GetRecast(g_CureSetting[curenum].recast_no) == 0 && myInfo.mp >= g_CureSetting[curenum].mp) {//0.43b�ŕύX
							ZeroMemory(szCmd, sizeof(szCmd));
							if (_tcslen(pInfo[priority[j].Player].name) == 0)break;
							_stprintf(szCmd, _T("%s %s"), g_CureSetting[curenum].szCmd, pInfo[priority[j].Player].name);
							if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
								ffhook_putcmd(szCmd);
								dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
								shRecastNo = g_CureSetting[curenum].recast_no;
								byteMagicNo = curenum;
								if (shRecastNo == 0) {//0.10�Œǉ�
									SearchRecastNo(0);
								}
								for (int nEquip = 0; nEquip<9; nEquip++) {//0.20�Œǉ� 0.43g�ŕύX
									if (_tcslen(g_CureSetting[curenum].szEquip[nEquip])) {
										ffhook_putcmd(g_CureSetting[curenum].szEquip[nEquip]);
									}
									else break;
								}
								return 1;
							}
						}
					}
				}
			}

			//�w�C�X�g�@H�A�C�R����PC
			if (dwRecastTime[6] <= dwNowTime && _tcslen(g_HasteSetting.szCmd) != 0) {//�w�C�X�g��6��
				for (int i = 0; i < nItemCount; i++) {
					if (chHasteRefreshFlag[i] & FLAG_HASTE && dwEffectTime[0][i] <= dwNowTime && GetRecast(g_HasteSetting.recast_no) == 0 && myInfo.mp >= g_HasteSetting.mp) {//0.43b�ŕύX
						if (g_HasteSetting.mode == 1) {//0.43b�Œǉ�
							if (pInfo[i].state == 0x01) {
							}
							else {
								continue;
							}
						}
						if (pInfo[i].hpp == 0) {//0.43b�Œǉ�
							continue;
						}

						ZeroMemory(szCmd, sizeof(szCmd));
						_stprintf(szCmd, _T("%s %s"), g_HasteSetting.szCmd, pInfo[i].name);
						if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
							ffhook_putcmd(szCmd);
							dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
							shRecastNo = g_HasteSetting.recast_no;
							byteTargetPlayerNo = i;
							byteMagicNo = 6;
							if (shRecastNo == 0) {//0.10�Œǉ�
								SearchRecastNo(0);
							}
							else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
								stbNoRecastMode = 1;
							}
							for (int nEquip = 0; nEquip<9; nEquip++) {//0.20�Œǉ� 0.43g�ŕύX
								if (_tcslen(g_HasteSetting.szEquip[nEquip])) {
									ffhook_putcmd(g_HasteSetting.szEquip[nEquip]);
								}
								else break;
							}
							return 1;
						}
						break;
					}
				}
			}

			//���t���@R�A�C�R����PC
			if (dwRecastTime[7] <= dwNowTime && _tcslen(g_RefreshSetting.szCmd) != 0) {//���t����7��
				for (int i = 0; i < nItemCount; i++) {
					if (chHasteRefreshFlag[i] & FLAG_REFRESH && dwEffectTime[1][i] <= dwNowTime && GetRecast(g_RefreshSetting.recast_no) == 0 && myInfo.mp >= g_RefreshSetting.mp) {//0.43b�ŕύX
						if (g_RefreshSetting.mode == 1) {//0.43b�Œǉ�
							if (pInfo[i].state == 0x01) {
							}
							else {
								continue;
							}
						}
						if (pInfo[i].hpp == 0) {//0.43b�Œǉ�
							continue;
						}

						ZeroMemory(szCmd, sizeof(szCmd));
						_stprintf(szCmd, _T("%s %s"), g_RefreshSetting.szCmd, pInfo[i].name);
						if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
							ffhook_putcmd(szCmd);
							dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
							shRecastNo = g_RefreshSetting.recast_no;
							byteTargetPlayerNo = i;
							byteMagicNo = 7;
							if (shRecastNo == 0) {
								SearchRecastNo(0);
							}
							else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
								stbNoRecastMode = 1;
							}

							for (int nEquip = 0; nEquip<9; nEquip++) {//0.20�Œǉ� 0.43g�ŕύX
								if (_tcslen(g_RefreshSetting.szEquip[nEquip])) {
									ffhook_putcmd(g_RefreshSetting.szEquip[nEquip]);
								}
								else break;
							}
							return 1;
						}
						break;
					}
				}
			}

			//���[�U�@U�A�C�R����PC
			for (int nUserNum = 0; nUserNum <= 2; nUserNum++) {
				if (dwRecastTime[nUserNum + 8] <= dwNowTime && _tcslen(g_UserSetting[nUserNum].szCmd) != 0) {//���[�U��8��
					for (int i = 0; i < nItemCount; i++) {
						if (chHasteRefreshFlag[i] & (FLAG_USER << nUserNum) && dwEffectTime[nUserNum + 2][i] <= dwNowTime && GetRecast(g_UserSetting[nUserNum].recast_no) == 0 && myInfo.mp >= g_UserSetting[nUserNum].mp) {//0.43b�ŕύX
							if (g_UserSetting[nUserNum].mode == 1) {//0.43b�Œǉ�
								if (pInfo[i].state == 0x01) {
								}
								else {
									continue;
								}
							}
							if (pInfo[i].hpp == 0) {//0.43b�Œǉ�
								continue;
							}

							ZeroMemory(szCmd, sizeof(szCmd));
							_stprintf(szCmd, _T("%s %s"), g_UserSetting[nUserNum].szCmd, pInfo[i].name);
							if (wcsncmp(szCmd, _T("/"), 1) == 0) {//�딚�h�~
								if (wcsncmp(g_UserSetting[nUserNum].szPreJa, _T("/"), 1) == 0) {//�v���W���u�A�r���e�B 0.31�Œǉ�
									ffhook_putcmd(g_UserSetting[nUserNum].szPreJa);
									Sleep(1500);
								}
								ffhook_putcmd(szCmd);
								dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
								shRecastNo = g_UserSetting[nUserNum].recast_no;
								byteTargetPlayerNo = i;
								byteMagicNo = nUserNum + 8;
								if (shRecastNo == 0) {
									SearchRecastNo(0);
								}
								else if (shRecastNo == -1) {//0.27�Œǉ� ���L���X�g�����Ȃ����[�h
									stbNoRecastMode = 1;
								}

								for (int nEquip = 0; nEquip<9; nEquip++) {//0.20�Œǉ� 0.43g�ŕύX
									if (_tcslen(g_UserSetting[nUserNum].szEquip[nEquip])) {
										ffhook_putcmd(g_UserSetting[nUserNum].szEquip[nEquip]);
									}
									else break;
								}
								return 1;
							}
							break;
						}
					}
				}
			}
			if (exec_queue() == 1) {//0.13a�ŕύX
				dwTime += g_CmdInterval;//0.13a�Œǉ� 0.14�ŕύX
				return 1;
			}
		}
	}
	else {//�������ꍇ�͈ʒu����ۑ�
		memcpy(&myInfo, &pInfo[0], sizeof(PARTYINFORMATION));//pInfo[0]�͎���
		dwTime = dwNowTime + g_MoveWaitTime;//0.14�ŕύX
	}

	return 0;
}


//�L�����N�^�[���擾
PARTYINFORMATION GetCharInfo(_TCHAR* name)
{
	PARTYINFORMATION info;
	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

#ifdef _DEBUG//0.13�ŏC��&�폜
	for (int i = 0x000; i < 0x700; i++) {
#else
	for (int i = 0x400; i<0x700; i++) {
#endif
		info = GetNPCInfo(i);
		if (_tcscmp (info.name, name) == 0) {
			break;
		}
	}

	return info;
	}

//�L���[�����s 0.13�Œǉ� 0.13b�ŕύX
int exec_queue()
{
	CMD_QUEUE *exe_queue;//0.42c�ŕύX
	HANDLE hEvent;
	int ret = 1;

	if (cqueue->nextpointer == NULL)return -1;

	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
	WaitForSingleObject(hEvent, 1000);//0.42c�ŕύX
	ResetEvent(hEvent);
	exe_queue = cqueue->nextpointer;//0.42c�ŕύX
	if (_tcslen(exe_queue->szCmd) == 0) {
		ret = -2;
	}
	else {
		if (wcsncmp(exe_queue->szCmd, _T("/"), 1) == 0) {//0.42a�ŏ����ǉ�
			ffhook_putcmd(exe_queue->szCmd);
		}
		cqueue->nextpointer = exe_queue->nextpointer;//0.40�ňړ�
		free(exe_queue);//0.42c�ŕύX
	}

	SetEvent(hEvent);
	CloseHandle(hEvent);

	return ret;
}

//0.13�Œǉ� 0.13a�ŕύX 0.13b�ŕύX
int PutCmdQueue(HWND hWnd)
{
	_TCHAR szCmd[0x40];
	int ret = 0;

	for (int i = 0; i<10; i++) {
		ffhook_getcmd(szCmd, sizeof(szCmd) / sizeof(_TCHAR) - 1);
		if (_tcslen(szCmd) == 0) {
			break;
		}
		else if (_tcscmp (szCmd, _T("/healer run")) == 0) {
			//�}�N��������s�����//healer�͖��������(//���_�����ۂ�)0.40a�ŕύX
			ret = 2;
		}
		else if (_tcscmp (szCmd, _T("/healer stop")) == 0) {
			ret = 3;
		}
		else if (wcsncmp(szCmd, _T("/healer "), 8) == 0) {
			COPYDATASTRUCT cp;
			ZeroMemory(&cp, sizeof(COPYDATASTRUCT));
			cp.lpData = &szCmd[8];
			cp.cbData = (_tcslen(&szCmd[8]) + 1) * sizeof(_TCHAR);
			SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cp);
			ret = 1;
		}
	}

	return ret;
}


//�ݒ� 臒l�̃E�B���h�v���V�[�W�� 0.36�Œǉ� ���[�h���X�_�C�A���O
LRESULT CALLBACK WndProcSettingThreshold(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HBRUSH hBrush;//0.37�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		HDC hDC;
		hDC = GetDC(GetDlgItem(GetParent(hWnd), IDC_TAB_SETTING));
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		ReleaseDC(GetParent(hWnd), hDC);

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 95));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_DeltaHppPriority[1]);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 95));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_DeltaHppPriority[2]);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 95));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_DeltaHppPriority[3]);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 95));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_DeltaHppPriority[4]);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 95));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_DeltaHppPriority[5]);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5));
		break;

	case WM_SETTING_DONE:
		g_DeltaHppPriority[1] = (BYTE)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1), TBM_GETPOS, NULL, NULL);
		g_DeltaHppPriority[2] = (BYTE)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2), TBM_GETPOS, NULL, NULL);
		g_DeltaHppPriority[3] = (BYTE)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3), TBM_GETPOS, NULL, NULL);
		g_DeltaHppPriority[4] = (BYTE)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4), TBM_GETPOS, NULL, NULL);
		g_DeltaHppPriority[5] = (BYTE)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5), TBM_GETPOS, NULL, NULL);
		break;

	case WM_HSCROLL:
		_TCHAR szNum[0x10];
		if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP1)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_DELTA_HPP1), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP2)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_DELTA_HPP2), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP3)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_DELTA_HPP3), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP4)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_DELTA_HPP4), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_DELTA_HPP5)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_DELTA_HPP5), szNum);
		}
		break;

	case WM_CTLCOLORDLG:
		if (hWnd == (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hWnd != (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CLOSE:
		DeleteObject((HGDIOBJ)hBrush);//0.37�Œǉ�
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}

//�ݒ� �ҋ@���Ԃ̃E�B���h�v���V�[�W�� 0.36�Œǉ� ���[�h���X�_�C�A���O
LRESULT CALLBACK WndProcSettingTime(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HBRUSH hBrush;//0.37�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		HDC hDC;
		hDC = GetDC(GetDlgItem(GetParent(hWnd), IDC_TAB_SETTING));
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		ReleaseDC(GetParent(hWnd), hDC);

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(5000, 30000));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE), TBM_SETPAGESIZE, 0, 500);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(int)(g_SpellDistance * 1000));
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1000, 5000));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME), TBM_SETPAGESIZE, 0, 100);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_CastWaitTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(400, 2000));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL), TBM_SETPAGESIZE, 0, 100);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_CmdInterval);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(400, 1000));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME), TBM_SETPAGESIZE, 0, 100);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_CmdWaitTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1000, 9500));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME), TBM_SETPAGESIZE, 0, 500);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_MoveWaitTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(5000, 30000));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME), TBM_SETPAGESIZE, 0, 1000);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_AreaWaitTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME));

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 10000));//0.40�Œǉ�
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME), TBM_SETPAGESIZE, 0, 500);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_CureWaitTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME));
		break;

	case WM_SETTING_DONE:
		g_SpellDistance = (double)SendMessage(GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE), TBM_GETPOS, 0, 0) / 1000;
		g_CastWaitTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME), TBM_GETPOS, 0, 0);
		g_CmdInterval = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL), TBM_GETPOS, 0, 0);
		g_CmdWaitTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME), TBM_GETPOS, 0, 0);
		g_MoveWaitTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME), TBM_GETPOS, 0, 0);
		g_AreaWaitTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME), TBM_GETPOS, 0, 0);
		g_CureWaitTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME), TBM_GETPOS, 0, 0);//0.40�Œǉ�
		break;

	case WM_HSCROLL:
		_TCHAR szNum[0x10];
		if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_SPELL_DISTANCE)) {
			double Distance = (double)SendMessage((HWND)lp, TBM_GETPOS, 0, 0) / 1000;
			_stprintf(szNum, _T("%.2f"), Distance);
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_SPELL_DISTANCE), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_CAST_WAIT_TIME)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_CAST_WAIT_TIME), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_CMD_INTERVAL)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_CMD_INTERVAL), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_CMD_WAIT_TIME)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_CMD_WAIT_TIME), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_MOVE_WAIT_TIME)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_MOVE_WAIT_TIME), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_AREA_WAIT_TIME)) {
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_AREA_WAIT_TIME), szNum);
		}
		else if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_CURE_WAIT_TIME)) {//0.40�Œǉ�
			_stprintf(szNum, _T("%d"), (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0));
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_CURE_WAIT_TIME), szNum);
		}
		break;

	case WM_CTLCOLORDLG:
		if (hWnd == (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hWnd != (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CLOSE:
		DeleteObject((HGDIOBJ)hBrush);//0.37�Œǉ�
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}

//�ݒ� �N�����\���̃E�B���h�v���V�[�W�� 0.36�Œǉ� ���[�h���X�_�C�A���O
LRESULT CALLBACK WndProcSettingOpen(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HBRUSH hBrush;//0.37�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		HDC hDC;
		hDC = GetDC(GetDlgItem(GetParent(hWnd), IDC_TAB_SETTING));
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		ReleaseDC(GetParent(hWnd), hDC);

		if (g_Log.default_open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_LOG_DEFAULTOPEN, BST_CHECKED);
		}
		if (g_Follow.default_open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_FOLLOW_DEFAULTOPEN, BST_CHECKED);
		}
		if (g_Reserved.default_open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_RESERVED_DEFAULTOPEN, BST_CHECKED);
		}
		if (g_Target.default_open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN, BST_CHECKED);
		}
		if (g_Link.open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN_LINKGROUP, BST_CHECKED);
		}
		if (g_Target_Add.default_open) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN_ADD, BST_CHECKED);
		}

		SetDlgItemText(hWnd, IDC_EDIT_DEFAULT_PIDNAME, g_szDefaultPIDName);//0.43�Œǉ�
		break;

	case WM_SETTING_DONE:
		g_Log.default_open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_LOG_DEFAULTOPEN);
		g_Follow.default_open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_FOLLOW_DEFAULTOPEN);
		g_Reserved.default_open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_RESERVED_DEFAULTOPEN);
		g_Target.default_open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN);
		g_Link.open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN_LINKGROUP);
		g_Target_Add.default_open = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_TARGET_DEFAULTOPEN_ADD);
		GetDlgItemText(hWnd, IDC_EDIT_DEFAULT_PIDNAME, g_szDefaultPIDName, sizeof(g_szDefaultPIDName) / sizeof(_TCHAR) - 1);//0.43�Œǉ�
		break;

	case WM_CTLCOLORDLG:
		if (hWnd == (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hWnd != (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CLOSE:
		DeleteObject((HGDIOBJ)hBrush);//0.37�Œǉ�
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}

//�ݒ� ���̑��̃E�B���h�v���V�[�W�� 0.36�Œǉ� ���[�h���X�_�C�A���O
LRESULT CALLBACK WndProcSettingOther(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HBRUSH hBrush;//0.37�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		HDC hDC;
		hDC = GetDC(GetDlgItem(GetParent(hWnd), IDC_TAB_SETTING));
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		ReleaseDC(GetParent(hWnd), hDC);

		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("0"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("1"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("2"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("3"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("4"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_ADDSTRING, 0, (LPARAM)_T("5"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_SETCURSEL, g_DefaultPriority, 0);

		SetDlgItemText(hWnd, IDC_EDIT_DEFAULT_FOLLOWER, g_DefaultFollower);

		if (g_UseWindowerHelper) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_WINDOWERHELPER, BST_CHECKED);
		}
		if (g_SavePosition) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_SAVE_POSITION, BST_CHECKED);
		}
		if (g_HinshiDeltaHp) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_HINSHI_DELTAHP, BST_CHECKED);
		}
		if (g_AreaStop) {//0.42�Œǉ�
			CheckDlgButton(hWnd, IDC_CHECKBOX_AREASTOP, BST_CHECKED);
		}

		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 300));//0.40�Œǉ�
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME), TBM_SETPAGESIZE, 0, 5);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)g_StopTime);
		SendMessage(hWnd, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME));

		SetDlgItemText(hWnd, IDC_EDIT_STOPWAV, g_szStopWav);//0.41�Œǉ�
		break;

	case WM_COMMAND://0.41�Œǉ�
		if (LOWORD(wp) == IDC_BUTTON_OPEN_STOPWAV) {
			_TCHAR *szFileName;
			szFileName = (_TCHAR*)malloc(MAX_PATH * sizeof(_TCHAR));
			int nRet = GetFileName(hWnd, szFileName, MAX_PATH);
			if (nRet == -1) {
				MessageBox(hWnd, _T("�t�@�C�����J���܂���"), NULL, MB_ICONERROR);
			}
			else if (nRet == 1) {
				SetDlgItemText(hWnd, IDC_EDIT_STOPWAV, szFileName);//0.41�Œǉ�
			}
			free(szFileName);
		}
		break;

	case WM_SETTING_DONE:
		g_DefaultPriority = SendMessage(GetDlgItem(hWnd, IDC_COMBO_DEFAULT_PRIORITY), CB_GETCURSEL, 0, 0);
		GetDlgItemText(hWnd, IDC_EDIT_DEFAULT_FOLLOWER, g_DefaultFollower, sizeof(g_DefaultFollower) / sizeof(_TCHAR) - 1);
		g_UseWindowerHelper = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_WINDOWERHELPER);
		g_HinshiDeltaHp = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_HINSHI_DELTAHP);
		g_SavePosition = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_SAVE_POSITION);
		g_StopTime = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME), TBM_GETPOS, 0, 0);//0.40�Œǉ�
		GetDlgItemText(hWnd, IDC_EDIT_STOPWAV, g_szStopWav, sizeof(g_szStopWav) / sizeof(_TCHAR) - 1);//0.41�Œǉ�
		g_AreaStop = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_AREASTOP);//0.42�Œǉ�
		break;

	case WM_CTLCOLORDLG:
		if (hWnd == (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hWnd != (HWND)lp) {
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_HSCROLL://0.40�Œǉ�
		_TCHAR szNum[0x10];
		if ((HWND)lp == GetDlgItem(hWnd, IDC_SLIDER_STOP_TIME)) {
			int nSliderPos = (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0);
			if (nSliderPos == 0) {
				_stprintf(szNum, _T("��"));
			}
			else {
				_stprintf(szNum, _T("%d"), nSliderPos);
			}
			SetWindowText(GetDlgItem(hWnd, IDC_STATIC_STOP_TIME), szNum);
		}
		break;

	case WM_CLOSE:
		DeleteObject((HGDIOBJ)hBrush);//0.37�Œǉ�
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}

//�ݒ� �I�t�Z�b�g�̃E�B���h�v���V�[�W�� 0.36�Œǉ� ���[�h���X�_�C�A���O
LRESULT CALLBACK WndProcSettingOffset(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HBRUSH hBrush;//0.37�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		HDC hDC;
		hDC = GetDC(GetDlgItem(GetParent(hWnd), IDC_TAB_SETTING));
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		ReleaseDC(GetParent(hWnd), hDC);

		if (g_Offset.Auto) {
			CheckDlgButton(hWnd, IDC_CHECKBOX_OFFSET, BST_CHECKED);
			PostMessage(hWnd, WM_COMMAND, IDC_CHECKBOX_OFFSET, NULL);
		}
		break;

	case WM_SETTING_DONE:
		g_Offset.Auto = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_OFFSET);
		if (g_Offset.Auto) {
			if (GetOffsetAddressAll(g_polHandle, g_DllAddress) != 0) {
				MessageBox(hWnd, _T("�I�t�Z�b�g�擾�Ɏ��s"), NULL, MB_ICONERROR);
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_CHECKBOX_OFFSET:
			if (IsDlgButtonChecked(hWnd, IDC_CHECKBOX_OFFSET)) {
				DWORD dwBuf;
				_TCHAR szBuf[0x10];
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constNpcmapPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NPCMAP, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constNowstaPattern);
				if (dwBuf)dwBuf -= NOWSTA_HOSEI;
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NOWSTA, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constMapPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_MAP, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constRecastPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_RECAST, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constCastPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_CAST, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constVeiwstylePattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_VIEWSTYLE, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constNpclistPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NPCLIST, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constActivityinfoPattern);
				if (dwBuf)dwBuf += ACTIVITY_INFO_HOSEI;
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_ACTIVITY_INFO, szBuf);
				dwBuf = GetOffsetAddress(g_polHandle, g_DllAddress, (const BYTE*)constTargetPattern);
				_stprintf(szBuf, _T("%X"), dwBuf);
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_TARGET, szBuf);
			}
			else {
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NPCMAP, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NOWSTA, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_MAP, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_RECAST, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_CAST, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_VIEWSTYLE, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_NPCLIST, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_ACTIVITY_INFO, _T(""));
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET_TARGET, _T(""));
			}
			break;
		}
		break;

	case WM_CTLCOLORDLG:
		if (hWnd == (HWND)lp) {
			return (LRESULT)hBrush;
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hWnd != (HWND)lp) {
			return (LRESULT)hBrush;
		}
		break;

	case WM_CLOSE:
		DeleteObject((HGDIOBJ)hBrush);//0.37�Œǉ�
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}

//�ݒ�̃E�B���h�v���V�[�W�� 0.36�ő啝�ύX
LRESULT CALLBACK WndProcSetting(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HWND hThresholdWnd;
	static HWND hTimeWnd;
	static HWND hOpenWnd;
	static HWND hOtherWnd;
	static HWND hOffsetWnd;
	const _TCHAR szTab[][0x10] = { _T("�������l"),_T("�ҋ@����"),_T("�N�����\��"),_T("�I�t�Z�b�g"),_T("���̑�") };

	switch (msg) {
	case WM_INITDIALOG:
		TCITEM tci;
		tci.mask = TCIF_TEXT;
		for (int i = 0; i < 5; i++) {
			tci.pszText = (LPWSTR)szTab[i];
			TabCtrl_InsertItem(GetDlgItem(hWnd, IDC_TAB_SETTING), i, &tci);
		}

		RECT rect;
		GetWindowRect(GetParent(hWnd), &rect);
		SetWindowPos(hWnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		hThresholdWnd = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING_THRESHOLD), GetDlgItem(hWnd, IDC_TAB_SETTING), (DLGPROC)WndProcSettingThreshold);
		hTimeWnd = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING_TIME), GetDlgItem(hWnd, IDC_TAB_SETTING), (DLGPROC)WndProcSettingTime);
		hOpenWnd = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING_OPEN), GetDlgItem(hWnd, IDC_TAB_SETTING), (DLGPROC)WndProcSettingOpen);
		hOffsetWnd = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING_OFFSET), GetDlgItem(hWnd, IDC_TAB_SETTING), (DLGPROC)WndProcSettingOffset);
		hOtherWnd = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SETTING_OTHER), GetDlgItem(hWnd, IDC_TAB_SETTING), (DLGPROC)WndProcSettingOther);

		RECT rectChild;
		GetClientRect(GetDlgItem(hWnd, IDC_TAB_SETTING), &rect);
		GetClientRect(hThresholdWnd, &rectChild);

		SetWindowPos(hThresholdWnd, NULL, (rect.right - rectChild.right) / 2, rect.bottom - 4 - rectChild.bottom, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
		SetWindowPos(hTimeWnd, NULL, (rect.right - rectChild.right) / 2, rect.bottom - 4 - rectChild.bottom, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
		SetWindowPos(hOpenWnd, NULL, (rect.right - rectChild.right) / 2, rect.bottom - 4 - rectChild.bottom, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
		SetWindowPos(hOffsetWnd, NULL, (rect.right - rectChild.right) / 2, rect.bottom - 4 - rectChild.bottom, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
		SetWindowPos(hOtherWnd, NULL, (rect.right - rectChild.right) / 2, rect.bottom - 4 - rectChild.bottom, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);

		ShowWindow(hThresholdWnd, SW_SHOWNORMAL);

		SetFocus(GetDlgItem(hWnd, IDC_BUTTON_DONE));
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_DONE:
			SendMessage(hThresholdWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hTimeWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOpenWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOtherWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOffsetWnd, WM_SETTING_DONE, NULL, NULL);
			MessageBox(hWnd, _T("�X�V���܂���"), _T("�ݒ�"), MB_OK);
			break;

		case IDC_BUTTON_SAVE:
			//�X�V���Ă����
			SendMessage(hThresholdWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hTimeWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOpenWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOtherWnd, WM_SETTING_DONE, NULL, NULL);
			SendMessage(hOffsetWnd, WM_SETTING_DONE, NULL, NULL);

			//�ۑ�
			_TCHAR szBuf[0x10];
			_TCHAR szFullPathName[MAX_PATH];
			int ret;

			if (0 == GetFullPathName(_T("healer.ini"), sizeof(szFullPathName) / sizeof(_TCHAR) - 1, szFullPathName, NULL)) {
				MessageBox(hWnd, _T("�ۑ��Ɏ��s���܂���"), _T("�ݒ�"), MB_ICONERROR);
				break;
			}

			ret = 1;
			_stprintf(szBuf, _T("%d"), g_Offset.Auto);
			ret &= WritePrivateProfileString(_T("OFFSET"), _T("AUTO"), szBuf, szFullPathName);

			ret &= WritePrivateProfileString(_T("INIT"), _T("DEFAULT_PIDNAME"), g_szDefaultPIDName, szFullPathName);//0.43�Œǉ�
			_stprintf(szBuf, _T("%d"), g_DeltaHppPriority[1]);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DELTA_HPP_PRIORITY1"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_DeltaHppPriority[2]);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DELTA_HPP_PRIORITY2"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_DeltaHppPriority[3]);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DELTA_HPP_PRIORITY3"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_DeltaHppPriority[4]);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DELTA_HPP_PRIORITY4"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_DeltaHppPriority[5]);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DELTA_HPP_PRIORITY5"), szBuf, szFullPathName);

			_stprintf(szBuf, _T("%.2f"), g_SpellDistance);
			ret &= WritePrivateProfileString(_T("INIT"), _T("SPELL_DISTANCE"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_UseWindowerHelper);
			ret &= WritePrivateProfileString(_T("INIT"), _T("USE_WINDOWERHELPER"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_DefaultPriority);
			ret &= WritePrivateProfileString(_T("INIT"), _T("PRIORITY"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_CastWaitTime);
			ret &= WritePrivateProfileString(_T("INIT"), _T("CAST_WAIT_TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_CmdInterval);
			ret &= WritePrivateProfileString(_T("INIT"), _T("CMD_INTERVAL"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_CmdWaitTime);
			ret &= WritePrivateProfileString(_T("INIT"), _T("CMD_WAIT_TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_MoveWaitTime);
			ret &= WritePrivateProfileString(_T("INIT"), _T("MOVE_WAIT_TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_AreaWaitTime);
			ret &= WritePrivateProfileString(_T("INIT"), _T("AREA_WAIT_TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_CureWaitTime);//0.40�Œǉ�
			ret &= WritePrivateProfileString(_T("INIT"), _T("CURE_WAIT_TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_StopTime);//0.41�Œǉ�
			ret &= WritePrivateProfileString(_T("INIT"), _T("STOP_TIME"), szBuf, szFullPathName);
			ret &= WritePrivateProfileString(_T("INIT"), _T("STOP_WAV"), g_szStopWav, szFullPathName);//0.41�Œǉ�
			_stprintf(szBuf, _T("%d"), g_AreaStop);//0.42�Œǉ�
			ret &= WritePrivateProfileString(_T("INIT"), _T("AREA_STOP"), szBuf, szFullPathName);//0.42�Œǉ�

			_stprintf(szBuf, _T("%d"), g_SavePosition);
			ret &= WritePrivateProfileString(_T("INIT"), _T("SAVE_POSITION"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Log.default_open);
			ret &= WritePrivateProfileString(_T("INIT"), _T("WINDOW_LOG"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Follow.default_open);
			ret &= WritePrivateProfileString(_T("INIT"), _T("WINDOW_FOLLOW"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Reserved.default_open);
			ret &= WritePrivateProfileString(_T("INIT"), _T("WINDOW_RESERVED"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Target.default_open);
			ret &= WritePrivateProfileString(_T("INIT"), _T("WINDOW_TARGET"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Target_Add.default_open);
			ret &= WritePrivateProfileString(_T("INIT"), _T("WINDOW_TARGET_ADD"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_HinshiDeltaHp);
			ret &= WritePrivateProfileString(_T("INIT"), _T("HINSHI_DELTA_HP"), szBuf, szFullPathName);
			ret &= WritePrivateProfileString(_T("INIT"), _T("DEFAULT_FOLLOWER"), g_DefaultFollower, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_Link.open);
			ret &= WritePrivateProfileString(_T("LINK"), _T("LINK_OPEN"), szBuf, szFullPathName);

			_stprintf(szBuf, _T("%d"), g_ColumnCx[0]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("NAME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[1]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("HASTE"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[2]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("REFRESH"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[3]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("USER"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[4]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("USER2"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[5]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("USER3"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[6]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("HP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[7]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("HPP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[8]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("MP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[9]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("MPP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[10]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("TP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[11]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("PRIORITY"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[12]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("DISTANCE"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCx[13]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("MAXHP"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCxLog[0]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("ATTR"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCxLog[1]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("TIME"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCxLog[2]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("LOG"), szBuf, szFullPathName);
			_stprintf(szBuf, _T("%d"), g_ColumnCxLog[3]);
			ret &= WritePrivateProfileString(_T("COLUMN"), _T("RECASTNO"), szBuf, szFullPathName);

			if (ret == 1)MessageBox(hWnd, _T("�ۑ����܂���"), _T("�ݒ�"), MB_OK);
			else MessageBox(hWnd, _T("�ۑ��Ɏ��s���܂���"), _T("�ݒ�"), MB_ICONERROR);
			break;

		default:
			break;
		}
		break;

	case WM_NOTIFY:
		switch (((NMHDR *)lp)->code) {
		case TCN_SELCHANGE:
			switch (TabCtrl_GetCurSel(GetDlgItem(hWnd, IDC_TAB_SETTING))) {
			case 0:
				ShowWindow(hThresholdWnd, SW_SHOWNORMAL);
				ShowWindow(hTimeWnd, SW_HIDE);
				ShowWindow(hOpenWnd, SW_HIDE);
				ShowWindow(hOffsetWnd, SW_HIDE);
				ShowWindow(hOtherWnd, SW_HIDE);
				break;
			case 1:
				ShowWindow(hThresholdWnd, SW_HIDE);
				ShowWindow(hTimeWnd, SW_SHOWNORMAL);
				ShowWindow(hOpenWnd, SW_HIDE);
				ShowWindow(hOffsetWnd, SW_HIDE);
				ShowWindow(hOtherWnd, SW_HIDE);
				break;
			case 2:
				ShowWindow(hThresholdWnd, SW_HIDE);
				ShowWindow(hTimeWnd, SW_HIDE);
				ShowWindow(hOpenWnd, SW_SHOWNORMAL);
				ShowWindow(hOffsetWnd, SW_HIDE);
				ShowWindow(hOtherWnd, SW_HIDE);
				break;
			case 3:
				ShowWindow(hThresholdWnd, SW_HIDE);
				ShowWindow(hTimeWnd, SW_HIDE);
				ShowWindow(hOpenWnd, SW_HIDE);
				ShowWindow(hOtherWnd, SW_HIDE);
				ShowWindow(hOffsetWnd, SW_SHOWNORMAL);
				break;
			case 4:
				ShowWindow(hThresholdWnd, SW_HIDE);
				ShowWindow(hTimeWnd, SW_HIDE);
				ShowWindow(hOpenWnd, SW_HIDE);
				ShowWindow(hOffsetWnd, SW_HIDE);
				ShowWindow(hOtherWnd, SW_SHOWNORMAL);
				break;
			}
			break;
		}
		break;

	case WM_CLOSE:
		SendMessage(hThresholdWnd, WM_CLOSE, NULL, NULL);
		SendMessage(hTimeWnd, WM_CLOSE, NULL, NULL);
		SendMessage(hOpenWnd, WM_CLOSE, NULL, NULL);
		SendMessage(hOtherWnd, WM_CLOSE, NULL, NULL);
		SendMessage(hOffsetWnd, WM_CLOSE, NULL, NULL);
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_SETTING_OPEN, 1);
		break;
	}

	return 0;
}



//Target�̃E�B���h�v���V�[�W�� 0.17�Œǉ�
LRESULT CALLBACK WndProcTarget(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rect, rectgroup;
	_TCHAR szName[0x20];
	_TCHAR szID[0x10];
	_TCHAR szParam[MAX_PATH + 0x20];
	HWND hPol;
	static RECT stWindowRect, stButtonRect;
	static HBRUSH hBrush;
	POINT point = { 0,0 };

	switch (msg) {
	case WM_INITDIALOG:
		SetWindowPos(hWnd, NULL, g_Target.x, g_Target.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		GetWindowRect(hWnd, &rect);
		GetWindowRect(GetDlgItem(hWnd, IDC_GROUP_TARGET), &rectgroup);
		GetWindowRect(hWnd, &stWindowRect);//0.18�Œǉ�
		GetWindowRect(GetDlgItem(hWnd, IDC_BUTTON_TARGET_ADD), &stButtonRect);
		ClientToScreen(hWnd, &point);
		stButtonRect.top = abs(stButtonRect.top - point.y);
		stButtonRect.bottom = abs(stButtonRect.bottom - point.y);
		stButtonRect.left = abs(stButtonRect.left - point.x);
		stButtonRect.right = abs(stButtonRect.right - point.x);
		for (int i = 0; i<4; i++) {
			if (_tcslen(g_Link.link[i])) {
				SetDlgItemText(hWnd, IDC_BUTTON_TARGET_LINK1 + i, g_Link.link[i]);
			}
			if (_tcslen(g_Link.url[i]) == 0) {
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_TARGET_LINK1 + i), false);
			}
		}
		if (g_Link.open) {//0.18�ŕύX
			g_Link.open = 0;
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_TARGET_LINK_OPEN, NULL);
		}
		else {
			g_Link.open = 1;
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_TARGET_LINK_OPEN, NULL);
		}
		if (g_Target_Add.default_open) {
			g_Link.add = 0;
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_TARGET_ADD, NULL);
		}
		hBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));//0.18�Œǉ�
		SetTimer(hWnd, TIMER_ID_TARGET, 200, NULL);
		break;

	case WM_TIMER:
		KillTimer(hWnd, TIMER_ID_TARGET);
		UpdateTarget(hWnd);
		if (g_Link.add) {
			hPol = GetHWNDFromPID(g_ProcessID);
			if (hPol) {//0.35�ŕύX
				WINDOWPLACEMENT wndPlace;//0.35�ŕύX
				GetWindowPlacement(hPol, &wndPlace);
				rect.left = wndPlace.rcNormalPosition.left;
				rect.top = wndPlace.rcNormalPosition.top;
				SetWindowPos(hWnd, NULL, rect.left + g_Target_Add.x, rect.top + g_Target_Add.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				SendMessage(GetDlgItem(hWnd, IDC_BUTTON_TARGET_ADD), WM_KILLFOCUS, NULL, NULL);
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		SetTimer(hWnd, TIMER_ID_TARGET, 200, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_TARGET_ADD://0.18�Œǉ�
			if (g_Link.add) {
				SetWindowLong(hWnd, GWL_STYLE, DS_3DLOOK | DS_CENTER | DS_MODALFRAME | DS_SHELLFONT | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU);
				SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
				MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_TARGET_ADD), stButtonRect.left, stButtonRect.top, stButtonRect.right - stButtonRect.left, stButtonRect.bottom - stButtonRect.top, 1);
				SetWindowPos(hWnd, NULL, stWindowRect.left, stWindowRect.top, stWindowRect.right - stWindowRect.left, stWindowRect.bottom - stWindowRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
				SetDlgItemText(hWnd, IDC_BUTTON_TARGET_ADD, _T("�d�˂�"));
				g_Link.add = 0;
			}
			else {
				GetWindowRect(hWnd, &stWindowRect);
				SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
				SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
				SetLayeredWindowAttributes(hWnd, GetSysColor(COLOR_3DFACE), NULL, LWA_COLORKEY);
				GetWindowRect(GetDlgItem(hWnd, IDC_EDIT_TARGET_DISTANCE), &rect);
				MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_TARGET_ADD), rect.right - stWindowRect.left + 3, 2, stButtonRect.right - stButtonRect.left, stButtonRect.bottom - stButtonRect.top, 1);
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, stWindowRect.right - stWindowRect.left + stButtonRect.right - stButtonRect.left + 5, stButtonRect.bottom - stButtonRect.top + 4, SWP_FRAMECHANGED);
				SetDlgItemText(hWnd, IDC_BUTTON_TARGET_ADD, _T("�߂�"));
				g_Link.add = 1;
			}
			break;
		case IDC_BUTTON_TARGET_LINK_OPEN:
			if (g_Link.open) {
				GetWindowRect(hWnd, &rect);
				GetWindowRect(GetDlgItem(hWnd, IDC_GROUP_TARGET), &rectgroup);
				SetWindowPos(hWnd, NULL, 0, 0, rect.right - rect.left, rectgroup.top - rect.top + 4, SWP_NOMOVE | SWP_NOOWNERZORDER);
				SetDlgItemText(hWnd, IDC_BUTTON_TARGET_LINK_OPEN, _T("�J��"));
				g_Link.open = 0;
			}
			else {
				GetWindowRect(hWnd, &rect);
				GetWindowRect(GetDlgItem(hWnd, IDC_GROUP_TARGET), &rectgroup);
				SetWindowPos(hWnd, NULL, 0, 0, rect.right - rect.left, rectgroup.bottom - rect.top + 4, SWP_NOMOVE | SWP_NOOWNERZORDER);
				SetDlgItemText(hWnd, IDC_BUTTON_TARGET_LINK_OPEN, _T("�B��"));
				g_Link.open = 1;
			}
			break;
		case IDC_BUTTON_TARGET_LINK1:
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_CHANGEDID, szID, sizeof(szID) / sizeof(_TCHAR) - 1);
			if (_tcstoul(szID, NULL, 16) >= 0x400)MessageBox(hWnd, _T("PC�̓_��"), HAKOSIRO, MB_OK | MB_ICONERROR);
			else if (_tcslen(szName) >= 3) {
				_stprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[0], szName);
				ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
			}
			else MessageBox(hWnd, _T("�^�[�Q�b�g���I������Ă��܂���"), HAKOSIRO, MB_OK | MB_ICONERROR);
			break;
		case IDC_BUTTON_TARGET_LINK2:
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_CHANGEDID, szID, sizeof(szID) / sizeof(_TCHAR) - 1);
			if (_tcstoul(szID, NULL, 16) >= 0x400)MessageBox(hWnd, _T("PC�̓_��"), HAKOSIRO, MB_OK | MB_ICONERROR);
			else if (_tcslen(szName) >= 3) {
				_stprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[1], szName);
				ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
			}
			else MessageBox(hWnd, _T("�^�[�Q�b�g���I������Ă��܂���"), HAKOSIRO, MB_OK | MB_ICONERROR);
			break;
		case IDC_BUTTON_TARGET_LINK3:
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_CHANGEDID, szID, sizeof(szID) / sizeof(_TCHAR) - 1);
			if (_tcstoul(szID, NULL, 16) >= 0x400)MessageBox(hWnd, _T("PC�̓_��"), HAKOSIRO, MB_OK | MB_ICONERROR);
			else if (_tcslen(szName) >= 3) {
				_stprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[2], szName);
				ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
			}
			else MessageBox(hWnd, _T("�^�[�Q�b�g���I������Ă��܂���"), HAKOSIRO, MB_OK | MB_ICONERROR);
			break;
		case IDC_BUTTON_TARGET_LINK4:
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_NAME, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
			GetDlgItemText(hWnd, IDC_EDIT_TARGET_CHANGEDID, szID, sizeof(szID) / sizeof(_TCHAR) - 1);
			if (_tcstoul(szID, NULL, 16) >= 0x400)MessageBox(hWnd, _T("PC�̓_��"), HAKOSIRO, MB_OK | MB_ICONERROR);
			else if (_tcslen(szName) >= 3) {
				_stprintf(szParam, _T("%s %s%s"), g_Link.cmd, g_Link.url[3], szName);
				ShellExecute(NULL, NULL, g_Link.exe, szParam, NULL, SW_SHOWDEFAULT);
			}
			else MessageBox(hWnd, _T("�^�[�Q�b�g���I������Ă��܂���"), HAKOSIRO, MB_OK | MB_ICONERROR);
			break;
		}
		break;

	case WM_CTLCOLORSTATIC://0.18�Œǉ�
		SetBkMode((HDC)wp, TRANSPARENT);
		if (g_Link.add)SetTextColor((HDC)wp, GetSysColor(COLOR_CAPTIONTEXT));//0.18a�ŕύX
		else SetTextColor((HDC)wp, GetSysColor(COLOR_WINDOWTEXT));//0.18a�ŕύX
		return (LRESULT)hBrush;
		break;

	case WM_CLOSE:
		DeleteObject(hBrush);
		if (g_Link.add) {//0.18�Œǉ�
			SetWindowLong(hWnd, GWL_STYLE, DS_3DLOOK | DS_CENTER | DS_MODALFRAME | DS_SHELLFONT | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU);
			SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
			MoveWindow(GetDlgItem(hWnd, IDC_BUTTON_TARGET_ADD), stButtonRect.left, stButtonRect.top, stButtonRect.right - stButtonRect.left, stButtonRect.bottom - stButtonRect.top, 1);
			SetWindowPos(hWnd, NULL, stWindowRect.left, stWindowRect.top, stWindowRect.right - stWindowRect.left, stWindowRect.bottom - stWindowRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			SetDlgItemText(hWnd, IDC_BUTTON_TARGET_ADD, _T("�d�˂�"));
		}
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_TARGET_OPEN, 1);
		break;
	}

	return 0;
}

//Target���̍X�V 0.17�Œǉ�
int UpdateTarget(HWND hWnd)
{
	WORD wChangeID;
	static WORD wPastChangeID = -2;
	PARTYINFORMATION TargetInfo;
	static PARTYINFORMATION PastTargetInfo;
	static HWND sthWnd;
	_TCHAR szHpp[0x10];
	_TCHAR szDistance[0x10];
	_TCHAR szChangedID[0x10];
	_TCHAR szFixedID[0x10];

	wChangeID = GetTargetID();
	GetDlgItemText(hWnd, IDC_EDIT_TARGET_FIXEDID, szFixedID, sizeof(szFixedID) / sizeof(_TCHAR) - 1);

	if (wChangeID != wPastChangeID || _tcslen(szFixedID) == 0) {//0.18�ŕύX
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_NAME), _T("-"));
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_HPP), _T("-"));
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_DISTANCE), _T("-"));
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_CHANGEDID), _T("-"));
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_FIXEDID), _T("-"));
		wPastChangeID = -2;//0.18�Œǉ�
	}
	if (wChangeID == -1) {
		wPastChangeID = -2;
		return -1;
	}

	TargetInfo = GetNPCInfo(wChangeID);
	if (TargetInfo.code == -1) {
		wPastChangeID = -2;
		return -2;
	}
	else if (_tcslen(TargetInfo.name) < 3) {
		wPastChangeID = -2;
		return -3;
	}

	_stprintf(szHpp, _T("%d%%"), TargetInfo.hpp);
	_stprintf(szDistance, _T("%.1f"), sqrt(TargetInfo.distance));
	_stprintf(szChangedID, _T("%03X"), TargetInfo.id);
	_stprintf(szFixedID, _T("%X"), TargetInfo.fixedid);
	if (wChangeID != wPastChangeID) {
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_NAME), TargetInfo.name);
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_CHANGEDID), szChangedID);
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_FIXEDID), szFixedID);
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_HPP), szHpp);
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_DISTANCE), szDistance);
	}
	else {
		if (TargetInfo.hpp != PastTargetInfo.hpp)
			SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_HPP), szHpp);
		if (TargetInfo.distance != PastTargetInfo.distance) {
			SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TARGET_DISTANCE), szDistance);
		}
	}

	wPastChangeID = wChangeID;
	PastTargetInfo = TargetInfo;

	return 0;
}

//�^�[�Q�b�g���Ă���NPC/PC��ID���擾 0.17�Œǉ�
WORD GetTargetID()
{
	DWORD dwAddress;
	WORD change_id;

	dwAddress = g_DllAddress + g_Offset.Target;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		return -1;
	}
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &change_id, sizeof(change_id), NULL)) {
		return -1;
	}

	return change_id;
}

//�G���AID���擾���� WORD�o�[�W���� 0.21�Œǉ�
WORD GetArea2ByteID()
{
	DWORD dwAddress;
	WORD wAreaID = 0;

	dwAddress = g_DllAddress + g_Offset.Map;
	ReadProcessMemory(g_polHandle, (const void *)dwAddress, &wAreaID, sizeof(WORD), NULL);

	return wAreaID;
}


//AutoCombat�̃E�B���h�v���V�[�W�� 0.28�Œǉ�
LRESULT CALLBACK WndProcCombat(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static BYTE stbyStartFlag;
	static CKeyboardHelper* pKeyboardHelper;
	static HMODULE hModule;
	static BYTE stbyUseWindower;
	static INPUTKEYSTATE stPastKeyState;
#ifndef UPLOAD
	INPUTKEYSTATE KeyState;
	INPUT InKey[10];
	const BYTE constWindowerNumpad[] = { NPNUM0,NPNUM1,NPNUM2,NPNUM3,NPNUM4,NPNUM5,NPNUM6,NPNUM7,NPNUM8,NPNUM9 };
#endif

	switch (msg) {
	case WM_INITDIALOG:
#ifdef UPLOAD
		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMBAT_CAMP), false);
		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMBAT_LIST), false);
		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMBAT_START), false);
		EnableWindow(GetDlgItem(hWnd, IDC_LIST_COMBAT), false);
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_HPP), false);
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_YM), false);
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_YM_SEARCH), false);
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_WAIT_TIME), false);//0.29�Œǉ�
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_WS), false);
		EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_NMID), false);
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKBOX_COMBAT_WS), false);
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKBOX_COMBAT_NMID), false);
		EnableWindow(GetDlgItem(hWnd, IDC_RADIO_COMBAT_DOWN), false);
		EnableWindow(GetDlgItem(hWnd, IDC_RADIO_COMBAT_UP), false);
#else
		SetWindowPos(hWnd, NULL, g_Combat.x, g_Combat.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		//���X�g�r���[��EX�X�^�C���t�^
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST_COMBAT),
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER,//0.47a�Ń_�u���o�b�t�@�ǉ�
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER
		);

		//���X�g�r���[�̃T�u�N���X��
		SetWindowLong(GetDlgItem(hWnd, IDC_LIST_COMBAT), GWL_WNDPROC, (LONG)WndProcSubClass_Combat);

		LVCOLUMN col;
		ZeroMemory(&col, sizeof(col));
		//�J�����쐬
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 55;
		col.iSubItem = 0;
		col.pszText = _T("ID");
		if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_COMBAT), 0, &col)) {
			return 0;
		}
		col.cx = 150;
		col.iSubItem = 1;
		col.pszText = _T("Name");
		if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST_COMBAT), 1, &col)) {
			return 0;
		}

		//�����l�ݒ�
		SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_YM, 10, false);
		SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_YM_SEARCH, 50, false);
		SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_WAIT_TIME, 5000, false);//0.29�Œǉ�
		SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_HPP, 5, false);
		CheckDlgButton(hWnd, IDC_RADIO_COMBAT_UP, BM_SETCHECK);

		//WindowerHelper�ݒ�
		if (g_UseWindowerHelper && (hModule = LoadLibrary(_T("WindowerHelper.dll"))) != NULL) {//0.1.1.0�Œǉ�
			if ((DLL_CreateKeyboardHelper = (void* (__stdcall*)(char*))GetProcAddress(hModule, "CreateKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find CreateKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_DeleteKeyboardHelper = (void(__stdcall*)(void*))GetProcAddress(hModule, "DeleteKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find DeleteKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_CKHSetKey = (void(__stdcall*)(void*, unsigned char, bool))GetProcAddress(hModule, "CKHSetKey")) == NULL) {
				MessageBox(hWnd, _T("Cant find CKHSetKey()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			SetWindowText(hWnd, _T("Combat - WindowerHelper.dll"));
			char szHandler[MAX_PATH];
			_stprintfA(szHandler, "WindowerMMFKeyboardHandler_%d", g_ProcessID);//0.43i�ŕύX
			pKeyboardHelper = (CKeyboardHelper*)DLL_CreateKeyboardHelper(szHandler);
		}
		else if (g_UseWindowerHelper) {
			MessageBox(hWnd, _T("Cant find WindowerHelper.dll"), NULL, MB_ICONERROR);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			return 0;
		}
		stbyUseWindower = g_UseWindowerHelper;//0.29�Œǉ�
		stbyStartFlag = 0;//0.30�Œǉ�

		SetTimer(hWnd, TIMER_ID_COMBAT, 200, NULL);
#endif
		break;

#ifndef UPLOAD
	case WM_TIMER:
		KillTimer(hWnd, TIMER_ID_COMBAT);
		if (stbyUseWindower != g_UseWindowerHelper) {
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		}
		ZeroMemory(&KeyState, sizeof(INPUTKEYSTATE));

		if (stbyStartFlag) {
			CombatLoop(hWnd, pKeyboardHelper, &KeyState);
		}

		if (g_UseWindowerHelper && pKeyboardHelper) {
			for (int i = 0; i<10; i++) {
				if (KeyState.numpad[i] && stPastKeyState.numpad[i] == 0) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], true);
				}
				else if (KeyState.numpad[i] == 0 && stPastKeyState.numpad[i]) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], false);
				}
			}
		}
		else if (!g_UseWindowerHelper && GetForegroundWindow() == FindWindow(_T("FFXiClass"), NULL) && GetViewPoint() == 1) {
			InKey[0].type = INPUT_KEYBOARD;
			for (int i = 0; i<10; i++) {
				if (KeyState.numpad[i] && stPastKeyState.numpad[i] == 0) {
					InKey[0].ki.dwFlags = 0;
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
				else if (KeyState.numpad[i] == 0 && stPastKeyState.numpad[i]) {
					InKey[0].ki.dwFlags = KEYEVENTF_KEYUP;
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
			}
		}
		stPastKeyState = KeyState;

		if (!stbyStartFlag || GetViewPoint() != 1) {
			if (g_UseWindowerHelper && pKeyboardHelper) {
				for (int i = 0; i<10; i++) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], false);
				}
			}
			else if (!g_UseWindowerHelper && GetForegroundWindow() == FindWindow(_T("FFXiClass"), NULL)) {
				InKey[0].type = INPUT_KEYBOARD;
				InKey[0].ki.dwFlags = KEYEVENTF_KEYUP;
				for (int i = 0; i<10; i++) {
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
			}
		}

		SetTimer(hWnd, TIMER_ID_COMBAT, 200, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_COMBAT_LIST:
			MakeList(hWnd);
			break;
		case IDC_BUTTON_COMBAT_CAMP:
			SetCamp(hWnd);
			break;
		case IDC_BUTTON_COMBAT_START:
			if (stbyStartFlag) {
				stbyStartFlag = 0;
				SetDlgItemText(hWnd, IDC_BUTTON_COMBAT_START, _T("�J�n"));
			}
			else {
				WORD wAreaID = GetArea2ByteID();
				if (wAreaID != 0 && wAreaID == GetDlgItemInt(hWnd, IDC_EDIT_COMBAT_AREA, NULL, false)) {
					stbyStartFlag = 1;
					SetDlgItemText(hWnd, IDC_BUTTON_COMBAT_START, _T("��~"));
				}
			}
			break;
		case IDC_CHECKBOX_COMBAT_NMID:
			if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_COMBAT_NMID), BM_GETCHECK, 0, 0)) {
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_NMID), true);
			}
			else {
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT_COMBAT_NMID), false);
			}
			break;
		}
		break;
#endif

	case WM_CLOSE:
#ifndef UPLOAD
		if (stbyStartFlag) {//0.34�Œǉ�
			stbyStartFlag = 0;
			SendMessage(hWnd, WM_TIMER, 0, 0);
		}
		if (g_UseWindowerHelper && pKeyboardHelper)DLL_DeleteKeyboardHelper(pKeyboardHelper);
#endif
		FreeLibrary(hModule);
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_COMBAT_OPEN, 1);
		break;
	}

	return 0;
}

//�����̃A�N�e�B�r�e�B(���)���擾 0.28�Œǉ�
/* XiAceLite������p�@ACTIVITY_INFO = Cast�̃A�N�V����
'** �A�N�e�B�r�e�B
Public Enum eActivity As Short
Standing = 0
Fighting = 1
Dead1 = 2
Dead2 = 3
CutScene = 4
Chocobo = 5
Fishing = 6
Healing = 33
FishBite = 38
Obtained = 39
RodBreak = 40
LineBreak = 41
LostCatch = 43
CatchMonster = 42
Synthing = 44
Sitting = 47
WaitBite = 50
Unknown = 28525 '2011.Feb�̃o�[�W�����A�b�v����NPC�̃X�e�[�^�X�����������l�q�B
*/
WORD GetActivityInfo()
{
	WORD wAct = 0;
	DWORD dwAddress;

	dwAddress = g_DllAddress + g_Offset.ActivityInfo;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &wAct, sizeof(WORD), NULL)) {
		return -1;
	}

	return wAct;
}

#ifndef UPLOAD

//NPC�̃��X�g���쐬�B�ʏ�2���A���Ă���͂��@0.28�Œǉ�
#define NPCLIST_ID	0x1C//0.43h'1�ŕύX
#define NPCLIST_SIZE	0x20//0.43h'1�ŕύX
//NPCLIST�쐬�̍ő吔
#define MAX_SEARCH	0x400
int MakeList(HWND hWnd)
{
	LVITEM item;
	_TCHAR szId[0x05];
	_TCHAR szName[0x20];

	if (FALSE == ListView_DeleteAllItems(GetDlgItem(hWnd, IDC_LIST_COMBAT))) {
		return -1;
	}

	//�A�C�e���ǉ�
	for (int i = 0, j = 0; i<MAX_SEARCH; i++) {
		//id,name�擾
		if (GetList(i, szId, szName) != 1) {
			return 2;
		}
		//ID��!=0 && Name�̈ꕶ���ڂ��啶���̎��A2�����ڂ��������̎����X�g�ɒǉ��@����"???" ���͏��߂��X�y�[�X" Veridical Conflux #??"(�G�R�[�Y)
		if (
			_tcstoul(szId, NULL, 16) != 0 &&
			(
			(isupper(szName[0]) != 0 && islower(szName[1]) != 0) ||
				_tcscmp (szName, _T("???")) == 0 ||
				(wcsncmp(szName, _T(" "), 1) == 0 && isupper(szName[1]) && islower(szName[2]))
				)
			) {
			item.mask = LVIF_TEXT | LVIF_PARAM;//LVIF_PARAM�ǉ��Ń\�[�g�I��
			item.iItem = j;//�s
			item.iSubItem = 0;//��
			item.pszText = szId;//id
			item.lParam = i;//�\�[�g�plParam��ݒ�
			if (-1 == ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST_COMBAT), &item)) {
				return 0;
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = 1;//��
			item.pszText = szName;//Name
			if (FALSE == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST_COMBAT), &item)) {
				return 0;
			}
			j++;
		}
	}

	return 1;
}

//����������NPC��id�A���O�ǂݍ��݁@0.28�Œǉ�
int GetList(int counter, _TCHAR *id, _TCHAR *name)
{
	char dump[0x20];
	DWORD dwAddress;
	int nID;

	dwAddress = g_DllAddress + g_Offset.NpcList;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}
	dwAddress = dwAddress + NPCLIST_SIZE * counter;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, NPCLIST_SIZE, NULL)) {
		return -1;
	}

	memcpy(&nID, &dump[NPCLIST_ID], sizeof(int));
	nID = nID & 0x0FFF;

	wcscpy(id, _T("0"));//id������
	if (dump[NPCLIST_SIZE - 1] == 0x01 && nID >= counter && nID <= 0x400) {
		_stprintf(id, _T("%03X"), nID);//id�쐬
	}
	else if (dump[NPCLIST_SIZE - 1] != 0x01 && dump[NPCLIST_SIZE - 1] != 0x00) {//List�����I��
		return 0;
	}

	dump[NPCLIST_ID] = 0;//�ő啶����΍�
	mbstowcs(name, dump, NPCLIST_ID + 1);

	return 1;
}

//�L�����v���Z�b�g 0.28�Œǉ�
int SetCamp(HWND hWnd)
{
	PARTYINFORMATION myInfo = GetPartyInfo(0);
	SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_X, (UINT)myInfo.x, true);
	SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_Y, (UINT)myInfo.y, true);
	SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_Z, (UINT)myInfo.z, true);
	SetDlgItemInt(hWnd, IDC_EDIT_COMBAT_AREA, GetArea2ByteID(), false);

	return 1;
}

//Combat���̃��C�� 0.28�Œǉ�
int CombatLoop(HWND hWnd, CKeyboardHelper *pKeyboardHelper, INPUTKEYSTATE *pKeyState)
{
	static BYTE stState = 1;//���݂̏�� �ҋ@��:1 �퓬��:2 ���݊m�F��:3 HP�ɋA�Ғ�:4 �C�x���g���Ȃ�:5
	static DWORD dwTimer = 0;

	INPUT InKey[10];

	//�őO�ʊm�F
	if (!g_UseWindowerHelper && GetForegroundWindow() != FindWindow(_T("FFXiClass"), NULL)) {
		return -1;
	}
	//�������_�m�F
	if (GetViewPoint() != 1) {
		return -1;
	}

	//�����̏�Ԏ擾
	switch (GetActivityInfo()) {
	case 0://Standing
		if (stState == 2) {//���O�܂Ő퓬���Ă���
			dwTimer = timeGetTime();
			stState = 3;
		}
		else if (stState == 3) {//�w�莞�ԑҋ@���ė��݊m�F��
			if (dwTimer < timeGetTime() - 1000) {
				WORD wTargetId = GetTargetID();
				if (wTargetId < 0x400 && wTargetId != 0 && GetNPCInfo(wTargetId).state) {//���݊m�F
					dwTimer = 0;
					ffhook_putcmd(_T("/a on"));//�u/a on�v�𑗐M
					stState = 1;
				}
				else if (wTargetId != 0 && wTargetId > 0x400) {
					//�L�[�𑗂�
					if (g_UseWindowerHelper && pKeyboardHelper) {
						DLL_CKHSetKey(pKeyboardHelper, ESCAPE, true);
						DLL_CKHSetKey(pKeyboardHelper, ESCAPE, false);
					}
					else {
						InKey[0].type = INPUT_KEYBOARD;
						InKey[0].ki.wVk = VK_ESCAPE;
						InKey[0].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
						InKey[0].ki.dwFlags = 0;
						InKey[1].type = INPUT_KEYBOARD;
						InKey[1].ki.wVk = VK_ESCAPE;
						InKey[1].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
						InKey[1].ki.dwFlags = KEYEVENTF_KEYUP;
						SendInput(2, InKey, sizeof(INPUT));
					}
				}

				int nWaitTime = GetDlgItemInt(hWnd, IDC_EDIT_COMBAT_WAIT_TIME, NULL, false);//0.29�Œǉ�
				if (nWaitTime < 0 || nWaitTime > 100000)nWaitTime = 5000;//0.29�Œǉ�
																		 //if(dwTimer < timeGetTime() - 5000){//5�b�ҋ@
				if (dwTimer < timeGetTime() - nWaitTime) {//�w�莞�ԑҋ@ 0.28�ŕύX
					dwTimer = timeGetTime();
					stState = 1;
				}
			}
		}
		else if (stState == 4) {//�A�Ғ�
			if (BackToCamp(hWnd, pKeyboardHelper, pKeyState) == 0) {//�A�҂��I�����
				dwTimer = 0;
				stState = 1;
			}
			else {//�A�Ғ����T�[�`���Ă݂�
				if (MoveToNPC(hWnd, pKeyboardHelper, pKeyState) != -1) {
					dwTimer = 0;
					stState = 1;
				}
			}

			if (GetTargetID() != 0 && GetTargetID() < 0x400) {//����
				ffhook_putcmd(_T("/a on"));//�u/a on�v�𑗐M
				stState = 1;
			}
		}
		else {//�ҋ@��
			int nRetMovToNPC = MoveToNPC(hWnd, pKeyboardHelper, pKeyState);
			if (nRetMovToNPC == -1) {//������Ȃ�
				stState = 4;//�L�����v�ɖ߂��Ă݂�
				dwTimer = timeGetTime();
			}

			if (nRetMovToNPC == 0 || nRetMovToNPC == -1) {//���܂�Ă邩�m�F�\
				WORD wTargetId = GetTargetID();
				if (wTargetId < 0x400 && wTargetId != 0 && GetNPCInfo(wTargetId).state) {//���݊m�F
					ffhook_putcmd(_T("/a on"));//�u/a on�v�𑗐M
				}
				else if (wTargetId != 0 && wTargetId > 0x400) {
					//�L�[�𑗂�
					if (g_UseWindowerHelper && pKeyboardHelper) {
						DLL_CKHSetKey(pKeyboardHelper, ESCAPE, true);
						DLL_CKHSetKey(pKeyboardHelper, ESCAPE, false);
					}
					else {
						InKey[0].type = INPUT_KEYBOARD;
						InKey[0].ki.wVk = VK_ESCAPE;
						InKey[0].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
						InKey[0].ki.dwFlags = 0;
						InKey[1].type = INPUT_KEYBOARD;
						InKey[1].ki.wVk = VK_ESCAPE;
						InKey[1].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
						InKey[1].ki.dwFlags = KEYEVENTF_KEYUP;
						SendInput(2, InKey, sizeof(INPUT));
					}
				}
			}
			else {
				stState = 1;
			}
		}
		break;
	case 1://Fighting
		AutoCombat(hWnd, pKeyboardHelper, pKeyState);
		stState = 2;
		break;
	default:
		stState = 5;
		break;
	}

	return 1;
}

//�퓬���̏���
int AutoCombat(HWND hWnd, CKeyboardHelper *pKeyboardHelper, INPUTKEYSTATE *pKeyState)
{
	//INPUT InKey[10];

	//�^�[�Q�b�g���m�F
	WORD wTargetID = GetTargetID();
	PARTYINFORMATION targetInfo;
	if (wTargetID >= 0x400)return -1;
	else {
		targetInfo = GetNPCInfo(wTargetID);
		if (targetInfo.code != 0) {
			return -2;
		}
	}

	//�������m�F
	PARTYINFORMATION myInfo = GetPartyInfo(0);
	if (myInfo.code != 0) {
		return -10;
	}

	//�^�[�Q�b�g�Ƃ̋����ƕ��p���m�F
	if (targetInfo.distance == 0 || targetInfo.distance > 50 * 50) {//0.28a�ŕύX
		return -20;
	}


	//5�ŉ�ʂ̐؂�ւ����s���Ă�����
	//4��6�Ŋp�x��+-10�x�ɂ��킹��
	double dKakudo = 180 / 3.141592*(atan2(targetInfo.z - myInfo.z, targetInfo.x - myInfo.x) + myInfo.radian);
	if (dKakudo < 0)dKakudo += 360;
	if (dKakudo <= 350 && dKakudo >= 180) {//�E�ɉ�]
										   //�L�[�𑗂�
		pKeyState->numpad[6] = 1;
		return 1;
	}
	else if (dKakudo >= 10 && dKakudo <= 180) {//���ɉ�]
											   //�L�[�𑗂�
		pKeyState->numpad[4] = 1;
		return 1;
	}

	//�^�[�Q�b�g�ɋ߂Â�
	if (targetInfo.distance > 3.5*3.5) {//0.30�ŕύX
										//�L�[�𑗂�
		pKeyState->numpad[8] = 1;
		return 1;
	}
	//�^�[�Q�b�g���߂�����0.28a�Œǉ�
	if (targetInfo.distance <= 1 * 1) {
		//�L�[�𑗂�
		pKeyState->numpad[2] = 1;
		return 1;
	}

	//�퓬���̏���(WS)
	if ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_COMBAT_WS), BM_GETCHECK, 0, 0)) && myInfo.tp >= 1000 && targetInfo.hpp < 100) {//0.5.0.2�ŕύX
		_TCHAR szWsText[0xA0];
		_TCHAR szCmd[0x100];
		GetDlgItemText(hWnd, IDC_EDIT_COMBAT_WS, szWsText, sizeof(szWsText) / sizeof(_TCHAR) - 1);
		if (_tcslen(szWsText) >= 1) {
			_stprintf(szCmd, _T("/ws %s <t>"), szWsText);
		}
		else {
			return -50;
		}

		int nUseHpp = GetDlgItemInt(hWnd, IDC_EDIT_COMBAT_HPP, NULL, false);
		if (nUseHpp < 0 || nUseHpp >= 100)nUseHpp = 5;
		if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RADIO_COMBAT_UP), BM_GETCHECK, 0, 0)) {
			if (targetInfo.hpp >= nUseHpp) {
				ffhook_putcmd(szCmd);
			}
		}
		else {
			if (targetInfo.hpp <= nUseHpp) {
				ffhook_putcmd(szCmd);
			}
		}
	}

	return 1;
}

//�A�ҏ��� �A�Ғ��̖߂�l1 �A�ҏI����0
int BackToCamp(HWND hWnd, CKeyboardHelper *pKeyboardHelper, INPUTKEYSTATE *pKeyState)
{
	_TCHAR szBuf[10];
	INPUT InKey[10];

	//�L�����v�̍��W���擾
	PARTYINFORMATION targetInfo;
	ZeroMemory(&targetInfo, sizeof(PARTYINFORMATION));
	GetDlgItemText(hWnd, IDC_EDIT_COMBAT_X, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
	targetInfo.x = (float)wcstod(szBuf, NULL);
	GetDlgItemText(hWnd, IDC_EDIT_COMBAT_Y, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
	targetInfo.y = (float)wcstod(szBuf, NULL);
	GetDlgItemText(hWnd, IDC_EDIT_COMBAT_Z, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
	targetInfo.z = (float)wcstod(szBuf, NULL);
	int nCampDistance;
	GetDlgItemText(hWnd, IDC_EDIT_COMBAT_YM, szBuf, sizeof(szBuf) / sizeof(_TCHAR) - 1);
	nCampDistance = wcstol(szBuf, NULL, 10);
	if (nCampDistance < 1) {
		nCampDistance = 1;
	}

	//�����̍��W���擾
	PARTYINFORMATION myInfo = GetPartyInfo(0);
	targetInfo.distance = pow(myInfo.x - targetInfo.x, 2) + pow(myInfo.z - targetInfo.z, 2);

	//���݊m�F 0.28a�Œǉ�
	WORD wTargetId = GetTargetID();
	if (wTargetId < 0x400 && wTargetId != 0 && GetNPCInfo(wTargetId).state) {//���݊m�F
		ffhook_putcmd(_T("/a on"));//�u/a on�v�𑗐M
		return 1;
	}
	else if (wTargetId != 0 && wTargetId > 0x400) {
		//�L�[�𑗂�
		if (g_UseWindowerHelper && pKeyboardHelper) {
			DLL_CKHSetKey(pKeyboardHelper, ESCAPE, true);
			DLL_CKHSetKey(pKeyboardHelper, ESCAPE, false);
		}
		else {
			InKey[0].type = INPUT_KEYBOARD;
			InKey[0].ki.wVk = VK_ESCAPE;
			InKey[0].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
			InKey[0].ki.dwFlags = 0;
			InKey[1].type = INPUT_KEYBOARD;
			InKey[1].ki.wVk = VK_ESCAPE;
			InKey[1].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
			InKey[1].ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(2, InKey, sizeof(INPUT));
		}
		return 1;
	}

	//�͈͓��Ȃ牽�����Ȃ�
	if (targetInfo.distance <= nCampDistance * nCampDistance) {
		return 0;
	}

	//�ړ�
	double dKakudo = 180 / 3.141592*(atan2(targetInfo.z - myInfo.z, targetInfo.x - myInfo.x) + myInfo.radian);
	if (dKakudo < 0)dKakudo += 360;
	if (dKakudo <= 340 && dKakudo >= 180) {//�E�ɉ�]
										   //�L�[�𑗂�
		pKeyState->numpad[6] = 1;
		return 1;
	}
	else if (dKakudo >= 20 && dKakudo <= 180) {//���ɉ�]
											   //�L�[�𑗂�
		pKeyState->numpad[4] = 1;
		return 1;
	}

	//�L�����v�ɖ߂�
	if (targetInfo.distance >= nCampDistance * nCampDistance) {
		//�L�[�𑗂�
		pKeyState->numpad[8] = 1;
		return 1;
	}

	return 0;
}

//�Ώۂɋ߂Â� �Ԃ�l-1:������Ȃ� 0:���܂�Ă邩�m�F�\
int MoveToNPC(HWND hWnd, CKeyboardHelper *pKeyboardHelper, INPUTKEYSTATE *pKeyState)
{
	_TCHAR szID[10];
	INPUT InKey[10];
	PARTYINFORMATION npcInfo;
	PARTYINFORMATION targetInfo;
	PARTYINFORMATION myInfo;
	struct {
		int nId;
		float fDistance;
	}MostNearlyNPC;

	//�T�[�`�͈̔͂�ݒ�
	int nSerchDistance = GetDlgItemInt(hWnd, IDC_EDIT_COMBAT_YM_SEARCH, NULL, false);
	if (nSerchDistance < 1) {
		nSerchDistance = 50;
	}
	MostNearlyNPC.nId = 0;
	MostNearlyNPC.fDistance = (float)(nSerchDistance * nSerchDistance);

	//��ԋ߂��^�[�Q�b�g��NPC���T�[�`
	for (int i = ListView_GetItemCount(GetDlgItem(hWnd, IDC_LIST_COMBAT)) - 1; i >= 0; i--) {
		if (ListView_GetCheckState(GetDlgItem(hWnd, IDC_LIST_COMBAT), i)) {
			ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_COMBAT), i, 0, szID, sizeof(szID) / sizeof(_TCHAR));
			int nID = wcstol(szID, NULL, 16);
			npcInfo = GetNPCInfo(nID);
			if (npcInfo.code != -3) {
				if (npcInfo.inrange && npcInfo.hpp == 100 && npcInfo.distance <= MostNearlyNPC.fDistance) {
					MostNearlyNPC.nId = npcInfo.id;
					MostNearlyNPC.fDistance = npcInfo.distance;
					targetInfo = npcInfo;
				}
			}
		}
	}
	//NM�D��
	if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_COMBAT_NMID), BM_GETCHECK, 0, 0)) {
		GetDlgItemText(hWnd, IDC_EDIT_COMBAT_NMID, szID, sizeof(szID) / sizeof(_TCHAR) - 1);
		int nID = wcstol(szID, NULL, 16);
		npcInfo = GetNPCInfo(nID);
		if (npcInfo.code != -3) {
			if (npcInfo.inrange && npcInfo.hpp == 100) {
				MostNearlyNPC.nId = npcInfo.id;
				MostNearlyNPC.fDistance = npcInfo.distance;
				targetInfo = npcInfo;
			}
		}
	}
	if (MostNearlyNPC.nId == 0)return -1;

	//�����̏����擾
	myInfo = GetPartyInfo(0);

	//�^�[�Q�b�g�Ƃ̋����ƕ��p���m�F
	int nDistance = (int)sqrt(targetInfo.distance);
	//5�ŉ�ʂ̐؂�ւ����s���Ă�����
	//4��6�Ŋp�x��+-20�x�ɂ��킹��
	double dKakudo = 180 / 3.141592*(atan2(targetInfo.z - myInfo.z, targetInfo.x - myInfo.x) + myInfo.radian);
	if (dKakudo < 0)dKakudo += 360;
	if (dKakudo <= 340 && dKakudo >= 180) {//�E�ɉ�]
										   //�L�[�𑗂�
		pKeyState->numpad[6] = 1;
		return 1;
	}
	else if (dKakudo >= 20 && dKakudo <= 180) {//���ɉ�]
											   //�L�[�𑗂�
		pKeyState->numpad[4] = 1;
		return 1;
	}

	//�^�[�Q�b�g�ɋ߂Â�(5Ym)
	if (targetInfo.distance >= 5 * 5) {
		//�L�[�𑗂�
		pKeyState->numpad[8] = 1;
		return 1;
	}

	//�^�[�Q�b�g�����킹��
	if (GetTargetID() == 0) {
		ffhook_putcmd(_T("/targetnpc"));
		return 0;
	}
	else if (targetInfo.id != GetTargetID()) {
		//�^�[�Q�b�g��ύX����
		//�L�[�𑗂�
		if (g_UseWindowerHelper && pKeyboardHelper) {
			DLL_CKHSetKey(pKeyboardHelper, TAB, true);
			DLL_CKHSetKey(pKeyboardHelper, TAB, false);
		}
		else {
			InKey[0].type = INPUT_KEYBOARD;
			InKey[0].ki.wVk = VK_TAB;
			InKey[0].ki.wScan = MapVirtualKey(VK_TAB, 0);
			InKey[0].ki.dwFlags = 0;
			InKey[1].type = INPUT_KEYBOARD;
			InKey[1].ki.wVk = VK_TAB;
			InKey[1].ki.wScan = MapVirtualKey(VK_TAB, 0);
			InKey[1].ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(2, InKey, sizeof(INPUT));
		}
		return 0;
	}
	else {
		//�u/a on�v�R�}���h�𑗐M����
		ffhook_putcmd(_T("/a on"));
	}

	return 0;
}


//LISTVIEW�R���g���[���̃T�u�N���X�� 0.29�Œǉ�
LRESULT CALLBACK WndProcSubClass_Combat(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_KEYDOWN && wp == VK_SPACE && GetKeyState(VK_CONTROL) & 0x80 || msg == WM_LBUTTONDBLCLK) {
		int nItemCount = ListView_GetItemCount(hWnd);
		if (nItemCount == 0) {
			return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
		}
		//���݂̑I���ʒu���擾
		int nItemNow = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);
		BYTE nItemState = ListView_GetCheckState(hWnd, nItemNow);
		_TCHAR szName[0x20], szCmpName[0x20];
		ListView_GetItemText(hWnd, nItemNow, 1, szName, sizeof(szName) / sizeof(_TCHAR) - 1);
		for (int i = 0; i<nItemCount; i++) {
			ListView_GetItemText(hWnd, i, 1, szCmpName, sizeof(szCmpName) / sizeof(_TCHAR) - 1);
			if (_tcscmp (szCmpName, szName) == 0) {
				if (nItemState != 0) {
					ListView_SetCheckState(hWnd, i, 0);//���Z�b�g
				}
				else {
					ListView_SetCheckState(hWnd, i, 1);//�Z�b�g
				}
			}
		}
		return 0;
	}
	else if (msg == WM_CHAR) {
		if (wp >= 'a' && wp <= 'z' || wp >= 'A' && wp <= 'Z') {//NPCLIST��������
			_TCHAR szChar[2];
			_TCHAR szName[0x20];
			_stprintf(szChar, _T("%c"), toupper(wp));
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
					if (wcsncmp(szChar, szName, 1) == 0) {
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
					if (wcsncmp(szChar, szName, 1) == 0) {
						ListView_SetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
						ListView_EnsureVisible(hWnd, i, FALSE);
						break;
					}
				}
			}

			return 0;
		}
	}

	return CallWindowProc(WndProcMainClass, hWnd, msg, wp, lp);
}


#endif


//Fellow�̃E�B���h�v���V�[�W�� 0.34�Œǉ�
LRESULT CALLBACK WndProcFellow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static int nStart = 0;
	static CKeyboardHelper* pKeyboardHelper;
	static HMODULE hModule;
	static INPUTKEYSTATE stPastKeyState;
	INPUTKEYSTATE KeyState;
	INPUT InKey[10];
	const BYTE constWindowerNumpad[] = { NPNUM0,NPNUM1,NPNUM2,NPNUM3,NPNUM4,NPNUM5,NPNUM6,NPNUM7,NPNUM8,NPNUM9 };
	static int FellowMode = 0;

	switch (msg) {
	case WM_INITDIALOG:
		if (g_UseWindowerHelper && (hModule = LoadLibrary(_T("WindowerHelper.dll"))) != NULL) {
			if ((DLL_CreateKeyboardHelper = (void* (__stdcall*)(char*))GetProcAddress(hModule, "CreateKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find CreateKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_DeleteKeyboardHelper = (void(__stdcall*)(void*))GetProcAddress(hModule, "DeleteKeyboardHelper")) == NULL) {
				MessageBox(hWnd, _T("Cant find DeleteKeyboardHelper()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			if ((DLL_CKHSetKey = (void(__stdcall*)(void*, unsigned char, bool))GetProcAddress(hModule, "CKHSetKey")) == NULL) {
				MessageBox(hWnd, _T("Cant find CKHSetKey()"), _T("WindowerHelper.dll"), MB_ICONERROR);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				return 0;
			}
			SetWindowText(hWnd, _T("Fellow - WindowerHelper.dll"));
			char szHandler[MAX_PATH];
			_stprintf(szHandler, _T("WindowerMMFKeyboardHandler_%d"), g_ProcessID);//0.43i�ŕύX
			pKeyboardHelper = (CKeyboardHelper*)DLL_CreateKeyboardHelper(szHandler);
		}
		else if (g_UseWindowerHelper) {
			MessageBox(hWnd, _T("Cant find WindowerHelper.dll"), NULL, MB_ICONERROR);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			return 0;
		}

		SetWindowPos(hWnd, 0, g_Fellow.x, g_Fellow.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		SetDlgItemText(hWnd, IDC_COMBO_FELLOW_MASTER_NAME, GetPartyInfo(1).name);
		SetDlgItemInt(hWnd, IDC_EDIT_FELLOW_HPP, 20, false);
		CheckDlgButton(hWnd, IDC_RADIO_FELLOW_WS_NOTUSE, true);
		CheckDlgButton(hWnd, IDC_RADIO_FELLOW_UP, true);
		//�^�C�}�[�J�n
		SetTimer(hWnd, TIMER_ID_FELLOW, 1000, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_COMBO_FELLOW_MASTER_NAME:
			PARTYINFORMATION pInfo;
			if (HIWORD(wp) == CBN_DROPDOWN) {
				while (SendMessage(GetDlgItem(hWnd, IDC_COMBO_FELLOW_MASTER_NAME), CB_GETCOUNT, 0, 0) != 0) {//�R���{�{�b�N�X��������
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_FELLOW_MASTER_NAME), CB_DELETESTRING, 0, 0);
				}
				for (int i = 1; i<18; i++) {
					pInfo = GetPartyInfo(i);
					if (pInfo.inrange && _tcslen((const _TCHAR*)pInfo.name) >= 3 && pInfo.inrange) {
						SendMessage(GetDlgItem(hWnd, IDC_COMBO_FELLOW_MASTER_NAME), CB_ADDSTRING, 0, (LPARAM)pInfo.name);
					}
				}
			}
			break;

		case IDC_RADIO_FELLOW_WS_NOTUSE:
			FellowMode = 0;
			break;

		case IDC_RADIO_FELLOW_WS_USE:
			FellowMode = 1;
			break;

		case IDC_RADIO_FELLOW_WS_FIRST:
			FellowMode = 2;
			break;

		case IDC_RADIO_FELLOW_WS_NEXT:
			FellowMode = 3;
			break;

		case IDC_BUTTON_FELLOW_START:
			if (nStart == 0) {
				SetDlgItemText(hWnd, IDC_BUTTON_FELLOW_START, _T("��~"));
				nStart = 1;
			}
			else {
				SetDlgItemText(hWnd, IDC_BUTTON_FELLOW_START, _T("�J�n"));
				nStart = 0;
			}
		}
		break;

	case WM_TIMER:
		KillTimer(hWnd, TIMER_ID_FELLOW);
		ZeroMemory(&KeyState, sizeof(INPUTKEYSTATE));

		if (!(IsDlgButtonChecked(hWnd, IDC_CHECKBOX_FELLOW_CAST) && Casting() > 0) && nStart && GetViewPoint() == 1 && GetSubTarget() == 0) {//�T�u�^�[�Q�b�g���o�Ă���Ƃ� 0.35�ŕύX �r�����͈ړ����Ȃ�0.43d�ŕύX
			_TCHAR szTargetName[0x20];

			GetWindowText(GetDlgItem(hWnd, IDC_COMBO_FELLOW_MASTER_NAME), szTargetName, 0x20 - 1);
			if (_tcslen(szTargetName)) {
				PARTYINFORMATION masterInfo;
				for (int i = 1; i < 18; i++) {
					masterInfo = GetPartyInfo(i);
					if (_tcscmp (masterInfo.name, szTargetName) == 0) {
						break;
					}
				}
				if (_tcscmp (masterInfo.name, szTargetName) == 0 && masterInfo.inrange) {
					_TCHAR szCmd[MAX_PATH];
					if (masterInfo.state == 0x01) {//�퓬��
						if (GetTargetID() != 0 && GetTargetID() < 0x400) {//�G���^�Q���Ă���
							if (GetPartyInfo(0).state != 1) {
								_stprintf(szCmd, _T("/assist %s"), masterInfo.name);
								ffhook_putcmd(szCmd);
								Sleep(500);
								ffhook_putcmd(_T("/attack on"));
							}
							else if (GetLockOn() == 0) {
								ffhook_putcmd(_T("/lockon"));
							}
							else {
								AutoCombatFellow(hWnd, pKeyboardHelper, &KeyState, masterInfo, FellowMode);
							}
						}
						else {//�G���^�Q���Ă��Ȃ�
							_stprintf(szCmd, _T("/assist %s"), masterInfo.name);
							ffhook_putcmd(szCmd);
						}
					}
					else {//�A�C�h����
						if (GetTargetID() != 0 && GetTargetID() < 0x400) {//�G���^�Q���Ă���
							if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX_FELLOW_LINK), BM_GETCHECK, 0, 0)) {
								if (GetPartyInfo(0).state == 0x01) {
									ffhook_putcmd(_T("/attack off"));
								}
								else {
									_stprintf(szCmd, _T("/ta %s"), masterInfo.name);
									ffhook_putcmd(szCmd);
								}
							}
							else {//�����N���Ă����� 0.35�Œǉ�
								_stprintf(szCmd, _T("/ta %s"), masterInfo.name);
								ffhook_putcmd(szCmd);
							}
						}
						else {//�G���^�Q���Ă��Ȃ�
							if (_tcscmp (GetNPCInfo(GetTargetID()).name, szTargetName) == 0) {
								if (GetLockOn() == 0) {
									ffhook_putcmd(_T("/lockon"));
								}
							}
							else {
								_stprintf(szCmd, _T("/ta %s"), masterInfo.name);
								ffhook_putcmd(szCmd);
							}
						}

						if (masterInfo.inrange && masterInfo.distance >= 1) {//�O�i 0.35�ŕύX
							KeyState.numpad[8] = 1;
						}
					}
				}
			}
		}

		if (g_UseWindowerHelper && pKeyboardHelper) {
			for (int i = 0; i < 10; i++) {
				if (KeyState.numpad[i] && stPastKeyState.numpad[i] == 0) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], true);
				}
				else if (KeyState.numpad[i] == 0 && stPastKeyState.numpad[i]) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], false);
				}
			}
		}
		else if (!g_UseWindowerHelper && GetForegroundWindow() == FindWindow(_T("FFXiClass"), NULL) && GetViewPoint() == 1) {
			InKey[0].type = INPUT_KEYBOARD;
			for (int i = 0; i < 10; i++) {
				if (KeyState.numpad[i] && stPastKeyState.numpad[i] == 0) {
					InKey[0].ki.dwFlags = 0;
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
				else if (KeyState.numpad[i] == 0 && stPastKeyState.numpad[i]) {
					InKey[0].ki.dwFlags = KEYEVENTF_KEYUP;
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
			}
		}
		stPastKeyState = KeyState;

		if (!nStart || GetViewPoint() != 1) {
			if (g_UseWindowerHelper && pKeyboardHelper) {
				for (int i = 0; i < 10; i++) {
					DLL_CKHSetKey(pKeyboardHelper, constWindowerNumpad[i], false);
				}
			}
			else if (!g_UseWindowerHelper && GetForegroundWindow() == FindWindow(_T("FFXiClass"), NULL)) {
				InKey[0].type = INPUT_KEYBOARD;
				InKey[0].ki.dwFlags = KEYEVENTF_KEYUP;
				for (int i = 0; i < 10; i++) {
					InKey[0].ki.wVk = VK_NUMPAD0 + i;
					InKey[0].ki.wScan = MapVirtualKey(VK_NUMPAD0 + i, 0);
					SendInput(1, InKey, sizeof(INPUT));
				}
			}
		}

		SetTimer(hWnd, TIMER_ID_FELLOW, 100, NULL);
		break;

	case WM_CLOSE:
		if (nStart) {
			nStart = 0;
			SendMessage(hWnd, WM_TIMER, 0, 0);
		}
		if (g_UseWindowerHelper && pKeyboardHelper)DLL_DeleteKeyboardHelper(pKeyboardHelper);
		FreeLibrary(hModule);
		SendMessage(GetParent(hWnd), WM_COMMAND, IDM_FELLOW_OPEN, 1);
		break;
	}

	return 0;
}


//�퓬���̏��� Fellow�p 0.34�Œǉ�
int AutoCombatFellow(HWND hWnd, CKeyboardHelper *pKeyboardHelper, INPUTKEYSTATE *pKeyState, PARTYINFORMATION masterInfo, int FellowMode)
{
	static DWORD stPastMasterTP;
	//�^�[�Q�b�g���m�F
	WORD wTargetID = GetTargetID();
	PARTYINFORMATION targetInfo;
	if (wTargetID >= 0x400)return -1;
	else {
		targetInfo = GetNPCInfo(wTargetID);
		if (targetInfo.code != 0) {
			return -2;
		}
	}

	if (GetViewPoint() != 1)return -3;

	//�������m�F
	PARTYINFORMATION myInfo = GetPartyInfo(0);
	if (myInfo.code != 0) {
		return -10;
	}

	if (targetInfo.distance == 0 || targetInfo.distance > 50 * 50) {
		return -20;
	}

	if (GetLockOn() == 0) {
	}


	//5�ŉ�ʂ̐؂�ւ����s���Ă�����
	//4��6�Ŋp�x��+-15�x�ɂ��킹��
	double dKakudo = 180 / 3.141592 * (atan2(targetInfo.z - myInfo.z, targetInfo.x - myInfo.x) + myInfo.radian);
	if (dKakudo < 0)dKakudo += 360;
	if (dKakudo <= 345 && dKakudo >= 15) {
		pKeyState->numpad[8] = 1;
		return 1;
	}

	//�^�[�Q�b�g�ɋ߂Â�
	if (targetInfo.distance > 3.1*3.1) {
		pKeyState->numpad[8] = 1;
		return 1;
	}
	//�^�[�Q�b�g���߂�����
	if (targetInfo.distance <= 1 * 1) {
		pKeyState->numpad[2] = 1;
		return 1;
	}

	//�퓬���̏���(WS)
	if (FellowMode && myInfo.tp >= 1000 && targetInfo.hpp < 100) {//0.5.0.2�ŕύX
		_TCHAR szWsText[0xA0];
		_TCHAR szCmd[0x100];
		GetDlgItemText(hWnd, IDC_EDIT_FELLOW_WS, szWsText, sizeof(szWsText) / sizeof(_TCHAR) - 1);
		if (_tcslen(szWsText) >= 1) {
			_stprintf(szCmd, _T("/ws %s <t>"), szWsText);
		}
		else {
			return -50;
		}

		int nUseHpp = GetDlgItemInt(hWnd, IDC_EDIT_FELLOW_HPP, NULL, false);
		if (nUseHpp < 0 || nUseHpp >= 100)nUseHpp = 5;
		if (FellowMode == 1) {//������
			if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RADIO_FELLOW_UP), BM_GETCHECK, 0, 0)) {
				if (targetInfo.hpp >= nUseHpp) {
					ffhook_putcmd(szCmd);
				}
			}
			else {
				if (targetInfo.hpp <= nUseHpp) {
					ffhook_putcmd(szCmd);
				}
			}
		}
		else if (FellowMode == 2) {//�挂��
			if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RADIO_FELLOW_UP), BM_GETCHECK, 0, 0)) {
				if (targetInfo.hpp >= nUseHpp && masterInfo.tp >= 1000) {//0.5.0.2�ŕύX
					ffhook_putcmd(szCmd);
				}
			}
			else {
				if (targetInfo.hpp <= nUseHpp && masterInfo.tp >= 1000) {//0.5.0.2�ŕύX
					ffhook_putcmd(szCmd);
				}
			}
		}
		else if (FellowMode == 3) {//�㌂��
			if (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RADIO_FELLOW_UP), BM_GETCHECK, 0, 0)) {
				if (targetInfo.hpp >= nUseHpp && stPastMasterTP > 850 && masterInfo.tp < 300 && masterInfo.tp != 0) {//0.5.0.2�ŕύX
					Sleep(4000);
					ffhook_putcmd(szCmd);
				}
			}
			else {
				if (targetInfo.hpp <= nUseHpp && stPastMasterTP > 850 && masterInfo.tp < 300 && masterInfo.tp != 0) {//0.5.0.2�ŕύX
					Sleep(4000);
					ffhook_putcmd(szCmd);
				}
			}
		}
	}
	stPastMasterTP = masterInfo.tp;

	return 1;
}



//�w�b�_�[�̕���ύX 0.34�Œǉ�
int ListChangeHeaderWidth(HWND hWnd, int nColumn, int nWidth)
{
	HDITEM hdi;
	ZeroMemory(&hdi, sizeof(HDITEM));
	hdi.mask = HDI_WIDTH;
	hdi.cxy = nWidth;

	Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST_PC)), nColumn, &hdi);
	if (IsWindow(GetDlgItem(hWnd, IDC_LIST_RESERVED))) {
		Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd, IDC_LIST_RESERVED)), nColumn, &hdi);
	}

	return 1;
}

//�w�b�_�[�̕����擾 0.34�Œǉ�
//hDlgWnd�̓R���g���[���̃n���h���Ȃ̂Œ���
int ListGetHeaderWidth(HWND hDlgWnd, int nColumn)
{
	HDITEM hdi;
	ZeroMemory(&hdi, sizeof(HDITEM));
	hdi.mask = HDI_WIDTH;
	Header_GetItem(hDlgWnd, nColumn, &hdi);

	return hdi.cxy;
}

//0.41�Œǉ�
//�t�@�C�����J���Ńt�@�C�������擾
int GetFileName(HWND hWnd, _TCHAR* szFileName, int nFileNameSize)
{
	_TCHAR szCurrentDir[MAX_PATH];
	const _TCHAR szFilter[] = _T("*.wav\0*.wav\0All (*.*)\0*.*\0\0");
	int ret;

	ZeroMemory(szFileName, nFileNameSize);

	//�J�����g�t�H���_���擾
	GetCurrentDirectory(sizeof(szCurrentDir) / sizeof(_TCHAR), szCurrentDir);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szCurrentDir;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = nFileNameSize;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("wav");

	if (GetOpenFileName(&ofn) == 0) {//���s��
		if (CommDlgExtendedError() != 0) {//0�̎��̓L�����Z��
			ret = -1;
		}
		else {
			ret = 0;
		}
	}
	else {//������
		ret = 1;
	}

	//�J�����g�t�H���_��ݒ�
	SetCurrentDirectory(szCurrentDir);

	return ret;
}

