#include "struct.h"
#include "windowerhelper.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
//DWORD WINAPI ThreadLoop(LPVOID);
unsigned WINAPI ThreadLoop(void*);
unsigned WINAPI ThreadLoopSort(void*);
void ShowVersion(HWND);
int compfunc(const void*,const void*);

#ifndef _T
#define _T(x)	L##x
#endif


#define BAZAAR_STRING	_T("Bazaar")
#define ID_PROCESS_TIMER	1000//0.08で追加

#define MAX_PID_COUNT	255//PIDをサーチする最大数
#define NPC_MEM_SIZE	0x200//メモリを読み込むサイズ

#define NOWSTA_HOSEI		(-0x06)//開始位置が微妙に違うみたいなので補正
#define NOWSTA_MEM_CHANGEID	0x1C

#define COLUMN_MAX_COUNT	5
#define COLUMN_PC	0
#define COLUMN_NAME	1
#define COLUMN_PRICE	2
#define COLUMN_NUMBER	3
#define COLUMN_NOTE	4


#ifdef MAIN_CPP//main.cpp内での定義
DWORD g_OffsetNpcMap;
DWORD g_OffsetNowSta;
DWORD g_OffsetBazaar;
BYTE g_AutoOffset;//0.08で追加
BYTE g_UseFilterPrice;
int g_FilterPrice;
int g_KeyDelay;
int g_WaitUpdate;
int g_Retry;
DWORD g_ProcessID;
DWORD g_DllAddress;//FFXiMain.dllのアドレス
HANDLE g_polHandle = 0;
NPC_MEM_MAP NPC_MEM;//NPC_MEMのオフセット情報
volatile BYTE g_byThreadFlag;
_TCHAR g_szSound[MAX_PATH];
WANTED g_Wanted[100];
_TCHAR g_szBlackListName[100][0x20];//0.03で追加
int g_ColumnWidth[10];
BYTE g_SaveWindow;
BYTE g_SaveColumn;
BYTE g_ListColor;
BYTE g_AutoScroll;//0.02で追加
BYTE g_BlackList;//0.03で追加
BYTE g_ListUp;//0.03で追加
BYTE g_WantedIni;//利用するwanted%02d.ini 0.03で追加 ←0.04でwanted用のini数に変更
_TCHAR g_WantedIniFileName[MAX_PATH] = _T("wanted.ini");//0.04で追加
BYTE g_UseFFXIAH;//0.04で追加
_TCHAR g_FFXIAH[MAX_PATH];//0.04で追加
BYTE g_TargetName;//0.08で追加
BYTE g_ExtractSave;//0.04で追加
_TCHAR g_DirSave[MAX_PATH];//0.04で追加
COLORREF g_ColorLow;
COLORREF g_ColorHigh;
volatile BYTE g_BazaarOrder;//0.09で追加
volatile BYTE g_BazaarNumWait05;//0.09で追加
volatile BYTE g_BazaarNumWait10;//0.09で追加
volatile BYTE g_BazaarNumWait20;//0.09で追加
volatile BYTE g_BazaarNumWait21;//0.09で追加


#else//それ以外での定義
extern DWORD g_OffsetNpcMap;
extern DWORD g_OffsetNowSta;
extern DWORD g_OffsetBazaar;
extern BYTE g_AutoOffset;//0.08で追加
extern BYTE g_UseFilterPrice;
extern int g_FilterPrice;
extern int g_KeyDelay;
extern int g_WaitUpdate;
extern int g_Retry;
extern DWORD g_ProcessID;
extern DWORD g_DllAddress;//FFXiMain.dllのアドレス
extern HANDLE g_polHandle;
extern NPC_MEM_MAP NPC_MEM;//NPC_MEMのオフセット情報
extern _TCHAR g_szSound[MAX_PATH];
extern WANTED g_Wanted[100];
extern _TCHAR g_szBlackListName[100][0x20];
extern int g_ColumnWidth[10];
extern BYTE g_SaveWindow;
extern BYTE g_SaveColumn;
extern BYTE g_ListColor;
extern BYTE g_AutoScroll;//0.02で追加
extern BYTE g_BlackList;//0.03で追加
extern BYTE g_ListUp;//0.03で追加
extern BYTE g_WantedIni;//0.03で追加
extern _TCHAR g_WantedIniFileName[MAX_PATH];//0.04で追加
extern BYTE g_UseFFXIAH;//0.04で追加
extern _TCHAR g_FFXIAH[MAX_PATH];//0.04で追加
extern BYTE g_ExtractSave;//0.04で追加
extern BYTE g_TargetName;//0.08で追加
extern _TCHAR g_DirSave[MAX_PATH];//0.04で追加
extern COLORREF g_ColorLow;
extern COLORREF g_ColorHigh;
extern volatile BYTE g_BazaarOrder;//0.09で追加
extern volatile BYTE g_BazaarNumWait05;//0.09で追加
extern volatile BYTE g_BazaarNumWait10;//0.09で追加
extern volatile BYTE g_BazaarNumWait20;//0.09で追加
extern volatile BYTE g_BazaarNumWait21;//0.09で追加

#endif

