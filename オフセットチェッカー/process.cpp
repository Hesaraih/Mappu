//���j�R�[�h�ŃR���p�C��
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "main.h"
#include "process.h"


//���W���[���֘A�̎擾
//Num�Ԗڂ̃v���Z�X�̏����擾(�ʏ�1�A�����v���Z�X�̂���ꍇ�̂�2�ȏ�)
DWORD GetDllModuleAddress(int Num,int *pId,HANDLE *polHandle,_TCHAR *sztDllFullPath)//0.05�ŕύX
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int nRet;

	if(Num < 1){
		return 0;
	}
	if(*polHandle)CloseHandle(*polHandle);

	nRet = GetProcess(&pe32,Num);
	if(nRet != 0){
		return nRet;
	}
	
	//pol�̃v���Z�X�n���h�����擾
	*polHandle = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);
	if(*polHandle == NULL){
		return 0;
	}

	nRet = GetModule(&mo32,pe32);
	if(nRet != 0){
		return nRet;
	}

	*pId = pe32.th32ProcessID;

	if(sztDllFullPath != NULL){//0.05�Œǉ�
		_tcscpy_s(sztDllFullPath,MAX_PATH,(_TCHAR*)mo32.szExePath);
	}

	return (DWORD)mo32.modBaseAddr;
}

//�����v���Z�X�Ή��v���Z�XID���擾
int GetProcess(PROCESSENTRY32 *pe32,int iProcessCounter)
{
	int nRet = 1;
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
			if(wcscmp(_T("pol.exe"),pe32->szExeFile) == 0){
				iProcessCounter--;
				if(iProcessCounter <=0){
					nRet = 0;
					break;
				}
			}
		}while(Process32Next(hSnapshot,pe32));
	}
	CloseHandle(hSnapshot);

	if(nRet == 1){
		ZeroMemory(pe32,sizeof(PROCESSENTRY32));
	}

	return nRet;
}

//�����v���Z�X�Ή����W���[�����擾
int GetModule(MODULEENTRY32 *mo32,PROCESSENTRY32 pe32)
{
	int nFlag = 0;
	//���W���[���̃X�i�b�v�V���b�g���쐬
	HANDLE hModuleSnapshot;
	hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe32.th32ProcessID);

	if(hModuleSnapshot == INVALID_HANDLE_VALUE){//0.5�ŏC��
		return 1;
	}

	//FFXiMain.dll�̃��W���[���̏����擾
	ZeroMemory(mo32,sizeof(MODULEENTRY32));
	mo32->dwSize = sizeof(MODULEENTRY32);

	if(Module32First(hModuleSnapshot,mo32)){
		do{
			if(wcscmp(_T("FFXiMain.dll"),mo32->szModule) == 0){
				nFlag = 1;
				break;
			}
		}while(Module32Next(hModuleSnapshot,mo32));
	}
	CloseHandle(hModuleSnapshot);

	if(nFlag == 0){
		return 2;
	}

	return 0;
}
