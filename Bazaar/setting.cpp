#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//ÉÜÉjÉRÅ[ÉhÇ≈ÉRÉìÉpÉCÉã
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define SETTING_CPP

#include <windows.h>
#include <tchar.h>
#include <CommCtrl.h>
#include "main.h"
#include "setting.h"
#include "resource.h"
#include "ini.h"
#include "searchaddr.h"//0.08Ç≈í«â¡


//É_ÉCÉAÉçÉOÉvÉçÉVÅ[ÉWÉÉIDD_SETTING ÉÇÅ[É_ÉãÉ_ÉCÉAÉçÉO
BOOL CALLBACK DlgProcSetting(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	static HMENU hMenu,hSubMenu;
	_TCHAR szNum[0x10];
	_TCHAR szBuf[MAX_PATH];
	DWORD dwBuf;

	switch(msg){
	case WM_INITDIALOG:
		if(g_UseFilterPrice){
			EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FILTER_PRICE),true);
			CheckDlgButton(hWnd,IDC_CHECKBOX_FILTER_PRICE,true);
		}
		else{
			EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FILTER_PRICE),false);
		}
		if(g_AutoScroll){
			CheckDlgButton(hWnd,IDC_CHECKBOX_AUTO_SCROLL,true);
		}
		if(g_SaveWindow){
			CheckDlgButton(hWnd,IDC_CHECKBOX_SAVE_WINSOW,true);
		}
		if(g_SaveWindow){
			CheckDlgButton(hWnd,IDC_CHECKBOX_SAVE_COLUMN,true);
		}
		if(g_ListColor){
			CheckDlgButton(hWnd,IDC_CHECKBOX_LIST_COLOR,true);
		}
		if(g_BlackList){
			CheckDlgButton(hWnd,IDC_CHECKBOX_BLIST,true);
		}
		if(g_UseFFXIAH){
			CheckDlgButton(hWnd,IDC_CHECKBOX_FFXIAH,true);
			EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FFXIAH),true);
		}
		else{
			EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FFXIAH),false);
		}
		if(g_TargetName){//0.08Ç≈í«â¡
			CheckDlgButton(hWnd,IDC_CHECKBOX_TARGET,true);
		}
		if(g_ExtractSave){
			CheckDlgButton(hWnd,IDC_CHECKBOX_EXTRACT_SAVE,true);
		}
		if(g_AutoOffset){//0.08Ç≈í«â¡
			CheckDlgButton(hWnd,IDC_CHECKBOX_OFFSET,true);
			SendMessage(hWnd,WM_COMMAND,IDC_CHECKBOX_OFFSET,NULL);
		}

		SetDlgItemInt(hWnd,IDC_EDIT_FILTER_PRICE,g_FilterPrice,true);
		SetDlgItemText(hWnd,IDC_EDIT_FFXIAH,g_FFXIAH);
		SetDlgItemText(hWnd,IDC_EDIT_DIR_SAVE,g_DirSave);
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(100,1000));
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY),TBM_SETPAGESIZE,0,50);
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY),TBM_SETPOS,(WPARAM)TRUE,(LPARAM)g_KeyDelay);
		SendMessage(hWnd,WM_HSCROLL,0,(LPARAM)GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY));
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(1000,5000));
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE),TBM_SETPAGESIZE,0,200);
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE),TBM_SETPOS,(WPARAM)TRUE,(LPARAM)g_WaitUpdate);
		SendMessage(hWnd,WM_HSCROLL,0,(LPARAM)GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE));
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_RETRY),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(3,10));
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_RETRY),TBM_SETPAGESIZE,0,1);
		SendMessage(GetDlgItem(hWnd,IDC_SLIDER_RETRY),TBM_SETPOS,(WPARAM)TRUE,(LPARAM)g_Retry);
		SendMessage(hWnd,WM_HSCROLL,0,(LPARAM)GetDlgItem(hWnd,IDC_SLIDER_RETRY));

		//0.09Ç≈í«â¡
		if(g_BazaarOrder == 0)SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_ID),BM_SETCHECK,BST_CHECKED,0);
		else if(g_BazaarOrder == 1)SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_N),BM_SETCHECK,BST_CHECKED,0);
		else if(g_BazaarOrder == 2)SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_S),BM_SETCHECK,BST_CHECKED,0);
		else if(g_BazaarOrder == 3)SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_E),BM_SETCHECK,BST_CHECKED,0);
		else if(g_BazaarOrder == 4)SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_W),BM_SETCHECK,BST_CHECKED,0);
		else{
			g_BazaarOrder = 0;
			SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_ID),BM_SETCHECK,BST_CHECKED,0);
		}

		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("0ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("1ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("2ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("4ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ3Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_SETCURSEL,g_BazaarNumWait05,0);
		
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("0ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("1ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("2ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("4ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ3Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_SETCURSEL,g_BazaarNumWait10,0);
		
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("0ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("1ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("2ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("4ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ3Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_SETCURSEL,g_BazaarNumWait20,0);
		
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("0ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("1ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("2ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("4ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`3ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ0Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_ADDSTRING,0,(LPARAM)_T("ÉâÉìÉ_ÉÄ3Å`5ïb"));
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_SETCURSEL,g_BazaarNumWait21,0);

		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDC_CHECKBOX_FILTER_PRICE:
			if(IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FILTER_PRICE)){
				EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FILTER_PRICE),true);
			}
			else{
				EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FILTER_PRICE),false);
			}
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_FILTER_PRICE));
			break;
			
		case IDC_CHECKBOX_FFXIAH:
			if(IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FFXIAH)){
				EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FFXIAH),true);
			}
			else{
				EnableWindow(GetDlgItem(hWnd,IDC_EDIT_FFXIAH),false);
			}
			SetFocus(GetDlgItem(hWnd,IDC_EDIT_FFXIAH));
			break;

		case IDC_BUTTON_SETTING_DONE:
			g_UseFilterPrice = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FILTER_PRICE);
			GetDlgItemText(hWnd,IDC_EDIT_FILTER_PRICE,szBuf,sizeof(szBuf)/sizeof(_TCHAR)-1);
			for(int i=0,nPos=0;szBuf[i];i++){
				if(szBuf[i] >= '0' && szBuf[i] <= '9'){
					szNum[nPos++] = szBuf[i];
				}
			}
			g_FilterPrice = wcstoul(szNum,NULL,10);
			g_ListColor = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_LIST_COLOR);
			g_AutoScroll = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_AUTO_SCROLL);
			g_UseFFXIAH = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FFXIAH);
			GetDlgItemText(hWnd,IDC_EDIT_FFXIAH,g_FFXIAH,sizeof(g_FFXIAH)/sizeof(_TCHAR)-1);
			g_TargetName = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_TARGET);//0.08Ç≈í«â¡
			g_ExtractSave = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_EXTRACT_SAVE);
			GetDlgItemText(hWnd,IDC_EDIT_DIR_SAVE,g_DirSave,sizeof(g_DirSave)/sizeof(_TCHAR)-1);
			g_SaveWindow = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_SAVE_WINSOW);
			g_SaveColumn = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_SAVE_COLUMN);
			g_BlackList = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_BLIST);
			g_KeyDelay = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY),TBM_GETPOS,NULL,NULL);
			g_WaitUpdate = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE),TBM_GETPOS,NULL,NULL);
			g_Retry = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_RETRY),TBM_GETPOS,NULL,NULL);
			g_AutoOffset = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_OFFSET);//0.08Ç≈í«â¡
			if(g_AutoOffset)GetOffsetAddressAll(g_polHandle,g_DllAddress);//0.08Ç≈í«â¡

			//0.09Ç≈í«â¡
			if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_ID),BM_GETCHECK,0,0))g_BazaarOrder = 0;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_N),BM_GETCHECK,0,0))g_BazaarOrder = 1;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_S),BM_GETCHECK,0,0))g_BazaarOrder = 2;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_E),BM_GETCHECK,0,0))g_BazaarOrder = 3;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_W),BM_GETCHECK,0,0))g_BazaarOrder = 4;
			g_BazaarNumWait05 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_GETCURSEL,0,0);
			g_BazaarNumWait10 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_GETCURSEL,0,0);
			g_BazaarNumWait20 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_GETCURSEL,0,0);
			g_BazaarNumWait21 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_GETCURSEL,0,0);
			MessageBox(hWnd,_T("çXêVÇµÇ‹ÇµÇΩ"),BAZAAR_STRING,MB_OK);
			break;

		case IDC_BUTTON_SETTING_SAVE:
			g_UseFilterPrice = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FILTER_PRICE);
			GetDlgItemText(hWnd,IDC_EDIT_FILTER_PRICE,szBuf,sizeof(szBuf)/sizeof(_TCHAR)-1);
			for(int i=0,nPos=0;szBuf[i];i++){
				if(szBuf[i] >= '0' && szBuf[i] <= '9'){
					szNum[nPos++] = szBuf[i];
				}
			}
			g_FilterPrice = wcstoul(szNum,NULL,10);
			g_ListColor = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_LIST_COLOR);
			g_AutoScroll = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_AUTO_SCROLL);
			g_UseFFXIAH = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_FFXIAH);
			GetDlgItemText(hWnd,IDC_EDIT_FFXIAH,g_FFXIAH,sizeof(g_FFXIAH)/sizeof(_TCHAR)-1);
			g_TargetName = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_TARGET);//0.08Ç≈í«â¡
			g_ExtractSave = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_EXTRACT_SAVE);
			GetDlgItemText(hWnd,IDC_EDIT_DIR_SAVE,g_DirSave,sizeof(g_DirSave)/sizeof(_TCHAR)-1);
			g_SaveWindow = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_SAVE_WINSOW);
			g_SaveColumn = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_SAVE_COLUMN);
			g_BlackList = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_BLIST);
			g_KeyDelay = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY),TBM_GETPOS,NULL,NULL);
			g_WaitUpdate = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE),TBM_GETPOS,NULL,NULL);
			g_Retry = (int)SendMessage(GetDlgItem(hWnd,IDC_SLIDER_RETRY),TBM_GETPOS,NULL,NULL);
			g_AutoOffset = IsDlgButtonChecked(hWnd,IDC_CHECKBOX_OFFSET);//0.08Ç≈í«â¡
			if(g_AutoOffset)GetOffsetAddressAll(g_polHandle,g_DllAddress);//0.08Ç≈í«â¡

			//0.09Ç≈í«â¡
			if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_ID),BM_GETCHECK,0,0))g_BazaarOrder = 0;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_N),BM_GETCHECK,0,0))g_BazaarOrder = 1;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_S),BM_GETCHECK,0,0))g_BazaarOrder = 2;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_E),BM_GETCHECK,0,0))g_BazaarOrder = 3;
			else if(BST_CHECKED == SendMessage(GetDlgItem(hWnd,IDC_RADIO_NUM_W),BM_GETCHECK,0,0))g_BazaarOrder = 4;
			g_BazaarNumWait05 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_05),CB_GETCURSEL,0,0);
			g_BazaarNumWait10 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_10),CB_GETCURSEL,0,0);
			g_BazaarNumWait20 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_20),CB_GETCURSEL,0,0);
			g_BazaarNumWait21 = (BYTE)SendMessage(GetDlgItem(hWnd,IDC_COMBO_WAIT_21),CB_GETCURSEL,0,0);
			if(WriteIni()){
				MessageBox(hWnd,_T("ï€ë∂ÇµÇ‹ÇµÇΩ"),BAZAAR_STRING,MB_OK);
				SendMessage(hWnd,WM_CLOSE,0,0);
			}
			else{
				MessageBox(hWnd,_T("ï€ë∂Ç…é∏îsÇµÇ‹ÇµÇΩ"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			}
			break;

		case IDC_CHECKBOX_OFFSET://0.08Ç≈í«â¡
			if(IsDlgButtonChecked(hWnd,IDC_CHECKBOX_OFFSET)){
				dwBuf = GetOffsetAddress(g_polHandle,g_DllAddress,(const BYTE*)constNpcmapPattern);
				_stprintf(szBuf,_T("%X"),dwBuf);
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_NPCMAP,szBuf);
				dwBuf = GetOffsetAddress(g_polHandle,g_DllAddress,(const BYTE*)constNowstaPattern);
				if(dwBuf)dwBuf -= NOWSTA_HOSEI;
				_stprintf(szBuf,_T("%X"),dwBuf);
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_NOWSTA,szBuf);
				dwBuf = GetOffsetAddress(g_polHandle,g_DllAddress,(const BYTE*)constBazaarPattern);
				_stprintf(szBuf,_T("%X"),dwBuf);
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_BAZAAR,szBuf);
			}
			else{
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_NPCMAP,_T(""));
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_NOWSTA,_T(""));
				SetDlgItemText(hWnd,IDC_EDIT_OFFSET_BAZAAR,_T(""));
			}
			break;
		}
		return TRUE;

	case WM_HSCROLL:
		if((HWND)lp == GetDlgItem(hWnd,IDC_SLIDER_KEY_DELAY)){
			_swprintf(szNum,_T("%d"),(int)SendMessage((HWND)lp,TBM_GETPOS,0,0));
			SetWindowText(GetDlgItem(hWnd,IDC_EDIT_KEY_DELAY),szNum);
		}
		else if((HWND)lp == GetDlgItem(hWnd,IDC_SLIDER_WAIT_UPDATE)){
			_swprintf(szNum,_T("%d"),(int)SendMessage((HWND)lp,TBM_GETPOS,0,0));
			SetWindowText(GetDlgItem(hWnd,IDC_EDIT_WAIT_UPDATE),szNum);
		}
		else if((HWND)lp == GetDlgItem(hWnd,IDC_SLIDER_RETRY)){
			_swprintf(szNum,_T("%d"),(int)SendMessage((HWND)lp,TBM_GETPOS,0,0));
			SetWindowText(GetDlgItem(hWnd,IDC_EDIT_RETRY),szNum);
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd,0);
		return TRUE;
	}

	return FALSE;
}

