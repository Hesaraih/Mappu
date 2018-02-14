#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//0.36で追加

//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "memorymap.h"
#include "searchaddr.h"

extern FFXIOFFSET g_Offset;


int GetOffsetAddressAll(HANDLE hProcess, DWORD dwDllAddress)
{
	int ret = 0;
	g_Offset.NpcMap = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constNpcmapPattern);
	g_Offset.Map = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constMapPattern);
	g_Offset.NowSta = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constNowstaPattern);//最後に補正する
	g_Offset.Recast = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constRecastPattern);
	g_Offset.Cast = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constCastPattern);
	g_Offset.ViewPoint = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constVeiwstylePattern);
	g_Offset.NpcList = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constNpclistPattern);
	g_Offset.ActivityInfo = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constActivityinfoPattern);
	g_Offset.Target = GetOffsetAddress(hProcess, dwDllAddress, (const BYTE*)constTargetPattern);

	//ビットフラグ
	if (g_Offset.NpcMap == 0) {
		ret |= 0x01;
	}
	if (g_Offset.Map == 0) {
		ret |= 0x01;
	}
	if (g_Offset.NowSta == 0) {
		ret |= 0x02;
	}
	else {//NOWSTAの補正
		g_Offset.NowSta -= NOWSTA_HOSEI;
	}
	if (g_Offset.Recast == 0) {
		ret |= 0x04;
	}
	if (g_Offset.Cast == 0) {
		ret |= 0x08;
	}
	if (g_Offset.ViewPoint == 0) {
		ret |= 0x10;
	}
	if (g_Offset.NpcList == 0) {
		ret |= 0x20;
	}
	if (g_Offset.ActivityInfo == 0) {
		ret |= 0x40;
	}
	else {//Activityinfoの補正
		g_Offset.ActivityInfo += ACTIVITY_INFO_HOSEI;
	}
	if (g_Offset.Target == 0) {
		ret |= 0x80;
	}

	return ret;
}

DWORD GetOffsetAddress(HANDLE hProcess, DWORD dwDllAddress, const BYTE* pattern)
{
	BYTE *ptr = 0;
	DWORD dwAddress;
	DWORD dwOffset = 0;
	BYTE buf[SEARCH_BLOCK + PATTERN_MAX_RANGE];

	for (int i = 0; i < SEARCH_RANGE / SEARCH_BLOCK; i++) {
		dwAddress = dwDllAddress + SEARCH_BLOCK * i;
		if (0 == ReadProcessMemory(hProcess, (LPCVOID)dwAddress, buf, SEARCH_BLOCK + PATTERN_MAX_RANGE, NULL)) {
			break;
		}

		ptr = (BYTE*)SearchAddress(buf, pattern);
		if (ptr) {
			memcpy(&dwOffset, (const void*)&buf[ptr - buf], sizeof(dwOffset));
			dwOffset -= dwDllAddress;
			break;
		}
	}

	return dwOffset;
}


// パターン検索
DWORD SearchAddress(BYTE *base, const BYTE *pattern)
{
	BYTE	dat = 0, ptrn[256], mask[256], *p1, *p2;
	char	c, toggle = 0;
	int		n, cnt = 0, fp;
	DWORD	dwAddress = 0;

	//検索パターン展開
	ZeroMemory(ptrn, sizeof(ptrn));
	ZeroMemory(mask, sizeof(mask));

	toggle = 0;
	cnt = 0;

	for (n = 0; n < 100; n++) {
		c = (char)tolower(pattern[n]);
		if (c == ' ') {
			continue;
		}
		if (c == 0) {
			mask[cnt] = 1;
			break;
		}
		if (c == '.') {
			mask[cnt] = 1;		// マスクパターン
		}
		else if (c == 'x') {
			mask[cnt] = 2;		// アドレス部
		}
		//else if(c == 'p'){
		//	mask[cnt] = 3;		// オフセット部
		//}
		else if ('0' <= c && c <= '9') {
			dat |= (c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			dat |= (c - 'a' + 10);
		}
		else {
			return 0;
		}

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		}
		else {
			dat <<= 4;
		}
		toggle ^= 1;
	}

	// 最初に有効なパターンの位置を検索(.を飛ばす)
	for (n = 0; n < cnt; n++) {
		if (mask[n] == 0)break;
	}

	fp = n;

	for (p1 = base; p1 < base + SEARCH_BLOCK; p1++) {
		if (*p1 != ptrn[fp]) {
			continue;
		}
		p2 = p1 + 1;
		for (n = fp + 1; n<cnt; n++, p2++) {
			if (mask[n]) {
				continue;
			}
			else if (*p2 != ptrn[n]) {
				break;
			}
		}
		if (n == cnt) {
			for (n = 0; n < cnt; n++) {
				if (mask[n] == 2) {
					dwAddress = (DWORD)(p1 - fp + n);
					break;
				}
			}
			break;
		}
	}

	return dwAddress;
}