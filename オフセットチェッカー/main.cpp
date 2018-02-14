//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define MAINCPP

#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <CommCtrl.h>
#include <Shlwapi.h>//0.05で追加
#include "main.h"
#include "process.h"
#include "searchaddr.h"
#include "resource.h"
#include "format.h"//0.05bで追加

#pragma comment(lib,"version.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"shlwapi.lib")//0.05で追加

// SysLinkコントロール用にCommonControlのバージョンを6.0に指定
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

//WinMain
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)WndProc);

#ifdef DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return true;
}


//ウィンドウプロシージャIDD_DIALOG
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int pId;
	HANDLE hPol = 0;
	static HMODULE hModule;
	static _TCHAR stsztDllFullPath[MAX_PATH] = _T("");//0.05で追加
	static SYSTEMTIME stSysTimeUpdata;//0.05で追加

	switch (msg) {
	case WM_INITDIALOG:
		//ロケールをOSデフォルトに設定
		//VCの場合これが無いとIOで文字化けする
		_wsetlocale(LC_ALL, _T(""));

		//アイコンの設定
		SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)LoadImage((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDI_ICON),IMAGE_ICON,16,16,LR_DEFAULTCOLOR|LR_SHARED));

		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		ShowVersion(hWnd);
		return TRUE;//0.5で変更

	case WM_COMMAND:
		if (LOWORD(wp) == IDC_BUTTON_START) {
			wsprintf(stsztDllFullPath, _T(""));//0.05で追加
			SetDlgItemText(hWnd, IDC_EDIT_OFFSET, _T(""));//0.05で追加
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), FALSE);//0.05で追加
			g_Offset.dwDllAddress = GetDllModuleAddress(1, &pId, &hPol, stsztDllFullPath);//0.05で変更
			GetOffsetAddressAll(hPol, g_Offset.dwDllAddress);
			_TCHAR *szText;
			szText = (_TCHAR*)malloc(0x1000 * sizeof(_TCHAR));
			if (szText == NULL) {
				MessageBox(hWnd, _T("メモリの割当てに失敗"), NULL, MB_ICONERROR);
				return TRUE;//0.5で変更
			}
			HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_OFFSET);//0.05bで一括出力から分割出力に変更
			ShowWindow(hEdit, SW_HIDE);//ちらつき軽減
			SetFocus(hEdit);
			SetWindowText(hEdit, _T("※出力された値は全て参考値です\r\n\r\n"));
			wsprintf(szText, FORMAT_MAPPU);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_HEALER);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_BAZAAR);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_GRAPHICSID2JOBS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_NPCMEM);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_SOUKOBAN);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_REALTIMELOGGETS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_XIACE);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_KODOUGU);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_FISHKAI);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			//0.10で追加
			wsprintf(szText, FORMAT_MADMAN);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_MADMOVE);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_PUPSTATUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_ARGUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_CAST);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_DORO_DLL);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_OTHER);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_FFXI_CHOCOBO);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_FFXI_LOGIN_POINT);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_XINPC);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			wsprintf(szText, FORMAT_CHOCOBO_CHECKER_PLUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);

#ifdef HOOK_INFO
			_TCHAR szHexLog[0x100] = _T("");
			for (int i = 0; i<0x20; i++) {
				_TCHAR szBuff[10];
				swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_offset.Ffxihook_log[i]);
				_tcscat_s(szHexLog, szBuff);
			}
			szHexLog[ARRAYSIZE(szHexLog) - 1] = '\0';
			//0.07で追加
			_TCHAR szHexCmd[0x100] = _T("");
			for (int i = 0; i<0x20; i++) {
				_TCHAR szBuff[10];
				swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_offset.Ffxihook_cmd[i]);
				_tcscat_s(szHexCmd, szBuff);
			}
			szHexCmd[ARRAYSIZE(szHexCmd) - 1] = '\0';
			wsprintf(szText, FORMAT_HOOK_INFO);//0.05bで変更
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
#endif
			SendMessage(hEdit, WM_VSCROLL, SB_TOP, 0);
			ShowWindow(hEdit, SW_NORMAL);
			free(szText);
			if (GetTimeStamp_Update(stsztDllFullPath, &stSysTimeUpdata) == 1) {//0.05で追加
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), TRUE);//0.05で追加
			}
			if (hPol)CloseHandle(hPol);
			return TRUE;//0.05で変更
		}
		else if (LOWORD(wp) == IDC_BUTTON_OPEN) {
			_TCHAR *szFileName;
			szFileName = (_TCHAR*)malloc(MAX_PATH * sizeof(_TCHAR));
			int nRet = GetFileName(hWnd, szFileName, MAX_PATH);
			if (nRet == -1) {
				MessageBox(hWnd, _T("ファイルが開けません"), NULL, MB_ICONERROR);
			}
			else if (nRet == 1) {
				wsprintf(stsztDllFullPath, _T(""));//0.05で追加
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET, _T(""));//0.05で追加
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), FALSE);//0.05で追加
#ifdef DEBUG//0.03で追加
				if (g_offset.dwDllAddress)FreeLibrary((HMODULE)g_offset.dwDllAddress);
#endif
				hPol = OpenProcess(PROCESS_VM_READ, false, GetCurrentProcessId());
				g_Offset.dwDllAddress = (DWORD)LoadLibrary(szFileName);
				GetOffsetAddressAll(hPol, g_Offset.dwDllAddress);
				_TCHAR *szText;
				szText = (_TCHAR*)malloc(0x1000 * sizeof(_TCHAR));
				if (szText == NULL) {
					MessageBox(hWnd, _T("メモリの割当てに失敗"), NULL, MB_ICONERROR);
				}
				else {
					HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_OFFSET);//0.05bで一括出力から小分けに変更
					ShowWindow(hEdit, SW_HIDE);//ちらつき軽減
					SetFocus(hEdit);
					SetWindowText(hEdit, _T("※出力された値は全て参考値です\r\n\r\n"));
					wsprintf(szText, FORMAT_MAPPU);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_HEALER);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_BAZAAR);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_GRAPHICSID2JOBS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_NPCMEM);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_SOUKOBAN);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_REALTIMELOGGETS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_XIACE);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_KODOUGU);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_FISHKAI);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_MADMAN);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_MADMOVE);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_PUPSTATUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_ARGUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_CAST);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_DORO_DLL);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_OTHER);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_FFXI_CHOCOBO);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_FFXI_LOGIN_POINT);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_XINPC);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					wsprintf(szText, FORMAT_CHOCOBO_CHECKER_PLUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);

#ifdef HOOK_INFO
					_TCHAR szHexLog[0x100] = _T("");
					for (int i = 0; i<0x20; i++) {
						_TCHAR szBuff[10];
						swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_offset.Ffxihook_log[i]);
						_tcscat_s(szHexLog, szBuff);
					}
					szHexLog[ARRAYSIZE(szHexLog) - 1] = '\0';
					//0.07で追加
					_TCHAR szHexCmd[0x100] = _T("");
					for (int i = 0; i<0x20; i++) {
						_TCHAR szBuff[10];
						swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_offset.Ffxihook_cmd[i]);
						_tcscat_s(szHexCmd, szBuff);
					}
					szHexCmd[ARRAYSIZE(szHexCmd) - 1] = '\0';
					wsprintf(szText, FORMAT_HOOK_INFO);//0.05bで変更
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
#endif
					SendMessage(hEdit, WM_VSCROLL, SB_TOP, 0);
					ShowWindow(hEdit, SW_NORMAL);
					wsprintf(stsztDllFullPath, _T("%s"), szFileName);
					if (GetTimeStamp_Update(stsztDllFullPath, &stSysTimeUpdata) == 1) {//0.05で追加
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), TRUE);//0.05で追加
					}
					free(szText);
				}
				if (hPol)CloseHandle(hPol);
#ifndef DEBUG//0.03で追加
				if (g_Offset.dwDllAddress)FreeLibrary((HMODULE)g_Offset.dwDllAddress);
#endif
			}
			free(szFileName);
			return TRUE;//0.5で変更
		}
		else if (LOWORD(wp) == IDC_BUTTON_SAVE) {//0.05で追加
			_TCHAR sztFileName[MAX_PATH];
			wsprintf(sztFileName, _T("%s_%d%02d%02d.txt"), PathFindFileName(stsztDllFullPath), stSysTimeUpdata.wYear, stSysTimeUpdata.wMonth, stSysTimeUpdata.wDay);
			int nRet = Save(sztFileName, hWnd, GetDlgItem(hWnd, IDC_EDIT_OFFSET));
			if (nRet == 1) {
				MessageBox(hWnd, _T("保存しました"), PathFindFileName(sztFileName), MB_OK);
			}
			else if (nRet < 0) {
				MessageBox(hWnd, _T("保存に失敗しました"), PathFindFileName(sztFileName), MB_ICONERROR);
			}
			return TRUE;
		}
		return TRUE;//0.5で変更

	case WM_NOTIFY://0.05bで追加
		switch (((LPNMHDR)lp)->idFrom) {
		case IDC_LINK:
			if (((PNMLINK)lp)->hdr.code == NM_CLICK) {
				ShellExecuteW(hWnd, _T("open"), ((PNMLINK)lp)->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
			}
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;


	case WM_CLOSE:
		EndDialog(hWnd, 0);//0.5で変更
		return TRUE;//0.5で変更
	}

	return 0;
}

void ShowVersion(HWND hWnd)
{
	_TCHAR szFileName[MAX_PATH];
	DWORD dwBlockSize;
	BYTE *pbyVersionInfo;

	if (GetModuleFileName(NULL, szFileName, sizeof(szFileName) / sizeof(_TCHAR) - 1) == 0) {
		MessageBox(hWnd, _T("Cant get VersionInfo"), NULL, MB_ICONERROR);
	}
	else {
		dwBlockSize = GetFileVersionInfoSize(szFileName, 0);
		pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
		if (pbyVersionInfo) {
			if (GetFileVersionInfo(szFileName, 0, dwBlockSize, (LPVOID)pbyVersionInfo) == FALSE) {
				MessageBox(hWnd, _T("Cant get VersionInfo"), NULL, MB_ICONERROR);
			}
			else {
				struct {
					WORD wLanguage;
					WORD wCodePage;
				} *lpTranslate;
				VerQueryValue((LPVOID)pbyVersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, NULL);
				_TCHAR szDir[0x100];
				wsprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				void *pvVer;
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvVer, NULL);
				wsprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\LegalCopyright"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				void *pvCopyright;
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvCopyright, NULL);
				//0.05bで追加
				//wsprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				void *pvComment;
				//VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvComment, NULL);

				_TCHAR szVersion[0x100];
				wsprintf(szVersion, _T("<a href=\"https://github.com/Hesaraih\">%s Ver %s</a>"), pvCopyright, pvVer);//0.05bで変更
				SetDlgItemText(hWnd, IDC_LINK, szVersion);
			}
		}
		free(pbyVersionInfo);
	}
}


//ファイルを開くでファイル名を取得
int GetFileName(HWND hWnd, _TCHAR* szFileName, int nFileNameSize)
{
	_TCHAR szCurrentDir[MAX_PATH];
	_TCHAR szFFXiDir[MAX_PATH];
	const _TCHAR szFilter[] = _T("FFXiMain.dll\0FFXiMain.dll\0*.dll\0*.dll\0All (*.*)\0*.*\0\0");
	HKEY hKey;
	int ret;

	_tcscpy_s(szFileName, MAX_PATH, _T("FFXiMain.dll"));
	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(szCurrentDir) / sizeof(_TCHAR), szCurrentDir);

	//FF11がインストールされているフォルダを取得
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PlayOnline\\InstallFolder"), 0, KEY_READ, &hKey)) {
		//成功
		DWORD dwType = REG_SZ;
		DWORD cbData = MAX_PATH;
		RegQueryValueEx(hKey, _T("0001"), NULL, &dwType, (LPBYTE)szFFXiDir, &cbData);
		RegCloseKey(hKey);
	}
	else {
		_tcscpy_s(szFFXiDir, MAX_PATH, szCurrentDir);
	}

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szFFXiDir;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = nFileNameSize;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("dll");

	if (GetOpenFileName(&ofn) == 0) {//失敗時
		if (CommDlgExtendedError() != 0) {//0の時はキャンセル
			ret = -1;
		}
		else {
			ret = 0;
		}
	}
	else {//成功時
		ret = 1;
	}

	//カレントフォルダを設定
	SetCurrentDirectory(szCurrentDir);

	return ret;
}


//0.05で追加
//対象ファイル名の更新日時を取得
int GetTimeStamp_Update(_TCHAR *sztDllFullPath, SYSTEMTIME *SysTimeUpdate)
{
	ZeroMemory(SysTimeUpdate, sizeof(SYSTEMTIME));

	if (_tcslen(sztDllFullPath) == 0) {
		return -1;
	}

	HANDLE hFile = CreateFile(sztDllFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);//0.05aで変更
	if (hFile == INVALID_HANDLE_VALUE) {
		return -2;
	}

	FILETIME FileTime, FileTimeLocal;
	BOOL bRet = GetFileTime(hFile, NULL, NULL, &FileTime);

	CloseHandle(hFile);
	if (!bRet) {
		return -3;
	}
	bRet = FileTimeToLocalFileTime(&FileTime, &FileTimeLocal);
	if (!bRet) {
		return -4;
	}

	bRet = FileTimeToSystemTime(&FileTimeLocal, SysTimeUpdate);
	if (!bRet) {
		return -5;
	}

	return 1;
}


//0.05で追加
int Save(_TCHAR *sztFileName, HWND hWnd, HWND hEdit)
{
	_TCHAR szCurrentDir[MAX_PATH];
	const _TCHAR szFilter[] = _T("*.txt\0*.txt\0All (*.*)\0*.*\0\0");
	int ret;

	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(szCurrentDir) / sizeof(_TCHAR), szCurrentDir);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szCurrentDir;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = sztFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = _T("txt");

	if (GetSaveFileName(&ofn) == 0) {//失敗時
		if (CommDlgExtendedError() != 0) {//0の時はキャンセル
			ret = -1;
		}
		else {
			ret = 0;
		}
	}
	else {//成功時
		_TCHAR *sztText;
		sztText = (_TCHAR*)malloc(0x1000 * sizeof(_TCHAR));
		if (sztText == NULL) {
			ret = -2;
		}
		else {
			GetWindowText(hEdit, sztText, 0x1000);
			HANDLE hFile = CreateFile(sztFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				ret = -3;
			}
			else {
				DWORD dwWriteNum;
				BYTE bom[2] = { 0xFF,0xFE };//BOM.
				WriteFile(hFile, bom, (DWORD)(sizeof(bom)), &dwWriteNum, NULL);
				WriteFile(hFile, sztText, _tcslen(sztText) * sizeof(_TCHAR), &dwWriteNum, NULL);
				CloseHandle(hFile);
				ret = 1;
			}
			free(sztText);
		}
	}

	//カレントフォルダを設定
	SetCurrentDirectory(szCurrentDir);

	return ret;
}