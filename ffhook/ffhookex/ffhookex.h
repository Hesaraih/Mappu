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


// FF11のログを取得する
// index		FFInjectの戻り値
// attribute	アトリビュート格納バッファ
// buffer		ログ格納バッファ
// buffer_size	bufferのサイズ。このサイズ以降はカットされる
// fcut			ログカットされたログは1がセットされる
// 戻り値		ログ文字列長。準備できてない時は-1を返す
EXPORT int WINAPI FF11_GetLogA(int index, BYTE *attribute, char *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ログカットされたログは1が入る
#else
	,BYTE	*fcut;
#endif
);

// FF11のログを取得する(UNICODE版)
EXPORT int WINAPI FF11_GetLogW(int index, BYTE *attribute, wchar_t *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ログカットされたログは1が入る
#else
	,BYTE	*fcut;
#endif
);

// ログカット使用のON/OFF切替え
// index		FFInjectの戻り値
// val			0-ログカットOFF 1-ログカットON
EXPORT BOOL WINAPI FF11_LogCut(int index, BYTE val);


// ログカット文字列をセットする
// index		FFInjectの戻り値
// attr			有効アトリビュート配列、attr = NULLで全アトリビュート
// str			ログカットする文字列。^を先頭につけると前方一致とする
// 戻り値		ログカット登録数
EXPORT int WINAPI FF11_SetCutStringA(int index, char *attr, char *str);
EXPORT int WINAPI FF11_SetCutStringW(int index, wchar_t *attr, wchar_t *str);


// 登録済みのログカット情報をクリアする
// index		FFInjectの戻り値
// 戻り値		なし
EXPORT BOOL WINAPI FF11_ClrCutString(int index);

// iniからログカット文字列を登録
// index		FFInjectの戻り値
// ini			iniファイル名
// 戻り値		なし
EXPORT BOOL WINAPI FF11_LoadLogCutINIA(int index, char *ini);
EXPORT BOOL WINAPI FF11_LoadLogCutINIW(int index, wchar_t *ini);

// FF11のコマンドを取得する
// index		FFInjectの戻り値
// buffer		コマンドを取得するバッファ
// buffer_size	bufferのサイズ。このサイズ以降はカットされる
// 戻り値		コマンド文字列長。準備できてない時は-1を返す
EXPORT int WINAPI FF11_GetCmdA(int index, char *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ログカットされたログは1が入る
#else
	,BYTE	*fcut;
#endif
);
EXPORT int WINAPI FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ログカットされたログは1が入る
#else
	,BYTE	*fcut;
#endif
);


// コマンドカット使用のON/OFF切替え
// index		FFInjectの戻り値
// val			0-コマンドカットOFF 1-コマンドカットON
EXPORT BOOL WINAPI FF11_CmdCut(int index, BYTE val);

// コマンドカット文字列をセットする
// index		FFInjectの戻り値
// str			コマンドカットする文字列。^を先頭につけると前方一致とする
// 戻り値		コマンドカット登録数
EXPORT int WINAPI FF11_SetCutCmdA(int index, char *str);
EXPORT int WINAPI FF11_SetCutCmdW(int index, wchar_t *str);


// 登録済みのコマンドカット情報をクリアする
// index		FFInjectの戻り値
// 戻り値		なし
EXPORT BOOL WINAPI FF11_ClrCutCmd(int index);


// FF11にコマンド発行
// index		FFInjectの戻り値
// command		コマンドを格納したバッファ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_PutCmdA(int index, char *command);
EXPORT BOOL WINAPI FF11_PutCmdW(int index, wchar_t *command);


// KillEffectコマンド
// index		FFInjectの戻り値
// id			消したいアイコンのID
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_KillEffect(int index, int id);


// MergeItemコマンド
// index		FFInjectの戻り値
// object		0-かばん 1-モグ金庫 2-収納 3-モグサッチェル 4-モグサック
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_MergeItem(int index, int object);


// POLプロセス内のメモリリード
// index		FFInjectの戻り値
// ptr			リードするアドレス(絶対アドレス)
// buffer		格納用バッファ
// size			リードサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int size);


// キャラクタ情報読み込み
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetCharInfo(int index, BYTE *buffer, int size);


// PT情報読み込み
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetPTInfo(int index, BYTE *buffer, int size);


// NPCMAP読み込み
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetNPCMAP(int index, BYTE *buffer, int size);


// NPC情報読み込み
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetNPCData(int index, int id, BYTE *buffer, int size);

// ターゲット中のIDを取得
// index		FFInjectの戻り値
// 戻り値		ID。失敗時は0
EXPORT WORD WINAPI FF11_GetTargetId(int index);

// ターゲット中の情報を返す
// index		FFInjectの戻り値
// id			ターゲット中のIDを格納するバッファ
// sid			サブターゲット中のIDを格納するバッファ
// lastst		ラストターゲットの名前を格納するバッファ(24文字以上)
// 戻り値		ID。失敗時は0
EXPORT BOOL WINAPI FF11_GetTargetInfoA(int index, WORD *id, WORD *sid, char *lastst);
EXPORT BOOL WINAPI FF11_GetTargetInfoW(int index, WORD *id, WORD *sid, wchar_t *lastst);


// アビリティの情報取得
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetAbiTable(int index, BYTE *buffer, int size);


// 魔法リキャストの情報取得
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetMagicTable(int index, BYTE *buffer, int size);


// 青魔法セットのペナルティ時間
// index		FFInjectの戻り値
// 戻り値		ペナルティ時間
EXPORT WORD WINAPI FF11_GetAomaHold(int index);


// エリアの天候ID
// index		FFInjectの戻り値
// 戻り値		天候ID
EXPORT BYTE WINAPI FF11_GetWeatherId(int index);


// アイテム欄でターゲット中のパール等のLS名
// index		FFInjectの戻り値
// name			名前格納用バッファ(32文字以上)
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetLSNameA(int index, char *name);
EXPORT BOOL WINAPI FF11_GetLSNameW(int index, wchar_t *name);


// アイコンテーブル情報
// index		FFInjectの戻り値
// buffer		格納用バッファ
// num			取得するアイコンの数(64個くらい?)
// 戻り値		取得したアイコンの数
EXPORT int WINAPI FF11_GetIcon(int index, WORD *buffer, int num);

// メリポ情報
// index		FFInjectの戻り値
// rimipo		リミポ格納用バッファ
// meripo		メリポ格納用バッファ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetMeripo(int index, short *rimipo, char *meripo);


// 現在のアクションID
// index		FFInjectの戻り値
// 戻り値		アクションID
EXPORT WORD WINAPI FF11_GetActionId(int index);


// ターゲット中のアイテム情報の取得
// index		FFInjectの戻り値
// id			アイテムID格納用バッファ
// info			アイテム情報格納用バッファ(4バイト)
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetTargetItem(int index, WORD *id, BYTE *info);


// フレンド情報の取得
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetFList(int index, BYTE *buffer, int size);


// 装備情報テーブルの取得
// index		FFInjectの戻り値
// tequip		格納用バッファ(128バイト)
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetEquipTable(int index, BYTE *tequip);


// かばん内情報テーブルの取得
// index		FFInjectの戻り値
// buffer		格納用バッファ
// size			情報のサイズ
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetItemTable(int index, BYTE *buffer, int size);


// かばん情報
// index		FFInjectの戻り値
// num			持ち物数(ギル分による+1の値)
// max			かばんの容量(ギル分による+1の値)
// 戻り値		成功時はTRUE、失敗時はFALSEを返す
EXPORT BOOL WINAPI FF11_GetBag(int index, BYTE *num, BYTE *max);


// エリアチェンジ中フラグ
// index		FFInjectの戻り値
// 戻り値		TRUE-エリアチェンジ中、FALSE-通常
EXPORT BOOL WINAPI FF11_IsArea(int index);


// POLのHWND
// index		FFInjectの戻り値
// 戻り値		成功時はHWNDの値、失敗時は0
EXPORT HWND WINAPI FF11_GetHWND(int index);


// 選択窓の選択肢の指定
// index		FFInjectの戻り値
// no			番号 1-
// 戻り値		詠唱中-TRUE、通常-FALSE
EXPORT BOOL	WINAPI FF11_SetChoose(int index, int no);

// ロックオン状態の取得
// index		FFInjectの戻り値
// 戻り値		ロック中-TRUE、アンロック-FALSE
EXPORT WORD	WINAPI FF11_GetLockOn(int index);

// ペットIDの取得
// index		FFInjectの戻り値
// 戻り値		ID
EXPORT WORD	WINAPI FF11_GetPetID(int index);

// キャスト情報の取得(未実装)
// index		FFInjectの戻り値
// 戻り値		詠唱中-TRUE、通常-FALSE
EXPORT BOOL	WINAPI FF11_GetCast(int index, DWORD *cast, DWORD *remain, float *gage);



#ifdef UNICODE
#define FF11_GetAddress		FF11_GetAddressW
#define FF11_GetLog			FF11_GetLogW
#define FF11_SetCutString	FF11_SetCutStringW
#define FF11_SetCutCmd		FF11_SetCutCmdW
#define FF11_GetCmd			FF11_GetCmdW
#define FF11_PutCmd			FF11_PutCmdW
#define FF11_LoadLogCutINI	FF11_LoadLogCutINIW
#define FF11_GetLSName		FF11_GetLSNameW
#define FF11_GetTargetInfo	FF11_GetTargetInfoW
#else
#define FF11_GetAddress		FF11_GetAddressA
#define FF11_GetLog			FF11_GetLogA
#define FF11_SetCutString	FF11_SetCutStringA
#define FF11_SetCutCmd		FF11_SetCutCmdA
#define FF11_GetCmd			FF11_GetCmdA
#define FF11_PutCmd			FF11_PutCmdA
#define FF11_LoadLogCutINI	FF11_LoadLogCutINIA
#define FF11_GetLSName		FF11_GetLSNameA
#define FF11_GetTargetInfo	FF11_GetTargetInfoA
#endif
