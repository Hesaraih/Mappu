#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <process.h>
#include "resource.h"

// デバッグログはコマンドバッファと共通。

// コンフィグ
#define LOG_HOOK		1				// ログをフックする(0-OFF/1-ON)
#define LOG_DECODE		1				// ログの不要な制御コード排除(0-OFF/1-ON)
#define LOG_CUT			1				// ログカット機能(0-OFF/1-ON)

#define CMD_HOOK		1				// コマンドをフックする(0-OFF/1-ON)
#define CMD_CUT			1				// コマンドカット機能(0-OFF/1-ON)
#define	CMD_PUT			1				// 外部からコマンド実行(0-OFF/1-ON)

#define KILLEFFECT		1				// killeffectを使用する(0-OFF/1-ON)
#define MERGEITEM		1				// アイテム整頓を使用する(0-OFF/1-ON)

#define DBG_DLL			0				// デバッグ(0-OFF/1-ON)

#define MAX_PROCESS		6				// フックできるPOLの数
#define MAX_INJECT		12				// 1つのPOLにインジェクトできる数

#define MAX_INDEX		(MAX_PROCESS*MAX_INJECT+1)

#define SIZE_LOG_FIFO	1000			// ログFIFO数
#define LEN_LOG_BUFFER	1000			// ログ文字列長
#define MAX_LOGCUT		300				// 1つのPOLプロセスに登録できるログカット情報数

#define SIZE_CMD_FIFO	100				// コマンドFIFO数
#define LEN_CMD_BUFFER	100				// コマンド文字列長

#define LEN_AM_BUFFER	32				// アビ魔法名の最大長
#define MAX_AMCUT		600				// 1つのPOLプロセスに登録できるアビ魔法カット情報数

#define SEARCH_RANGE	0x300000		// 自動オフセのbaseからの検索範囲

// 定数
#define CMD_CUT_FMACRO	0x01			// マクロ、コマンドラインからのコマンド
#define CMD_CUT_FMENU	0x02			// メニューからのコマンド

#define C_CMD			(WM_USER + 1)
#define C_KILLEFFECT	(WM_USER + 2)
#define C_MERGEITEM		(WM_USER + 3)
#define C_MACROEXE		(WM_USER + 4)
#define C_END			(WM_USER + 9)

#include "ffhookex.h"
#include "madChook.h"

#pragma comment(lib, "madCHook")

#define POL_NAME		_T("pol.exe")
#define DLL_NAME		_T("FFXiMain.dll")
#define MAP_NAME		_T("FFHOOK_MAP_%d")		// マップファイル名(pid)
#define EVENT_NAME		_T("FFHOOK_EVENT_%d")	// イベント名(pid)
#define MTX_NAME		_T("FFHOOK_MTX_%d")		// Mutex名(pid)

// デバッグ用
#if DBG_DLL == 1
#define	SLwsprintfA		wsprintfA
#else
#define	SLwsprintfA
#define syslog
#endif

#if CMD_PUT == 1 || KILLEFFECT == 1 || MERGEITEM == 1
#define USE_EXCHANGE
#endif

#if LOG_HOOK == 1 || CMD_HOOK == 1 || CMD_PUT == 1
#define USE_HOOK
#endif

// ログFIFO
typedef struct {
	BYTE	tAttr;						// アトリビュート格納用
	BYTE	fCut;						// カットされたログはTRUE
	char	Buffer[LEN_LOG_BUFFER];		// ログバッファ
	wchar_t	BufferW[LEN_LOG_BUFFER];	// ログバッファ
} TLOG;

// ログカット情報
typedef struct _tcut {
	BYTE	tAttr[256];					// アトリビュートテーブル
	int		nLen;						// ログカット文字列長
	char	Buffer[LEN_LOG_BUFFER];		// ログカット文字列
} TCUT, *PTCUT;

// コマンドFIFO
typedef struct {
	BYTE	fCut;						// カットされたコマンドはTRUE
	char	Buffer[LEN_CMD_BUFFER];
	wchar_t	BufferW[LEN_CMD_BUFFER];
} TCMD;

// アビ魔法カット情報
typedef struct _tcutcmd {
	int		nLen;						// アビ魔法文字列長
	char	Buffer[LEN_AM_BUFFER];		// アビ魔法文字列
	int		fPass;						// 省略時1
} TCUTAM, *PTCUTAM;

// 共有メモリ情報テーブル
// FIFOバッファはプロセスに１個とし、読込みポインタだけ独立させる。
// 読み遅れると古いログから消される。
// 各アプリ間で整合性を保つために各機能未使用時でもテーブルは固定にする。
typedef struct {
	BOOL	fLD_madC;				// madChook.dllをpol.exeにインジェクトしたか？(TRUE)
	BOOL	fLD_hook;				// このdllをpol.exeにインジェクトしたか？(TRUE)

	DWORD	fRdy;					// 各種準備完了フラグ
	PBYTE	pBase;					// FFXiMain.dllベースアドレス
	int		nInject;				// このプロセスで使用中のindex数
	BYTE	fEna[MAX_INJECT];		// 使用中のテーブルは1になる

	int		pWriteLog;				// ログFIFO 書込みポインタ
	int		pReadLog[MAX_INJECT];	// ログFIFO 読込みポインタ(テーブル毎に独立)
	TLOG	tLog[SIZE_LOG_FIFO];	// ログバッファ

	BYTE	fCut;					// ログカットのON/OFF
	int		nCut;					// ログカット情報登録数
	TCUT	tCut[MAX_LOGCUT];		// ログカット情報

	int		pWriteCmd;				// コマンドFIFO 書込みポインタ
	int		pReadCmd[MAX_INJECT];	// コマンドFIFO 読込みポインタ(テーブル毎に独立)
	TCMD	tCmd[SIZE_CMD_FIFO];	// コマンドバッファ

	HANDLE			hThread;		// スレッドハンドル用
	unsigned int	idThread;		// スレッドID
	char	nPrm[LEN_LOG_BUFFER];	// コマンド依存パラメータ用

	HWND	hWnd;					// FF窓のハンドル
	PBYTE	tPtr[50];				// 各アドレス格納用(FF11_GetAddressで使用)

	BYTE	fCutAM;					// アビ魔法カットのON/OFF
	int		nCutAM;					// アビ魔法カット情報登録数
	TCUTAM	tCutAM[MAX_AMCUT];		// アビ魔法カット情報
} TDATA, *PTDATA;

// プロセス別管理テーブル
typedef struct {
	int		idPro;					// プロセスID
	HANDLE	hPro;					// プロセスハンドル
	HANDLE	hMap;					// 共有メモリマップハンドル
	HANDLE	hMtx;					// Mutexハンドル
	HANDLE	hEvent;					// イベント用
	PTDATA	pData;					// 共有メモリ情報テーブルへのポインタ
	HWND	hWnd;					// FF11窓ハンドル
} TPROC, *PTPROC;

// インデックス管理テーブル(index=0は使わない事にした)
typedef struct {
	PTPROC	pProc;					// ターゲットプロセス
	int		pInject;				// インジェクト番号
} TINDEX;

// アタッチ情報
typedef struct _ihook_ {
	BOOL	fAttach;				// アタッチしたらTRUE
	PBYTE	pCode;					// アタッチする命令があるアドレス
	PBYTE	pHook;					// フックアドレス
	PBYTE	pNext;					// 飛び先
	BYTE	func[12];
} IHOOK, *PIHOOK;

typedef struct _tprocess {
	TCHAR	sName[256];
	int		nPro;
} TPLIST;

// アプリ側で使用する
static HINSTANCE	hInst;
static TPROC	tProcess[MAX_PROCESS];	// プロセス管理テーブル
static TINDEX	tIndex[MAX_INDEX];		// インデックス管理テーブル
static int		nIndex = 0;				// 使用中インデックス数
static TPLIST	tPList[MAX_PROCESS];

// POLにインジェクトされたDLL側で使用する
static BOOL		fPol = FALSE;		// POL側の時TRUEになる
static int		idPro = 0;			// プロセスID
static HANDLE	hMap = 0;			// 共有メモリのハンドル
static HANDLE	hMtx = 0;			// 排他処理
static HANDLE	hEvent = 0;			// イベント用
static PTDATA	pData = 0;			// 共有メモリ情報テーブルへのポインタ

#include "pattern.h"

LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

// -----------------------------------------------------
// POL側で使用する関数
// -----------------------------------------------------
#if LOG_HOOK == 1 || DBG_DLL == 1

// ログをFIFOに書込む
// attribute	アトリビュート
// str			ログ本文
BOOL hook_log(BYTE *attr, char *str)
{
	int		n, wp, rp, inject;
	BOOL	res = TRUE;

	if (hMtx == 0 || str == NULL)	return res;

	WaitForSingleObject(hMtx, INFINITE);
	wp = pData->pWriteLog;

#if LOG_DECODE == 1
	{
		char	*ptr;
		ptr = pData->tLog[wp].Buffer;
		for (n = 0; n < LEN_LOG_BUFFER-1 && *str; n++) {
			if (*str == 0x1f || *str == 0x7f) {
				// 制御文字なら2文字分スキップ
				if (*(++str) != 0) {
					str++;
					n++;
					continue;
				}
			} else if (*(unsigned char*)str == 0xef) {
				// タブ変換文字列
				if (*(str+1) == 0x27 || *(str+1) == 0x28) {
					*ptr++ = 0x1e;
					str++;
					n++;
				}
			}
			*ptr++ = *str++;
		}
		*ptr = 0;
	}
#else
	strncpy_s(pData->tLog[wp].Buffer, LEN_LOG_BUFFER-1, str, LEN_LOG_BUFFER-2);
#endif
	pData->tLog[wp].fCut = 0;
	pData->tLog[wp].BufferW[0] = 0;
	pData->tLog[wp].tAttr = (attr) ? *attr : 0;

#if LOG_CUT == 1
	if (pData->fCut) {
		if (attr && pData->nCut) {
			char	*ptr;
			PTCUT	p = pData->tCut;

			ptr = pData->tLog[wp].Buffer;
			for (n = 0; n < pData->nCut; n++, p++) {
				if (p->tAttr[*attr] == 0)	continue;	// アトリビュート不一致

				if (p->Buffer[0] == 0) {
					// 本文なし
					pData->tLog[wp].fCut = 1;
					res = FALSE;
					break;
				} else if (p->Buffer[0] == '^') {
					// 前方一致
					if (!strncmp(ptr, &p->Buffer[1], p->nLen)) {
						pData->tLog[wp].fCut = 1;
						res = FALSE;
						break;
					}
				} else {
					if (strstr(ptr, p->Buffer)) {
						pData->tLog[wp].fCut = 1;
						res = FALSE;
						break;
					}
				}
			}
		}
	}
#endif

	if (++wp >= SIZE_LOG_FIFO)
		wp = 0;

	pData->pWriteLog = wp;

	rp = (wp == SIZE_LOG_FIFO-1) ? 0 : wp+1;
	inject = pData->nInject;

	// ライトポインタがリードポインタに追いついていたらリードポインタをインクリメント
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		if (pData->fEna[n]) {
			// 有効なテーブルだけチェックする
			if (pData->pReadLog[n] == wp)
				pData->pReadLog[n] = rp;
			inject--;
		}
	}

	ReleaseMutex(hMtx);

	return res;
}

// フック関数
__declspec(naked) void __stdcall fnHookLog(char *str, BYTE *attr, int dummy1, int dummy2)
{
	// レジスタを退避しないとハングした
	__asm {
		push	eax
		push	ebx
		push	ecx
		push	edx
		mov     eax, dword ptr [esp+14h]
		mov     ebx, dword ptr [esp+18h]
		push	eax
		push	ebx
		call	hook_log
		add		esp,8h
		pop		edx
		pop		ecx
		pop		ebx
		test	eax, eax
		jne		next_func
		pop		eax
		ret		10h
next_func:
		pop		eax
		jmp		pNextLogHook
	}
}
#endif


#if CMD_HOOK == 1

// コマンドもしくはデバッグログをFIFOに書込む
// str			ログ本文
BOOL hook_cmd(char *str)
{
	int		n, wp, rp, inject, typ;
	BOOL	res = TRUE;
	char	*ptr, *buf, *c;

	if (hMtx == 0 || str == NULL)	return res;

	WaitForSingleObject(hMtx, INFINITE);
	wp = pData->pWriteCmd;

	strncpy_s(pData->tCmd[wp].Buffer, LEN_CMD_BUFFER-1, str, LEN_CMD_BUFFER-2);

	ptr = pData->tCmd[wp].Buffer;
	pData->tCmd[wp].BufferW[0] = 0;
	pData->tCmd[wp].fCut = 0;

#if CMD_CUT == 1
	// コマンドカット

	buf = 0;
	typ = 0;

	if (pData->fCutAM && pData->nCutAM && *ptr == '/' && !strstr(ptr, "<st")) {
		ptr++;

		// メニュー発行された命令
		if (pData->fCutAM & CMD_CUT_FMENU) {
			if (!strncmp(ptr, "magic \"", 7)) {
				buf = &pData->tCmd[wp].Buffer[8];
			} else if (!strncmp(ptr, "weaponskill \"", 13)) {
				buf = &pData->tCmd[wp].Buffer[14];
			} else if (!strncmp(ptr, "jobability \"", 12)) {
				buf = &pData->tCmd[wp].Buffer[13];
			}
			if (buf)
				typ = 1;
		}

		// マクロもしくはコマンドラインより発行された命令
		if (typ == 0 && pData->fCutAM & CMD_CUT_FMACRO) {
			if (!strncmp(ptr, "ma ", 3) || !strncmp(ptr, "ja ", 3) || !strncmp(ptr, "ws ", 3)) {
				buf = &pData->tCmd[wp].Buffer[4];
				typ = 2;
			}
		}

		if (typ) {
			PTCUTAM	p = pData->tCutAM;

			for (n = 0; n < pData->nCutAM; n++, p++) {
				if (!strncmp(buf, p->Buffer, p->nLen - (p->fPass != 0))) {
					if (p->fPass == 2) {
						c = &buf[p->nLen-1];
						if (*c != 'I' && *c != 'V' && *c != 'X' && *c != ' ' &&
							strncmp(c, "壱", 2) && strncmp(c, "弐", 2) && strncmp(c, "参", 2) && strncmp(c, "四", 2))
							continue;
					}

					if (typ == 1) {
						pData->tCmd[wp].fCut = 3;
					} else {
						if (!strpbrk(buf, "0123456789")) {
							pData->tCmd[wp].fCut = 1;
						} else {
							pData->tCmd[wp].fCut = 2;
						}
					}
					res = FALSE;
					break;
				}
			}
		}
	}
#endif

	if (++wp >= SIZE_CMD_FIFO)
		wp = 0;

	pData->pWriteCmd = wp;

	rp = (wp == SIZE_CMD_FIFO-1) ? 0 : wp+1;
	inject = pData->nInject;

	// ライトポインタがリードポインタに追いついていたらリードポインタをインクリメント
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		// 有効なテーブルだけチェックする
		if (pData->fEna[n]) {
			if (pData->pReadCmd[n] == wp)
				pData->pReadCmd[n] = rp;
			inject--;
		}
	}

	ReleaseMutex(hMtx);

	return res;
}

#if CMD_HOOK == 1
// コマンドフック関数
__declspec(naked) void __stdcall fnHookCmd(char *str)
{
	__asm {
		push	eax
		push	ebx
		push	ecx
		push	edx
		mov     eax, dword ptr [esp+14h]
		push	eax
		call	hook_cmd
		add		esp,4h
		pop		edx
		pop		ecx
		pop		ebx
		test	eax, eax
		jne		next_func
		pop		eax
		ret
next_func:
		pop		eax
		jmp		pNextCmdHook
	}
}
#endif
#endif


#ifdef USE_HOOK

// アタッチ処理
PIHOOK attach(PBYTE pCode, PBYTE pCallBackFunc, PBYTE *pNextHook)
{
	PIHOOK	pi;
	PBYTE	src, dis, ptr;

	if (pCode == NULL || pNextHook == NULL)
		return NULL;	// 引数異常

	if ((pi = new IHOOK) == NULL)
		return NULL;	// メモリ確保失敗

	ZeroMemory(pi, sizeof(IHOOK));

	pi->pCode = pCode;
	src = pCode;
	dis = pi->func;

	// 6バイト分の命令をコピー
	for (int n = 0; n < 6; n++)
		*dis++ = *src++;

	// 本来飛ぶべきアドレスを計算してセット
	if (*pCode == 0xe8) {
		// call部分にアタッチ
		ptr = (PBYTE)(DWORD64)(*(DWORD *)(pCode+1));
		ptr = (PBYTE)((DWORD64)pCode + (DWORD64)ptr + 5);
		*pNextHook = ptr;
	} else {
		pi->pNext = pCode + 6;
		*dis++ = 0xff;
		*dis++ = 0x25;
		*(DWORD *)dis = (DWORD)(DWORD64)(&pi->pNext);
		*pNextHook = pi->func;
	}


	if (pCallBackFunc) {
		WaitForSingleObject(hMtx, INFINITE);
		pi->fAttach = TRUE;
		pi->pHook = pCallBackFunc;

		// アタッチする
		if (*pCode == 0xe8) {
			// call部分にアタッチ
			DWORD addr;
			ptr = pCode+1;
			addr = (DWORD)((DWORD64)pCallBackFunc - (DWORD64)pCode - 5);
			*(DWORD *)(pCode+1) = addr;
		} else {
			*pCode++ = 0xff;
			*pCode++ = 0x25;
			*(DWORD *)pCode = (DWORD)(DWORD64)&pi->pHook;
		}
		ReleaseMutex(hMtx);
	}


	return pi;
}

// デタッチ処理
void detach(PIHOOK pihook)
{
	PBYTE	src, dis;

	if (pihook == NULL)
		return;		// 引数異常

	if (pihook->fAttach) {
		WaitForSingleObject(hMtx, INFINITE);

		src = pihook->func;
		dis = pihook->pCode;

		// 元に戻す
		for (int n = 0; n < 6; n++)
			*dis++ = *src++;

		ReleaseMutex(hMtx);
	}

	delete pihook;
}

#endif

#if DBG_DLL == 1
// デバッグ用ログ出力
void syslog(char *str)
{
	char buf[512];
	BYTE attr = 255;

	wsprintfA(buf, "[system log]%s", str);
	hook_log(&attr, buf);
}
#endif

// パターン検索
static PVOID search_addr(BYTE *base, TCHAR *pattern, DWORD *addr = NULL, DWORD *adj = NULL)
{
	BYTE	dat = 0, ptrn[256], mask[256], *p1, *p2;
	char	c, toggle = 0;
	int		n, cnt = 0, fp;
	PVOID	ptr = 0;

	//検索パターン展開
	ZeroMemory(ptrn, sizeof(ptrn));
	ZeroMemory(mask, sizeof(mask));
	if (addr)		*addr = 0;
	if (adj)		*adj = 0;

	toggle = 0;
	cnt = 0;

	for (n = 0; n < 100; n++) {
		c = (char)tolower(pattern[n]);
		if (c == ' ')
			continue;
		if (c == 0) {
			mask[cnt] = 1;
			break;
		}
		if (c == '.')
			mask[cnt] = 1;		//「.」 マスクパターン
		else if (c == 'x')
			mask[cnt] = 2;		//「x」 アドレス部
		else if (c == 'p')
			mask[cnt] = 3;		//「p」 オフセット部
		else if ('0' <= c && c <= '9')
			dat |= (c -'0');
		else if ('a' <= c && c <= 'f')
			dat |= (c -'a'+10);
		else
			return 0;

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		} else
			dat <<= 4;
		toggle ^= 1;
	}

	// 最初に有効なパターンの位置を検索(.を飛ばす)
	for (n = 0; n < cnt; n++)
		if (mask[n] == 0) break;

	fp = n;

	for (p1 = (BYTE*)base; p1 < (BYTE*)base + SEARCH_RANGE; p1++) {
		if (*p1 != ptrn[fp]) continue;
		p2 = p1 + 1;
		for (n = fp + 1; n < cnt; p2++, n++) {
			if (mask[n]) continue;
			if (*p2 != ptrn[n]) break;
		}
		if (n == cnt) {
			ptr = (PVOID)(p1 - fp);
			if (addr) {
				for (n = 0; n < cnt; n++) {
					if (mask[n] == 2) {
						//「x」 アドレス部
						*addr = *(DWORD *)(p1-fp+n);
						break;
					}
				}
			}
			if (adj) {
				for (n = 0; n < cnt; n++) {
					if (mask[n] == 3) {
						//「p」 オフセット部
						*adj = *(DWORD *)(p1-fp+n);
						break;
					}
				}
			}
			break;
		}
	}

	return ptr;
}

// -------------------------------------------------
// コマンドインターフェース アプリからPOLにコマンド
// -------------------------------------------------
#ifdef USE_EXCHANGE
void  exchange(void *data)
{
	BOOL	fend = FALSE;
	MSG		msg;

	pData->idThread = GetCurrentThreadId();

	while (!fend) {
		if (GetMessage(&msg, NULL, 0, 0) != 1)
			break;

		switch (msg.message) {
#if CMD_PUT == 1
		case C_CMD:
			if (pData->tPtr[FF11ADDR_PTINFO]) {
				if (*pData->tPtr[FF11ADDR_PTINFO] == 0)
					break;
			}
			if (pData->nPrm)
				pNextCmdHook(pData->nPrm, 1);
			break;
#endif
#if KILLEFFECT == 1
		case C_KILLEFFECT:
			if (pData->tPtr[FF11ADDR_PTINFO]) {
				if (*pData->tPtr[FF11ADDR_PTINFO] == 0)
					break;
			}
			if (pKillEffect && msg.wParam) {
				pKillEffect((int)msg.wParam);
			}
			break;
#endif
#if MERGEITEM == 1
		case C_MERGEITEM:
			if (pData->tPtr[FF11ADDR_PTINFO]) {
				if (*pData->tPtr[FF11ADDR_PTINFO] == 0)
					break;
			}
			if (pMergeItem)
				pMergeItem((int)msg.wParam);
			break;
#endif
#if MACROEXE == 1
		case C_MACROEXE:
			if (pData->tPtr[FF11ADDR_PTINFO]) {
				if (*pData->tPtr[FF11ADDR_PTINFO] == 0)
					break;
			}
			if (pMacro)
				pMacro((int)msg.wParam, (int)msg.lParam);
			break;
#endif
		case C_END:
			fend = TRUE;
			break;
		}
//		Sleep(5);
		SetEvent(hEvent);
	}

	_endthread();
}
#endif // USE_EXCHENGE

BOOL init(void)
{
	TCHAR	obj_name[256];
	char	mes[256];
	BOOL	res = TRUE;
	PVOID	ptr = NULL;

	if (pData)	{
		syslog("すでに初期済み?");
		return TRUE;	// 初期化済み
	}

	idPro = GetCurrentProcessId();

	wsprintf(obj_name, MAP_NAME, idPro);
	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, obj_name);
	if (hMap == 0)		return FALSE;

	wsprintf(obj_name, MTX_NAME, idPro);
	hMtx = OpenMutex(MUTEX_ALL_ACCESS, NULL, obj_name);
	if (hMtx == 0)		return FALSE;

#ifdef USE_EXCHANGE
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
	if (hEvent == 0)	return FALSE;
#endif // USE_EXCHENGE

	pData = (PTDATA)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pData == NULL)	return FALSE;

	SLwsprintfA(mes, "プロセスID = %d", idPro);
	syslog(mes);
	SLwsprintfA(mes, "baseアドレス = %08x", pData->pBase);
	syslog(mes);

	// FFxiMain.Dllが見つかっていないのでアタッチできない
	if (pData->pBase == NULL)	return FALSE;

#if LOG_HOOK == 1
	{
		ptr = search_addr(pData->pBase, pattern_log);

		SLwsprintfA(mes, "ログアタッチアドレス = %08x", ptr);
		syslog(mes);

		if (ptr)
			pILOG_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookLog, (PBYTE*)&pNextLogHook);

		if (pILOG_HOOK) {
			pData->fRdy |= FF11_Rdy_GetLog;
			syslog("ログアタッチ成功!");
		} else {
			syslog("ログアタッチ失敗!");
		}
	}
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
	{
		ptr = search_addr(pData->pBase, pattern_cmd);

		SLwsprintfA(mes, "コマンドアタッチアドレス = %08x", ptr);
		syslog(mes);

#if CMD_HOOK == 1
		if (ptr)
			pICMD_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookCmd, (PBYTE*)&pNextCmdHook);
#else
		if (ptr)
			pICMD_HOOK = attach((PBYTE)ptr, NULL, (PBYTE*)&pNextCmdHook);
#endif
		if (pICMD_HOOK) {
			pData->fRdy |= FF11_Rdy_GetCmd;
			syslog("コマンドアタッチ成功!");
		} else {
			syslog("コマンドアタッチ失敗!");
		}
	}
#endif

#ifdef USE_EXCHANGE
	if (pData->hThread = (HANDLE)_beginthread(exchange, 0, NULL)) {
#if CMD_PUT == 1
		if (pICMD_HOOK)
			pData->fRdy |= FF11_Rdy_PutCmd;
#endif
#if KILLEFFECT == 1
		{
			ptr = search_addr(pData->pBase, pattern_killeffect);
			SLwsprintfA(mes, "KILLEFFECTアタッチアドレス = %08x", ptr);
			syslog(mes);
			if (pKillEffect = (void (*)(int))ptr)
				pData->fRdy |= FF11_Rdy_KillEffect;
		}
#endif
#if MERGEITEM == 1
		{
			ptr = search_addr(pData->pBase, pattern_mergeitem);
			SLwsprintfA(mes, "MERGEITEMアタッチアドレス = %08x", ptr);
			syslog(mes);
			if (pMergeItem = (void (*)(int))ptr)
				pData->fRdy |= FF11_Rdy_MergeItem;
		}
#endif
#if MACROEXE == 1
		{
			ptr = search_addr(pData->pBase, pattern_macroexe);
			SLwsprintfA(mes, "MACROアタッチアドレス = %08x", ptr);
			syslog(mes);
			if (pMacro = (void (__stdcall *)(int,int))ptr)
				pData->fRdy |= FF11_Rdy_Macro;
		}
#endif
	}
#endif // USE_EXCHENGE

	DWORD addr, adj;

	pData->tPtr[FF11ADDR_BASE] = pData->pBase;

	if (search_addr(pData->pBase, pattern_job, &addr))
		pData->tPtr[FF11ADDR_JOBINFO] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_ptinfo, &addr)) {
		addr += ADJUST_PTINFO;
		pData->tPtr[FF11ADDR_PTINFO] = (PBYTE)(DWORD64)addr;
	}

	if (search_addr(pData->pBase, pattern_npcmap, &addr))
		pData->tPtr[FF11ADDR_NPCMAP] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_targetinfo, &addr))
		pData->tPtr[FF11ADDR_TARGETINFO] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_abitab, &addr))
		pData->tPtr[FF11ADDR_ABIITABLE] = (PBYTE)(DWORD64)addr;
	syslog(mes);

	if (search_addr(pData->pBase, pattern_magtab, &addr))
		pData->tPtr[FF11ADDR_MAGICTABLE] = (PBYTE)(DWORD64)addr;
	syslog(mes);

	if (search_addr(pData->pBase, pattern_aomahold, &addr))
		pData->tPtr[FF11ADDR_AOMAHOLD] = (PBYTE)(DWORD64)addr;
	syslog(mes);

	if (search_addr(pData->pBase, pattern_weather, &addr))
		pData->tPtr[FF11ADDR_WEATHER] = (PBYTE)(DWORD64)addr;
	syslog(mes);

	if (search_addr(pData->pBase, pattern_lsname, &addr))
		pData->tPtr[FF11ADDR_LSNAME] = (PBYTE)(DWORD64)addr;
	syslog(mes);

	if (search_addr(pData->pBase, pattern_icon, &addr))
		pData->tPtr[FF11ADDR_ICONTABLE] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_meripo, &addr))
		pData->tPtr[FF11ADDR_MERIPO] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_act, &addr)) {
		addr += ADJUST_ACT;
		pData->tPtr[FF11ADDR_ACTION] = (PBYTE)(DWORD64)addr;
	}

	if (search_addr(pData->pBase, pattern_item, &addr))
		pData->tPtr[FF11ADDR_TARGETITEM] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_flist, &addr, &adj)) {
		addr = *(DWORD *)(DWORD64)(PBYTE)(DWORD64)addr;
		pData->tPtr[FF11ADDR_FLIST] = (PBYTE)(DWORD64)(addr+adj);
	}

	if (search_addr(pData->pBase, pattern_login, &addr))
		pData->tPtr[FF11ADDR_LOGIN] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_bnum, &addr))
		pData->tPtr[FF11ADDR_NUMBAG] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_ownpos, &addr))
		pData->tPtr[FF11ADDR_OWNPOS] = (PBYTE)(DWORD64)addr + 4;

	if (search_addr(pData->pBase, pattern_choose, &addr))
		pData->tPtr[FF11ADDR_CHOOSE] = (PBYTE)(DWORD64)addr;

	if (search_addr(pData->pBase, pattern_lockon, &addr)) {
		addr += ADJUST_LOCKON;
		pData->tPtr[FF11ADDR_LOCKON] = (PBYTE)(DWORD64)addr;
	}

	if (search_addr(pData->pBase, pattern_petid, &addr)) {
		addr += ADJUST_PETID;
		pData->tPtr[FF11ADDR_PETID] = (PBYTE)(DWORD64)addr;
	}

	if (search_addr(pData->pBase, pattern_equip, &addr)) {
		pData->tPtr[FF11ADDR_ITEMBASE] = (PBYTE)(DWORD64)addr;

		if (search_addr(pData->pBase, pattern_item_adj, &addr))
			pData->tPtr[FF11ADDR_ITEMTABLE] = (PBYTE)(DWORD64)addr;

		if (search_addr(pData->pBase, pattern_equip_adj, &addr))
			pData->tPtr[FF11ADDR_EQUIPTABLE] = (PBYTE)(DWORD64)addr;

		if (search_addr(pData->pBase, pattern_bag_adj, &addr))
			pData->tPtr[FF11ADDR_MAXBAG] = (PBYTE)(DWORD64)addr;
	}

	if (pData->tPtr[FF11ADDR_ICONTABLE])
		pData->tPtr[FF11ADDR_ISAREA] = pData->tPtr[FF11ADDR_ICONTABLE] - 0x100;


#if DBG_DLL == 1
	for (int n = 0; n < FF11ADDR_ENDPOINT; n++) {
		SLwsprintfA(mes, "%s = %08x", Addres[n], pData->tPtr[n]);
		syslog(mes);
	}
#endif

	return TRUE;
}

// -----------------------------------------------------
// アプリ側で使用する関数
// -----------------------------------------------------

typedef struct enumwindowprm {
	DWORD	id;
	HWND	hWnd;
	TCHAR	name[128];
} PENUM;

BOOL CALLBACK EnumWindowsProc(HWND hwnd , LPARAM lp)
{
	PENUM	*ep;
	DWORD	pid;
	TCHAR	buf[1024];

	ep = (PENUM *)lp;

	GetWindowThreadProcessId(hwnd, &pid);

	if (pid == ep->id) {
		GetWindowText(hwnd, buf, 1024);
		if (ep->name[0]) {
			if (!_tcscmp(ep->name, buf))
				return TRUE;
		} else {
			_tcsncpy_s(ep->name, 128, buf, 127);
		}

		ep->hWnd = hwnd;
		return FALSE;
	}

	return TRUE;
}

// POLのプロセスIDを返す
// no		no番目のPOLのIDを返す
// 戻り値	POLID
EXPORT DWORD WINAPI FF11_GetProcessId(int no)
{
	PROCESSENTRY32	pe;
	HANDLE	hps;
	BOOL	res;
	DWORD	id = 0;
	PENUM	ep;
	int		n, cnt = 0;

	no--;
	if (no < -2 || MAX_PROCESS <= no)		return 0;

	// POL.EXEプロセス検索
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);

	hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hps == INVALID_HANDLE_VALUE)	return 0;

	res = Process32First(hps, &pe);

	// 起動中のPOLをすべて登録する
	while (res) {
		if (!lstrcmp(POL_NAME, pe.szExeFile)) {
			// POL発見
			id = pe.th32ProcessID;

			// すでに登録済みかチェックする
			for (n = 0; n < MAX_PROCESS; n++) {
				if (tProcess[n].idPro == id)
					break;
			}

			// 未登録なので登録する
			if (n == MAX_PROCESS) {
				for (; cnt < MAX_PROCESS; cnt++) {
					if (tProcess[cnt].idPro == 0) {
						tProcess[cnt++].idPro = id;
						break;
					}
				}
				// 登録可能数オーバー
				if (cnt >= MAX_PROCESS)
					break;
			}
		}
		res = Process32Next(hps, &pe);
	}

	CloseHandle(hps);

	ZeroMemory(tPList, sizeof(tPList));

	if (no == -1) {
		// プロセスがひとつのみ
		if (cnt == 1) {
			no = 0;
		} else {
			cnt = 0;
			for (n = 0; n < MAX_PROCESS; n++) {
				if (tProcess[n].idPro) {
					ep.id = tProcess[n].idPro;
					ep.hWnd = 0;
					ep.name[0] = 0;
					tPList[cnt].nPro = n;
					EnumWindows(EnumWindowsProc , (LPARAM)&ep);

					wsprintf(tPList[cnt].sName, _T("0x%08x %s"), tProcess[n].idPro, ep.name);
					cnt++;
				}
			}

			no = 0;
			n = (int)DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
			if (n >= 0 && n < MAX_PROCESS)
				no = tPList[n].nPro;

		}
	}

	return tProcess[no].idPro;
}

// BASEアドレスを返す
// pid		POLのプロセスID
// 戻り値	ベースアドレス
EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD pid)
{
    MODULEENTRY32	me;
	HANDLE	hms;
	BOOL	res;
	PBYTE	ptr = NULL;

	if (pid == 0)	return NULL;

	// FFXiMain.dllを探す
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	hms = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hms == INVALID_HANDLE_VALUE)	return NULL;

	res = Module32First(hms, &me);

	while (res) {
		if (!lstrcmp(DLL_NAME, me.szModule)) {
			// DLL発見
			ptr = me.modBaseAddr;
			break;
		}
		res = Module32Next(hms, &me);
	}

	CloseHandle(hms);

	return ptr;
}

// BASEアドレスを返す
// pid		POLのプロセスID
// 戻り値	index番号
EXPORT int WINAPI FF11_Inject(DWORD pid)
{
	DWORD64	hpro;
	PTDATA	pd;
	PTPROC	proc = NULL;
	int		index, n, res = -1;
	TCHAR	obj_name[256];
	PBYTE	pbase;

	FF11_GetProcessId(-1);

	pbase = FF11_GetBaseAddress(pid);

	// まだFFXiMain.dllが読込まれていないのでアタッチできない
	if (pbase == NULL)			return 0;

	// インデックス登録数オーバー
	if (nIndex >= MAX_INDEX)	return -1;

	// プロセスID検索
	for (n = 0; n < MAX_PROCESS; n++) {
		if (tProcess[n].idPro == pid) {
			proc = &tProcess[n];
			break;
		}
	}

	if (n >= MAX_PROCESS)	return -1;	// 登録なし

	// ミューテックス作成
	if (proc->hMtx == 0) {
		wsprintf(obj_name, MTX_NAME, pid);
		proc->hMtx = CreateMutex(NULL, FALSE, obj_name);
	}

	if (proc->hMtx == 0)	return -1;	// ミューテックス作成失敗

	WaitForSingleObject(proc->hMtx, INFINITE);

	// プロセスオープン
	if (proc->hPro == 0)
		proc->hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (proc->hPro == 0)	goto err_end;

	hpro = (DWORD64)proc->hPro;

	// イベント作成
#ifdef USE_EXCHANGE
	if (proc->hEvent == 0)
		proc->hEvent = CreateEvent(NULL, TRUE, TRUE, EVENT_NAME);
	if (proc->hEvent == 0)	goto err_end;
#endif // USE_EXCHENGE

	// 空きインデックスを検索(index=0は使わない)
	for (index = 1; index < MAX_INDEX; index++) {
		// プロセス管理テーブルへのポインタがNULLなら空き
		if (tIndex[index].pProc == NULL)	break;
	}

	// 共有メモリオープン
	if (proc->hMap == 0) {
		BOOL first;
		wsprintf(obj_name, MAP_NAME, pid);
		proc->hMap = CreateFileMapping((HANDLE)-1,
				NULL, PAGE_READWRITE, 0, sizeof(TDATA), obj_name);
		if (proc->hMap == 0)		goto err_end;

		// 共有ファイルが新規作成なら初期化する
		first = (GetLastError() != ERROR_ALREADY_EXISTS);

		proc->pData = (PTDATA)MapViewOfFile(proc->hMap, FILE_MAP_ALL_ACCESS,0,0,0);
		if (proc->pData == NULL)	goto err_end;
		if (first)
			ZeroMemory(proc->pData, sizeof(TDATA));
	}

	pd = proc->pData;
	if (pd == NULL)					goto err_end;
	if (pd->nInject >= MAX_INJECT)	goto err_end;	// これ以上インジェクトできない

	pd->pBase = pbase;

	nIndex++;
	tIndex[index].pProc = proc;

	ReleaseMutex(proc->hMtx);

	if (pd->nInject == 0) {
		// このプロセスへの初登録なのでdllをインジェクトする
		if (InjectLibrary((DWORD)hpro, _T("madCHook.dll")))
			pd->fLD_madC = TRUE;
		if (InjectLibrary((DWORD)hpro, _T("ffhookex.dll")))
			pd->fLD_hook = TRUE;
	}

	WaitForSingleObject(proc->hMtx, INFINITE);

	// 空きを探して確保
	for (n = 0; n < MAX_INJECT; n++) {
		if (!pd->fEna[n]) {
			pd->nInject++;
			pd->fEna[n] = TRUE;
			tIndex[index].pInject = n;

#if DBG_DLL == 0
			pd->pReadLog[n] = pd->pWriteLog;
#else
			// 最初のinjectならシステムログを取るためにFIFOクリアをしない
			if (pd->nInject != 0)
				pd->pReadLog[n] = pd->pWriteLog;
#endif

#if CMD_HOOK == 1
			pd->pReadCmd[n] = pd->pWriteCmd;
#endif
			break;
		}
	}

	ReleaseMutex(proc->hMtx);

	return index;

err_end :

	if (proc->hMap) {
		CloseHandle(proc->hMap);
		proc->hMap = 0;
	}
	if (proc->hPro) {
		CloseHandle(proc->hPro);
		proc->hPro = 0;
	}
#ifdef USE_EXCHANGE
	if (proc->hEvent) {
		CloseHandle(proc->hEvent);
		proc->hEvent = 0;
	}
#endif
	if (proc->hMtx) {
		ReleaseMutex(proc->hMtx);
		CloseHandle(proc->hMtx);
		proc->hMtx = 0;
	}

	return -1;
}

// polが落ちていたらFALSEを返す
BOOL check_dll(HANDLE hpro)
{
	BOOL	res;
	DWORD	code;

	res = GetExitCodeProcess(hpro, &code);
	if (res == 0 || code != STILL_ACTIVE)
		return FALSE;

	return TRUE;
}

BOOL check_login(HANDLE hpro, PTDATA pd)
{
	DWORD n;

	return TRUE;
	if (pd->tPtr[FF11ADDR_LOGIN]) {
		if (ReadProcessMemory(hpro, pd->tPtr[FF11ADDR_LOGIN],(PBYTE)&n,4,NULL)) {
			if (n == 0x3f800000)
				return FALSE;
		}
	}

	return TRUE;
}

// index値からprocとpdを求める。異常があればFALSEを返す
BOOL check_index(int index, PTPROC *proc, PTDATA *pd)
{
	if (MAX_INDEX <= (DWORD)index)				return FALSE;
	if ((*proc = tIndex[index].pProc) == NULL)	return FALSE;
	if ((*pd = (*proc)->pData) == NULL)			return FALSE;

	if (!check_dll((*proc)->hPro)) {
		FF11_Eject(index);
		return FALSE;
	}

	return TRUE;
}

EXPORT int WINAPI FF11_Eject(int index)
{
	DWORD64	hpro;
	PTDATA	pd;
	PTPROC	proc;
	int		n, nadd = MAX_INDEX;

	if (index >= MAX_INDEX)
		return FALSE;		// index値が異常

	if (index <= 0) {
		// indexが0以下なら全部をインジェクトする
		nadd = 1;
		index = 1;
	}

	for (n = index; n < MAX_INDEX && 0 < nIndex; n += nadd) {
		proc = tIndex[n].pProc;
		if (proc == NULL)	continue;
		pd = proc->pData;
		if (pd == NULL)		continue;

		pd->fEna[tIndex[n].pInject] = FALSE;
		tIndex[n].pProc = NULL;
		tIndex[n].pInject = 0;
		nIndex--;

		// プロセス最後のイジェクトならいろいろ開放する
		if (--pd->nInject <= 0) {
			if (proc->idPro) {
				if (hpro = (DWORD64)proc->hPro) {
					if (pd->fLD_hook)
						UninjectLibrary((DWORD)hpro, _T("ffhookex.dll"));
					if (pd->fLD_madC)
						UninjectLibrary((DWORD)hpro, _T("madChook.dll"));
				}
				proc->idPro = 0;
				pd->fLD_hook = FALSE;
				pd->fLD_madC = FALSE;
			}

			if (proc->pData) {
				UnmapViewOfFile(proc->pData);
				proc->pData = NULL;
			}
			if (proc->hPro) {
				CloseHandle(proc->hPro);
				proc->hPro = 0;
			}
			if (proc->hMap) {
				CloseHandle(proc->hMap);
				proc->hMap = 0;
			}
#ifdef USE_EXCHANGE
			if (proc->hEvent) {
				CloseHandle(proc->hEvent);
				proc->hEvent = 0;
			}
#endif // USE_EXCHENGE
			if (proc->hMtx) {
				CloseHandle(proc->hMtx);
				proc->hMtx = 0;
			}
		}
	}

	return nIndex;
}

EXPORT PBYTE WINAPI get_address(int index, int object)
{
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd)) {
		if ((DWORD)object < FF11ADDR_ENDPOINT)
			return pd->tPtr[object];
	}

	return NULL;
}

EXPORT PBYTE WINAPI FF11_GetAddressA(int index, int object, char *name)
{
	BYTE	*ptr;

	if ((DWORD)object >= FF11ADDR_ENDPOINT) {
		if (name)
			*name = 0;
		return 0;
	}

	ptr = get_address(index, object);

	if (name)
		strcpy_s(name, 64, Addres[object]);

	return ptr;
}

EXPORT PBYTE WINAPI FF11_GetAddressW(int index, int object, wchar_t *name)
{
	BYTE	*ptr;

	if ((DWORD)object >= FF11ADDR_ENDPOINT) {
		if (name)
			*name = 0;
		return 0;
	}

	ptr = get_address(index, object);

	if (name)
		MultiByteToWideChar(CP_ACP, 0, Addres[object], -1, name, 64);

	return ptr;
}

EXPORT PBYTE WINAPI FF11_SetAddress(int index, int object, DWORD address)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr = NULL;

	if (check_index(index, &proc, &pd)) {
		if (object == 0) {
			ptr = (PBYTE)(DWORD64)address;
		} else if ((DWORD)object <= FF11ADDR_OWNPOS) {
			ptr = pd->tPtr[FF11ADDR_BASE] + address;
		}
		pd->tPtr[object] = ptr;
	}

	return ptr;
}

EXPORT int WINAPI FF11_Rdy(int index)
{
	PTPROC	proc;
	PTDATA	pd;

	if (!check_index(index, &proc, &pd)) {
		FF11_Eject(index);
		return -1;
	}

	if (!check_dll(proc->hPro)) {
		FF11_Eject(index);
		return -1;
	}

	if (pd->fRdy == 0) {
		FF11_Eject(index);
		return -1;
	}

	return pd->fRdy;
}

#if LOG_HOOK == 1 || DBG_DLL == 1
int get_log(int index, BYTE *attr, char *buf, wchar_t *wbuf, int sbuf, BYTE *fcut)
{
	PTPROC	proc;
	PTDATA	pd;
	int		p, no, res = 0;

	if (buf)	*buf = 0;
	if (wbuf)	*wbuf = 0;

	if (sbuf < 2 || (buf == NULL && wbuf == NULL))	return -1;

	if (!check_index(index, &proc, &pd))
		return -1;

#if DBG_DLL != 1
	if (!(pd->fRdy & FF11_Rdy_GetLog))		return 0;
#endif

	WaitForSingleObject(proc->hMtx, INFINITE);

	no = tIndex[index].pInject;
	if (pd->fEna[no]) {
		p = pd->pReadLog[no];
		if (p != pd->pWriteLog) {
			if (buf) {
				strncpy_s(buf, sbuf, pd->tLog[p].Buffer, sbuf-1);
				buf[sbuf-1] = '\0';		// 念のため終端コード
				res = (int)strlen(buf);
			} else {
				if (pd->tLog[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tLog[p].Buffer, -1, pd->tLog[p].BufferW,
									sbuf-1); 
				wcsncpy_s(wbuf, sbuf, pd->tLog[p].BufferW, sbuf-1);
				wbuf[sbuf-1] = '\0';	// 念のため終端コード
				res = (int)wcslen(wbuf);
			}

			if (attr)
				*attr = pd->tLog[p].tAttr;
			if (fcut)
				*fcut = pd->tLog[p].fCut;
			if (++p >= SIZE_LOG_FIFO)
				p = 0;
			pd->pReadLog[no] = p;
		}
	}

	ReleaseMutex(proc->hMtx);

	return res;
}
#endif

#if CMD_HOOK == 1
int get_cmd(int index, char *buf, wchar_t *wbuf, int sbuf, BYTE *fcut)
{
	PTPROC	proc;
	PTDATA	pd;
	int		p, no, res = 0;

	if (buf)	*buf = 0;
	if (wbuf)	*wbuf = 0;

	if (sbuf < 2 || (buf == NULL && wbuf == NULL))	return -1;
	if (!check_index(index, &proc, &pd))
		return -1;

	if (!(pd->fRdy & FF11_Rdy_GetCmd))		return 0;

	WaitForSingleObject(proc->hMtx, INFINITE);

	no = tIndex[index].pInject;
	if (pd->fEna[no]) {
		p = pd->pReadCmd[no];
		if (p != pd->pWriteCmd) {
			if (buf) {
				strncpy_s(buf, sbuf, pd->tCmd[p].Buffer, sbuf-1);
				buf[sbuf - 1] = '\0';		// 念のため終端コード
				res = (int)strlen(buf);
			} else {
				if (pd->tCmd[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tCmd[p].Buffer, -1, pd->tCmd[p].BufferW,
									sbuf-1); 
				wcsncpy_s(wbuf, sbuf, pd->tCmd[p].BufferW, sbuf-1);
				wbuf[sbuf - 1] = '\0';	// 念のため終端コード
				res = (int)wcslen(wbuf);
			}
			if (fcut)
				*fcut = pd->tCmd[p].fCut;
			if (++p >= SIZE_CMD_FIFO)
				p = 0;
			pd->pReadCmd[no] = p;
		}
	}
	ReleaseMutex(proc->hMtx);

	return res;
}
#endif

EXPORT int WINAPI FF11_GetLogA(int index, BYTE *attr, char *buffer, int buffer_size, BYTE *fcut)
{
#if LOG_HOOK == 1 || DBG_DLL == 1
	return get_log(index, attr, buffer, NULL, buffer_size, fcut);
#else
	return -1;
#endif
}

EXPORT int WINAPI FF11_GetLogW(int index, BYTE *attr, wchar_t *buffer, int buffer_size, BYTE *fcut)
{
#if LOG_HOOK == 1 || DBG_DLL == 1
	return get_log(index, attr, NULL, buffer, buffer_size, fcut);
#else
	return -1;
#endif
}

EXPORT BOOL WINAPI FF11_LogCut(int index, BYTE val)
{
#if LOG_CUT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd))	{
		pd->fCut = val;
		return TRUE;
	}
#endif
	return FALSE;
}

#if LOG_HOOK == 1 && LOG_CUT == 1
int set_cut(PTPROC proc, PTDATA pd, char *attr, char *str)
{
	int		val, res = -1;
	PTCUT	pcut;
	char	c, *p1, *p2, buf[512];

	WaitForSingleObject(proc->hMtx, INFINITE);

	if (pd->nCut < MAX_LOGCUT) {
		pcut = &pd->tCut[pd->nCut++];
		ZeroMemory(pcut, sizeof(TCUT));
		if (str) {
			if (strcmp(".*", str)) {
				strncpy_s(pcut->Buffer, LEN_LOG_BUFFER, str, LEN_LOG_BUFFER-1);
			}
		}
		pcut->Buffer[LEN_LOG_BUFFER-1] = 0;
		pcut->nLen = (int)strlen(pcut->Buffer);

		if (attr == NULL) {
			FillMemory(pcut->tAttr, 256, 1);
		} else {
			ZeroMemory(pcut->tAttr, 256);

			strncpy_s(buf, 512, attr, 511);
			buf[511] = 0;
			p1 = buf;

			while (p1 && *p1) {
				if (p2 = strpbrk(p1, " ,\0")) {
					c = *p2;
					*p2++ = 0;
				} else {
					c = 0;
				}
				if (!_stricmp(p1, "ALL")) {
					FillMemory(pcut->tAttr, 256, 1);
				} else {
					sscanf_s(p1, "%x", &val);
					if (val < 0x100)
						pcut->tAttr[val] = 1;
				}
				if (c == 0)
					break;
				p1 = p2 + strspn(p2, " ,");
			}
		}
	}
	ReleaseMutex(proc->hMtx);

	return res;
}
#endif

EXPORT int WINAPI FF11_SetCutStringA(int index, char *attr, char *str)
{
#if LOG_HOOK == 1 && LOG_CUT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd))
		return set_cut(proc, pd, attr, str);
#endif
	return -1;
}

EXPORT int WINAPI FF11_SetCutStringW(int index, wchar_t *attr, wchar_t *str)
{
#if LOG_CUT == 1
	char	bstr[512], battr[512], *p = NULL;
	if (str == NULL)
		return -1;

	WideCharToMultiByte(CP_ACP, 0, str, -1, bstr, 512, 0, 0);

	if (attr) {
		WideCharToMultiByte(CP_ACP, 0, attr, -1, battr, 512, 0, 0);
		p = battr;
	}
	return FF11_SetCutStringA(index, p, bstr);
#else
	return -1;
#endif
}

EXPORT BOOL WINAPI FF11_ClrCutString(int index)
{
#if LOG_CUT == 1
	PTDATA	pd;
	PTPROC	proc;

	if (check_index(index, &proc, &pd)) {
		WaitForSingleObject(proc->hMtx, 3000);
		pd->nCut = 0;
		ReleaseMutex(proc->hMtx);
		return TRUE;
	}
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_CmdCut(int index, BYTE val)
{
#if CMD_CUT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd))	{
		pd->fCutAM = val;
		return TRUE;
	}
#endif
	return FALSE;
}

#if CMD_HOOK == 1 && CMD_CUT == 1
int set_cutcmd(PTPROC proc, PTDATA pd, char *str)
{
	int		n = 0, res = -1;
	PTCUTAM	pcut;

	WaitForSingleObject(proc->hMtx, INFINITE);

	if (pd->nCutAM < MAX_AMCUT) {
		pcut = &pd->tCutAM[pd->nCutAM++];
		ZeroMemory(pcut, sizeof(TCUTAM));
		pcut->fPass = 0;

		if (str) {
			n = (int)strlen(str);
			if (str[n-1] == '*') {
				str[n-1] = '\0';
				pcut->fPass = 1;
			} else if (str[n-1] == '`') {
				str[n-1] = '\0';
				pcut->fPass = 2;
			}
			sprintf_s(pcut->Buffer, LEN_AM_BUFFER, "%s ", str);
			n = (int)strlen(pcut->Buffer);
		}
		pcut->Buffer[LEN_AM_BUFFER-1] = 0;
		pcut->nLen = n;
	}

	ReleaseMutex(proc->hMtx);

	return res;
}
#endif

EXPORT int WINAPI FF11_SetCutCmdA(int index, char *str)
{
#if CMD_HOOK == 1 && CMD_CUT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd))
		return set_cutcmd(proc, pd, str);
#endif
	return -1;
}

EXPORT int WINAPI FF11_SetCutCmdW(int index, wchar_t *str)
{
#if CMD_CUT == 1
	char	bstr[512];
	if (str == NULL)
		return -1;

	WideCharToMultiByte(CP_ACP, 0, str, -1, bstr, 512, 0, 0);

	return FF11_SetCutCmdA(index, bstr);
#else
	return -1;
#endif
}

EXPORT BOOL WINAPI FF11_ClrCutCmd(int index)
{
#if LOG_CUT == 1
	PTDATA	pd;
	PTPROC	proc;

	if (check_index(index, &proc, &pd)) {
		WaitForSingleObject(proc->hMtx, 3000);
		pd->nCutAM = 0;
		ReleaseMutex(proc->hMtx);
		return TRUE;
	}
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_LoadLogCutINIA(int index, char *ini)
{
#if LOG_CUT == 1
	PTDATA	pd;
	PTPROC	proc;

	int		n;
	char	buffer[1024], attr[1024], key[16];

	if (!FF11_ClrCutString(index))
		return FALSE;

	proc = tIndex[index].pProc;
	pd = proc->pData;

	for (n = 0; n < MAX_LOGCUT; n++) {
		wsprintfA(key, "str%d", n);
		GetPrivateProfileStringA("LOGCUT", key, "", buffer, 1024, ini);
		if (buffer[0]) {
			wsprintfA(key, "attr%d", n);
			GetPrivateProfileStringA("LOGCUT", key, "ALL", attr, 1024, ini);
			set_cut(proc, pd, attr, buffer);
		}
	}

	return TRUE;
#else
	return FALSE;
#endif
}

EXPORT BOOL WINAPI FF11_LoadLogCutINIW(int index, wchar_t *ini)
{
#if LOG_CUT == 1
	char	bstr[MAX_PATH];

	if (ini == NULL)	return FALSE;

	WideCharToMultiByte(CP_ACP, 0, ini, -1, bstr, LEN_CMD_BUFFER, 0, 0);

	return FF11_LoadLogCutINIA(index, bstr);
#else
	return FALSE;
#endif
}

EXPORT int WINAPI FF11_GetCmdA(int index, char *buffer, int buffer_size, BYTE *fcut)
{
#if CMD_HOOK == 1
	int		res;
	res = get_cmd(index, buffer, NULL, buffer_size, fcut);
	return (res <= 0) ? res : (int)strlen(buffer);
#else
	return -1;
#endif
}

EXPORT int WINAPI FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size, BYTE *fcut)
{
#if CMD_HOOK == 1
	int		res;
	res = get_cmd(index, NULL, buffer, buffer_size, fcut);
	return (res <= 0) ? res : (int)_tcslen((LPCTSTR)buffer);
#else
	return -1;
#endif
}

EXPORT BOOL WINAPI FF11_PutCmdA(int index, char *command)
{
#if CMD_PUT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (command == NULL)					return FALSE;
	if (!check_index(index, &proc, &pd))	return FALSE;
	if (!(pd->fRdy & FF11_Rdy_PutCmd))		return FALSE;
	if (!check_login(proc->hPro, pd))		return FALSE;

	WaitForSingleObject(proc->hEvent, 10000);
	ResetEvent(proc->hEvent);
	strncpy_s(pd->nPrm, LEN_CMD_BUFFER, command, LEN_CMD_BUFFER-1);
	PostThreadMessage(pd->idThread, C_CMD, 0, 0);

	return TRUE;
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_PutCmdW(int index, wchar_t *command)
{
#if CMD_PUT == 1
	char	bstr[LEN_CMD_BUFFER];

	if (command) {
		WideCharToMultiByte(CP_ACP, 0, command, -1, bstr, LEN_CMD_BUFFER, 0, 0);
		return FF11_PutCmdA(index, bstr);
	}
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_KillEffect(int index, int id)
{
#if KILLEFFECT == 1
	PTPROC	proc;
	PTDATA	pd;

	if (id == 0)							return FALSE;
	if (!check_index(index, &proc, &pd))	return FALSE;
	if (!(pd->fRdy & FF11_Rdy_KillEffect))	return FALSE;
	if (!check_login(proc->hPro, pd))		return FALSE;

	WaitForSingleObject(proc->hEvent, 10000);
	ResetEvent(proc->hEvent);
	PostThreadMessage(pd->idThread, C_KILLEFFECT, id, 0);

	return TRUE;
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_MergeItem(int index, int object)
{
#if MERGEITEM == 1
	PTPROC	proc;
	PTDATA	pd;

	if (!check_index(index, &proc, &pd))	return FALSE;
	if (!(pd->fRdy & FF11_Rdy_MergeItem))	return FALSE;
	if (!check_login(proc->hPro, pd))		return FALSE;

	WaitForSingleObject(proc->hEvent, 10000);
	ResetEvent(proc->hEvent);
	PostThreadMessage(pd->idThread, C_MERGEITEM, object, 0);

	return TRUE;
#endif
	return FALSE;
}

EXPORT BOOL WINAPI FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int size)
{
	PTPROC	proc;
	PTDATA	pd;

	if (ptr != NULL && buffer != NULL && size > 0) {
		if (check_index(index, &proc, &pd))	{
			if (ReadProcessMemory(proc->hPro, ptr, buffer, size, NULL))
				return TRUE;
		}
	}

	ZeroMemory(buffer, size);

	return FALSE;
}

EXPORT BOOL WINAPI FF11_GetData(int index, int offset, BYTE *buffer, int size)
{
	PTPROC	proc;
	PTDATA	pd;
	DWORD	addr;
	PBYTE	ptr = NULL;

	if (buffer != NULL && size > 0) {
		if (check_index(index, &proc, &pd))	{
			switch (offset) {
			case FF11ADDR_ITEMTABLE:
			case FF11ADDR_EQUIPTABLE:
			case FF11ADDR_MAXBAG:
				if (ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_ITEMBASE], (BYTE*)&addr,4, NULL))
					ptr = pd->tPtr[offset] + addr;
				break;
			case FF11ADDR_NUMBAG:
				if (pd->tPtr[FF11ADDR_NUMBAG] == 0)
					break;
				if (ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_NUMBAG], (PBYTE*)&ptr, 4, NULL))
					ptr += 0x1c;
				else
					ptr = NULL;
				break;
			default :
				ptr = pd->tPtr[offset];
			}

			if (ptr && check_login(proc->hPro, pd)) {
				if (ReadProcessMemory(proc->hPro, ptr, buffer, size, NULL))
					return TRUE;
			}
		}
	}

	ZeroMemory(buffer, size);

	return FALSE;
}

EXPORT BOOL WINAPI FF11_GetCharInfo(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_JOBINFO, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetPTInfo(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_PTINFO, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetNPCMAP(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_NPCMAP, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetNPCData(int index, int id, BYTE *buffer, int size)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr;

	if (check_index(index, &proc, &pd))	{
		if (pd->tPtr[FF11ADDR_NPCMAP] && check_login(proc->hPro, pd)) {
			if (ReadProcessMemory(proc->hPro,pd->tPtr[FF11ADDR_NPCMAP]+(id*4),(PBYTE)&ptr,4,NULL)) {
				if (ReadProcessMemory(proc->hPro, ptr, buffer, size, NULL))
					return TRUE;
			}
		}
	}

	ZeroMemory(buffer, size);

	return FALSE;
}

EXPORT WORD WINAPI FF11_GetTargetId(int index)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr;
	WORD	id = 0;

	if (check_index(index, &proc, &pd))	{
		if (pd->tPtr[FF11ADDR_TARGETINFO] && id && check_login(proc->hPro, pd)) {
			if (ReadProcessMemory(proc->hPro,pd->tPtr[FF11ADDR_TARGETINFO],(PBYTE)&ptr,4,NULL)) {
				if (ptr)
					ReadProcessMemory(proc->hPro, ptr, &id, 2, NULL);
			}
		}
	}

	return id;
}

EXPORT BOOL WINAPI FF11_GetTargetInfoA(int index, WORD *id, WORD *sid, char *lastst)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr;
	DWORD	v;

	if (id)		*id = 0;
	if (sid)	*sid = 0;
	if (lastst)	*lastst = 0;

	if (!check_index(index, &proc, &pd))	return FALSE;

	if (pd->tPtr[FF11ADDR_TARGETINFO] && check_login(proc->hPro, pd)) {
		if (!ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_TARGETINFO], (PBYTE)&ptr, 4, NULL))
			return FALSE;
		if (ptr) {
			if (id) {
				if (!ReadProcessMemory(proc->hPro, ptr, id, 2, NULL))
					return FALSE;
			}

			if (sid) {
				if (!ReadProcessMemory(proc->hPro, ptr + 0x30, &v, 4, NULL))
					return FALSE;
				if (v != 0xffffffff) {
					*sid = 0xfffe;
				} else if (!ReadProcessMemory(proc->hPro, ptr + OFFSET_STARGET, sid, 2, NULL)) {
					return FALSE;
				}
			}

			if (lastst) {
				if (!ReadProcessMemory(proc->hPro, ptr + OFFSET_LASTST, lastst, 24, NULL))
					return FALSE;
				lastst[24] = 0;
			}
			return TRUE;
		}
	}

	return FALSE;
}

EXPORT BOOL WINAPI FF11_GetTargetInfoW(int index, WORD *id, WORD *sid, wchar_t *lastst)
{
	char buffer[32];

	if (FF11_GetTargetInfoA(index, id, sid, buffer)) {
		if (lastst)
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, lastst, 32);
		return TRUE;
	}

	return FALSE;
}

EXPORT BOOL WINAPI FF11_GetAbiTable(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_ABIITABLE, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetMagicTable(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_MAGICTABLE, buffer, size);
}

EXPORT WORD WINAPI FF11_GetAomaHold(int index)
{
	WORD hold = 0;

	FF11_GetData(index, FF11ADDR_AOMAHOLD, (BYTE*)&hold, 2);

	return hold;
}

EXPORT WORD WINAPI FF11_GetLockOn(int index)
{
	WORD lockon = 0;

	FF11_GetData(index, FF11ADDR_LOCKON, (BYTE*)&lockon, 2);

	return lockon;
}

EXPORT WORD WINAPI FF11_GetPetID(int index)
{
	WORD petid = 0;

	FF11_GetData(index, FF11ADDR_PETID, (BYTE*)&petid, 2);

	return petid;
}

EXPORT BYTE WINAPI FF11_GetWeatherId(int index)
{
	BYTE id = 0;

	FF11_GetData(index, FF11ADDR_WEATHER, &id, 1);

	return id;
}

EXPORT BOOL WINAPI FF11_GetLSNameA(int index, char *name)
{
	return FF11_GetData(index, FF11ADDR_LSNAME, (BYTE*)name, 24);
}

EXPORT BOOL WINAPI FF11_GetLSNameW(int index, wchar_t *name)
{
	char buf[33];

	if (FF11_GetLSNameA(index, buf)) {
		MultiByteToWideChar(CP_ACP, 0, buf, -1, name, 32);
		buf[32] = 0;
		return TRUE;
	}

	return FALSE;
}

EXPORT int WINAPI FF11_GetIcon(int index, WORD *buffer, int num)
{
	int		n = 0;

	if (FF11_GetData(index, FF11ADDR_ICONTABLE, (BYTE*)buffer, num*2)) {
		for (n = 0; n < num; n++, buffer++) {
			if (*buffer == 0xffff)
				break;
		}
		return n;
	}

	return -1;
}

EXPORT BOOL WINAPI FF11_GetMeripo(int index, short *rimipo, char *meripo)
{
	PTPROC	proc;
	PTDATA	pd;

	if (rimipo)		*rimipo = 0;
	if (meripo)		*meripo = 0;

	if (!check_index(index, &proc, &pd))	return FALSE;

	if (pd->tPtr[FF11ADDR_MERIPO] && check_login(proc->hPro, pd)) {
		if (rimipo) {
			if (!ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_MERIPO], (PBYTE)rimipo, 2, NULL))
				return FALSE;
		}

		if (meripo) {
			if (!ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_MERIPO]+2, (PBYTE)meripo, 1, NULL))
				return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

EXPORT WORD WINAPI FF11_GetActionId(int index)
{
	WORD id;

	FF11_GetData(index, FF11ADDR_ACTION, (BYTE*)&id, 2);

	return id;
}

EXPORT BOOL WINAPI FF11_GetTargetItem(int index, WORD *id, BYTE *info)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr;

	if (id)		*id = 0;
	if (info)	*info = 0;

	if (!check_index(index, &proc, &pd))	return FALSE;

	if (pd->tPtr[FF11ADDR_TARGETITEM] && check_login(proc->hPro, pd)) {
		if (!ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_TARGETITEM], (PBYTE)&ptr, 4, NULL))
			return FALSE;
		if (ptr) {
			if (id) {
				if (!ReadProcessMemory(proc->hPro, ptr+0x0024, id, 2, NULL))
					return FALSE;
			}
			if (info) {
				if (!ReadProcessMemory(proc->hPro, ptr+0x0288, info, 4, NULL))
					return FALSE;
			}
			return TRUE;
		}
	}

	return FALSE;
}

EXPORT BOOL WINAPI FF11_GetFList(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_FLIST, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetEquipTable(int index, BYTE *buffer)
{
	return FF11_GetData(index, FF11ADDR_EQUIPTABLE, buffer, 128);
}

EXPORT BOOL WINAPI FF11_GetItemTable(int index, BYTE *buffer, int size)
{
	return FF11_GetData(index, FF11ADDR_ITEMTABLE, buffer, size);
}

EXPORT BOOL WINAPI FF11_GetBag(int index, BYTE *num, BYTE *max)
{
	if (num)	*num = 0;
	if (max)	*max = 0;

	if (num) {
		if (FF11_GetData(index, FF11ADDR_NUMBAG, num, 1)) {
			if (*num)
				(*num)--;
		}
	}

	if (max) {
		if (FF11_GetData(index, FF11ADDR_MAXBAG, max, 1)) {
			if (*max)
				(*max)--;
		}
	}

	return TRUE;
}

EXPORT BOOL WINAPI FF11_IsArea(int index)
{
	PTPROC	proc;
	PTDATA	pd;
	DWORD	buf[16];

	if (!check_index(index, &proc, &pd))	return FALSE;

	if (pd->tPtr[FF11ADDR_ISAREA] && check_login(proc->hPro, pd)) {
		if (ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_ISAREA], buf, sizeof(buf), NULL)) {
			for (int n = 0; n < 16; n++) {
				if (buf[n])
					return TRUE;
			}
			return FALSE;
		}
	}

	return TRUE;
}

EXPORT BOOL WINAPI FF11_SetChoose(int index, int no)
{
	PTPROC	proc;
	PTDATA	pd;
	PBYTE	ptr;

	if (!check_index(index, &proc, &pd))	return FALSE;

	if (pd->tPtr[FF11ADDR_CHOOSE] && check_login(proc->hPro, pd)) {
		if (!ReadProcessMemory(proc->hPro, pd->tPtr[FF11ADDR_CHOOSE], (PBYTE)&ptr, 4, NULL))
			return FALSE;
		if (ptr) {
			ptr += ADJUST_CHOOSE;
			if (!WriteProcessMemory(proc->hPro, ptr, (PBYTE)&no, 1, NULL))
				return FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

EXPORT HWND WINAPI FF11_GetHWND(int index)
{
	PTPROC	proc;
	PTDATA	pd;
	PENUM	ep;

	if (!check_index(index, &proc, &pd))	return 0;

	ep.id = proc->idPro;
	ep.hWnd = 0;
	_tcscpy_s(ep.name, 128, _T("PlayOnlineMask"));

	EnumWindows(EnumWindowsProc , (LPARAM)&ep);

	proc->hWnd = ep.hWnd;

	return proc->hWnd;
}

// 未実装
EXPORT BOOL WINAPI FF11_Macro(int index, int type, int no)
{
#if MACROEXE == 1
	PTPROC	proc;
	PTDATA	pd;

	if (!check_index(index, &proc, &pd))	return FALSE;
	if (!(pd->fRdy & FF11_Rdy_Macro))		return FALSE;

	if (WaitForSingleObject(proc->hMtx, 3000) == WAIT_OBJECT_0) {

		WaitForSingleObject(proc->hEvent, 10000);
		PostThreadMessage(pd->idThread, C_MERGEITEM, 0, 0);

		ReleaseMutex(proc->hMtx);
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL DllAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	// このdllはpol.exeプロセス内なので内部からアタッチ処理を行う
	if (GetModuleHandle(POL_NAME) != NULL) {
		fPol = TRUE;
		init();
	}

	return TRUE;
}

BOOL DllDetach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (fPol == FALSE) {
		FF11_Eject(0);
		return TRUE;
	}

#ifdef USE_HOOK
	PBYTE pbase = FF11_GetBaseAddress(idPro);
#if LOG_HOOK == 1
	if (pILOG_HOOK) {
		if (pbase)
			detach(pILOG_HOOK);
		else
			delete pILOG_HOOK;
	}
#endif
#if CMD_HOOK == 1 || CMD_PUT == 1
	if (pICMD_HOOK) {
		if (pbase)
			detach(pICMD_HOOK);
		else
			delete pICMD_HOOK;
	}
#endif
#endif // USE_HOOK

	if (hMap) {
		if (pData) {
#ifdef USE_EXCHANGE
			if (pData->idThread) {
				if (pData->nInject != 0)
					PostThreadMessage(pData->idThread, C_END, 0, 0);
			}
#endif // USE_EXCHENGE
			pData->fRdy = 0;
			pData->fLD_hook = FALSE;
			UnmapViewOfFile(pData);
		}
		CloseHandle(hMap);
	}

#ifdef USE_EXCHANGE
	if (hEvent)
		CloseHandle(hEvent);
#endif // USE_EXCHENGE

	if (hMtx)
		CloseHandle(hMtx);

	return TRUE;
}

BOOL __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason) {
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		return DllAttach(hModule, dwReason, lpReserved);
	case DLL_PROCESS_DETACH:
		return DllDetach(hModule, dwReason, lpReserved);
	}
	return TRUE;
}

int initDlg(HWND hDlgWnd)
{
	int n, cnt;
    for (cnt = 0, n = 0; n < MAX_PROCESS; n++)
    	if (tPList[n].sName[0])
	        SendMessage(GetDlgItem(hDlgWnd, IDC_LIST1), LB_INSERTSTRING, (WPARAM)cnt++, (LPARAM)tPList[n].sName);
    return 0;
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	int		w, h, nWidth, nHeight, res;
	RECT	rc;

	switch (msg) {
	case WM_INITDIALOG:
		initDlg(hDlg);

		GetWindowRect(hDlg, &rc);

		nWidth  = GetSystemMetrics(SM_CXFULLSCREEN);
		nHeight = GetSystemMetrics(SM_CYFULLSCREEN);

		w = rc.right - rc.left;
		h = rc.bottom - rc.top;

		MoveWindow(hDlg, (nWidth-w)/2, (nHeight-h)/2, w, h, FALSE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		case IDC_LIST1:
			if (HIWORD(wp) == LBN_DBLCLK){
				res = (int)(DWORD)SendMessage(GetDlgItem(hDlg, IDC_LIST1), LB_GETCURSEL, 0, 0);
				EndDialog(hDlg, res);
			}
			break;
		default:
			return FALSE;
		}
	}
	return FALSE;
}
