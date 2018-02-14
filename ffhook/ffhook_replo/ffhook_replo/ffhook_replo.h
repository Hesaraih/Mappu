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
#define FF11_Rdy_GetReplo		0x0100


// FF11�̃��O���擾����
// index		FFInject�̖߂�l
// attr			�A�g���r���[�g������i�T�C�Y��53�K�v�j
// data			���O������
// data_size	���O������̃T�C�Y�B���̃T�C�Y�ȍ~�̓J�b�g�����
// time			���O�o�͎���
// �߂�l		���O�����񒷁B�����ł��ĂȂ�����-1��Ԃ�
EXPORT int WINAPI FF11_GetReploA(int index, char *attr, char *data, int data_size, time_t *time);

// FF11�̃��O���擾����(UNICODE��)
EXPORT int WINAPI FF11_GetReploW(int index, wchar_t *attr, wchar_t *data, int data_size, time_t *time);


// ���O�C�����Ă���L�����N�^�[�l�[�����擾����
// index		FFInject�̖߂�l
// name			�L�����N�^�[�l�[��
// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
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
