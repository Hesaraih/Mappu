#if(_MSC_VER >= 1400)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//���j�R�[�h�ŃR���p�C��
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "header.h"
#include "main.h"
#include "process.h"

#pragma comment(lib,"psapi.lib")

//���W���[���֘A�̎擾
//Num�Ԗڂ̃v���Z�X�̏����擾(�ʏ�1�A�����v���Z�X�̂���ꍇ�̂�2�ȏ�)
DWORD GetDllModuleAddress(int Num)
{
	PROCESSENTRY32 pe32;
	MODULEENTRY32 mo32;
	int nRet;

	if(Num < 1){
		g_ErrorCode = 1000;
		return 0;
	}
	if(g_polHandle)CloseHandle(g_polHandle);

	nRet = GetProcess(&pe32,Num);
	if(nRet != 0){
		g_ErrorCode = 1010 + nRet;
		return 0;
	}
	
	//pol�̃v���Z�X�n���h�����擾
	g_polHandle = OpenProcess(PROCESS_VM_READ,false,pe32.th32ProcessID);
	if(g_polHandle == NULL){
		g_ErrorCode = 1020;
		return 0;
	}

	nRet = GetModule(&mo32,pe32);
	if(nRet != 0){
		g_ErrorCode = 1030 + nRet;
		return 0;
	}

	g_ProcessID = pe32.th32ProcessID;
	g_DllSize = mo32.modBaseSize;

	//���W���[���̃t�@�C���T�C�Y���擾
	HANDLE hFile;
	hFile = CreateFile(mo32.szExePath,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	g_DLLFileSize = GetFileSize(hFile,NULL);

	CloseHandle(hFile);

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
			if(strcmp(STRING_POL,pe32->szExeFile) == 0){
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

	if(hModuleSnapshot == NULL){
		return 1;
	}

	//FFXiMain.dll�̃��W���[���̏����擾
	ZeroMemory(mo32,sizeof(MODULEENTRY32));
	mo32->dwSize = sizeof(MODULEENTRY32);

	if(Module32First(hModuleSnapshot,mo32)){
		do{
			if(strcmp(STRING_DLL,mo32->szModule) == 0){
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

//�v���Z�XID�����݂��邩�m�F
int CheckProcessID(DWORD pID)
{
	int nFlag = 0;
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
				nFlag = 1;
				break;
			}
		}while(Process32Next(hSnapshot,&pe32));
	}
	CloseHandle(hSnapshot);

	if(nFlag == 0){
		return 2;
	}

	return 0;
}
