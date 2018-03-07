#pragma once

#define EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif


	// FF�̃v���Z�XID��Ԃ�
	// no			no�ԖڂɌ�����v���Z�X(0�ȏ�)
	// �߂�l		��������ƃv���Z�XID�A���s����0
	EXPORT DWORD WINAPI _stdcall FF11_GetProcessId(int no);


	// FFXiMain.dll�̃x�[�X�A�h���X���擾
	// pid			pol.exe�̃v���Z�XID
	// �߂�l		�x�[�X�A�h���X�B���s����NULL
	EXPORT PBYTE WINAPI _stdcall FF11_GetBaseAddress(DWORD pid);


	// dll��pol.exe�ɒ�������
	// pid			�^�[�Q�b�gPOL.exe�̃v���Z�XID
	// �߂�l		��������Ɗm�ۂ���index�l(1�ȏ�)��Ԃ��B
	//				FFXiMain.dll�������[�h����0�A���̑��̗v���Ŏ��s��-1��Ԃ�
	//				�ȍ~����index�l���g����dll�ƒʐM���s��
	EXPORT int WINAPI _stdcall FF11_Inject(DWORD pid);


	// dll��pol.exe������O��
	// index		FFInject�̖߂�l�Bindex = 0�őS�C�W�F�N�g
	// �߂�l		�܂��g�p����index��
	EXPORT int WINAPI _stdcall FF11_Eject(int index);


	// �C���W�F�N�g�����t���O�̃`�F�b�N
	// index		FFInject�̖߂�l
	// �߂�l		�e�@�\�̏���������Ԃ�Ԃ��B�����������̓r�b�gON
	//				FFXiMain.dll���������ꍇ��-1��Ԃ�
#define FF11_Rdy_GetLog			0x0001
#define FF11_Rdy_GetCmd			0x0002
#define FF11_Rdy_PutCmd			0x0004
#define FF11_Rdy_KillEffect		0x0010
#define FF11_Rdy_MergeItem		0x0020
	EXPORT int WINAPI _stdcall FF11_Rdy(int index);


	// �e�A�h���X���擾
	// index		FFInject�̖߂�l
	// object		�擾����A�h���X
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

	// �߂�l		�������̓A�h���X�|�C���^�A���s����NULL
	EXPORT PBYTE WINAPI _stdcall FF11_GetAddress(int index, int object);


	// FF11�̃��O���擾����
	// index		FFInject�̖߂�l
	// attribute	�A�g���r���[�g�i�[�o�b�t�@
	// buffer		���O�i�[�o�b�t�@
	// buffer_size	���O�i�[�o�b�t�@�T�C�Y
	// fcut			���O�J�b�g���ꂽ���O��1���Z�b�g�����
	// �߂�l		���O�����񒷁B�����ł��ĂȂ�����-1��Ԃ�
	EXPORT int WINAPI _stdcall FF11_GetLogA(int index, BYTE *attribute, char *buffer, int buffer_size, BYTE	*fcut);

	// FF11�̃��O���擾����(UNICODE��)
	EXPORT int WINAPI _stdcall FF11_GetLogW(int index, BYTE *attribute, wchar_t *buffer, int buffer_size, BYTE	*fcut);

	// ���O�J�b�g�g�p��ON/OFF�ؑւ�
	// index		FFInject�̖߂�l
	// val			0��off 1��on
	EXPORT BOOL WINAPI _stdcall FF11_LogCut(int index, BYTE val);


	// ���O�J�b�g��������Z�b�g����
	// index		FFInject�̖߂�l
	// attr			�L���A�g���r���[�g�Aattr = NULL�őS�A�g���r���[�g
	// str			���O�J�b�g���镶����B^��擪�ɂ���ƑO����v�Ƃ���
	// �߂�l		���O�J�b�g�o�^��
	EXPORT int WINAPI _stdcall FF11_SetCutStringA(int index, char *attr, char *str);
	EXPORT int WINAPI _stdcall FF11_SetCutStringW(int index, wchar_t *attr, wchar_t *str);


	// �o�^�������O�J�b�g��������N���A����
	// index		FFInject�̖߂�l
	// �߂�l		�Ȃ�
	EXPORT BOOL WINAPI _stdcall FF11_ClrCutString(int index);

	// FF11�̃R�}���h���擾����
	// index		FFInject�̖߂�l
	// buffer		�R�}���h���擾����o�b�t�@
	// buffer_size	buffer�̃T�C�Y�B���̃T�C�Y�ȍ~�̓J�b�g�����
	// �߂�l		�R�}���h�����񒷁B�����ł��ĂȂ�����-1��Ԃ�
	EXPORT int WINAPI _stdcall FF11_GetCmdA(int index, LPTSTR buffer, int buffer_size);
	EXPORT int WINAPI _stdcall FF11_GetCmdW(int index, wchar_t *buffer, int buffer_size);


	// FF11�ɃR�}���h���s
	// index		FFInject�̖߂�l
	// buffer		�R�}���h���i�[�����o�b�t�@
	// �߂�l		��������TRUE�A���s����FALSE��Ԃ�
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
