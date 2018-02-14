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

#define LIST_CPP


#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "main.h"
#include "list.h"
#include "ffxiinfo.h"
#include "UseWindowerHelper.h"
#include "resource.h"


#pragma comment(lib,"Comctl32.lib")


//リストのカラム作成
int ListInit(HWND hWnd)
{
	LVCOLUMN col;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	//リストビューにEXスタイル付与
	ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd,IDC_LIST),
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER,//ダブルバッファ
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER
	);
	ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd,IDC_LIST_LISTUP),
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER,//ダブルバッファ
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER
	);

	ZeroMemory(&col,sizeof(col));
	
	//カラム作成
	col.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
	for(int i=0;i<COLUMN_MAX_COUNT;i++){
		col.cx = g_ColumnWidth[i];
		col.iSubItem = i;
		col.pszText = (_TCHAR*)szColumn[i];
		col.fmt = LVCFMT_LEFT;
		if(i == COLUMN_PRICE || i == COLUMN_NUMBER)col.fmt = LVCFMT_RIGHT;
		if(-1 == ListView_InsertColumn(GetDlgItem(hWnd,IDC_LIST),i,&col)){
			return 0;
		}
		if(-1 == ListView_InsertColumn(GetDlgItem(hWnd,IDC_LIST_LISTUP),i,&col)){
			return 0;
		}
	}

	//リストをサブクラス化
	WndProcMainClass = (WNDPROC)SetWindowLong(GetDlgItem(hWnd,IDC_LIST),GWL_WNDPROC,(LONG)WndProcSubClass);
	SetWindowLong(GetDlgItem(hWnd,IDC_LIST_LISTUP),GWL_WNDPROC,(LONG)WndProcSubClass);
	
	ShowWindow(GetDlgItem(hWnd,IDC_LIST),SW_NORMAL);
	ShowWindow(GetDlgItem(hWnd,IDC_LIST_LISTUP),SW_HIDE);

	hList = GetDlgItem(hWnd,IDC_LIST);
	hList_ListUp = GetDlgItem(hWnd,IDC_LIST_LISTUP);

	g_ListUp = 0;

	return 1;
}


//アプリケーション定義比較関数
//lp1がlp2より先行する場合は負の値、逆の時は正の値、同じ時は０を返す
int CALLBACK CmpProc(LPARAM lp1,LPARAM lp2,LPARAM lp3)
{
	static LV_FINDINFO lvf;
	static int nItem1,nItem2;
	static _TCHAR szBuf1[0x20],szBuf2[0x20];
	static int nValue1,nValue2;
	
	lvf.flags = LVFI_PARAM;
	lvf.lParam = lp1;
	nItem1 = ListView_FindItem(hList,-1,&lvf);
	
	lvf.lParam = lp2;
	nItem2 = ListView_FindItem(hList,-1,&lvf);
	
	ListView_GetItemText(hList,nItem1,(int)lp3,szBuf1,sizeof(szBuf1)/sizeof(_TCHAR));
	ListView_GetItemText(hList,nItem2,(int)lp3,szBuf2,sizeof(szBuf2)/sizeof(_TCHAR));

	if(lp3 == COLUMN_PRICE){
		_TCHAR szBuf[0x20];
		wcscpy(szBuf,szBuf1);
		ZeroMemory(szBuf1,sizeof(szBuf1));
		for(int i=0,nPos=0;szBuf[i];i++){
			if(szBuf[i] >= '0' && szBuf[i] <= '9'){
				szBuf1[nPos++] = szBuf[i];
			}
		}
		wcscpy(szBuf,szBuf2);
		ZeroMemory(szBuf2,sizeof(szBuf2));
		for(int i=0,nPos=0;szBuf[i];i++){
			if(szBuf[i] >= '0' && szBuf[i] <= '9'){
				szBuf2[nPos++] = szBuf[i];
			}
		}
	}

	if(Sort[(int)lp3] == SORT_UP){
		if(lp3 == COLUMN_PRICE || lp3 == COLUMN_NUMBER){//数字比較
			nValue1 = wcstoul(szBuf1,NULL,10);
			nValue2 = wcstoul(szBuf2,NULL,10);
			if(nValue1 == nValue2)return 0;
			else if(nValue1 < nValue2)return -1;
			else return 1;
		}
		else{
			return wcscmp(szBuf1,szBuf2);
		}
	}
	else{
		if(lp3 == COLUMN_PRICE || lp3 == COLUMN_NUMBER){//数字比較
			nValue1 = wcstoul(szBuf1,NULL,10);
			nValue2 = wcstoul(szBuf2,NULL,10);
			if(nValue1 == nValue2)return 0;
			else if(nValue1 < nValue2)return 1;
			else return -1;
		}
		else{
			return wcscmp(szBuf2,szBuf1);
		}
	}
}


//対象PC名を削除 戻り値は消した行数
int ListDeletePC(HWND hWnd,_TCHAR *szPCName)
{
	int nCount;
	HWND hList;
	_TCHAR szBuf[MAX_PATH];
	int nRet=0;

	hList = GetDlgItem(hWnd,IDC_LIST);
	nCount = ListView_GetItemCount(hList);
	if(nCount < 1){
		return 0;
	}

	for(int i=nCount-1;i>=0;i--){
		ListView_GetItemText(hList,i,COLUMN_PC,szBuf,sizeof(szBuf)/sizeof(_TCHAR));
		if(wcscmp(szPCName,szBuf) == 0){
			ListView_DeleteItem(hList,i);
			nRet++;
		}
	}

	return nRet;
}


//ファイルからリストに書込む
int ListReadFileCSV(HWND hWnd)
{
	_TCHAR szFileName[MAX_PATH];
	_TCHAR szDirSave[MAX_PATH];
	_TCHAR szCurrentDir[MAX_PATH];
	_TCHAR szPC[MAX_PATH];
	_TCHAR szName[MAX_PATH];
	_TCHAR szPrice[MAX_PATH];
	_TCHAR szNumber[MAX_PATH];
	_TCHAR szNote[MAX_PATH];
	_TCHAR szBuf[MAX_PATH];
	_TCHAR *p;
	char szcBuf[MAX_PATH];
	const _TCHAR szFilter[] = _T("CSV(*.csv)\0*.csv\0All (*.*)\0*.*\0\0");
	HANDLE hFile;
	DWORD dwReadNumber;
	DWORD dwReadCount;
	LVITEM item;
	int nItemCount;
	int nRow;
	int nBazaarPrice;
	
	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(szCurrentDir)/sizeof(_TCHAR),szCurrentDir);

	//0.04で追加
	if(wcslen(g_DirSave)){
		_swprintf(szDirSave,_T("%s\\%s"),szCurrentDir,g_DirSave);
	}
	else{
		_swprintf(szDirSave,_T("%s"),szCurrentDir);
	}

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ZeroMemory(szFileName,sizeof(szFileName));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szDirSave;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName)/sizeof(_TCHAR)-1;
	//ofn.Flags = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
	ofn.Flags = OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR|OFN_HIDEREADONLY;//0.08で変更
	ofn.lpstrDefExt = _T("csv");

	if(GetOpenFileName(&ofn) == 0){//失敗時
		if(CommDlgExtendedError() != 0 ){//0の時はキャンセル
			SetCurrentDirectory(szCurrentDir);
			return -1;
		}
	}
	else{//成功時
		hFile = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == NULL){
			SetCurrentDirectory(szCurrentDir);
			MessageBox(hWnd,_T("読込みに失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			return -1;
		}

		nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,IDC_LIST));
		nRow = nItemCount;
		dwReadCount = 0;
		do{
			ZeroMemory(szcBuf,sizeof(szcBuf));
			if(ReadFile(hFile,szcBuf,MAX_PATH-1,&dwReadNumber,NULL) == 0){
				break;
			}
			if(strlen(szcBuf) == 0)break;
			for(int i=0;i<MAX_PATH;i++){
				if(szcBuf[i] == '\r' && szcBuf[i+1] == '\n'){
					szcBuf[i] = '\0';
					szcBuf[i+1] = '\0';
					dwReadCount += i+2;
					break;
				}
			}

			if(mbstowcs(szBuf,szcBuf,MAX_PATH-1) == -1){
				MessageBeep(1);
			}
			
			ReplaceString(szBuf,'|','\r');
			for(int i=wcslen(szBuf);i>=0;i--){
				if(szBuf[i] == ',')szBuf[i] = '\0';
			}

			p = szBuf;
			wcscpy(szPC,p);
			p += wcslen(szPC)+1;
			wcscpy(szName,p);
			p += wcslen(szName)+1;
			wcscpy(szPrice,p);
			p += wcslen(szPrice)+1;
			wcscpy(szNumber,p);
			p += wcslen(szNumber)+1;
			wcscpy(szNote,p);

			SetFilePointer(hFile,dwReadCount,NULL,FILE_BEGIN);

			//既にリストに記載のあるPCは追加しない
			if(nItemCount){
				for(int i=nItemCount-1;i>=0;i--){
					ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_PC,szBuf,sizeof(szBuf)/sizeof(_TCHAR));
					if(wcscmp(szPC,szBuf) == 0){
						break;
					}
				}
				if(wcscmp(szPC,szBuf) == 0){
					continue;
				}
			}
		
			item.mask = LVIF_TEXT|LVIF_PARAM;//LVIF_PARAM追加でソートオン
			item.iItem = nRow;//行
			item.iSubItem = COLUMN_PC;//列
			item.pszText = szPC;
			item.lParam = (LPARAM)nRow;//ソート用lParamを設定
			if(-1 == ListView_InsertItem(GetDlgItem(hWnd,IDC_LIST),&item)){
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = COLUMN_NAME;//列
			item.pszText = szName;
			if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST),&item)){
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = COLUMN_PRICE;//列
			nBazaarPrice = wcstoul(szPrice,NULL,10);
			if(nBazaarPrice >= 1000000){
				_swprintf(szPrice,_T("%d,%03d,%03d"),nBazaarPrice/1000/1000,nBazaarPrice/1000%1000,nBazaarPrice%1000);
			}
			else if(nBazaarPrice >= 1000){
				_swprintf(szPrice,_T("%d,%03d"),nBazaarPrice/1000%1000,nBazaarPrice%1000);
			}
			else{
				_swprintf(szPrice,_T("%d"),nBazaarPrice);
			}
			item.pszText = szPrice;
			if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST),&item)){
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = COLUMN_NUMBER;//列
			item.pszText = szNumber;
			if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST),&item)){
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = COLUMN_NOTE;//列
			item.pszText = szNote;
			if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST),&item)){
			}


			nRow++;
		}while(dwReadNumber>3);

		CloseHandle(hFile);
	}

	//カレントフォルダを設定
	SetCurrentDirectory(szCurrentDir);

	return 1;
}


//ファイルにリストを書込む
int ListWriteFileCSV(HWND hWnd,int nID)
{
	_TCHAR szFileName[MAX_PATH];
	_TCHAR szCurrentDir[MAX_PATH];
	_TCHAR szBuf_PC[MAX_PATH];
	_TCHAR szBuf_Name[MAX_PATH];
	_TCHAR szBuf_Price[MAX_PATH];
	_TCHAR szBuf_Number[MAX_PATH];
	_TCHAR szBuf_Note[MAX_PATH];
	_TCHAR szText[MAX_PATH];
	_TCHAR szBuf[MAX_PATH];
	char szcWriteText[MAX_PATH];
	const _TCHAR szFilter[] = _T("CSV(*.csv)\0*.csv\0All (*.*)\0*.*\0\0");
	HANDLE hFile;
	DWORD dwWriteNumber;
	int nItemCount;
	SYSTEMTIME sysTime;
	
	//現在の時間からファイル名を作成
	GetLocalTime(&sysTime);
	if(wcslen(g_DirSave)){
		_swprintf(szFileName,_T("%s\\%d-%02d-%02dT%02d%02d.csv"),g_DirSave,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);
	}
	else{
		_swprintf(szFileName,_T("%d-%02d-%02dT%02d%02d.csv"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);
	}

	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(szCurrentDir)/sizeof(_TCHAR),szCurrentDir);

	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,nID));

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szCurrentDir;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName)/sizeof(_TCHAR)-1;
	ofn.Flags = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = _T("csv");

	if(GetSaveFileName(&ofn) == 0){//失敗時
		if(CommDlgExtendedError() != 0 ){//0の時はキャンセル
			MessageBox(hWnd,_T("保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			SetCurrentDirectory(szCurrentDir);
			return -1;
		}
	}
	else{//成功時
		//カレントフォルダを設定
		SetCurrentDirectory(szCurrentDir);

		hFile = CreateFile(szFileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == NULL){
			MessageBox(hWnd,_T("保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			return -1;
		}

		for(int i=0;i<nItemCount;i++){
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_PC,szBuf_PC,sizeof(szBuf_PC)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NAME,szBuf_Name,sizeof(szBuf_Name)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_PRICE,szBuf_Price,sizeof(szBuf_Price)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NUMBER,szBuf_Number,sizeof(szBuf_Number)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NOTE,szBuf_Note,sizeof(szBuf_Note)/sizeof(_TCHAR)-1);
			ReplaceString(szBuf_PC,'\n','|');
			ReplaceString(szBuf_Name,'\n','|');
			ReplaceString(szBuf_Price,'\n','|');
			ReplaceString(szBuf_Number,'\n','|');
			ReplaceString(szBuf_Note,'\n','|');
			wcscpy(szBuf,szBuf_Price);
			ZeroMemory(szBuf_Price,sizeof(szBuf_Price));
			for(int j=0,nPos=0;szBuf[j];j++){
				if(szBuf[j] >= '0' && szBuf[j] <= '9'){
					szBuf_Price[nPos++] = szBuf[j];
				}
			}
			_swprintf(szText,_T("%s,%s,%s,%s,%s\r\n"),szBuf_PC,szBuf_Name,szBuf_Price,szBuf_Number,szBuf_Note);
			wcstombs(szcWriteText,szText,sizeof(szcWriteText)-1);
			WriteFile(hFile,szcWriteText,strlen(szcWriteText),&dwWriteNumber,NULL);
		}

		CloseHandle(hFile);
	}

	return 1;
}


//ファイルにリストを書込む
int ListWriteFileHTML(HWND hWnd,int nID)
{
	_TCHAR szFileName[MAX_PATH];
	_TCHAR szCurrentDir[MAX_PATH];
	_TCHAR szBuf_PC[MAX_PATH];
	_TCHAR szBuf_Name[MAX_PATH];
	_TCHAR szBuf_Price[MAX_PATH];
	_TCHAR szBuf_Number[MAX_PATH];
	_TCHAR szBuf_Note[MAX_PATH];
	_TCHAR szText[MAX_PATH];
	char szcWriteText[MAX_PATH];
	const _TCHAR szFilter[] = _T("HTML(*.html)\0*.html\0All (*.*)\0*.*\0\0");
	HANDLE hFile;
	DWORD dwWriteNumber;
	int nItemCount;
	SYSTEMTIME sysTime;
	
	//現在の時間からファイル名を作成
	GetLocalTime(&sysTime);
	if(wcslen(g_DirSave)){
		_swprintf(szFileName,_T("%s\\%d-%02d-%02dT%02d%02d.html"),g_DirSave,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);
	}
	else{
		_swprintf(szFileName,_T("%d-%02d-%02dT%02d%02d.html"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);
	}
	
	//カレントフォルダを取得
	GetCurrentDirectory(sizeof(szCurrentDir)/sizeof(_TCHAR),szCurrentDir);

	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,nID));

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = szCurrentDir;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName)/sizeof(_TCHAR)-1;
	ofn.Flags = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = _T("html");

	if(GetSaveFileName(&ofn) == 0){//失敗時
		if(CommDlgExtendedError() != 0 ){//0の時はキャンセル
			MessageBox(hWnd,_T("保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			SetCurrentDirectory(szCurrentDir);
			return -1;
		}
	}
	else{//成功時
		//カレントフォルダを設定
		SetCurrentDirectory(szCurrentDir);

		hFile = CreateFile(szFileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == NULL){
			MessageBox(hWnd,_T("保存に失敗"),BAZAAR_STRING,MB_OK|MB_ICONERROR);
			//SetCurrentDirectory(szCurrentDir);
			return -1;
		}

		sprintf(szcWriteText,"<html>\r\n<head><title>バザー</title></head>\r\n<body bgcolor=\"#4169e1\" text=\"#ffffff\">\r\n");
		WriteFile(hFile,szcWriteText,strlen(szcWriteText),&dwWriteNumber,NULL);
		sprintf(szcWriteText,"<table border=\"1\">\r\n\t<tr><th>名前</th><th>品名</th><th>価格</th><th>個数</th><th>備考</th></tr>\r\n");
		WriteFile(hFile,szcWriteText,strlen(szcWriteText),&dwWriteNumber,NULL);

		for(int i=0;i<nItemCount;i++){
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_PC,szBuf_PC,sizeof(szBuf_PC)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NAME,szBuf_Name,sizeof(szBuf_Name)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_PRICE,szBuf_Price,sizeof(szBuf_Price)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NUMBER,szBuf_Number,sizeof(szBuf_Number)/sizeof(_TCHAR)-1);
			ListView_GetItemText(GetDlgItem(hWnd,nID),i,COLUMN_NOTE,szBuf_Note,sizeof(szBuf_Note)/sizeof(_TCHAR)-1);
			//_swprintf(szText,_T("\t<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>\r\n"),szBuf_PC,szBuf_Name,szBuf_Price,szBuf_Number,szBuf_Note);
			_swprintf(szText,_T("\t<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\r\n"),szBuf_PC,szBuf_Name,szBuf_Price,szBuf_Number,szBuf_Note);//0.07で修正
			wcstombs(szcWriteText,szText,sizeof(szcWriteText)-1);
			WriteFile(hFile,szcWriteText,strlen(szcWriteText),&dwWriteNumber,NULL);
		}
		sprintf(szcWriteText,"</table>\r\n</body>\r\n</html>");
		WriteFile(hFile,szcWriteText,strlen(szcWriteText),&dwWriteNumber,NULL);

		CloseHandle(hFile);
	}

	return 1;
}


//LISTVIEWコントロールのサブクラス化
LRESULT CALLBACK WndProcSubClass(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	if(msg == WM_CONTEXTMENU){//WM_RBUTTONUPよりこちらが優先
		//マウスのスクリーン座標をクライアント座標に変換
		POINT Point;
		Point.x = LOWORD(lp);
		Point.y = HIWORD(lp);
		ScreenToClient(GetParent(hWnd),&Point);
		lp = Point.x + ((Point.y)<<16);
		SendMessage(GetParent(hWnd),WM_RBUTTONUP,wp,lp);
		return 0;
	}
	else if(msg == WM_KEYDOWN){
		if(wp == VK_DELETE){
			;
		}
	}
	
	return CallWindowProc(WndProcMainClass,hWnd,msg,wp,lp);
}


//選択状態のキャラクターが可視範囲にいるかチェック
int ListSendOpenBazaar(HWND hWnd)
{
	int nSelected;
	_TCHAR szName[0x20];
	char szcName[0x20];
	char szcCmd[0x30];
	INFORMATION pcInfo;
	int nRet=0;
	HWND hUseList;

	if(g_ListUp == 0){
		hUseList = GetDlgItem(hWnd,IDC_LIST);
	}
	else{
		hUseList = GetDlgItem(hWnd,IDC_LIST_LISTUP);
	}
	
	nSelected = ListView_GetNextItem(hUseList,-1,LVNI_SELECTED);
	if(nSelected == -1)return -1;

	ListView_GetItemText(hUseList,nSelected,COLUMN_PC,szName,sizeof(szName)/sizeof(_TCHAR));

	for(int i=0x400;i<0x700;i++){
		pcInfo = GetNPCInfo(i);
		if(pcInfo.inrange && wcscmp(szName,pcInfo.name) == 0){
			wcstombs(szcName,szName,sizeof(szcName));
			sprintf(szcCmd,"/c %s",szcName);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			Sleep(g_KeyDelay);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			Sleep(g_KeyDelay);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,true);
			DLL_CKHSetKey(pKeyboardHelper,CKH_ESCAPE,false);
			Sleep(g_KeyDelay);
			DLL_CKHSendString(pKeyboardHelper,szcCmd);//「/c name」発行
			Sleep(g_WaitUpdate);
			DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,true);//Enter
			DLL_CKHSetKey(pKeyboardHelper,CKH_RETURN,false);

			nRet = 1;
			break;
		}
	}

	return nRet;
}


//選択対象をFFXIAHで検索
int ListOpenFFXIAH(HWND hWnd)
{
	_TCHAR szItemName[MAX_PATH];
	char szcUTF8[MAX_PATH];
	char szcUrl[MAX_PATH];
	char szcBuf[MAX_PATH];
	int nSelected;
	HWND hUseList;

	if(g_UseFFXIAH){
		wcstombs(szcUrl,g_FFXIAH,sizeof(g_FFXIAH)/sizeof(_TCHAR));
		if(strlen(szcUrl) == 0)return 0;
	}
	else return 0;

	if(g_ListUp == 0){
		hUseList = GetDlgItem(hWnd,IDC_LIST);
	}
	else{
		hUseList = GetDlgItem(hWnd,IDC_LIST_LISTUP);
	}

	nSelected = ListView_GetNextItem(hUseList,-1,LVNI_SELECTED);
	if(nSelected == -1)return -1;

	ListView_GetItemText(hUseList,nSelected,COLUMN_NAME,szItemName,sizeof(szItemName)/sizeof(_TCHAR));

	if(WideCharToMultiByte(CP_UTF8,NULL,szItemName,-1,szcUTF8,sizeof(szcUTF8),NULL,NULL)){//返り値は変換文字列数
		for(int i=0;szcUTF8[i];i++){
			memcpy(szcBuf,szcUrl,sizeof(szcUrl));
			sprintf(szcUrl,"%s%%%02X",szcBuf,(BYTE)szcUTF8[i]);
		}
		ShellExecuteA(NULL,"open",szcUrl,NULL,NULL,SW_SHOWDEFAULT);
	}
	else return 0;

	return 1;
}


//ソート用にシリアルなLVIF_LPARAMを再設定 0.02aで追加
int ListResetLparam(HWND hWnd)
{
	HWND hList;
	LVITEM item;
	int nRow;

	hList = GetDlgItem(hWnd,IDC_LIST);
	nRow = ListView_GetItemCount(hList);

	item.mask = LVIF_PARAM;//LVIF_PARAMでソートオン
	item.iSubItem = 0;//列
	for(int i=0;i<nRow;i++){
		item.iItem = i;//行
		item.lParam = (LPARAM)i;//ソート用lParamを設定
		if(FALSE == ListView_SetItem(hList,&item)){
			return 0;
		}
	}

	return 1;
}


//ヘッダー操作
//iSubItemに-1を指定するとヘッダーとソートをリセット
void ListSort(HWND hWnd,int iSubItem)
{
	//ヘッダーに△▽マークを表示
	HWND hHeader;
	HWND hHeaderListUp;

	hHeader = ListView_GetHeader(GetDlgItem(hWnd,IDC_LIST));
	hHeaderListUp = ListView_GetHeader(GetDlgItem(hWnd,IDC_LIST_LISTUP));
				
	//リセット
	HDITEM headerItem;
	for(int i=0;i<5;i++){
		headerItem.mask = HDI_FORMAT;
		Header_GetItem(hHeader,i,&headerItem);
		headerItem.fmt &= ~HDF_SORTDOWN & ~HDF_SORTUP;
		Header_SetItem(hHeader,i,&headerItem);
		Header_SetItem(hHeaderListUp,i,&headerItem);
	}

	if(iSubItem == -1){
		for(int i=0;i<COLUMN_MAX_COUNT;i++){
			Sort[i] = SORT_DOWN;
		}
		return;
	}
	else{
		for(int i=0;i<COLUMN_MAX_COUNT;i++){
			if(i != iSubItem)Sort[i] = SORT_DOWN;
		}
	}

	headerItem.mask = HDI_FORMAT;
	Header_GetItem(hHeader,iSubItem,&headerItem);
	headerItem.fmt &= ~HDF_SORTDOWN & ~HDF_SORTUP;

	//ソートする
	if(Sort[iSubItem] == SORT_UP){
		Sort[iSubItem] = SORT_DOWN;
		headerItem.fmt |= HDF_SORTDOWN | HDF_STRING;
	}
	else{
		Sort[iSubItem] = SORT_UP;
		headerItem.fmt |= HDF_SORTUP | HDF_STRING;
	}
	if(FALSE == ListView_SortItems(GetDlgItem(hWnd,IDC_LIST),CmpProc,iSubItem)){
	}
	Header_SetItem(hHeader,iSubItem,&headerItem);
	Header_SetItem(hHeaderListUp,iSubItem,&headerItem);
}


//抽出
int ListMakeListUp(HWND hWnd)
{
	LVITEM item;
	int nItemCount;
	int nRow;
	_TCHAR szName[0x30];
	_TCHAR szPC[0x20];
	_TCHAR szPrice[0x10];
	_TCHAR szNumber[0x10];
	_TCHAR szNote[0x100];

	ListView_DeleteAllItems(GetDlgItem(hWnd,IDC_LIST_LISTUP));
	nItemCount = ListView_GetItemCount(GetDlgItem(hWnd,IDC_LIST));

	if(nItemCount == 0)return 0;
	
	nRow = 0;
	for(int i=0;i<nItemCount;i++){
		ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_NAME,szName,sizeof(szName)/sizeof(_TCHAR));
		for(int j=0;j<100;j++){
			if(g_Wanted[j].use && wcscmp(szName,g_Wanted[j].szName) == 0){
				ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_PC,szPC,sizeof(szPC)/sizeof(_TCHAR));
				ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_PRICE,szPrice,sizeof(szPrice)/sizeof(_TCHAR));
				ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_NUMBER,szNumber,sizeof(szNumber)/sizeof(_TCHAR));
				ListView_GetItemText(GetDlgItem(hWnd,IDC_LIST),i,COLUMN_NOTE,szNote,sizeof(szNote)/sizeof(_TCHAR));
			
				item.mask = LVIF_TEXT|LVIF_PARAM;//LVIF_PARAM追加でソートオン
				item.iItem = nRow;//行
				item.iSubItem = COLUMN_PC;//列
				item.pszText = szPC;
				item.lParam = (LPARAM)nRow;
				if(-1 == ListView_InsertItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),&item)){
					return -1;
				}

				item.mask = LVIF_TEXT;
				item.iSubItem = COLUMN_NAME;//列
				item.pszText = szName;
				if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),&item)){
					return -1;
				}

				item.mask = LVIF_TEXT;
				item.iSubItem = COLUMN_PRICE;//列
				item.pszText = szPrice;
				if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),&item)){
					return -1;
				}

				item.mask = LVIF_TEXT;
				item.iSubItem = COLUMN_NUMBER;//列
				item.pszText = szNumber;
				if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),&item)){
					return -1;
				}

				item.mask = LVIF_TEXT;
				item.iSubItem = COLUMN_NOTE;//列
				item.pszText = szNote;
				if(FALSE == ListView_SetItem(GetDlgItem(hWnd,IDC_LIST_LISTUP),&item)){
					return -1;
				}
				nRow++;

				break;
			}
		}
	}


	return nRow;
}


//ヘッダーの幅を変更
int ListChangeHeaderWidth(HWND hWnd,int nColumn,int nWidth)
{
	HDITEM hdi;
	ZeroMemory(&hdi,sizeof(HDITEM));
	hdi.mask = HDI_WIDTH;
	hdi.cxy = nWidth;
	Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd,IDC_LIST)),nColumn,&hdi);
	Header_SetItem(ListView_GetHeader(GetDlgItem(hWnd,IDC_LIST_LISTUP)),nColumn,&hdi);

	return 1;
}

//ヘッダーの幅を取得 0.05で追加
//hDlgWndはコントロールのハンドルなので注意
int ListGetHeaderWidth(HWND hDlgWnd,int nColumn)
{	
	HDITEM hdi;
	ZeroMemory(&hdi,sizeof(HDITEM));
	hdi.mask = HDI_WIDTH;
	Header_GetItem(hDlgWnd,nColumn,&hdi);

	return hdi.cxy;
}


//FF内改行コード\rを置換
void ReplaceString(_TCHAR *szText,_TCHAR tA,_TCHAR tB)
{
	while(*szText){
		if(*szText == tA){
			*szText = tB;
		}
		szText++;
	}
}