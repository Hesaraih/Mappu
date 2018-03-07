#pragma once

#define EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif


	// FFのプロセスIDを返す
	// no			no番目に見つかるプロセス(0以上)
	// 戻り値		成功するとプロセスID、失敗時は0
	EXPORT DWORD WINAPI _stdcall FF11_GetProcessId(int no);


	// FFXiMain.dllのベースアドレスを取得
	// pid			pol.exeのプロセスID
	// 戻り値		ベースアドレス。失敗時はNULL
	EXPORT PBYTE WINAPI _stdcall FF11_GetBaseAddress(DWORD pid);


	// dllをpol.exeに注入する
	// pid			ターゲットPOL.exeのプロセスID
	// 戻り値		成功すると確保したindex値(1以上)を返す。
	//				FFXiMain.dllが未ロード時は0、その他の要因で失敗は-1を返す
	//				以降このindex値を使ってdllと通信を行う
	EXPORT int WINAPI _stdcall FF11_Inject(DWORD pid);


	// dllをpol.exeから取り外す
	// index		FFInjectの戻り値。index = 0で全イジェクト
	// 戻り値		まだ使用中のindex数
	EXPORT int WINAPI _stdcall FF11_Eject(int index);


	// インジェクト完了フラグのチェック
	// index		FFInjectの戻り値
	// 戻り値		各機能の準備完了状態を返す。準備完了時はビットON
	//				FFXiMain.dllが落ちた場合は-1を返す
#define FF11_Rdy_GetLog			0x0001
#define FF11_Rdy_GetCmd			0x0002
#define FF11_Rdy_PutCmd			0x0004
#define FF11_Rdy_KillEffect		0x0010
#define FF11_Rdy_MergeItem		0x0020
	EXPORT int WINAPI _stdcall FF11_Rdy(int index);


	// 各アドレスを取得
	// index		FFInjectの戻り値
	// object		取得するアドレス
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
#define FF11ADDR_NUMBAG			15
#define FF11ADDR_EQUIPTABLE		16
#define FF11ADDR_ITEMTABLE		17
#define FF11ADDR_ISAREA			18
#define FF11ADDR_LOGIN			19
#define FF11ADDR_MAXBAG			20

	// 戻り値		成功時はアドレスポインタ、失敗時はNULL
	EXPORT PBYTE WINAPI _stdcall FF11_GetAddress(int index, int object);


	// FF11のログを取得する
	// index		FFInjectの戻り値
	// attribute	アトリビュート格納バッファ
	// buffer		ログ格納バッファ
	// buffer_size	ログ格納バッファサイズ
	// fcut			ログカットされたログは1がセットされる
	// 戻り値		ログ文字列長。準備できてない時は-1を返す
	EXPORT int WINAPI _stdcall FF11_GetLogA(int index, BYTE *attribute, char *buffer, int buffer_size, BYTE	*fcut);

	// FF11のログを取得する(UNICODE版)
	EXPORT int WINAPI _stdcall FF11_GetLogW(int index, BYTE *attribute, wchar_t *buffer, int buffer_size, BYTE	*fcut);

	// ログカット使用のON/OFF切替え
	// index		FFInjectの戻り値
	// val			0でoff 1でon
	EXPORT BOOL WINAPI _stdcall FF11_LogCut(int index, BYTE val);


	// ログカット文字列をセットする
	// index		FFInjectの戻り値
	// attr			有効アトリビュート、attr = NULLで全アトリビュート
	// str			ログカットする文字列。^を先頭につけると前方一致とする
	// 戻り値		ログカット登録数
	EXPORT int WINAPI _stdcall FF11_SetCutStringA(int index, char *attr, char *str);
	EXPORT int WINAPI _stdcall FF11_SetCutStringW(int index, wchar_t *attr, wchar_t *str);


	// 登録したログカット文字列をクリアする
	// index		FFInjectの戻り値
	// 戻り値		なし
	EXPORT BOOL WINAPI _stdcall FF11_ClrCutString(int index);

	// FF11のコマンドを取得する
	// index		FFInjectの戻り値
	// buffer		コマンドを取得するバッファ
	// buffer_size	bufferのサイズ。このサイズ以降はカットされる
	// 戻り値		コマンド文字列長。準備できてない時は-1を返す
	EXPORT int WINAPI _stdcall FF11_GetCmdA(int index, LPTSTR buffer, int buffer_size);
	EXPORT int WINAPI _stdcall FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size);


	// FF11にコマンド発行
	// index		FFInjectの戻り値
	// buffer		コマンドを格納したバッファ
	// 戻り値		成功時はTRUE、失敗時はFALSEを返す
	EXPORT BOOL WINAPI _stdcall FF11_PutCmdA(int index, char *command);
	EXPORT BOOL WINAPI _stdcall FF11_PutCmdW(int index, wchar_t *command);

#ifdef UNICODE
#define FF11_GetLog			FF11_GetLogW
#define FF11_SetCutString	FF11_SetCutStringW
#define FF11_GetCmd			FF11_GetCmdW
#define FF11_PutCmd			FF11_PutCmdW
#else
#define FF11_GetLog			FF11_GetLogA
#define FF11_SetCutString	FF11_SetCutStringA
#define FF11_GetCmd			FF11_GetCmdA
#define FF11_PutCmd			FF11_PutCmdA
#endif

	EXPORT BOOL WINAPI _stdcall FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int len);

#ifdef __cplusplus
}
#endif
