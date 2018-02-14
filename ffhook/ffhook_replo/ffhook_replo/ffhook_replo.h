#pragma once

#define EXPORT extern "C" __declspec(dllexport)


// FFのプロセスIDを返す
// no			no番目に見つかるプロセス(0以上)
// 戻り値		成功するとプロセスID、失敗時は0
EXPORT DWORD WINAPI FF11_GetProcessId(int no);


// FFXiMain.dllのベースアドレスを取得
// pid			pol.exeのプロセスID
// 戻り値		ベースアドレス。失敗時はNULL
EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD pid);


// dllをpol.exeに注入する
// pid			ターゲットPOL.exeのプロセスID
// 戻り値		成功すると確保したindex値(1以上)を返す。
//				FFXiMain.dllが未ロード時は0、その他の要因で失敗は-1を返す
//				以降このindex値を使ってdllと通信を行う
EXPORT int WINAPI FF11_Inject(DWORD pid);


// dllをpol.exeから取り外す
// index		FFInjectの戻り値。index = 0で全イジェクト
// 戻り値		まだ使用中のindex数
EXPORT int WINAPI FF11_Eject(int index);


// 各アドレスを取得
// index		FFInjectの戻り値
// object		取得するアドレス
// 戻り値		成功時はアドレスポインタ、失敗時はNULL
EXPORT PBYTE WINAPI FF11_GetAddressA(int index, int object, char *name);
EXPORT PBYTE WINAPI FF11_GetAddressW(int index, int object, wchar_t *name);
EXPORT PBYTE WINAPI FF11_SetAddress(int index, int object, DWORD address);

// アドレスobject一覧
#define FF11ADDR_BASE			0
#define FF11ADDR_JOBINFO		1
#define FF11ADDR_PTINFO			2
#define FF11ADDR_NPCMAP			3
#define FF11ADDR_TARGETINFO		4
#define FF11ADDR_ABIITABLE		5
#define FF11ADDR_MAGICTABLE		6
#define FF11ADDR_AOMAHOLD		7
#define FF11ADDR_WEATHER		8
#define FF11ADDR_LSNAME			9
#define FF11ADDR_ICONTABLE		10
#define FF11ADDR_MERIPO			11
#define FF11ADDR_ACTION			12
#define FF11ADDR_TARGETITEM		13
#define FF11ADDR_FLIST			14
#define FF11ADDR_LOGIN			15
#define FF11ADDR_NUMBAG			16
#define FF11ADDR_ITEMBASE		17
#define FF11ADDR_EQUIPTABLE		18
#define FF11ADDR_ITEMTABLE		19
#define FF11ADDR_ISAREA			20
#define FF11ADDR_MAXBAG			21
#define FF11ADDR_OWNPOS			22
#define FF11ADDR_CHOOSE			23
#define FF11ADDR_CAST			24
#define FF11ADDR_PETID			25
#define FF11ADDR_LOCKON			26
#define FF11ADDR_ENDPOINT		27


// インジェクト完了フラグのチェック
// index		FFInjectの戻り値
// 戻り値		各機能の準備完了状態を返す。準備完了時はビットON
EXPORT int WINAPI FF11_Rdy(int index);

// 戻り値のBit
#define FF11_Rdy_GetLog			0x0001
#define FF11_Rdy_GetCmd			0x0002
#define FF11_Rdy_PutCmd			0x0004
#define FF11_Rdy_KillEffect		0x0010
#define FF11_Rdy_MergeItem		0x0020
#define FF11_Rdy_Macro			0x0040	// 未実装
#define FF11_Rdy_GetReplo		0x0100


// FF11のログを取得する
// index		FFInjectの戻り値
// attr			アトリビュート文字列（サイズは53必要）
// data			ログ文字列
// data_size	ログ文字列のサイズ。このサイズ以降はカットされる
// time			ログ出力時間
// 戻り値		ログ文字列長。準備できてない時は-1を返す
EXPORT int WINAPI FF11_GetReploA(int index, char *attr, char *data, int data_size, time_t *time);

// FF11のログを取得する(UNICODE版)
EXPORT int WINAPI FF11_GetReploW(int index, wchar_t *attr, wchar_t *data, int data_size, time_t *time);


// ログインしているキャラクターネームを取得する
// index		FFInjectの戻り値
// name			キャラクターネーム
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetPCnameA(int index, char *name);

EXPORT BOOL WINAPI FF11_GetPCnameW(int index, wchar_t *name);


#ifdef UNICODE
#define FF11_GetAddress		FF11_GetAddressW
#define FF11_GetReplo		FF11_GetReploW
#define FF11_GetPCname		FF11_GetPCnameW
#else
#define FF11_GetAddress		FF11_GetAddressA
#define FF11_GetReplo		FF11_GetReploA
#define FF11_GetPCname		FF11_GetPCnameA
#endif
