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
#include "ffxiinfo.h"



//自分の位置を取得
INFORMATION GetMyInfo()
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[0x40];
	WORD change_id = 0;

	//構造体初期化
	ZeroMemory(&info,sizeof(info));

	//メモリリード
	dwAddress = g_DllAddress + g_OffsetNowSta + NOWSTA_HOSEI;
	if( 0 == ReadProcessMemory(g_polHandle,(const void *)dwAddress,dump,sizeof(dump),NULL)){
		info.code = -1;
		return info;
	}

	memcpy(&change_id,&dump[NOWSTA_MEM_CHANGEID],sizeof(WORD));

	info = GetNPCInfo((int)change_id);

	return info;
}


//NPC情報取得
INFORMATION GetNPCInfo(int id)
{
	DWORD dwAddress;
	INFORMATION info;
	char dump[NPC_MEM_SIZE];

	//構造体初期化
	ZeroMemory(&info,sizeof(info));
	
	//メモリリード
	dwAddress = g_DllAddress + g_OffsetNpcMap + id * sizeof(DWORD);
	if(0 == ReadProcessMemory(g_polHandle,(const void *)dwAddress,&dwAddress,sizeof(dwAddress),NULL)){
		info.code = -1;
		return info;
	}
	if(dwAddress != 0){
		//情報アドレスが存在する時の処理
		if(0 == ReadProcessMemory(g_polHandle,(const void *)dwAddress,dump,sizeof(dump),NULL)){
			info.code = -2;
			return info;
		}
		memcpy(&(info.x),&dump[NPC_MEM.POS_X],sizeof(float));
		//memcpy(&(info.y),&dump[NPC_MEM.POS_Y],sizeof(float));//0.09で削除
		memcpy(&(info.z),&dump[NPC_MEM.POS_Z],sizeof(float));
		mbstowcs(info.name,&dump[NPC_MEM.Name],sizeof(info.name)/sizeof(_TCHAR)-1);

		info.name[0x18] = _T('\0');//最大文字列対策
		info.inrange = dump[NPC_MEM.Inrange];
		info.id = (WORD)id;
		//memcpy(&(info.distance_2),&dump[NPC_MEM.DISTANCE],sizeof(float));0.09で削除
		memcpy(&(info.bazaar),&dump[NPC_MEM.Bazaar],sizeof(BYTE));

#ifdef DEBUG
		WORD change_id = 0;

		dwAddress = g_DllAddress + g_OffsetNowSta + NOWSTA_HOSEI;
		if( 0 == ReadProcessMemory(g_PolHandle,(const void *)dwAddress,dump,sizeof(dump),NULL)){
			info.code = -1;
		}

		memcpy(&change_id,&dump[NOWSTA_MEM_CHANGEID],sizeof(WORD));
		if(change_id == info.id){
			info.inrange = 1;
		}
#endif

	}
	else{
		//dwAddressにアドレスが無い場合
		info.code = -3;
	}

	return info;
}
