#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//���j�R�[�h�ŃR���p�C��
#ifndef _UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "process.h"
#include "resource.h"
#include "memorymap.h"//0.37�Œǉ�
#include "searchaddr.h"//0.37�Œǉ�

#define STRING_POL	_T("pol.exe")
#define STRING_DLL	_T("FFXiMain.dll")

extern volatile HANDLE g_polHandle;//�v���C�I�����C���̃n���h��
extern volatile DWORD g_ProcessID;//�Ď�����v���Z�XID
extern volatile DWORD g_DllAddress;//FFXiMain.dll�̃A�h���X
extern volatile DWORD g_DllSize;//FFXiMAin.dll�̃T�C�Y
extern FFXIOFFSET g_Offset;//0.37�Œǉ�


//���W���[���֘A�̎擾
//Num�Ԗڂ̃v���Z�X�̏����擾(�ʏ�1�A�����v���Z�X�̂���ꍇ�̂�2�ȏ�)
DWORD GetDllModuleAddress(int Num)
{
	HANDLE hPol;//0.37�Œǉ�
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int iRet;

	if(Num < 1){
		return -1;
	}
	if(g_polHandle)CloseHandle(g_polHandle);

	iRet = GetProcess(&pe32,Num);
	if(iRet != 0){
		return -2;
	}
	
	//pol�̃v���Z�X�n���h�����擾
	hPol = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);//0.37�ŕύX

	//if(g_polHandle == NULL){
	if(hPol == NULL){//0.37�ŕύX
		return -3;
	}

	iRet = GetModule(&mo32,pe32);
	if(iRet != 0){
		return -4;
	}

	g_polHandle = hPol;
	g_ProcessID = pe32.th32ProcessID;
	g_DllSize = mo32.modBaseSize;

	return (DWORD)mo32.modBaseAddr;
}

//�����v���Z�X�Ή��v���Z�XID���擾
int GetProcess(PROCESSENTRY32 *pe32,int iProcessCounter)
{
	int iRet = 1;
	//�v���Z�X�̃X�i�b�v�V���b�g�ꗗ�擾
	HANDLE hSnapshot;
	ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	pe32->dwSize = sizeof(PROCESSENTRY32);

	if(iProcessCounter < 1){
		return 1;
	}

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)return 0;
	
	//pol.exe�̃v���Z�X���擾
	if(Process32First(hSnapshot,pe32)){
		do{
			if(_tcscmp (STRING_POL,pe32->szExeFile) == 0){
				iProcessCounter--;
				if(iProcessCounter <=0){
					iRet = 0;
					break;
				}
			}
		}while(Process32Next(hSnapshot,pe32));
	}
	CloseHandle(hSnapshot);

	if(iRet == 1){
		ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	}

	return iRet;
}

//�����v���Z�X�Ή����W���[�����擾
int GetModule(MODULEENTRY32 *mo32,PROCESSENTRY32 pe32)
{
	int iFlag = 0;
	//���W���[���̃X�i�b�v�V���b�g���쐬
	HANDLE hModuleSnapshot;
	hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe32.th32ProcessID);

	if(hModuleSnapshot == NULL){
		return 1;
	}

	//FFXiMain.dll�̃��W���[���̏����擾
	ZeroMemory(mo32,sizeof(MODULEENTRY32));
	mo32->dwSize = sizeof(MODULEENTRY32);

	if(Module32First(hModuleSnapshot,mo32)){
		do{
			if(_tcscmp (STRING_DLL,mo32->szModule) == 0){
				iFlag = 1;
				break;
			}
		}while(Module32Next(hModuleSnapshot,mo32));
	}
	CloseHandle(hModuleSnapshot);

	if(iFlag == 0){
		return 2;
	}

	return 0;
}

//�v���Z�XID�����݂��邩�m�F
int CheckProcessID(DWORD pID)
{
	int iFlag = 0;
	//�v���Z�X�̃X�i�b�v�V���b�g�ꗗ�擾
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32,sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)return 0;
	
	//pol.exe�̃v���Z�X���擾
	if(Process32First(hSnapshot,&pe32)){
		do{
			if(pID == pe32.th32ProcessID){
				iFlag = 1;
				break;
			}
		}while(Process32Next(hSnapshot,&pe32));
	}
	CloseHandle(hSnapshot);

	if(iFlag == 0){
		return 2;
	}

	return 0;
}

//PID�ꗗ���쐬����
//HWND�ɂ̓R���{�{�b�N�X�̃n���h��
int MakeListPID(HWND hWnd)
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;//0.37�Œǉ�
	_TCHAR str[0x20];
	int i;

	//�R���{�{�b�N�X��������
	while(SendMessage(hWnd,CB_GETCOUNT,0,0) != 0){
		SendMessage(hWnd,CB_DELETESTRING,0,0);
	}


	//PID������΃R���{�{�b�N�X�ɒǉ�
	for(i=1;i<=0x10;i++){
		if(GetProcess(&pe32,i) == 0 && GetModule(&mo32,pe32) == 0){//0.37�ŕύX
			if(GetPCNameFromPid(pe32.th32ProcessID,str,sizeof(str)) != 0){//0.37�Œǉ�
				_stprintf(str,_T("PID:%03X"),pe32.th32ProcessID);
			}
			SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)str);
			if(g_ProcessID == pe32.th32ProcessID){//���łɑI������Ă���PID�ɃJ�[�\�����Z�b�g
				SendMessage(hWnd,CB_SETCURSEL,i-1,0);
			}
		}
		else break;
	}

	if(i == 1){//���PID��������Ȃ��ꍇ
		return 0;
	}

	//�R���{�{�b�N�X�̃J�[�\�����Z�b�g����Ă��Ȃ��ꍇ
	if(SendMessage(hWnd,CB_GETCURSEL,0,0) == -1){
		if(SendMessage(hWnd,CB_GETLBTEXT,0,(LPARAM)str) != CB_ERR){
			GetProcess(&pe32,1);//0.37�ŕύX
			g_ProcessID = pe32.th32ProcessID;

			SendMessage(hWnd,CB_SETCURSEL,0,0);//��ԏ���Z�b�g
		}
	}

	return 1;
}


//PID����E�B���h�E�n���h�����擾�@0.18�ŕ���
HWND GetHWNDFromPID(DWORD dwPID)
{
	DWORD ProcessID;//0.35�ŕύX
	HWND hWnd = GetTopWindow(NULL);

	do{
		if(GetWindowLong(hWnd,GWL_HWNDPARENT) != 0 || !IsWindowVisible(hWnd)){
			continue;
		}
		GetWindowThreadProcessId(hWnd,&ProcessID);
		if(dwPID == ProcessID){
			return hWnd;
		}
	}while((hWnd = GetNextWindow(hWnd,GW_HWNDNEXT)) != NULL);

	return NULL;
}

//PID����L���������擾 0.37�Œǉ�
int GetPCNameFromPid(DWORD dwPID,_TCHAR *szPCName,int nSize)
{
	HANDLE hPol;
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;

	ZeroMemory(&pe32,sizeof(PROCESSENTRY32));
	pe32.th32ProcessID = dwPID;
	//pol�̃v���Z�X�n���h�����擾
	hPol = OpenProcess(PROCESS_VM_READ,false,dwPID);


	if(GetModule(&mo32,pe32) != 0){
		return -1;
	}

	if(0 != GetPCNameFromHandle(hPol,(DWORD)mo32.modBaseAddr,0,szPCName,nSize)){
		return -2;
	}

	if(!(_tcslen(szPCName) >= 3 && szPCName[0] >= 'A' && szPCName[0] <= 'Z')){
		return -3;
	}

	return 0;
}

//0.37�Œǉ�
int GetPCNameFromHandle(HANDLE hPol,DWORD dwDllAddr,int pos,_TCHAR* szPCName,int nSize)
{
	DWORD dwAddress;
	char dump[0x210];

	//�\���̏�����
	ZeroMemory(szPCName,sizeof(szPCName));
	
	//���������[�h
	dwAddress = dwDllAddr + g_Offset.NowSta + NOWSTA_HOSEI + pos * NOWSTA_SIZE;//�X�^�[�g�ʒu�̕␳NOWSTA_HOSEI
	if(0 == ReadProcessMemory(hPol,(const void *)dwAddress,dump,sizeof(dump),NULL)){
		return -1;
	}

	//NOWSTA_MEM_PARTY��0�̎����łɒE��
	if(dump[NOWSTA_MEM_PARTY] == 0x01){
		mbstowcs(szPCName,&dump[NOWSTA_MEM_NAME],nSize-1);
	}
	else{
		return -2;
	}

	return 0;
}

//����PID��I�� 0.43�Œǉ�
int SetInitPID(HWND hWnd,_TCHAR* str)
{
	if(_tcslen(str) == 0)return -1;

	int nPIDcount = SendMessage(hWnd,CB_GETCOUNT,0,0);
	_TCHAR szText[0x20];

	if(nPIDcount <= 0)return -1;

	for(int i=0;i<nPIDcount;i++){
		SendMessage(hWnd,CB_GETLBTEXT,i,(LPARAM)szText);
		if(_tcscmp (str,szText) == 0){
			SendMessage(hWnd,CB_SETCURSEL,i,0);
			SendMessage(GetParent(hWnd),WM_COMMAND,IDC_COMBO_PID|(CBN_SELCHANGE<<16),0);
			break;
		}
	}

	return 0;
}