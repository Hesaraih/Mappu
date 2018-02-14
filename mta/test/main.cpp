#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include "process.h"
#include "..\\mta\\mta.h"

//#define STATICLINK

#ifdef STATICLINK
#pragma comment(lib,"mta.lib")
#endif

int main()
{
#ifdef STATICLINK
	//�ÓI�����N
	MTA mta = MTA(GetPid(1),0x4FCA84,0x40C548);//�������F�v���Z�XID�ATARGET�̃I�t�Z�ANPCMAP�̃I�t�Z
	mta.SetTarget(0xB);//ID=0xB���^�Q��
	printf("�L�[���͑҂�");
	while(!_kbhit()){}//�L�[���͂�����܂őҋ@
	mta.UnSetTarget();//�^�Q����

#else
	//���I�����N
	void* (__stdcall *mta_Init)(DWORD,DWORD,DWORD);
	void* (__stdcall *mta_InitFromIniFile)(DWORD);
	void (__stdcall *mta_Delete)(void*);
	void (__stdcall *mta_SetTarget)(void*,DWORD);
	void (__stdcall *mta_UnSetTarget)(void*);

	HMODULE hModule;
	MTA *pMta;
	if((hModule = LoadLibrary(_T("mta.dll"))) != NULL){
		if((mta_Init = (void* (__stdcall*)(DWORD,DWORD,DWORD))GetProcAddress(hModule,"mta_Init")) == NULL){
			MessageBox(NULL,_T("Cant find mta_Init()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_InitFromIniFile = (void* (__stdcall*)(DWORD))GetProcAddress(hModule,"mta_InitFromIniFile")) == NULL){
			MessageBox(NULL,_T("Cant find mta_InitFromIniFile()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_Delete = (void (__stdcall*)(void*))GetProcAddress(hModule,"mta_Delete")) == NULL){
			MessageBox(NULL,_T("Cant find mta_Delete()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_SetTarget = (void (__stdcall*)(void*,DWORD))GetProcAddress(hModule,"mta_SetTarget")) == NULL){
			MessageBox(NULL,_T("Cant find mta_SetTarget()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		if((mta_UnSetTarget = (void (__stdcall*)(void*))GetProcAddress(hModule,"mta_UnSetTarget")) == NULL){
			MessageBox(NULL,_T("Cant find mta_UnSetTarget()"),_T("mta.dll"),MB_ICONERROR);
			return false;
		}
		//pMta = (MTA*)mta_Init(GetPid(1),0x4FCA84,0x40C548);//�������F�v���Z�XID�ATARGET�̃I�t�Z�ANPCMAP�̃I�t�Z
		pMta = (MTA*)mta_InitFromIniFile(GetPid(1));//�������F�v���Z�XID�A�I�t�Z����mta.dll����擾
	}
	else{
		MessageBox(NULL,_T("Cant find mta.dll"),_T("mta.dll"),MB_ICONERROR);
		return false;
	}
	//mta_SetTarget(pMta,0x400);//ID=0x400���^�Q��
	mta_SetTarget(pMta,0xB);//ID=0xB���^�Q��
	printf("�L�[���͑҂�");
	while(!_kbhit()){}//�L�[���͂�����܂őҋ@
	mta_UnSetTarget(pMta);//�^�Q����
	mta_Delete(pMta);//�I��
	//CloseHandle(hModule);
#endif

	return true;
}