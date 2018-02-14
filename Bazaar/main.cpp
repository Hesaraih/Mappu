#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define MAIN_CPP

#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <process.h>//0.09で追加
#include <stdlib.h>//0.09で追加
#include "resource.h"
#include "main.h"
#include "setting.h"
#include "ffxiinfo.h"
#include "menu.h"
#include "process.h"
#include "ini.h"
#include "list.h"
#include "UseWindowerHelper.h"
#include "bazaar.h"
#include "searchaddr.h"//0.08で追加

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"version.lib")

//0.07でswprintf及びwsprintfを_swprintfに変更

//WinMain
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
	DialogBox(hInst,MAKEINTRESOURCE(IDD_DIALOG),NULL,(DLGPROC)WndProc);

	return true;
}


//ウィンドウプロシージャIDD_DIALOG
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	static HMENU hMenu,hSubMenu0,hSubMenu1,hSubMenu2,hSubMenu3;
	static HMENU hPopupMenu,hSubPopupMenu;

	switch(msg){
	case WM_INITDIALOG:
		//ロケールをOSデフォルトに設定
		//VCの場合これが無いとIOで文字化けする
		_wsetlocale(LC_ALL,_T(""));
		//COMを初期化ShellExecute用 0.08で追加
		CoInitializeEx(NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
		//ini読込み
		ReadIni(hWnd);
		//メニュー作成
		hMenu = LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU));
		hSubMenu0 = GetSubMenu(hMenu,0);
		hSubMenu1 = GetSubMenu(hMenu,1);
		hSubMenu2 = GetSubMenu(hMenu,2);
		hSubMenu3 = GetSubMenu(hMenu,3);
		DeleteMenu(hSubMenu1,IDM_PID,FALSE);//ダミーで作ったメニュー項目を削除

		hPopupMenu = LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_POPUP));
		hSubPopupMenu = GetSubMenu(hPopupMenu,0);

		g_WantedIni = 0;

		//FFXiMain.dllのアドレスを取得
		if(GetDllModuleAddress(1) == 0){//失敗した時
			MenuItemEnable(hSubMenu0,IDM_START,false);
		}
		ChangePID(hWnd,1);
		SetMenu(hWnd,hMenu);

		if(g_DllAddress >= 0x400000 && g_AutoOffset){//0.08で追加
			if(GetOffsetAddressAll(g_polHandle,g_DllAddress) != 0){
				MessageBox(hWnd,_T("オフセット取得に失敗"),_T("Bazaar"),MB_ICONERROR);
				SendMessage(hWnd,WM_CLOSE,NULL,NULL);
				break;
			}
			ChangePID(hWnd,1);
		}

		if(InitWindowerHelper(hWnd) == 0){
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		}
		ListInit(hWnd);
		PostMessage(hWnd,WM_SIZE,0,0);

		//メニューの一部をグレーに
		MenuItemEnable(hSubMenu0,IDM_PAUSE,false);
		MenuItemEnable(hSubMenu0,IDM_STOP,false);

		if(g_ListColor){
			MenuChangeCheck(hSubMenu2,IDM_COLOR);
			ReadWantedFromString(g_WantedIniFileName);
		}

		//アイコンの設定
		SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)LoadImage((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDI_ICON),IMAGE_ICON,16,16,LR_DEFAULTCOLOR|LR_SHARED));
		break;

	case WM_COMMAND:
		if(LOWORD(wp) == IDM_START && g_byThreadFlag == 0){
			ReadWantedFromString(g_WantedIniFileName);
			if(g_BlackList)ReadBlackList();
			ListSort(hWnd,-1);
			if(g_ListUp)SendMessage(hWnd,WM_COMMAND,IDM_LISTUP,0);
			g_byThreadFlag = 0x01;
			//CreateThread(NULL,0,ThreadLoop,(LPVOID)hWnd,0,NULL);
			if(g_BazaarOrder == 0)_beginthreadex(NULL,0,ThreadLoop,(LPVOID)hWnd,0,NULL);//0.09で変更
			else if(g_BazaarOrder > 0 && g_BazaarOrder <= 4)_beginthreadex(NULL,0,ThreadLoopSort,(LPVOID)hWnd,0,NULL);
		}
		else if(LOWORD(wp) == IDM_START_WANTED && g_byThreadFlag == 0){
			ReadWantedFromString(g_WantedIniFileName);
			if(g_BlackList)ReadBlackList();
			ListSort(hWnd,-1);
			if(g_ListUp)SendMessage(hWnd,WM_COMMAND,IDM_LISTUP,0);
			g_byThreadFlag = 0x02;
			//CreateThread(NULL,0,ThreadLoop,(LPVOID)hWnd,0,NULL);
			if(g_BazaarOrder == 0)_beginthreadex(NULL,0,ThreadLoop,(LPVOID)hWnd,0,NULL);//0.09で変更
			else if(g_BazaarOrder > 0 && g_BazaarOrder <= 4)_beginthreadex(NULL,0,ThreadLoopSort,(LPVOID)hWnd,0,NULL);
		}
		else if(LOWORD(wp) == IDM_PAUSE && g_byThreadFlag != 0){//一時停止は4ビット目を立てる
			if((g_byThreadFlag & 0x10) == 0){
				g_byThreadFlag |= 0x10;
			}
			else {
				g_byThreadFlag &= 0x0F;
			}
		}
		else if(LOWORD(wp) == IDM_STOP){
			g_byThreadFlag = 0x00;
		}
		else if(LOWORD(wp) == IDM_OPEN_BAZAAR){
			if(g_byThreadFlag == 0x00){//スレッドが動いてない
				switch(ListSendOpenBazaar(hWnd)){
				case -1:
					MessageBox(hWnd,_T("キャラクターが選択されていません"),BAZAAR_STRING,MB_OK|MB_ICONINFORMATION);
					break;

				case 0:
					MessageBox(hWnd,_T("対象キャラクターは可視範囲内にいません"),BAZAAR_STRING,MB_OK|MB_ICONINFORMATION);
					break;
				}
			}
		}
		else if(LOWORD(wp) == IDM_EXIT){
			PostMessage(hWnd,WM_CLOSE,0,0);
		}
		else if(LOWORD(wp) >= IDM_PID && LOWORD(wp) < IDM_PID + MAX_PID_COUNT){
			if(g_byThreadFlag == 0x00){
				if(pKeyboardHelper)CloseWindowerHelper();
				ChangePID(hWnd,LOWORD(wp) - IDM_PID);
				if(g_AutoOffset){
					if(GetOffsetAddressAll(g_polHandle,g_DllAddress) != 0){
						MessageBox(hWnd,_T("オフセット取得に失敗"),_T("Bazaar"),MB_ICONERROR);
						SendMessage(hWnd,WM_CLOSE,NULL,NULL);
						break;
					}
					ChangePID(hWnd,LOWORD(wp) - IDM_PID);
				}
				if(InitWindowerHelper(hWnd) == 0){
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;
				}
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_SAVE_HTML){
			if(g_byThreadFlag == 0x00){
				if(g_ListUp == 0 || g_ExtractSave == 0){
					ListWriteFileHTML(hWnd,IDC_LIST);
				}
				else{
					ListWriteFileHTML(hWnd,IDC_LIST_LISTUP);
				}
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_SAVE_CSV){
			if(g_byThreadFlag == 0x00){
				if(g_ListUp == 0 || g_ExtractSave == 0){
					ListWriteFileCSV(hWnd,IDC_LIST);
				}
				else{
					ListWriteFileCSV(hWnd,IDC_LIST_LISTUP);
				}
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_READ_CSV){
			if(g_byThreadFlag == 0){
				ListReadFileCSV(hWnd);
				if(g_ListUp){
					ListMakeListUp(hWnd);
				}
				InvalidateRect(GetDlgItem(hWnd,IDC_LIST),NULL,TRUE);
				InvalidateRect(GetDlgItem(hWnd,IDC_LIST_LISTUP),NULL,TRUE);
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_COLOR){
			ReadWantedFromString(g_WantedIniFileName);
			if(MenuGetCheck(hSubMenu2,IDM_COLOR)){
				g_ListColor = 0;
			}
			else{
				g_ListColor = 1;
			}
			MenuChangeCheck(hSubMenu2,IDM_COLOR);
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST),NULL,TRUE);
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST_LISTUP),NULL,TRUE);
		}
		else if(LOWORD(wp) == IDM_LISTUP){
			if(g_byThreadFlag == 0 && ListView_GetItemCount(GetDlgItem(hWnd,IDC_LIST))){
				if(MenuGetCheck(hSubMenu2,IDM_LISTUP)){
					ShowWindow(GetDlgItem(hWnd,IDC_LIST),SW_NORMAL);
					ShowWindow(GetDlgItem(hWnd,IDC_LIST_LISTUP),SW_HIDE);
					g_ListUp = 0;
				}
				else{
					ShowWindow(GetDlgItem(hWnd,IDC_LIST),SW_HIDE);
					ShowWindow(GetDlgItem(hWnd,IDC_LIST_LISTUP),SW_NORMAL);
					ListMakeListUp(hWnd);
					g_ListUp = 1;
				}
				MenuChangeCheck(hSubMenu2,IDM_LISTUP);
				InvalidateRect(GetDlgItem(hWnd,IDC_LIST),NULL,TRUE);
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_CLEAR){
			if(g_byThreadFlag == 0x00){
				ListView_DeleteAllItems(GetDlgItem(hWnd,IDC_LIST));
				ListView_DeleteAllItems(GetDlgItem(hWnd,IDC_LIST_LISTUP));
			}
			else{
				MessageBeep(1);
			}
		}
		else if(LOWORD(wp) == IDM_RELOAD_WANTED){
			ReadWantedFromString(g_WantedIniFileName);
			if(g_ListUp){
				ListMakeListUp(hWnd);
			}
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST),NULL,TRUE);
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST_LISTUP),NULL,TRUE);
		}
		else if(LOWORD(wp) == IDM_OPEN_WANTED){
			ShellExecute(NULL,_T("open"),g_WantedIniFileName,NULL,NULL,SW_SHOWDEFAULT);
		}
		else if(LOWORD(wp) >= IDM_LOAD_WANTED && LOWORD(wp) < IDM_LOAD_WANTED + 100){
			GetMenuString(hMenu,LOWORD(wp),g_WantedIniFileName,sizeof(g_WantedIniFileName)/sizeof(_TCHAR)-1,MF_BYCOMMAND);
			ReadWantedFromString(g_WantedIniFileName);
			if(g_ListUp){
				ListMakeListUp(hWnd);
			}
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST),NULL,TRUE);
			InvalidateRect(GetDlgItem(hWnd,IDC_LIST_LISTUP),NULL,TRUE);
		}
		else if(LOWORD(wp) == IDM_OPEN_BLIST){
			ShellExecute(NULL,_T("open"),_T("blist.ini"),NULL,NULL,SW_SHOWDEFAULT);
		}
		else if(LOWORD(wp) == IDM_SETTING){
			DialogBox((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDD_SETTING),hWnd,(DLGPROC)DlgProcSetting);
		}
		else if(LOWORD(wp) == IDM_README_TXT){
			ShellExecute(NULL,_T("open"),_T("readme.txt"),NULL,NULL,SW_SHOWDEFAULT);
		}
		else if(LOWORD(wp) == IDM_VERSION){
			ShowVersion(hWnd);
		}
		else if(LOWORD(wp) == IDM_FFXIAH){
			ListOpenFFXIAH(hWnd);
			break;
		}
		break;

	case WM_INITMENUPOPUP:
		if(g_byThreadFlag == 0){
			MenuMakePID(hMenu);//メニュー作成
		}
		MenuMakeFindWantedIni(hMenu);//wantedメニュー作成

		if(ListView_GetItemCount(GetDlgItem(hWnd,IDC_LIST)) == 0){
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_HTML,false);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_CSV,false);
		}
		else{
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_HTML,true);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_CSV,true);
		}

		if(g_ProcessID == 0){
			MenuItemEnable(hSubMenu0,IDM_START,false);
			MenuItemEnable(hSubMenu0,IDM_START_WANTED,false);
			MenuItemEnable(hSubMenu0,IDM_PAUSE,false);
			MenuItemEnable(hSubMenu0,IDM_STOP,false);
			MenuItemEnable(hSubMenu0,IDM_OPEN_BAZAAR,false);
			MenuItemEnable(hSubMenu2,IDM_CLEAR,true);
			MenuItemEnable(hSubMenu3,IDM_SETTING,true);
			//break;//0.08で削除
		}

		if(g_byThreadFlag == 1 || g_byThreadFlag == 2){
			MenuItemEnable(hSubMenu0,IDM_START,false);
			MenuItemEnable(hSubMenu0,IDM_START_WANTED,false);
			MenuItemEnable(hSubMenu0,IDM_PAUSE,true);
			MenuItemEnable(hSubMenu0,IDM_STOP,true);
			MenuItemEnable(hSubMenu0,IDM_PAUSE,true);
			MenuItemEnable(hSubMenu0,IDM_STOP,true);
			MenuItemEnable(hSubMenu2,IDM_CLEAR,false);
			MenuItemEnable(hSubMenu3,IDM_SETTING,false);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_HTML,false);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_CSV,false);
		}
		else if(g_byThreadFlag > 0x10){//一時停止中
			MenuItemEnable(hSubMenu0,IDM_START,false);
			MenuItemEnable(hSubMenu0,IDM_START_WANTED,false);
			MenuItemEnable(hSubMenu0,IDM_PAUSE,true);
			MenuItemEnable(hSubMenu0,IDM_STOP,true);
			MenuItemEnable(hSubMenu2,IDM_CLEAR,false);
			MenuItemEnable(hSubMenu3,IDM_SETTING,false);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_HTML,false);
			MenuItemEnable(GetSubMenu(hSubMenu2,0),IDM_SAVE_CSV,false);
		}
		else{
			MenuItemEnable(hSubMenu0,IDM_START,true);
			MenuItemEnable(hSubMenu0,IDM_START_WANTED,true);
			MenuItemEnable(hSubMenu0,IDM_PAUSE,false);
			MenuItemEnable(hSubMenu0,IDM_STOP,false);
			MenuItemEnable(hSubMenu2,IDM_CLEAR,true);
			MenuItemEnable(hSubMenu3,IDM_SETTING,true);
		}
		
		if(g_byThreadFlag == 0x00 && 
			(
			ListView_GetNextItem(GetDlgItem(hWnd,IDC_LIST),-1,LVNI_SELECTED) != -1
			||
			ListView_GetNextItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),-1,LVNI_SELECTED) != -1
			)
		){
			MenuItemEnable(hSubMenu0,IDM_OPEN_BAZAAR,true);
		}
		else{
			MenuItemEnable(hSubMenu0,IDM_OPEN_BAZAAR,false);
		}
		break;
			
	case WM_NOTIFY:
		if((int)wp == IDC_LIST || (int)wp == IDC_LIST_LISTUP){
			switch(((LV_DISPINFO*)lp)->hdr.code){
			case LVN_COLUMNCLICK://カラムクリック時
				if(g_byThreadFlag == 0){//実行中はソート不可
					ListResetLparam(hWnd);
					ListSort(hWnd,((NM_LISTVIEW*)lp)->iSubItem);
					if(g_ListUp){
						ListMakeListUp(hWnd);
					}
				}
				else{
					MessageBeep(1);
				}
				break;
			}
		}
		else{
			if(((LPNMHDR)lp)->code == HDN_ENDTRACK){//カラム幅変更
				g_ColumnWidth[((NMHEADER*)lp)->iItem] = ((NMHEADER*)lp)->pitem->cxy;
				ListChangeHeaderWidth(hWnd,((NMHEADER*)lp)->iItem,((NMHEADER*)lp)->pitem->cxy);
			}
			else if(((LPNMHDR)lp)->code == HDN_DIVIDERDBLCLICK){//ヘッダーダブルクリック
				g_ColumnWidth[((NMHEADER*)lp)->iItem] = ListGetHeaderWidth(((LPNMHDR)lp)->hwndFrom,((NMHEADER*)lp)->iItem);
				ListChangeHeaderWidth(hWnd,((NMHEADER*)lp)->iItem,g_ColumnWidth[((NMHEADER*)lp)->iItem]);
			}
		}
		if(((LPNMHDR)lp)->hwndFrom == GetDlgItem(hWnd,IDC_LIST) || ((LPNMHDR)lp)->hwndFrom == GetDlgItem(hWnd,IDC_LIST_LISTUP)){
			if(((LPNMLISTVIEW)lp)->hdr.code == NM_CUSTOMDRAW){
				switch(((LPNMLVCUSTOMDRAW)lp)->nmcd.dwDrawStage){
				case CDDS_PREPAINT:
					SetWindowLong(hWnd,DWL_MSGRESULT,(LONG)CDRF_NOTIFYITEMDRAW);
					return TRUE;
				case CDDS_ITEMPREPAINT:
					_TCHAR szBuf[0x30];
					_TCHAR szPrice[0x30];
					ZeroMemory(szBuf,sizeof(szBuf));
					ZeroMemory(szPrice,sizeof(szPrice));

					if(g_ListColor){
						ListView_GetItemText(((LPNMHDR)lp)->hwndFrom,((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec,COLUMN_NAME,szBuf,sizeof(szBuf)/sizeof(_TCHAR)-1);
						for(int i=0;i<100;i++){
							if(g_Wanted[i].use && wcscmp(g_Wanted[i].szName,szBuf) == 0){
								ListView_GetItemText(((LPNMHDR)lp)->hwndFrom,((LPNMLVCUSTOMDRAW)lp)->nmcd.dwItemSpec,COLUMN_PRICE,szBuf,sizeof(szBuf)/sizeof(_TCHAR)-1);
								
								for(int j=0,nPos=0;szBuf[j];j++){
									if(szBuf[j] >= '0' && szBuf[j] <= '9'){
										szPrice[nPos++] = szBuf[j];
									}
								}
								if(wcslen(szPrice) && g_Wanted[i].price >= (int)wcstoul(szPrice,NULL,10)){
									((LPNMLVCUSTOMDRAW)lp)->clrTextBk = g_ColorLow;//背景色を変更
								}
								else{
									((LPNMLVCUSTOMDRAW)lp)->clrTextBk = g_ColorHigh;//背景色を変更
								}
							}
						}
					}
					SetWindowLong(hWnd,DWL_MSGRESULT,(LONG)CDRF_NEWFONT);
					return TRUE;
				default:
					SetWindowLong(hWnd,DWL_MSGRESULT,(LONG)CDRF_DODEFAULT);
					return FALSE;
				}
			}
		}
		break;

	case WM_RBUTTONUP:
		if(g_byThreadFlag == 0x00 && 
			(
			ListView_GetNextItem(GetDlgItem(hWnd,IDC_LIST),-1,LVNI_SELECTED) != -1
			||
			ListView_GetNextItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),-1,LVNI_SELECTED) != -1
			)
		){
			POINT pos;
			pos.x = LOWORD(lp);
			pos.y = HIWORD(lp);
			if(0 == ClientToScreen(hWnd,&pos)){
			}

			if(g_UseFFXIAH == 0 || wcslen(g_FFXIAH) == 0){
				MenuItemEnable(hSubPopupMenu,IDM_FFXIAH,false);
			}
			else{
				MenuItemEnable(hSubPopupMenu,IDM_FFXIAH,true);
			}

			SetForegroundWindow(hWnd);
			if(0 == TrackPopupMenu(hSubPopupMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,0,hWnd,NULL)){
			}
			PostMessage(hWnd,0,0,0);
		}
		break;

	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd,&rect);
		//サイズ変更があった時、各コントロールの大きさを変更
		MoveWindow(GetDlgItem(hWnd,IDC_LIST),1,1,rect.right-1*2,rect.bottom-1*2,1);
		MoveWindow(GetDlgItem(hWnd,IDC_LIST_LISTUP),1,1,rect.right-1*2,rect.bottom-1*2,1);
		break;

	case WM_CLOSE:
		if(g_SaveWindow && WriteIniWindow(hWnd) == 0){
			MessageBox(hWnd,_T("ウィンドウの位置/大きさの保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
		}
		if(g_SaveColumn && WriteIniColumn() == 0){
			MessageBox(hWnd,_T("カラム幅の保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
		}
		if(pKeyboardHelper)CloseWindowerHelper();
		DestroyMenu(hMenu);
		DestroyWindow(hWnd);
		break;
	}

	return 0;
}


//DWORD WINAPI ThreadLoop(LPVOID vdParam)
unsigned WINAPI ThreadLoop(void* vdParam)//0.09で変更
{
	char szCmd[MAX_PATH];
	char szName[0x20];
	_TCHAR szTTitle[MAX_PATH];
	_TCHAR szTText[0x400];
	_TCHAR szTBuf[0x400];
	_TCHAR szTBazzarName[MAX_PATH];
	_TCHAR szTBazzarComment[MAX_PATH];
	_TCHAR szTBazaarPrice[MAX_PATH];
	_TCHAR szTBazaarNumber[MAX_PATH];
	int nBazaarNumber;
	int nBazaarPrice;
	int nBazaarTimeOut;
	INFORMATION pcInfo,myInfo;
	LVITEM item;
	int nRow;
	BYTE byBlackList;

	myInfo = GetMyInfo();
	nRow = ListView_GetItemCount(GetDlgItem((HWND)vdParam,IDC_LIST));

	for(WORD id=0x400;id<0x700;id++){
		//フラグチェック
		if(g_byThreadFlag == 0x00){//中止処理
			break;
		}
		else if(g_byThreadFlag & 0x10){//一時停止
			_swprintf(szTTitle,_T("%s - %s %d%% 一時停止"),BAZAAR_STRING,myInfo.name,100*(id - 0x400 + 1)/(0x700-0x400));
			SetWindowText((HWND)vdParam,szTTitle);
			while(g_byThreadFlag & 0x10){
				Sleep(10);
			}
		}

		pcInfo = GetNPCInfo(id);
#ifndef	DEBUG
		if(pcInfo.id == myInfo.id){
			continue;
		}
#endif
		_swprintf(szTTitle,_T("%s - %s %d%%"),BAZAAR_STRING,myInfo.name,100*(id - 0x400 + 1)/(0x700-0x400));
		SetWindowText((HWND)vdParam,szTTitle);

		if(pcInfo.inrange && pcInfo.bazaar == 2){
			if(g_BlackList){//ブラックリスト
				byBlackList = 0;
				for(int j=0;j<100;j++){
					if(wcscmp(pcInfo.name,g_szBlackListName[j]) == 0){
						byBlackList = 1;
						break;
					}
				}
				if(byBlackList == 1){
					continue;
				}
			}
			wcstombs(szName,pcInfo.name,sizeof(szName));
			//sprintf(szCmd,"/c %s",szName);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			Sleep(g_KeyDelay);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			Sleep(g_KeyDelay);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			if(g_TargetName){
				Sleep(g_KeyDelay);
				sprintf(szCmd,"/ta %s",szName);//0.08で追加
				DLL_CKHSendString(pKeyboardHelper,szCmd);//「/ta name」発行
			}
			Sleep(g_KeyDelay);
			sprintf(szCmd,"/c %s",szName);//0.08で移動
			DLL_CKHSendString(pKeyboardHelper,szCmd);//「/c name」発行
			Sleep(g_WaitUpdate);
			DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,true);//Enter
			DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,false);
			for(int j=0;j<g_Retry;j++){
				Sleep(g_WaitUpdate);
				if(BazaarWaitUpdate(id) == 1){
					nBazaarTimeOut = g_Retry;
					nRow -= ListDeletePC((HWND)vdParam,pcInfo.name);

					ZeroMemory(szTText,sizeof(szTText));

					for(WORD w=0;w<BazaarGetItemCount();w++){
						wcscpy(szTBazzarName,BazaarGetItemName(w));
						wcscpy(szTBazzarComment,BazaarGetItemNote(w));
						nBazaarNumber = BazaarGetItemNumber(w);
						nBazaarPrice = BazaarGetItemPrice(w);

						if(wcslen(szTBazzarName) == 0 || nBazaarNumber < 1 || nBazaarPrice < 1){
							if(nBazaarTimeOut-- <= 0){//飛ばす
								nBazaarTimeOut = g_Retry;
								continue;
							}
							else{//再試行
								w--;
								Sleep(g_KeyDelay);
								continue;
							}
						}
						else if(g_UseFilterPrice && g_FilterPrice <= nBazaarPrice){
							continue;
						}
						
						
						item.mask = LVIF_TEXT|LVIF_PARAM;//LVIF_PARAM追加でソートオン
						item.iItem = nRow;//行
						item.iSubItem = COLUMN_PC;//列
						item.pszText = pcInfo.name;
						item.lParam = (LPARAM)nRow;//ソート用lParamを設定
						if(-1 == ListView_InsertItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
							g_byThreadFlag = 0;
							//ExitThread(-1);
							_endthreadex(-1);//0.09で変更
							return -1; 
						}

						item.mask = LVIF_TEXT;
						item.iSubItem = COLUMN_NAME;//列
						item.pszText = szTBazzarName;
						if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
							g_byThreadFlag = 0;
							//ExitThread(-1);
							_endthreadex(-1);//0.09で変更
							return -1; 
						}

						item.mask = LVIF_TEXT;
						item.iSubItem = COLUMN_PRICE;//列
						if(nBazaarPrice >= 1000000){
							_swprintf(szTBazaarPrice,_T("%d,%03d,%03d"),nBazaarPrice/1000/1000,nBazaarPrice/1000%1000,nBazaarPrice%1000);
						}
						else if(nBazaarPrice >= 1000){
							_swprintf(szTBazaarPrice,_T("%d,%03d"),nBazaarPrice/1000%1000,nBazaarPrice%1000);
						}
						else{
							_swprintf(szTBazaarPrice,_T("%d"),nBazaarPrice);
						}
						item.pszText = szTBazaarPrice;
						if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
							g_byThreadFlag = 0;
							//ExitThread(-1);
							_endthreadex(-1);//0.09で変更
							return -1; 
						}

						item.mask = LVIF_TEXT;
						item.iSubItem = COLUMN_NUMBER;//列
						_swprintf(szTBazaarNumber,_T("%d"),nBazaarNumber);
						item.pszText = szTBazaarNumber;
						if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
							g_byThreadFlag = 0;
							//ExitThread(-1);
							_endthreadex(-1);//0.09で変更
							return -1; 
						}

						item.mask = LVIF_TEXT;
						item.iSubItem = COLUMN_NOTE;//列
						item.pszText = szTBazzarComment;
						if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
							g_byThreadFlag = 0;
							//ExitThread(-1);
							_endthreadex(-1);//0.09で変更
							return -1; 
						}

						nRow++;

						if((g_byThreadFlag & 0x0F) == 0x02){//wanted.iniをチェックする
							for(int n=0;n<100;n++){//同一PCのバザーをチェック
								if(g_Wanted[n].use && wcscmp(g_Wanted[n].szName,szTBazzarName) == 0 && g_Wanted[n].price >= nBazaarPrice){
									wcscpy(szTBuf,szTText);
									if(wcslen(szTText)){//2個目以降
										_swprintf(szTText,_T("%s\n\"%s\"\t金額%s 個数%d"),szTBuf,szTBazzarName,szTBazaarPrice,nBazaarNumber);
									}
									else{//1個目
										_swprintf(szTText,_T("該当品名を表示します\n\"%s\"\t金額%s 個数%d"),szTBazzarName,szTBazaarPrice,nBazaarNumber);
									}
									//InvalidateRect(GetDlgItem((HWND)vdParam,IDC_LIST),NULL,TRUE);//0.07で削除
								}
							}
						}
					}
					if(g_AutoScroll){
						ListView_EnsureVisible(GetDlgItem((HWND)vdParam,IDC_LIST),nRow-1,FALSE);
					}
					InvalidateRect(GetDlgItem((HWND)vdParam,IDC_LIST),NULL,TRUE);
					
					if((g_byThreadFlag & 0x0F) == 0x02 && wcslen(szTText)){
						MessageBox((HWND)vdParam,szTText,BAZAAR_STRING,MB_OK|MB_ICONINFORMATION);
					}
					else{//0.09で追加
						WORD wBazNum = BazaarGetItemCount();
						DWORD dwSleepTime = 0;
						if(g_BazaarNumWait05 > 0 && wBazNum <= 5){
							if(g_BazaarNumWait05 <= 5)dwSleepTime = g_BazaarNumWait05 * 1000;
							else if(g_BazaarNumWait05 == 6)dwSleepTime = rand()%3001;
							else if(g_BazaarNumWait05 == 7)dwSleepTime = rand()%5001;
							else if(g_BazaarNumWait05 == 8)dwSleepTime = 3000 + rand()%2001;
						}
						//else if(g_BazaarNumWait10 > 0 && wBazNum <= 10){
						else if(g_BazaarNumWait10 > 0 && wBazNum > 5 && wBazNum <= 10){//0.09aで変更
							if(g_BazaarNumWait10 <= 5)dwSleepTime = g_BazaarNumWait10 * 1000;
							else if(g_BazaarNumWait10 == 6)dwSleepTime = rand()%3001;
							else if(g_BazaarNumWait10 == 7)dwSleepTime = rand()%5001;
							else if(g_BazaarNumWait10 == 8)dwSleepTime = 3000 + rand()%2001;
						}
						//else if(g_BazaarNumWait20 > 0 && wBazNum <= 20){
						else if(g_BazaarNumWait10 > 0 && wBazNum > 10 && wBazNum <= 20){//0.09aで変更
							if(g_BazaarNumWait20 <= 5)dwSleepTime = g_BazaarNumWait20 * 1000;
							else if(g_BazaarNumWait20 == 6)dwSleepTime = rand()%3001;
							else if(g_BazaarNumWait20 == 7)dwSleepTime = rand()%5001;
							else if(g_BazaarNumWait20 == 8)dwSleepTime = 3000 + rand()%2001;
						}
						else if(g_BazaarNumWait21 > 0 && wBazNum > 20){
							if(g_BazaarNumWait21 <= 5)dwSleepTime = g_BazaarNumWait21 * 1000;
							else if(g_BazaarNumWait21 == 6)dwSleepTime = rand()%3001;
							else if(g_BazaarNumWait21 == 7)dwSleepTime = rand()%5001;
							else if(g_BazaarNumWait21 == 8)dwSleepTime = 3000 + rand()%2001;
						}
						Sleep(dwSleepTime);
					}
					break;
				}
			}
		}
	}
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
	Sleep(g_KeyDelay);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
	Sleep(g_KeyDelay);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);

	
	InvalidateRect(GetDlgItem((HWND)vdParam,IDC_LIST),NULL,TRUE);

	if(g_byThreadFlag == 0x01 || g_byThreadFlag == 0x02){
		_swprintf(szTTitle,_T("%s - %s 100%%"),BAZAAR_STRING,myInfo.name);
		SetWindowText((HWND)vdParam,szTTitle);
		MessageBox((HWND)vdParam,_T("チェック完了"),BAZAAR_STRING,MB_OK);
	}
	else if(g_byThreadFlag == 0){
		_swprintf(szTTitle,_T("%s - %s"),BAZAAR_STRING,myInfo.name);
		SetWindowText((HWND)vdParam,szTTitle);
		MessageBox((HWND)vdParam,_T("中止しました"),BAZAAR_STRING,MB_OK);
	}
	g_byThreadFlag = 0;

	//ExitThread(0);
	_endthreadex(0);//0.09で変更
    return 0; 

}


//ソート 0.09で追加
unsigned WINAPI ThreadLoopSort(void* vdParam)
{
	char szCmd[MAX_PATH];
	char szName[0x20];
	_TCHAR szTTitle[MAX_PATH];
	_TCHAR szTText[0x400];
	_TCHAR szTBuf[0x400];
	_TCHAR szTBazzarName[MAX_PATH];
	_TCHAR szTBazzarComment[MAX_PATH];
	_TCHAR szTBazaarPrice[MAX_PATH];
	_TCHAR szTBazaarNumber[MAX_PATH];
	int nBazaarNumber;
	int nBazaarPrice;
	int nBazaarTimeOut;
	INFORMATION pcInfo,myInfo;
	LVITEM item;
	int nRow;
	BYTE byBlackList;

	BAZAAR_DATA *BazData;
	int DataNum = 0;

	BazData = (BAZAAR_DATA*)malloc(sizeof(BAZAAR_DATA) * 0x300);
	if(BazData == NULL){
		MessageBox((HWND)vdParam,_T("メモリ確保失敗"),NULL,MB_ICONERROR);
		_endthreadex(-1);
		return -1; 
	}

	myInfo = GetMyInfo();
	nRow = ListView_GetItemCount(GetDlgItem((HWND)vdParam,IDC_LIST));

	//データを作成
	for(WORD id=0x400;id<0x700;id++){
		pcInfo = GetNPCInfo(id);
#ifndef	DEBUG
		if(pcInfo.id == myInfo.id){
			continue;
		}
#endif
		_swprintf(szTTitle,_T("%s - %s データ取得中 %d%%"),BAZAAR_STRING,myInfo.name,100*(id - 0x400 + 1)/(0x700-0x400));
		SetWindowText((HWND)vdParam,szTTitle);

		if(pcInfo.inrange && pcInfo.bazaar == 2){
			wcscpy(BazData[DataNum].szTName,pcInfo.name);
			BazData[DataNum].id = pcInfo.id;
			BazData[DataNum].fx = pcInfo.x;
			BazData[DataNum].fz = pcInfo.z;
			DataNum++;
		}
	}

	//ソート
	qsort(BazData,DataNum,sizeof(BAZAAR_DATA),compfunc);

	for(int num=0;num<DataNum;num++){
		//フラグチェック
		if(g_byThreadFlag == 0x00){//中止処理
			break;
		}
		else if(g_byThreadFlag & 0x10){//一時停止
			_swprintf(szTTitle,_T("%s - %s %d%% 一時停止"),BAZAAR_STRING,myInfo.name,100*(num + 1)/(DataNum));
			SetWindowText((HWND)vdParam,szTTitle);
			while(g_byThreadFlag & 0x10){
				Sleep(10);
			}
		}

		_swprintf(szTTitle,_T("%s - %s %d%%"),BAZAAR_STRING,myInfo.name,100*(num + 1)/(DataNum));
		SetWindowText((HWND)vdParam,szTTitle);

		if(g_BlackList){//ブラックリスト
			byBlackList = 0;
			for(int j=0;j<100;j++){
				if(wcscmp(BazData[num].szTName,g_szBlackListName[j]) == 0){
					byBlackList = 1;
					break;
				}
			}
			if(byBlackList == 1){
				continue;
			}
		}
		
		wcstombs(szName,BazData[num].szTName,sizeof(szName));
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
		Sleep(g_KeyDelay);
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
		Sleep(g_KeyDelay);
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
		DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
		if(g_TargetName){
			Sleep(g_KeyDelay);
			sprintf(szCmd,"/ta %s",szName);
			DLL_CKHSendString(pKeyboardHelper,szCmd);//「/ta name」発行
		}
		Sleep(g_KeyDelay);
		sprintf(szCmd,"/c %s",szName);
		DLL_CKHSendString(pKeyboardHelper,szCmd);//「/c name」発行
		Sleep(g_WaitUpdate);
		DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,true);//Enter
		DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,false);
		for(int j=0;j<g_Retry;j++){
			Sleep(g_WaitUpdate);
			if(BazaarWaitUpdate(BazData[num].id) == 1){
				nBazaarTimeOut = g_Retry;
				nRow -= ListDeletePC((HWND)vdParam,BazData[num].szTName);

				ZeroMemory(szTText,sizeof(szTText));

				for(WORD w=0;w<BazaarGetItemCount();w++){
					wcscpy(szTBazzarName,BazaarGetItemName(w));
					wcscpy(szTBazzarComment,BazaarGetItemNote(w));
					nBazaarNumber = BazaarGetItemNumber(w);
					nBazaarPrice = BazaarGetItemPrice(w);

					if(wcslen(szTBazzarName) == 0 || nBazaarNumber < 1 || nBazaarPrice < 1){
						if(nBazaarTimeOut-- <= 0){//飛ばす
							nBazaarTimeOut = g_Retry;
							continue;
						}
						else{//再試行
							w--;
							Sleep(g_KeyDelay);
							continue;
						}
					}
					else if(g_UseFilterPrice && g_FilterPrice <= nBazaarPrice){
						continue;
					}
						
						
					item.mask = LVIF_TEXT|LVIF_PARAM;//LVIF_PARAM追加でソートオン
					item.iItem = nRow;//行
					item.iSubItem = COLUMN_PC;//列
					item.pszText = BazData[num].szTName;
					item.lParam = (LPARAM)nRow;//ソート用lParamを設定
					if(-1 == ListView_InsertItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
						g_byThreadFlag = 0;
						free(BazData);
						_endthreadex(-1);
						return -1; 
					}

					item.mask = LVIF_TEXT;
					item.iSubItem = COLUMN_NAME;//列
					item.pszText = szTBazzarName;
					if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
						g_byThreadFlag = 0;
						free(BazData);
						_endthreadex(-1);
						return -1; 
					}

					item.mask = LVIF_TEXT;
					item.iSubItem = COLUMN_PRICE;//列
					if(nBazaarPrice >= 1000000){
						_swprintf(szTBazaarPrice,_T("%d,%03d,%03d"),nBazaarPrice/1000/1000,nBazaarPrice/1000%1000,nBazaarPrice%1000);
					}
					else if(nBazaarPrice >= 1000){
						_swprintf(szTBazaarPrice,_T("%d,%03d"),nBazaarPrice/1000%1000,nBazaarPrice%1000);
					}
					else{
						_swprintf(szTBazaarPrice,_T("%d"),nBazaarPrice);
					}
					item.pszText = szTBazaarPrice;
					if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
						g_byThreadFlag = 0;
						free(BazData);
						_endthreadex(-1);
						return -1; 
					}

					item.mask = LVIF_TEXT;
					item.iSubItem = COLUMN_NUMBER;//列
					_swprintf(szTBazaarNumber,_T("%d"),nBazaarNumber);
					item.pszText = szTBazaarNumber;
					if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
						g_byThreadFlag = 0;
						free(BazData);
						_endthreadex(-1);
						return -1; 
					}

					item.mask = LVIF_TEXT;
					item.iSubItem = COLUMN_NOTE;//列
					item.pszText = szTBazzarComment;
					if(FALSE == ListView_SetItem(GetDlgItem((HWND)vdParam,IDC_LIST),&item)){
						g_byThreadFlag = 0;
						free(BazData);
						_endthreadex(-1);
						return -1; 
					}

					nRow++;

					if((g_byThreadFlag & 0x0F) == 0x02){//wanted.iniをチェックする
						for(int n=0;n<100;n++){//同一PCのバザーをチェック
							if(g_Wanted[n].use && wcscmp(g_Wanted[n].szName,szTBazzarName) == 0 && g_Wanted[n].price >= nBazaarPrice){
								wcscpy(szTBuf,szTText);
								if(wcslen(szTText)){//2個目以降
									_swprintf(szTText,_T("%s\n\"%s\"\t金額%s 個数%d"),szTBuf,szTBazzarName,szTBazaarPrice,nBazaarNumber);
								}
								else{//1個目
									_swprintf(szTText,_T("該当品名を表示します\n\"%s\"\t金額%s 個数%d"),szTBazzarName,szTBazaarPrice,nBazaarNumber);
								}
							}
						}
					}
				}
				if(g_AutoScroll){
					ListView_EnsureVisible(GetDlgItem((HWND)vdParam,IDC_LIST),nRow-1,FALSE);
				}
				InvalidateRect(GetDlgItem((HWND)vdParam,IDC_LIST),NULL,TRUE);
					
				if((g_byThreadFlag & 0x0F) == 0x02 && wcslen(szTText)){
					MessageBox((HWND)vdParam,szTText,BAZAAR_STRING,MB_OK|MB_ICONINFORMATION);
				}
				else{//0.09で追加
					WORD wBazNum = BazaarGetItemCount();
					DWORD dwSleepTime = 0;
					if(g_BazaarNumWait05 > 0 && wBazNum <= 5){
						if(g_BazaarNumWait05 <= 5)dwSleepTime = g_BazaarNumWait05 * 1000;
						else if(g_BazaarNumWait05 == 6)dwSleepTime = rand()%3001;
						else if(g_BazaarNumWait05 == 7)dwSleepTime = rand()%5001;
						else if(g_BazaarNumWait05 == 8)dwSleepTime = 3000 + rand()%2001;
					}
					//else if(g_BazaarNumWait10 > 0 && wBazNum <= 10){
					else if(g_BazaarNumWait10 > 0 && wBazNum > 5 && wBazNum <= 10){//0.09aで変更
						if(g_BazaarNumWait10 <= 5)dwSleepTime = g_BazaarNumWait10 * 1000;
						else if(g_BazaarNumWait10 == 6)dwSleepTime = rand()%3001;
						else if(g_BazaarNumWait10 == 7)dwSleepTime = rand()%5001;
						else if(g_BazaarNumWait10 == 8)dwSleepTime = 3000 + rand()%2001;
					}
					//else if(g_BazaarNumWait20 > 0 && wBazNum <= 20){
					else if(g_BazaarNumWait20 > 0 && wBazNum > 10 && wBazNum <= 20){//0.09aで変更
						if(g_BazaarNumWait20 <= 5)dwSleepTime = g_BazaarNumWait20 * 1000;
						else if(g_BazaarNumWait20 == 6)dwSleepTime = rand()%3001;
						else if(g_BazaarNumWait20 == 7)dwSleepTime = rand()%5001;
						else if(g_BazaarNumWait20 == 8)dwSleepTime = 3000 + rand()%2001;
					}
					else if(g_BazaarNumWait21 > 0 && wBazNum > 20){
						if(g_BazaarNumWait21 <= 5)dwSleepTime = g_BazaarNumWait21 * 1000;
						else if(g_BazaarNumWait21 == 6)dwSleepTime = rand()%3001;
						else if(g_BazaarNumWait21 == 7)dwSleepTime = rand()%5001;
						else if(g_BazaarNumWait21 == 8)dwSleepTime = 3000 + rand()%2001;
					}
					Sleep(dwSleepTime);
				}
				break;
			}
		}
	}
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
	Sleep(g_KeyDelay);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
	Sleep(g_KeyDelay);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
	DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);

	
	InvalidateRect(GetDlgItem((HWND)vdParam,IDC_LIST),NULL,TRUE);

	if(g_byThreadFlag == 0x01 || g_byThreadFlag == 0x02){
		_swprintf(szTTitle,_T("%s - %s 100%%"),BAZAAR_STRING,myInfo.name);
		SetWindowText((HWND)vdParam,szTTitle);
		MessageBox((HWND)vdParam,_T("チェック完了"),BAZAAR_STRING,MB_OK);
	}
	else if(g_byThreadFlag == 0){
		_swprintf(szTTitle,_T("%s - %s"),BAZAAR_STRING,myInfo.name);
		SetWindowText((HWND)vdParam,szTTitle);
		MessageBox((HWND)vdParam,_T("中止しました"),BAZAAR_STRING,MB_OK);
	}
	g_byThreadFlag = 0;

	free(BazData);

	_endthreadex(0);
    return 0; 

}


void ShowVersion(HWND hWnd)
{
	_TCHAR szFileName[MAX_PATH];
	DWORD dwBlockSize;
	BYTE *pbyVersionInfo;

	if(GetModuleFileName(NULL,szFileName,sizeof(szFileName)/sizeof(_TCHAR)-1) == 0){
		MessageBox(hWnd,_T("Cant get VersionInfo"),NULL,MB_ICONERROR);
	}
	else{
		dwBlockSize = GetFileVersionInfoSize(szFileName,0);
		pbyVersionInfo = (BYTE*)malloc(dwBlockSize);
		if(pbyVersionInfo){
			if(GetFileVersionInfo(szFileName,0,dwBlockSize,(LPVOID)pbyVersionInfo) == FALSE){
				MessageBox(hWnd,_T("Cant get VersionInfo"),NULL,MB_ICONERROR);
			}
			else{
				struct {
					WORD wLanguage;
					WORD wCodePage;
				} *lpTranslate;
				VerQueryValue((LPVOID)pbyVersionInfo,_T("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,NULL);
				_TCHAR szDir[0x100];
				_swprintf(szDir,_T("\\StringFileInfo\\%04x%04x\\FileVersion"),lpTranslate->wLanguage,lpTranslate->wCodePage);
				void *pvVer;
				VerQueryValue((LPVOID)pbyVersionInfo,szDir,(LPVOID*)&pvVer,NULL);
				_swprintf(szDir,_T("\\StringFileInfo\\%04x%04x\\LegalCopyright"),lpTranslate->wLanguage,lpTranslate->wCodePage);
				void *pvCopyright;
				VerQueryValue((LPVOID)pbyVersionInfo,szDir,(LPVOID*)&pvCopyright,NULL);

				_TCHAR szVersion[0x100];
				_swprintf(szVersion,_T("Version %s\n\n%s"),pvVer,pvCopyright);
				MessageBox(hWnd,szVersion,_T("バージョン"),MB_OK|MB_ICONINFORMATION);
			}
		}
		free(pbyVersionInfo);
	}
}

//比較関数 0.09で追加
int compfunc(const void* c1,const void* c2)
{
#ifdef DEBUG
	float fx1,fx2,fz1,fz2;
	_TCHAR szName1[0x20],szName2[0x20];
	fx1 = ((BAZAAR_DATA*)c1)->fx;
	fz1 = ((BAZAAR_DATA*)c1)->fz;
	fx2 = ((BAZAAR_DATA*)c2)->fx;
	fz2 = ((BAZAAR_DATA*)c2)->fz;
	wcscpy(szName1,((BAZAAR_DATA*)c1)->szTName);
	wcscpy(szName2,((BAZAAR_DATA*)c2)->szTName);
#endif

	if(g_BazaarOrder == 1){//北
		return (int)(((BAZAAR_DATA*)c2)->fz - ((BAZAAR_DATA*)c1)->fz);
	}
	else if(g_BazaarOrder == 2){//南
		return (int)(((BAZAAR_DATA*)c1)->fz - ((BAZAAR_DATA*)c2)->fz);
	}
	else if(g_BazaarOrder == 3){//東
		return (int)(((BAZAAR_DATA*)c2)->fx - ((BAZAAR_DATA*)c1)->fx);
	}
	else if(g_BazaarOrder == 4){//西
		return (int)(((BAZAAR_DATA*)c1)->fx - ((BAZAAR_DATA*)c2)->fx);
	}
	else return 1;
}