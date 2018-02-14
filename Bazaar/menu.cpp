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
#include "main.h"
#include "menu.h"
#include "process.h"
#include "ffxiinfo.h"
#include "resource.h"


//���j���[��ύX false��Disable true��Enable
int MenuItemEnable(HMENU hMenu,DWORD dwID,BOOL bFlag)
{
	MENUITEMINFO mii;

	//���j���[�̈ꕔ���O���[��
	ZeroMemory(&mii,sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(bFlag == (BOOL)true){
		mii.fState = MFS_ENABLED;
	}
	else{
		mii.fState = MFS_DISABLED;
	}
	SetMenuItemInfo(hMenu,dwID,false,&mii);

	return 1;
}


//PID�I�����j���[�̍쐬
int MenuMakePID(HMENU hMenu)
{
	PROCESSENTRY32 pe32;
	_TCHAR szMenuStr[0x20];
	HMENU hPIDMenu;

	for(int nCount=1;nCount<=MAX_PID_COUNT;nCount++){//��������j���[���ڂ��폜
		if(DeleteMenu(hMenu,IDM_PID + nCount,FALSE) == 0){//���s������
			break;
		}
	}

	hPIDMenu = GetSubMenu(hMenu,1);

	//�v���Z�X�����J�E���g
	for(int nCount=1;nCount<=MAX_PID_COUNT;nCount++){
		if(GetProcess(&pe32,nCount) != 0)break;
		else{
			//���j���[�ɒǉ�
			//���j���[�𑝂₵���ꍇ�}����ɒ��ӂ��邱��
			_swprintf(szMenuStr,_T("PID:%X"),pe32.th32ProcessID);
			InsertMenu(hPIDMenu,nCount,MF_BYPOSITION|MF_STRING,IDM_PID + nCount,szMenuStr);
			//���݊Ď����Ă���PID�Ɠ����Ȃ�`�F�b�N����
			if(g_ProcessID == pe32.th32ProcessID){
				CheckMenuItem(hPIDMenu,IDM_PID + nCount,MF_BYCOMMAND|MFS_CHECKED);
			}
		}
	}

	return 0;
}


//���j���[�̃`�F�b�N�̃A���`�F�b�N�؂�ւ�
BYTE MenuChangeCheck(HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	BYTE byRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	mii.fMask = MIIM_STATE;
	if(mii.fState == MFS_CHECKED){//�`�F�b�N����Ă���ꍇ
		mii.fState = MFS_UNCHECKED;
		byRet = 0;
	}
	else{//�`�F�b�N����Ă��Ȃ��ꍇ
		mii.fState = MFS_CHECKED;
		byRet = 1;
	}
	//�`�F�b�N�{�b�N�X�X�V
	if(SetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	
	return byRet;
}

//���j���[�̏�Ԃ��m�F�`�F�b�N��1�A���`�F�b�N��0
UINT MenuGetCheck(HMENU hMenu,UINT uID)
{
	MENUITEMINFO mii;
	UINT uRet;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if(GetMenuItemInfo(hMenu,uID,FALSE,&mii) == 0){
		return -1;
	}
	if(mii.fState == MFS_CHECKED){//�`�F�b�N����Ă���ꍇ
		uRet = 1;
	}
	else{//�`�F�b�N����Ă��Ȃ��ꍇ
		uRet = 0;
	}
	
	return uRet;
}


//wanted\*.ini���T�[�`���Ĉꗗ�쐬
int MenuMakeFindWantedIni(HMENU hMenu)
{
	WIN32_FIND_DATA wfd;
	int nFindNum = 0;
	HANDLE hFind;
	HMENU hSubMenu;
	HMENU hWantedMenu;
	HANDLE hFile;
	_TCHAR szPath[MAX_PATH];

	hSubMenu = GetSubMenu(hMenu,3);
	hWantedMenu = GetSubMenu(hSubMenu,2);

	
	//�J�����g�f�B���N�g����wanted.ini���T�[�`
	hFile = CreateFile(_T("wanted.ini"),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		MenuItemEnable(hWantedMenu,IDM_LOAD_WANTED,true);
		nFindNum++;
		//���ݎg�p���Ă���ini�Ɠ����Ȃ�`�F�b�N����
		if(wcscmp(g_WantedIniFileName,_T("wanted.ini")) == 0){
			CheckMenuItem(hWantedMenu,IDM_LOAD_WANTED,MF_BYCOMMAND|MFS_CHECKED);
		}
	}
	else{
		if(wcscmp(g_WantedIniFileName,_T("wanted.ini")) == 0){//wanted.ini��������Ȃ����Ǎ��݃t�@�C����wanted.ini�̎�(������)
			_swprintf(g_WantedIniFileName,_T(""));
		}
		MenuItemEnable(hWantedMenu,IDM_LOAD_WANTED,false);
	}
	CloseHandle(hFile);

	for(int nCount=1;nCount<g_WantedIni;nCount++){//��������j���[���ڂ��폜
		if(DeleteMenu(hWantedMenu,IDM_LOAD_WANTED + nCount,MF_BYCOMMAND) == 0){//���s������
			g_WantedIni = nFindNum;
			return 0;
		}
	}

	hFind = FindFirstFile(_T("wanted\\*.ini"),&wfd);
	if(hFind == INVALID_HANDLE_VALUE){
		g_WantedIni = nFindNum;
		return 0;
	}

	//wanted�f�B���N�g�����T�[�`
	do{
		_swprintf(szPath,_T("wanted\\%s"),wfd.cFileName);
		InsertMenu(hWantedMenu,IDM_LOAD_WANTED + nFindNum + 1,MF_BYCOMMAND|MF_STRING,IDM_LOAD_WANTED + nFindNum,szPath);
		//���ݎg�p���Ă���ini�Ɠ����Ȃ�`�F�b�N����
		if(wcscmp(g_WantedIniFileName,szPath) == 0){
			CheckMenuItem(hWantedMenu,IDM_LOAD_WANTED + nFindNum,MF_BYCOMMAND|MFS_CHECKED);
		}
		nFindNum++;
	}while(FindNextFile(hFind,&wfd));//���s���͏I����0
	
	if(GetLastError() == ERROR_NO_MORE_FILES){//�񋓏I��
	}
	else{//���s
		nFindNum = 0;
	}

	FindClose(hFind);

	g_WantedIni = nFindNum;

	return nFindNum;
}