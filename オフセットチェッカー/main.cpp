//���j�R�[�h�ŃR���p�C��
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define MAINCPP
#define HOOK_INFO

#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <CommCtrl.h>
#include <Shlwapi.h>//0.05�Œǉ�
#include "main.h"
#include "process.h"
#include "searchaddr.h"
#include "resource.h"
#include "format.h"//0.05b�Œǉ�
#include <crtdbg.h>  

#pragma comment(lib,"version.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"shlwapi.lib")//0.05�Œǉ�

// SysLink�R���g���[���p��CommonControl�̃o�[�W������6.0�Ɏw��
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE hInstance;	// ���݂̃C���^�[�t�F�C�X

//WinMain
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	hInstance = hInst; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)WndProc);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return true;
}


//�E�B���h�E�v���V�[�W��IDD_DIALOG
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int pId;
	HANDLE hPol = 0;
	HICON hIcon;
	static HMODULE hModule;
	static _TCHAR stsztDllFullPath[MAX_PATH] = _T("");//0.05�Œǉ�
	static SYSTEMTIME stSysTimeUpdata;//0.05�Œǉ�

	switch (msg) {
	case WM_INITDIALOG:
		//���P�[����OS�f�t�H���g�ɐݒ�
		//VC�̏ꍇ���ꂪ������IO�ŕ�����������
		_wsetlocale(LC_ALL, _T(""));

		//�A�C�R���̐ݒ�
		hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		ShowVersion(hWnd);
		return TRUE;//0.5�ŕύX

	case WM_COMMAND:
		if (LOWORD(wp) == IDC_BUTTON_START) {
			_stprintf_s(stsztDllFullPath, sizeof(stsztDllFullPath), _T(""));//0.05�Œǉ�
			SetDlgItemText(hWnd, IDC_EDIT_OFFSET, _T(""));//0.05�Œǉ�
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), FALSE);//0.05�Œǉ�
			g_Offset.dwDllAddress = GetDllModuleAddress(1, &pId, &hPol, stsztDllFullPath);//0.05�ŕύX
			GetOffsetAddressAll(hPol, g_Offset.dwDllAddress);
			_TCHAR *szText;
			szText = (_TCHAR*)malloc(0x1000 * sizeof(_TCHAR));
			if (szText == NULL) {
				MessageBox(hWnd, _T("�������̊����ĂɎ��s"), NULL, MB_ICONERROR);
				return TRUE;//0.5�ŕύX
			}
			HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_OFFSET);//0.05b�ňꊇ�o�͂��番���o�͂ɕύX
			ShowWindow(hEdit, SW_HIDE);//������y��
			SetFocus(hEdit);
			SetWindowText(hEdit, _T("���o�͂��ꂽ�l�͑S�ĎQ�l�l�ł�\r\n\r\n"));
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_MAPPU);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_HEALER);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_BAZAAR);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_GRAPHICSID2JOBS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_NPCMEM);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_SOUKOBAN);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_REALTIMELOGGETS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_XIACE);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_KODOUGU);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_FISHKAI);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			//0.10�Œǉ�
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_MADMAN);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_MADMOVE);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_PUPSTATUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_ARGUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_CAST);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_DORO_DLL);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_OTHER);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_FFXI_CHOCOBO);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_FFXI_LOGIN_POINT);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_XINPC);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_CHOCOBO_CHECKER_PLUS);
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);

#ifdef HOOK_INFO
			_TCHAR szHexLog[0x100] = _T("");
			for (int i = 0; i < 0x20; i++) {
				_TCHAR szBuff[10];
				swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_Offset.Ffxihook_log[i]);
				_tcscat_s(szHexLog, szBuff);
			}
			szHexLog[ARRAYSIZE(szHexLog) - 1] = '\0';
			//0.07�Œǉ�
			_TCHAR szHexCmd[0x100] = _T("");
			for (int i = 0; i < 0x20; i++) {
				_TCHAR szBuff[10];
				swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_Offset.Ffxihook_cmd[i]);
				_tcscat_s(szHexCmd, szBuff);
			}
			szHexCmd[ARRAYSIZE(szHexCmd) - 1] = '\0';
			_stprintf_s(szText, 0x1000 * sizeof(_TCHAR), FORMAT_HOOK_INFO);//0.05b�ŕύX
			SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
#endif
			SendMessage(hEdit, WM_VSCROLL, SB_TOP, 0);
			ShowWindow(hEdit, SW_NORMAL);
			free(szText);
			if (GetTimeStamp_Update(stsztDllFullPath, &stSysTimeUpdata) == 1) {//0.05�Œǉ�
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), TRUE);//0.05�Œǉ�
			}
			if (hPol)CloseHandle(hPol);
			return TRUE;//0.05�ŕύX
		}
		else if (LOWORD(wp) == IDC_BUTTON_OPEN) {
			_TCHAR *szFileName;
			szFileName = (_TCHAR*)malloc(MAX_PATH * sizeof(_TCHAR));
			int nRet = GetFileName(hWnd, szFileName, MAX_PATH);
			if (nRet == -1) {
				MessageBox(hWnd, _T("�t�@�C�����J���܂���"), NULL, MB_ICONERROR);
			}
			else if (nRet == 1) {
				_stprintf_s(stsztDllFullPath, _countof(stsztDllFullPath), _T(""));//0.05�Œǉ�
				SetDlgItemText(hWnd, IDC_EDIT_OFFSET, _T(""));//0.05�Œǉ�
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), FALSE);//0.05�Œǉ�
#ifdef _DEBUG//0.03�Œǉ�
				if (g_Offset.dwDllAddress) {
					FreeLibrary((HMODULE)g_Offset.dwDllAddress);
				}
#endif
				hPol = OpenProcess(PROCESS_VM_READ, false, GetCurrentProcessId());
				g_Offset.dwDllAddress = (DWORD)LoadLibrary(szFileName);
				GetOffsetAddressAll(hPol, g_Offset.dwDllAddress);
				_TCHAR *szText;
				szText = (_TCHAR*)malloc(0x1000 * sizeof(_TCHAR));
				if (szText == NULL) {
					MessageBox(hWnd, _T("�������̊����ĂɎ��s"), NULL, MB_ICONERROR);
				}
				else {
					HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_OFFSET);//0.05b�ňꊇ�o�͂��珬�����ɕύX
					ShowWindow(hEdit, SW_HIDE);//������y��
					SetFocus(hEdit);
					SetWindowText(hEdit, _T("���o�͂��ꂽ�l�͑S�ĎQ�l�l�ł�\r\n\r\n"));
					_stprintf_s(szText, sizeof(szText), FORMAT_MAPPU);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_HEALER);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_BAZAAR);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_GRAPHICSID2JOBS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_NPCMEM);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_SOUKOBAN);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_REALTIMELOGGETS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_XIACE);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_KODOUGU);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_FISHKAI);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_MADMAN);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_MADMOVE);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_PUPSTATUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_ARGUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_CAST);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_DORO_DLL);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_OTHER);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_FFXI_CHOCOBO);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_FFXI_LOGIN_POINT);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_XINPC);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					_stprintf_s(szText, sizeof(szText), FORMAT_CHOCOBO_CHECKER_PLUS);
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);

#ifdef HOOK_INFO
					_TCHAR szHexLog[0x100] = _T("");
					for (int i = 0; i < 0x20; i++) {
						_TCHAR szBuff[10];
						swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_Offset.Ffxihook_log[i]);
						_tcscat_s(szHexLog, szBuff);
					}
					szHexLog[ARRAYSIZE(szHexLog) - 1] = '\0';
					//0.07�Œǉ�
					_TCHAR szHexCmd[0x100] = _T("");
					for (int i = 0; i < 0x20; i++) {
						_TCHAR szBuff[10];
						swprintf_s(szBuff, 10, i == 0 ? _T("%02X") : _T(",%02X"), g_Offset.Ffxihook_cmd[i]);
						_tcscat_s(szHexCmd, szBuff);
					}
					szHexCmd[ARRAYSIZE(szHexCmd) - 1] = '\0';
					_stprintf_s(szText, sizeof(szText), FORMAT_HOOK_INFO);//0.05b�ŕύX
					SendMessage(hEdit, EM_SETSEL, (WPARAM)GetWindowTextLength(hEdit), (LPARAM)GetWindowTextLength(hEdit));
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
#endif
					SendMessage(hEdit, WM_VSCROLL, SB_TOP, 0);
					ShowWindow(hEdit, SW_NORMAL);
					_stprintf_s(stsztDllFullPath, _countof(stsztDllFullPath), _T("%s"), szFileName);
					if (GetTimeStamp_Update(stsztDllFullPath, &stSysTimeUpdata) == 1) {//0.05�Œǉ�
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SAVE), TRUE);//0.05�Œǉ�
					}
					free(szText);
				}
				if (hPol)CloseHandle(hPol);
#ifndef _DEBUG//0.03�Œǉ�
				if (g_Offset.dwDllAddress) {
					FreeLibrary((HMODULE)g_Offset.dwDllAddress);
				}
#endif
			}
			free(szFileName);
			return TRUE;//0.5�ŕύX
		}
		else if (LOWORD(wp) == IDC_BUTTON_SAVE) {//0.05�Œǉ�
			_TCHAR sztFileName[MAX_PATH];
			_stprintf_s(sztFileName, _countof(sztFileName), _T("%s_%d%02d%02d.txt"), PathFindFileName(stsztDllFullPath), stSysTimeUpdata.wYear, stSysTimeUpdata.wMonth, stSysTimeUpdata.wDay);
			int nRet = Save(sztFileName, hWnd, GetDlgItem(hWnd, IDC_EDIT_OFFSET));
			if (nRet == 1) {
				MessageBox(hWnd, _T("�ۑ����܂���"), PathFindFileName(sztFileName), MB_OK);
			}
			else if (nRet < 0) {
				MessageBox(hWnd, _T("�ۑ��Ɏ��s���܂���"), PathFindFileName(sztFileName), MB_ICONERROR);
			}
			return TRUE;
		}
		return TRUE;//0.5�ŕύX

	case WM_NOTIFY://0.05b�Œǉ�
		switch (((LPNMHDR)lp)->idFrom) {
		case IDC_LINK:
			//if (((PNMLINK)lp)->hdr.code == NM_CLICK) {
			if (((PNMLINK)lp)->hdr.code == 4294967294) {
				ShellExecuteW(hWnd, _T("open"), ((PNMLINK)lp)->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
			}
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;


	case WM_CLOSE:
		EndDialog(hWnd, 0);//0.5�ŕύX
		return TRUE;//0.5�ŕύX
	}

	return 0;
}

void ShowVersion(HWND hWnd)
{
	_TCHAR szFileName[MAX_PATH];
	DWORD dwBlockSize;
	BYTE *pbyVersionInfo;
	UINT queryLen;

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
				VerQueryValue((LPVOID)pbyVersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &queryLen);
				_TCHAR szDir[0x100];
				_stprintf_s(szDir, _countof(szDir), _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				_TCHAR *pvVer;
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvVer, &queryLen);
				_stprintf_s(szDir, _countof(szDir), _T("\\StringFileInfo\\%04x%04x\\LegalCopyright"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				_TCHAR *pvCopyright;
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvCopyright, &queryLen);
				//0.05b�Œǉ�
				//_stprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				//void *pvComment;
				//VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvComment, &queryLen);

				_TCHAR szVersion[0x100];
				_stprintf_s(szVersion, _countof(szVersion), _T("<a href=\"https://github.com/Hesaraih\">%s Ver %s</a>"), pvCopyright, pvVer);//0.05b�ŕύX
				SetDlgItemText(hWnd, IDC_LINK, szVersion);
			}
		}
		free(pbyVersionInfo);
	}
}


//�t�@�C�����J���Ńt�@�C�������擾
int GetFileName(HWND hWnd, _TCHAR* szFileName, int nFileNameSize)
{
	_TCHAR szCurrentDir[MAX_PATH];
	_TCHAR szFFXiDir[MAX_PATH];
	const _TCHAR szFilter[] = _T("FFXiMain.dll\0FFXiMain.dll\0*.dll\0*.dll\0All (*.*)\0*.*\0\0");
	HKEY hKey;
	int ret;

	_tcscpy_s(szFileName, MAX_PATH, _T("FFXiMain.dll"));
	//�J�����g�t�H���_���擾
	GetCurrentDirectory(sizeof(szCurrentDir) / sizeof(_TCHAR), szCurrentDir);

	//FF11���C���X�g�[������Ă���t�H���_���擾
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PlayOnline\\InstallFolder"), 0, KEY_READ, &hKey)) {
		//����
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


//0.05�Œǉ�
//�Ώۃt�@�C�����̍X�V�������擾
int GetTimeStamp_Update(_TCHAR *sztDllFullPath, SYSTEMTIME *SysTimeUpdate)
{
	ZeroMemory(SysTimeUpdate, sizeof(SYSTEMTIME));

	if (_tcslen(sztDllFullPath) == 0) {
		return -1;
	}

	HANDLE hFile = CreateFile(sztDllFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);//0.05a�ŕύX
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


//0.05�Œǉ�
int Save(_TCHAR *sztFileName, HWND hWnd, HWND hEdit)
{
	_TCHAR szCurrentDir[MAX_PATH];
	const _TCHAR szFilter[] = _T("*.txt\0*.txt\0All (*.*)\0*.*\0\0");
	int ret;

	//�J�����g�t�H���_���擾
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

	if (GetSaveFileName(&ofn) == 0) {//���s��
		if (CommDlgExtendedError() != 0) {//0�̎��̓L�����Z��
			ret = -1;
		}
		else {
			ret = 0;
		}
	}
	else {//������
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
				WriteFile(hFile, sztText, (DWORD)(_tcslen(sztText) * sizeof(_TCHAR)), &dwWriteNum, NULL);
				CloseHandle(hFile);
				ret = 1;
			}
			free(sztText);
		}
	}

	//�J�����g�t�H���_��ݒ�
	SetCurrentDirectory(szCurrentDir);

	return ret;
}