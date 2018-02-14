#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <process.h>
#include "resource.h"

// �f�o�b�O���O�̓R�}���h�o�b�t�@�Ƌ��ʁB

// �R���t�B�O
#define LOG_HOOK		1				// ���O���t�b�N����(0-OFF/1-ON)
#define LOG_DECODE		1				// ���O�̕s�v�Ȑ���R�[�h�r��(0-OFF/1-ON)
#define LOG_CUT			1				// ���O�J�b�g�@�\(0-OFF/1-ON)

#define CMD_HOOK		1				// �R�}���h���t�b�N����(0-OFF/1-ON)
#define CMD_CUT			1				// �R�}���h�J�b�g�@�\(0-OFF/1-ON)
#define	CMD_PUT			1				// �O������R�}���h���s(0-OFF/1-ON)

#define KILLEFFECT		1				// killeffect���g�p����(0-OFF/1-ON)
#define MERGEITEM		1				// �A�C�e�����ڂ��g�p����(0-OFF/1-ON)

#define DBG_DLL			0				// �f�o�b�O(0-OFF/1-ON)

#define MAX_PROCESS		6				// �t�b�N�ł���POL�̐�
#define MAX_INJECT		12				// 1��POL�ɃC���W�F�N�g�ł��鐔

#define MAX_INDEX		(MAX_PROCESS*MAX_INJECT+1)

#define SIZE_LOG_FIFO	1000			// ���OFIFO��
#define LEN_LOG_BUFFER	1000			// ���O������
#define MAX_LOGCUT		300				// 1��POL�v���Z�X�ɓo�^�ł��郍�O�J�b�g���

#define SIZE_CMD_FIFO	100				// �R�}���hFIFO��
#define LEN_CMD_BUFFER	100				// �R�}���h������

#define LEN_AM_BUFFER	32				// �A�r���@���̍ő咷
#define MAX_AMCUT		600				// 1��POL�v���Z�X�ɓo�^�ł���A�r���@�J�b�g���

#define SEARCH_RANGE	0x300000		// �����I�t�Z��base����̌����͈�

// �萔
#define CMD_CUT_FMACRO	0x01			// �}�N���A�R�}���h���C������̃R�}���h
#define CMD_CUT_FMENU	0x02			// ���j���[����̃R�}���h

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
#define MAP_NAME		_T("FFHOOK_MAP_%d")		// �}�b�v�t�@�C����(pid)
#define EVENT_NAME		_T("FFHOOK_EVENT_%d")	// �C�x���g��(pid)
#define MTX_NAME		_T("FFHOOK_MTX_%d")		// Mutex��(pid)

// �f�o�b�O�p
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

// ���OFIFO
typedef struct {
	BYTE	tAttr;						// �A�g���r���[�g�i�[�p
	BYTE	fCut;						// �J�b�g���ꂽ���O��TRUE
	char	Buffer[LEN_LOG_BUFFER];		// ���O�o�b�t�@
	wchar_t	BufferW[LEN_LOG_BUFFER];	// ���O�o�b�t�@
} TLOG;

// ���O�J�b�g���
typedef struct _tcut {
	BYTE	tAttr[256];					// �A�g���r���[�g�e�[�u��
	int		nLen;						// ���O�J�b�g������
	char	Buffer[LEN_LOG_BUFFER];		// ���O�J�b�g������
} TCUT, *PTCUT;

// �R�}���hFIFO
typedef struct {
	BYTE	fCut;						// �J�b�g���ꂽ�R�}���h��TRUE
	char	Buffer[LEN_CMD_BUFFER];
	wchar_t	BufferW[LEN_CMD_BUFFER];
} TCMD;

// �A�r���@�J�b�g���
typedef struct _tcutcmd {
	int		nLen;						// �A�r���@������
	char	Buffer[LEN_AM_BUFFER];		// �A�r���@������
	int		fPass;						// �ȗ���1
} TCUTAM, *PTCUTAM;

// ���L���������e�[�u��
// FIFO�o�b�t�@�̓v���Z�X�ɂP�Ƃ��A�Ǎ��݃|�C���^�����Ɨ�������B
// �ǂݒx���ƌÂ����O����������B
// �e�A�v���ԂŐ�������ۂ��߂Ɋe�@�\���g�p���ł��e�[�u���͌Œ�ɂ���B
typedef struct {
	BOOL	fLD_madC;				// madChook.dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)
	BOOL	fLD_hook;				// ����dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)

	DWORD	fRdy;					// �e�폀�������t���O
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
	int		pReadCmd[MAX_INJECT];	// �R�}���hFIFO �Ǎ��݃|�C���^(�e�[�u�����ɓƗ�)
	TCMD	tCmd[SIZE_CMD_FIFO];	// �R�}���h�o�b�t�@

	HANDLE			hThread;		// �X���b�h�n���h���p
	unsigned int	idThread;		// �X���b�hID
	char	nPrm[LEN_LOG_BUFFER];	// �R�}���h�ˑ��p�����[�^�p

	HWND	hWnd;					// FF���̃n���h��
	PBYTE	tPtr[50];				// �e�A�h���X�i�[�p(FF11_GetAddress�Ŏg�p)

	BYTE	fCutAM;					// �A�r���@�J�b�g��ON/OFF
	int		nCutAM;					// �A�r���@�J�b�g���o�^��
	TCUTAM	tCutAM[MAX_AMCUT];		// �A�r���@�J�b�g���
} TDATA, *PTDATA;

// �v���Z�X�ʊǗ��e�[�u��
typedef struct {
	int		idPro;					// �v���Z�XID
	HANDLE	hPro;					// �v���Z�X�n���h��
	HANDLE	hMap;					// ���L�������}�b�v�n���h��
	HANDLE	hMtx;					// Mutex�n���h��
	HANDLE	hEvent;					// �C�x���g�p
	PTDATA	pData;					// ���L���������e�[�u���ւ̃|�C���^
	HWND	hWnd;					// FF11���n���h��
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

typedef struct _tprocess {
	TCHAR	sName[256];
	int		nPro;
} TPLIST;

// �A�v�����Ŏg�p����
static HINSTANCE	hInst;
static TPROC	tProcess[MAX_PROCESS];	// �v���Z�X�Ǘ��e�[�u��
static TINDEX	tIndex[MAX_INDEX];		// �C���f�b�N�X�Ǘ��e�[�u��
static int		nIndex = 0;				// �g�p���C���f�b�N�X��
static TPLIST	tPList[MAX_PROCESS];

// POL�ɃC���W�F�N�g���ꂽDLL���Ŏg�p����
static BOOL		fPol = FALSE;		// POL���̎�TRUE�ɂȂ�
static int		idPro = 0;			// �v���Z�XID
static HANDLE	hMap = 0;			// ���L�������̃n���h��
static HANDLE	hMtx = 0;			// �r������
static HANDLE	hEvent = 0;			// �C�x���g�p
static PTDATA	pData = 0;			// ���L���������e�[�u���ւ̃|�C���^

#include "pattern.h"

LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

// -----------------------------------------------------
// POL���Ŏg�p����֐�
// -----------------------------------------------------
#if LOG_HOOK == 1 || DBG_DLL == 1

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
		for (n = 0; n < LEN_LOG_BUFFER-1 && *str; n++) {
			if (*str == 0x1f || *str == 0x7f) {
				// ���䕶���Ȃ�2�������X�L�b�v
				if (*(++str) != 0) {
					str++;
					n++;
					continue;
				}
			} else if (*(unsigned char*)str == 0xef) {
				// �^�u�ϊ�������
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
				if (p->tAttr[*attr] == 0)	continue;	// �A�g���r���[�g�s��v

				if (p->Buffer[0] == 0) {
					// �{���Ȃ�
					pData->tLog[wp].fCut = 1;
					res = FALSE;
					break;
				} else if (p->Buffer[0] == '^') {
					// �O����v
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

// �R�}���h�������̓f�o�b�O���O��FIFO�ɏ�����
// str			���O�{��
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
	// �R�}���h�J�b�g

	buf = 0;
	typ = 0;

	if (pData->fCutAM && pData->nCutAM && *ptr == '/' && !strstr(ptr, "<st")) {
		ptr++;

		// ���j���[���s���ꂽ����
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

		// �}�N���������̓R�}���h���C����蔭�s���ꂽ����
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
							strncmp(c, "��", 2) && strncmp(c, "��", 2) && strncmp(c, "�Q", 2) && strncmp(c, "�l", 2))
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

	// ���C�g�|�C���^�����[�h�|�C���^�ɒǂ����Ă����烊�[�h�|�C���^���C���N�������g
	for (n = 0; n < MAX_INJECT && inject > 0; n++) {
		// �L���ȃe�[�u�������`�F�b�N����
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
// �R�}���h�t�b�N�֐�
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

		// �A�^�b�`����
		if (*pCode == 0xe8) {
			// call�����ɃA�^�b�`
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
void syslog(char *str)
{
	char buf[512];
	BYTE attr = 255;

	wsprintfA(buf, "[system log]%s", str);
	hook_log(&attr, buf);
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
			mask[cnt] = 1;		//�u.�v �}�X�N�p�^�[��
		else if (c == 'x')
			mask[cnt] = 2;		//�ux�v �A�h���X��
		else if (c == 'p')
			mask[cnt] = 3;		//�up�v �I�t�Z�b�g��
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
						//�ux�v �A�h���X��
						*addr = *(DWORD *)(p1-fp+n);
						break;
					}
				}
			}
			if (adj) {
				for (n = 0; n < cnt; n++) {
					if (mask[n] == 3) {
						//�up�v �I�t�Z�b�g��
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
// �R�}���h�C���^�[�t�F�[�X �A�v������POL�ɃR�}���h
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
		syslog("���łɏ����ς�?");
		return TRUE;	// �������ς�
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

	SLwsprintfA(mes, "�v���Z�XID = %d", idPro);
	syslog(mes);
	SLwsprintfA(mes, "base�A�h���X = %08x", pData->pBase);
	syslog(mes);

	// FFxiMain.Dll���������Ă��Ȃ��̂ŃA�^�b�`�ł��Ȃ�
	if (pData->pBase == NULL)	return FALSE;

#if LOG_HOOK == 1
	{
		ptr = search_addr(pData->pBase, pattern_log);

		SLwsprintfA(mes, "���O�A�^�b�`�A�h���X = %08x", ptr);
		syslog(mes);

		if (ptr)
			pILOG_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookLog, (PBYTE*)&pNextLogHook);

		if (pILOG_HOOK) {
			pData->fRdy |= FF11_Rdy_GetLog;
			syslog("���O�A�^�b�`����!");
		} else {
			syslog("���O�A�^�b�`���s!");
		}
	}
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
	{
		ptr = search_addr(pData->pBase, pattern_cmd);

		SLwsprintfA(mes, "�R�}���h�A�^�b�`�A�h���X = %08x", ptr);
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
			syslog("�R�}���h�A�^�b�`����!");
		} else {
			syslog("�R�}���h�A�^�b�`���s!");
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
			SLwsprintfA(mes, "KILLEFFECT�A�^�b�`�A�h���X = %08x", ptr);
			syslog(mes);
			if (pKillEffect = (void (*)(int))ptr)
				pData->fRdy |= FF11_Rdy_KillEffect;
		}
#endif
#if MERGEITEM == 1
		{
			ptr = search_addr(pData->pBase, pattern_mergeitem);
			SLwsprintfA(mes, "MERGEITEM�A�^�b�`�A�h���X = %08x", ptr);
			syslog(mes);
			if (pMergeItem = (void (*)(int))ptr)
				pData->fRdy |= FF11_Rdy_MergeItem;
		}
#endif
#if MACROEXE == 1
		{
			ptr = search_addr(pData->pBase, pattern_macroexe);
			SLwsprintfA(mes, "MACRO�A�^�b�`�A�h���X = %08x", ptr);
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
// �A�v�����Ŏg�p����֐�
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

// POL�̃v���Z�XID��Ԃ�
// no		no�Ԗڂ�POL��ID��Ԃ�
// �߂�l	POLID
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

	// POL.EXE�v���Z�X����
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);

	hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hps == INVALID_HANDLE_VALUE)	return 0;

	res = Process32First(hps, &pe);

	// �N������POL�����ׂēo�^����
	while (res) {
		if (!lstrcmp(POL_NAME, pe.szExeFile)) {
			// POL����
			id = pe.th32ProcessID;

			// ���łɓo�^�ς݂��`�F�b�N����
			for (n = 0; n < MAX_PROCESS; n++) {
				if (tProcess[n].idPro == id)
					break;
			}

			// ���o�^�Ȃ̂œo�^����
			if (n == MAX_PROCESS) {
				for (; cnt < MAX_PROCESS; cnt++) {
					if (tProcess[cnt].idPro == 0) {
						tProcess[cnt++].idPro = id;
						break;
					}
				}
				// �o�^�\���I�[�o�[
				if (cnt >= MAX_PROCESS)
					break;
			}
		}
		res = Process32Next(hps, &pe);
	}

	CloseHandle(hps);

	ZeroMemory(tPList, sizeof(tPList));

	if (no == -1) {
		// �v���Z�X���ЂƂ̂�
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

// BASE�A�h���X��Ԃ�
// pid		POL�̃v���Z�XID
// �߂�l	�x�[�X�A�h���X
EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD pid)
{
    MODULEENTRY32	me;
	HANDLE	hms;
	BOOL	res;
	PBYTE	ptr = NULL;

	if (pid == 0)	return NULL;

	// FFXiMain.dll��T��
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	hms = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
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

// BASE�A�h���X��Ԃ�
// pid		POL�̃v���Z�XID
// �߂�l	index�ԍ�
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

		proc->pData = (PTDATA)MapViewOfFile(proc->hMap, FILE_MAP_ALL_ACCESS,0,0,0);
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
		if (InjectLibrary((DWORD)hpro, _T("ffhookex.dll")))
			pd->fLD_hook = TRUE;
	}

	WaitForSingleObject(proc->hMtx, INFINITE);

	// �󂫂�T���Ċm��
	for (n = 0; n < MAX_INJECT; n++) {
		if (!pd->fEna[n]) {
			pd->nInject++;
			pd->fEna[n] = TRUE;
			tIndex[index].pInject = n;

#if DBG_DLL == 0
			pd->pReadLog[n] = pd->pWriteLog;
#else
			// �ŏ���inject�Ȃ�V�X�e�����O����邽�߂�FIFO�N���A�����Ȃ�
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

// index�l����proc��pd�����߂�B�ُ킪�����FALSE��Ԃ�
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

		// �v���Z�X�Ō�̃C�W�F�N�g�Ȃ炢�낢��J������
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
				buf[sbuf-1] = '\0';		// �O�̂��ߏI�[�R�[�h
				res = (int)strlen(buf);
			} else {
				if (pd->tLog[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tLog[p].Buffer, -1, pd->tLog[p].BufferW,
									sbuf-1); 
				wcsncpy_s(wbuf, sbuf, pd->tLog[p].BufferW, sbuf-1);
				wbuf[sbuf-1] = '\0';	// �O�̂��ߏI�[�R�[�h
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
				buf[sbuf - 1] = '\0';		// �O�̂��ߏI�[�R�[�h
				res = (int)strlen(buf);
			} else {
				if (pd->tCmd[p].BufferW[0] == 0)
					MultiByteToWideChar(CP_ACP, 0, pd->tCmd[p].Buffer, -1, pd->tCmd[p].BufferW,
									sbuf-1); 
				wcsncpy_s(wbuf, sbuf, pd->tCmd[p].BufferW, sbuf-1);
				wbuf[sbuf - 1] = '\0';	// �O�̂��ߏI�[�R�[�h
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

// ������
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
