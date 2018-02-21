#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <process.h>

// �f�o�b�O���O�̓R�}���h�o�b�t�@�Ƌ��ʁB

// �R���t�B�O
#define LOG_HOOK		1				// ���O���t�b�N����(0-OFF/1-ON)
#define LOG_DECODE		1				// ���O�̕s�v�Ȑ���R�[�h�r��(0-OFF/1-ON)
#define LOG_CUT			1				// ���O�J�b�g�@�\(0-OFF/1-ON)
#define CMD_HOOK		1				// �R�}���h���t�b�N����(0-OFF/1-ON)
#define	CMD_PUT			1				// �O������R�}���h���s����@�\(0-OFF/1-ON)
#define DBG_DLL			1				// �f�o�b�O(0-OFF/1-ON)

// �萔
#define MAX_PROCESS		18				// �t�b�N�ł���POL�̐�
#define MAX_INJECT		32				// 1��POL�ɃC���W�F�N�g�ł��鐔
#define MAX_INDEX		(MAX_PROCESS * MAX_INJECT + 1)

#define SIZE_LOG_FIFO	1000			// ���OFIFO�̐[��
#define LEN_LOG_BUFFER	1000			// ���O������
#define MAX_LOGCUT		200				// 1��POL�v���Z�X�ɓo�^�ł��郍�O�J�b�g���

#define SIZE_CMD_FIFO	100				// �R�}���hFIFO�̐[��
#define LEN_CMD_BUFFER	100				// �R�}���h������

#define SEARCH_RANGE	0x300000		// base����̌����͈�

#define C_CMD			(WM_USER + 1)
#define C_END			(WM_USER + 9)

#include "ffhook.h"
#include "madChook.h"

#pragma comment(lib, "madCHook")

#define POL_NAME		_T("pol.exe")
#define DLL_NAME		_T("FFXiMain.dll")
#define MAP_NAME		_T("FFHOOK_MAP_%d")		// �}�b�v�t�@�C����(pid)
#define EVENT_NAME		_T("FFHOOK_EVENT_%d")	// �C�x���g��(pid)
#define MTX_NAME		_T("FFHOOK_MTX_%d")		// Mutex��(pid)

// �f�o�b�O�p
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

// ���OFIFO
typedef struct {
	BYTE	tAttr;						// �A�g���r���[�g�i�[�p
	BYTE	fCut;						// �J�b�g���ꂽ���O
	char	Buffer[LEN_LOG_BUFFER];		// ���O�o�b�t�@
	wchar_t	BufferW[LEN_LOG_BUFFER];	// ���O�o�b�t�@
} TLOG;

// ���O�J�b�g���
typedef struct _tcut {
	BYTE	tAttr[256];					// �A�g���r���[�g�e�[�u��
	int		nLen;						// ���O�J�b�g������
	char	sCut[LEN_LOG_BUFFER];		// ���O�J�b�g������
} TCUT, *PTCUT;

// �R�}���hFIFO
typedef struct {
	TCHAR	Buffer[LEN_CMD_BUFFER];
	wchar_t	BufferW[LEN_CMD_BUFFER];
} TCMD;

// ���L���������e�[�u��
// FIFO�o�b�t�@�̓v���Z�X�ɂP�Ƃ��A�Ǎ��݃|�C���^�����Ɨ�������B
// �ǂݒx���ƌÂ����O����������B
// �e�A�v���ԂŐ�������ۂ��߂Ɋe�@�\���g�p���ł��e�[�u���͌Œ�ɂ���B
typedef struct {
	BOOL	fLD_madC;				// madChook.dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)
	BOOL	fLD_hook;				// ����dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)

	int		fRdy;					// �e�폀�������t���O
	PBYTE	pBase;					// FFXiMain.dll�x�[�X�A�h���X
	int		nInject;				// ���̃v���Z�X�Ŏg�p����index��
	BYTE	fEna[MAX_INJECT];		// �g�p���̃e�[�u����1�ɂȂ�

	int		pWriteLog;				// ���OFIFO �����݃|�C���^
	int		pReadLog[MAX_INJECT];	// ���OFIFO �Ǎ��݃|�C���^(�e�[�u�����ɓƗ�)
	TLOG	tLog[SIZE_LOG_FIFO];	// ���O�o�b�t�@

	BYTE	fCut;					// ���O�J�b�g��ON/OFF
	int		nCut;					// ���O�J�b�g���o�^��
	TCUT	tCut[MAX_LOGCUT];		// ���O�J�b�g���

	int		pWriteCmd;				// �R�}���hFIFO �����݃|�C���^
	int		pReadCmd[MAX_INJECT];	// �R�}���hFIFO �Ǎ��݃|�C���^
	TCMD	tCmd[SIZE_CMD_FIFO];	// �R�}���h�o�b�t�@

	HANDLE			hThread;		// �X���b�h�n���h���p
	unsigned int	idThread;		// �X���b�hID
	char	nPrm[LEN_LOG_BUFFER];	// �R�}���h�ˑ��p�����[�^���n���p

	HWND	hWnd;					// FF���̃n���h��
	PBYTE	tPtr[50];				// �e�A�h���X�i�[�p(FF11_GetAddress�Ŏg�p)
} TDATA, *PTDATA;

// �v���Z�X�ʊǗ��e�[�u��
typedef struct {
	int		idPro;					// �v���Z�XID
	HANDLE	hPro;					// �v���Z�X�n���h��
	HANDLE	hMap;					// ���L�������}�b�v�n���h��
	HANDLE	hMtx;					// Mutex�n���h��
#ifdef USE_EXCHANGE
	HANDLE	hEvent;					// �C�x���g�p
#endif
	PTDATA	pData;					// ���L���������e�[�u���ւ̃|�C���^
} TPROC, *PTPROC;

// �C���f�b�N�X�Ǘ��e�[�u��(index=0�͎g��Ȃ����ɂ���)
typedef struct {
	PTPROC	pProc;					// �^�[�Q�b�g�v���Z�X
	int		pInject;				// �C���W�F�N�g�ԍ�
} TINDEX;

// �A�^�b�`���
typedef struct _ihook_ {
	BOOL	fAttach;				// �A�^�b�`������TRUE
	PBYTE	pCode;					// �A�^�b�`���閽�߂�����A�h���X
	PBYTE	pHook;					// �t�b�N�A�h���X
	PBYTE	pNext;					// ��ѐ�
	BYTE	func[12];
} IHOOK, *PIHOOK;

// �A�v�����Ŏg�p����
static TPROC	tProcess[MAX_PROCESS];	// �v���Z�X�Ǘ��e�[�u��
static TINDEX	tIndex[MAX_INDEX];		// �C���f�b�N�X�Ǘ��e�[�u��
static int		nIndex = 0;				// �g�p���C���f�b�N�X��

										// POL�ɃC���W�F�N�g���ꂽDLL���Ŏg�p����
static BOOL		fPol = FALSE;		// POL���̎�TRUE�ɂȂ�
static int		idPro = 0;			// �v���Z�XID
static HANDLE	hMap = 0;			// ���L�������̃n���h��
static HANDLE	hMtx = 0;			// �r������
#ifdef USE_EXCHANGE
static HANDLE	hEvent = 0;			// �C�x���g�p
#endif
static PTDATA	pData = 0;			// ���L���������e�[�u���ւ̃|�C���^

									// �A�^�b�`����A�h���X�����p�p�^�[��
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
// POL���Ŏg�p����֐�
// -----------------------------------------------------
#if LOG_HOOK == 1

// ���O��FIFO�ɏ�����
// attribute	�A�g���r���[�g
// str			���O�{��
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
				// ���䕶���Ȃ�2�������X�L�b�v
				if (*(++str) != 0) {
					str++;
					n++;
					continue;
				}
			}
			else if (*(unsigned char*)str == 0xef) {
				// �^�u�ϊ�������
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
				if (p->tAttr[*attr] == 0)	continue;	// �A�g���r���[�g�s��v

				if (p->sCut[0] == 0) {
					// �{���Ȃ�
					pData->tLog[wp].fCut = 1;
					res = FALSE;
					break;
				}
				else if (p->sCut[0] == '^') {
					// �O����v
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

	// ���C�g�|�C���^�����[�h�|�C���^�ɒǂ����Ă����烊�[�h�|�C���^���C���N�������g
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		if (pData->fEna[n]) {
			// �L���ȃe�[�u�������`�F�b�N����
			if (pData->pReadLog[n] == wp)
				pData->pReadLog[n] = rp;
			inject--;
		}
	}

	ReleaseMutex(hMtx);

	return res;
}

// �t�b�N�֐�
__declspec(naked) void __stdcall fnHookLog(char *str, BYTE *attr, int dummy1, int dummy2)
{
	// ���W�X�^��ޔ����Ȃ��ƃn���O����
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

// �R�}���h�������̓f�o�b�O���O��FIFO�ɏ�����
// str			���O�{��
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

	// ���C�g�|�C���^�����[�h�|�C���^�ɒǂ����Ă����烊�[�h�|�C���^���C���N�������g
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		if (pData->fEna[n]) {
			// �L���ȃe�[�u�������`�F�b�N����
			if (pData->pReadCmd[n] == wp)
				pData->pReadCmd[n] = rp;
			inject--;
		}
	}

	ReleaseMutex(hMtx);
}

#if CMD_HOOK == 1
// �R�}���h�t�b�N�֐�
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

// �A�^�b�`����
PIHOOK attach(PBYTE pCode, PBYTE pCallBackFunc, PBYTE *pNextHook)
{
	PIHOOK	pi;
	PBYTE	src, dis, ptr;

	if (pCode == NULL || pNextHook == NULL)
		return NULL;	// �����ُ�

	if ((pi = new IHOOK) == NULL)
		return NULL;	// �������m�ێ��s

	ZeroMemory(pi, sizeof(IHOOK));

	pi->pCode = pCode;
	src = pCode;
	dis = pi->func;

	// 6�o�C�g���̖��߂��R�s�[
	for (int n = 0; n < 6; n++)
		*dis++ = *src++;

	// �{����Ԃׂ��A�h���X���v�Z���ăZ�b�g
	if (*pCode == 0xe8) {
		// call�����ɃA�^�b�`
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

		// �A�^�b�`����
		if (*pCode == 0xe8) {
			// call�����ɃA�^�b�`
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

// �f�^�b�`����
void detach(PIHOOK pihook)
{
	PBYTE	src, dis;

	if (pihook == NULL)
		return;		// �����ُ�

	if (pihook->fAttach) {
		WaitForSingleObject(hMtx, INFINITE);

		src = pihook->func;
		dis = pihook->pCode;

		// ���ɖ߂�
		for (int n = 0; n < 6; n++)
			*dis++ = *src++;

		ReleaseMutex(hMtx);
	}

	delete pihook;
}

#endif

#if DBG_DLL == 1
// �f�o�b�O�p���O�o��
void syslog(LPTSTR str)
{
	TCHAR buf[512];
	_stprintf(buf, _T("system log:%s"), str);
	hook_cmd(buf);
}
#endif

// �p�^�[������
static PVOID search_addr(BYTE *base, TCHAR *pattern, DWORD *addr = NULL, DWORD *adj = NULL)
{
	BYTE	dat = 0, ptrn[256], mask[256], *p1, *p2;
	char	c, toggle = 0;
	int		n, cnt = 0, fp;
	PVOID	ptr = 0;

	//�����p�^�[���W�J
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
			mask[cnt] = 1;		// �}�X�N�p�^�[��
		else if (c == 'x')
			mask[cnt] = 2;		// �A�h���X��
		else if (c == 'p')
			mask[cnt] = 3;		// �I�t�Z�b�g��
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

	// �ŏ��ɗL���ȃp�^�[���̈ʒu������(.���΂�)
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
// �A�v������POL�ɃR�}���h��n��
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
			// ���O�C����������ăR�}���h�𔭍s����ׂ������A
			// �܂����O�C������Ɏg�p�ł���Ƃ����T���Ă��Ȃ�
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
		syslog(_T("�����ς�?"));
		return TRUE;	// �������ς�
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

	SLwsprintfA(mes, _T("�v���Z�XID = %d"), idPro);
	syslog(mes);
	SLwsprintfA(mes, _T("base�A�h���X = %08x"), pData->pBase);
	syslog(mes);

	// FFxiMain.Dll���������Ă��Ȃ��̂ŃA�^�b�`�ł��Ȃ�
	if (pData->pBase == NULL)	return FALSE;

#if LOG_HOOK == 1
	{
		ptr = search_addr(pData->pBase, pattern_log);

		SLwsprintfA(mes, _T("���O�A�^�b�`�A�h���X = %08x"), ptr);
		syslog(mes);

		if (ptr)
			pILOG_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookLog, (PBYTE*)&pNextLogHook);

		if (pILOG_HOOK) {
			pData->fRdy |= FF11_Rdy_GetLog;
			syslog(_T("���O�A�^�b�`����!"));
		}
		else {
			syslog(_T("���O�A�^�b�`���s!"));
		}
	}
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
	{
		ptr = search_addr(pData->pBase, pattern_cmd);

		SLwsprintfA(mes, _T("�R�}���h�A�^�b�`�A�h���X = %08x"), ptr);
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
			syslog(_T("�R�}���h�A�^�b�`����!"));
		}
		else {
			syslog(_T("�R�}���h�A�^�b�`���s!"));
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

	// �x�[�X�|�C���^
	pData->tPtr[FF11ADDR_BASE] = pData->pBase;

	// �L�������
	if (search_addr(pData->pBase, pattern_job, &addr))
		pData->tPtr[FF11ADDR_JOBINFO] = (PBYTE)(DWORD64)addr;

	// �p�[�e�B���
	if (search_addr(pData->pBase, pattern_ptinfo, &addr)) {
		addr += ADJUST_PTINFO;
		pData->tPtr[FF11ADDR_PTINFO] = (PBYTE)(DWORD64)addr;
	}

	return TRUE;
}

// -----------------------------------------------------
// �A�v�����Ŏg�p����֐�
// -----------------------------------------------------
EXPORT DWORD WINAPI FF11_GetProcessId(int no)
{
	PROCESSENTRY32	pe;
	HANDLE	hps;
	BOOL	res;
	DWORD	id = 0;
	int		n, cnt = 0;

	if (no < 0 || MAX_PROCESS <= no)		return 0;

	// POL.EXE�v���Z�X����
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);

	hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hps == INVALID_HANDLE_VALUE)	return 0;

	res = Process32First(hps, &pe);

	while (res) {
		if (!lstrcmp(POL_NAME, pe.szExeFile)) {
			// POL����
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

	// FFXiMain.dll��T��
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	hms = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
	if (hms == INVALID_HANDLE_VALUE)	return NULL;

	res = Module32First(hms, &me);

	while (res) {
		if (!lstrcmp(DLL_NAME, me.szModule)) {
			// DLL����
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

	// �܂�FFXiMain.dll���Ǎ��܂�Ă��Ȃ��̂ŃA�^�b�`�ł��Ȃ�
	if (pbase == NULL)			return 0;

	// �C���f�b�N�X�o�^���I�[�o�[
	if (nIndex >= MAX_INDEX)	return -1;

	// �v���Z�XID����
	for (n = 0; n < MAX_PROCESS; n++) {
		if (tProcess[n].idPro == pid) {
			proc = &tProcess[n];
			break;
		}
	}

	if (n >= MAX_PROCESS)	return -1;	// �o�^�Ȃ�

										// �~���[�e�b�N�X�쐬
	if (proc->hMtx == 0) {
		wsprintf(obj_name, MTX_NAME, pid);
		proc->hMtx = CreateMutex(NULL, FALSE, obj_name);
	}
	if (proc->hMtx == 0)	return -1;	// �~���[�e�b�N�X�쐬���s

	WaitForSingleObject(proc->hMtx, INFINITE);

	// �v���Z�X�I�[�v��
	if (proc->hPro == 0)
		proc->hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (proc->hPro == 0)	goto err_end;

	hpro = (DWORD64)proc->hPro;

	// �C�x���g�쐬
#ifdef USE_EXCHANGE
	if (proc->hEvent == 0)
		proc->hEvent = CreateEvent(NULL, TRUE, TRUE, EVENT_NAME);
	if (proc->hEvent == 0)	goto err_end;
#endif // USE_EXCHENGE

	// �󂫃C���f�b�N�X������(index=0�͎g��Ȃ�)
	for (index = 1; index < MAX_INDEX; index++) {
		// �v���Z�X�Ǘ��e�[�u���ւ̃|�C���^��NULL�Ȃ��
		if (tIndex[index].pProc == NULL)	break;
	}

	// ���L�������I�[�v��
	if (proc->hMap == 0) {
		BOOL first;
		wsprintf(obj_name, MAP_NAME, pid);
		proc->hMap = CreateFileMapping((HANDLE)-1,
			NULL, PAGE_READWRITE, 0, sizeof(TDATA), obj_name);
		if (proc->hMap == 0)		goto err_end;

		// ���L�t�@�C�����V�K�쐬�Ȃ珉��������
		first = (GetLastError() != ERROR_ALREADY_EXISTS);

		proc->pData = (PTDATA)MapViewOfFile(proc->hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (proc->pData == NULL)	goto err_end;
		if (first)
			ZeroMemory(proc->pData, sizeof(TDATA));
	}

	pd = proc->pData;
	if (pd == NULL)					goto err_end;
	if (pd->nInject >= MAX_INJECT)	goto err_end;	// ����ȏ�C���W�F�N�g�ł��Ȃ�

	pd->pBase = pbase;

	nIndex++;
	tIndex[index].pProc = proc;

	ReleaseMutex(proc->hMtx);

	if (pd->nInject == 0) {
		// ���̃v���Z�X�ւ̏��o�^�Ȃ̂�dll���C���W�F�N�g����
		if (InjectLibrary((DWORD)hpro, _T("madCHook.dll")))
			pd->fLD_madC = TRUE;
		if (InjectLibrary((DWORD)hpro, _T("ffhook.dll")))
			pd->fLD_hook = TRUE;
	}

	WaitForSingleObject(proc->hMtx, INFINITE);

	// �󂫂�T���Ċm��
	for (n = 0; n < MAX_INJECT; n++) {
		if (!pd->fEna[n]) {
			pd->fEna[n] = TRUE;
#if HOOK_DLL == 1
			pd->pReadLog[n] = pd->pWriteLog;
#endif
#if CMD_HOOK == 1 || DBG_DLL == 1
			// �ŏ���inject�Ȃ�V�X�e�����O����邽�߂�FIFO�N���A�����Ȃ�
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

// pol�������Ă�����FALSE��Ԃ�
BOOL check_dll(HANDLE hpro)
{
	BOOL	res;
	DWORD	code;

	res = GetExitCodeProcess(hpro, &code);
	if (res == 0 || code != STILL_ACTIVE)
		return FALSE;

	return TRUE;
}

// index�l����proc��pd�����߂�B��������������NULL��Ԃ�
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
		return FALSE;		// index�l���ُ�

	if (index <= 0) {
		// index��0�ȉ��Ȃ�S�����C���W�F�N�g����
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

		// �v���Z�X�Ō�̃C�W�F�N�g�Ȃ̂ł��낢��J������
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
				buf[sbuf - 1] = '\0';		// �O�̂��ߏI�[�R�[�h
				res = (int)strlen(buf);
			}
			else {
				if (pd->tLog[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tLog[p].Buffer, -1, pd->tLog[p].BufferW,
						sbuf - 1);
				wcsncpy_s(wbuf, sbuf, pd->tLog[p].BufferW, sbuf - 1);
				wbuf[sbuf - 1] = '\0';	// �O�̂��ߏI�[�R�[�h
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
				buf[sbuf - 1] = '\0';		// �O�̂��ߏI�[�R�[�h
				res = (int)_tcslen(buf);
			}
			else {
				if (pd->tCmd[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, (LPCCH)pd->tCmd[p].Buffer, -1, pd->tCmd[p].BufferW,
						sbuf - 1);
				wcsncpy_s(wbuf, sbuf, pd->tCmd[p].BufferW, sbuf - 1);
				wbuf[sbuf - 1] = '\0';	// �O�̂��ߏI�[�R�[�h
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
	// ����dll��pol.exe�v���Z�X���Ȃ̂œ�������A�^�b�`�������s��
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
