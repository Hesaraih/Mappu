//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "header.h"
#include "searchaddr.h"

extern FFXIOFFSET g_Offset;
extern NPC_MEM_MAP NPC_MEM;


int GetOffsetAddressAll(HANDLE hProcess, DWORD dwDllAddress)
{
	int ret = 0;

	g_Offset.NpcMap = GetOffsetAddress(hProcess, dwDllAddress, constNpcmapPattern);
	g_Offset.NowSta = GetOffsetAddress(hProcess, dwDllAddress, constNowstaPattern);
	if (g_Offset.NowSta) {
		g_Offset.NowSta -= NOWSTA_HOSEI;
	}
	g_Offset.NpcList = GetOffsetAddress(hProcess, dwDllAddress, constNpclistPattern);
	g_Offset.Target = GetOffsetAddress(hProcess, dwDllAddress, constTargetPattern);
	g_Offset.Weather = GetOffsetAddress(hProcess, dwDllAddress, constWeatherPattern);


	//NPC_MEM 0.05で追加
	NPC_MEM.Radian = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemRadianPattern) & 0xFF);
	NPC_MEM.ChangedId = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemChangedIdPattern) & 0xFF);
	NPC_MEM.FixedId = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemFixedIdPattern) & 0xFF);
	NPC_MEM.Name = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNamePattern) & 0xFF);//0.05aで変更
	NPC_MEM.ViewAddress = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemViewAddressPattern);
	NPC_MEM.Distance = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemDistancePattern);
	NPC_MEM.Hpp = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemHpPattern);
	NPC_MEM.NpcType = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNpcTypePattern);
	if (NPC_MEM.NpcType) {
		NPC_MEM.View = (WORD)(NPC_MEM.NpcType + 1);
	}
	NPC_MEM.Inrange = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemInRangePattern);
	NPC_MEM.State = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemStatePattern);
	NPC_MEM.LastHateId = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemLastHateIdPattern);

	//ビットフラグ 0.03でret++に変更
	if (g_Offset.NpcMap == 0) { ret++; }
	if (g_Offset.NowSta == 0) { ret++; }
	if (g_Offset.NpcList == 0) { ret++; }
	if (g_Offset.Target == 0) { ret++; }
	if (g_Offset.Weather == 0) { ret++; }

	//0.05で追加
	if (NPC_MEM.Radian == 0) { ret++; }
	if (NPC_MEM.ChangedId == 0) { ret++; }
	if (NPC_MEM.FixedId == 0) { ret++; }
	if (NPC_MEM.Name == 0) { ret++; }
	if (NPC_MEM.ViewAddress == 0) { ret++; }
	if (NPC_MEM.Distance == 0) { ret++; }
	if (NPC_MEM.Hpp == 0) { ret++; }
	if (NPC_MEM.NpcType == 0) { ret++; }
	if (NPC_MEM.View == 0) { ret++; }
	if (NPC_MEM.Inrange == 0) { ret++; }
	if (NPC_MEM.State == 0) { ret++; }
	if (NPC_MEM.LastHateId == 0) { ret++; }
	return ret;
}

/// <summary>
/// オフセット取得
/// </summary>
/// <param name="hProcess"></param>
/// <param name="dwDllAddress">FFXiMain.dllのアドレス(ベースアドレス)</param>
/// <param name="pattern">バイトパターン</param>
/// <param name="dwStartAddress">サーチ開始アドレス（未指定時はdwDllAddressと同じ)</param>
/// <param name="dwSearchRange">未使用</param>
/// <returns></returns>
DWORD GetOffsetAddress(HANDLE hProcess, DWORD dwDllAddress, const BYTE* pattern, DWORD dwStartAddress, DWORD dwSearchRange)//0.05で変更
{
	DWORD dwAddress;
	DWORD dwOffset = 0;
	BYTE buf[SEARCH_BLOCK + PATTERN_MAX_RANGE];
	if (dwStartAddress == 0) {
		dwStartAddress = dwDllAddress;//サーチ開始アドレスをFFXiMain.dllのアドレスにする 0.05で追加
	}

	for (int i = 0; i <= (int)SEARCH_RANGE / SEARCH_BLOCK; i++) {//0.05で変更
		dwAddress = dwStartAddress + SEARCH_BLOCK * i;//0.05で変更
		if (0 == ReadProcessMemory(hProcess, (LPCVOID)dwAddress, buf, SEARCH_BLOCK + PATTERN_MAX_RANGE, NULL)) {
			break;
		}

		if ((dwOffset = SearchAddress(buf, pattern, dwAddress, dwDllAddress)) != 0) {
			break;
		}
	}

	return dwOffset;
}

/// <summary>
/// パターン検索
/// </summary>
/// <param name="base"></param>
/// <param name="pattern"></param>
/// <param name="dwReadAddress">開始アドレス</param>
/// <param name="dwDllAddress">FFXiMain.dllのアドレス(ベースアドレス)</param>
/// <returns></returns>
DWORD SearchAddress(BYTE *base, const BYTE *pattern, DWORD dwReadAddress, DWORD dwDllAddress)
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
		if (c == 0) { //文字列の終端コード
			mask[cnt] = 1;
			break;
		}
		if (c == '.') {
			mask[cnt] = 1;		//「.」マスクパターン
		}
		else if (c == 'x') {
			mask[cnt] = 2;		//「x」アドレス部(ベースアドレス+オフセット値)
		}
		else if (c == 'p') {//0.05から利用可
			mask[cnt] = 3;		// 「p」オフセット部
		}
		else if (c == 'z') {
			mask[cnt] = 4;		// 「z」オフセット部
		}
		else if ('0' <= c && c <= '9') {
			dat |= (c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			dat |= (c - 'a' + 10);
		}
		else {
			return false;
		}

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		}
		else {
			dat <<= 4;
		}
		toggle ^= 1; //上位4bit,下位4bitの切り替えフラグ
	}

	// 最初に有効なパターンの位置を検索(.を飛ばす)
	for (n = 0; n < cnt; n++) {
		if (mask[n] == 0) {
			//「.」のときスキップ
			break;
		}
	}

	fp = n;

	for (p1 = base; p1 < base + SEARCH_BLOCK; p1++) {
		if (*p1 != ptrn[fp]) {
			continue;
		}
		p2 = p1 + 1;
		for (n = fp + 1; n < cnt; n++, p2++) {
			if (mask[n]) {
				continue;
			}
			else if (*p2 != ptrn[n]) {
				break;
			}
		}
		if (n == cnt) {
			//一致したFFXiMain.dll先頭からの差分アドレス(xpが存在しないパターンはこの値が使われる)
			dwAddress = (DWORD)(p1 - fp - base) + dwReadAddress - dwDllAddress;
			for (n = 0; n < cnt; n++) {
				if (mask[n] == 2) {//x:アドレス部
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					dwAddress -= dwDllAddress;//こっちでベースアドレスを引く
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//pに対応
				if (mask[n] == 3) {//p:オフセット部
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//zに対応
				if (mask[n] == 4) {//z:byte値
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(BYTE));
					break;
				}
			}
			break;
		}
	}

	return dwAddress;
}