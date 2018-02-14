#include <Windows.h>
#include <locale.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <process.h>
#include "pol.h"
#include "resource.h"
#include "CompareFile.h"

#pragma comment(lib,"Comctl32")
#pragma comment(lib,"Shlwapi")

#define WM_PROGRESSBAR	(WM_USER + 1)
#define WP_PROGRESS_START	1
#define WP_PROGRESS_STOP	2
#define WP_PROGRESS_END		3
#define WP_PROGRESS_PAINT	4
#define LP_PROGRESS_OPEN	0
#define LP_PROGRESS_COMPARE	1
#define ID_STATUSBAR	(100)

unsigned __stdcall CompareThread(void *lpParam);
int CompareOpenFile(HWND hEdit, HWND hList);
int CompareStart(HWND hList1, HWND hList2, HWND hList3);
int ReadCsvFile(_TCHAR *sztFileName, HWND hList);
int WriteFileFromList(HWND hWnd);

volatile int g_nThreadStop;

//ファイル比較ダイアログプロシージャ
BOOL CALLBACK CompareFileProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HANDLE hCompareThread;
	static HWND hOwnWnd;
	DWORD dwExitCode;

	switch (msg) {
	case WM_INITDIALOG:
		_tsetlocale(LC_ALL, _T(""));
		InitCommonControls();
		DWORD dwStyle;
		dwStyle = ListView_GetExtendedListViewStyle(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE1));
		dwStyle |= LVS_EX_FULLROWSELECT;
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE1), dwStyle);
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE2), dwStyle);
		ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3), dwStyle);
		SendMessage(GetDlgItem(hWnd, IDC_PROGRESS_COMPARE), PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 100));
		SendMessage(GetDlgItem(hWnd, IDC_PROGRESS_COMPARE), PBM_SETSTEP, (WPARAM)0, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_COMPARE_OPEN_FILE1:
			if (CompareOpenFile(GetDlgItem(hWnd, IDC_EDIT_COMPARE_FILE1), GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE1)) == 1) {
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_SAVE), FALSE);
			}
			return TRUE;
		case IDC_BUTTON_COMPARE_OPEN_FILE2:
			if (CompareOpenFile(GetDlgItem(hWnd, IDC_EDIT_COMPARE_FILE2), GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE2)) == 1) {
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_SAVE), FALSE);
			}
			return TRUE;
		case IDC_BUTTON_COMPARE_START:
			hOwnWnd = hWnd;
			SetFocus(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3));
			g_nThreadStop = 0;
			hCompareThread = (HANDLE)_beginthreadex(NULL, 0, CompareThread, hOwnWnd, 0, NULL);
			return TRUE;
		case IDC_BUTTON_COMPARE_STOP:
			g_nThreadStop = 1;
			return TRUE;
		case IDC_BUTTON_COMPARE_SAVE:
			WriteFileFromList(hWnd);
			return TRUE;
		}
		return TRUE;
	case WM_PROGRESSBAR:
		if (wp == WP_PROGRESS_START) {
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE1), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE2), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_SAVE), FALSE);
			if (lp == LP_PROGRESS_COMPARE) {
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_STOP), SW_SHOW);
			}
		}
		else if (wp == WP_PROGRESS_STOP) {
			SetDlgItemText(hWnd, IDC_STATIC_COMPARE_STATUS, _T("中止しました"));
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE1), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE2), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_SAVE), FALSE);
			if (lp == LP_PROGRESS_COMPARE) {
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_STOP), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), SW_SHOW);
			}
			while (1) {
				Sleep(1);
				GetExitCodeThread(hCompareThread, &dwExitCode);
				if (dwExitCode == STILL_ACTIVE) {
					SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_COMPARE_STOP, 0);
				}
				else {
					CloseHandle(hCompareThread);
					break;
				}
			}
		}
		else if (wp == WP_PROGRESS_END) {
			SetDlgItemText(hWnd, IDC_STATIC_COMPARE_STATUS, _T("抽出完了！"));
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE1), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_OPEN_FILE2), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), TRUE);
			if (lp == LP_PROGRESS_COMPARE) {
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_STOP), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_START), SW_SHOW);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_COMPARE_SAVE), TRUE);
			}
			while (1) {
				Sleep(1);
				GetExitCodeThread(hCompareThread, &dwExitCode);
				if (dwExitCode == STILL_ACTIVE) {
					SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_COMPARE_STOP, 0);
				}
				else {
					CloseHandle(hCompareThread);
					break;
				}
			}
			SendMessage(GetDlgItem(hWnd, IDC_PROGRESS_COMPARE), PBM_SETPOS, 0, 0);
		}
		else if (wp == WP_PROGRESS_PAINT) {
			SendMessage(GetDlgItem(hWnd, IDC_PROGRESS_COMPARE), PBM_SETPOS, (WPARAM)lp, 0);
		}
		return TRUE;
	case WM_CLOSE:
		GetExitCodeThread(hCompareThread, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE) {
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_COMPARE_STOP, 0);
		}
		else {
			EndDialog(hWnd, IDOK);
		}
		return TRUE;
	}
	return FALSE;
}


int CompareOpenFile(HWND hEdit, HWND hList)
{
	_TCHAR sztCurrentDir[MAX_PATH];
	_TCHAR sztFileName[MAX_PATH];
	const _TCHAR sztFilter[] = _T("*.txt\0*.txt\0*.csv\0*.csv\0All (*.*)\0*.*\0\0");
	int ret;

	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(sztCurrentDir) / sizeof(_TCHAR), sztCurrentDir);
	ZeroMemory(sztFileName, sizeof(sztFileName));

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hList;
	ofn.lpstrInitialDir = sztCurrentDir;
	ofn.lpstrFilter = sztFilter;
	ofn.lpstrFile = sztFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("txt");

	if (GetOpenFileName(&ofn) == 0) {//失敗時
		if (CommDlgExtendedError() != 0) {//0の時はキャンセル
			ret = -1;
		}
		else {
			ret = 0;
		}
	}
	else {//成功時
		  //ファイル名を設定
		SetWindowText(hEdit, sztFileName);
	}

	//カレントフォルダを設定
	SetCurrentDirectory(sztCurrentDir);

	return 1;
}

int CompareStart(HWND hList1, HWND hList2, HWND hList3)
{
	return 1;
}

unsigned __stdcall CompareThread(void *lpParam)
{
	HWND hWnd = (HWND)lpParam;
	HWND hList1 = GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE1);
	HWND hList2 = GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE2);
	HWND hList3 = GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3);
	HWND hEdit1 = GetDlgItem(hWnd, IDC_EDIT_COMPARE_FILE1);
	HWND hEdit2 = GetDlgItem(hWnd, IDC_EDIT_COMPARE_FILE2);

	_TCHAR sztFileName[MAX_PATH];
	GetWindowText(hEdit1, sztFileName, ARRAYSIZE(sztFileName));
	SetDlgItemText(hWnd, IDC_STATIC_COMPARE_STATUS, _T("比較ファイル(新情報)を読込み中..."));
	ReadCsvFile(sztFileName, hList1);
	GetWindowText(hEdit2, sztFileName, ARRAYSIZE(sztFileName));
	SetDlgItemText(hWnd, IDC_STATIC_COMPARE_STATUS, _T("比較ファイル(古情報)を読込み中..."));
	ReadCsvFile(sztFileName, hList2);

	int nRows1, nRows2, nRows3;//各リストの行数
	nRows1 = ListView_GetItemCount(hList1);
	nRows2 = ListView_GetItemCount(hList2);
	nRows3 = 0;
	if (nRows1 == 0 || nRows2 == 0) {
		MessageBox(hWnd, _T("比較できません"), NULL, MB_ICONERROR);
		PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_STOP, (LPARAM)LP_PROGRESS_COMPARE);//プログレスバー
		_endthreadex(1);
		return 1;
	}

	int nCols1, nCols2;//各リストの要素数
	nCols1 = Header_GetItemCount(ListView_GetHeader(hList1));
	nCols2 = Header_GetItemCount(ListView_GetHeader(hList2));
	if (nCols1 != nCols2 || nCols1 == 0) {
		MessageBox(hWnd, _T("要素数が異なります"), NULL, MB_ICONERROR);
		PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_STOP, (LPARAM)LP_PROGRESS_COMPARE);//プログレスバー
		_endthreadex(2);
		return 2;
	}

	//ヘッダー作成
	ListView_DeleteAllItems(hList3);
	while (Header_GetItemCount(ListView_GetHeader(hList3))) {
		ListView_DeleteColumn(hList3, 0);
	}
	_TCHAR sztText[0x200];
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = 40;
	for (int i = 0; i<nCols1; i++) {
		col.iSubItem = i;
		wsprintf(sztText, _T("%d"), i);
		col.pszText = sztText;
		ListView_InsertColumn(hList3, i, &col);
	}

	SetDlgItemText(hWnd, IDC_STATIC_COMPARE_STATUS, _T("変更点を抽出中..."));
	//比較開始
	LVITEM item1, item2, item3;
	_TCHAR sztText1[0x100], sztText2[0x100];
	item1.mask = LVIF_TEXT;
	item2.mask = LVIF_TEXT;
	item3.mask = LVIF_TEXT;
	PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_START, (LPARAM)LP_PROGRESS_COMPARE);//プログレスバー
	for (int i = 0; i<nRows1 - 1; i++) {
		if (g_nThreadStop == 1) {
			PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_STOP, (LPARAM)LP_PROGRESS_COMPARE);//プログレスバー
			_endthreadex(100);
			return 100;
		}
		//進行度
		PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_PAINT, (LPARAM)(int)(i * 100 / nRows1));//プログレスバー
																									  //hList1のアイテム情報を取得
		ZeroMemory(sztText1, sizeof(sztText1));
		item1.iItem = i;
		item1.pszText = sztText1;
		item1.cchTextMax = ARRAYSIZE(sztText1);
		item1.iSubItem = 0;
		ListView_GetItem(hList1, &item1);

		int nFlag = 0;
		for (int j = 0; j<nRows2 - 1; j++) {
			//hList2のアイテム情報を取得
			ZeroMemory(sztText2, sizeof(sztText2));
			item2.iItem = j;
			item2.pszText = sztText2;
			item2.cchTextMax = ARRAYSIZE(sztText2);
			item2.iSubItem = 0;
			ListView_GetItem(hList2, &item2);
			if (_tcscmp(sztText1, sztText2) != 0) {//一致しない場合
				continue;
			}
			else {
				nFlag = 1;
			}

			for (int k = 1; k<nCols1; k++) {
				//hList1のアイテム情報を取得
				item1.iItem = i;
				item1.pszText = sztText1;
				item1.cchTextMax = ARRAYSIZE(sztText1);
				item1.iSubItem = k;
				ListView_GetItem(hList1, &item1);

				//hList2のアイテム情報を取得
				item2.iItem = j;
				item2.pszText = sztText2;
				item2.cchTextMax = ARRAYSIZE(sztText2);
				item2.iSubItem = k;
				ListView_GetItem(hList2, &item2);
				if (_tcscmp(sztText1, sztText2) == 0) {//一致
					nFlag = 1;
				}
				else {
					nFlag = 0;
					break;
				}
			}
			if (nFlag == 1) {//完全一致
				ListView_DeleteItem(hList2, j);
				nRows2--;
				break;
			}
		}
		if (nFlag == 0) {
			for (int m = 0; m<nCols1; m++) {
				//hList1のアイテム情報を取得
				item1.iItem = i;
				item1.pszText = sztText1;
				item1.cchTextMax = ARRAYSIZE(sztText1);
				item1.iSubItem = m;
				ListView_GetItem(hList1, &item1);

				//hList3にアイテムを追加
				item3.iItem = nRows3;
				item3.iSubItem = m;
				item3.pszText = sztText1;
				m == 0 ? ListView_InsertItem(hList3, &item3) : ListView_SetItem(hList3, &item3);
			}
			nRows3++;
		}
	}
	PostMessage(hWnd, WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_END, (LPARAM)LP_PROGRESS_COMPARE);//プログレスバー

	_endthreadex(0);
	return 0;
}


int ReadCsvFile(_TCHAR *sztFileName, HWND hList)
{
	int ret;

	HANDLE hFile = CreateFile(sztFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		ret = -1;
	}
	else {
		DWORD dwFilePointer = 0;
		_TCHAR sztBuf[0x1000], *psztBuf;
		DWORD dwReadNum;
		int nCols = 0;
		//要素数を取得
		ReadFile(hFile, sztBuf, 0x1000, &dwReadNum, NULL);
		sztBuf[_tcsstr(sztBuf, _T("\r\n")) - sztBuf] = _T('\0');
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		psztBuf = sztBuf;
		while (psztBuf != NULL) {
			psztBuf = _tcsstr(psztBuf, _T(","));
			if (psztBuf != NULL)psztBuf += 1;//,分移動
			nCols++;
		}

		//ヘッダー作成
		ListView_DeleteAllItems(hList);
		while (Header_GetItemCount(ListView_GetHeader(hList))) {
			ListView_DeleteColumn(hList, 0);
		}
		_TCHAR sztText[0x200];
		LVCOLUMN col;
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		for (int i = 0; i<nCols; i++) {
			col.iSubItem = i;
			wsprintf(sztText, _T("%d"), i);
			col.pszText = sztText;
			ListView_InsertColumn(hList, i, &col);
		}

		//読込み
		LVITEM item;
		item.mask = LVIF_TEXT;
		int nRows = 0;
		DWORD dwFileSize = SetFilePointer(hFile, 0, NULL, FILE_END);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		PostMessage(GetParent(hList), WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_START, (LPARAM)LP_PROGRESS_OPEN);//プログレスバー
		do {
			ReadFile(hFile, sztBuf, 0x1000, &dwReadNum, NULL);
			if (_tcsstr(sztBuf, _T("\r\n")) != 0) {
				dwFilePointer += _tcsstr(sztBuf, _T("\r\n")) - sztBuf;
				if (dwFilePointer != 0)dwFilePointer += 2;//\r\n分移動
				sztBuf[_tcsstr(sztBuf, _T("\r\n")) - sztBuf] = _T('\0');
				SetFilePointer(hFile, dwFilePointer, NULL, FILE_BEGIN);
				PostMessage(GetParent(hList), WM_PROGRESSBAR, (WPARAM)WP_PROGRESS_PAINT, (LPARAM)(int)(dwFilePointer * 100 / dwFileSize));//プログレスバー
			}

			item.iItem = nRows;
			psztBuf = sztBuf;
			for (int i = 0; i<nCols; i++) {
				_tcscpy_s(sztText, psztBuf);
				if (_tcsstr(sztText, _T(",")) != 0)sztText[_tcsstr(sztText, _T(",")) - sztText] = _T('\0');
				item.iSubItem = i;
				item.pszText = sztText;
				i == 0 ? ListView_InsertItem(hList, &item) : ListView_SetItem(hList, &item);
				psztBuf = _tcsstr(psztBuf, _T(","));
				if (psztBuf != NULL)psztBuf += 1;//,分移動
			}
			nRows++;
		} while (dwReadNum);

		ret = 1;
	}

	return ret;
}


int WriteFileFromList(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_LIST_COMPARE_FILE3);
	_TCHAR sztCurrentDir[MAX_PATH];
	_TCHAR sztFileName[MAX_PATH];
	const _TCHAR sztFilter[] = _T("Text (*.txt)\0*.txt\0All (*.*)\0*.*\0\0");
	//const _TCHAR sztFilter[] = _T("Text (*.txt)\0*.txt\0HTML (*.html)\0*.html\0All (*.*)\0*.*\0\0");
	int ret;

	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(sztCurrentDir) / sizeof(_TCHAR), sztCurrentDir);
	_TCHAR sztFileName1[MAX_PATH], sztFileName2[MAX_PATH];
	GetDlgItemText(hWnd, IDC_EDIT_COMPARE_FILE1, sztFileName, ARRAYSIZE(sztFileName));
	GetFileTitle(sztFileName, sztFileName1, ARRAYSIZE(sztFileName1));
	if (_tcsstr(sztFileName1, PathFindExtension(sztFileName1)) != 0)sztFileName1[_tcsstr(sztFileName1, PathFindExtension(sztFileName1)) - sztFileName1] = _T('\0');
	GetDlgItemText(hWnd, IDC_EDIT_COMPARE_FILE2, sztFileName, ARRAYSIZE(sztFileName));
	GetFileTitle(sztFileName, sztFileName2, ARRAYSIZE(sztFileName2));
	if (_tcsstr(sztFileName2, PathFindExtension(sztFileName2)) != 0)sztFileName2[_tcsstr(sztFileName2, PathFindExtension(sztFileName2)) - sztFileName2] = _T('\0');
	wsprintf(sztFileName, _T("diff_%s-%s.txt"), sztFileName1, sztFileName2);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hList;
	ofn.lpstrInitialDir = sztCurrentDir;
	ofn.lpstrFilter = sztFilter;
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
			HANDLE hFile = CreateFile(sztFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				ret = -3;
			}
			else {
				int nItemCount = ListView_GetItemCount(hList);
				int nColumnCount = Header_GetItemCount(ListView_GetHeader(hList));
				LVITEM lvi;
				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_TEXT;
				DWORD dwWriteNum;

				for (int nRow = 0; nRow<nItemCount; nRow++) {
					ZeroMemory(sztText, sizeof(sztText));
					for (int nCol = 0; nCol<nColumnCount; nCol++) {
						lvi.iItem = nRow;
						lvi.iSubItem = nCol;
						lvi.cchTextMax = 0x1000 - _tcslen(sztText);
						lvi.pszText = sztText + _tcslen(sztText) * sizeof(_TCHAR);
						ListView_GetItem(hList, &lvi);
						if (nCol != nColumnCount - 1) {
							StrNCat(sztText, _T(","), 2);
						}
						else {
							StrNCat(sztText, _T("\r\n"), 3);
						}
					}
#ifdef UNICODE
					BYTE bom[2] = { 0xFF,0xFE };//BOM.
					WriteFile(hFile, bom, (DWORD)(sizeof(bom)), &dwWriteNum, NULL);
#endif
					WriteFile(hFile, sztText, _tcslen(sztText) * sizeof(_TCHAR), &dwWriteNum, NULL);
				}
				CloseHandle(hFile);
				ret = 1;
			}
			free(sztText);
		}
	}

	//カレントフォルダを設定
	SetCurrentDirectory(sztCurrentDir);

	return ret;
}