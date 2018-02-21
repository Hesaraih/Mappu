#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <olectl.h>
#include <commctrl.h>
#include <shlwapi.h>
#include "header.h"
#include "main.h"
#include "resource.h"
#include "draw.h"
#include "other.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"msimg32.lib")


#define DRAW_EDGE(hDC,nX,nZ,szShow)	TextOut(hDC,nX+1,nZ-1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX+1,nZ+0,szShow,_tcslen(szShow));\
					TextOut(hDC,nX+1,nZ+1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX+0,nZ-1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX+0,nZ+1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX-1,nZ-1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX-1,nZ+1,szShow,_tcslen(szShow));\
					TextOut(hDC,nX-1,nZ+0,szShow,_tcslen(szShow));

static POSXYZR g_OwnPos[300];//0.55で追加 0.55aで要素数増

/// <summary>
/// 描画関連の雑記
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hLIST"></param>
/// <param name="hCHARACTER"></param>
/// <returns></returns>
int Draw(HWND hWnd, HWND hLIST, HWND hCHARACTER)
{
	int nRet;
	MAP_INFORMATION mapInfo;
	INFORMATION mypos;

	//自分の情報を取得
	mypos = GetMyInfo();

	//マップの情報を取得
	mapInfo = GetMAPINFORMATION(mypos);

#ifndef DEBUG
	//WindowTextを更新 0.50で追加
	_TCHAR szText[0x20], szCmp[0x20];
	GetWindowText(hWnd, szCmp, sizeof(szCmp) / sizeof(_TCHAR) - 1);
	_stprintf(szText, _T("Mappu - %s"), mypos.name);
	if (_tcscmp(szText, szCmp) != 0 && _tcslen(mypos.name) != 0)SetWindowText(hWnd, szText);
#endif

	//マップ表示(Mapウィンドウ)
	if ((nRet = DrawMap(hWnd, mypos, &mapInfo)) != 0) {
		return nRet;//nRetは5～40
	}

	//キャラクター表示(キャラクターウィンドウ)
	if ((nRet = DrawCharacter(hWnd, hLIST, hCHARACTER, mypos, mapInfo)) != 0) {
		return nRet;//nRetは41～69
	}

	//天候アイコンを表示
	if ((nRet = DrawWeatherIcon(hWnd)) != 0) {
		return nRet;//nRetは80～
	}

	return 0;
}

/// <summary>
/// マップを描画
/// </summary>
/// <param name="hWnd"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <returns>正常終了:0</returns>
int DrawMap(HWND hWnd, INFORMATION mypos, MAP_INFORMATION *mapInfo)
{
	HDC hDC;
	HDC hMemDC;
	HBITMAP hBitmap;
	_TCHAR szBuf[MAX_PATH];
	static IPicture *pPic = NULL;
	static MAP_INFORMATION cmp_mapInfo;

	//ウィンドウの大きさを取得
	RECT rect;
	if (0 == GetWindowRect(hWnd, &rect))return 5;

	//マップ表示(MAPウィンドウ)
	//再描画(false=背景消去しない)
	if (0 == InvalidateRect(hWnd, NULL, false))return 6;

	//ダブルバッファ
	hDC = GetDC(hWnd);//GetDC→ReleaseDC
	if (hDC == NULL)return 7;
	if ((hMemDC = CreateCompatibleDC(hDC)) == NULL)return 8;//CreateCompaticleDC→DeleteDC

															// 大きいメモリデバイスコンテキストを作る（べつに小さくてもOK）
	BITMAPINFO bmpinfo;
	ZeroMemory(&bmpinfo, sizeof(bmpinfo));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = rect.right - rect.left;
	bmpinfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;//32ビットカラー
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	if ((hBitmap = CreateDIBSection(NULL, &bmpinfo, DIB_RGB_COLORS, NULL, NULL, NULL)) == NULL)return 9;
	// 今作ったビットマップを、メモリデバイスコンテキストに選択する
	if (NULL == SelectObject(hMemDC, hBitmap))return 10;
	if (0 == DeleteObject(hBitmap))return 11;
	if (0 == ReleaseDC(hWnd, hDC))return 12;

	//フォント
	HFONT hOldFont;
	hOldFont = (HFONT)SelectObject(hMemDC, g_FontSystem.hFont);

	//クライアントの大きさを取得
	if (0 == GetClientRect(hWnd, &rect))return 15;

	//透過色で埋める
	if (g_Menu.transparent) {
		HBRUSH hBrush;
		hBrush = CreateSolidBrush(g_ColorTransparent);
		FillRect(hMemDC, &rect, hBrush);
		if (0 == DeleteObject(hBrush)) {
			return 16;
		}
	}
	else {
		HBRUSH hBrush;
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hMemDC, &rect, hBrush);
		if (0 == DeleteObject(hBrush)) {
			return 17;
		}
	}

	//文字背景透過
	if (0 == SetBkMode(hMemDC, TRANSPARENT))return 20;

	*mapInfo = GetMAPINFORMATION(mypos);//gif表示

	if (!pPic && (cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID)) {//読み込みしていない時に初期化 0.54で変更
		ZeroMemory(&cmp_mapInfo, sizeof(cmp_mapInfo));
	}

	if (mapInfo->code == 0) {//map.ini読み込みが正常に終了した場合
		if (cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID) {//読み込み処理が重いので同じマップの時は再利用
			if (pPic) {
				pPic->Release();
			}
			//pPic = LoadMAP(*mapInfo);
			pPic = LoadMAP(*mapInfo, _T("gif"));
			//if (!pPic)pPic = LoadMAP(*mapInfo, _T("png"));
			if (!pPic)pPic = LoadMAP(*mapInfo, _T("bmp"));

			//DefaultZoomSetting
			if (cmp_mapInfo.AreaID != mapInfo->AreaID && g_Menu.default_zoomset) {
				if (mapInfo->zoom == (float)0.1) { g_Zoom = g_Zoom_01; }
				else if (mapInfo->zoom == (float)0.2) { g_Zoom = g_Zoom_02; }
				else if (mapInfo->zoom == (float)0.4) { g_Zoom = g_Zoom_04; }
			}
			float fZoomIni = GetZoomIni();//0.54で追加
			if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54で追加
		}

		if (pPic) {
			RenderMAP(hMemDC, pPic, mypos, *mapInfo, rect, g_Zoom);
		}
		else {
			if (mapInfo->AreaID >= 0x0100) {//2Byteエリア 0.53で追加
				_stprintf(szBuf, _T("No MAP!Area = %x"), mapInfo->AreaID);
			}
			else {
				_stprintf(szBuf, _T("No MAP!Area = %02x"), mapInfo->AreaID);
			}
			SetTextColor(hMemDC, g_ColorMapName);
			DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
			mapInfo->zoom = (float)0.4;

			//if(cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID){//0.52で変更
			if (cmp_mapInfo.AreaID != mapInfo->AreaID) {//0.54で変更
				if (g_Menu.default_zoomset) {//DefaultZoomSetting
					g_Zoom = g_Zoom_04;
				}
				float fZoomIni = GetZoomIni();//0.54で追加
				if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54で追加
			}
		}
	}
	else if (mapInfo->code == 1) {
		//map.iniが見つからなかった時
		_stprintf(szBuf, _T("map.iniが見つかりません。"));
		SetTextColor(hMemDC, g_ColorMapName);
		DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);//0.47aで変更
		mapInfo->zoom = (float)0.4;
		if (g_Menu.default_zoomset) {//DefaultZoomSetting
			g_Zoom = g_Zoom_04;
		}
		float fZoomIni = GetZoomIni();//0.54で追加
		if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54で追加
	}
	else {//map.ini読み込み時のその他エラー
		if (mapInfo->AreaID >= 0x0100) {
			_stprintf(szBuf, _T("map.ini読み込みエラー。 code = %d,Area = %x"), mapInfo->code, mapInfo->AreaID);
		}
		else {
			_stprintf(szBuf, _T("map.ini読み込みエラー。 code = %d,Area = %02x"), mapInfo->code, mapInfo->AreaID);
		}
		SetTextColor(hMemDC, g_ColorMapName);
		DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);//0.47aで変更
		mapInfo->zoom = (float)0.4;
		if (g_Menu.default_zoomset) {//DefaultZoomSetting
			g_Zoom = g_Zoom_04;
		}
		float fZoomIni = GetZoomIni();//0.54で追加
		if (fZoomIni >= (float)0.5) {
			g_Zoom = fZoomIni;
		}//0.54で追加
	}

	//情報を表示
	if (DrawInfo(&hMemDC, mypos, *mapInfo, rect) <= 0) {
		return 25;
	}

	//比較用にコピー
	cmp_mapInfo = *mapInfo;


	SelectObject(hMemDC, hOldFont);

	//描画
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);

	if (hDC == NULL) {
		return 30;
	}
	if (0 == BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY)) {//CAPTUREBLTで透過率もコピー
		return 31;
	}

	EndPaint(hWnd, &ps);

	if (0 == DeleteDC(hMemDC)) {
		return 32;
	}

	return 0;
}

/// <summary>
/// キャラクターウィンドウに描画
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hLIST"></param>
/// <param name="hCHARACTER"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <returns>正常終了:0</returns>
int DrawCharacter(HWND hWnd, HWND hLIST, HWND hCHARACTER, INFORMATION mypos, MAP_INFORMATION mapInfo)
{
	HDC hDC, hMemDC;
	HBITMAP hBitmap;
	INFORMATION targetInfo;
	int nCenter_X, nCenter_Z;
	RECT rect;

	targetInfo = GetTargetInfo();

	//ウィンドウの大きさを取得
	if (0 == GetWindowRect(hWnd, &rect))return 40;

	//再描画(false=背景消去しない)
	if (0 == InvalidateRect(hCHARACTER, NULL, false))return 41;

	//ダブルバッファ
	hDC = GetDC(hCHARACTER);
	if (hDC == NULL)return 42;
	if ((hMemDC = CreateCompatibleDC(hDC)) == NULL)return 43;
	// 大きいメモリデバイスコンテキストを作る（べつに小さくてもOK）
	BITMAPINFO bmpinfo;
	ZeroMemory(&bmpinfo, sizeof(bmpinfo));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = rect.right - rect.left;
	bmpinfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;//32ビットカラー
	bmpinfo.bmiHeader.biCompression = BI_RGB;

	if ((hBitmap = CreateDIBSection(NULL, &bmpinfo, DIB_RGB_COLORS, NULL, NULL, NULL)) == NULL) {
		return 44;
	}
	// 今作ったビットマップを、メモリデバイスコンテキストに選択する
	if (NULL == SelectObject(hMemDC, hBitmap))return 45;
	if (0 == DeleteObject(hBitmap))return 46;
	if (0 == ReleaseDC(hWnd, hDC))return 47;

	//クライアントの大きさを取得
	if (0 == GetClientRect(hWnd, &rect))return 48;
	//ウィンドウの中心座標
	nCenter_X = rect.right / 2;
	nCenter_Z = rect.bottom / 2;

	//文字背景透過
	if (0 == SetBkMode(hMemDC, TRANSPARENT))return 50;

	//可視範囲円を描画
	if (DrawCircle(&hMemDC, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 55;
	}

	//自分の過去の位置を表示 0.55で追加
	if (DrawPastOwn(&hMemDC, mypos, mapInfo, nCenter_X, nCenter_Z) <= 0) {
		return 58;
	}

	//自分の位置を表示
	if (DrawOwn(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) <= 0) {
		return 60;
	}

	//パーティメンバーとPCの位置を表示
	if (DrawPC(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 61;
	}

	//PETの位置を表示
	if (DrawPET(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 62;
	}

	//NPCの位置を表示
	if (DrawNPC(&hMemDC, hLIST, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 63;
	}

	//Targetの位置を表示
	if (DrawTarget(&hMemDC, hLIST, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 64;
	}

	//座標計算
	int x, z;
	if (g_Menu.center_own == 1) {
		//x = 0;
		//z = 0;
		x = -g_CenterMAP_X;//0.52で変更
		z = -g_CenterMAP_Z;//0.52で変更
	}
	else {
		x = (int)(nCenter_X - mapInfo.x * g_Zoom - mypos.x * g_Zoom * 2 * mapInfo.zoom) - g_CenterMAP_X;
		z = (int)(nCenter_Z - mapInfo.z * g_Zoom + mypos.z * g_Zoom * 2 * mapInfo.zoom) - g_CenterMAP_Z;
	}

	//GDI関数による描画を開始する
	PAINTSTRUCT ps;
	hDC = BeginPaint(hCHARACTER, &ps);
	if (hDC == NULL) {
		return 65;
	}
	//有効範囲を透過色の黒で塗りつぶす
	//if(g_Menu.center_own == 0 && 0 == PatBlt(hDC,0,0,rect.right,rect.bottom,BLACKNESS)){
	if (0 == PatBlt(hDC, 0, 0, rect.right, rect.bottom, BLACKNESS)) {//0.52で変更
		return 66;
	}
	if (0 == BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, x, z, SRCCOPY)) {//CAPTUREBLTで透過率もコピー
		return 67;
	}

	//GDI関数による描画を終了する
	EndPaint(hWnd, &ps);

	if (0 == DeleteDC(hMemDC)) {
		return 68;
	}

	return 0;
}

/// <summary>
/// キャラクターウィンドウに天候を描画
/// </summary>
/// <param name="hWnd"></param>
/// <returns>正常終了：0</returns>
int DrawWeatherIcon(HWND hWnd)
{
	static BYTE cmp_PastWeather = 0;
	static int nFlagIcon = 0;//現在の天候アイコン
	BYTE byteWeather;
	HICON hIcon;
	const _TCHAR szWeather[][8] = {
		_T("fine"),_T("fine"),			//快晴・晴れ
		_T("clouds"),_T("fog"),			//くもり・霧
		_T("fire"),_T("fire"),			//熱波・灼熱波
		_T("water"),_T("water"),		//雨・スコール
		_T("earth"),_T("earth"),		//砂塵・砂嵐
		_T("wind"),_T("wind"),			//風・暴風
		_T("ice"),_T("ice"),			//雪・吹雪
		_T("thunder"),_T("thunder"),	//雷・雷雨
		_T("light"),_T("light"),		//オーロラ・神光
		_T("dark"),_T("dark"),			//妖霧・闇
	};
	_TCHAR szIconName[0x20];

	byteWeather = GetWeather();
	if (g_Menu.weather_icon && byteWeather != nFlagIcon && byteWeather > 3) {//0.48で変更 0.50で変更
		_stprintf(szIconName, _T(".\\weather\\%s.ico"), szWeather[byteWeather]);
		hIcon = (HICON)LoadImage(0, szIconName, IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_SHARED);
		if (hIcon) {
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			nFlagIcon = byteWeather;
		}
		else {
			hIcon = (HICON)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			nFlagIcon = 0;
		}
	}
	else if (g_Menu.weather_icon == 0 && nFlagIcon >= 3) {
		hIcon = (HICON)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		nFlagIcon = 0;
	}

	if (g_Menu.weather_sound && cmp_PastWeather != byteWeather && ((cmp_PastWeather <= 3 && byteWeather > 3) || cmp_PastWeather > 3)) {
		if (FALSE == PlaySound(g_WeatherWaveFileName, NULL, SND_FILENAME | SND_ASYNC)) {
			return -1;
		}
	}

	cmp_PastWeather = byteWeather;

	return 0;
}

/// <summary>
/// 各種サークルを描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawCircle(HDC *hMemDC, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hOldBrush;
	HPEN hPen, hOldPen;

	//可視範囲円を描画
	int nCurcle;
	if (g_Menu.circle_visible) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorVisible);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)((float)50 * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}
	//アビ範囲
	if (g_Menu.circle_ability) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorAbility);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)(MAX_DISTANCE_ABL * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}
	//魔法範囲
	if (g_Menu.circle_magic) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorMagic);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)(MAX_DISTANCE_MGC * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}
	//遠隔攻撃範囲
	if (g_Menu.circle_range) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorRange);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)(MAX_DISTANCE_RNG * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}
	//管楽器強化歌範囲
	if (g_Menu.circle_windsong) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorWindSong);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)(MAX_DISTANCE_WINDSONG * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}
	//魔法感知範囲
	if (g_Menu.circle_detection) {
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorDetection);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
		nCurcle = (int)(MAX_DISTANCE_DETECTION * g_Zoom * 2 * mapInfo.zoom);
		Ellipse(*hMemDC, nCenter_X - nCurcle, nCenter_Z - nCurcle, nCenter_X + nCurcle, nCenter_Z + nCurcle);
		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
	}

	return 0;
}

/// <summary>
/// 方向付き位置を描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="fPointSize"></param>
/// <param name="fRadian"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <param name="nX"></param>
/// <param name="nZ"></param>
/// <returns></returns>
/// <remarks>0.48で追加</remarks>
int DrawPoint(HDC *hMemDC, float fPointSize, float fRadian, int nCenter_X, int nCenter_Z, int nX, int nZ)
{
	POINT point[3];
	point[0].x = nCenter_X + nX + (long)(-fPointSize * cos(fRadian) - fPointSize * sin(fRadian));
	point[0].y = nCenter_Z - nZ + (long)(-fPointSize * sin(fRadian) + fPointSize * cos(fRadian));
	point[1].x = nCenter_X + nX + (long)(-fPointSize * cos(fRadian) + fPointSize * sin(fRadian));
	point[1].y = nCenter_Z - nZ + (long)(-fPointSize * sin(fRadian) - fPointSize * cos(fRadian));
	point[2].x = nCenter_X + nX + (long)(2 * fPointSize*cos(fRadian) - 0 * fPointSize*sin(fRadian));
	point[2].y = nCenter_Z - nZ + (long)(2 * fPointSize*sin(fRadian) + 0 * fPointSize*cos(fRadian));
	Polygon(*hMemDC, point, sizeof(point) / sizeof(point[0]));

	return 0;
}

/// <summary>
/// 過去の自分を描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
/// <remarks>0.55で追加</remarks>
int DrawPastOwn(HDC *hMemDC, INFORMATION mypos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	static WORD stwAreaID = -1;
	static BYTE stbSubID = -1;

	if (g_Menu.own_trace_line == 0 && g_Menu.own_trace_point == 0) {
		InitOwnPos();
		stwAreaID = -1;
		stbSubID = -1;
		return 10;
	}

	if (stwAreaID != mapInfo.AreaID || stbSubID != mapInfo.SubID) {
		InitOwnPos();
	}
	stwAreaID = mapInfo.AreaID;//0.55aで移動
	stbSubID = mapInfo.SubID;
	SaveOwnPos(mypos);

	hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorOWN);
	hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
	hBrush = CreateSolidBrush(g_ColorOWN);
	hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);

	int x, z;
	float fx, fz;
	//自分の位置
	if (g_ColorOWN) {
		MoveToEx(*hMemDC, nCenter_X, nCenter_Z, NULL);
		POSXYZR OwnPosMinus = LoadOwnPos(0);
		for (int i = 0; i<sizeof(g_OwnPos) / sizeof(POSXYZR); i++) {
			POSXYZR OwnPos = LoadOwnPos(i);
			if (OwnPos.x == -10000)break;
			else if (OwnPos.x == 0 && OwnPos.y == 0 && OwnPos.z == 0 && OwnPos.radian == 0)continue;//0.55aで追加

			if (!g_Menu.radar) {
				//int変換、距離補正
				x = (int)((float)(OwnPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(OwnPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int変換、距離補正
				fx = (float)(OwnPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(OwnPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//座標変換(回転)
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//前の位置との距離が50Ym以上の時線を切る 0.55aで追加
			if (sqrt(pow(OwnPos.x - OwnPosMinus.x, 2) + pow(OwnPos.z - OwnPosMinus.z, 2)) >= (float)50) {
				MoveToEx(*hMemDC, (int)(nCenter_X + x), (int)(nCenter_Z - z), NULL);
			}
			OwnPosMinus.x = OwnPos.x;
			OwnPosMinus.z = OwnPos.z;

			//描画
			if (!g_Menu.radar) {
				if (g_Menu.own_trace_point) {
					//Ellipse(*hMemDC,nCenter_X+x-g_PointSizeTRACE,nCenter_Z-z-g_PointSizeTRACE,nCenter_X+x+g_PointSizeTRACE,nCenter_Z-z+g_PointSizeTRACE);
					DrawPoint(hMemDC, g_PointSizeTRACE, OwnPos.radian, nCenter_X, nCenter_Z, x, z);
				}
				if (g_Menu.own_trace_line) {
					LineTo(*hMemDC, nCenter_X + x, nCenter_Z - z);
				}
			}
			else {
				if (g_Menu.own_trace_point) {
					//Ellipse(*hMemDC,nCenter_X-g_PointSizeTRACE,nCenter_Z-g_PointSizeTRACE,nCenter_X+g_PointSizeTRACE,nCenter_Z+g_PointSizeTRACE);
					DrawPoint(hMemDC, g_PointSizeTRACE, OwnPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);
				}
				if (g_Menu.own_trace_line) {
					LineTo(*hMemDC, nCenter_X + x, nCenter_Z - z);
				}
			}
		}
	}

	SelectObject(*hMemDC, hOldPen);
	SelectObject(*hMemDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	return 1;
}

/// <summary>
/// 自分を描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="targetpos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawOwn(HDC *hMemDC, INFORMATION mypos, INFORMATION targetpos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	int nCurcle;

	hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorOWN);
	hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
	hBrush = CreateSolidBrush(g_ColorOWN);
	hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);

	//自分の位置
	if (targetpos.id == mypos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
		if (g_ColorOWN) {//0.46で追加
						 //方向線を表示
			if (g_Menu.own_direction) {
				MoveToEx(*hMemDC, nCenter_X, nCenter_Z, NULL);
				nCurcle = (int)((float)500 * g_Zoom * 2 * mapInfo.zoom);
				if (!g_Menu.radar) {
					LineTo(*hMemDC, (int)(nCenter_X + nCurcle * cos(mypos.radian)), (int)(nCenter_Z + (int)nCurcle*sin(mypos.radian)));
				}
				else {
					LineTo(*hMemDC, (int)(nCenter_X + nCurcle * 0), (int)(nCenter_Z + (int)nCurcle*-1));
				}
			}
		}
	}
	else if (g_ColorOWN) {
		if (!g_Menu.radar) {
			DrawPoint(hMemDC, g_PointSizeOWN, mypos.radian, nCenter_X, nCenter_Z, 0, 0);//0.48で変更
		}
		else {
			POINT point[3];
			point[0].x = nCenter_X + (long)(-g_PointSizeOWN * 0 - g_PointSizeOWN * -1);
			point[0].y = nCenter_Z + (long)(-g_PointSizeOWN * -1 + g_PointSizeOWN * 0);
			point[1].x = nCenter_X + (long)(-g_PointSizeOWN * 0 + g_PointSizeOWN * -1);
			point[1].y = nCenter_Z + (long)(-g_PointSizeOWN * -1 - g_PointSizeOWN * 0);
			point[2].x = nCenter_X + (long)(2 * g_PointSizeOWN * 0 - 0 * g_PointSizeOWN*-1);
			point[2].y = nCenter_Z + (long)(2 * g_PointSizeOWN*-1 + 0 * g_PointSizeOWN * 0);
			Polygon(*hMemDC, point, sizeof(point) / sizeof(point[0]));
		}
		//方向線を表示
		if (g_Menu.own_direction) {
			MoveToEx(*hMemDC, nCenter_X, nCenter_Z, NULL);
			nCurcle = (int)((float)500 * g_Zoom * 2 * mapInfo.zoom);
			if (!g_Menu.radar) {
				LineTo(*hMemDC, (int)(nCenter_X + nCurcle * cos(mypos.radian)), (int)(nCenter_Z + (int)nCurcle*sin(mypos.radian)));
			}
			else {
				LineTo(*hMemDC, (int)(nCenter_X + nCurcle * 0), (int)(nCenter_Z + (int)nCurcle*-1));
			}
		}
	}

	SelectObject(*hMemDC, hOldPen);
	SelectObject(*hMemDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	return 1;
}

/// <summary>
/// 情報を描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <param name="WindowRect"></param>
/// <returns></returns>
int DrawInfo(HDC *hMemDC, INFORMATION mypos, MAP_INFORMATION mapInfo, RECT WindowRect)
{
	float fx, fz;
	_TCHAR szBuf[MAX_PATH];

	//WindowRectを一回り小さくする
	WindowRect.top += 1;
	WindowRect.bottom -= 1;
	WindowRect.left += 1;
	WindowRect.right -= 1;

	//DropShadow用のRect
	RECT Rect;
	Rect.top = WindowRect.top - g_FontSystem.nDS_z;
	Rect.bottom = WindowRect.bottom - g_FontSystem.nDS_z;;
	Rect.left = WindowRect.left + g_FontSystem.nDS_x;
	Rect.right = WindowRect.right + g_FontSystem.nDS_x;

	//Edge用のRect
	RECT RectEdge[8];
	char cEdge[8][2] = { { 1,1 },{ 1,0 },{ 1,-1 },{ 0,1 },{ 0,-1 },{ -1,1 },{ -1,0 },{ -1,-1 } };
	for (int i = 0; i<8; i++) {
		RectEdge[i].top = WindowRect.top + cEdge[i][0];
		RectEdge[i].bottom = WindowRect.bottom + cEdge[i][0];
		RectEdge[i].left = WindowRect.left + cEdge[i][1];
		RectEdge[i].right = WindowRect.right + cEdge[i][1];
	}

	//Area名表示
	if (_tcslen(mapInfo.AreaName) != 0 && g_ColorMapName) {
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, mapInfo.AreaName, -1, &Rect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, mapInfo.AreaName, -1, &RectEdge[i], DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		}
		SetTextColor(*hMemDC, g_ColorMapName);
		DrawText(*hMemDC, mapInfo.AreaName, -1, &WindowRect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
	}

	//倍率表示
	if (g_ColorZoom) {
		_stprintf(szBuf, _T("x%0.1f"), g_Zoom);
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_SINGLELINE | DT_TOP | DT_RIGHT);
		}
		SetTextColor(*hMemDC, g_ColorZoom);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
	}

	//Distance距離を表示
	INFORMATION targetpos = GetTargetInfo();
	if (g_Offset.Target != 0 && targetpos.id != 0 && targetpos.id != mypos.id && g_ColorDistance) {
		double dDistanceXZ, dDistanceXYZ;
		dDistanceXYZ = (double)sqrt(pow((long double)targetpos.x - mypos.x, 2) + pow((long double)targetpos.y - mypos.y, 2) + pow((long double)targetpos.z - mypos.z, 2));
		dDistanceXZ = (double)sqrt(pow((long double)targetpos.x - mypos.x, 2) + pow((long double)targetpos.z - mypos.z, 2));
		_stprintf(szBuf, _T("\n%0.1lf(%0.1f)"), dDistanceXYZ, dDistanceXZ);
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_TOP | DT_RIGHT);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_TOP | DT_RIGHT);
		}
		SetTextColor(*hMemDC, g_ColorDistance);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_TOP | DT_RIGHT);
	}

	//MyPosition表示
	if (g_Menu.own_position && g_ColorMyPosition) {
		if (mapInfo.AreaID >= 0x0100) {
			_stprintf(szBuf, _T("Area:%X_%1X (%.1f, %.1f, %.1f)"), mapInfo.AreaID, mapInfo.SubID, mypos.x, mypos.y, mypos.z);
		}
		else {
			_stprintf(szBuf, _T("Area:%02X_%1X (%.1f, %.1f, %.1f)"), mapInfo.AreaID, mapInfo.SubID, mypos.x, mypos.y, mypos.z);
		}
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_ColorMyPosition);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
	}

	//<pos>表示
	if (g_Menu.own_lt_pos_gt && (mapInfo.x == 0 || mapInfo.z == 0) && g_Colorlt_Pos_gt) {
		if (g_Menu.own_position) {
			_stprintf(szBuf, _T("\n(?-?)"));
		}
		else {
			_stprintf(szBuf, _T("(?-?)"));
		}
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
		return 3;
	}
	else if (g_Menu.own_lt_pos_gt && (mapInfo.zoom == (float)0.1 || mapInfo.zoom == (float)0.2 || mapInfo.zoom == (float)0.4) && g_Colorlt_Pos_gt) {
		//通常マップ倍率の時のみ表示
		//マップ上の座標に変換
		fx = ((mypos.x * mapInfo.zoom * 2 + mapInfo.x));
		fz = ((-mypos.z * mapInfo.zoom * 2 + mapInfo.z));
		//マップ上の座標から<pos>に変換
		//(fx-a)/(b) a:実座標(A-1)の開始位置 b:1マスあたりの幅
		if (g_Menu.own_position) {
			_stprintf(szBuf, _T("\n(%c-%d)"), (int)((fx - 17) / 32) + ('A'), (int)((fz - 17) / 32) + 1);
		}
		else {
			_stprintf(szBuf, _T("(%c-%d)"), (int)((fx - 17) / 32) + ('A'), (int)((fz - 17) / 32) + 1);
		}
		//ドロップシャドウ
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//縁
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
	}
	else if (g_Menu.own_lt_pos_gt && g_Colorlt_Pos_gt) {
		if (g_Menu.own_position) {
			//ドロップシャドウ
			if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				DrawText(*hMemDC, _T("\n(?-?)"), -1, &Rect, DT_LEFT);
			}
			//縁
			else if (g_FontSystem.nEffect == 2) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
			}
			SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
			DrawText(*hMemDC, _T("\n(?-?)"), -1, &WindowRect, DT_LEFT);
		}
		else {
			//ドロップシャドウ
			if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				DrawText(*hMemDC, _T("(?-?)"), -1, &Rect, DT_LEFT);
			}
			//縁
			else if (g_FontSystem.nEffect == 2) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
			}
			SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
			DrawText(*hMemDC, _T("(?-?)"), -1, &WindowRect, DT_LEFT);
		}
		return 4;
	}

	return 1;
}

/// <summary>
/// パーティメンバーとPCを描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="targetpos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawPC(HDC *hMemDC, INFORMATION mypos, INFORMATION targetpos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	float fx, fz;
	int x, z;
	INFORMATION partypos[18];
	INFORMATION pcPos;

	ZeroMemory(partypos, sizeof(partypos));

	if (g_Menu.party_position && g_ColorPARTY) {
		//Partyの位置表示
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPARTY);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPARTY);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPARTY);

		//標準フォントをNormalに設定
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0; i<6; i++) {
			partypos[i] = GetPartyInfo(i);
			if (partypos[i].code < 0)continue;
			else if (_tcslen(partypos[i].name) == 0)continue;
			else if (partypos[i].inrange == 0) {//範囲外
				continue;
			}
			else if (mypos.id == partypos[i].id) {
				continue;
			}
			else if (targetpos.id == partypos[i].id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - partypos[i].y)) > g_height_diff) {
				continue;//0.56で追加
			}

			if (!g_Menu.radar) {
				//int変換、距離補正
				x = (int)((float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int変換、距離補正
				fx = (float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//座標変換(回転)
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//描画
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			else {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			if (g_Menu.party_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePARTY);//0.46で変更
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//ドロップシャドウ
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, partypos[i].name, _tcslen(partypos[i].name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//縁作成
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, partypos[i].name);//0.52でDefineに変更
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, partypos[i].name, _tcslen(partypos[i].name));
			}
		}
		//フォントを戻す
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
	if (g_Menu.party_position && g_ColorALIANCE) {//Alianceの位置表示
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorALIANCE);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorALIANCE);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorALIANCE);

		//標準フォントをNormalに設定
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 6; i<18; i++) {
			partypos[i] = GetPartyInfo(i);
			if (partypos[i].code < 0)continue;
			else if (_tcslen(partypos[i].name) == 0)continue;
			else if (partypos[i].inrange == 0) {//範囲外
				continue;
			}
			else if (mypos.id == partypos[i].id) {
				continue;
			}
			else if (targetpos.id == partypos[i].id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - partypos[i].y)) > g_height_diff) {
				continue;//0.56で追加
			}

			if (!g_Menu.radar) {
				//int変換、距離補正
				x = (int)((float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int変換、距離補正
				fx = (float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//座標変換(回転)
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//描画
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			else {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			if (g_Menu.party_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePARTY);//0.46で変更
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//ドロップシャドウ
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, partypos[i].name, _tcslen(partypos[i].name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//縁作成
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, partypos[i].name);//0.52でDefineに変更
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, partypos[i].name, _tcslen(partypos[i].name));
			}
		}
		//フォントを戻す
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	if (g_Menu.pc_position && g_ColorPC) {
		//PCの位置表示
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPC);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPC);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPC);

		//標準フォントをNormalに設定
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0x400; i<0x700; i++) {//PCはNPCIDの0x400～0x700番目に格納
			pcPos = GetNPCInfo(i);
			if (pcPos.code < 0)continue;
			else if (fabs(pcPos.x) > 10000 || fabs(pcPos.y) > 10000 || fabs(pcPos.z) > 10000) {
				continue;
			}
			else if (pcPos.inrange == 0) {//範囲外
				continue;
			}
			else if (pcPos.id == mypos.id) {
				//自分自身の時は飛ばす
				continue;
			}
			else if (pcPos.x == 0 && pcPos.y == 0 && pcPos.z == 0) {
				continue;
			}
			else if (g_Menu.party_position) {
				int iFlag;
				iFlag = 0;
				for (int j = 1; j<18; j++) {
					//パーティメンバーの時は飛ばす
					if (pcPos.id == partypos[j].id) {
						iFlag = 1;
						break;
					}
				}
				if (iFlag)continue;
			}
			else if (targetpos.id == pcPos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - pcPos.y)) > g_height_diff) {
				continue;//0.56で追加
			}

			if (!g_Menu.radar) {
				//int変換、距離補正
				x = (int)((float)(pcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(pcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int変換、距離補正
				fx = (float)(pcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(pcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//座標変換(回転)
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//描画
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePC, pcPos.radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			else {
				DrawPoint(hMemDC, g_PointSizePC, pcPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			if (g_Menu.pc_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePC);
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//ドロップシャドウ
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, pcPos.name, _tcslen(pcPos.name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//縁作成
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, pcPos.name);//0.52でDefineに変更
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, pcPos.name, _tcslen(pcPos.name));
			}
		}
		//フォントを戻す
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	return 0;
}

/// <summary>
/// ペットを描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="targetpos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawPET(HDC *hMemDC, INFORMATION mypos, INFORMATION targetpos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	float fx, fz;
	int x, z;
	INFORMATION petPos;

	if (g_Menu.pet_position && g_ColorPET) {
		//PETの位置表示
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPET);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPET);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPET);

		//標準フォントをNormalに設定
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0x700; i < 0x800; i++) {//PETはNPCIDの0x700～0x800番目に格納?
			petPos = GetNPCInfo(i);
			if (petPos.code < 0)continue;
			else if (fabs(petPos.x) > 10000 || fabs(petPos.y) > 10000 || fabs(petPos.z) > 10000) {
				continue;
			}
			else if (petPos.inrange == 0) {//範囲外
				continue;
			}
			else if (petPos.x == 0 && petPos.y == 0 && petPos.z == 0) {
				continue;
			}
			else if (targetpos.id == petPos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - petPos.y)) > g_height_diff) {
				continue;//0.56で追加
			}

			if (!g_Menu.radar) {
				//int変換、距離補正
				x = (int)((float)(petPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(petPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int変換、距離補正
				fx = (float)(petPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(petPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//座標変換(回転)
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//描画
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePET, petPos.radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			else {
				DrawPoint(hMemDC, g_PointSizePET, petPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			if (g_Menu.pet_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePET);
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//ドロップシャドウ
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, petPos.name, _tcslen(petPos.name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//縁作成
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, petPos.name);//0.52でDefineに変更
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, petPos.name, _tcslen(petPos.name));
			}
		}
		//フォントを戻す
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	return 0;
}

/// <summary>
/// NPCを描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="hLIST"></param>
/// <param name="mypos"></param>
/// <param name="targetpos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawNPC(HDC *hMemDC, HWND hLIST, INFORMATION mypos, INFORMATION targetpos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	float fx, fz;
	int x, z;
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szShow[0x30];
	_TCHAR szType[0x10];
	int nType = 0;
	INFORMATION npcPos;
	HFONT hOldFont;


	//NPCの数を取得
	int item_count = ListView_GetItemCount(hLIST);
	int checked_id[MAX_SEARCH];
	ZeroMemory(&checked_id, sizeof(checked_id));
	//チェックされたインデックスを取得
	for (int i = 0; i<item_count; i++) {
		if (ListView_GetCheckState(hLIST, i) != 0) {
			ListView_GetItemText(hLIST, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
			checked_id[i] = _tcstoul(szBuf, NULL, 16);
		}
	}


	for (int nNormalExSp = 0; nNormalExSp <= 2; nNormalExSp++) {//Normal→Ex→Spの順番に描画する 0.60で追加
		for (int i = 0; i<item_count; i++) {
			//チェックボックスがチェックされている
			if (checked_id[i] != 0) {
				npcPos = GetNPCInfo(checked_id[i]);
				if (fabs(npcPos.x) > 10000 || fabs(npcPos.y) > 10000 || fabs(npcPos.z) > 10000) {
					continue;
				}
				else if (npcPos.x == 0 && npcPos.y == 0 && npcPos.z == 0) {
					continue;
				}
				else if (targetpos.id == npcPos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
					continue;
				}
				else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - npcPos.y)) > g_height_diff) {
					continue;//0.56で追加
				}

				if (!g_Menu.radar) {
					//int変換、距離補正
					x = (int)((float)(npcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
					z = (int)((float)(npcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
				}
				else {
					//int変換、距離補正
					fx = (float)(npcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
					fz = (float)(npcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

					//座標変換(回転)
					x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
					z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
				}

				//Typeの取得
				ListView_GetItemText(hLIST, i, 4, szType, sizeof(szType) / sizeof(_TCHAR));
				if (_tcscmp(szType, _T("EX")) == 0) {
					if (nNormalExSp != 1) {//EX描画以外では戻る 0.60で追加
						continue;
					}
					checked_id[i] = 0;//一度処理した物は削除する 0.60で追加
					nType = 1;//EX Type
				}
				else if (_tcscmp(szType, _T("SP")) == 0) {
					if (nNormalExSp != 2) {//SP描画以外では戻る 0.60で追加
						continue;
					}
					checked_id[i] = 0;//一度処理した物は削除する 0.60で追加
					nType = 2;//SP Type
				}
				else {
					if (nNormalExSp != 0) {//Normal描画以外では戻る 0.60で追加
						continue;
					}
					checked_id[i] = 0;//一度処理した物は削除する 0.60で追加
					nType = 0;//Normal Type
				}

				COLORREF color;//0.52で追加

							   //点描画
				if (npcPos.inrange != 0) {//範囲内
					if (npcPos.npctype == 0x40 && _tcscmp(_T("???"), npcPos.name) != 0) {//???は除外 0.54aで変更
						continue;//0.53で追加 範囲外のNPC(デバッグ情報だと思われる)
					}

					if (NPC_CORPSE(npcPos)) {//死亡
						color = g_ColorNPC_Corpse[nType];//0.52で変更
					}
					else if (NPC_WAR(npcPos)) {//戦闘中
						BYTE byFlag = 0;//0.46で追加 占有中の敵
						if (npcPos.lasthateid != 0) {
							for (int i = 0; i<18; i++) {
								INFORMATION info = GetPartyInfo(i);
								if (npcPos.lasthateid == info.fixedid) {
									byFlag = 1;
									break;
								}
							}
						}
						if (byFlag == 1) {
							color = g_ColorNPC_Share[nType];//0.52で追加
						}
						else {
							color = g_ColorNPC_War[nType];//0.52で追加
						}
					}
					else if (NPC_POP(npcPos)) {//フリー
						color = g_ColorNPC_Pop[nType];//0.52で追加
					}
					else {//その他、不明
						color = g_ColorNPC_Out[nType];//0.52で追加
					}
				}
				else {//範囲外
					if (nType == 0 && g_Menu.npc_onlyvisible) {
						continue;//Normal
					}
					else if (nType == 1 && g_Menu.npc_onlyvisible_ex) {
						continue;//Extra
					}
					else if (nType == 2 && g_Menu.npc_onlyvisible_sp) {//0.47で追加
						continue;//Sp
					}
					//ｖ0.41で追加
					if (g_NotChangeColorCorpse == 1 && npcPos.hpp == 0) {//Corpse
						color = g_ColorNPC_Corpse[nType];//0.52で追加
					}
					else if (g_NotChangeColorWar == 1 && (npcPos.state != 0 && npcPos.hpp != 0)) {//War
						color = g_ColorNPC_War[nType];//0.52で追加
					}
					//
					else {
						color = g_ColorNPC_Out[nType];//0.52で追加
					}
				}
				//0.52で追加
				hPen = CreatePen(PS_INSIDEFRAME, 1, color);
				hBrush = CreateSolidBrush(color);
				SetTextColor(*hMemDC, color);
				//

				hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
				hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);

				if (GetTextColor(*hMemDC)) {
					//方向付き描画
					if (!g_Menu.radar) {
						DrawPoint(hMemDC, g_PointSizeNPC, npcPos.radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
					}
					else {
						DrawPoint(hMemDC, g_PointSizeNPC, npcPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
					}

					//ID(HPP%):NAME,表示
					ZeroMemory(&szShow, sizeof(szShow));
					if (nType == 0) {//Normal
						_TCHAR szID[0x10] = _T("");
						_TCHAR szHPP[0x10] = _T("");
						_TCHAR szTIMER[0x10] = _T("");
						_TCHAR szNAME[0x20] = _T("");

						//ID
						if (g_Menu.npc_id) {
							//_stprintf(szID,_T("%03X"),checked_id[i]);
							_stprintf(szID, _T("%03X"), npcPos.id);//0.60aで変更
						}
						//HPP
						if (g_Menu.npc_hpp) {
							_stprintf(szHPP, _T("(%d%%)"), npcPos.hpp);
						}
						//Timer
						if (g_Menu.npc_timer) {
							_TCHAR szBuf[0x10];
							ListView_GetItemText(hLIST, i, 3, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
							if (_tcslen(szBuf) != 0) {
								_stprintf(szTIMER, _T("\"%s\""), szBuf);
							}
						}
						//Name
						if (g_Menu.npc_name) {
							_stprintf(szNAME, _T("%s"), npcPos.name);
						}
						//表示
						if (g_Menu.npc_id || g_Menu.npc_hpp || g_Menu.npc_timer || g_Menu.npc_name) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_Font.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//ドロップシャドウ
							hOldFont = (HFONT)SelectObject(*hMemDC, g_Font.hFont);
							if (g_Font.nEffect == 1 && !(g_Font.nDS_x == 0 && g_Font.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_Font.ColorEffect);
								TextOut(*hMemDC, nX + g_Font.nDS_x, nZ - g_Font.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_Font.nEffect == 2) {//縁作成
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_Font.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52でDefineに変更
								SetTextColor(*hMemDC, colorBuf);
							}
							TextOut(*hMemDC, nX, nZ, szShow, _tcslen(szShow));
							SelectObject(*hMemDC, hOldFont);
						}
					}
					else if (nType == 1) {//Extra
						_TCHAR szID[0x10] = _T("");
						_TCHAR szHPP[0x10] = _T("");
						_TCHAR szTIMER[0x10] = _T("");
						_TCHAR szNAME[0x20] = _T("");

						//ID
						if (g_Menu.npc_id_ex) {
							_stprintf(szID, _T("%03X"), checked_id[i]);
						}
						//HPP
						if (g_Menu.npc_hpp_ex) {
							_stprintf(szHPP, _T("(%d%%)"), npcPos.hpp);
						}
						//Timer
						if (g_Menu.npc_timer_ex) {
							_TCHAR szBuf[0x10];
							ListView_GetItemText(hLIST, i, 3, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
							if (_tcslen(szBuf) != 0) {
								_stprintf(szTIMER, _T("\"%s\""), szBuf);
							}
						}
						//Name
						if (g_Menu.npc_name_ex) {
							_stprintf(szNAME, _T("%s"), npcPos.name);
						}
						//表示
						if (g_Menu.npc_id_ex || g_Menu.npc_hpp_ex || g_Menu.npc_timer_ex || g_Menu.npc_name_ex) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_FontEx.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//ドロップシャドウ
							hOldFont = (HFONT)SelectObject(*hMemDC, g_FontEx.hFont);
							if (g_FontEx.nEffect == 1 && !(g_FontEx.nDS_x == 0 && g_FontEx.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontEx.ColorEffect);
								TextOut(*hMemDC, nX + g_FontEx.nDS_x, nZ - g_FontEx.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_FontEx.nEffect == 2) {//縁作成
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontEx.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52でDefineに変更
								SetTextColor(*hMemDC, colorBuf);
							}
							TextOut(*hMemDC, nX, nZ, szShow, _tcslen(szShow));
							SelectObject(*hMemDC, hOldFont);
						}
					}
					else if (nType == 2) {//Sp
						_TCHAR szID[0x10] = _T("");
						_TCHAR szHPP[0x10] = _T("");
						_TCHAR szTIMER[0x10] = _T("");
						_TCHAR szNAME[0x20] = _T("");

						//ID
						if (g_Menu.npc_id_sp) {
							_stprintf(szID, _T("%03X"), checked_id[i]);
						}
						//HPP
						if (g_Menu.npc_hpp_sp) {
							_stprintf(szHPP, _T("(%d%%)"), npcPos.hpp);
						}
						//Timer
						if (g_Menu.npc_timer_sp) {
							_TCHAR szBuf[0x10];
							ListView_GetItemText(hLIST, i, 3, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
							if (_tcslen(szBuf) != 0) {
								_stprintf(szTIMER, _T("\"%s\""), szBuf);
							}
						}
						//Name
						if (g_Menu.npc_name_sp) {
							_stprintf(szNAME, _T("%s"), npcPos.name);
						}
						//表示
						if (g_Menu.npc_id_sp || g_Menu.npc_hpp_sp || g_Menu.npc_timer_sp || g_Menu.npc_name_sp) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_FontSp.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//ドロップシャドウ
							hOldFont = (HFONT)SelectObject(*hMemDC, g_FontSp.hFont);
							if (g_FontSp.nEffect == 1 && !(g_FontSp.nDS_x == 0 && g_FontSp.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontSp.ColorEffect);
								TextOut(*hMemDC, nX + g_FontSp.nDS_x, nZ - g_FontSp.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_FontSp.nEffect == 2) {//縁作成
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontSp.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52でDefineに変更
								SetTextColor(*hMemDC, colorBuf);
							}
							TextOut(*hMemDC, nX, nZ, szShow, _tcslen(szShow));
							SelectObject(*hMemDC, hOldFont);
						}
					}
				}

				SelectObject(*hMemDC, hOldPen);
				SelectObject(*hMemDC, hOldBrush);
				DeleteObject(hPen);
				DeleteObject(hBrush);
			}
		}
	}


	return 0;
}

/// <summary>
/// ターゲットしているNPC/PCを描画
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="hLIST"></param>
/// <param name="mypos"></param>
/// <param name="targetpos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
int DrawTarget(HDC *hMemDC, HWND hLIST, INFORMATION mypos, INFORMATION targetpos, MAP_INFORMATION mapInfo, int nCenter_X, int nCenter_Z)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	float fx, fz;
	int x, z;
	_TCHAR szShow[0x30];
	HFONT hOldFont, hFont;
	int nType = 0;//0でNormal 1でEx 2でSp
				  //NPCの数を取得
	int item_count = ListView_GetItemCount(hLIST);

	if (targetpos.id != 0 && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
		if (!g_Menu.radar) {
			//int変換、距離補正
			x = (int)((float)(targetpos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
			z = (int)((float)(targetpos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
		}
		else {
			//int変換、距離補正
			fx = (float)(targetpos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
			fz = (float)(targetpos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

			//座標変換(回転)
			x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
			z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
		}

		COLORREF color;
		INFORMATION Info;

		//Type 取得
		if (targetpos.id > 0 && targetpos.id < MAX_SEARCH) {
			//EXを探す
			_TCHAR szBuf[0x10];
			for (int i = 0; i<item_count; i++) {
				if (ListView_GetCheckState(hLIST, i) != 0) {
					//ID取得
					ListView_GetItemText(hLIST, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
					if (_tcstoul(szBuf, NULL, 16) == targetpos.id) {
						//Typeの取得
						ListView_GetItemText(hLIST, i, 4, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
						if (_tcscmp(szBuf, _T("EX")) == 0) {//EX Type
							nType = 1;
						}
						else if (_tcscmp(szBuf, _T("SP")) == 0) {//SP Type
							nType = 2;//0.51で修正
						}
						else {//Normal
							nType = 0;
						}
						break;
					}
				}
			}
		}

		if (g_ColorTarget == RGB(0, 0, 0)) {//ターゲットしていない時と同じ色を使用する時はRGB(0,0,0)を指定する
			if (targetpos.id > 0 && targetpos.id < MAX_SEARCH) {
				if (targetpos.inrange != 0) {//範囲内
					if (NPC_WAR(targetpos)) {//戦闘中
						BYTE byFlag = 0;//0.52で追加 占有中の敵
						if (targetpos.lasthateid != 0) {
							for (int i = 0; i<18; i++) {
								INFORMATION info = GetPartyInfo(i);
								if (targetpos.lasthateid == info.fixedid) {
									byFlag = 1;
									break;
								}
							}
						}
						if (byFlag == 1) {
							color = g_ColorNPC_Share[nType];
						}
						else {
							color = g_ColorNPC_War[nType];
						}
					}
					else if (NPC_POP(targetpos)) {//フリー
						color = g_ColorNPC_Pop[nType];
					}
					else if (NPC_CORPSE(targetpos)) {//死亡
						color = g_ColorNPC_Corpse[nType];
					}
					else {//その他、不明
						color = g_ColorNPC_Out[nType];
					}
				}
				else {//範囲外全て
					color = g_ColorNPC_Out[nType];
				}
			}
			else if (targetpos.id >= MAX_SEARCH && targetpos.id < 0x700) {//PCのID範囲
				if (targetpos.id == mypos.id) {
					color = g_ColorOWN;
				}
				else {
					color = g_ColorPC;
					for (int i = 1; i<18; i++) {
						Info = GetPartyInfo(i);
						if (targetpos.id == Info.id) {
							color = g_ColorPARTY;
							break;
						}
					}
				}
			}
			else if (targetpos.id >= 0x700 && targetpos.id < 0x800) {//PETのID範囲
				color = g_ColorPET;
			}
		}
		else {
			color = g_ColorTarget;
		}

		//フォント作成
		LOGFONT logFont;
		ZeroMemory(&logFont, sizeof(logFont));
		logFont.lfCharSet = SHIFTJIS_CHARSET;
		logFont.lfQuality = ANTIALIASED_QUALITY;
		if (g_FontTarget.nSize == 0) {
			if (nType == 0) {//Normal
				logFont.lfHeight = -MulDiv(g_Font.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72);
			}
			else if (nType == 1) {//Ex
				logFont.lfHeight = -MulDiv(g_FontEx.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72);
			}
			else if (nType == 2) {//Sp
				logFont.lfHeight = -MulDiv(g_FontSp.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72);
			}
		}
		else {
			logFont.lfHeight = -MulDiv(g_FontTarget.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72);
		}
		if (_tcslen(g_FontTarget.szType) == 0) {
			if (nType == 0) {//Normal
				_tcscpy(logFont.lfFaceName, g_Font.szType);
			}
			else if (nType == 1) {//Ex
				_tcscpy(logFont.lfFaceName, g_FontEx.szType);
			}
			else if (nType == 2) {//Sp
				_tcscpy(logFont.lfFaceName, g_FontSp.szType);
			}
		}
		else {
			_tcscpy(logFont.lfFaceName, g_FontTarget.szType);
		}
		if (g_FontTarget.nBold == -1) {
			if (nType == 0 && g_Font.nBold) {//Normal
				logFont.lfWeight = FW_BOLD;
			}
			else if (nType == 1 && g_FontEx.nBold) {//Ex
				logFont.lfWeight = FW_BOLD;
			}
			else if (nType == 2 && g_FontSp.nBold) {//Ex
				logFont.lfWeight = FW_BOLD;
			}
		}
		else if (g_FontTarget.nBold == 1) {
			logFont.lfWeight = FW_BOLD;
		}
		hFont = CreateFontIndirect(&logFont);
		hOldFont = (HFONT)SelectObject(*hMemDC, hFont);

		if (color) {
			//描画
			hPen = CreatePen(PS_INSIDEFRAME, 1, color);
			hBrush = CreateSolidBrush(color);
			SetTextColor(*hMemDC, color);
			hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
			hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
			//方向付き描画
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizeTARGET, targetpos.radian, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			else {
				DrawPoint(hMemDC, g_PointSizeTARGET, targetpos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48で変更
			}
			//ハイライト
			if (g_TargetHighlight) {
				SelectObject(*hMemDC, GetStockObject(HOLLOW_BRUSH));
				Ellipse(*hMemDC, nCenter_X + x - g_TargetHighlight, nCenter_Z - z - g_TargetHighlight, nCenter_X + x + g_TargetHighlight, nCenter_Z - z + g_TargetHighlight);
			}
			SelectObject(*hMemDC, hOldBrush);
			SelectObject(*hMemDC, hOldPen);
			DeleteObject(hPen);
			DeleteObject(hBrush);


			_TCHAR szID[0x10] = _T("");
			_TCHAR szHPP[0x10] = _T("");
			_TCHAR szNAME[0x20] = _T("");
			//ID
			if (g_Menu.target_id) {
				_stprintf(szID, _T("%03X"), targetpos.id);
			}
			//HPP
			if (g_Menu.target_hpp) {
				_stprintf(szHPP, _T("(%d%%)"), targetpos.hpp);
			}
			//Name
			if (g_Menu.target_name) {
				_stprintf(szNAME, _T("%s"), targetpos.name);
			}

			//表示
			if (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name) {
				_stprintf(szShow, _T("%s%s%s"), szID, szHPP, szNAME);
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizeTARGET);
				nZ = nCenter_Z - z + logFont.lfHeight / 2;
				//ドロップシャドウ
				if (g_FontTarget.nEffect == 1 && !(g_FontTarget.nDS_x == 0 && g_FontTarget.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontTarget.ColorEffect);
					TextOut(*hMemDC, nX + g_FontTarget.nDS_x, nZ - g_FontTarget.nDS_z, szShow, _tcslen(szShow));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontTarget.nEffect == 2) {//縁作成
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontTarget.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52でDefineに変更
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, szShow, _tcslen(szShow));
			}
		}

		//フォントを元にもどす
		SelectObject(*hMemDC, hOldFont);
		DeleteObject(hFont);
	}

	return 0;
}

//エリアチェンジした時に更新させるようにする
MAP_INFORMATION GetMAPINFORMATION_SECTION(INFORMATION);

/// <summary>
/// map.ini解析
/// </summary>
/// <param name="mypos"></param>
/// <returns>MAP_INFORMATION構造体</returns>
/// <remarks>0.47で大幅変更 0.53でBYTEとWORDに分けた 0.54で大幅変更</remarks>
MAP_INFORMATION GetMAPINFORMATION(INFORMATION mypos)
{
	MAP_INFORMATION retMapInfo;
	retMapInfo = GetMAPINFORMATION_SECTION(mypos);//0.54で変更

	return retMapInfo;
}

/// <summary>
/// map.ini解析部
/// </summary>
/// <param name="mypos"></param>
/// <returns>MAP_INFORMATION構造体</returns>
/// <remarks>SubIDは配列として保存する</remarks>
MAP_INFORMATION GetMAPINFORMATION_SECTION(INFORMATION mypos)
{
	MAP_INFORMATION mapInfo;
	static MAP_INFORMATION retInfo;
	INFORMATION info[0x20];
	_TCHAR szAreaID[0x10];
	_TCHAR szSection[0x20];
	_TCHAR szFileName[MAX_PATH + 1];
	_TCHAR szFullPathName[MAX_PATH + 1];
	static _TCHAR stszAreaName[MAX_PATH + 1];
	static _TCHAR stszMapiniString[MAP_SUB_ID_MAX][0x400];
	int nSearchNow;
	static char stcSubID;//サブIDの数
	WORD wNowAreaID;
	static WORD stwAreaID = 0;//前回のエリアID

	ZeroMemory(&mapInfo, sizeof(mapInfo));

	//エリアID取得
	wNowAreaID = GetAreaID();

	if (stwAreaID != wNowAreaID) {
		ZeroMemory(&retInfo, sizeof(retInfo));
		ZeroMemory(stszAreaName, sizeof(stszAreaName));
		ZeroMemory(stszMapiniString, sizeof(stszMapiniString));
		stcSubID = 0;
		stwAreaID = wNowAreaID;
		retInfo.AreaID = stwAreaID;

		_stprintf(szFileName, _T("%s\\map.ini"), g_MapPath);
		if (0 == GetFullPathName(szFileName, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
			//map.iniが存在しないとき
			retInfo.code = 1;
			return retInfo;
		}

		if (stwAreaID >= 0x0100) {//2byteエリア
			_stprintf(szAreaID, _T("%x"), stwAreaID);
		}
		else {//1byteエリア
			_stprintf(szAreaID, _T("%02x"), stwAreaID);
		}

		//[Map] エリア名取得
		_stprintf(szSection, _T("%s_jname"), szAreaID); //a0_jname等
		GetPrivateProfileString(_T("Map"), szSection, _T(""), stszAreaName, sizeof(stszAreaName) / sizeof(_TCHAR), szFullPathName);
		if (_tcslen(stszAreaName) == 0) { //日本語名が取得できなかった時
			_stprintf(szSection, _T("%s_ename"), szAreaID); //英語名で取得
			GetPrivateProfileString(_T("Map"), szSection, _T(""), stszAreaName, sizeof(stszAreaName) / sizeof(_TCHAR), szFullPathName);
		}

		//SubIDが無い表記の配列
		int nSubID = 0;
		GetPrivateProfileString(_T("Map"), szAreaID, _T(""), stszMapiniString[nSubID], sizeof(stszMapiniString[0]) / sizeof(_TCHAR), szFullPathName);
		//SubIDがある表記の配列
		for (nSubID = 1; nSubID < 0x20; nSubID++) {
			_stprintf(szSection, _T("%s_%x"), szAreaID, nSubID - 1);
			GetPrivateProfileString(_T("Map"), szSection, _T(""), stszMapiniString[nSubID], sizeof(stszMapiniString[0]) / sizeof(_TCHAR), szFullPathName);
		}
	}

	//SubID判別
	int i;
	for (int j = 0; j < MAP_SUB_ID_MAX; j++) {
		if (_tcslen(stszMapiniString[j]) == 0) {
			continue;
		}

		//mapInfo.zoom
		i = 0;
		nSearchNow = i;
		mapInfo.zoom = (float)_ttof(&stszMapiniString[j][nSearchNow]);

		//mapInfo.x
		i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
		if (i == 0) {
			retInfo.code = 3;	//x軸オフセットが存在しない
			break;
		}
		nSearchNow += i;

		mapInfo.x = (float)_ttof(&stszMapiniString[j][nSearchNow]) * 2;	//512*512マップ使用

		//mapInfo.zoom
		i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
		if (i == 0) {
			retInfo.code = 4;	//zoom(z軸)が存在しない
			break;
		}
		nSearchNow += i;

		//mapInfo.zoom = -1 * (float)_ttof(&stszMapiniString[j][nSearchNow]);

		//mapInfo.z
		i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
		if (i == 0) {
			retInfo.code = 5;	//z軸オフセットが存在しない
			break;
		}
		nSearchNow += i;

		mapInfo.z = (float)_ttof(&stszMapiniString[j][nSearchNow]) * 2;

		int nEnd = 0x20; //マップ1枚あたり16ブロックまで
		for (int k = 0; k < nEnd; k++) {
			//info[k].x
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				if (k == 0) {
					retInfo.code = 6;	//座標がまったく存在しない
					return retInfo;
				}
				else if (k % 2 == 0){
					nEnd = k;	//ブロック数確定
					break;
				}
				else {
					retInfo.code = 7;	//x2座標が存在しない
					return retInfo;
				}
			}
			nSearchNow += i;
			info[k].x = (float)_ttof(&stszMapiniString[j][nSearchNow]);
			//info[k].y
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				retInfo.code = 8;	//y座標が存在しない
				return retInfo;
			}
			nSearchNow += i;
			info[k].y = (float)_ttof(&stszMapiniString[j][nSearchNow]);
			//info[k].z
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				retInfo.code = 9;	//z座標が存在しない
				return retInfo;
			}
			nSearchNow += i;
			info[k].z = (float)_ttof(&stszMapiniString[j][nSearchNow]);
		}

		//現在のサブエリアに自分の位置が含まれているかチェック
		for (int k = 0; k < nEnd; k = k + 2) {
			//xyz1 < xyz2となるようにする
			if (info[k].x > info[k + 1].x) {
				float min_num = info[k + 1].x;
				info[k + 1].x = info[k].x;
				info[k].x = min_num;
			}
			if (info[k].y > info[k + 1].y) {
				float min_num = info[k + 1].y;
				info[k + 1].y = info[k].y;
				info[k].y = min_num;
			}
			if (info[k].z > info[k + 1].z) {
				float min_num = info[k + 1].z;
				info[k + 1].z = info[k].z;
				info[k].z = min_num;
			}

			if (
				info[k].x < mypos.x && info[k + 1].x >= mypos.x &&
				info[k].y < mypos.y && info[k + 1].y >= mypos.y &&
				info[k].z < mypos.z && info[k + 1].z >= mypos.z
				) {//含まれていればMap情報を返す
				if (j == 0)retInfo.SubID = 0;
				else retInfo.SubID = j - 1;
				retInfo.AreaID = stwAreaID;
				retInfo.x = mapInfo.x;
				retInfo.z = mapInfo.z;
				retInfo.zoom = mapInfo.zoom;
				retInfo.code = 0;//0は正常終了
				_tcscpy(retInfo.AreaName, stszAreaName);

				return retInfo;
			}
		}
	}

	return retInfo;
}

//0.54で作成
float GetZoomIni()
{
	float retfzoom;
	_TCHAR szFileName[MAX_PATH + 1];
	_TCHAR szFullPathName[MAX_PATH + 1];
	_TCHAR szSection[0x20];
	_TCHAR szZoom[0x10];
	WORD wAreaID;

	_stprintf(szFileName, _T("%s\\zoom.ini"), g_MapPath);
	if (0 == GetFullPathName(szFileName, sizeof(szFullPathName) / sizeof(_TCHAR), szFullPathName, NULL)) {
		return -1;
	}

	wAreaID = GetAreaID();
	if (wAreaID >= 0x0100) {
		_stprintf(szSection, _T("%04x"), wAreaID);
	}
	else {
		_stprintf(szSection, _T("%02x"), wAreaID);
	}

	GetPrivateProfileString(_T("ZOOM"), szSection, _T(""), szZoom, sizeof(szZoom) / sizeof(_TCHAR), szFullPathName);
	retfzoom = (float)_ttof(szZoom);

	return retfzoom;
}


/// <summary>
/// マップがあれば読み込み
/// </summary>
/// <param name="mapInfo"></param>
/// <param name="szExtension"></param>
/// <returns></returns>
/// <remarks>0.58でGIF/BMP両対応化</remarks>
IPicture* LoadMAP(MAP_INFORMATION mapInfo, _TCHAR *szExtension)
{
	IPicture *pPic;
	HRESULT hr;
	_TCHAR szBuf[MAX_PATH + 1];
	_TCHAR cFilename[MAX_PATH + 1];
	wchar_t wcFilename[MAX_PATH + 1];

	//ファイル名を作成
	if (g_HexToDec == 0) {
		if (mapInfo.AreaID >= 0x0100) {//2Byteエリア 0.53で追加
			_stprintf(szBuf, _T("%s\\%x_%x.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x_%x.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
	}
	else {
		if (mapInfo.AreaID >= 0x0100) {//2Byteエリア 0.53で追加
			_stprintf(szBuf, _T("%s\\%x_%d.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x_%d.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
	}

	if (0 == GetFullPathName(szBuf, sizeof(cFilename) / sizeof(_TCHAR), cFilename, NULL)) {
		return 0;
	}
	if (!PathFileExists(cFilename)) {//ファイルが存在しなかった時
		if (mapInfo.AreaID >= 0x0100) {//SubIDの無いファイルを読み込む
			_stprintf(szBuf, _T("%s\\%x.%s"), g_MapPath, mapInfo.AreaID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x.%s"), g_MapPath, mapInfo.AreaID, szExtension);
		}
		if (0 == GetFullPathName(szBuf, sizeof(cFilename) / sizeof(_TCHAR), cFilename, NULL)) {
			return 0;
		}
	}

#ifndef UNICODE//ユニコード文字列に変換
	mbstowcs(wcFilename, cFilename, _tcslen(cFilename) + 1);
#else//元々ユニコードなのでコピーするだけ
	wcscpy(wcFilename, cFilename);
#endif
	hr = OleLoadPicturePath(wcFilename, NULL, 0, 0, IID_IPicture, (void**)&pPic);
	if (FAILED(hr)) {
		return 0;
	}

	return pPic;
}

/// <summary>
/// マップを描画
/// </summary>
/// <param name="hDC"></param>
/// <param name="pPic"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <param name="rect"></param>
/// <param name="zoom"></param>
/// <returns></returns>
int RenderMAP(HDC hDC, IPicture *pPic, INFORMATION mypos, MAP_INFORMATION mapInfo, RECT rect, float zoom)
{
	OLE_XSIZE_HIMETRIC hmWidth;
	OLE_YSIZE_HIMETRIC hmHeight;
	SIZEL sizel;

	if (!pPic)return 0;
	pPic->get_Width(&hmWidth);
	pPic->get_Height(&hmHeight);
	sizel.cx = hmWidth;
	sizel.cy = hmHeight;
	// HIMETRIC(2540dpi) -> pixel(標準で96dpi)
	const int PPI_X = GetDeviceCaps(hDC, LOGPIXELSX);
	const int PPI_Y = GetDeviceCaps(hDC, LOGPIXELSY);
	const int HPI = 2540;
	LONG lWidth = (PPI_X * hmWidth + HPI / 2) / HPI;
	LONG lHeight = (PPI_Y * hmHeight + HPI / 2) / HPI;

	lWidth = (LONG)(lWidth * zoom);
	lHeight = (LONG)(lHeight * zoom);

	double x = 0, z = 0;//0.52で初期値0に変更

	if (g_Menu.center_own) {
		//座標生成
		x = rect.right / 2 - mapInfo.x * zoom - mypos.x * zoom * 2 * mapInfo.zoom;
		z = rect.bottom / 2 - mapInfo.z * zoom + mypos.z * zoom * 2 * mapInfo.zoom;
	}

	if (!g_Menu.radar) {//0.57aで変更
		x += g_CenterMAP_X;
		z += g_CenterMAP_Z;
	}
	else {
		x += g_CenterMAP_X * cos(mypos.radian + 3.141592 / 2) - g_CenterMAP_Z * sin(mypos.radian + 3.141592 / 2);
		z += g_CenterMAP_X * sin(mypos.radian + 3.141592 / 2) + g_CenterMAP_Z * cos(mypos.radian + 3.141592 / 2);
	}

	//画像描画
	pPic->Render(hDC, (int)x, (int)z, lWidth, lHeight, 0, hmWidth, hmHeight, -hmHeight, NULL);


	//Mappu0.40にて追加
	//RGB(1,1,1)をRGB(0,0,0)に変換する
	if (g_Menu.transparent && g_ColorTransparent == RGB(0, 0, 0)) {
		//モノクロ作成　背景色が白となる
		COLORREF defaultBkColor = SetBkColor(hDC, RGB(1, 1, 1));
		HDC hMonoDC = CreateCompatibleDC(hDC);
		HBITMAP hMonoBmp = CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 1, 1, NULL);
		SelectObject(hMonoDC, hMonoBmp);
		BitBlt(hMonoDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hDC, 0, 0, SRCCOPY);
		DeleteObject(hMonoBmp);
		//背景色を戻す
		SetBkColor(hDC, defaultBkColor);

		//色付き　NOTSRCVOPYで背景色を黒とする
		HDC hColorDC = CreateCompatibleDC(hDC);
		HBITMAP hColorBmp = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hColorDC, hColorBmp);
		DeleteObject(hColorBmp);
		BitBlt(hColorDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMonoDC, 0, 0, NOTSRCCOPY);

		//合成　黒は0なのでANDしたときに描画しない
		BitBlt(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hColorDC, 0, 0, SRCAND);
		DeleteDC(hMonoDC);
		DeleteDC(hColorDC);
	}

	//画像反転
	if (g_Menu.image_reverse) {
		BitBlt(hDC, (int)x, (int)z, lWidth, lHeight, hDC, (int)x, (int)z, NOTSRCCOPY);
	}

	//レーダー表示
	if (g_Menu.radar) {
		float fx, fz;
		int x, z;
		//回転
		POINT pntRotate[3];
		pntRotate[0].x = -(rect.right - rect.left) / 2;
		pntRotate[0].y = -(rect.bottom - rect.top) / 2;
		pntRotate[1].x = +(rect.right - rect.left) / 2;
		pntRotate[1].y = -(rect.bottom - rect.top) / 2;
		pntRotate[2].x = -(rect.right - rect.left) / 2;
		pntRotate[2].y = +(rect.bottom - rect.top) / 2;

		for (int i = 0; i<3; i++) {
			//int変換、距離補正
			fx = (float)(pntRotate[i].x);
			fz = (float)(pntRotate[i].y);

			//座標変換(回転)
			x = (int)((float)fx * cos(-3.141592 / 2 - mypos.radian) - (float)fz * sin(-3.141592 / 2 - mypos.radian)) + (rect.right - rect.left) / 2;
			z = (int)((float)fx * sin(-3.141592 / 2 - mypos.radian) + (float)fz * cos(-3.141592 / 2 - mypos.radian)) + (rect.bottom - rect.top) / 2;

			pntRotate[i].x = x;
			pntRotate[i].y = z;
		}
		HDC hRotateDC = CreateCompatibleDC(hDC);
		HBITMAP hRotateBmp = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hRotateDC, hRotateBmp);
		DeleteObject(hRotateBmp);
		PlgBlt(hRotateDC, pntRotate, hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, 0, 0);
		BitBlt(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hRotateDC, 0, 0, SRCCOPY);
		DeleteDC(hRotateDC);
	}

	return 1;
}

//過去の自分の位置情報を初期化 0.55で追加
void InitOwnPos()
{
	for (int i = 0; i<sizeof(g_OwnPos) / sizeof(POSXYZR); i++) {
		g_OwnPos[0].x = -10000;
		g_OwnPos[0].y = -10000;
		g_OwnPos[0].z = -10000;
		g_OwnPos[0].radian = -10000;
	}
}

//自分の位置情報を保存 0.55で追加
void SaveOwnPos(INFORMATION myPos)
{
	static DWORD dwSaveTime = 0;
	DWORD dwNowTime = timeGetTime();

	if (dwNowTime - dwSaveTime >= 3 * 1000) {//3秒ごとに自分の位置を保存
		if (myPos.x == g_OwnPos[0].x && myPos.y == g_OwnPos[0].y && myPos.z == g_OwnPos[0].z) {
			return;
		}

		dwSaveTime = dwNowTime;
		for (int i = sizeof(g_OwnPos) / sizeof(POSXYZR) - 1; i>0; i--) {
			g_OwnPos[i] = g_OwnPos[i - 1];
		}
		g_OwnPos[0].x = myPos.x;
		g_OwnPos[0].y = myPos.y;
		g_OwnPos[0].z = myPos.z;
		g_OwnPos[0].radian = myPos.radian;
	}
}

//過去の自分の位置情報を出力 0.55で追加
POSXYZR LoadOwnPos(int i)
{
	return g_OwnPos[i];
}

/// <summary>
/// 自分の位置を取得
/// </summary>
/// <returns></returns>
INFORMATION GetMyInfo()
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[NOWSTA_SIZE];
	WORD change_id = 0;

	//構造体初期化
	ZeroMemory(&info, sizeof(info));

	//メモリリード
	dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
		info.code = -1;
		return info;
	}

	memcpy(&change_id, &dump[NOWSTA_MEM_CHANGEID], sizeof(WORD));

	info = GetNPCInfo(change_id);

	return info;
}

/// <summary>
/// ターゲットしているNPC/PCの情報
/// </summary>
/// <returns></returns>
INFORMATION GetTargetInfo()
{
	DWORD dwAddress;
	WORD change_id;
	INFORMATION info;

	ZeroMemory(&info, sizeof(INFORMATION));

	dwAddress = g_DllAddress + g_Offset.Target;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		info.code = -1;
		return info;
	}
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &change_id, sizeof(change_id), NULL)) {
		info.code = -1;
		return info;
	}

	if (change_id != 0) {
		info = GetNPCInfo(change_id);
	}

	return info;
}

/// <summary>
/// NPC情報取得
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
INFORMATION GetNPCInfo(int id)
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[NPC_MEM_SIZE];

	//構造体初期化
	ZeroMemory(&info, sizeof(info));

	//メモリリード
	dwAddress = g_DllAddress + g_Offset.NpcMap + id * sizeof(DWORD);
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		info.code = -1;
		return info;
	}
	if (dwAddress != 0) {
		//情報アドレスが存在する時の処理
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
			info.code = -2;
			return info;
		}
		memcpy(&(info.x), &dump[NPC_MEM.POS_X], sizeof(float));
		memcpy(&(info.y), &dump[NPC_MEM.POS_Y], sizeof(float));
		memcpy(&(info.z), &dump[NPC_MEM.POS_Z], sizeof(float));
		memcpy(&(info.radian), &dump[NPC_MEM.Radian], sizeof(float));
#ifdef UNICODE
		mbstowcs(info.name, &dump[NPC_MEM.Name], sizeof(info.name) / sizeof(_TCHAR) - 1);
#else
		_tcscpy(info.name, &dump[NPC_MEM.NAME]);
#endif
		info.name[NPCLIST_ID] = _T('\0');//最大文字列対策
		memcpy(&info.viewAddress, &dump[NPC_MEM.ViewAddress], sizeof(DWORD));//0.47で変更
		info.npctype = dump[NPC_MEM.NpcType];
		info.view = dump[NPC_MEM.View];
		info.hpp = dump[NPC_MEM.Hpp];
		info.state = dump[NPC_MEM.State];
		info.inrange = dump[NPC_MEM.Inrange];
		info.id = (WORD)id;
		memcpy(&(info.fixedid), &dump[NPC_MEM.FixedId], sizeof(DWORD));//0.46で追加
		memcpy(&(info.lasthateid), &dump[NPC_MEM.LastHateId], sizeof(DWORD));//0.46で追加
		memcpy(&(info.distance_2), &dump[NPC_MEM.Distance], sizeof(float));//0.57で追加
	}
	else {
		//dwAddressにアドレスが無い場合
		info.code = -3;
	}

	return info;
}

/// <summary>
/// PARTY情報取得
/// </summary>
/// <param name="pos"></param>
/// <returns></returns>
INFORMATION GetPartyInfo(int pos)
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[0x210];

	//構造体初期化
	ZeroMemory(&info, sizeof(info));

	//メモリリード
	dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI + pos * NOWSTA_SIZE;//スタート位置の補正NOWSTA_HOSEI
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
		info.code = -1;
		return info;
	}

	//NOWSTA_MEM_PARTYが0の時すでに脱退
	if (dump[NOWSTA_MEM_PARTY]) {
		memcpy(&(info.id), &dump[NOWSTA_MEM_CHANGEID], sizeof(WORD));

		info = GetNPCInfo(info.id);
	}
	else {
		info.code = -4;
	}

	return info;
}

/// <summary>
/// エリアIDを取得する
/// </summary>
/// <returns></returns>
WORD GetAreaID()
{
	DWORD dwAddress, dwStartAddress;
	DWORD dwFixedID;//固定ID
	WORD wNPCID;//0.53で変数名を修正
	WORD AreaID = 0;


	if (g_Offset.NowSta != 0) {
		//PC_StatusからエリアIDを取得

		BYTE dump[NOWSTA_SIZE];

		//メモリリード
		dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI;

		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
			AreaID = 0;
			return AreaID;
		}

		memcpy(&AreaID, &dump[NOWSTA_MEM_AREAID], sizeof(WORD));

	}
	else {//NPCLISTの敵固定IDからエリアIDを取得する
		BYTE dump[NPCLIST_SIZE];
		dwAddress = g_DllAddress + g_Offset.NpcList;
		//NPCLISTのポインタを取得してdwStartAddressに格納
		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwStartAddress, sizeof(dwStartAddress), NULL)) {
			return -1;
		}
		for (int i = 1; i < 0x10; i++) {
			dwAddress = dwStartAddress + NPCLIST_SIZE * i;
			if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, NPCLIST_SIZE, NULL)) {
				return -1;
			}
			memcpy(&dwFixedID, &dump[NPCLIST_ID], sizeof(DWORD));
			wNPCID = (WORD)(dwFixedID & 0xFFF);
			if (isupper(dump[0]) != 0 && wNPCID != 0) {
				AreaID = (WORD)((dwFixedID >> 12) & 0xFFFF);
				break;
			}
		}
	}

	return AreaID;
}

//天候を取得
BYTE GetWeather()
{
	BYTE byteWeather = 0;
	DWORD dwAddress;

	if (g_Offset.Weather == 0) {
		return 0;
	}
	dwAddress = g_DllAddress + g_Offset.Weather;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &byteWeather, sizeof(BYTE), NULL)) {
		return -1;
	}
	if (byteWeather >= 0x14) {
		return -1;
	}

	return byteWeather;
}


//文字列szBuf内の文字chの先頭位置をサーチ
//見つからなかった場合0
int SearchChar(_TCHAR *szBuf, _TCHAR ch)
{
	int i = 0;

	while (szBuf[i] != 0) {
		if (szBuf[i] == ch)return i + 1;
		i++;
	}

	return 0;
}
