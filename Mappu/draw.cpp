#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//���j�R�[�h�ŃR���p�C��
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

static POSXYZR g_OwnPos[300];//0.55�Œǉ� 0.55a�ŗv�f����

/// <summary>
/// �`��֘A�̎G�L
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

	//�����̏����擾
	mypos = GetMyInfo();

	//�}�b�v�̏����擾
	mapInfo = GetMAPINFORMATION(mypos);

#ifndef DEBUG
	//WindowText���X�V 0.50�Œǉ�
	_TCHAR szText[0x20], szCmp[0x20];
	GetWindowText(hWnd, szCmp, sizeof(szCmp) / sizeof(_TCHAR) - 1);
	_stprintf(szText, _T("Mappu - %s"), mypos.name);
	if (_tcscmp(szText, szCmp) != 0 && _tcslen(mypos.name) != 0)SetWindowText(hWnd, szText);
#endif

	//�}�b�v�\��(Map�E�B���h�E)
	if ((nRet = DrawMap(hWnd, mypos, &mapInfo)) != 0) {
		return nRet;//nRet��5�`40
	}

	//�L�����N�^�[�\��(�L�����N�^�[�E�B���h�E)
	if ((nRet = DrawCharacter(hWnd, hLIST, hCHARACTER, mypos, mapInfo)) != 0) {
		return nRet;//nRet��41�`69
	}

	//�V��A�C�R����\��
	if ((nRet = DrawWeatherIcon(hWnd)) != 0) {
		return nRet;//nRet��80�`
	}

	return 0;
}

/// <summary>
/// �}�b�v��`��
/// </summary>
/// <param name="hWnd"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <returns>����I��:0</returns>
int DrawMap(HWND hWnd, INFORMATION mypos, MAP_INFORMATION *mapInfo)
{
	HDC hDC;
	HDC hMemDC;
	HBITMAP hBitmap;
	_TCHAR szBuf[MAX_PATH];
	static IPicture *pPic = NULL;
	static MAP_INFORMATION cmp_mapInfo;

	//�E�B���h�E�̑傫�����擾
	RECT rect;
	if (0 == GetWindowRect(hWnd, &rect))return 5;

	//�}�b�v�\��(MAP�E�B���h�E)
	//�ĕ`��(false=�w�i�������Ȃ�)
	if (0 == InvalidateRect(hWnd, NULL, false))return 6;

	//�_�u���o�b�t�@
	hDC = GetDC(hWnd);//GetDC��ReleaseDC
	if (hDC == NULL)return 7;
	if ((hMemDC = CreateCompatibleDC(hDC)) == NULL)return 8;//CreateCompaticleDC��DeleteDC

															// �傫���������f�o�C�X�R���e�L�X�g�����i�ׂɏ������Ă�OK�j
	BITMAPINFO bmpinfo;
	ZeroMemory(&bmpinfo, sizeof(bmpinfo));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = rect.right - rect.left;
	bmpinfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;//32�r�b�g�J���[
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	if ((hBitmap = CreateDIBSection(NULL, &bmpinfo, DIB_RGB_COLORS, NULL, NULL, NULL)) == NULL)return 9;
	// ��������r�b�g�}�b�v���A�������f�o�C�X�R���e�L�X�g�ɑI������
	if (NULL == SelectObject(hMemDC, hBitmap))return 10;
	if (0 == DeleteObject(hBitmap))return 11;
	if (0 == ReleaseDC(hWnd, hDC))return 12;

	//�t�H���g
	HFONT hOldFont;
	hOldFont = (HFONT)SelectObject(hMemDC, g_FontSystem.hFont);

	//�N���C�A���g�̑傫�����擾
	if (0 == GetClientRect(hWnd, &rect))return 15;

	//���ߐF�Ŗ��߂�
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

	//�����w�i����
	if (0 == SetBkMode(hMemDC, TRANSPARENT))return 20;

	*mapInfo = GetMAPINFORMATION(mypos);//gif�\��

	if (!pPic && (cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID)) {//�ǂݍ��݂��Ă��Ȃ����ɏ����� 0.54�ŕύX
		ZeroMemory(&cmp_mapInfo, sizeof(cmp_mapInfo));
	}

	if (mapInfo->code == 0) {//map.ini�ǂݍ��݂�����ɏI�������ꍇ
		if (cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID) {//�ǂݍ��ݏ������d���̂œ����}�b�v�̎��͍ė��p
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
			float fZoomIni = GetZoomIni();//0.54�Œǉ�
			if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54�Œǉ�
		}

		if (pPic) {
			RenderMAP(hMemDC, pPic, mypos, *mapInfo, rect, g_Zoom);
		}
		else {
			if (mapInfo->AreaID >= 0x0100) {//2Byte�G���A 0.53�Œǉ�
				_stprintf(szBuf, _T("No MAP!Area = %x"), mapInfo->AreaID);
			}
			else {
				_stprintf(szBuf, _T("No MAP!Area = %02x"), mapInfo->AreaID);
			}
			SetTextColor(hMemDC, g_ColorMapName);
			DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
			mapInfo->zoom = (float)0.4;

			//if(cmp_mapInfo.AreaID != mapInfo->AreaID || cmp_mapInfo.SubID != mapInfo->SubID){//0.52�ŕύX
			if (cmp_mapInfo.AreaID != mapInfo->AreaID) {//0.54�ŕύX
				if (g_Menu.default_zoomset) {//DefaultZoomSetting
					g_Zoom = g_Zoom_04;
				}
				float fZoomIni = GetZoomIni();//0.54�Œǉ�
				if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54�Œǉ�
			}
		}
	}
	else if (mapInfo->code == 1) {
		//map.ini��������Ȃ�������
		_stprintf(szBuf, _T("map.ini��������܂���B"));
		SetTextColor(hMemDC, g_ColorMapName);
		DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);//0.47a�ŕύX
		mapInfo->zoom = (float)0.4;
		if (g_Menu.default_zoomset) {//DefaultZoomSetting
			g_Zoom = g_Zoom_04;
		}
		float fZoomIni = GetZoomIni();//0.54�Œǉ�
		if (fZoomIni >= (float)0.5)g_Zoom = fZoomIni;//0.54�Œǉ�
	}
	else {//map.ini�ǂݍ��ݎ��̂��̑��G���[
		if (mapInfo->AreaID >= 0x0100) {
			_stprintf(szBuf, _T("map.ini�ǂݍ��݃G���[�B code = %d,Area = %x"), mapInfo->code, mapInfo->AreaID);
		}
		else {
			_stprintf(szBuf, _T("map.ini�ǂݍ��݃G���[�B code = %d,Area = %02x"), mapInfo->code, mapInfo->AreaID);
		}
		SetTextColor(hMemDC, g_ColorMapName);
		DrawText(hMemDC, szBuf, -1, &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);//0.47a�ŕύX
		mapInfo->zoom = (float)0.4;
		if (g_Menu.default_zoomset) {//DefaultZoomSetting
			g_Zoom = g_Zoom_04;
		}
		float fZoomIni = GetZoomIni();//0.54�Œǉ�
		if (fZoomIni >= (float)0.5) {
			g_Zoom = fZoomIni;
		}//0.54�Œǉ�
	}

	//����\��
	if (DrawInfo(&hMemDC, mypos, *mapInfo, rect) <= 0) {
		return 25;
	}

	//��r�p�ɃR�s�[
	cmp_mapInfo = *mapInfo;


	SelectObject(hMemDC, hOldFont);

	//�`��
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);

	if (hDC == NULL) {
		return 30;
	}
	if (0 == BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY)) {//CAPTUREBLT�œ��ߗ����R�s�[
		return 31;
	}

	EndPaint(hWnd, &ps);

	if (0 == DeleteDC(hMemDC)) {
		return 32;
	}

	return 0;
}

/// <summary>
/// �L�����N�^�[�E�B���h�E�ɕ`��
/// </summary>
/// <param name="hWnd"></param>
/// <param name="hLIST"></param>
/// <param name="hCHARACTER"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <returns>����I��:0</returns>
int DrawCharacter(HWND hWnd, HWND hLIST, HWND hCHARACTER, INFORMATION mypos, MAP_INFORMATION mapInfo)
{
	HDC hDC, hMemDC;
	HBITMAP hBitmap;
	INFORMATION targetInfo;
	int nCenter_X, nCenter_Z;
	RECT rect;

	targetInfo = GetTargetInfo();

	//�E�B���h�E�̑傫�����擾
	if (0 == GetWindowRect(hWnd, &rect))return 40;

	//�ĕ`��(false=�w�i�������Ȃ�)
	if (0 == InvalidateRect(hCHARACTER, NULL, false))return 41;

	//�_�u���o�b�t�@
	hDC = GetDC(hCHARACTER);
	if (hDC == NULL)return 42;
	if ((hMemDC = CreateCompatibleDC(hDC)) == NULL)return 43;
	// �傫���������f�o�C�X�R���e�L�X�g�����i�ׂɏ������Ă�OK�j
	BITMAPINFO bmpinfo;
	ZeroMemory(&bmpinfo, sizeof(bmpinfo));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = rect.right - rect.left;
	bmpinfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;//32�r�b�g�J���[
	bmpinfo.bmiHeader.biCompression = BI_RGB;

	if ((hBitmap = CreateDIBSection(NULL, &bmpinfo, DIB_RGB_COLORS, NULL, NULL, NULL)) == NULL) {
		return 44;
	}
	// ��������r�b�g�}�b�v���A�������f�o�C�X�R���e�L�X�g�ɑI������
	if (NULL == SelectObject(hMemDC, hBitmap))return 45;
	if (0 == DeleteObject(hBitmap))return 46;
	if (0 == ReleaseDC(hWnd, hDC))return 47;

	//�N���C�A���g�̑傫�����擾
	if (0 == GetClientRect(hWnd, &rect))return 48;
	//�E�B���h�E�̒��S���W
	nCenter_X = rect.right / 2;
	nCenter_Z = rect.bottom / 2;

	//�����w�i����
	if (0 == SetBkMode(hMemDC, TRANSPARENT))return 50;

	//���͈͉~��`��
	if (DrawCircle(&hMemDC, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 55;
	}

	//�����̉ߋ��̈ʒu��\�� 0.55�Œǉ�
	if (DrawPastOwn(&hMemDC, mypos, mapInfo, nCenter_X, nCenter_Z) <= 0) {
		return 58;
	}

	//�����̈ʒu��\��
	if (DrawOwn(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) <= 0) {
		return 60;
	}

	//�p�[�e�B�����o�[��PC�̈ʒu��\��
	if (DrawPC(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 61;
	}

	//PET�̈ʒu��\��
	if (DrawPET(&hMemDC, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 62;
	}

	//NPC�̈ʒu��\��
	if (DrawNPC(&hMemDC, hLIST, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 63;
	}

	//Target�̈ʒu��\��
	if (DrawTarget(&hMemDC, hLIST, mypos, targetInfo, mapInfo, nCenter_X, nCenter_Z) != 0) {
		return 64;
	}

	//���W�v�Z
	int x, z;
	if (g_Menu.center_own == 1) {
		//x = 0;
		//z = 0;
		x = -g_CenterMAP_X;//0.52�ŕύX
		z = -g_CenterMAP_Z;//0.52�ŕύX
	}
	else {
		x = (int)(nCenter_X - mapInfo.x * g_Zoom - mypos.x * g_Zoom * 2 * mapInfo.zoom) - g_CenterMAP_X;
		z = (int)(nCenter_Z - mapInfo.z * g_Zoom + mypos.z * g_Zoom * 2 * mapInfo.zoom) - g_CenterMAP_Z;
	}

	//GDI�֐��ɂ��`����J�n����
	PAINTSTRUCT ps;
	hDC = BeginPaint(hCHARACTER, &ps);
	if (hDC == NULL) {
		return 65;
	}
	//�L���͈͂𓧉ߐF�̍��œh��Ԃ�
	//if(g_Menu.center_own == 0 && 0 == PatBlt(hDC,0,0,rect.right,rect.bottom,BLACKNESS)){
	if (0 == PatBlt(hDC, 0, 0, rect.right, rect.bottom, BLACKNESS)) {//0.52�ŕύX
		return 66;
	}
	if (0 == BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, x, z, SRCCOPY)) {//CAPTUREBLT�œ��ߗ����R�s�[
		return 67;
	}

	//GDI�֐��ɂ��`����I������
	EndPaint(hWnd, &ps);

	if (0 == DeleteDC(hMemDC)) {
		return 68;
	}

	return 0;
}

/// <summary>
/// �L�����N�^�[�E�B���h�E�ɓV���`��
/// </summary>
/// <param name="hWnd"></param>
/// <returns>����I���F0</returns>
int DrawWeatherIcon(HWND hWnd)
{
	static BYTE cmp_PastWeather = 0;
	static int nFlagIcon = 0;//���݂̓V��A�C�R��
	BYTE byteWeather;
	HICON hIcon;
	const _TCHAR szWeather[][8] = {
		_T("fine"),_T("fine"),			//�����E����
		_T("clouds"),_T("fog"),			//������E��
		_T("fire"),_T("fire"),			//�M�g�E�ܔM�g
		_T("water"),_T("water"),		//�J�E�X�R�[��
		_T("earth"),_T("earth"),		//���o�E����
		_T("wind"),_T("wind"),			//���E�\��
		_T("ice"),_T("ice"),			//��E����
		_T("thunder"),_T("thunder"),	//���E���J
		_T("light"),_T("light"),		//�I�[�����E�_��
		_T("dark"),_T("dark"),			//�d���E��
	};
	_TCHAR szIconName[0x20];

	byteWeather = GetWeather();
	if (g_Menu.weather_icon && byteWeather != nFlagIcon && byteWeather > 3) {//0.48�ŕύX 0.50�ŕύX
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
/// �e��T�[�N����`��
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

	//���͈͉~��`��
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
	//�A�r�͈�
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
	//���@�͈�
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
	//���u�U���͈�
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
	//�Ǌy�틭���͈̔�
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
	//���@���m�͈�
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
/// �����t���ʒu��`��
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="fPointSize"></param>
/// <param name="fRadian"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <param name="nX"></param>
/// <param name="nZ"></param>
/// <returns></returns>
/// <remarks>0.48�Œǉ�</remarks>
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
/// �ߋ��̎�����`��
/// </summary>
/// <param name="hMemDC"></param>
/// <param name="mypos"></param>
/// <param name="mapInfo"></param>
/// <param name="nCenter_X"></param>
/// <param name="nCenter_Z"></param>
/// <returns></returns>
/// <remarks>0.55�Œǉ�</remarks>
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
	stwAreaID = mapInfo.AreaID;//0.55a�ňړ�
	stbSubID = mapInfo.SubID;
	SaveOwnPos(mypos);

	hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorOWN);
	hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
	hBrush = CreateSolidBrush(g_ColorOWN);
	hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);

	int x, z;
	float fx, fz;
	//�����̈ʒu
	if (g_ColorOWN) {
		MoveToEx(*hMemDC, nCenter_X, nCenter_Z, NULL);
		POSXYZR OwnPosMinus = LoadOwnPos(0);
		for (int i = 0; i<sizeof(g_OwnPos) / sizeof(POSXYZR); i++) {
			POSXYZR OwnPos = LoadOwnPos(i);
			if (OwnPos.x == -10000)break;
			else if (OwnPos.x == 0 && OwnPos.y == 0 && OwnPos.z == 0 && OwnPos.radian == 0)continue;//0.55a�Œǉ�

			if (!g_Menu.radar) {
				//int�ϊ��A�����␳
				x = (int)((float)(OwnPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(OwnPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int�ϊ��A�����␳
				fx = (float)(OwnPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(OwnPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//���W�ϊ�(��])
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//�O�̈ʒu�Ƃ̋�����50Ym�ȏ�̎�����؂� 0.55a�Œǉ�
			if (sqrt(pow(OwnPos.x - OwnPosMinus.x, 2) + pow(OwnPos.z - OwnPosMinus.z, 2)) >= (float)50) {
				MoveToEx(*hMemDC, (int)(nCenter_X + x), (int)(nCenter_Z - z), NULL);
			}
			OwnPosMinus.x = OwnPos.x;
			OwnPosMinus.z = OwnPos.z;

			//�`��
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
/// ������`��
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

	//�����̈ʒu
	if (targetpos.id == mypos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
		if (g_ColorOWN) {//0.46�Œǉ�
						 //��������\��
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
			DrawPoint(hMemDC, g_PointSizeOWN, mypos.radian, nCenter_X, nCenter_Z, 0, 0);//0.48�ŕύX
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
		//��������\��
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
/// ����`��
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

	//WindowRect�����菬��������
	WindowRect.top += 1;
	WindowRect.bottom -= 1;
	WindowRect.left += 1;
	WindowRect.right -= 1;

	//DropShadow�p��Rect
	RECT Rect;
	Rect.top = WindowRect.top - g_FontSystem.nDS_z;
	Rect.bottom = WindowRect.bottom - g_FontSystem.nDS_z;;
	Rect.left = WindowRect.left + g_FontSystem.nDS_x;
	Rect.right = WindowRect.right + g_FontSystem.nDS_x;

	//Edge�p��Rect
	RECT RectEdge[8];
	char cEdge[8][2] = { { 1,1 },{ 1,0 },{ 1,-1 },{ 0,1 },{ 0,-1 },{ -1,1 },{ -1,0 },{ -1,-1 } };
	for (int i = 0; i<8; i++) {
		RectEdge[i].top = WindowRect.top + cEdge[i][0];
		RectEdge[i].bottom = WindowRect.bottom + cEdge[i][0];
		RectEdge[i].left = WindowRect.left + cEdge[i][1];
		RectEdge[i].right = WindowRect.right + cEdge[i][1];
	}

	//Area���\��
	if (_tcslen(mapInfo.AreaName) != 0 && g_ColorMapName) {
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, mapInfo.AreaName, -1, &Rect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, mapInfo.AreaName, -1, &RectEdge[i], DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		}
		SetTextColor(*hMemDC, g_ColorMapName);
		DrawText(*hMemDC, mapInfo.AreaName, -1, &WindowRect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
	}

	//�{���\��
	if (g_ColorZoom) {
		_stprintf(szBuf, _T("x%0.1f"), g_Zoom);
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_SINGLELINE | DT_TOP | DT_RIGHT);
		}
		SetTextColor(*hMemDC, g_ColorZoom);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_SINGLELINE | DT_TOP | DT_RIGHT);
	}

	//Distance������\��
	INFORMATION targetpos = GetTargetInfo();
	if (g_Offset.Target != 0 && targetpos.id != 0 && targetpos.id != mypos.id && g_ColorDistance) {
		double dDistanceXZ, dDistanceXYZ;
		dDistanceXYZ = (double)sqrt(pow((long double)targetpos.x - mypos.x, 2) + pow((long double)targetpos.y - mypos.y, 2) + pow((long double)targetpos.z - mypos.z, 2));
		dDistanceXZ = (double)sqrt(pow((long double)targetpos.x - mypos.x, 2) + pow((long double)targetpos.z - mypos.z, 2));
		_stprintf(szBuf, _T("\n%0.1lf(%0.1f)"), dDistanceXYZ, dDistanceXZ);
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_TOP | DT_RIGHT);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_TOP | DT_RIGHT);
		}
		SetTextColor(*hMemDC, g_ColorDistance);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_TOP | DT_RIGHT);
	}

	//MyPosition�\��
	if (g_Menu.own_position && g_ColorMyPosition) {
		if (mapInfo.AreaID >= 0x0100) {
			_stprintf(szBuf, _T("Area:%X_%1X (%.1f, %.1f, %.1f)"), mapInfo.AreaID, mapInfo.SubID, mypos.x, mypos.y, mypos.z);
		}
		else {
			_stprintf(szBuf, _T("Area:%02X_%1X (%.1f, %.1f, %.1f)"), mapInfo.AreaID, mapInfo.SubID, mypos.x, mypos.y, mypos.z);
		}
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_ColorMyPosition);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
	}

	//<pos>�\��
	if (g_Menu.own_lt_pos_gt && (mapInfo.x == 0 || mapInfo.z == 0) && g_Colorlt_Pos_gt) {
		if (g_Menu.own_position) {
			_stprintf(szBuf, _T("\n(?-?)"));
		}
		else {
			_stprintf(szBuf, _T("(?-?)"));
		}
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
		return 3;
	}
	else if (g_Menu.own_lt_pos_gt && (mapInfo.zoom == (float)0.1 || mapInfo.zoom == (float)0.2 || mapInfo.zoom == (float)0.4) && g_Colorlt_Pos_gt) {
		//�ʏ�}�b�v�{���̎��̂ݕ\��
		//�}�b�v��̍��W�ɕϊ�
		fx = ((mypos.x * mapInfo.zoom * 2 + mapInfo.x));
		fz = ((-mypos.z * mapInfo.zoom * 2 + mapInfo.z));
		//�}�b�v��̍��W����<pos>�ɕϊ�
		//(fx-a)/(b) a:�����W(A-1)�̊J�n�ʒu b:1�}�X������̕�
		if (g_Menu.own_position) {
			_stprintf(szBuf, _T("\n(%c-%d)"), (int)((fx - 17) / 32) + ('A'), (int)((fz - 17) / 32) + 1);
		}
		else {
			_stprintf(szBuf, _T("(%c-%d)"), (int)((fx - 17) / 32) + ('A'), (int)((fz - 17) / 32) + 1);
		}
		//�h���b�v�V���h�E
		if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			DrawText(*hMemDC, szBuf, -1, &Rect, DT_LEFT);
		}
		//��
		else if (g_FontSystem.nEffect == 2) {
			SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
			for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
		}
		SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
		DrawText(*hMemDC, szBuf, -1, &WindowRect, DT_LEFT);
	}
	else if (g_Menu.own_lt_pos_gt && g_Colorlt_Pos_gt) {
		if (g_Menu.own_position) {
			//�h���b�v�V���h�E
			if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				DrawText(*hMemDC, _T("\n(?-?)"), -1, &Rect, DT_LEFT);
			}
			//��
			else if (g_FontSystem.nEffect == 2) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				for (int i = 0; i<8; i++)DrawText(*hMemDC, szBuf, -1, &RectEdge[i], DT_LEFT);
			}
			SetTextColor(*hMemDC, g_Colorlt_Pos_gt);
			DrawText(*hMemDC, _T("\n(?-?)"), -1, &WindowRect, DT_LEFT);
		}
		else {
			//�h���b�v�V���h�E
			if (g_FontSystem.nEffect == 1 && !(g_FontSystem.nDS_x == 0 && g_FontSystem.nDS_z == 0)) {
				SetTextColor(*hMemDC, g_FontSystem.ColorEffect);
				DrawText(*hMemDC, _T("(?-?)"), -1, &Rect, DT_LEFT);
			}
			//��
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
/// �p�[�e�B�����o�[��PC��`��
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
		//Party�̈ʒu�\��
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPARTY);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPARTY);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPARTY);

		//�W���t�H���g��Normal�ɐݒ�
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0; i<6; i++) {
			partypos[i] = GetPartyInfo(i);
			if (partypos[i].code < 0)continue;
			else if (_tcslen(partypos[i].name) == 0)continue;
			else if (partypos[i].inrange == 0) {//�͈͊O
				continue;
			}
			else if (mypos.id == partypos[i].id) {
				continue;
			}
			else if (targetpos.id == partypos[i].id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - partypos[i].y)) > g_height_diff) {
				continue;//0.56�Œǉ�
			}

			if (!g_Menu.radar) {
				//int�ϊ��A�����␳
				x = (int)((float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int�ϊ��A�����␳
				fx = (float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//���W�ϊ�(��])
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//�`��
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			else {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			if (g_Menu.party_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePARTY);//0.46�ŕύX
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//�h���b�v�V���h�E
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, partypos[i].name, _tcslen(partypos[i].name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//���쐬
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, partypos[i].name);//0.52��Define�ɕύX
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, partypos[i].name, _tcslen(partypos[i].name));
			}
		}
		//�t�H���g��߂�
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
	if (g_Menu.party_position && g_ColorALIANCE) {//Aliance�̈ʒu�\��
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorALIANCE);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorALIANCE);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorALIANCE);

		//�W���t�H���g��Normal�ɐݒ�
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 6; i<18; i++) {
			partypos[i] = GetPartyInfo(i);
			if (partypos[i].code < 0)continue;
			else if (_tcslen(partypos[i].name) == 0)continue;
			else if (partypos[i].inrange == 0) {//�͈͊O
				continue;
			}
			else if (mypos.id == partypos[i].id) {
				continue;
			}
			else if (targetpos.id == partypos[i].id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - partypos[i].y)) > g_height_diff) {
				continue;//0.56�Œǉ�
			}

			if (!g_Menu.radar) {
				//int�ϊ��A�����␳
				x = (int)((float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int�ϊ��A�����␳
				fx = (float)(partypos[i].x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(partypos[i].z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//���W�ϊ�(��])
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//�`��
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			else {
				DrawPoint(hMemDC, g_PointSizePARTY, partypos[i].radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			if (g_Menu.party_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePARTY);//0.46�ŕύX
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//�h���b�v�V���h�E
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, partypos[i].name, _tcslen(partypos[i].name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//���쐬
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, partypos[i].name);//0.52��Define�ɕύX
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, partypos[i].name, _tcslen(partypos[i].name));
			}
		}
		//�t�H���g��߂�
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	if (g_Menu.pc_position && g_ColorPC) {
		//PC�̈ʒu�\��
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPC);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPC);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPC);

		//�W���t�H���g��Normal�ɐݒ�
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0x400; i<0x700; i++) {//PC��NPCID��0x400�`0x700�ԖڂɊi�[
			pcPos = GetNPCInfo(i);
			if (pcPos.code < 0)continue;
			else if (fabs(pcPos.x) > 10000 || fabs(pcPos.y) > 10000 || fabs(pcPos.z) > 10000) {
				continue;
			}
			else if (pcPos.inrange == 0) {//�͈͊O
				continue;
			}
			else if (pcPos.id == mypos.id) {
				//�������g�̎��͔�΂�
				continue;
			}
			else if (pcPos.x == 0 && pcPos.y == 0 && pcPos.z == 0) {
				continue;
			}
			else if (g_Menu.party_position) {
				int iFlag;
				iFlag = 0;
				for (int j = 1; j<18; j++) {
					//�p�[�e�B�����o�[�̎��͔�΂�
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
				continue;//0.56�Œǉ�
			}

			if (!g_Menu.radar) {
				//int�ϊ��A�����␳
				x = (int)((float)(pcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(pcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int�ϊ��A�����␳
				fx = (float)(pcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(pcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//���W�ϊ�(��])
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//�`��
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePC, pcPos.radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			else {
				DrawPoint(hMemDC, g_PointSizePC, pcPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			if (g_Menu.pc_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePC);
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//�h���b�v�V���h�E
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, pcPos.name, _tcslen(pcPos.name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//���쐬
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, pcPos.name);//0.52��Define�ɕύX
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, pcPos.name, _tcslen(pcPos.name));
			}
		}
		//�t�H���g��߂�
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	return 0;
}

/// <summary>
/// �y�b�g��`��
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
		//PET�̈ʒu�\��
		hPen = CreatePen(PS_INSIDEFRAME, 1, g_ColorPET);
		hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
		hBrush = CreateSolidBrush(g_ColorPET);
		hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
		SetTextColor(*hMemDC, g_ColorPET);

		//�W���t�H���g��Normal�ɐݒ�
		HFONT hOldFont;
		hOldFont = (HFONT)SelectObject(*hMemDC, g_FontPC.hFont);

		for (int i = 0x700; i < 0x800; i++) {//PET��NPCID��0x700�`0x800�ԖڂɊi�[?
			petPos = GetNPCInfo(i);
			if (petPos.code < 0)continue;
			else if (fabs(petPos.x) > 10000 || fabs(petPos.y) > 10000 || fabs(petPos.z) > 10000) {
				continue;
			}
			else if (petPos.inrange == 0) {//�͈͊O
				continue;
			}
			else if (petPos.x == 0 && petPos.y == 0 && petPos.z == 0) {
				continue;
			}
			else if (targetpos.id == petPos.id && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
				continue;
			}
			else if (g_Menu.hide_height_diff && _abs64((long)(mypos.y - petPos.y)) > g_height_diff) {
				continue;//0.56�Œǉ�
			}

			if (!g_Menu.radar) {
				//int�ϊ��A�����␳
				x = (int)((float)(petPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
				z = (int)((float)(petPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
			}
			else {
				//int�ϊ��A�����␳
				fx = (float)(petPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
				fz = (float)(petPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

				//���W�ϊ�(��])
				x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
				z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
			}

			//�`��
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizePET, petPos.radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			else {
				DrawPoint(hMemDC, g_PointSizePET, petPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			if (g_Menu.pet_name) {
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizePET);
				nZ = nCenter_Z - z - (MulDiv(g_FontPC.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
				//�h���b�v�V���h�E
				if (g_FontPC.nEffect == 1 && !(g_FontPC.nDS_x == 0 && g_FontPC.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					TextOut(*hMemDC, nX + g_FontPC.nDS_x, nZ - g_FontPC.nDS_z, petPos.name, _tcslen(petPos.name));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontPC.nEffect == 2) {//���쐬
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontPC.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, petPos.name);//0.52��Define�ɕύX
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, petPos.name, _tcslen(petPos.name));
			}
		}
		//�t�H���g��߂�
		SelectObject(*hMemDC, hOldFont);

		SelectObject(*hMemDC, hOldPen);
		SelectObject(*hMemDC, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}

	return 0;
}

/// <summary>
/// NPC��`��
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


	//NPC�̐����擾
	int item_count = ListView_GetItemCount(hLIST);
	int checked_id[MAX_SEARCH];
	ZeroMemory(&checked_id, sizeof(checked_id));
	//�`�F�b�N���ꂽ�C���f�b�N�X���擾
	for (int i = 0; i<item_count; i++) {
		if (ListView_GetCheckState(hLIST, i) != 0) {
			ListView_GetItemText(hLIST, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
			checked_id[i] = _tcstoul(szBuf, NULL, 16);
		}
	}


	for (int nNormalExSp = 0; nNormalExSp <= 2; nNormalExSp++) {//Normal��Ex��Sp�̏��Ԃɕ`�悷�� 0.60�Œǉ�
		for (int i = 0; i<item_count; i++) {
			//�`�F�b�N�{�b�N�X���`�F�b�N����Ă���
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
					continue;//0.56�Œǉ�
				}

				if (!g_Menu.radar) {
					//int�ϊ��A�����␳
					x = (int)((float)(npcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
					z = (int)((float)(npcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
				}
				else {
					//int�ϊ��A�����␳
					fx = (float)(npcPos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
					fz = (float)(npcPos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

					//���W�ϊ�(��])
					x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
					z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
				}

				//Type�̎擾
				ListView_GetItemText(hLIST, i, 4, szType, sizeof(szType) / sizeof(_TCHAR));
				if (_tcscmp(szType, _T("EX")) == 0) {
					if (nNormalExSp != 1) {//EX�`��ȊO�ł͖߂� 0.60�Œǉ�
						continue;
					}
					checked_id[i] = 0;//��x�����������͍폜���� 0.60�Œǉ�
					nType = 1;//EX Type
				}
				else if (_tcscmp(szType, _T("SP")) == 0) {
					if (nNormalExSp != 2) {//SP�`��ȊO�ł͖߂� 0.60�Œǉ�
						continue;
					}
					checked_id[i] = 0;//��x�����������͍폜���� 0.60�Œǉ�
					nType = 2;//SP Type
				}
				else {
					if (nNormalExSp != 0) {//Normal�`��ȊO�ł͖߂� 0.60�Œǉ�
						continue;
					}
					checked_id[i] = 0;//��x�����������͍폜���� 0.60�Œǉ�
					nType = 0;//Normal Type
				}

				COLORREF color;//0.52�Œǉ�

							   //�_�`��
				if (npcPos.inrange != 0) {//�͈͓�
					if (npcPos.npctype == 0x40 && _tcscmp(_T("???"), npcPos.name) != 0) {//???�͏��O 0.54a�ŕύX
						continue;//0.53�Œǉ� �͈͊O��NPC(�f�o�b�O��񂾂Ǝv����)
					}

					if (NPC_CORPSE(npcPos)) {//���S
						color = g_ColorNPC_Corpse[nType];//0.52�ŕύX
					}
					else if (NPC_WAR(npcPos)) {//�퓬��
						BYTE byFlag = 0;//0.46�Œǉ� ��L���̓G
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
							color = g_ColorNPC_Share[nType];//0.52�Œǉ�
						}
						else {
							color = g_ColorNPC_War[nType];//0.52�Œǉ�
						}
					}
					else if (NPC_POP(npcPos)) {//�t���[
						color = g_ColorNPC_Pop[nType];//0.52�Œǉ�
					}
					else {//���̑��A�s��
						color = g_ColorNPC_Out[nType];//0.52�Œǉ�
					}
				}
				else {//�͈͊O
					if (nType == 0 && g_Menu.npc_onlyvisible) {
						continue;//Normal
					}
					else if (nType == 1 && g_Menu.npc_onlyvisible_ex) {
						continue;//Extra
					}
					else if (nType == 2 && g_Menu.npc_onlyvisible_sp) {//0.47�Œǉ�
						continue;//Sp
					}
					//��0.41�Œǉ�
					if (g_NotChangeColorCorpse == 1 && npcPos.hpp == 0) {//Corpse
						color = g_ColorNPC_Corpse[nType];//0.52�Œǉ�
					}
					else if (g_NotChangeColorWar == 1 && (npcPos.state != 0 && npcPos.hpp != 0)) {//War
						color = g_ColorNPC_War[nType];//0.52�Œǉ�
					}
					//
					else {
						color = g_ColorNPC_Out[nType];//0.52�Œǉ�
					}
				}
				//0.52�Œǉ�
				hPen = CreatePen(PS_INSIDEFRAME, 1, color);
				hBrush = CreateSolidBrush(color);
				SetTextColor(*hMemDC, color);
				//

				hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
				hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);

				if (GetTextColor(*hMemDC)) {
					//�����t���`��
					if (!g_Menu.radar) {
						DrawPoint(hMemDC, g_PointSizeNPC, npcPos.radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
					}
					else {
						DrawPoint(hMemDC, g_PointSizeNPC, npcPos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
					}

					//ID(HPP%):NAME,�\��
					ZeroMemory(&szShow, sizeof(szShow));
					if (nType == 0) {//Normal
						_TCHAR szID[0x10] = _T("");
						_TCHAR szHPP[0x10] = _T("");
						_TCHAR szTIMER[0x10] = _T("");
						_TCHAR szNAME[0x20] = _T("");

						//ID
						if (g_Menu.npc_id) {
							//_stprintf(szID,_T("%03X"),checked_id[i]);
							_stprintf(szID, _T("%03X"), npcPos.id);//0.60a�ŕύX
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
						//�\��
						if (g_Menu.npc_id || g_Menu.npc_hpp || g_Menu.npc_timer || g_Menu.npc_name) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_Font.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//�h���b�v�V���h�E
							hOldFont = (HFONT)SelectObject(*hMemDC, g_Font.hFont);
							if (g_Font.nEffect == 1 && !(g_Font.nDS_x == 0 && g_Font.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_Font.ColorEffect);
								TextOut(*hMemDC, nX + g_Font.nDS_x, nZ - g_Font.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_Font.nEffect == 2) {//���쐬
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_Font.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52��Define�ɕύX
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
						//�\��
						if (g_Menu.npc_id_ex || g_Menu.npc_hpp_ex || g_Menu.npc_timer_ex || g_Menu.npc_name_ex) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_FontEx.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//�h���b�v�V���h�E
							hOldFont = (HFONT)SelectObject(*hMemDC, g_FontEx.hFont);
							if (g_FontEx.nEffect == 1 && !(g_FontEx.nDS_x == 0 && g_FontEx.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontEx.ColorEffect);
								TextOut(*hMemDC, nX + g_FontEx.nDS_x, nZ - g_FontEx.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_FontEx.nEffect == 2) {//���쐬
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontEx.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52��Define�ɕύX
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
						//�\��
						if (g_Menu.npc_id_sp || g_Menu.npc_hpp_sp || g_Menu.npc_timer_sp || g_Menu.npc_name_sp) {
							_stprintf(szShow, _T("%s%s%s%s"), szID, szHPP, szTIMER, szNAME);
							int nX, nZ;
							nX = nCenter_X + x + 1 + int(2 * g_PointSizeNPC);
							nZ = nCenter_Z - z - (MulDiv(g_FontSp.nSize, GetDeviceCaps(*hMemDC, LOGPIXELSY), 72) / 2);
							//�h���b�v�V���h�E
							hOldFont = (HFONT)SelectObject(*hMemDC, g_FontSp.hFont);
							if (g_FontSp.nEffect == 1 && !(g_FontSp.nDS_x == 0 && g_FontSp.nDS_z == 0)) {
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontSp.ColorEffect);
								TextOut(*hMemDC, nX + g_FontSp.nDS_x, nZ - g_FontSp.nDS_z, szShow, _tcslen(szShow));
								SetTextColor(*hMemDC, colorBuf);
							}
							else if (g_FontSp.nEffect == 2) {//���쐬
								COLORREF colorBuf;
								colorBuf = GetTextColor(*hMemDC);
								SetTextColor(*hMemDC, g_FontSp.ColorEffect);
								DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52��Define�ɕύX
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
/// �^�[�Q�b�g���Ă���NPC/PC��`��
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
	int nType = 0;//0��Normal 1��Ex 2��Sp
				  //NPC�̐����擾
	int item_count = ListView_GetItemCount(hLIST);

	if (targetpos.id != 0 && (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name)) {
		if (!g_Menu.radar) {
			//int�ϊ��A�����␳
			x = (int)((float)(targetpos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom);
			z = (int)((float)(targetpos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom);
		}
		else {
			//int�ϊ��A�����␳
			fx = (float)(targetpos.x - mypos.x) * g_Zoom * 2 * mapInfo.zoom;
			fz = (float)(targetpos.z - mypos.z) * g_Zoom * 2 * mapInfo.zoom;

			//���W�ϊ�(��])
			x = (int)((float)fx * cos(mypos.radian + 3.141592 / 2) - (float)fz * sin(mypos.radian + 3.141592 / 2));
			z = (int)((float)fx * sin(mypos.radian + 3.141592 / 2) + (float)fz * cos(mypos.radian + 3.141592 / 2));
		}

		COLORREF color;
		INFORMATION Info;

		//Type �擾
		if (targetpos.id > 0 && targetpos.id < MAX_SEARCH) {
			//EX��T��
			_TCHAR szBuf[0x10];
			for (int i = 0; i<item_count; i++) {
				if (ListView_GetCheckState(hLIST, i) != 0) {
					//ID�擾
					ListView_GetItemText(hLIST, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
					if (_tcstoul(szBuf, NULL, 16) == targetpos.id) {
						//Type�̎擾
						ListView_GetItemText(hLIST, i, 4, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
						if (_tcscmp(szBuf, _T("EX")) == 0) {//EX Type
							nType = 1;
						}
						else if (_tcscmp(szBuf, _T("SP")) == 0) {//SP Type
							nType = 2;//0.51�ŏC��
						}
						else {//Normal
							nType = 0;
						}
						break;
					}
				}
			}
		}

		if (g_ColorTarget == RGB(0, 0, 0)) {//�^�[�Q�b�g���Ă��Ȃ����Ɠ����F���g�p���鎞��RGB(0,0,0)���w�肷��
			if (targetpos.id > 0 && targetpos.id < MAX_SEARCH) {
				if (targetpos.inrange != 0) {//�͈͓�
					if (NPC_WAR(targetpos)) {//�퓬��
						BYTE byFlag = 0;//0.52�Œǉ� ��L���̓G
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
					else if (NPC_POP(targetpos)) {//�t���[
						color = g_ColorNPC_Pop[nType];
					}
					else if (NPC_CORPSE(targetpos)) {//���S
						color = g_ColorNPC_Corpse[nType];
					}
					else {//���̑��A�s��
						color = g_ColorNPC_Out[nType];
					}
				}
				else {//�͈͊O�S��
					color = g_ColorNPC_Out[nType];
				}
			}
			else if (targetpos.id >= MAX_SEARCH && targetpos.id < 0x700) {//PC��ID�͈�
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
			else if (targetpos.id >= 0x700 && targetpos.id < 0x800) {//PET��ID�͈�
				color = g_ColorPET;
			}
		}
		else {
			color = g_ColorTarget;
		}

		//�t�H���g�쐬
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
			//�`��
			hPen = CreatePen(PS_INSIDEFRAME, 1, color);
			hBrush = CreateSolidBrush(color);
			SetTextColor(*hMemDC, color);
			hOldPen = (HPEN)SelectObject(*hMemDC, hPen);
			hOldBrush = (HBRUSH)SelectObject(*hMemDC, hBrush);
			//�����t���`��
			if (!g_Menu.radar) {
				DrawPoint(hMemDC, g_PointSizeTARGET, targetpos.radian, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			else {
				DrawPoint(hMemDC, g_PointSizeTARGET, targetpos.radian - mypos.radian - (float)3.141592 / 2, nCenter_X, nCenter_Z, x, z);//0.48�ŕύX
			}
			//�n�C���C�g
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

			//�\��
			if (g_Menu.target_id || g_Menu.target_hpp || g_Menu.target_name) {
				_stprintf(szShow, _T("%s%s%s"), szID, szHPP, szNAME);
				int nX, nZ;
				nX = nCenter_X + x + 1 + int(2 * g_PointSizeTARGET);
				nZ = nCenter_Z - z + logFont.lfHeight / 2;
				//�h���b�v�V���h�E
				if (g_FontTarget.nEffect == 1 && !(g_FontTarget.nDS_x == 0 && g_FontTarget.nDS_z == 0)) {
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontTarget.ColorEffect);
					TextOut(*hMemDC, nX + g_FontTarget.nDS_x, nZ - g_FontTarget.nDS_z, szShow, _tcslen(szShow));
					SetTextColor(*hMemDC, colorBuf);
				}
				else if (g_FontTarget.nEffect == 2) {//���쐬
					COLORREF colorBuf;
					colorBuf = GetTextColor(*hMemDC);
					SetTextColor(*hMemDC, g_FontTarget.ColorEffect);
					DRAW_EDGE(*hMemDC, nX, nZ, szShow);//0.52��Define�ɕύX
					SetTextColor(*hMemDC, colorBuf);
				}
				TextOut(*hMemDC, nX, nZ, szShow, _tcslen(szShow));
			}
		}

		//�t�H���g�����ɂ��ǂ�
		SelectObject(*hMemDC, hOldFont);
		DeleteObject(hFont);
	}

	return 0;
}

//�G���A�`�F���W�������ɍX�V������悤�ɂ���
MAP_INFORMATION GetMAPINFORMATION_SECTION(INFORMATION);

/// <summary>
/// map.ini���
/// </summary>
/// <param name="mypos"></param>
/// <returns>MAP_INFORMATION�\����</returns>
/// <remarks>0.47�ő啝�ύX 0.53��BYTE��WORD�ɕ����� 0.54�ő啝�ύX</remarks>
MAP_INFORMATION GetMAPINFORMATION(INFORMATION mypos)
{
	MAP_INFORMATION retMapInfo;
	retMapInfo = GetMAPINFORMATION_SECTION(mypos);//0.54�ŕύX

	return retMapInfo;
}

/// <summary>
/// map.ini��͕�
/// </summary>
/// <param name="mypos"></param>
/// <returns>MAP_INFORMATION�\����</returns>
/// <remarks>SubID�͔z��Ƃ��ĕۑ�����</remarks>
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
	static char stcSubID;//�T�uID�̐�
	WORD wNowAreaID;
	static WORD stwAreaID = 0;//�O��̃G���AID

	ZeroMemory(&mapInfo, sizeof(mapInfo));

	//�G���AID�擾
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
			//map.ini�����݂��Ȃ��Ƃ�
			retInfo.code = 1;
			return retInfo;
		}

		if (stwAreaID >= 0x0100) {//2byte�G���A
			_stprintf(szAreaID, _T("%x"), stwAreaID);
		}
		else {//1byte�G���A
			_stprintf(szAreaID, _T("%02x"), stwAreaID);
		}

		//[Map] �G���A���擾
		_stprintf(szSection, _T("%s_jname"), szAreaID); //a0_jname��
		GetPrivateProfileString(_T("Map"), szSection, _T(""), stszAreaName, sizeof(stszAreaName) / sizeof(_TCHAR), szFullPathName);
		if (_tcslen(stszAreaName) == 0) { //���{�ꖼ���擾�ł��Ȃ�������
			_stprintf(szSection, _T("%s_ename"), szAreaID); //�p�ꖼ�Ŏ擾
			GetPrivateProfileString(_T("Map"), szSection, _T(""), stszAreaName, sizeof(stszAreaName) / sizeof(_TCHAR), szFullPathName);
		}

		//SubID�������\�L�̔z��
		int nSubID = 0;
		GetPrivateProfileString(_T("Map"), szAreaID, _T(""), stszMapiniString[nSubID], sizeof(stszMapiniString[0]) / sizeof(_TCHAR), szFullPathName);
		//SubID������\�L�̔z��
		for (nSubID = 1; nSubID < 0x20; nSubID++) {
			_stprintf(szSection, _T("%s_%x"), szAreaID, nSubID - 1);
			GetPrivateProfileString(_T("Map"), szSection, _T(""), stszMapiniString[nSubID], sizeof(stszMapiniString[0]) / sizeof(_TCHAR), szFullPathName);
		}
	}

	//SubID����
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
			retInfo.code = 3;	//x���I�t�Z�b�g�����݂��Ȃ�
			break;
		}
		nSearchNow += i;

		mapInfo.x = (float)_ttof(&stszMapiniString[j][nSearchNow]) * 2;	//512*512�}�b�v�g�p

		//mapInfo.zoom
		i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
		if (i == 0) {
			retInfo.code = 4;	//zoom(z��)�����݂��Ȃ�
			break;
		}
		nSearchNow += i;

		//mapInfo.zoom = -1 * (float)_ttof(&stszMapiniString[j][nSearchNow]);

		//mapInfo.z
		i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
		if (i == 0) {
			retInfo.code = 5;	//z���I�t�Z�b�g�����݂��Ȃ�
			break;
		}
		nSearchNow += i;

		mapInfo.z = (float)_ttof(&stszMapiniString[j][nSearchNow]) * 2;

		int nEnd = 0x20; //�}�b�v1��������16�u���b�N�܂�
		for (int k = 0; k < nEnd; k++) {
			//info[k].x
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				if (k == 0) {
					retInfo.code = 6;	//���W���܂��������݂��Ȃ�
					return retInfo;
				}
				else if (k % 2 == 0){
					nEnd = k;	//�u���b�N���m��
					break;
				}
				else {
					retInfo.code = 7;	//x2���W�����݂��Ȃ�
					return retInfo;
				}
			}
			nSearchNow += i;
			info[k].x = (float)_ttof(&stszMapiniString[j][nSearchNow]);
			//info[k].y
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				retInfo.code = 8;	//y���W�����݂��Ȃ�
				return retInfo;
			}
			nSearchNow += i;
			info[k].y = (float)_ttof(&stszMapiniString[j][nSearchNow]);
			//info[k].z
			i = SearchChar(&stszMapiniString[j][nSearchNow], _T(','));
			if (i == 0) {
				retInfo.code = 9;	//z���W�����݂��Ȃ�
				return retInfo;
			}
			nSearchNow += i;
			info[k].z = (float)_ttof(&stszMapiniString[j][nSearchNow]);
		}

		//���݂̃T�u�G���A�Ɏ����̈ʒu���܂܂�Ă��邩�`�F�b�N
		for (int k = 0; k < nEnd; k = k + 2) {
			//xyz1 < xyz2�ƂȂ�悤�ɂ���
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
				) {//�܂܂�Ă����Map����Ԃ�
				if (j == 0)retInfo.SubID = 0;
				else retInfo.SubID = j - 1;
				retInfo.AreaID = stwAreaID;
				retInfo.x = mapInfo.x;
				retInfo.z = mapInfo.z;
				retInfo.zoom = mapInfo.zoom;
				retInfo.code = 0;//0�͐���I��
				_tcscpy(retInfo.AreaName, stszAreaName);

				return retInfo;
			}
		}
	}

	return retInfo;
}

//0.54�ō쐬
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
/// �}�b�v������Γǂݍ���
/// </summary>
/// <param name="mapInfo"></param>
/// <param name="szExtension"></param>
/// <returns></returns>
/// <remarks>0.58��GIF/BMP���Ή���</remarks>
IPicture* LoadMAP(MAP_INFORMATION mapInfo, _TCHAR *szExtension)
{
	IPicture *pPic;
	HRESULT hr;
	_TCHAR szBuf[MAX_PATH + 1];
	_TCHAR cFilename[MAX_PATH + 1];
	wchar_t wcFilename[MAX_PATH + 1];

	//�t�@�C�������쐬
	if (g_HexToDec == 0) {
		if (mapInfo.AreaID >= 0x0100) {//2Byte�G���A 0.53�Œǉ�
			_stprintf(szBuf, _T("%s\\%x_%x.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x_%x.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
	}
	else {
		if (mapInfo.AreaID >= 0x0100) {//2Byte�G���A 0.53�Œǉ�
			_stprintf(szBuf, _T("%s\\%x_%d.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x_%d.%s"), g_MapPath, mapInfo.AreaID, mapInfo.SubID, szExtension);
		}
	}

	if (0 == GetFullPathName(szBuf, sizeof(cFilename) / sizeof(_TCHAR), cFilename, NULL)) {
		return 0;
	}
	if (!PathFileExists(cFilename)) {//�t�@�C�������݂��Ȃ�������
		if (mapInfo.AreaID >= 0x0100) {//SubID�̖����t�@�C����ǂݍ���
			_stprintf(szBuf, _T("%s\\%x.%s"), g_MapPath, mapInfo.AreaID, szExtension);
		}
		else {
			_stprintf(szBuf, _T("%s\\%02x.%s"), g_MapPath, mapInfo.AreaID, szExtension);
		}
		if (0 == GetFullPathName(szBuf, sizeof(cFilename) / sizeof(_TCHAR), cFilename, NULL)) {
			return 0;
		}
	}

#ifndef UNICODE//���j�R�[�h������ɕϊ�
	mbstowcs(wcFilename, cFilename, _tcslen(cFilename) + 1);
#else//���X���j�R�[�h�Ȃ̂ŃR�s�[���邾��
	wcscpy(wcFilename, cFilename);
#endif
	hr = OleLoadPicturePath(wcFilename, NULL, 0, 0, IID_IPicture, (void**)&pPic);
	if (FAILED(hr)) {
		return 0;
	}

	return pPic;
}

/// <summary>
/// �}�b�v��`��
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
	// HIMETRIC(2540dpi) -> pixel(�W����96dpi)
	const int PPI_X = GetDeviceCaps(hDC, LOGPIXELSX);
	const int PPI_Y = GetDeviceCaps(hDC, LOGPIXELSY);
	const int HPI = 2540;
	LONG lWidth = (PPI_X * hmWidth + HPI / 2) / HPI;
	LONG lHeight = (PPI_Y * hmHeight + HPI / 2) / HPI;

	lWidth = (LONG)(lWidth * zoom);
	lHeight = (LONG)(lHeight * zoom);

	double x = 0, z = 0;//0.52�ŏ����l0�ɕύX

	if (g_Menu.center_own) {
		//���W����
		x = rect.right / 2 - mapInfo.x * zoom - mypos.x * zoom * 2 * mapInfo.zoom;
		z = rect.bottom / 2 - mapInfo.z * zoom + mypos.z * zoom * 2 * mapInfo.zoom;
	}

	if (!g_Menu.radar) {//0.57a�ŕύX
		x += g_CenterMAP_X;
		z += g_CenterMAP_Z;
	}
	else {
		x += g_CenterMAP_X * cos(mypos.radian + 3.141592 / 2) - g_CenterMAP_Z * sin(mypos.radian + 3.141592 / 2);
		z += g_CenterMAP_X * sin(mypos.radian + 3.141592 / 2) + g_CenterMAP_Z * cos(mypos.radian + 3.141592 / 2);
	}

	//�摜�`��
	pPic->Render(hDC, (int)x, (int)z, lWidth, lHeight, 0, hmWidth, hmHeight, -hmHeight, NULL);


	//Mappu0.40�ɂĒǉ�
	//RGB(1,1,1)��RGB(0,0,0)�ɕϊ�����
	if (g_Menu.transparent && g_ColorTransparent == RGB(0, 0, 0)) {
		//���m�N���쐬�@�w�i�F�����ƂȂ�
		COLORREF defaultBkColor = SetBkColor(hDC, RGB(1, 1, 1));
		HDC hMonoDC = CreateCompatibleDC(hDC);
		HBITMAP hMonoBmp = CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 1, 1, NULL);
		SelectObject(hMonoDC, hMonoBmp);
		BitBlt(hMonoDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hDC, 0, 0, SRCCOPY);
		DeleteObject(hMonoBmp);
		//�w�i�F��߂�
		SetBkColor(hDC, defaultBkColor);

		//�F�t���@NOTSRCVOPY�Ŕw�i�F�����Ƃ���
		HDC hColorDC = CreateCompatibleDC(hDC);
		HBITMAP hColorBmp = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hColorDC, hColorBmp);
		DeleteObject(hColorBmp);
		BitBlt(hColorDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMonoDC, 0, 0, NOTSRCCOPY);

		//�����@����0�Ȃ̂�AND�����Ƃ��ɕ`�悵�Ȃ�
		BitBlt(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hColorDC, 0, 0, SRCAND);
		DeleteDC(hMonoDC);
		DeleteDC(hColorDC);
	}

	//�摜���]
	if (g_Menu.image_reverse) {
		BitBlt(hDC, (int)x, (int)z, lWidth, lHeight, hDC, (int)x, (int)z, NOTSRCCOPY);
	}

	//���[�_�[�\��
	if (g_Menu.radar) {
		float fx, fz;
		int x, z;
		//��]
		POINT pntRotate[3];
		pntRotate[0].x = -(rect.right - rect.left) / 2;
		pntRotate[0].y = -(rect.bottom - rect.top) / 2;
		pntRotate[1].x = +(rect.right - rect.left) / 2;
		pntRotate[1].y = -(rect.bottom - rect.top) / 2;
		pntRotate[2].x = -(rect.right - rect.left) / 2;
		pntRotate[2].y = +(rect.bottom - rect.top) / 2;

		for (int i = 0; i<3; i++) {
			//int�ϊ��A�����␳
			fx = (float)(pntRotate[i].x);
			fz = (float)(pntRotate[i].y);

			//���W�ϊ�(��])
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

//�ߋ��̎����̈ʒu���������� 0.55�Œǉ�
void InitOwnPos()
{
	for (int i = 0; i<sizeof(g_OwnPos) / sizeof(POSXYZR); i++) {
		g_OwnPos[0].x = -10000;
		g_OwnPos[0].y = -10000;
		g_OwnPos[0].z = -10000;
		g_OwnPos[0].radian = -10000;
	}
}

//�����̈ʒu����ۑ� 0.55�Œǉ�
void SaveOwnPos(INFORMATION myPos)
{
	static DWORD dwSaveTime = 0;
	DWORD dwNowTime = timeGetTime();

	if (dwNowTime - dwSaveTime >= 3 * 1000) {//3�b���ƂɎ����̈ʒu��ۑ�
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

//�ߋ��̎����̈ʒu�����o�� 0.55�Œǉ�
POSXYZR LoadOwnPos(int i)
{
	return g_OwnPos[i];
}

/// <summary>
/// �����̈ʒu���擾
/// </summary>
/// <returns></returns>
INFORMATION GetMyInfo()
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[NOWSTA_SIZE];
	WORD change_id = 0;

	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

	//���������[�h
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
/// �^�[�Q�b�g���Ă���NPC/PC�̏��
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
/// NPC���擾
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
INFORMATION GetNPCInfo(int id)
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[NPC_MEM_SIZE];

	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

	//���������[�h
	dwAddress = g_DllAddress + g_Offset.NpcMap + id * sizeof(DWORD);
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(dwAddress), NULL)) {
		info.code = -1;
		return info;
	}
	if (dwAddress != 0) {
		//���A�h���X�����݂��鎞�̏���
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
		info.name[NPCLIST_ID] = _T('\0');//�ő啶����΍�
		memcpy(&info.viewAddress, &dump[NPC_MEM.ViewAddress], sizeof(DWORD));//0.47�ŕύX
		info.npctype = dump[NPC_MEM.NpcType];
		info.view = dump[NPC_MEM.View];
		info.hpp = dump[NPC_MEM.Hpp];
		info.state = dump[NPC_MEM.State];
		info.inrange = dump[NPC_MEM.Inrange];
		info.id = (WORD)id;
		memcpy(&(info.fixedid), &dump[NPC_MEM.FixedId], sizeof(DWORD));//0.46�Œǉ�
		memcpy(&(info.lasthateid), &dump[NPC_MEM.LastHateId], sizeof(DWORD));//0.46�Œǉ�
		memcpy(&(info.distance_2), &dump[NPC_MEM.Distance], sizeof(float));//0.57�Œǉ�
	}
	else {
		//dwAddress�ɃA�h���X�������ꍇ
		info.code = -3;
	}

	return info;
}

/// <summary>
/// PARTY���擾
/// </summary>
/// <param name="pos"></param>
/// <returns></returns>
INFORMATION GetPartyInfo(int pos)
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[0x210];

	//�\���̏�����
	ZeroMemory(&info, sizeof(info));

	//���������[�h
	dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI + pos * NOWSTA_SIZE;//�X�^�[�g�ʒu�̕␳NOWSTA_HOSEI
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
		info.code = -1;
		return info;
	}

	//NOWSTA_MEM_PARTY��0�̎����łɒE��
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
/// �G���AID���擾����
/// </summary>
/// <returns></returns>
WORD GetAreaID()
{
	DWORD dwAddress, dwStartAddress;
	DWORD dwFixedID;//�Œ�ID
	WORD wNPCID;//0.53�ŕϐ������C��
	WORD AreaID = 0;


	if (g_Offset.NowSta != 0) {
		//PC_Status����G���AID���擾

		BYTE dump[NOWSTA_SIZE];

		//���������[�h
		dwAddress = g_DllAddress + g_Offset.NowSta + NOWSTA_HOSEI;

		if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, sizeof(dump), NULL)) {
			AreaID = 0;
			return AreaID;
		}

		memcpy(&AreaID, &dump[NOWSTA_MEM_AREAID], sizeof(WORD));

	}
	else {//NPCLIST�̓G�Œ�ID����G���AID���擾����
		BYTE dump[NPCLIST_SIZE];
		dwAddress = g_DllAddress + g_Offset.NpcList;
		//NPCLIST�̃|�C���^���擾����dwStartAddress�Ɋi�[
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

//�V����擾
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


//������szBuf���̕���ch�̐擪�ʒu���T�[�`
//������Ȃ������ꍇ0
int SearchChar(_TCHAR *szBuf, _TCHAR ch)
{
	int i = 0;

	while (szBuf[i] != 0) {
		if (szBuf[i] == ch)return i + 1;
		i++;
	}

	return 0;
}
