#pragma once

#define EXPORT extern "C" __declspec(dllexport)


// FF�̃v���Z�XID��Ԃ�
// no			no�ԖڂɌ�����v���Z�X(0�ȏ�)
// �߂�l		��������ƃv���Z�XID�A���s����0
EXPORT DWORD WINAPI FF11_GetProcessId(int no);


// FFXiMain.dll�̃x�[�X�A�h���X���擾
// pid			pol.exe�̃v���Z�XID
// �߂�l		�x�[�X�A�h���X�B���s����NULL
EXPORT PBYTE WINAPI FF11_GetBaseAddress(DWORD pid);


// dll��pol.exe�ɒ�������
// pid			�^�[�Q�b�gPOL.exe�̃v���Z�XID
// �߂�l		��������Ɗm�ۂ���index�l(1�ȏ�)��Ԃ��B
//				FFXiMain.dll�������[�h����0�A���̑��̗v���Ŏ��s��-1��Ԃ�
//				�ȍ~����index�l���g����dll�ƒʐM���s��
EXPORT int WINAPI FF11_Inject(DWORD pid);


// dll��pol.exe������O��
// index		FFInject�̖߂�l�Bindex = 0�őS�C�W�F�N�g
// �߂�l		�܂��g�p����index��
EXPORT int WINAPI FF11_Eject(int index);


// �e�A�h���X���擾
// index		FFInject�̖߂�l
// object		�擾����A�h���X
// �߂�l		�������̓A�h���X�|�C���^�A���s����NULL
EXPORT PBYTE WINAPI FF11_GetAddressA(int index, int object, char *name);
EXPORT PBYTE WINAPI FF11_GetAddressW(int index, int object, wchar_t *name);
EXPORT PBYTE WINAPI FF11_SetAddress(int index, int object, DWORD address);

// �A�h���Xobject�ꗗ
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

// �C���W�F�N�g�����t���O�̃`�F�b�N
// index		FFInject�̖߂�l
// �߂�l		�e�@�\�̏���������Ԃ�Ԃ��B�����������̓r�b�gON
EXPORT int WINAPI FF11_Rdy(int index);

// �߂�l��Bit
#define FF11_Rdy_GetLog			0x0001
#define FF11_Rdy_GetCmd			0x0002
#define FF11_Rdy_PutCmd			0x0004
#define FF11_Rdy_KillEffect		0x0010
#define FF11_Rdy_MergeItem		0x0020
#define FF11_Rdy_Macro			0x0040	// ������


// FF11�̃��O���擾����
// index		FFInject�̖߂�l
// attribute	�A�g���r���[�g�i�[�o�b�t�@
// buffer		���O�i�[�o�b�t�@
// buffer_size	buffer�̃T�C�Y�B���̃T�C�Y�ȍ~�̓J�b�g�����
// fcut			���O�J�b�g���ꂽ���O��1���Z�b�g�����
// �߂�l		���O�����񒷁B�����ł��ĂȂ�����-1��Ԃ�
EXPORT int WINAPI FF11_GetLogA(int index, BYTE *attribute, char *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ���O�J�b�g���ꂽ���O��1������
#else
	,BYTE	*fcut;
#endif
);

// FF11�̃��O���擾����(UNICODE��)
EXPORT int WINAPI FF11_GetLogW(int index, BYTE *attribute, wchar_t *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ���O�J�b�g���ꂽ���O��1������
#else
	,BYTE	*fcut;
#endif
);

// ���O�J�b�g�g�p��ON/OFF�ؑւ�
// index		FFInject�̖߂�l
// val			0-���O�J�b�gOFF 1-���O�J�b�gON
EXPORT BOOL WINAPI FF11_LogCut(int index, BYTE val);


// ���O�J�b�g��������Z�b�g����
// index		FFInject�̖߂�l
// attr			�L���A�g���r���[�g�z��Aattr = NULL�őS�A�g���r���[�g
// str			���O�J�b�g���镶����B^��擪�ɂ���ƑO����v�Ƃ���
// �߂�l		���O�J�b�g�o�^��
EXPORT int WINAPI FF11_SetCutStringA(int index, char *attr, char *str);
EXPORT int WINAPI FF11_SetCutStringW(int index, wchar_t *attr, wchar_t *str);


// �o�^�ς݂̃��O�J�b�g�����N���A����
// index		FFInject�̖߂�l
// �߂�l		�Ȃ�
EXPORT BOOL WINAPI FF11_ClrCutString(int index);

// ini���烍�O�J�b�g�������o�^
// index		FFInject�̖߂�l
// ini			ini�t�@�C����
// �߂�l		�Ȃ�
EXPORT BOOL WINAPI FF11_LoadLogCutINIA(int index, char *ini);
EXPORT BOOL WINAPI FF11_LoadLogCutINIW(int index, wchar_t *ini);

// FF11�̃R�}���h���擾����
// index		FFInject�̖߂�l
// buffer		�R�}���h���擾����o�b�t�@
// buffer_size	buffer�̃T�C�Y�B���̃T�C�Y�ȍ~�̓J�b�g�����
// �߂�l		�R�}���h�����񒷁B�����ł��ĂȂ�����-1��Ԃ�
EXPORT int WINAPI FF11_GetCmdA(int index, char *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ���O�J�b�g���ꂽ���O��1������
#else
	,BYTE	*fcut;
#endif
);
EXPORT int WINAPI FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size
#ifdef __cplusplus
	,BYTE	*fcut = NULL	// ���O�J�b�g���ꂽ���O��1������
#else
	,BYTE	*fcut;
#endif
);


// �R�}���h�J�b�g�g�p��ON/OFF�ؑւ�
// index		FFInject�̖߂�l
// val			0-�R�}���h�J�b�gOFF 1-�R�}���h�J�b�gON
EXPORT BOOL WINAPI FF11_CmdCut(int index, BYTE val);

// �R�}���h�J�b�g��������Z�b�g����
// index		FFInject�̖߂�l
// str			�R�}���h�J�b�g���镶����B^��擪�ɂ���ƑO����v�Ƃ���
// �߂�l		�R�}���h�J�b�g�o�^��
EXPORT int WINAPI FF11_SetCutCmdA(int index, char *str);
EXPORT int WINAPI FF11_SetCutCmdW(int index, wchar_t *str);


// �o�^�ς݂̃R�}���h�J�b�g�����N���A����
// index		FFInject�̖߂�l
// �߂�l		�Ȃ�
EXPORT BOOL WINAPI FF11_ClrCutCmd(int index);


// FF11�ɃR�}���h���s
// index		FFInject�̖߂�l
// command		�R�}���h���i�[�����o�b�t�@
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_PutCmdA(int index, char *command);
EXPORT BOOL WINAPI FF11_PutCmdW(int index, wchar_t *command);


// KillEffect�R�}���h
// index		FFInject�̖߂�l
// id			���������A�C�R����ID
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_KillEffect(int index, int id);


// MergeItem�R�}���h
// index		FFInject�̖߂�l
// object		0-���΂� 1-���O���� 2-���[ 3-���O�T�b�`�F�� 4-���O�T�b�N
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_MergeItem(int index, int object);


// POL�v���Z�X���̃��������[�h
// index		FFInject�̖߂�l
// ptr			���[�h����A�h���X(��΃A�h���X)
// buffer		�i�[�p�o�b�t�@
// size			���[�h�T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_ReadMemory(int index, BYTE *ptr, BYTE *buffer, int size);


// �L�����N�^���ǂݍ���
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetCharInfo(int index, BYTE *buffer, int size);


// PT���ǂݍ���
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetPTInfo(int index, BYTE *buffer, int size);


// NPCMAP�ǂݍ���
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetNPCMAP(int index, BYTE *buffer, int size);


// NPC���ǂݍ���
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetNPCData(int index, int id, BYTE *buffer, int size);

// �^�[�Q�b�g����ID���擾
// index		FFInject�̖߂�l
// �߂�l		ID�B���s����0
EXPORT WORD WINAPI FF11_GetTargetId(int index);

// �^�[�Q�b�g���̏���Ԃ�
// index		FFInject�̖߂�l
// id			�^�[�Q�b�g����ID���i�[����o�b�t�@
// sid			�T�u�^�[�Q�b�g����ID���i�[����o�b�t�@
// lastst		���X�g�^�[�Q�b�g�̖��O���i�[����o�b�t�@(24�����ȏ�)
// �߂�l		ID�B���s����0
EXPORT BOOL WINAPI FF11_GetTargetInfoA(int index, WORD *id, WORD *sid, char *lastst);
EXPORT BOOL WINAPI FF11_GetTargetInfoW(int index, WORD *id, WORD *sid, wchar_t *lastst);


// �A�r���e�B�̏��擾
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetAbiTable(int index, BYTE *buffer, int size);


// ���@���L���X�g�̏��擾
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetMagicTable(int index, BYTE *buffer, int size);


// ���@�Z�b�g�̃y�i���e�B����
// index		FFInject�̖߂�l
// �߂�l		�y�i���e�B����
EXPORT WORD WINAPI FF11_GetAomaHold(int index);


// �G���A�̓V��ID
// index		FFInject�̖߂�l
// �߂�l		�V��ID
EXPORT BYTE WINAPI FF11_GetWeatherId(int index);


// �A�C�e�����Ń^�[�Q�b�g���̃p�[������LS��
// index		FFInject�̖߂�l
// name			���O�i�[�p�o�b�t�@(32�����ȏ�)
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetLSNameA(int index, char *name);
EXPORT BOOL WINAPI FF11_GetLSNameW(int index, wchar_t *name);


// �A�C�R���e�[�u�����
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// num			�擾����A�C�R���̐�(64���炢?)
// �߂�l		�擾�����A�C�R���̐�
EXPORT int WINAPI FF11_GetIcon(int index, WORD *buffer, int num);

// �����|���
// index		FFInject�̖߂�l
// rimipo		���~�|�i�[�p�o�b�t�@
// meripo		�����|�i�[�p�o�b�t�@
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetMeripo(int index, short *rimipo, char *meripo);


// ���݂̃A�N�V����ID
// index		FFInject�̖߂�l
// �߂�l		�A�N�V����ID
EXPORT WORD WINAPI FF11_GetActionId(int index);


// �^�[�Q�b�g���̃A�C�e�����̎擾
// index		FFInject�̖߂�l
// id			�A�C�e��ID�i�[�p�o�b�t�@
// info			�A�C�e�����i�[�p�o�b�t�@(4�o�C�g)
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetTargetItem(int index, WORD *id, BYTE *info);


// �t�����h���̎擾
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetFList(int index, BYTE *buffer, int size);


// �������e�[�u���̎擾
// index		FFInject�̖߂�l
// tequip		�i�[�p�o�b�t�@(128�o�C�g)
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetEquipTable(int index, BYTE *tequip);


// ���΂�����e�[�u���̎擾
// index		FFInject�̖߂�l
// buffer		�i�[�p�o�b�t�@
// size			���̃T�C�Y
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetItemTable(int index, BYTE *buffer, int size);


// ���΂���
// index		FFInject�̖߂�l
// num			��������(�M�����ɂ��+1�̒l)
// max			���΂�̗e��(�M�����ɂ��+1�̒l)
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
EXPORT BOOL WINAPI FF11_GetBag(int index, BYTE *num, BYTE *max);


// �G���A�`�F���W���t���O
// index		FFInject�̖߂�l
// �߂�l		TRUE-�G���A�`�F���W���AFALSE-�ʏ�
EXPORT BOOL WINAPI FF11_IsArea(int index);


// POL��HWND
// index		FFInject�̖߂�l
// �߂�l		��������HWND�̒l�A���s����0
EXPORT HWND WINAPI FF11_GetHWND(int index);


// �I�𑋂̑I�����̎w��
// index		FFInject�̖߂�l
// no			�ԍ� 1-
// �߂�l		�r����-TRUE�A�ʏ�-FALSE
EXPORT BOOL	WINAPI FF11_SetChoose(int index, int no);

// ���b�N�I����Ԃ̎擾
// index		FFInject�̖߂�l
// �߂�l		���b�N��-TRUE�A�A�����b�N-FALSE
EXPORT WORD	WINAPI FF11_GetLockOn(int index);

// �y�b�gID�̎擾
// index		FFInject�̖߂�l
// �߂�l		ID
EXPORT WORD	WINAPI FF11_GetPetID(int index);

// �L���X�g���̎擾(������)
// index		FFInject�̖߂�l
// �߂�l		�r����-TRUE�A�ʏ�-FALSE
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
