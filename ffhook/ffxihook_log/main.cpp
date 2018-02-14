#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#include "process.h"
#include "ffxihook_log.h"
#include "madCHook.h"

#pragma comment(lib,"madCHook")

int main()
{
	int pId = 0;
	HANDLE polHandle = NULL;
	HINSTANCE hDll;

	printf("FFXiMain.dll�̏����擾\n");
	DWORD dwDllAddress = GetDllModuleAddress(1,&pId,&polHandle);
	printf("    pId = 0x%08X\n",pId);
	printf("    polHandle = 0x%08X\n",polHandle);
	printf("    dwDllAddress = 0x%08X\n",dwDllAddress);
	
	/*
	InjectLibrary
	�n���h�����w�肷��ꍇ��OpenProcess��PROCESS_ALL_ACCESS���w��
	*/
	printf("madCHook���C���W�F�N�g\n");
	InjectLibrary((DWORD)polHandle,_T("madCHook.dll"));

	/*
	ffxihook_log�͓��I�����N�œǍ���
	*/
	printf("ffxihook_log�����[�h\n");
	hDll = LoadLibrary(_T("ffxihook_log.dll"));
	printf("    ffxihook_log.dll = 0x%08X\n",hDll);
	if(hDll == NULL){
		printf("  ���[�h�Ɏ��s\n");
	}

	printf("�֐������[�h\n");
	bool (__stdcall *FFXiHookLog_Inject)(DWORD);
	FFXiHookLog_Inject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Inject");
	if(FFXiHookLog_Inject){
		printf("    FFXiHookLog_Inject = 0x%08X\n",FFXiHookLog_Inject);
	}
	bool (__stdcall *FFXiHookLog_Eject)(DWORD);
	FFXiHookLog_Eject = (bool (__stdcall *)(DWORD))GetProcAddress(hDll,"FFXiHookLog_Eject");
	if(FFXiHookLog_Eject){
		printf("    FFXiHookLog_Eject = 0x%08X\n",FFXiHookLog_Eject);
	}
	IFFXiHooks* (__stdcall *FFXiHookLog_Interface)(void);
	FFXiHookLog_Interface = (IFFXiHooks* (__stdcall *)(void))GetProcAddress(hDll,"FFXiHookLog_Interface");
	if(FFXiHookLog_Interface){
		printf("    FFXiHookLog_Interface = 0x%08X\n",FFXiHookLog_Interface);
	}
	if(FFXiHookLog_Inject == NULL || FFXiHookLog_Eject == NULL || FFXiHookLog_Interface == NULL){
		printf("    ���[�h�Ɏ��s\n");
	}
	
	/*
	ffxihook_log���C���W�F�N�g
	madCHook�ł���Ă��ǂ�����
	*/
	printf("ffxihook_log���C���W�F�N�g\n");
	if(FFXiHookLog_Inject(pId) == FALSE){
		printf("    ���s\n");
	}
	
	/*
	ffxihook_log�̏�����
	*/
	printf("ffxihook_log�̏�����\n");
	IFFXiHooks* pHooks = FFXiHookLog_Interface();
	if(pHooks->Open() == FALSE){
		printf("    pHooks->Open()���s\n");
	}

	/*
	���O���擾���Ă݂�
	�v�����v�g�łǂꂩ�L�[�������ƃ��[�v�I��
	*/
	printf("------���[�v�J�n------\n");
	BYTE attr;
	size_t index = 0;
	_TCHAR cstr[0x200];
	while(!_kbhit()){
		attr = 0;
		_stprintf_s(cstr, _T("%s"),pHooks->Get(attr));
		if(attr)printf("%02X : %s\n",attr,cstr);
		Sleep(10);
	}
	printf("------���[�v�I��------\n");
	
	/*
	ffxihook_log�̏I������
	*/
	printf("ffxihook_log�̏I������\n");
	pHooks->Close();
	pHooks->Destroy();
	
	/*
	���ߍ���ffxihook_log.dll���A�����[�h
	*/
	printf("ffxihook_log���G�W�F�N�g\n");
	if(FFXiHookLog_Eject(pId) == 0){
		printf("    ���s\n");
	}
	
	/*
	���ߍ���madCHook.dll���A�����[�h
	*/
	printf("madCHook���G�W�F�N�g\n");
	UninjectLibrary((DWORD)polHandle,_T("madCHook.dll"));
	
	/*
	���
	*/
	FreeLibrary(hDll);
	CloseHandle(polHandle);

	_getch();

	return 0;
}