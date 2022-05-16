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
#include <tchar.h>
#include "main.h"
#include "bazaar.h"


int BazaarWaitUpdate(int id)
{
	DWORD dwAddress;
	WORD get_id;

	if (id < 0x400 || id > 0x700) {
		return -1;
	}

	//メモリリード
	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x14;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &get_id, sizeof(WORD), NULL)) {
		return -1;
	}

	if (id != get_id) {
		return 0;
	}
	else if (BazaarGetItemCount() < 1) {
		return 0;
	}
	else {
		for (int i = BazaarGetItemCount(); i > 0; i--) {
			if (wcslen(BazaarGetItemName(i - 1)) == 0) {//データも読み込んでみる
				return 0;
			}
		}
	}

	return 1;
}

/// <summary>
/// バザーの品数を取得
/// </summary>
WORD BazaarGetItemCount()
{
	DWORD dwAddress;
	WORD wCount;

	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x4;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &wCount, sizeof(WORD), NULL)) {
		return -1;
	}

	return wCount;
}


//nNum番目のアイテム名を取得 nNum=0～79
_TCHAR* BazaarGetItemName(int nNum)
{
	DWORD dwAddress;
	char szName[0x30];
	static _TCHAR szTRet[0x30];

	ZeroMemory(szTRet, sizeof(szTRet));

	if (nNum < 0 || nNum >= 80) {
		return szTRet;
	}

	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	dwAddress += 0x8;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	dwAddress += 0x2C + 0x50 * nNum;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	if (dwAddress == 0)return szTRet;

	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, szName, sizeof(szName), NULL)) {
		return szTRet;
	}

	szName[sizeof(szName) - 1] = '\0';
	if (mbstowcs(szTRet, szName, sizeof(szName) - 1) == -1) {
		_swprintf(szTRet, _T("Error - mbstowcs失敗"));
	}

	szTRet[sizeof(szTRet) / sizeof(_TCHAR) - 1] = 0x00;//安全策

	return szTRet;
}



//nNum番目のアイテムの備考を取得 nNum=0～79
_TCHAR* BazaarGetItemNote(int nNum)
{
	DWORD dwAddress;
	char szBuf[0x130];
	char szComment[0x100];
	static _TCHAR szTRet[0x100];

	ZeroMemory(szTRet, sizeof(szTRet));

	if (nNum < 0 || nNum >= 80) {
		return szTRet;
	}

	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	dwAddress += 0x8;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	if (dwAddress == 0)return szTRet;

	dwAddress += 0x2C + 0x50 * nNum;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return szTRet;
	}

	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, szBuf, sizeof(szBuf), NULL)) {
		return szTRet;
	}


	szBuf[sizeof(szBuf) - 1] = '\0';

	switch ((strlen(szBuf) + 1) % 4) {
	case 0:
		strncpy(szComment, &szBuf[0x1C + strlen(szBuf) + 1], sizeof(szComment) - 1);
		break;
	case 1:
		strncpy(szComment, &szBuf[0x1C + strlen(szBuf) + 1 + 3], sizeof(szComment) - 1);
		break;
	case 2:
		strncpy(szComment, &szBuf[0x1C + strlen(szBuf) + 1 + 2], sizeof(szComment) - 1);
		break;
	case 3:
		strcpy(szComment, &szBuf[0x1C + strlen(szBuf) + 1 + 1]);
		break;
	}

	if (mbstowcs(szTRet, szComment, sizeof(szComment) - 1) == -1) {
		_swprintf(szTRet, _T("Error - mbstowcs失敗"));
	}

	szTRet[sizeof(szTRet) / sizeof(_TCHAR) - 1] = 0x00;//安全策

	return szTRet;
}


//nNum番目の金額を取得
int BazaarGetItemPrice(int nNum)
{
	DWORD dwAddress;
	int nValue;

	if (nNum < 0 || nNum >= 80) {
		return -1;
	}

	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x8;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x28 + 0x50 * nNum;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x0C;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &nValue, sizeof(int), NULL)) {
		return -1;
	}

	return nValue;
}


//nNum番目の個数を取得
char BazaarGetItemNumber(int nNum)
{
	DWORD dwAddress;
	char cNumber;

	if (nNum < 0 || nNum >= 80) {
		return -1;
	}

	dwAddress = g_DllAddress + g_OffsetBazaar;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x8;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x28 + 0x50 * nNum;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress += 0x4;
	if (0 == ReadProcessMemory(g_polHandle, (const void*)dwAddress, &cNumber, sizeof(char), NULL)) {
		return -1;
	}


	return cNumber;
}