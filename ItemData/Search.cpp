//1.4で追加

#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <CommCtrl.h>
#include "resource.h"
#include "Search.h"

//検索ダイアログプロシージャ
LRESULT CALLBACK SearchProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		_tsetlocale(LC_ALL, _T(""));
		SendMessage(GetDlgItem(hWnd, IDC_RADIO_SEARCH_DOWN), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(hWnd, WM_COMMAND, IDC_COMBO_SEARCH | (CBN_DROPDOWN << 16), 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON_SEARCH:
			_TCHAR sztSearchText[0x100];
			GetDlgItemText(hWnd, IDC_EDIT_SEARCH, sztSearchText, sizeof(sztSearchText) - 1);
			_TCHAR sztSearchColumn[0x100];
			GetDlgItemText(hWnd, IDC_COMBO_SEARCH, sztSearchColumn, sizeof(sztSearchColumn) - 1);
			HWND hList;
			hList = GetDlgItem(GetParent(hWnd), IDC_LIST);
			int nColCount;
			nColCount = Header_GetItemCount(ListView_GetHeader(hList));
			int nItemCount;
			nItemCount = ListView_GetItemCount(hList);
			int nNowRow;
			nNowRow = ListView_GetNextItem(hList, -1, LVNI_ALL | LVNI_SELECTED);
			if (nNowRow) {
				if (SendMessage(GetDlgItem(hWnd, IDC_RADIO_SEARCH_DOWN), BM_GETCHECK, 0, 0) == BST_CHECKED) {
					nNowRow++;//選択された行の次からサーチ
				}
				else {
					nNowRow--;//選択された行の次からサーチ
				}
			}

			if (nColCount == 0 || nItemCount == 0) {
				MessageBox(hWnd, _T("検索するデータがありません"), NULL, MB_ICONERROR);
			}
			else if (_tcslen(sztSearchColumn) == 0) {
				MessageBox(hWnd, _T("検索するカラムが設定されていません"), NULL, MB_ICONERROR);
			}
			else if (_tcslen(sztSearchText)) {
				if (nItemCount == 0)break;
				for (int nCol = 0, nRow = nNowRow; nCol<nColCount; nCol++) {
					HDITEM hdi;
					_TCHAR sztColumnName[0x100];
					hdi.mask = HDI_TEXT;
					hdi.cchTextMax = sizeof(sztColumnName) - 1;
					hdi.pszText = sztColumnName;
					Header_GetItem(ListView_GetHeader(hList), nCol, &hdi);
					if (_tcscmp(sztColumnName, sztSearchColumn) == 0) {//一致
						if (SendMessage(GetDlgItem(hWnd, IDC_RADIO_SEARCH_DOWN), BM_GETCHECK, 0, 0) == BST_CHECKED) {//下へ
							for (; nRow < nItemCount; nRow++) {
								_TCHAR sztText[0x100];
								ListView_GetItemText(hList, nRow, nCol, sztText, sizeof(sztText) - 1);
								if (_tcsstr(sztText, sztSearchText) != NULL) {//部分一致
									SetFocus(GetDlgItem(GetParent(hWnd), IDC_LIST));
									ListView_SetItemState(hList, nRow, LVIS_FOCUSED, LVIS_FOCUSED);
									ListView_SetItemState(hList, nRow, LVIS_SELECTED, LVIS_SELECTED);
									ListView_EnsureVisible(hList, nRow, TRUE);
									break;
								}
							}
							if (nRow == nItemCount)MessageBeep(1);
							break;
						}
						else {//上へ
							for (; nRow >= 0; nRow--) {
								_TCHAR sztText[0x100];
								ListView_GetItemText(hList, nRow, nCol, sztText, sizeof(sztText) - 1);
								if (_tcsstr(sztText, sztSearchText) != NULL) {//部分一致
									SetFocus(GetDlgItem(GetParent(hWnd), IDC_LIST));
									ListView_SetItemState(hList, nRow, LVIS_FOCUSED, LVIS_FOCUSED);
									ListView_SetItemState(hList, nRow, LVIS_SELECTED, LVIS_SELECTED);
									ListView_EnsureVisible(hList, nRow, TRUE);
									break;
								}
							}
							if (nRow == -1)MessageBeep(1);
							break;
						}
					}
				}
			}
			else {
				MessageBeep(1);
			}
			break;
		case IDC_BUTTON_SEARCH_CLOSE:
			DestroyWindow(hWnd);
			break;
		case IDC_COMBO_SEARCH:
			if (HIWORD(wp) == CBN_DROPDOWN) {
				HWND hList = GetDlgItem(GetParent(hWnd), IDC_LIST);
				HWND hCombo = GetDlgItem(hWnd, IDC_COMBO_SEARCH);
				SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
				int nColCount = Header_GetItemCount(ListView_GetHeader(hList));
				HDITEM hdi;
				_TCHAR sztColumnName[0x100];
				hdi.mask = HDI_TEXT;
				hdi.cchTextMax = sizeof(sztColumnName) - 1;
				hdi.pszText = sztColumnName;
				for (int nCol = 0; nCol<nColCount; nCol++) {
					Header_GetItem(ListView_GetHeader(hList), nCol, &hdi);
					SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)sztColumnName);
				}
				SendMessage(hCombo, CB_SELECTSTRING, 0, (LPARAM)_T("名前"));
			}
			break;
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		DestroyWindow(hWnd);
		break;
	}
	return 0;
}
