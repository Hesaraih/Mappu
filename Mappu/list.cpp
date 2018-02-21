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
#include <stdio.h>
#include <tchar.h>
#include <olectl.h>
#include <commctrl.h>
#include <time.h>
#include <Mmsystem.h>
#include <math.h>
#include "header.h"
#include "main.h"
#include "list.h"
#include "draw.h"
#include "other.h"
#include "resource.h"


#pragma comment(lib,"Comctl32.lib")

//���X�g�̃J�����쐬
int InitList(HWND hWnd)
{
	LVCOLUMN col;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	//���X�g�r���[��EX�X�^�C���t�^
	ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_LIST),
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER,//0.47a�Ń_�u���o�b�t�@�ǉ�
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER
	);

	ZeroMemory(&col,sizeof(col));
	
	//�J�����쐬
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = g_Menu.column.id;//0.50�ŕύX
	col.iSubItem = 0;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 0, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.name;//0.50�ŕύX
	col.iSubItem = 1;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 1, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.state;//0.50�ŕύX
	col.iSubItem = 2;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 2, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.timer;//0.50�ŕύX
	col.iSubItem = 3;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 3, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.type;//0.50�ŕύX
	col.iSubItem = 4;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 4, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.ym;//0.57�Œǉ�
	col.iSubItem = 5;
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 5, &col)) {
		return 0;
	}
	col.cx = g_Menu.column.note;//0.50�ŕύX
	col.iSubItem = 6;//0.57�ŕύX
	col.pszText = (_TCHAR*)szColumn[col.iSubItem];//0.57a�ŕύX
	if (-1 == ListView_InsertColumn(GetDlgItem(hWnd, IDC_LIST), 6, &col)) {//0.57�ŕύX
		return 0;
	}

	return 1;
}

/// <summary>
/// NPC�̃��X�g���쐬
/// </summary>
/// <param name="hWnd"></param>
/// <returns>����擾:2�A1��������Ȃ�:1�A�\�[�g�����s:0</returns>
int MakeList(HWND hWnd)
{
	LVITEM item;
	_TCHAR szId[0x05];
	_TCHAR szName[0x20];

	//NPCLIST�ɕ\�����Ă���G���AID��ۑ�
	g_ListAreaID = GetAreaID();

	//�A�C�e���ǉ�
	for (int i = 0, j = 0; i < MAX_SEARCH; i++) {
		//id,name�擾
		if (GetList(i, szId, szName) != 1) {
			return 2;
		}
		//ID��!=0 && Name�̈ꕶ���ڂ��啶���̎��A2�����ڂ��������̎����X�g�ɒǉ�
		//����"???" ���͈ꕶ���ڂ��X�y�[�X" Veridical Conflux #??"(�G�R�[�Y)
		if (
			_tcstoul(szId, NULL, 16) != 0 &&
			(
			(isupper(szName[0]) != 0 && islower(szName[1]) != 0) ||
				_tcscmp(szName, _T("???")) == 0 ||
				(_tcsncmp(szName, _T(" "), 1) == 0 && isupper(szName[1]) && islower(szName[2]))
				)
			) {
			item.mask = LVIF_TEXT | LVIF_PARAM;//LVIF_PARAM�ǉ��Ń\�[�g�I��
			item.iItem = j;//�s
			item.iSubItem = 0;//��
			item.pszText = szId;//id
			item.lParam = i;//�\�[�g�plParam��ݒ�
			if (-1 == ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST), &item)) {
				return 0;
			}

			item.mask = LVIF_TEXT;
			item.iSubItem = 1;//��
			item.pszText = szName;//Name
			if (FALSE == ListView_SetItem(GetDlgItem(hWnd, IDC_LIST), &item)) {
				return 0;
			}
			j++;
		}
	}

	return 1;
}

/// <summary>
/// ����������NPC��id�A���O�ǂݍ���
/// </summary>
/// <param name="counter"></param>
/// <param name="id"></param>
/// <param name="name"></param>
/// <returns></returns>
int GetList(int counter,_TCHAR *id,_TCHAR *name)
{
	char dump[0x20];
	DWORD dwAddress;
	int nID;
	
	dwAddress = g_DllAddress + g_Offset.NpcList;
	//NpcList�̃|�C���^�[�ǂݍ���
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, &dwAddress, sizeof(DWORD), NULL)) {
		return -1;
	}

	dwAddress = dwAddress + NPCLIST_SIZE * counter;
	if (0 == ReadProcessMemory(g_polHandle, (const void *)dwAddress, dump, NPCLIST_SIZE, NULL)) {
		return -1;
	}//0.47�ŕύX

	memcpy(&nID,&dump[NPCLIST_ID],sizeof(int));
	nID = nID & 0x0FFF;//0.47�ŕύX

	_tcscpy(id,_T("0"));//id������
	if(dump[NPCLIST_SIZE-1] == 0x01 && nID >= counter && nID <= 0x400){
		_stprintf(id,_T("%03X"),nID);//id�쐬
	}
	else if(dump[NPCLIST_SIZE-1] != 0x01 && dump[NPCLIST_SIZE-1] != 0x00){//List�����I��
		return 0;
	}
	
	dump[NPCLIST_ID] = 0;//�ő啶����΍�
#ifdef UNICODE
	mbstowcs(name, dump, NPCLIST_ID + 1);
#else
	memcpy(name, dump, NPCLIST_ID + 1);
#endif
	return 1;
}

//�A�v���P�[�V������`��r�֐�
//lp1��lp2����s����ꍇ�͕��̒l�A�t�̎��͐��̒l�A�������͂O��Ԃ�
int CALLBACK CmpProc(LPARAM lp1,LPARAM lp2,LPARAM lp3)
{
	static LV_FINDINFO lvf;
	static int nItem1, nItem2;
	static _TCHAR szBuf1[0x20], szBuf2[0x20];

	lvf.flags = LVFI_PARAM;
	lvf.lParam = lp1;
	nItem1 = ListView_FindItem(g_hList, -1, &lvf);

	lvf.lParam = lp2;
	nItem2 = ListView_FindItem(g_hList, -1, &lvf);

	ListView_GetItemText(g_hList, nItem1, (int)lp3, szBuf1, sizeof(szBuf1) / sizeof(_TCHAR));
	ListView_GetItemText(g_hList, nItem2, (int)lp3, szBuf2, sizeof(szBuf2) / sizeof(_TCHAR));

	//"-"�̎��͈�ԉ��ɂ��� 0.57�Œǉ�
	if(_tcscmp(szBuf1,_T("-"))==0){
		if (g_Sort[(int)lp3] == UP)_stprintf(szBuf1, _T("ZZZZZZZZZZ"));
		else _stprintf(szBuf1, _T(""));
	}
	else if (wcslen(szBuf1) == 0) {//�� 0.57a�Œǉ�
		if (g_Sort[(int)lp3] == UP)_stprintf(szBuf1, _T("ZZZZZZZZZZZZ"));
	}
	if (_tcscmp(szBuf2, _T("-")) == 0) {
		if (g_Sort[(int)lp3] == UP)_stprintf(szBuf2, _T("ZZZZZZZZZZ"));
		else _stprintf(szBuf2, _T(""));
	}
	else if (wcslen(szBuf2) == 0) {//�� 0.57a�Œǉ�
		if (g_Sort[(int)lp3] == UP)_stprintf(szBuf2, _T("ZZZZZZZZZZZZ"));
	}

	if (g_Sort[(int)lp3] == UP) {
		return _tcscmp(szBuf1, szBuf2);
	}
	else {
		return _tcscmp(szBuf2, szBuf1);
	}
}

//������0�AHWND�ɂ�NPC���X�g�̃n���h��
int List_AddState(HWND hLIST){
	static int snPastChecked = 0;
	int nNowChecked = 0;
	int nCheckedID;
	int nItemCount = ListView_GetItemCount(hLIST);
	LVITEM item;
	_TCHAR szId[0x10];
	_TCHAR szState[0x10];
	INFORMATION target;
	time_t nowtime;
	_TCHAR szTime[0x20];
	_TCHAR szType[0x10];
	_TCHAR szDistance[0x10];//0.57�Œǉ�
	struct STATE{
		BYTE state;//���̃X�e�[�g��INFORMATION�̃X�e�[�g�͒l���قȂ�̂Œ���
		time_t time;
		BYTE timerid;//�^�C�}�[���ǂ�ID��
		BYTE sound;//0���m�[�}���A1��EX�T�E���h�A2��SP�T�E���h
	};
	STATE nowState;
	static STATE pastState[MAX_SEARCH];
//#define ST_OUT		0
//#define ST_WAR		(1<<0)
//#define ST_POP		(1<<1)
//#define ST_CORPSE	(1<<2)
#define ST_NONE		0//0.60�Œǉ��ύX
#define ST_OUT		(1<<0)
#define ST_WAR		(1<<1)
#define ST_POP		(1<<2)
#define ST_CORPSE	(1<<3)

#define ID_TIMER_WAR	(1<<0)
#define ID_TIMER_CORPSE	(1<<1)
#define ID_TIMER_WAIT_VIEW_NULL	(1<<2)

	UINT uSoundType[3]={0,0,0};//0.49�Œǉ�
#define CORPSE_POP	(1<<0)
#define CORPSE_OUT	(1<<1)
#define CORPSE_WAR	(1<<2)
#define WAR_POP	(1<<3)
#define OUT_POP	(1<<4)
#define	OUT_CORPSE	(1<<5)
#define	OUT_WAR	(1<<6)
#define	POP_WAR	(1<<7)
#define	POP_OUT	(1<<8)
#define POP_CORPSE	(1<<9)
#define	WAR_CORPSE	(1<<10)
#define	WAR_OUT	(1<<11)

	//���݂̎��Ԃ��擾(�b)
	time(&nowtime);
	
	//�`�F�b�N���ꂽNPC�����擾
	for (int i = 0; i < nItemCount; i++) {
		if (ListView_GetCheckState(hLIST, i) != 0) {
			nNowChecked++;
		}
	}

	HWND hDebugWnd;//0.53�Œǉ�
	_TCHAR szSendText[MAX_PATH];
	ZeroMemory(szSendText, sizeof(szSendText));
	hDebugWnd = FindWindow(_T("GetMessageWnd"), NULL);

	item.mask = LVIF_TEXT;
	item.pszText = _T("");//������
	//�`�F�b�N���ꂽNPC���ɕϓ�������Ζ��`�F�b�N��NPCLIST�y��pastState���N���A
	if (nNowChecked != snPastChecked) {
		for (int i = 0; i < nItemCount; i++) {
			item.iItem = i;//�s
			if (ListView_GetCheckState(hLIST, i) == 0) {//���`�F�b�N�̎�
				item.iSubItem = 2;//State
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 1;
				}
				item.iSubItem = 3;//Timer
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 2;
				}
				item.iSubItem = 4;//Type
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 3;
				}
				item.iSubItem = 5;//Ym 0.57a�Œǉ�
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 4;
				}

				//ID���擾
				ListView_GetItemText(hLIST, i, 0, szId, sizeof(szId) / sizeof(_TCHAR));
				nCheckedID = _tcstoul(szId, NULL, 16);
				//pastState[nCheckedID].state = 0;
				pastState[nCheckedID].state = ST_NONE;//0.60�ŕύX
				pastState[nCheckedID].time = 0;
				pastState[nCheckedID].timerid = 0;
				pastState[nCheckedID].sound = 0;
			}
		}
	}
	snPastChecked = nNowChecked;
	
	//�`�F�b�N���ꂽ���ڂɌ��݂�State����������
	for (int i = 0; i < nItemCount; i++) {
		if (ListView_GetCheckState(hLIST, i) != 0) {
			//ID���擾
			ListView_GetItemText(hLIST, i, 0, szId, sizeof(szId) / sizeof(_TCHAR));
			nCheckedID = _tcstoul(szId, NULL, 16);

			//0.53�Œǉ� �o�O���
			if (nCheckedID == 0 || nCheckedID >= MAX_SEARCH)continue;

			//�Ώۂ̏�Ԃ��擾
			target = GetNPCInfo(nCheckedID);

			//Sound�̔�r
			ListView_GetItemText(hLIST, i, 4, szType, sizeof(szType) / sizeof(_TCHAR));
			if (_tcscmp(szType, _T("EX")) == 0) {//��͒ʏ�AEX��EX�T�E���h //0.47�ŕύX
				nowState.sound = 1;
			}
			else if (_tcscmp(szType, _T("SP")) == 0) {
				nowState.sound = 2;
			}
			else nowState.sound = 0;

			item.pszText = _T("");//0.57�Œǉ�

								  //State����@�R�R�������������ꍇ��DrawNPC/List_AddState�����������鎖
			if (target.inrange != 0) {//�͈͓�
				if (NPC_CORPSE(target)) {//ST_CORPSE
					item.pszText = _T("Corpse");
					nowState.state = ST_CORPSE;
					if (pastState[nCheckedID].timerid != ID_TIMER_CORPSE) {
						pastState[nCheckedID].time = nowtime;
						pastState[nCheckedID].timerid = ID_TIMER_CORPSE;
					}
					if (target.x == 0 && target.y == 0 && target.z == 0) {//0.47b�ŕύX�@0.49�Ŗ߂�
																		  //	continue;//�S�~�f�[�^ 0.53�Œǉ�
					}
					else {
						//SOUND����p
						switch (pastState[nCheckedID].state) {
						case ST_WAR:
							uSoundType[nowState.sound] |= WAR_CORPSE;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s WAR->CORPSE code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);//0.53�Œǉ�
							break;
						case ST_POP:
							uSoundType[nowState.sound] |= POP_CORPSE;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s POP->CORPSE code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						case ST_CORPSE:
							break;
						case ST_OUT://0.60�ŕύX ����ɉ����钲��
							uSoundType[nowState.sound] |= OUT_CORPSE;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s OUT->CORPSE code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						default://0.60�ŕύX ����ɉ����钲��
							uSoundType[nowState.sound] |= OUT_CORPSE;
							_stprintf(szSendText, _T("%-24s NON->CORPSE code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						}
					}
				}
				else if (NPC_WAR(target)) {//ST_WAR
					item.pszText = _T("War");
					nowState.state = ST_WAR;
					if (target.hpp == 0 && target.viewAddress != 0) {//������̂�҂��Ă鎞
						pastState[nCheckedID].time = 0;//�^�C�}�[���J�E���g�����Ȃ�
						pastState[nCheckedID].timerid = ID_TIMER_WAIT_VIEW_NULL;//View��������̂�҂�
					}
					else if (pastState[nCheckedID].timerid != ID_TIMER_WAR) {
						pastState[nCheckedID].time = nowtime;
						pastState[nCheckedID].timerid = ID_TIMER_WAR;//ST_TIMER_WAR�̏���t�^
					}
					if (target.x == 0 && target.y == 0 && target.z == 0) {
						//	continue;//�S�~�f�[�^ 0.53�Œǉ�
					}
					else {
						//SOUND����p
						switch (pastState[nCheckedID].state) {
						case ST_WAR:
							break;
						case ST_POP:
							uSoundType[nowState.sound] |= POP_WAR;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s POP->WAR    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						case ST_CORPSE:
							uSoundType[nowState.sound] |= CORPSE_WAR;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s CORPSE->WAR code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						case ST_OUT://0.60�ŕύX
							uSoundType[nowState.sound] |= OUT_WAR;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s OUT->WAR    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						default://0.60�ŕύX
							uSoundType[nowState.sound] |= OUT_WAR;
							_stprintf(szSendText, _T("%-24s NON->WAR    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						}
					}
				}
				else if (NPC_POP(target)) {//ST_POP
					item.pszText = _T("Pop");
					nowState.state = ST_POP;
					pastState[nCheckedID].time = 0;
					pastState[nCheckedID].timerid = 0;
					if (target.x == 0 && target.y == 0 && target.z == 0) {
						//	continue;//�S�~�f�[�^ 0.53�Œǉ�
					}
					else {
						//SOUND����p
						switch (pastState[nCheckedID].state) {
						case ST_WAR:
							uSoundType[nowState.sound] |= WAR_POP;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s WAR->POP    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						case ST_POP:
							break;
						case ST_CORPSE:
							uSoundType[nowState.sound] |= CORPSE_POP;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s CORPSE->POP code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						case ST_OUT://0.60�ŕύX
							uSoundType[nowState.sound] |= OUT_POP;//0.49�ŕύX
							_stprintf(szSendText, _T("%-24s OUT->POP    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						default://0.60�ŕύX
							uSoundType[nowState.sound] |= OUT_POP;
							_stprintf(szSendText, _T("%-24s NOC->POP    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
							break;
						}
					}
				}
			}
			else {//�͈͊O�S�� ST_OUT
				item.pszText = _T("-");
				nowState.state = ST_OUT;
				if (pastState[nCheckedID].timerid == ID_TIMER_WAIT_VIEW_NULL) {//�ȑO�̏�Ԃ�View��������̂�҂��Ă��鎞
					pastState[nCheckedID].time = nowtime;
					pastState[nCheckedID].timerid = ID_TIMER_CORPSE;//�͈͊O�ɍs��������View�Ɉˑ����Ȃ��Ń^�C�}�[�N��
				}
				if (target.x == 0 && target.y == 0 && target.z == 0) {
					//	continue;//�S�~�f�[�^ 0.53�Œǉ�
				}
				else {
					//SOUND����p
					switch (pastState[nCheckedID].state) {
					case ST_WAR:
						uSoundType[nowState.sound] |= WAR_OUT;//0.49�ŕύX
						_stprintf(szSendText, _T("%-24s WAR->OUT    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
						break;
					case ST_POP:
						uSoundType[nowState.sound] |= POP_OUT;//0.49�ŕύX
						_stprintf(szSendText, _T("%-24s POP->OUT    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
						break;
					case ST_CORPSE:
						uSoundType[nowState.sound] |= CORPSE_OUT;//0.49�ŕύX
						_stprintf(szSendText, _T("%-24s CORPSE->OUT code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
						break;
					case ST_OUT://0.60�ŕύX
						break;
					default://0.60�ŕύX
						_stprintf(szSendText, _T("%-24s NON->OUT    code=%d id=%03X hpp=%02X inrange=%02X state=%02X\n"), target.name, target.code, target.id, target.hpp, target.inrange, target.state);
						break;
					}
				}
			}

			item.iSubItem = 2;//��State
			item.iItem = i;//�s
						   //��������
			ListView_GetItemText(hLIST, i, 2, szState, sizeof(szState) / sizeof(_TCHAR));
			if (_tcscmp(szState, item.pszText) != 0) {
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 5;
				}
			}

			//�ΏۂƂ̋�����\�� 0.57�Œǉ�
			item.pszText = szDistance;
			item.iSubItem = 5;//��Distance
			if (target.inrange) {
				float fDistance = (float)sqrt(target.distance_2);
				//if(NPC_CORPSE(target) || fDistance > 50.0){
				if (NPC_CORPSE(target) || fDistance > 60.0) {//0.57a�ŕύX
					_stprintf(szDistance, _T("-"));//ST_CORPSE
				}
				else if (fDistance >= 10.0)_stprintf(szDistance, _T("%.1f"), fDistance);
				else _stprintf(szDistance, _T("0%.1f"), fDistance);//�Ȃ���"%02.1f"�����p�o���Ȃ�����
			}
			else _stprintf(szDistance, _T("-"));
			if (FALSE == ListView_SetItem(hLIST, &item)) {
				return 8;
			}


			//���Ԃ�0����Ȃ���Timer�X�V
			if (pastState[nCheckedID].time != 0) {
				if (nowtime - pastState[nCheckedID].time == 0) {//0.42�Œǉ�00:00�̎��\�����Ȃ�
				}
				else {
					int tHour, tMin, tSec;
					tHour = (int)(nowtime - pastState[nCheckedID].time) / 60 / 60;
					tMin = (int)((nowtime - pastState[nCheckedID].time) / 60) % 60;
					tSec = (int)(nowtime - pastState[nCheckedID].time) % 60;
					if (tHour != 0) {
						_stprintf(szTime, _T("%d:%02d:%02d"), tHour, tMin, tSec);
					}
					else {
						_stprintf(szTime, _T("%02d:%02d"), tMin, tSec);
					}
					item.iSubItem = 3;//��Timer
					item.pszText = szTime;
					//��������
					if (FALSE == ListView_SetItem(hLIST, &item)) {
						return 6;
					}
				}
			}

			//State��Corpse/-��Pop�ɂȂ������A�^�C�}�[������
			if (g_ClearTime == 1 && nowState.state == ST_POP && (pastState[nCheckedID].state == ST_CORPSE || pastState[nCheckedID].state == ST_OUT)) {
				item.iSubItem = 3;//��Timer
				item.pszText = _T("");
				//��������
				if (FALSE == ListView_SetItem(hLIST, &item)) {
					return 7;
				}
			}

			if (hDebugWnd != NULL && wcslen(szSendText)) {//0.53�Œǉ�
				COPYDATASTRUCT copydata;
				ZeroMemory(&copydata, sizeof(COPYDATASTRUCT));
				copydata.lpData = szSendText;
				copydata.cbData = (wcslen(szSendText) + 1) * sizeof(_TCHAR);
				SendMessage(hDebugWnd, WM_COPYDATA, NULL, (LPARAM)&copydata);
				ZeroMemory(szSendText, sizeof(szSendText));
			}

			//�ߋ��̃X�e�[�g�ɃR�s�[
			pastState[nCheckedID].state = nowState.state;
		}
	}

	InvalidateRect(hLIST,0,TRUE);

	//0.49�ŕύX
	for (int i = 2; i >= 0; i--) {
		if ((uSoundType[i] & CORPSE_POP) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[0][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 50;
			}
			else break;
		}
		else if ((uSoundType[i] & CORPSE_OUT) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[1][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 51;
			}
			else break;
		}
		else if ((uSoundType[i] & CORPSE_WAR) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[2][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 52;
			}
			else break;
		}
		else if ((uSoundType[i] & WAR_POP) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[3][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 53;
			}
			else break;
		}
		else if ((uSoundType[i] & OUT_POP) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[4][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 54;
			}
			else break;
		}
		else if ((uSoundType[i] & OUT_CORPSE) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[5][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 55;
			}
			else break;
		}
		else if ((uSoundType[i] & OUT_WAR) && (g_Menu.popsound[i] || g_Menu.statesound[i])) {
			if (FALSE == PlaySound(g_WaveFileName[6][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 56;
			}
			else break;
		}
		else if ((uSoundType[i] & POP_WAR) && g_Menu.statesound[i]) {
			if (FALSE == PlaySound(g_WaveFileName[7][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 57;
			}
			else break;
		}
		else if ((uSoundType[i] & POP_OUT) && g_Menu.statesound[i]) {
			if (FALSE == PlaySound(g_WaveFileName[8][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 58;
			}
			else break;
		}
		else if ((uSoundType[i] & POP_CORPSE) && g_Menu.statesound[i]) {
			if (FALSE == PlaySound(g_WaveFileName[9][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 59;
			}
			else break;
		}
		else if ((uSoundType[i] & WAR_CORPSE) && g_Menu.statesound[i]) {
			if (FALSE == PlaySound(g_WaveFileName[10][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 60;
			}
			else break;
		}
		else if ((uSoundType[i] & WAR_OUT) && g_Menu.statesound[i]) {
			if (FALSE == PlaySound(g_WaveFileName[11][i], NULL, SND_FILENAME | SND_ASYNC)) {
				return 61;
			}
			else break;
		}
	}

	return 0;
}

//�t�@�C������NPCLIST��ǂݍ���
//iFlagAutoLoad��0�ȊO�̂Ƃ�AutoLoad.AreaXX�������œǍ���
int List_ReadNPC(HWND hWnd,HWND hList,int iFlagAutoLoad)
{
	_TCHAR szSection[0x10];
	_TCHAR szKeyName[0x20];
	_TCHAR szFullPathName[MAX_PATH];
	_TCHAR szFileName[0x80];
	_TCHAR szFilter[0x80];
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szCurrentDir[MAX_PATH];
	//�`�F�b�N���ꂽNPC�̐�
	int nSetCount = 0;
	//�J������
	int nItemCount;
	WORD AreaID;

	AreaID = GetAreaID();
	
	//�J�����g�t�H���_���擾
	GetCurrentDirectory(sizeof(szCurrentDir)/sizeof(_TCHAR),szCurrentDir);

	if (AreaID >= 0x0100) {
		//�Z�N�V���������쐬
		_stprintf(szSection, _T("Area%04X"), AreaID);
		//�t�B���^�[���쐬
		ZeroMemory(szFilter, sizeof(szFilter));
		if (g_Menu.list_control == 0) {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%04X"), AreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%04X File(*.Area%04X)_*.Area%04X_All (*.*)_*.*__"), AreaID, AreaID, AreaID);
		}
		else {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%04X-Name"), AreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%04X File(*.Area%04X-Name)_*.Area%04X-Name_All (*.*)_*.*__"), AreaID, AreaID, AreaID);
		}
	}
	else {
		//�Z�N�V���������쐬
		_stprintf(szSection, _T("Area%02X"), AreaID);
		//�t�B���^�[���쐬
		ZeroMemory(szFilter, sizeof(szFilter));
		if (g_Menu.list_control == 0) {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%02X"), AreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%02X File(*.Area%02X)_*.Area%02X_All (*.*)_*.*__"), AreaID, AreaID, AreaID);
		}
		else {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%02X-Name"), AreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%02X File(*.Area%02X-Name)_*.Area%02X-Name_All (*.*)_*.*__"), AreaID, AreaID, AreaID);
		}
	}

	for(int i=0;i<sizeof(szFilter);i++){
		if(szFilter[i] == _T('_')){
			szFilter[i] = _T('\0');
		}
	}
	
	if(iFlagAutoLoad == 0){
		//�_�C�A���O��\������
		OPENFILENAME ofn;
		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ZeroMemory(szFileName,sizeof(szFileName));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.lpstrInitialDir = g_NpcListPath;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = szFullPathName;
		ofn.nMaxFile = sizeof(szFullPathName)/sizeof(_TCHAR)-1;
		ofn.lpstrFileTitle = szFileName;
		ofn.nMaxFileTitle = sizeof(szFileName)/sizeof(_TCHAR)-1;
		ofn.Flags = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = szSection;

		if(GetOpenFileName(&ofn) == 0){//���s��
			//�J�����g�t�H���_��ݒ�
			SetCurrentDirectory(szCurrentDir);
			if(CommDlgExtendedError() != 0 ){//0�̎��̓L�����Z��
				return -4;
			}
			else{
				return 20;
			}
		}
	}
	else{
		if (AreaID >= 0x0100) {
			if (g_Menu.list_control == 0) {
				_stprintf(szFileName, _T("%s\\AutoLoad.Area%04X"), g_NpcListPath, AreaID);
			}
			else {
				_stprintf(szFileName, _T("%s\\AutoLoad.Area%04X-Name"), g_NpcListPath, AreaID);
			}
		}
		else {
			if (g_Menu.list_control == 0) {
				_stprintf(szFileName, _T("%s\\AutoLoad.Area%02X"), g_NpcListPath, AreaID);
			}
			else {
				_stprintf(szFileName, _T("%s\\AutoLoad.Area%02X-Name"), g_NpcListPath, AreaID);
			}
		}
		if(0 == GetFullPathName(szFileName,sizeof(szFullPathName)/sizeof(_TCHAR),szFullPathName,NULL)){
			return -5;
		}
	}

	//������
	//���X�g���N���A
	SendMessage(hWnd,WM_COMMAND,IDM_LIST_UPDATE,0);

	//�J�����g�t�H���_��ݒ�
	SetCurrentDirectory(szCurrentDir);

	//�J���������擾
	nItemCount = ListView_GetItemCount(hList);
	if(nItemCount == 0){
		return 10;
	}

	_TCHAR szName[0x20];
	_TCHAR szReplaceName[0x20];
	_TCHAR szID[0x10];
	_TCHAR szType[0x10];
	_TCHAR szNote[MAX_PATH];
	LVITEM item;
	LPARAM lParam=0;

	_TCHAR szReplaceFileName[0x80];
	_stprintf(szReplaceFileName, _T("%s\\ReplaceName.ini"), szCurrentDir);

	if (g_Menu.list_control == 0) {
		//�Ǘ�����:ID
		//���[�h���ꂽ�����Z�b�g
		nSetCount = GetPrivateProfileInt(szSection, _T("COUNT"), 0, szFullPathName);
		for (int i = 0, iCounter = 1; i < nSetCount; i++, iCounter++) {
			//�L�[�l�[�����쐬
			_stprintf(szKeyName, _T("SET%03d"), iCounter);
			GetPrivateProfileString(szSection, szKeyName, NULL, szID, sizeof(szID) / sizeof(_TCHAR), szFullPathName);
			_stprintf(szKeyName, _T("TYPE%03d"), iCounter);
			GetPrivateProfileString(szSection, szKeyName, NULL, szType, sizeof(szType) / sizeof(_TCHAR), szFullPathName);
			for (int j = 0; j < nItemCount; j++) {
				//NPCLIST���T�[�`
				ListView_GetItemText(hList, j, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
				if (_tcscmp(szID, szBuf) == 0) {
					ZeroMemory(&item, sizeof(LVITEM));
					item.mask = LVIF_TEXT;
					item.iItem = j;//�s
					item.iSubItem = 1;//��
					item.pszText = szName;//Name
					item.cchTextMax = sizeof(szName) / sizeof(_TCHAR); //pszText�����擾���̃o�b�t�@�T�C�Y
					ListView_GetItem(hList, &item);

					//���X�g����폜
					ListView_DeleteItem(hList, j);

					//��ɒǉ�
					ZeroMemory(&item, sizeof(LVITEM));
					item.mask = LVIF_TEXT | LVIF_PARAM;
					item.iItem = i;//�s
					item.iSubItem = 0;//��
					item.pszText = szID;
					item.lParam = i + 0x1000;//�\�[�g�plParam��ݒ�
					if (-1 == ListView_InsertItem(hList, &item)) {
						return -5;
					}

					item.mask = LVIF_TEXT;
					item.iSubItem = 1;//��

					//ReplaceName.ini�Œu�������閼�O������Βu��
					_stprintf(szKeyName, _T("%s"), szID);
					GetPrivateProfileString(szSection, szKeyName, NULL, szReplaceName, sizeof(szReplaceName) / sizeof(_TCHAR), szReplaceFileName);
					if (_tcscmp(szName, NULL) == 0) {
						item.pszText = szName;//Name���̂܂�
					}
					else {
						item.pszText = szReplaceName;//Name�u��
					}

					item.cchTextMax = 0; //��������܂�
					item.lParam = 0;
					if (FALSE == ListView_SetItem(hList, &item)) {
						return -6;
					}

					item.iSubItem = 4;//��
					item.pszText = szType;//Type
					if (FALSE == ListView_SetItem(hList, &item)) {
						return -7;
					}
					ListView_SetCheckState(hList, i, 1);//�Z�b�g
					break;
				}
			}
		}
		//Note��Ǎ���
		for (int i = 1; i <= nItemCount; i++) {
			_stprintf(szKeyName, _T("NOTE%03X"), i);
			GetPrivateProfileString(szSection, szKeyName, NULL, szNote, sizeof(szNote) / sizeof(_TCHAR), szFullPathName);
			if (_tcslen(szNote) != 0) {
				for (int j = 0; j < nItemCount; j++) {
					//NPCLIST���T�[�`
					ListView_GetItemText(hList, j, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
					if (i == _tcstoul(szBuf, 0, 16)) {
						ZeroMemory(&item, sizeof(LVITEM));
						item.mask = LVIF_TEXT;
						item.iItem = j;//�s
									   //item.iSubItem = 5;//��
						item.iSubItem = 6;//�� 0.57�ŕύX
						item.pszText = szNote;//Note
						if (FALSE == ListView_SetItem(hList, &item)) {
							return -8;
						}
						break;
					}
				}
			}
		}
	}
	else {
		//�Ǘ�����:Name
		//Type��Ǎ���
		for (int i = 1; i <= nItemCount; i++) {
			//�L�[�l�[�����쐬
			ZeroMemory(szKeyName, sizeof(szKeyName));
			//NPC�̖��O��NPCLIST����擾
			ListView_GetItemText(hList, i, 1, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
			_stprintf(szKeyName, _T("%s"), szBuf);

			GetPrivateProfileString(szSection, szKeyName, _T("Normal"), szType, sizeof(szType) / sizeof(_TCHAR), szFullPathName);

			if ((_tcscmp(szType, _T("Normal")) == 0) || (_tcscmp(szType, _T("Ex")) == 0) || (_tcscmp(szType, _T("Sp")) == 0)) {
				ZeroMemory(&item, sizeof(LVITEM)); 
				item.iItem = i;//�s
				item.mask = LVIF_TEXT;
				item.iSubItem = 4;//��:Type
				if (_tcscmp(szType, _T("Normal")) == 0) {
					item.pszText = _T("");//Normal
				}
				else{
					//Ex,Sp�̎�
					item.pszText = szType;//Type
				}

				if (FALSE == ListView_SetItem(hList, &item)) {
					return -9;
				}
				ListView_SetCheckState(hList, i, 1);//�Z�b�g
			}
		}
	}

	return 1;
}


//NPCLIST���t�@�C���֏�������
int List_WriteNPC(HWND hWnd,HWND hList)
{
	_TCHAR szSection[0x10];
	_TCHAR szKeyName[0x20];
	_TCHAR szFullPathName[MAX_PATH];
	_TCHAR szFileName[0x80];
	_TCHAR szFilter[0x80];
	_TCHAR szBuf[MAX_PATH];
	_TCHAR szCurrentDir[MAX_PATH];
	//�`�F�b�N���ꂽNPC�̐�
	int nSetCount = 0;
	//�J������
	int nItemCount;

	//�G���A��0x00�̎�
	if(g_ListAreaID == 0){
		return -1;
	}

	//�J���������擾
	nItemCount = ListView_GetItemCount(hList);
	if(nItemCount == 0){
		return -2;
	}
	//�`�F�b�N���ꂽNPC�̐����擾
	for (int i = 0; i < nItemCount; i++) {
		if (ListView_GetCheckState(hList, i) != 0) {
			nSetCount++;
		}
	}

	//�J�����g�t�H���_���擾
	GetCurrentDirectory(sizeof(szCurrentDir) / sizeof(_TCHAR), szCurrentDir);

	if (g_ListAreaID >= 0x0100) {
		//�Z�N�V���������쐬
		_stprintf(szSection, _T("Area%04X"), g_ListAreaID);
		//�t�B���^�[���쐬

		ZeroMemory(szFilter, sizeof(szFilter));
		if (g_Menu.list_control == 0) {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%04X-Name"), g_ListAreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%04X File(*.Area%04X)_*.Area%04X_All (*.*)_*.*__"), g_ListAreaID, g_ListAreaID, g_ListAreaID);
		}
		else {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%04X"), g_ListAreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%04X File(*.Area%04X-Name)_*.Area%04X-Name_All (*.*)_*.*__"), g_ListAreaID, g_ListAreaID, g_ListAreaID);
		}
	}
	else {
		//�Z�N�V���������쐬
		_stprintf(szSection, _T("Area%02X"), g_ListAreaID);
		//�t�B���^�[���쐬
		ZeroMemory(szFilter, sizeof(szFilter));
		if (g_Menu.list_control == 0) {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%02X"), g_ListAreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%02X File(*.Area%02X)_*.Area%02X_All (*.*)_*.*__"), g_ListAreaID, g_ListAreaID, g_ListAreaID);
		}
		else {
			//�t�@�C���l�[���쐬
			_stprintf(szFullPathName, _T("AutoLoad.Area%02X-Name"), g_ListAreaID);
			//\0��_�ō쐬���Ă��邽�߁A��Œu��
			_stprintf(szFilter, _T("Area%02X File(*.Area%02X-Name)_*.Area%02X-Name_All (*.*)_*.*__"), g_ListAreaID, g_ListAreaID, g_ListAreaID);
		}
	}
	for (int i = 0; i < sizeof(szFilter); i++) {
		if (szFilter[i] == _T('_')) {
			szFilter[i] = _T('\0');
		}
	}

	//�t�@�C�������쐬
	//�_�C�A���O��\������
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ZeroMemory(szFileName,sizeof(szFileName));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrInitialDir = g_NpcListPath;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFullPathName;
	ofn.nMaxFile = sizeof(szFullPathName)/sizeof(_TCHAR)-1;
	ofn.lpstrFileTitle = szFileName;
	ofn.nMaxFileTitle = sizeof(szFileName)/sizeof(_TCHAR)-1;
	ofn.Flags = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = szSection;

	if(GetSaveFileName(&ofn) == 0){//���s��
		//�J�����g�t�H���_��ݒ�
		SetCurrentDirectory(szCurrentDir);
		if(CommDlgExtendedError() != 0 ){//0�̎��̓L�����Z��
			return -4;
		}
		else{
			return 20;
		}
	}
	else{//������
		//�J�����g�t�H���_��ݒ�
		SetCurrentDirectory(szCurrentDir);

		//�S�~���c��̂Ńt�@�C�����폜
		DeleteFile(szFullPathName);

		if (g_Menu.list_control == 0) {
			//�Ǘ�����:ID
			//�`�F�b�N���ꂽNPC�̐�����������
			_stprintf(szBuf, _T("%d"), nSetCount);
			WritePrivateProfileString(szSection, _T("COUNT"), szBuf, szFullPathName);

			//�`�F�b�N���ꂽNPC���t�@�C���ɏ�������
			for (int i = 0, iCounter = 1; i < nItemCount; i++) {
				if (ListView_GetCheckState(hList, i) != 0) {
					//�L�[�l�[�����쐬
					ZeroMemory(szKeyName, sizeof(szKeyName));
					_stprintf(szKeyName, _T("SET%03d"), iCounter);
					//NPC��ID��NPCLIST����擾
					ListView_GetItemText(hList, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
					WritePrivateProfileString(szSection, szKeyName, szBuf, szFullPathName);
					//TYPE EX
					ListView_GetItemText(hList, i, 4, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
					if (_tcslen(szBuf) != 0) {
						_stprintf(szKeyName, _T("TYPE%03d"), iCounter);
						WritePrivateProfileString(szSection, szKeyName, szBuf, szFullPathName);
					}
					iCounter++;
				}
			}

			//Note����������
			for (int i = 0; i < nItemCount; i++) {
				//�L�[�l�[�����쐬
				ZeroMemory(szKeyName, sizeof(szKeyName));
				ListView_GetItemText(hList, i, 0, szBuf, sizeof(szBuf) / sizeof(_TCHAR));//NPC��ID��NPCLIST����擾
				_stprintf(szKeyName, _T("NOTE%s"), szBuf);
				//NOTE
				ListView_GetItemText(hList, i, 6, szBuf, sizeof(szBuf) / sizeof(_TCHAR));//0.57�ŕύX
				if (_tcslen(szBuf) != 0) {
					WritePrivateProfileString(szSection, szKeyName, szBuf, szFullPathName);
				}
			}
		}
		else {
			//�Ǘ�����:Name
			//�`�F�b�N���ꂽNPC���t�@�C���ɏ�������
			for (int i = 0; i < nItemCount; i++) {
				//�L�[�l�[�����쐬
				ZeroMemory(szKeyName, sizeof(szKeyName));
				//NPC�̖��O��NPCLIST����擾
				ListView_GetItemText(hList, i, 1, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
				_stprintf(szKeyName, _T("%s"), szBuf);
				//TYPE EX
				ListView_GetItemText(hList, i, 4, szBuf, sizeof(szBuf) / sizeof(_TCHAR));
				if (_tcslen(szBuf) != 0) {
					WritePrivateProfileString(szSection, szKeyName, szBuf, szFullPathName);
				}
				else {
					WritePrivateProfileString(szSection, szKeyName, _T("Normal"), szFullPathName);
				}

			}
		}
	}

	return 1;
}

