#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <process.h>

// デバッグログはコマンドバッファと共通。

// コンフィグ
#define LOG_HOOK		1				// ログをフックする(0-OFF/1-ON)
#define LOG_DECODE		1				// ログの不要な制御コード排除(0-OFF/1-ON)
#define LOG_CUT			1				// ログカット機能(0-OFF/1-ON)
#define CMD_HOOK		1				// コマンドをフックする(0-OFF/1-ON)
#define	CMD_PUT			1				// 外部からコマンド実行する機能(0-OFF/1-ON)
#define DBG_DLL			1				// デバッグ(0-OFF/1-ON)

// 定数
#define MAX_PROCESS		18				// フックできるPOLの数
#define MAX_INJECT		32				// 1つのPOLにインジェクトできる数
#define MAX_INDEX		(MAX_PROCESS * MAX_INJECT + 1)

#define SIZE_LOG_FIFO	1000			// ログFIFOの深さ
#define LEN_LOG_BUFFER	1000			// ログ文字列長
#define MAX_LOGCUT		200				// 1つのPOLプロセスに登録できるログカット情報数

#define SIZE_CMD_FIFO	100				// コマンドFIFOの深さ
#define LEN_CMD_BUFFER	100				// コマンド文字列長

#define SEARCH_RANGE	0x300000		// baseからの検索範囲

#define C_CMD			(WM_USER + 1)
#define C_END			(WM_USER + 9)

#include "ffhook.h"
#include "madChook.h"

#pragma comment(lib, "madCHook")

#define POL_NAME		_T("pol.exe")
#define DLL_NAME		_T("FFXiMain.dll")
#define MAP_NAME		_T("FFHOOK_MAP_%d")		// マップファイル名(pid)
#define EVENT_NAME		_T("FFHOOK_EVENT_%d")	// イベント名(pid)
#define MTX_NAME		_T("FFHOOK_MTX_%d")		// Mutex名(pid)

// デバッグ用
#if DBG_DLL == 1
#define	SLwsprintfA		_stprintf
#else
#define	SLwsprintfA
#define syslog
#endif

#if CMD_PUT == 1
#define USE_EXCHANGE
#endif

#if LOG_HOOK == 1 || CMD_HOOK == 1 || CMD_PUT == 1
#define USE_HOOK
#endif

// ログFIFO
typedef struct {
	BYTE	tAttr;						// アトリビュート格納用
	BYTE	fCut;						// カットされたログ
	char	Buffer[LEN_LOG_BUFFER];		// ログバッファ
	wchar_t	BufferW[LEN_LOG_BUFFER];	// ログバッファ
} TLOG;

// ログカット情報
typedef struct _tcut {
	BYTE	tAttr[256];					// アトリビュートテーブル
	int		nLen;						// ログカット文字列長
	char	sCut[LEN_LOG_BUFFER];		// ログカット文字列
} TCUT, *PTCUT;

// コマンドFIFO
typedef struct {
	TCHAR	Buffer[LEN_CMD_BUFFER];
	wchar_t	BufferW[LEN_CMD_BUFFER];
} TCMD;

// 共有メモリ情報テーブル
// FIFOバッファはプロセスに１個とし、読込みポインタだけ独立させる。
// 読み遅れると古いログから消される。
// 各アプリ間で整合性を保つために各機能未使用時でもテーブルは固定にする。
typedef struct {
	BOOL	fLD_madC;				// madChook.dllをpol.exeにインジェクトしたか？(TRUE)
	BOOL	fLD_hook;				// このdllをpol.exeにインジェクトしたか？(TRUE)

	int		fRdy;					// 各種準備完了フラグ
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
	int		pReadCmd[MAX_INJECT];	// コマンドFIFO 読込みポインタ
	TCMD	tCmd[SIZE_CMD_FIFO];	// コマンドバッファ

	HANDLE			hThread;		// スレッドハンドル用
	unsigned int	idThread;		// スレッドID
	char	nPrm[LEN_LOG_BUFFER];	// コマンド依存パラメータ引渡し用

	HWND	hWnd;					// FF窓のハンドル
	PBYTE	tPtr[50];				// 各アドレス格納用(FF11_GetAddressで使用)
} TDATA, *PTDATA;

// プロセス別管理テーブル
typedef struct {
	int		idPro;					// プロセスID
	HANDLE	hPro;					// プロセスハンドル
	HANDLE	hMap;					// 共有メモリマップハンドル
	HANDLE	hMtx;					// Mutexハンドル
#ifdef USE_EXCHANGE
	HANDLE	hEvent;					// イベント用
#endif
	PTDATA	pData;					// 共有メモリ情報テーブルへのポインタ
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

// アプリ側で使用する
static TPROC	tProcess[MAX_PROCESS];	// プロセス管理テーブル
static TINDEX	tIndex[MAX_INDEX];		// インデックス管理テーブル
static int		nIndex = 0;				// 使用中インデックス数

										// POLにインジェクトされたDLL側で使用する
static BOOL		fPol = FALSE;		// POL側の時TRUEになる
static int		idPro = 0;			// プロセスID
static HANDLE	hMap = 0;			// 共有メモリのハンドル
static HANDLE	hMtx = 0;			// 排他処理
#ifdef USE_EXCHANGE
static HANDLE	hEvent = 0;			// イベント用
#endif
static PTDATA	pData = 0;			// 共有メモリ情報テーブルへのポインタ

									// アタッチするアドレス検索用パターン
#define MAX_PATTERN		256

#if LOG_HOOK == 1
static PIHOOK	pILOG_HOOK = NULL;
TCHAR pattern_log[MAX_PATTERN] = _T("E8........8B470C8B5424184089470C84DB742E");	//2017.4.4
void(__stdcall *pNextLogHook)(char*, BYTE *, int, int);
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
static PIHOOK	pICMD_HOOK = NULL;
TCHAR pattern_cmd[MAX_PATTERN] = _T("............8B8C24080400005556578D84241002000068000200005033ED51");	//2017.4.4
void(*pNextCmdHook)(char*, int);
#endif

TCHAR pattern_job[] = _T("F605..........74..8A0Dxxxxxxxx80F9");	//2017.4.4
TCHAR pattern_ptinfo[] = _T("84C974098D80xxxxxxxxC2040033C0C2..00");	//2017.4.4
#define ADJUST_PTINFO		6


// -----------------------------------------------------
// POL側で使用する関数
// -----------------------------------------------------
#if LOG_HOOK == 1

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
		for (n = 0; n < LEN_LOG_BUFFER - 1 && *str; n++) {
			if (*str == 0x1f || *str == 0x7f) {
				// 制御文字なら2文字分スキップ
				if (*(++str) != 0) {
					str++;
					n++;
					continue;
				}
			}
			else if (*(unsigned char*)str == 0xef) {
				// タブ変換文字列
				if (*(str + 1) == 0x27 || *(str + 1) == 0x28) {
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
	_tcsncpy_s(pData->tLog[wp].Buffer, LEN_LOG_BUFFER - 1, str, LEN_LOG_BUFFER - 2);
#endif
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

				if (p->sCut[0] == 0) {
					// 本文なし
					pData->tLog[wp].fCut = 1;
					res = FALSE;
					break;
				}
				else if (p->sCut[0] == '^') {
					// 前方一致
					if (!strncmp(ptr, &p->sCut[1], p->nLen)) {
						pData->tLog[wp].fCut = 1;
						res = FALSE;
						break;
					}
				}
				else {
					if (strstr(ptr, p->sCut)) {
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

	rp = (wp == SIZE_LOG_FIFO - 1) ? 0 : wp + 1;
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
		mov     eax, dword ptr[esp + 14h]
		mov     ebx, dword ptr[esp + 18h]
		push	eax
		push	ebx
		call	hook_log
		add		esp, 8h
		pop		edx
		pop		ecx
		pop		ebx
		test	eax, eax
		jne		next_func
		pop		eax
		ret		10h
		next_func :
		pop		eax
			jmp		pNextLogHook
	}
}
#endif


#if CMD_HOOK == 1 || DBG_DLL == 1

// コマンドもしくはデバッグログをFIFOに書込む
// str			ログ本文
void hook_cmd(LPTSTR str)
{
	int		n, wp, rp, inject;
	BOOL	res = TRUE;
	TCHAR	*ptr;

	if (hMtx == 0 || str == NULL)
		return;

	WaitForSingleObject(hMtx, INFINITE);

	wp = pData->pWriteCmd;

	_tcsncpy_s(pData->tCmd[wp].Buffer, LEN_CMD_BUFFER - 1, str, LEN_CMD_BUFFER - 2);

	ptr = pData->tCmd[wp].Buffer;
	pData->tCmd[wp].BufferW[0] = 0;

	if (++wp >= SIZE_CMD_FIFO)
		wp = 0;

	pData->pWriteCmd = wp;

	rp = (wp == SIZE_CMD_FIFO - 1) ? 0 : wp + 1;
	inject = pData->nInject;

	// ライトポインタがリードポインタに追いついていたらリードポインタをインクリメント
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		if (pData->fEna[n]) {
			// 有効なテーブルだけチェックする
			if (pData->pReadCmd[n] == wp)
				pData->pReadCmd[n] = rp;
			inject--;
		}
	}

	ReleaseMutex(hMtx);
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
		mov     eax, dword ptr[esp + 14h]
		push	eax
		call	hook_cmd
		add		esp, 4h
		pop		edx
		pop		ecx
		pop		ebx
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
		ptr = (PBYTE)(DWORD64)(*(DWORD *)(pCode + 1));
		ptr = (PBYTE)((DWORD64)pCode + (DWORD64)ptr + 5);
		*pNextHook = ptr;
	}
	else {
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
			ptr = pCode + 1;
			addr = (DWORD)((DWORD64)pCallBackFunc - (DWORD64)pCode - 5);
			*(DWORD *)(pCode + 1) = addr;
		}
		else {
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
void syslog(LPTSTR str)
{
	TCHAR buf[512];
	_stprintf(buf, _T("system log:%s"), str);
	hook_cmd(buf);
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
	if (addr)	*addr = 0;
	if (adj)	*adj = 0;

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
			mask[cnt] = 1;		// マスクパターン
		else if (c == 'x')
			mask[cnt] = 2;		// アドレス部
		else if (c == 'p')
			mask[cnt] = 3;		// オフセット部
		else if ('0' <= c && c <= '9')
			dat |= (c - '0');
		else if ('a' <= c && c <= 'f')
			dat |= (c - 'a' + 10);
		else
			return 0;

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		}
		else
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
						*addr = *(DWORD *)(p1 - fp + n);
						break;
					}
				}
			}
			if (adj) {
				for (n = 0; n < cnt; n++) {
					if (mask[n] == 3) {
						*adj = *(DWORD *)(p1 - fp + n);
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
// アプリからPOLにコマンドを渡す
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
			// ログイン判定をしてコマンドを発行するべきだが、
			// まだログイン判定に使用できるところを探していない
			if (pData->tPtr[FF11ADDR_PTINFO]) {
				if (*pData->tPtr[FF11ADDR_PTINFO] == 0)
					break;
			}
			if (pData->nPrm)
				pNextCmdHook(pData->nPrm, 1);
			break;
#endif
		case C_END:
			fend = TRUE;
			break;
		}

		SetEvent(hEvent);
	}

	_endthread();
}
#endif // USE_EXCHENGE

BOOL init(void)
{
	TCHAR	obj_name[256];
	TCHAR	mes[256];
	BOOL	res = TRUE;
	PVOID	ptr = NULL;

	if (pData) {
		syslog(_T("初期済み?"));
		return TRUE;	// 初期化済み
	}

	idPro = GetCurrentProcessId();

	_stprintf(obj_name, MAP_NAME, idPro);
	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, obj_name);
	if (hMap == 0)		return FALSE;

	_stprintf(obj_name, MTX_NAME, idPro);
	hMtx = OpenMutex(MUTEX_ALL_ACCESS, NULL, obj_name);
	if (hMtx == 0)		return FALSE;

#ifdef USE_EXCHANGE
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
	if (hEvent == 0)	return FALSE;
#endif // USE_EXCHENGE

	pData = (PTDATA)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pData == NULL)	return FALSE;

	SLwsprintfA(mes, _T("プロセスID = %d"), idPro);
	syslog(mes);
	SLwsprintfA(mes, _T("baseアドレス = %08x"), pData->pBase);
	syslog(mes);

	// FFxiMain.Dllが見つかっていないのでアタッチできない
	if (pData->pBase == NULL)	return FALSE;

#if LOG_HOOK == 1
	{
		ptr = search_addr(pData->pBase, pattern_log);

		SLwsprintfA(mes, _T("ログアタッチアドレス = %08x"), ptr);
		syslog(mes);

		if (ptr)
			pILOG_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookLog, (PBYTE*)&pNextLogHook);

		if (pILOG_HOOK) {
			pData->fRdy |= FF11_Rdy_GetLog;
			syslog(_T("ログアタッチ成功!"));
		}
		else {
			syslog(_T("ログアタッチ失敗!"));
		}
	}
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
	{
		ptr = search_addr(pData->pBase, pattern_cmd);

		SLwsprintfA(mes, _T("コマンドアタッチアドレス = %08x"), ptr);
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
			syslog(_T("コマンドアタッチ成功!"));
		}
		else {
			syslog(_T("コマンドアタッチ失敗!"));
		}
	}
#endif

#ifdef USE_EXCHANGE
	if (pData->hThread = (HANDLE)_beginthread(exchange, 0, NULL)) {
#if CMD_PUT == 1
		if (pICMD_HOOK)
			pData->fRdy |= FF11_Rdy_PutCmd;
#endif
	}
#endif // USE_EXCHENGE

	DWORD addr;

	// ベースポインタ
	pData->tPtr[FF11ADDR_BASE] = pData->pBase;

	// キャラ情報
	if (search_addr(pData->pBase, pattern_job, &addr))
		pData->tPtr[FF11ADDR_JOBINFO] = (PBYTE)(DWORD64)addr;

	// パーティ情報
	if (search_addr(pData->pBase, pattern_ptinfo, &addr)) {
		addr += ADJUST_PTINFO;
		pData->tPtr[FF11ADDR_PTINFO] = (PBYTE)(DWORD64)addr;
	}

	return TRUE;
}

// -----------------------------------------------------
// アプリ側で使用する関数
// -----------------------------------------------------
EXPORT DWORD WINAPI FF11_GetProcessId(int no)
{
	PROCESSENTRY32	pe;
	HANDLE	hps;
	BOOL	res;
	DWORD	id = 0;
	int		n, cnt = 0;

	if (no < 0 || MAX_PROCESS <= no)		return 0;

	// POL.EXEプロセス検索
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);

	hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hps == INVALID_HANDLE_VALUE)	return 0;

	res = Process32First(hps, &pe);

	while (res) {
		if (!lstrcmp(POL_NAME, pe.szExeFile)) {
			// POL発見
			id = pe.th32ProcessID;
			for (n = 0; n < MAX_PROCESS; n++) {
				if (tProcess[n].idPro == id)
					break;
			}

			if (n == MAX_PROCESS) {
				for (; cnt < MAX_PROCESS; cnt++) {
					if (tProcess[cnt].idPro == 0) {
						tProcess[cnt++].idPro = id;
						break;
					}
				}
				if (cnt >= MAX_PROCESS)
					break;
			}
		}
		res = Process32Next(hps, &pe);
	}

	CloseHandle(hps);

	return tProcess[no].idPro;
}

EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD id)
{
	MODULEENTRY32	me;
	HANDLE	hms;
	BOOL	res;
	PBYTE	ptr = NULL;

	if (id == 0)	return NULL;

	// FFXiMain.dllを探す
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	hms = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
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

EXPORT int WINAPI FF11_Inject(DWORD pid)
{
	DWORD64	hpro;
	PTDATA	pd;
	PTPROC	proc = NULL;
	int		index, n, res = -1;
	TCHAR	obj_name[256];
	PBYTE	pbase;

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

		proc->pData = (PTDATA)MapViewOfFile(proc->hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
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
		if (InjectLibrary((DWORD)hpro, _T("ffhook.dll")))
			pd->fLD_hook = TRUE;
	}

	WaitForSingleObject(proc->hMtx, INFINITE);

	// 空きを探して確保
	for (n = 0; n < MAX_INJECT; n++) {
		if (!pd->fEna[n]) {
			pd->fEna[n] = TRUE;
#if HOOK_DLL == 1
			pd->pReadLog[n] = pd->pWriteLog;
#endif
#if CMD_HOOK == 1 || DBG_DLL == 1
			// 最初のinjectならシステムログを取るためにFIFOクリアをしない
#if DBG_DLL == 1
			if (pd->nInject != 0)
#endif
				pd->pReadCmd[n] = pd->pWriteCmd;
#endif
			tIndex[index].pInject = n;
			pd->nInject++;
			break;
		}
	}

	ReleaseMutex(proc->hMtx);

	return index;

err_end:

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

// index値からprocとpdを求める。おかしかったらNULLを返す
BOOL check_index(int index, PTPROC *proc, PTDATA *pd)
{
	if (index <= 0 || MAX_INDEX <= index)		return NULL;
	if ((*proc = tIndex[index].pProc) == NULL)	return NULL;
	if ((*pd = (*proc)->pData) == NULL)			return NULL;

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

		// プロセス最後のイジェクトなのでいろいろ開放する
		if (--pd->nInject <= 0) {
			if (proc->idPro) {
				if (hpro = (DWORD64)proc->hPro) {
					if (pd->fLD_hook)
						UninjectLibrary((DWORD)hpro, _T("ffhook.dll"));
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

EXPORT int WINAPI FF11_Rdy(int index)
{
	PTPROC	proc;
	PTDATA	pd;

	if (!check_index(index, &proc, &pd))
		return 0;

	if (!check_dll(proc->hPro)) {
		FF11_Eject(index);
		return -1;
	}

	return pd->fRdy;
}

EXPORT PBYTE WINAPI FF11_GetAddress(int index, int object)
{
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd)) {
		if ((DWORD)object <= FF11ADDR_MAXBAG)
			return pd->tPtr[object];
	}

	return NULL;
}

#if LOG_HOOK == 1
int get_log(int index, BYTE *attr, char *buf, wchar_t *wbuf, int sbuf, BYTE *fcut)
{
	PTPROC	proc;
	PTDATA	pd;
	int		p, no, res = 0;

	if (sbuf < 2 || (buf == NULL && wbuf == NULL))	return -1;

	if (!check_index(index, &proc, &pd))
		return -1;
	if (!(pd->fRdy & FF11_Rdy_GetLog))		return 0;

	WaitForSingleObject(proc->hMtx, INFINITE);

	no = tIndex[index].pInject;
	if (pd->fEna[no]) {
		p = pd->pReadLog[no];
		if (p != pd->pWriteLog) {
			if (buf) {
				strncpy_s(buf, sbuf, pd->tLog[p].Buffer, sbuf - 1);
				buf[sbuf - 1] = '\0';		// 念のため終端コード
				res = (int)strlen(buf);
			}
			else {
				if (pd->tLog[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tLog[p].Buffer, -1, pd->tLog[p].BufferW,
						sbuf - 1);
				wcsncpy_s(wbuf, sbuf, pd->tLog[p].BufferW, sbuf - 1);
				wbuf[sbuf - 1] = '\0';	// 念のため終端コード
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

#if CMD_HOOK == 1 || DBG_DLL == 1
int get_cmd(int index, LPTSTR buf, wchar_t *wbuf, int sbuf)
{
	PTPROC	proc;
	PTDATA	pd;
	int		p, no, res = 0;

	if (sbuf < 2 || (buf == NULL && wbuf == NULL))	return -1;
	if (!check_index(index, &proc, &pd))
		return -1;
#if DBG_DLL != 1
	if (!(pd->fRdy & FF11_Rdy_GetCmd))		return 0;
#endif

	WaitForSingleObject(proc->hMtx, INFINITE);

	no = tIndex[index].pInject;
	if (pd->fEna[no]) {
		p = pd->pReadCmd[no];
		if (p != pd->pWriteCmd) {
			if (buf) {
				_tcsncpy_s(buf, sbuf, pd->tCmd[p].Buffer, sbuf - 1);
				buf[sbuf - 1] = '\0';		// 念のため終端コード
				res = (int)_tcslen(buf);
			}
			else {
				if (pd->tCmd[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, (LPCCH)pd->tCmd[p].Buffer, -1, pd->tCmd[p].BufferW,
						sbuf - 1);
				wcsncpy_s(wbuf, sbuf, pd->tCmd[p].BufferW, sbuf - 1);
				wbuf[sbuf - 1] = '\0';	// 念のため終端コード
				res = (int)_tcslen(wbuf);
			}

			if (++p >= SIZE_LOG_FIFO)
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
#if LOG_HOOK == 1
	return get_log(index, attr, buffer, NULL, buffer_size, fcut);
#else
	return -1;
#endif
}

EXPORT int WINAPI FF11_GetLogW(int index, BYTE *attr, wchar_t *buffer, int buffer_size, BYTE *fcut)
{
#if LOG_HOOK == 1
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

	if (check_index(index, &proc, &pd)) {
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
		if (str)
			strncpy_s(pcut->sCut, LEN_LOG_BUFFER, str, LEN_LOG_BUFFER - 1);
		pcut->sCut[LEN_LOG_BUFFER - 1] = 0;
		pcut->nLen = (int)strlen(pcut->sCut);

		if (attr == NULL) {
			FillMemory(pcut->tAttr, 256, 1);
		}
		else {
			ZeroMemory(pcut->tAttr, 256);

			strncpy_s(buf, 512, attr, 511);
			buf[511] = 0;
			p1 = buf;

			while (p1 && *p1) {
				if (p2 = strpbrk(p1, " ,\0")) {
					c = *p2;
					*p2++ = 0;
				}
				else {
					c = 0;
				}
				if (!_stricmp(p1, "ALL")) {
					FillMemory(pcut->tAttr, 256, 1);
				}
				else {
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

EXPORT int WINAPI FF11_GetCmdA(int index, LPTSTR buffer, int buffer_size)
{
#if CMD_HOOK == 1 || DBG_DLL == 1
	int		res;
	res = get_cmd(index, buffer, NULL, buffer_size);
	return (res <= 0) ? res : (int)_tcslen(buffer);
#else
	return -1;
#endif
}

EXPORT int WINAPI FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size)
{
#if CMD_HOOK == 1 || DBG_DLL == 1
	int		res;
	res = get_cmd(index, NULL, buffer, buffer_size);
	return (res <= 0) ? res : (int)_tcslen(buffer);
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

	WaitForSingleObject(proc->hEvent, 10000);
	ResetEvent(proc->hEvent);
	strncpy_s(pd->nPrm, LEN_CMD_BUFFER, command, LEN_CMD_BUFFER - 1);
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

EXPORT BOOL WINAPI FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int len)
{
	PTPROC	proc;
	PTDATA	pd;

	if (check_index(index, &proc, &pd)) {
		if (ReadProcessMemory(proc->hPro, ptr, buffer, len, NULL))
			return TRUE;
	}
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
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: return DllAttach(hModule, dwReason, lpReserved);
	case DLL_PROCESS_DETACH: return DllDetach(hModule, dwReason, lpReserved);
	}
	return TRUE;
}
