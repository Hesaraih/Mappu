#include "stdafx.h"

#include "ffhook_replo.h"
#include "madChook.h"
#include <stdlib.h>//�ǉ�

#pragma comment(lib, "madCHook")

// �R���t�B�O
#define REPLO_HOOK      1               // ���O���t�b�N����(0-OFF/1-ON)
#define REPLO_TRIM      1               // ���O�̕s�v�Ȑ���R�[�h�r��(0-OFF/1-ON)

#define DBG_DLL         0               // �f�o�b�O(0-OFF/1-ON)

#define MAX_PROCESS     6               // �t�b�N�ł���POL�̐�
#define MAX_INJECT      12              // 1��POL�ɃC���W�F�N�g�ł��鐔

#define MAX_INDEX       (MAX_PROCESS*MAX_INJECT+1)

#define SIZE_LOG_FIFO   1000            // ���OFIFO��
#define LEN_LOG_BUFFER  1000            // ���O������

#define SEARCH_RANGE    0x300000        // �����I�t�Z��base����̌����͈�

// �萔
#define HOOK_NAME       _T("ffhook_replo.dll")
#define MADC_NAME       _T("madCHook.dll")
#define POL_NAME        _T("pol.exe")
#define DLL_NAME        _T("FFXiMain.dll")
#define MAP_NAME        _T("FFHOOK_REPLO_MAP_%d")       // �}�b�v�t�@�C����(pid)
#define MTX_NAME        _T("FFHOOK_REPLO_MTX_%d")       // Mutex��(pid)

// �f�o�b�O�p
#if DBG_DLL == 1
#define SLwsprintfA     wsprintfA
#else
#define SLwsprintfA
#define syslog
#endif

#if REPLO_HOOK == 1
#define USE_HOOK
#endif

// ���OFIFO
typedef struct {
    char    Buffer[LEN_LOG_BUFFER];     // ���O�o�b�t�@
    time_t  Timer;                      // ���O�o�͎���
} TREPLO;

// ���L���������e�[�u��
// FIFO�o�b�t�@�̓v���Z�X�ɂP�Ƃ��A�Ǎ��݃|�C���^�����Ɨ�������B
// �ǂݒx���ƌÂ����O����������B
// �e�A�v���ԂŐ�������ۂ��߂Ɋe�@�\���g�p���ł��e�[�u���͌Œ�ɂ���B
typedef struct {
    BOOL    fLD_madC;               // madChook.dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)
    BOOL    fLD_hook;               // ����dll��pol.exe�ɃC���W�F�N�g�������H(TRUE)

    DWORD   fRdy;                   // �e�폀�������t���O
    PBYTE   pBase;                  // FFXiMain.dll�x�[�X�A�h���X
    int     nInject;                // ���̃v���Z�X�Ŏg�p����index��
    BYTE    fEna[MAX_INJECT];       // �g�p���̃e�[�u����1�ɂȂ�

    int     pWriteReplo;            // ���OFIFO �����݃|�C���^
    int     pReadReplo[MAX_INJECT]; // ���OFIFO �Ǎ��݃|�C���^(�e�[�u�����ɓƗ�)
    TREPLO  tReplo[SIZE_LOG_FIFO];  // ���O�o�b�t�@

    HANDLE          hThread;        // �X���b�h�n���h���p
    unsigned int    idThread;       // �X���b�hID

    PBYTE   tPtr[50];               // �e�A�h���X�i�[�p(FF11_GetAddress�Ŏg�p)
} TDATA, *PTDATA;

// �v���Z�X�ʊǗ��e�[�u��
typedef struct {
    int     idPro;                  // �v���Z�XID
    HANDLE  hPro;                   // �v���Z�X�n���h��
    HANDLE  hMap;                   // ���L�������}�b�v�n���h��
    HANDLE  hMtx;                   // Mutex�n���h��
    PTDATA  pData;                  // ���L���������e�[�u���ւ̃|�C���^
} TPROC, *PTPROC;

// �C���f�b�N�X�Ǘ��e�[�u��(index=0�͎g��Ȃ����ɂ���)
typedef struct {
    PTPROC  pProc;                  // �^�[�Q�b�g�v���Z�X
    int     pInject;                // �C���W�F�N�g�ԍ�
} TINDEX;

// �A�^�b�`���
typedef struct _ihook_ {
    BOOL    fAttach;                // �A�^�b�`������TRUE
    PBYTE   pCode;                  // �A�^�b�`���閽�߂�����A�h���X
    PBYTE   pHook;                  // �t�b�N�A�h���X
    PBYTE   pNext;                  // ��ѐ�
    BYTE    func[12];
} IHOOK, *PIHOOK;

// �A�v�����Ŏg�p����
static HINSTANCE    hInst;
static TPROC    tProcess[MAX_PROCESS];  // �v���Z�X�Ǘ��e�[�u��
static TINDEX   tIndex[MAX_INDEX];      // �C���f�b�N�X�Ǘ��e�[�u��
static int      nIndex = 0;             // �g�p���C���f�b�N�X��

// POL�ɃC���W�F�N�g���ꂽDLL���Ŏg�p����
static BOOL     fPol = FALSE;       // POL���̎�TRUE�ɂȂ�
static int      idPro = 0;          // �v���Z�XID
static HANDLE   hMap = 0;           // ���L�������̃n���h��
static HANDLE   hMtx = 0;           // �r������
static PTDATA   pData = 0;          // ���L���������e�[�u���ւ̃|�C���^

#include "pattern.h"

// -----------------------------------------------------
// POL���Ŏg�p����֐�
// -----------------------------------------------------

#if REPLO_HOOK == 1

// ���O��FIFO�ɏ�����
// str          ���O�{��
void hook_replo(char *str)
{
    int n, wp, rp, inject;

    if (hMtx == 0 || str == NULL)   return;

    WaitForSingleObject(hMtx, INFINITE);

    wp = pData->pWriteReplo;

    strncpy_s(pData->tReplo[wp].Buffer, LEN_LOG_BUFFER-1, str, LEN_LOG_BUFFER-2);
    pData->tReplo[wp].Timer = time(NULL);
    
    if (++wp >= SIZE_LOG_FIFO)
        wp = 0;

    pData->pWriteReplo = wp;

    rp = (wp == SIZE_LOG_FIFO-1) ? 0 : wp+1;
    inject = pData->nInject;

    // ���C�g�|�C���^�����[�h�|�C���^�ɒǂ����Ă����烊�[�h�|�C���^���C���N�������g
    for (n = 0; n < MAX_INJECT && inject > 0; n++) {
        if (pData->fEna[n]) {
            // �L���ȃe�[�u�������`�F�b�N����
            if (pData->pReadReplo[n] == wp)
                pData->pReadReplo[n] = rp;
            inject--;
        }
    }

    ReleaseMutex(hMtx);

    return;
}

// �t�b�N�֐�
__declspec(naked) void __stdcall fnHookReplo(char *str, BYTE *dummy)
{
    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        mov     eax, dword ptr [esp+14h]
        push    eax
        call    hook_replo
        add     esp,4h
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        jmp     pNextReploHook
    }
}
#endif

#ifdef USE_HOOK

// �A�^�b�`����
PIHOOK attach(PBYTE pCode, PBYTE pCallBackFunc, PBYTE *pNextHook)
{
    PIHOOK  pi;
    PBYTE   src, dis, ptr;

    if (pCode == NULL || pNextHook == NULL)
        return NULL;    // �����ُ�

    if ((pi = new IHOOK) == NULL)
        return NULL;    // �������m�ێ��s

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
    PBYTE   src, dis;

    if (pihook == NULL)
        return;     // �����ُ�

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
    BYTE    dat = 0, ptrn[256], mask[256], *p1, *p2;
    char    c, toggle = 0;
    int     n, cnt = 0, fp;
    PVOID   ptr = 0;

    //�����p�^�[���W�J
    ZeroMemory(ptrn, sizeof(ptrn));
    ZeroMemory(mask, sizeof(mask));
    if (addr)       *addr = 0;
    if (adj)        *adj = 0;

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
            mask[cnt] = 1;      // �}�X�N�p�^�[��
        else if (c == 'x')
            mask[cnt] = 2;      // �A�h���X��
        else if (c == 'p')
            mask[cnt] = 3;      // �I�t�Z�b�g��
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

    for (p1 = (BYTE*)base; p1 < (BYTE*)base+SEARCH_RANGE; p1++) {
        if (*p1 != ptrn[fp]) continue;
        p2 = p1+1;
        for (n = fp+1; n < cnt; p2++, n++) {
            if (mask[n]) continue;
            if (*p2 != ptrn[n]) break;
        }
        if (n == cnt) {
            ptr = (PVOID)(p1 - fp);
            if (addr) {
                for (n = 0; n < cnt; n++) {
                    if (mask[n] == 2) {
                        *addr = *(DWORD *)(p1-fp+n);
                        break;
                    }
                }
            }
            if (adj) {
                for (n = 0; n < cnt; n++) {
                    if (mask[n] == 3) {
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

BOOL init(void)
{
    TCHAR   obj_name[256];
    char    mes[256];
    BOOL    res = TRUE;
    PVOID   ptr = NULL;

    if (pData)  {
        syslog("���łɏ����ς�?");
        return TRUE;    // �������ς�
    }

    idPro = GetCurrentProcessId();

    wsprintf(obj_name, MAP_NAME, idPro);
    hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, obj_name);
    if (hMap == 0)      return FALSE;

    wsprintf(obj_name, MTX_NAME, idPro);
    hMtx = OpenMutex(MUTEX_ALL_ACCESS, NULL, obj_name);
    if (hMtx == 0)      return FALSE;

#ifdef USE_EXCHANGE
    hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
    if (hEvent == 0)    return FALSE;
#endif // USE_EXCHENGE

    pData = (PTDATA)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (pData == NULL)  return FALSE;

    SLwsprintfA(mes, "�v���Z�XID = %d", idPro);
    syslog(mes);
    SLwsprintfA(mes, "base�A�h���X = %08x", pData->pBase);
    syslog(mes);

    // FFxiMain.Dll���������Ă��Ȃ��̂ŃA�^�b�`�ł��Ȃ�
    if (pData->pBase == NULL)   return FALSE;

#if REPLO_HOOK == 1
    {
        ptr = search_addr(pData->pBase, pattern_replo);

        SLwsprintfA(mes, "���O�A�^�b�`�A�h���X = %08x", ptr);
        syslog(mes);

        if (ptr)
            pIREPLO_HOOK = attach((PBYTE)ptr, (PBYTE)fnHookReplo, (PBYTE*)&pNextReploHook);

        if (pIREPLO_HOOK) {
            pData->fRdy |= FF11_Rdy_GetReplo;
            syslog("���O�A�^�b�`����!");
        } else {
            syslog("���O�A�^�b�`���s!");
        }
    }
#endif
    
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
        pData->tPtr[FF11ADDR_OWNPOS] = (PBYTE)(DWORD64)addr+4;

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

// POL�̃v���Z�XID��Ԃ�
// no       no�Ԗڂ�POL��ID��Ԃ�
// �߂�l   POLID
EXPORT DWORD WINAPI FF11_GetProcessId(int no)
{
    PROCESSENTRY32  pe;
    HANDLE  hps;
    BOOL    res;
    DWORD   id = 0;
    int     n, cnt = 0;

    no--;
    if (no < -2 || MAX_PROCESS <= no)       return 0;

    // POL.EXE�v���Z�X����
    ZeroMemory(&pe, sizeof(pe));
    pe.dwSize = sizeof(pe);

    hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hps == INVALID_HANDLE_VALUE)    return 0;

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

    no++;

    return tProcess[no].idPro;
}

// BASE�A�h���X��Ԃ�
// pid      POL�̃v���Z�XID
// �߂�l   �x�[�X�A�h���X
EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD pid)
{
    MODULEENTRY32   me;
    HANDLE  hms;
    BOOL    res;
    PBYTE   ptr = NULL;

    if (pid == 0)   return NULL;

    // FFXiMain.dll��T��
    ZeroMemory(&me, sizeof(me));
    me.dwSize = sizeof(me);

    hms = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hms == INVALID_HANDLE_VALUE)    return NULL;

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
// pid      POL�̃v���Z�XID
// �߂�l   index�ԍ�
EXPORT int WINAPI FF11_Inject(DWORD pid)
{
    DWORD64 hpro;
    PTDATA  pd;
    PTPROC  proc = NULL;
    int     index, n, res = -1;
    TCHAR   obj_name[256];
    PBYTE   pbase;

    FF11_GetProcessId(-1);

    pbase = FF11_GetBaseAddress(pid);

    // �܂�FFXiMain.dll���Ǎ��܂�Ă��Ȃ��̂ŃA�^�b�`�ł��Ȃ�
    if (pbase == NULL)          return 0;

    // �C���f�b�N�X�o�^���I�[�o�[
    if (nIndex >= MAX_INDEX)    return -1;

    // �v���Z�XID����
    for (n = 0; n < MAX_PROCESS; n++) {
        if (tProcess[n].idPro == pid) {
            proc = &tProcess[n];
            break;
        }
    }

    if (n >= MAX_PROCESS)   return -1;  // �o�^�Ȃ�

    // �~���[�e�b�N�X�쐬
    if (proc->hMtx == 0) {
        wsprintf(obj_name, MTX_NAME, pid);
        proc->hMtx = CreateMutex(NULL, FALSE, obj_name);
    }

    if (proc->hMtx == 0)    return -1;  // �~���[�e�b�N�X�쐬���s

    WaitForSingleObject(proc->hMtx, INFINITE);

    // �v���Z�X�I�[�v��
    if (proc->hPro == 0)
        proc->hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (proc->hPro == 0)    goto err_end;

    hpro = (DWORD64)proc->hPro;

    // �C�x���g�쐬
#ifdef USE_EXCHANGE
    if (proc->hEvent == 0)
        proc->hEvent = CreateEvent(NULL, TRUE, TRUE, EVENT_NAME);
    if (proc->hEvent == 0)  goto err_end;
#endif // USE_EXCHENGE

    // �󂫃C���f�b�N�X������(index=0�͎g��Ȃ�)
    for (index = 1; index < MAX_INDEX; index++) {
        // �v���Z�X�Ǘ��e�[�u���ւ̃|�C���^��NULL�Ȃ��
        if (tIndex[index].pProc == NULL)    break;
    }

    // ���L�������I�[�v��
    if (proc->hMap == 0) {
        BOOL first;
        wsprintf(obj_name, MAP_NAME, pid);
        proc->hMap = CreateFileMapping((HANDLE)-1,
                NULL, PAGE_READWRITE, 0, sizeof(TDATA), obj_name);
        if (proc->hMap == 0)        goto err_end;

        // ���L�t�@�C�����V�K�쐬�Ȃ珉��������
        first = (GetLastError() != ERROR_ALREADY_EXISTS);

        proc->pData = (PTDATA)MapViewOfFile(proc->hMap, FILE_MAP_ALL_ACCESS,0,0,0);
        if (proc->pData == NULL)    goto err_end;
        if (first)
            ZeroMemory(proc->pData, sizeof(TDATA));
    }

    pd = proc->pData;
    if (pd == NULL)                 goto err_end;
    if (pd->nInject >= MAX_INJECT)  goto err_end;   // ����ȏ�C���W�F�N�g�ł��Ȃ�

    pd->pBase = pbase;

    nIndex++;
    tIndex[index].pProc = proc;

    ReleaseMutex(proc->hMtx);

    if (pd->nInject == 0) {
        // ���̃v���Z�X�ւ̏��o�^�Ȃ̂�dll���C���W�F�N�g����
        if (InjectLibrary((DWORD)hpro, MADC_NAME))
            pd->fLD_madC = TRUE;
        if (InjectLibrary((DWORD)hpro, HOOK_NAME))
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
            pd->pReadReplo[n] = pd->pWriteReplo;
#else
            // �ŏ���inject�Ȃ�V�X�e�����O����邽�߂�FIFO�N���A�����Ȃ�
            if (pd->nInject != 0)
                pd->pReadReplo[n] = pd->pWriteReplo;
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
    BOOL    res;
    DWORD   code;

    res = GetExitCodeProcess(hpro, &code);
    if (res == 0 || code != STILL_ACTIVE)
        return FALSE;

    return TRUE;
}

BOOL check_login(HANDLE hpro, PTDATA pd)
{
    DWORD n;

    if (pd->tPtr[FF11ADDR_LOGIN]) {
        if (ReadProcessMemory(hpro, pd->tPtr[FF11ADDR_LOGIN],(PBYTE)&n,4,NULL)) {
            if (n != 0x00000000)
                return FALSE;
        }
    }

    return TRUE;
}

// index�l����proc��pd�����߂�B�ُ킪�����FALSE��Ԃ�
BOOL check_index(int index, PTPROC *proc, PTDATA *pd)
{
    if (MAX_INDEX <= (DWORD)index)              return FALSE;
    if ((*proc = tIndex[index].pProc) == NULL)  return FALSE;
    if ((*pd = (*proc)->pData) == NULL)         return FALSE;

    if (!check_dll((*proc)->hPro)) {
        FF11_Eject(index);
        return FALSE;
    }

    return TRUE;
}

EXPORT int WINAPI FF11_Eject(int index)
{
    DWORD64 hpro;
    PTDATA  pd;
    PTPROC  proc;
    int     n, nadd = MAX_INDEX;

    if (index >= MAX_INDEX)
        return FALSE;       // index�l���ُ�

    if (index <= 0) {
        // index��0�ȉ��Ȃ�S�����C���W�F�N�g����
        nadd = 1;
        index = 1;
    }

    for (n = index; n < MAX_INDEX && 0 < nIndex; n += nadd) {
        proc = tIndex[n].pProc;
        if (proc == NULL)   continue;
        pd = proc->pData;
        if (pd == NULL)     continue;

        pd->fEna[tIndex[n].pInject] = FALSE;
        tIndex[n].pProc = NULL;
        tIndex[n].pInject = 0;
        nIndex--;
        
        // �v���Z�X�Ō�̃C�W�F�N�g�Ȃ炢�낢��J������
        if (--pd->nInject <= 0) {
            if (proc->idPro) {
                if (hpro = (DWORD64)proc->hPro) {
                    if (pd->fLD_hook)
                        UninjectLibrary((DWORD)hpro, HOOK_NAME);
                    if (pd->fLD_madC)
                        UninjectLibrary((DWORD)hpro, MADC_NAME);
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
    PTPROC  proc;
    PTDATA  pd;

    if (check_index(index, &proc, &pd)) {
        if ((DWORD)object < FF11ADDR_ENDPOINT)
            return pd->tPtr[object];
    }

    return NULL;
}

EXPORT PBYTE WINAPI FF11_GetAddressA(int index, int object, char *name)
{
    BYTE    *ptr;

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
    BYTE    *ptr;

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
    PTPROC  proc;
    PTDATA  pd;
    PBYTE   ptr = NULL;

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
    PTPROC  proc;
    PTDATA  pd;

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

#if REPLO_HOOK == 1
int get_replo(int index, char *buf, int sbuf, time_t *time)
{
    PTPROC  proc;
    PTDATA  pd;
    int     p, no, res = 0;

    if (sbuf < 2 || buf == NULL) return -1;
    
    if (buf) *buf = 0;

    if (!check_index(index, &proc, &pd)) return -1;

    if (!(pd->fRdy & FF11_Rdy_GetReplo)) return 0;

    WaitForSingleObject(proc->hMtx, INFINITE);

    no = tIndex[index].pInject;
    if (pd->fEna[no]) {
        p = pd->pReadReplo[no];
        if (p != pd->pWriteReplo) {
            if (buf) {
                strncpy_s(buf, sbuf, pd->tReplo[p].Buffer, _TRUNCATE);
                res = (int)strlen(buf);
            }
            if (time)
                *time = pd->tReplo[p].Timer;

            if (++p >= SIZE_LOG_FIFO)
                p = 0;
            pd->pReadReplo[no] = p;
        }
    }

    ReleaseMutex(proc->hMtx);

    return res;
}
#endif

#if REPLO_HOOK == 1 && REPLO_TRIM == 1
BOOL strtrim(char *buf, char *sea, int range)
{
    char *hit;
    size_t sealen;
    errno_t err;

    sealen = strlen(sea);

    if (sealen == 0) return FALSE;
    if ((hit = strstr(buf, sea)) == NULL) return FALSE;

    err = memmove_s(
        hit,
        strnlen_s(hit, strlen(buf)),
        hit + sealen + range,
        strnlen_s(hit, strlen(buf)));
    if (err) return FALSE;

    return TRUE;
}

BOOL strrep(char *buf, char *sea, char *rep)
{
    char *hit;
    size_t sealen, replen;
    errno_t err;

    sealen = strlen(sea);
    replen = strlen(rep);

    if (sealen == 0) return FALSE;
    if ((hit = strstr(buf, sea)) == NULL) return FALSE;

    err = memmove_s(
        hit + replen,
        strnlen_s(hit, strlen(buf)) + replen,
        hit + sealen,
        strlen(buf) - (hit + sealen - buf) + 1);
    if (err) return FALSE;

    err = memcpy_s(hit, strlen(hit) + replen, rep, replen);
    if (err) return FALSE;

    return TRUE;
}
#endif

EXPORT int WINAPI FF11_GetReploA(int index, char *attr, char *data, int length, time_t *time)
{
#if REPLO_HOOK == 1
    int res = 0;
    char buf[LEN_LOG_BUFFER];
    long ulId;//2014.12�ǉ�
    static long stulId = 0;//2014.12�ǉ�

    res = get_replo(index, buf, LEN_LOG_BUFFER, time);
    res -= 62;
    if (res > 0) {
        ulId = strtol(&buf[27],NULL,16);//�ǉ�
        if(ulId <= stulId && ulId != 0) {
            return 0;//�ǉ�
        }
        stulId = ulId;//�ǉ�
        
        if (attr) {
            strncpy_s(attr, 62, buf, _TRUNCATE);
        }
        if (data) {
            strncpy_s(data, length, &buf[62], _TRUNCATE);
#if REPLO_TRIM == 1
            // �����̐��䕶�����폜
            while (strrep(data, "\x1E\x01\x81\x40\x1E\x01", "\x81\x40"));
            while (strtrim(data, "\x1E\x01\x1E\x01\x1E\x01", 0));
            while (strtrim(data, "\x1E\x01\x1E\x01", 0));

            // �^�u�ϊ�������̑O�E���ϊ��iGetLog���Ɂj
            while (strrep(data, "\xEF\x27", "\x1E\x27"));
            while (strrep(data, "\xEF\x28", "\x1E\x28"));

            while (strtrim(data, "\x1F", 1));
            while (strtrim(data, "\x7F", 1));
#endif
            data[length - 1] = '\0';
            res = (int)strlen(data);
        }
    } else {
        res = 0;
    }

    return res;
#else
    return -1;
#endif
}

EXPORT int WINAPI FF11_GetReploW(int index, wchar_t *attr, wchar_t *data, int length, time_t *time)
{
#if REPLO_HOOK == 1
    int res = 0;
    char abuf[62];
    char tbuf[LEN_LOG_BUFFER];

    res = FF11_GetReploA(index, abuf, tbuf, length, time);

    if (res > 0) {
        if (attr) {
            MultiByteToWideChar(CP_ACP, 0, abuf, -1, attr, 61);
            attr[61] = '\0';
        }
        if (data) {
            MultiByteToWideChar(CP_ACP, 0, tbuf, -1, data, length - 1);
            data[length - 1] = '\0';
            res = (int)wcslen(data);
        }
    }
    return res;
#else
    return -1;
#endif
}

EXPORT BOOL WINAPI FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int size)
{
    PTPROC  proc;
    PTDATA  pd;

    if (ptr != NULL && buffer != NULL && size > 0) {
        if (check_index(index, &proc, &pd)) {
            if (ReadProcessMemory(proc->hPro, ptr, buffer, size, NULL))
                return TRUE;
        }
    }
    if (buffer != NULL)
        ZeroMemory(buffer, size);

    return FALSE;
}

EXPORT BOOL WINAPI FF11_GetData(int index, int offset, BYTE *buffer, int size)
{
    PTPROC  proc;
    PTDATA  pd;
    DWORD   addr;
    PBYTE   ptr = NULL;

    if (buffer != NULL && size > 0) {
        if (check_index(index, &proc, &pd)) {
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

    if (buffer != NULL)
        ZeroMemory(buffer, size);

    return FALSE;
}

EXPORT BOOL WINAPI FF11_GetPCnameA(int index, char *name)
{
    BYTE buffer[17];

    if (FF11_GetData(index, FF11ADDR_PTINFO, buffer, 16))
    {
        sprintf_s(name, 16, "%s", buffer);
        return TRUE;
    }

    return FALSE;
}

EXPORT BOOL WINAPI FF11_GetPCnameW(int index, wchar_t *name)
{
    char buffer[17];

    if (FF11_GetPCnameA(index, buffer))
    {
        MultiByteToWideChar(CP_ACP, 0, buffer, -1, name, 16);
        name[16] = '\0';
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
#if REPLO_HOOK == 1
    if (pIREPLO_HOOK) {
        if (pbase)
            detach(pIREPLO_HOOK);
        else
            delete pIREPLO_HOOK;
    }
#endif
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
