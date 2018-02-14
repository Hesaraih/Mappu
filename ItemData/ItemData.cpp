#include <Windows.h>
#include <locale.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "ItemData.h"
#include "pol.h"
#include "resource.h"
#include "CompareFile.h"
#include "Search.h"//1.4で追加

#ifdef DEBUG
#include <crtdbg.h>
#endif

#pragma comment(lib,"Comctl32")
#pragma comment(lib,"version.lib")


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProc);
#ifdef DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return 0;
}

//メインダイアログプロシージャ
BOOL CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static CLASS_PATH path;
	static CLASS_TREE tree;
	static CLASS_LIST list;
	static HMENU hMenu, hSubMenu;
	static _TCHAR sztSaveFile[MAX_PATH];
	static int nShowList = 0;
	static HWND sthSearchWnd = 0;//1.4で追加

	switch (msg) {
	case WM_INITDIALOG:
		_tsetlocale(LC_ALL, _T(""));
		InitCommonControls();
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED));
		path.GetPath();
		tree.Init(GetDlgItem(hWnd, IDC_TREE), &path);
		list.Init(GetDlgItem(hWnd, IDC_LIST));
		hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU));
		hSubMenu = GetSubMenu(hMenu, 0);
		SetMenu(hWnd, hMenu);
		SendMessage(hWnd, WM_SIZE, NULL, NULL);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDM_SAVE:
			int nRet;
			nRet = 0;
			EnableWindow(hWnd, FALSE);
			if (_tcslen(sztSaveFile) > 0) {
				if (nShowList == LPARAM_WEAPON || nShowList == LPARAM_ARMOR) {
					nRet = list.SaveFileWeaponArmor(sztSaveFile);
				}
				else if (nShowList == LPARAM_MAGIC) {
					nRet = list.SaveFile(sztSaveFile);
				}
				else if (nShowList == LPARAM_ITEM) {
					nRet = list.SaveFile(sztSaveFile);
				}
			}
			if (nRet == 1) {
				MessageBox(hWnd, _T("保存しました"), sztSaveFile, MB_OK);
			}
			else if (nRet < 0) {
				MessageBox(hWnd, _T("保存出来ませんでした"), NULL, MB_ICONERROR);
			}
			EnableWindow(hWnd, TRUE);
			return TRUE;
		case IDM_VERSION:
			_TCHAR sztFileName[MAX_PATH];
			GetModuleFileName(NULL, sztFileName, sizeof(sztFileName) / sizeof(_TCHAR));
			DWORD dwBlockSize;
			dwBlockSize = GetFileVersionInfoSize(sztFileName, 0);
			BYTE *pbyVersionInfo;
			pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
			if (pbyVersionInfo) {
				GetFileVersionInfo(sztFileName, 0, dwBlockSize, (LPVOID)pbyVersionInfo);
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
				wsprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				void *pvComments;//1.3で追加
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvComments, NULL);
				_TCHAR sztVersion[0x100];
				wsprintf(sztVersion, _T("Version %s\n\n%s\n%s"), pvVer, pvComments, pvCopyright);
				MessageBox(hWnd, sztVersion, _T("ItemData"), MB_OK | MB_ICONINFORMATION);
			}
			free(pbyVersionInfo);
			return TRUE;
		case IDM_COMPARE://1.2で追加
			if (IsWindow(sthSearchWnd) == TRUE) {//1.4aで追加
				SendMessage(sthSearchWnd, WM_CLOSE, NULL, NULL);
			}
			DialogBox((HINSTANCE)GetWindowLongA(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_COMPARE), hWnd, CompareFileProc);
			return TRUE;
		case IDM_SEARCH://1.4で追加
			if (IsWindow(sthSearchWnd) == FALSE) {
				sthSearchWnd = CreateDialog((HINSTANCE)GetWindowLongA(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SEARCH), hWnd, (DLGPROC)SearchProc);
			}
			return TRUE;
		case IDM_MAPPU://1.4で追加
			GetModuleFileName(NULL, sztFileName, sizeof(sztFileName) / sizeof(_TCHAR));
			dwBlockSize = GetFileVersionInfoSize(sztFileName, 0);
			pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
			if (pbyVersionInfo) {
				GetFileVersionInfo(sztFileName, 0, dwBlockSize, (LPVOID)pbyVersionInfo);
				struct {
					WORD wLanguage;
					WORD wCodePage;
				} *lpTranslate;
				VerQueryValue((LPVOID)pbyVersionInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, NULL);
				_TCHAR szDir[0x100];
				wsprintf(szDir, _T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate->wLanguage, lpTranslate->wCodePage);
				void *pvComments;//1.3で追加
				VerQueryValue((LPVOID)pbyVersionInfo, szDir, (LPVOID*)&pvComments, NULL);
				ShellExecute(hWnd, _T("open"), (_TCHAR*)pvComments, NULL, NULL, SW_SHOWNORMAL);
			}
			free(pbyVersionInfo);
			return TRUE;
		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		return TRUE;

	case WM_NOTIFY:
		if (((LPNMHDR)lp)->idFrom == IDC_TREE) {
			const _TCHAR tLang[] = { _T('J'),_T('E'),_T('F'),_T('G') };
			LPNMTREEVIEW lpnmtree;
			lpnmtree = (LPNMTREEVIEW)lp;
			switch (lpnmtree->hdr.code) {
			case TVN_SELCHANGED:
				TVITEM tvi;
				ZeroMemory(&tvi, sizeof(tvi));
				tvi.mask = TVIF_HANDLE | TVIF_PARAM;
				tvi.hItem = lpnmtree->itemNew.hItem;
				TreeView_GetItem(lpnmtree->hdr.hwndFrom, &tvi);
				if (tvi.lParam) {//lParam = LPARAM_WEAPON/LPARAM_ARMOR | (nKey * 0x10) | (nLang * 0x100000) | (nClient * 0x10000000);
					int nType = tvi.lParam & 0x000000FF;//0x01=Weapon,0x02=Armor,0x10=Magic
					int nKey = (tvi.lParam & 0x00FFF00) / 0x100;//0～MAX_KEY_NUM
					int nLang = (tvi.lParam & 0x0F000000) / 0x1000000;//0=ja,1=en,2=fr,3=de
					int nClient = (tvi.lParam & 0xF0000000) / 0x10000000;//0=Main,1=Test
					if (nType == LPARAM_WEAPON) {
						_TCHAR *lpsztDatPath = NULL;
						if (nLang == 0)lpsztDatPath = path.Client[nClient].sztWeapon[nKey].ja;
						if (nLang == 1)lpsztDatPath = path.Client[nClient].sztWeapon[nKey].en;
						if (nLang == 2)lpsztDatPath = path.Client[nClient].sztWeapon[nKey].fr;
						if (nLang == 3)lpsztDatPath = path.Client[nClient].sztWeapon[nKey].de;

						SYSTEMTIME sysTime;//1.2で追加
						GetTimeStamp_Update(lpsztDatPath, &sysTime);//1.2で追加

						if (nClient == 0) {
							wsprintf(sztSaveFile, _T(".\\Weapon%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);//1.2で変更
						}
						else {
							wsprintf(sztSaveFile, _T(".\\TC_Weapon%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);//1.2で変更
						}
						nShowList = LPARAM_WEAPON;
						CLASS_ITEM item(lpsztDatPath, nType);
						if (item.IsItemDat() != 1)break;
						else {
							list.Clear();
							list.Add(&path, &item);
						}
					}
					else if (nType == LPARAM_ARMOR) {
						_TCHAR *lpsztDatPath = NULL;
						if (nLang == 0)lpsztDatPath = path.Client[nClient].sztArmor[nKey].ja;
						if (nLang == 1)lpsztDatPath = path.Client[nClient].sztArmor[nKey].en;
						if (nLang == 2)lpsztDatPath = path.Client[nClient].sztArmor[nKey].fr;
						if (nLang == 3)lpsztDatPath = path.Client[nClient].sztArmor[nKey].de;

						SYSTEMTIME sysTime;//1.2で追加
						GetTimeStamp_Update(lpsztDatPath, &sysTime);//1.2で追加

						if (nClient == 0) {
							wsprintf(sztSaveFile, _T(".\\Armor%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);//1.2で変更
						}
						else {
							wsprintf(sztSaveFile, _T(".\\TC_Armor%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);//1.2で変更
						}
						nShowList = LPARAM_ARMOR;
						CLASS_ITEM item(lpsztDatPath, nType);
						if (item.IsItemDat() != 1)break;
						else {
							list.Clear();
							list.Add(&path, &item);
						}
					}
					else if (nType == LPARAM_MAGIC) {
						_TCHAR *lpsztDatPath[3] = { NULL,NULL,NULL };
						if (nLang == 0) {
							lpsztDatPath[0] = path.Client[nClient].sztMagic[0].ja;
							lpsztDatPath[1] = path.Client[nClient].sztMagic[1].ja;
							lpsztDatPath[2] = path.Client[nClient].sztMagic[2].ja;
						}
						if (nLang == 1) {
							lpsztDatPath[0] = path.Client[nClient].sztMagic[0].en;
							lpsztDatPath[1] = path.Client[nClient].sztMagic[1].en;
							lpsztDatPath[2] = path.Client[nClient].sztMagic[2].en;
						}
						if (nLang == 2) {
							lpsztDatPath[0] = path.Client[nClient].sztMagic[0].fr;
							lpsztDatPath[1] = path.Client[nClient].sztMagic[1].fr;
							lpsztDatPath[2] = path.Client[nClient].sztMagic[2].fr;
						}
						if (nLang == 3) {
							lpsztDatPath[0] = path.Client[nClient].sztMagic[0].de;
							lpsztDatPath[1] = path.Client[nClient].sztMagic[1].de;
							lpsztDatPath[2] = path.Client[nClient].sztMagic[2].de;
						}

						SYSTEMTIME sysLastTime, sysTime[3];//1.2で追加
						ZeroMemory(&sysLastTime, sizeof(sysLastTime));
						for (int i = 0; i < 3; i++) {
							GetTimeStamp_Update(lpsztDatPath[i], &sysTime[i]);//1.2で追加
							if (sysLastTime.wYear * 13 * 32 + sysLastTime.wMonth * 32 + sysLastTime.wDay < sysTime[i].wYear * 13 * 32 + sysTime[i].wMonth * 32 + sysTime[i].wDay) {
								sysLastTime = sysTime[i];
							}
						}

						if (nClient == 0) {
#ifdef DEBUG
							wsprintf(sztSaveFile, _T(".\\DEBUG_魔法(%c)_%d%02d%02d.txt"), tLang[nLang], sysLastTime.wYear, sysLastTime.wMonth, sysLastTime.wDay);//1.2で変更
#else
							wsprintf(sztSaveFile, _T(".\\Magic(%c)_%d%02d%02d.txt"), tLang[nLang], sysLastTime.wYear, sysLastTime.wMonth, sysLastTime.wDay);//1.2で変更
#endif
						}
						else {
#ifdef DEBUG
							wsprintf(sztSaveFile, _T(".\\DEBUG_TC_魔法(%c)_%d%02d%02d.txt"), tLang[nLang], sysLastTime.wYear, sysLastTime.wMonth, sysLastTime.wDay);//1.2で変更
#else
							wsprintf(sztSaveFile, _T(".\\TC_Magic(%c)_%d%02d%02d.txt"), tLang[nLang], sysLastTime.wYear, sysLastTime.wMonth, sysLastTime.wDay);//1.2で変更
#endif
						}
						nShowList = LPARAM_MAGIC;
						CLASS_MAGIC magic(lpsztDatPath[0], lpsztDatPath[1], lpsztDatPath[2]);
						list.Clear();
						list.Add(&path, &magic);
					}
					else if (nType == LPARAM_ITEM) {//1.3で追加
						_TCHAR *lpsztDatPath = NULL;
						if (nLang == 0)lpsztDatPath = path.Client[nClient].sztItem[nKey].ja;
						if (nLang == 1)lpsztDatPath = path.Client[nClient].sztItem[nKey].en;
						if (nLang == 2)lpsztDatPath = path.Client[nClient].sztItem[nKey].fr;
						if (nLang == 3)lpsztDatPath = path.Client[nClient].sztItem[nKey].de;

						SYSTEMTIME sysTime;
						GetTimeStamp_Update(lpsztDatPath, &sysTime);

						if (nClient == 0) {
							wsprintf(sztSaveFile, _T(".\\Item%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);
						}
						else {
							wsprintf(sztSaveFile, _T(".\\TC_Item%d(%c)_%d%02d%02d.txt"), nKey + 1, tLang[nLang], sysTime.wYear, sysTime.wMonth, sysTime.wDay);
						}
						nShowList = LPARAM_ITEM;
						CLASS_ITEM item(lpsztDatPath, nType);
						if (item.IsItemDat() != 1)break;
						else {
							list.Clear();
							list.AddItem(&path, &item);
						}
					}
				}
				break;

			case TVN_KEYDOWN://1.4aで追加
				if (((LPNMTVKEYDOWN)lp)->wVKey == 'E' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_COMPARE, NULL);
				}
				else if (((LPNMLVKEYDOWN)lp)->wVKey == 'F' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_SEARCH, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == 'S' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == 'V' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_VERSION, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == VK_F3) {
					SendMessage(sthSearchWnd, WM_COMMAND, IDC_BUTTON_SEARCH, NULL);
				}
				break;
			}
		}
		else if (((LPNMHDR)lp)->idFrom == IDC_LIST) {//1.4aで追加
			switch (((LPNMLISTVIEW)lp)->hdr.code) {
			case LVN_KEYDOWN:
				if (((LPNMTVKEYDOWN)lp)->wVKey == 'E' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_COMPARE, NULL);
				}
				else if (((LPNMLVKEYDOWN)lp)->wVKey == 'F' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_SEARCH, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == 'S' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == 'V' && GetKeyState(VK_CONTROL) < 0) {
					SendMessage(hWnd, WM_COMMAND, IDM_VERSION, NULL);
				}
				else if (((LPNMTVKEYDOWN)lp)->wVKey == VK_F3) {
					SendMessage(sthSearchWnd, WM_COMMAND, IDC_BUTTON_SEARCH, NULL);
				}
				break;
			}
		}
		return TRUE;

	case WM_SIZE:
		RECT rect, rectTree;
		GetClientRect(hWnd, &rect);
		GetWindowRect(GetDlgItem(hWnd, IDC_TREE), &rectTree);
		POINT point[3];
		point[0].x = rectTree.left;
		point[0].y = rectTree.top;
		ScreenToClient(hWnd, &point[0]);
		point[1].x = rectTree.right - rectTree.left;
		point[2].x = rect.right - point[1].x;
		MoveWindow(GetDlgItem(hWnd, IDC_TREE), point[0].x, point[0].y, point[1].x, rect.bottom, 1);
		MoveWindow(GetDlgItem(hWnd, IDC_LIST), point[1].x, point[0].y, point[2].x, rect.bottom, 1);
		return TRUE;

	case WM_CLOSE:
		DestroyMenu(hMenu);
		EndDialog(hWnd, IDOK);
		return TRUE;
	}
	return FALSE;
}


//1.2で追加
int GetTimeStamp_Update(_TCHAR *sztDllFullPath, SYSTEMTIME *SysTimeUpdate)
{
	ZeroMemory(SysTimeUpdate, sizeof(SYSTEMTIME));

	if (_tcslen(sztDllFullPath) == 0) {
		return -1;
	}

	HANDLE hFile = CreateFile(sztDllFullPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
