#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include "pol.h"


//int CLASS_ITEM::_Decrypt(BYTE* byData)
//�A�C�e�����𕜍���
//byData�ňÍ����ꂽ�f�[�^����byData�ɕ����������f�[�^��Ԃ�
//�������̃T�C�Y��SIZEOF_ITEM_STRUCT
//�߂�l�F����1�A���s-1�ȉ�
int CLASS_ITEM::_Decrypt(BYTE* byData)
{
	for (int i = 0; i<SIZEOF_ITEM_STRUCT; i++) {
		BYTE byBuff = *byData;
		WORD wDecrypt;
		wDecrypt = (WORD)((byBuff >> 4) & 0x0F);
		wDecrypt |= (WORD)((byBuff << 4) & 0xF0);
		wDecrypt |= (WORD)((wDecrypt & 0x01) << 8);
		wDecrypt /= 2;
		*byData = (BYTE)wDecrypt;
		byData++;
	}

	return 1;
}


//int CLASS_ITEM;;IsItemDat()
//�t�@�C�����A�C�e��DAT���ǂ����`�F�b�N
//�Ԃ�l�F��1�A��0�ȉ�
int CLASS_ITEM::IsItemDat()
{
	if (this->_hFile == INVALID_HANDLE_VALUE)return -1;
	if (PathFileExists(this->_sztDatPath) == false)return -2;
	if (this->_nFileSize <= 0)return -3;
	if (this->_nFileSize % SIZEOF_ITEM_STRUCT != 0)return -4;

	return 1;
}


//int CLASS_ITEM::Get(int)
//�t�@�C���������Ǎ���
//�Ԃ�l�F����1�A����0�A���s-1�ȉ�
int CLASS_ITEM::Get(int nPos)
{
	BYTE byData[SIZEOF_ITEM_STRUCT];//�Ǎ��݃f�[�^
	DWORD dwReadSize;//�Ǎ��݃T�C�Y
	short shFlags;//ExRa���̃t���O
	int nStringCount;//�e�L�X�g�̐�
	int nStringOffset;//������̃I�t�Z�b�g
	int nStringLen;//������̒���

	this->Clear();

	if (SetFilePointer(this->_hFile, nPos * SIZEOF_ITEM_STRUCT, NULL, FILE_BEGIN) == 0xFFFFFFFF)return -1;
	if (ReadFile(this->_hFile, byData, sizeof(byData), &dwReadSize, NULL) == false)return -2;
	if (dwReadSize != SIZEOF_ITEM_STRUCT)return -3;

	_Decrypt(byData);

	//�R�R����f�[�^�𐶐�
	memcpy(&(this->m_ID), &byData[0x00], 4);
	memcpy(&shFlags, &byData[0x04], 2);//ItemFlags
	if ((shFlags & 0x8000) == 0x8000)this->m_Rare = 1;
	if ((shFlags & 0x6040) == 0x6040)this->m_Ex = 1;
	memcpy(&(this->m_StackSize), &byData[0x06], 2);
	memcpy(&(this->m_Type), &byData[0x08], 2);//��� 1.3�Œǉ�
	memcpy(&(this->m_ResourceID), &byData[0x0A], 2);
	if (this->m_nItemType == LPARAM_WEAPON) {//���� 1.3�ŕύX
		memcpy(&(this->m_Level), &byData[0x0E], 2);//1.3�ňړ�
		memcpy(&(this->m_Slot), &byData[0x10], 2);//
		memcpy(&(this->m_Race), &byData[0x12], 2);//
		memcpy(&(this->m_Job), &byData[0x14], 4);//
		memcpy(&(this->m_Damage), &byData[0x18], 2);
		memcpy(&(this->m_Delay), &byData[0x1A], 2);
		memcpy(&(this->m_Skill), &byData[0x1E], 2);
		memcpy(&nStringCount, &byData[0x30], 4);
		nStringOffset = 0x30 + byData[0x34];
	}
	else if (this->m_nItemType == LPARAM_ARMOR) {//�h�� 1.3�ŕύX
		memcpy(&(this->m_Level), &byData[0x0E], 2);//1.3�ňړ�
		memcpy(&(this->m_Slot), &byData[0x10], 2);//
		memcpy(&(this->m_Race), &byData[0x12], 2);//
		memcpy(&(this->m_Job), &byData[0x14], 4);//
		memcpy(&(this->m_Shild), &byData[0x18], 2);
		memcpy(&nStringCount, &byData[0x28], 4);
		nStringOffset = 0x28 + byData[0x2C];
	}
	else if (this->m_nItemType == LPARAM_ITEM) {//1.3�Œǉ�
		this->m_Element = 0xFFFF;
		this->m_StorageSlots = 0;
		//memcpy(&nStringCount,&byData[0x14],4);
		memcpy(&nStringCount, &byData[0x18], 4);//2013.10.08VU�ŕύX
												//nStringOffset = 0x14 + byData[0x18];
		nStringOffset = 0x18 + byData[0x1C];//2013.10.08VU�ŕύX

		switch (this->m_Type) {
		case 0x000A://Furnishing
		case 0x000C://Flowerpot
		case 0x000E://Mannequin
			memcpy(&(this->m_Element), &byData[0x0E], 2);
			memcpy(&(this->m_StorageSlots), &byData[0x10], 4);
			break;
		case 0x0016://MazeTabula_M
		case 0x0017://MazeTabula_R
		case 0x0018://MazeVoucher
		case 0x0019://MazeRune
		case 0x001B://StorageSlip
			memcpy(&nStringCount, &byData[0x54], 4);
			nStringOffset = 0x54 + byData[0x58];
			break;
		default:
			break;
		}
	}
	else {
		return -4;
	}
	if (nStringCount > 9) {//sanity check,2=ja,5=en,6=fr,9=de
		return -5;
	}
	if (byData[nStringOffset] != 1) {
		return -6;
	}
	else {
		nStringOffset += 0x1C;
	}

	switch (nStringCount) {
	case 2://ja
		wsprintfA(this->m_szcName1, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName1);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -10;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcText, "%s", &byData[nStringOffset]);
		break;
	case 5://en
		wsprintfA(this->m_szcName1, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName1);
		nStringOffset += nStringLen + (8 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -11;
		}
		else {
			if (strlen(m_szcName1) == 1)break;
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName2, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName2);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -12;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName3, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName3);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -13;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcText, "%s", &byData[nStringOffset]);
		break;
	case 6://fr
		wsprintfA(this->m_szcName1, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName1);
		nStringOffset += nStringLen + (12 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -14;
		}
		else {
			if (strlen(m_szcName1) == 1)break;
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName2, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName2);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -15;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName3, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName3);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -16;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcText, "%s", &byData[nStringOffset]);
		break;
	case 9://de
		wsprintfA(this->m_szcName1, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName1);
		nStringOffset += nStringLen + (16 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -17;
		}
		else {
			if (strlen(m_szcName1) == 1)break;
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName2, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName2);
		nStringOffset += nStringLen + (12 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -18;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcName3, "%s", &byData[nStringOffset]);
		nStringLen = strlen(m_szcName3);
		nStringOffset += nStringLen + (4 - nStringLen % 4);
		if (byData[nStringOffset] != 1) {
			return -19;
		}
		else {
			nStringOffset += 0x1C;
		}
		wsprintfA(this->m_szcText, "%s", &byData[nStringOffset]);
		break;
	default:
		return -20;
	}

	return 1;
}



//int CLASS_PATH::ReadIni()
//Ini��Ǎ���
//�Ԃ�l�F����1�A���s0�ȉ�
int CLASS_PATH::GetPath()
{
	_TCHAR sztBuff[MAX_PATH];
	_TCHAR sztKeyName[0x20];
	HKEY hKey;

	if (_tcslen(sztIni) <= 5) {
		return -1;
	}

	//FF11���C���X�g�[������Ă���t�H���_���擾
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PlayOnline\\InstallFolder"), 0, KEY_READ, &hKey)) {
		DWORD dwType = REG_SZ;
		DWORD cbData = MAX_PATH;
		RegQueryValueEx(hKey, _T("0001"), NULL, &dwType, (LPBYTE)this->Client[0].sztPol, &cbData);
		cbData = MAX_PATH;
		RegQueryValueEx(hKey, _T("0015"), NULL, &dwType, (LPBYTE)this->Client[1].sztPol, &cbData);
		RegCloseKey(hKey);
	}


	for (int i = 0; i<MAX_KEY_NUM; i++) {
		wsprintf(sztKeyName, _T("WEAPON%d"), i + 1);
		GetPrivateProfileString(_T("DAT_JA"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztWeapon[i].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_JA_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztWeapon[i].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztWeapon[i].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztWeapon[i].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztWeapon[i].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztWeapon[i].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztWeapon[i].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztWeapon[i].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);

		wsprintf(sztKeyName, _T("ARMOR%d"), i + 1);
		GetPrivateProfileString(_T("DAT_JA"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztArmor[i].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_JA_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztArmor[i].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztArmor[i].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztArmor[i].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztArmor[i].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztArmor[i].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztArmor[i].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztArmor[i].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);

		//1.3�Œǉ�
		wsprintf(sztKeyName, _T("ITEM%d"), i + 1);
		GetPrivateProfileString(_T("DAT_JA"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztItem[i].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_JA_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztItem[i].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztItem[i].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_EN_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztItem[i].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztItem[i].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_FR_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztItem[i].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[0].sztItem[i].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
		GetPrivateProfileString(_T("DAT_DE_TC"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		if (_tcslen(sztBuff))wsprintf(this->Client[1].sztItem[i].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	}

	GetPrivateProfileString(_T("DAT_JA"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[0].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_JA_TC"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[0].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[0].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN_TC"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[0].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[0].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR_TC"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[0].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[0].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE_TC"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[0].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);

	GetPrivateProfileString(_T("DAT_JA"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[1].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_JA_TC"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[1].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[1].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN_TC"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[1].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[1].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR_TC"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[1].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[1].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE_TC"), _T("MAGIC_TEXT"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[1].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);

	GetPrivateProfileString(_T("DAT_JA"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[2].ja, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_JA_TC"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[2].ja, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[2].en, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_EN_TC"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[2].en, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[2].fr, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_FR_TC"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[2].fr, _T("%s%s"), this->Client[1].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE"), _T("MAGIC_DETAILS"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[0].sztMagic[2].de, _T("%s%s"), this->Client[0].sztPol, sztBuff);
	GetPrivateProfileString(_T("DAT_DE_TC"), _T("MAGIC_NAME"), NULL, sztBuff, sizeof(sztBuff), this->sztIni);
	if (_tcslen(sztBuff))wsprintf(this->Client[1].sztMagic[2].de, _T("%s%s"), this->Client[1].sztPol, sztBuff);

	ZeroMemory(&(this->m_sztJobs[0][0]), sizeof(this->m_sztJobs));
	for (int i = 0; i<24; i++) {
		wsprintf(sztKeyName, _T("%02d"), i);
		GetPrivateProfileString(_T("JOB"), sztKeyName, NULL, sztBuff, sizeof(sztBuff), this->sztIni);
		_tcsncpy_s(this->m_sztJobs[i], sztBuff, sizeof(this->m_sztJobs) / ARRAYSIZE(this->m_sztJobs) - 1);
	}

	return 1;
}


//int CLASS_TREE::Init(HWND,CLASS_PATH)
//������
//�Ԃ�l�F����1�A���s0�ȉ�
int CLASS_TREE::Init(HWND hTreeWnd, CLASS_PATH* path)
{
	TVINSERTSTRUCT tvis;
	HTREEITEM hTreeItem_Root;
	HTREEITEM hTreeItem_Client;
	HTREEITEM hTreeItem_Lang;

	InitCommonControls();

	const _TCHAR sztClient[][0x10] = { _T("MainClient"),_T("TestClient") };
	const _TCHAR sztLang[][0x10] = { _T("���{��"),_T("English"),_T("French"),_T("German") };
	_TCHAR sztKey[0x100];

	this->_hWnd = hTreeWnd;

	ZeroMemory(&tvis, sizeof(tvis));
	hTreeItem_Root = TreeView_GetRoot(this->_hWnd);

	for (int nClient = 0; nClient<ARRAYSIZE(sztClient); nClient++) {
		if (_tcslen(path->Client[nClient].sztPol) == 0) {//�N���C�A���g�����݂��Ȃ��ꍇ
			continue;
		}
		tvis.hParent = hTreeItem_Root;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT;
		tvis.item.pszText = (_TCHAR*)sztClient[nClient];
		hTreeItem_Client = TreeView_InsertItem(this->_hWnd, &tvis);

		for (int nLang = 0; nLang<ARRAYSIZE(sztLang); nLang++) {
			tvis.hParent = hTreeItem_Client;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.mask = TVIF_TEXT;
			tvis.item.pszText = (_TCHAR*)sztLang[nLang];
			hTreeItem_Lang = TreeView_InsertItem(this->_hWnd, &tvis);

			for (int nKey = 0; nKey<MAX_KEY_NUM; nKey++) {
				if ((nLang == 0 && _tcslen(path->Client[nClient].sztWeapon[nKey].ja))
					|| (nLang == 1 && _tcslen(path->Client[nClient].sztWeapon[nKey].en))
					|| (nLang == 2 && _tcslen(path->Client[nClient].sztWeapon[nKey].fr))
					|| (nLang == 3 && _tcslen(path->Client[nClient].sztWeapon[nKey].de))
					) {
					wsprintf(sztKey, nKey == 0 ? _T("Weapon") : _T("Weapon%d"), nKey + 1);
					tvis.hParent = hTreeItem_Lang;
					tvis.hInsertAfter = TVI_LAST;
					tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
					tvis.item.pszText = sztKey;
					tvis.item.lParam = LPARAM_WEAPON | (nKey * 0x100) | (nLang * 0x1000000) | (nClient * 0x10000000);
					TreeView_InsertItem(this->_hWnd, &tvis);
				}
			}

			for (int nKey = 0; nKey<MAX_KEY_NUM; nKey++) {
				if ((nLang == 0 && _tcslen(path->Client[nClient].sztArmor[nKey].ja))
					|| (nLang == 1 && _tcslen(path->Client[nClient].sztArmor[nKey].en))
					|| (nLang == 2 && _tcslen(path->Client[nClient].sztArmor[nKey].fr))
					|| (nLang == 3 && _tcslen(path->Client[nClient].sztArmor[nKey].de))
					) {
					wsprintf(sztKey, nKey == 0 ? _T("Armor") : _T("Armor%d"), nKey + 1);
					tvis.hParent = hTreeItem_Lang;
					tvis.hInsertAfter = TVI_LAST;
					tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
					tvis.item.pszText = sztKey;
					tvis.item.lParam = LPARAM_ARMOR | (nKey * 0x100) | (nLang * 0x1000000) | (nClient * 0x10000000);
					TreeView_InsertItem(this->_hWnd, &tvis);
				}
			}

			//MAGIC
			if ((nLang == 0 && _tcslen(path->Client[nClient].sztMagic[0].ja))
				|| (nLang == 1 && _tcslen(path->Client[nClient].sztMagic[0].en))
				|| (nLang == 2 && _tcslen(path->Client[nClient].sztMagic[0].fr))
				|| (nLang == 3 && _tcslen(path->Client[nClient].sztMagic[0].de))
				) {
				tvis.hParent = hTreeItem_Lang;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
				tvis.item.pszText = (_TCHAR*)_T("Magic");
				tvis.item.lParam = LPARAM_MAGIC | (nLang * 0x1000000) | (nClient * 0x10000000);
				TreeView_InsertItem(this->_hWnd, &tvis);
			}

			//Item 1.3�Œǉ�
			for (int nKey = 0; nKey<MAX_KEY_NUM; nKey++) {
				if ((nLang == 0 && _tcslen(path->Client[nClient].sztItem[nKey].ja))
					|| (nLang == 1 && _tcslen(path->Client[nClient].sztItem[nKey].en))
					|| (nLang == 2 && _tcslen(path->Client[nClient].sztItem[nKey].fr))
					|| (nLang == 3 && _tcslen(path->Client[nClient].sztItem[nKey].de))
					) {
					wsprintf(sztKey, nKey == 0 ? _T("Item") : _T("Item%d"), nKey + 1);
					tvis.hParent = hTreeItem_Lang;
					tvis.hInsertAfter = TVI_LAST;
					tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
					tvis.item.pszText = sztKey;
					tvis.item.lParam = LPARAM_ITEM | (nKey * 0x100) | (nLang * 0x1000000) | (nClient * 0x10000000);
					TreeView_InsertItem(this->_hWnd, &tvis);
				}
			}
		}

		TreeView_Expand(this->_hWnd, hTreeItem_Client, TVE_EXPAND);
	}

	return 1;
}


//int CLASS_LIST::Init()
int CLASS_LIST::Init(HWND hListWnd)
{
	this->_hWnd = hListWnd;

	return 1;
}


//int CLASS_LIST::Clear()
int CLASS_LIST::Clear()
{
	int nCount = Header_GetItemCount(ListView_GetHeader(this->_hWnd));
	for (int i = 0; i<nCount; i++)ListView_DeleteColumn(this->_hWnd, 0);
	ListView_DeleteAllItems(this->_hWnd);
	return 1;
}


//int CLAS_LIST::Add(CLASS_PATH,CLASS_ITEM*)
//�A�C�e�������X�g�ɒǉ�
//�Ԃ�l�F����1�A���s0�ȉ�
int CLASS_LIST::Add(CLASS_PATH* path, CLASS_ITEM* item)
{
	LVITEM lvi;
	_TCHAR sztBuff[MAX_PATH];

	//ID,���\�[�XID,Name1,Name2,Name3,���,�푰����,�W���u,D�l/���^�C�v,�Ԋu,���x��,�X�^�b�N��,���A,EX,������
	const _TCHAR sztColumn[][0x18] = { _T("ID"),_T("RID"),_T("���O"),_T("���O2"),_T("���O3"),_T("���"),_T("�푰����"),_T("�W���u"),_T("D�l/���^�C�v"),_T("�Ԋu"),_T("Lv"),_T("�X�^�b�N��"),_T("Ra"),_T("Ex"),_T("Text"), };
	const int nColumnWidth[] = { 40,0,100,0,0,50,0,100,0,0,30,0,40,40,350 };
	LVCOLUMN col;

	ListView_SetExtendedListViewStyleEx(this->_hWnd,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
	);

	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	for (int nCol = 0; nCol<ARRAYSIZE(sztColumn); nCol++) {
		col.cx = nColumnWidth[nCol];
		col.pszText = (_TCHAR*)sztColumn[nCol];
		col.iSubItem = nCol;
		ListView_InsertColumn(this->_hWnd, nCol, &col);
	}

	for (int nPos = 0;; nPos++) {
		int nRet = item->Get(nPos);
		if (nRet == 0)break;//����
		else if (nRet < 0)return -1;

		//ID
		wsprintf(sztBuff, _T("%d"), item->m_ID);
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 0;//��
		lvi.pszText = sztBuff;
		lvi.lParam = nPos + 0x1000;//�\�[�g�plParam��ݒ�
		if (-1 == ListView_InsertItem(this->_hWnd, &lvi)) {
			return -10;
		}

		if (item->m_Job > 0) {
			//RID
			wsprintf(sztBuff, _T("%d"), item->m_ResourceID);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -11;
			}

			//Name1
#ifdef UNICODE
			mbstowcs_s(NULL, sztBuff, item->m_szcName1, _TRUNCATE);
#else
			_tcscpy_s(sztBuff, item->m_szcName1);
#endif
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 2;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -12;
			}

			//Name2
			if (strlen(item->m_szcName2)) {
#ifdef UNICODE
				mbstowcs_s(NULL, sztBuff, item->m_szcName2, _TRUNCATE);
#else
				_tcscpy_s(sztBuff, item->m_szcName2);
#endif
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 3;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -13;
				}
			}

			//Name3
			if (strlen(item->m_szcName3)) {
#ifdef UNICODE
				mbstowcs_s(NULL, sztBuff, item->m_szcName3, _TRUNCATE);
#else
				_tcscpy_s(sztBuff, item->m_szcName3);
#endif
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 4;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -14;
				}
			}

			//���
			ZeroMemory(sztBuff, sizeof(sztBuff));
			const _TCHAR szcSkill[0x100][0x10] = { \
				_T("None"),_T("�i��"),_T("�Z��"),_T("�Ў茕"),_T("���茕"),_T("�Ў蕀"),_T("���蕀"),_T("���芙"),_T("���葄"),_T("�Ў蓁"),_T("���蓁"),_T("�Ў螞"),_T("���螞"),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("�|�p"),_T("�ˌ�"),_T("���Ă�"),_T(""),_T(""),_T(""),_T(""),\
				_T("�_��"),_T("��"),_T("����"),_T("���"),_T("����"),_T("�Í�"),_T("����"),_T("�E�p"),_T("�̏�"),_T("���y��"),_T("�Ǌy��"),_T("��"),_T(""),_T(""),_T(""),_T(""),\
				_T("�ނ�"),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
				_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("(��E�e)"),\
			};
			if (item->m_nItemType == LPARAM_WEAPON) {//���� 1.3�ŕύX
				if (item->m_Skill != 0) {
					_tcscpy_s(sztBuff, szcSkill[item->m_Skill]);
				}
				else {
					if (item->m_Slot & 0x0001)_tcscpy_s(sztBuff, _T("�s��"));//�O���b�v�l��
					if (item->m_Slot & 0x0002)_tcscpy_s(sztBuff, _T("��E�e"));//�O���b�v(�ӂ��݊��ׁ̈u��E�e�v)
					if (item->m_Slot & 0x0004)_tcscpy_s(sztBuff, _T("��E�e"));//�X�g�����K�[(�ӂ��݊��ׁ̈u��E�e�v)
					if (item->m_Slot & 0x0008)_tcscpy_s(sztBuff, _T("��E�e"));
				}
			}
			else if (item->m_nItemType == LPARAM_ARMOR) {//�h�� 1.3�ŕύX
				if (item->m_Slot & 0x0002)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x0008)_tcscpy_s(sztBuff, _T("��E�e"));
				if (item->m_Slot & 0x0010)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x0020)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x0040)_tcscpy_s(sztBuff, _T("����"));
				if (item->m_Slot & 0x0080)_tcscpy_s(sztBuff, _T("���r"));
				if (item->m_Slot & 0x0100)_tcscpy_s(sztBuff, _T("����"));
				if (item->m_Slot & 0x0200)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x0400)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x8000)_tcscpy_s(sztBuff, _T("�w"));
				if (item->m_Slot & 0x1800)_tcscpy_s(sztBuff, _T("��"));
				if (item->m_Slot & 0x6000)_tcscpy_s(sztBuff, _T("�w"));
			}
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 5;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -15;
			}

			//�푰
			ZeroMemory(sztBuff, sizeof(sztBuff));
			if (item->m_Race == 0x01FE)_tcscpy_s(sztBuff, _T("�S��"));
			else if (item->m_Race == 0x012A)_tcscpy_s(sztBuff, _T("��"));
			else if (item->m_Race == 0x00D4)_tcscpy_s(sztBuff, _T("��"));
			else if (item->m_Race == 0x0006)_tcscpy_s(sztBuff, _T("�g"));
			else if (item->m_Race == 0x0002)_tcscpy_s(sztBuff, _T("�g��"));
			else if (item->m_Race == 0x0004)_tcscpy_s(sztBuff, _T("�g��"));
			else if (item->m_Race == 0x0018)_tcscpy_s(sztBuff, _T("�d"));
			else if (item->m_Race == 0x0008)_tcscpy_s(sztBuff, _T("�d��"));
			else if (item->m_Race == 0x0010)_tcscpy_s(sztBuff, _T("�d��"));
			else if (item->m_Race == 0x0060)_tcscpy_s(sztBuff, _T("�s"));
			else if (item->m_Race == 0x0020)_tcscpy_s(sztBuff, _T("�s��"));
			else if (item->m_Race == 0x0040)_tcscpy_s(sztBuff, _T("�s��"));
			else if (item->m_Race == 0x0080)_tcscpy_s(sztBuff, _T("�l"));
			else if (item->m_Race == 0x0100)_tcscpy_s(sztBuff, _T("�f"));
			else _tcscpy_s(sztBuff, _T("�s��"));
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 6;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -16;
			}

			//�W���u
			ZeroMemory(sztBuff, sizeof(sztBuff));
			for (int nJob = 1; nJob<24; nJob++) {
				if (item->m_Job & (0x00000001 << nJob))_tcscat_s(sztBuff, path->m_sztJobs[nJob]);
			}
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 7;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -17;
			}

			//D�l/���^�C�v
			if (item->m_nItemType == LPARAM_WEAPON) {//���� 1.3�ŕύX
				wsprintf(sztBuff, _T("%d"), item->m_Damage);
			}
			else if (item->m_nItemType == LPARAM_ARMOR) {//�h�� 1.3�ŕύX
				wsprintf(sztBuff, _T("%d"), item->m_Shild);
			}
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 8;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -18;
			}

			//�Ԋu
			if (item->m_nItemType == LPARAM_WEAPON) {//���� 1.3�ŕύX
				wsprintf(sztBuff, _T("%d"), item->m_Delay);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 9;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -19;
				}
			}

			//���x��
			wsprintf(sztBuff, _T("%d"), item->m_Level);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 10;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -20;
			}

			//�X�^�b�N��
			if (item->m_nItemType == LPARAM_WEAPON) {//����@1.3�ŕύX
				wsprintf(sztBuff, _T("%d"), item->m_StackSize);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 11;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -21;
				}
			}

			//Rare
			if (item->m_Rare) {
				_tcscpy_s(sztBuff, _T("Rare"));
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 12;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -22;
				}
			}

			//Ex
			if (item->m_Ex) {
				_tcscpy_s(sztBuff, _T("Ex"));
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 13;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -23;
				}
			}

			//Text
			if (strlen(item->m_szcText)) {
				//0x0A��'/'�ɒu��
				for (int i = 0; i<(int)strlen(item->m_szcText); i++) {
					if (item->m_szcText[i] == 0x0A) {
						item->m_szcText[i] = '/';
					}
				}
#ifdef UNICODE
				errno_t err = mbstowcs_s(NULL, sztBuff, item->m_szcText, _TRUNCATE);
				//���s���鎖������̂ŏo�����MBS�ŃR���p�C��
#else
				_tcscpy_s(sztBuff, item->m_szcText);
#endif
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 14;//��
				lvi.pszText = sztBuff;
				if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
					return -24;
				}
			}
		}

	}

	return 1;
}


//int CLAS_LIST::Add(CLASS_PATH*,CLASS_MAGIC*)
//���@�����X�g�ɒǉ�
//�Ԃ�l�F����1�A���s0�ȉ�
int CLASS_LIST::Add(CLASS_PATH* path, CLASS_MAGIC* magic)
{
	LVITEM lvi;
	_TCHAR sztBuff[MAX_PATH];

#ifdef DEBUG
	const _TCHAR sztColumn[][0x18] = { _T("Index"),_T("ID"),_T("���O"),_T("Text"),_T("MP"),_T("���"),_T("�X�L��"),_T("����"),_T("�Ώ�"),_T("�L���X�g"),_T("���L���X�g"),_T("�W���u"),_T("Unknown") };
	const int nColumnWidth[] = { 30,30,112,80,40,0,60,0,0,0,0,100,280 };
#else
	const _TCHAR sztColumn[][0x18] = { _T("Index"),_T("ID"),_T("���O"),_T("Text"),_T("MP"),_T("���"),_T("�X�L��"),_T("����"),_T("�Ώ�"),_T("�L���X�g"),_T("���L���X�g"),_T("�W���u") };
	const int nColumnWidth[] = { 30,30,112,280,40,0,60,0,0,0,0,200 };
#endif
	LVCOLUMN col;

	ListView_SetExtendedListViewStyleEx(this->_hWnd,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
	);

	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	for (int nCol = 0; nCol<ARRAYSIZE(sztColumn); nCol++) {
		col.cx = nColumnWidth[nCol];
		col.pszText = (_TCHAR*)sztColumn[nCol];
		col.iSubItem = nCol;
		ListView_InsertColumn(this->_hWnd, nCol, &col);
	}

	for (int nPos = 0;; nPos++) {
		int nRet = magic->Get(nPos);
		if (nRet == 0)break;//����
		else if (nRet < 0) {
			return -1;
		}

		//Index
		wsprintf(sztBuff, _T("%d"), magic->m_Index);
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 0;//��
		lvi.pszText = sztBuff;
		lvi.lParam = nPos + 0x1000;//�\�[�g�plParam��ݒ�
		if (-1 == ListView_InsertItem(this->_hWnd, &lvi)) {
			return -10;
		}

		//ID
		wsprintf(sztBuff, _T("%d"), magic->m_ID);
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 1;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -11;
		}

		//���O
#ifdef UNICODE
		errno_t err = mbstowcs_s(NULL, sztBuff, (char*)magic->m_szcName, _TRUNCATE);
		//���s���鎖������̂ŏo�����MBS�ŃR���p�C��
#else
		_tcscpy_s(sztBuff, magic->m_szcName);
#endif
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 2;//��
		lvi.pszText = sztBuff;;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -12;
		}

		//Text
#ifdef UNICODE
		err = mbstowcs_s(NULL, sztBuff, (char*)magic->m_szcText, _TRUNCATE);
		//���s���鎖������̂ŏo�����MBS�ŃR���p�C��
#else
		_tcscpy_s(sztBuff, magic->m_szcText);
#endif
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 3;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -13;
		}

		//MP
		wsprintf(sztBuff, _T("%d"), magic->m_Mp);
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 4;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -14;
		}

		//���
		const _TCHAR sztType[][0x20] = { _T("-"),_T("�����@"),_T("�����@"),_T("����"),_T("�E�p"),_T("��"),_T("���@") };
		if (magic->m_Type < ARRAYSIZE(sztType)) {
			wsprintf(sztBuff, _T("%s"), sztType[magic->m_Type]);
		}
		else {
			wsprintf(sztBuff, _T("-"));
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 5;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -15;
		}

		//Skill
		const _TCHAR sztSkill[0x100][0x10] = { \
			_T("None"),_T("�i��"),_T("�Z��"),_T("�Ў茕"),_T("���茕"),_T("�Ў蕀"),_T("���蕀"),_T("���芙"),_T("���葄"),_T("�Ў蓁"),_T("���蓁"),_T("�Ў螞"),_T("���螞"),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("�|�p"),_T("�ˌ�"),_T("���Ă�"),_T(""),_T(""),_T(""),_T(""),\
			_T("�_�����@"),_T("�񕜖��@"),_T("�������@"),_T("��̖��@"),_T("���얂�@"),_T("�Í����@"),_T("�������@"),_T("�E�p"),_T("�̏�"),_T("���y��"),_T("�Ǌy��"),_T("���@"),_T(""),_T(""),_T(""),_T(""),\
			_T("�ނ�"),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),\
			_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("(��E�e)"),\
		};
		if (magic->m_Skill < ARRAYSIZE(sztSkill)) {
			wsprintf(sztBuff, _T("%s"), sztSkill[magic->m_Skill]);
		}
		else {
			wsprintf(sztBuff, _T("-"));
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 6;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -16;
		}

		//����
		const _TCHAR sztElement[][0x20] = { _T("��"),_T("�X"),_T("��"),_T("�y"),_T("��"),_T("��"),_T("��"),_T("��") };
		//const _TCHAR sztElement[][0x20] = {_T("Fire"),_T("Ice"),_T("Air"),_T("Earth"),_T("Thunder"),_T("Watar"),_T("Light"),_T("Dark"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("Special")};
		if (magic->m_Element < ARRAYSIZE(sztElement)) {
			wsprintf(sztBuff, _T("%s"), sztElement[magic->m_Element]);
		}
		else {
			wsprintf(sztBuff, _T("-"));
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 7;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -17;
		}

		//�Ώ�
		ZeroMemory(sztBuff, sizeof(sztBuff));
		const _TCHAR sztTarget[][0x20] = { _T("����"),_T("PC"),_T("PT"),_T("�A��"),_T("NPC"),_T("�G"),_T("-"),_T("����") };
		for (int i = 0; i<ARRAYSIZE(sztTarget); i++) {
			if (magic->m_Target & (0x00000001 << i)) {
				if (_tcslen(sztBuff) != 0) {
					_tcscat_s(sztBuff, _T("/"));
				}
				_tcscat_s(sztBuff, sztTarget[i]);
			}
		}
		if (_tcslen(sztBuff) == 0)wsprintf(sztBuff, _T("-"));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 8;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -18;
		}

		//�L���X�g
		_stprintf_s(sztBuff, _T("%.02f"), magic->m_Cast);
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 9;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -19;
		}

		//���L���X�g
		_stprintf_s(sztBuff, _T("%.02f"), magic->m_Recast);
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 10;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -20;
		}

		//�W���u
		ZeroMemory(sztBuff, sizeof(sztBuff));
		for (int nJob = 1; nJob<24; nJob++) {
			if (magic->m_Level[nJob] != 0xFF) {
				if (_tcslen(path->m_sztJobs[nJob]) == 0) {
					continue;
				}
				else if (_tcslen(sztBuff) != 0) {
					_tcscat_s(sztBuff, _T("/"));
				}
				_TCHAR sztAdd[0x10];
				wsprintf(sztAdd, _T("%s%d"), path->m_sztJobs[nJob], magic->m_Level[nJob]);
				_tcscat_s(sztBuff, sztAdd);
			}
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 11;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -21;
		}

#ifdef DEBUG
		//Unknown
		ZeroMemory(sztBuff, sizeof(sztBuff));
		for (int i = 0; i<7; i++) {
			wsprintf(&sztBuff[_tcslen(sztBuff)], _T("%02X"), magic->m_Unknown[i]);
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 12;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -22;
		}
#endif
	}

	return 1;
}


//1.3�Œǉ�
//int CLAS_LIST::AddItem(CLASS_PATH,CLASS_ITEM*)
//�A�C�e�������X�g�ɒǉ�
//�Ԃ�l�F����1�A���s0�ȉ�
int CLASS_LIST::AddItem(CLASS_PATH* path, CLASS_ITEM* item)
{
	LVITEM lvi;
	_TCHAR sztBuff[MAX_PATH];

	//ID,���\�[�XID,Name1,Name2,Name3,���,����,���[��,�X�^�b�N��,���A,EX,������
	const _TCHAR sztColumn[][0x18] = { _T("ID"),_T("RID"),_T("���O"),_T("���O2"),_T("���O3"),_T("���"),_T("����"),_T("���[��"),_T("�X�^�b�N��"),_T("Ra"),_T("Ex"),_T("Text"), };
	const int nColumnWidth[] = { 40,0,100,0,0,70,50,30,30,40,40,350 };
	LVCOLUMN col;

	ListView_SetExtendedListViewStyleEx(this->_hWnd,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER
	);

	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	for (int nCol = 0; nCol<ARRAYSIZE(sztColumn); nCol++) {
		col.cx = nColumnWidth[nCol];
		col.pszText = (_TCHAR*)sztColumn[nCol];
		col.iSubItem = nCol;
		ListView_InsertColumn(this->_hWnd, nCol, &col);
	}

	for (int nPos = 0;; nPos++) {
		int nRet = item->Get(nPos);
		if (nRet == 0)break;//����
		else if (nRet < 0 && nRet >= -3)return -1;//�t�H�[�}�b�g������Ă��t�@�C�����Ō�܂œǂ�

												  //ID
		wsprintf(sztBuff, _T("%d"), item->m_ID);
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = nPos;//�s
		lvi.iSubItem = 0;//��
		lvi.pszText = sztBuff;
		lvi.lParam = nPos + 0x1000;//�\�[�g�plParam��ݒ�
		if (-1 == ListView_InsertItem(this->_hWnd, &lvi)) {
			return -10;
		}

		//if(strlen(item->m_szcName1) > 1){
		//RID
		wsprintf(sztBuff, _T("%d"), item->m_ResourceID);
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -11;
		}

		//Name1
#ifdef UNICODE
		mbstowcs_s(NULL, sztBuff, item->m_szcName1, _TRUNCATE);
#else
		_tcscpy_s(sztBuff, item->m_szcName1);
#endif
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 2;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -12;
		}

		//Name2
		if (strlen(item->m_szcName2)) {
#ifdef UNICODE
			mbstowcs_s(NULL, sztBuff, item->m_szcName2, _TRUNCATE);
#else
			_tcscpy_s(sztBuff, item->m_szcName2);
#endif
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 3;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -13;
			}
		}

		//Name3
		if (strlen(item->m_szcName3)) {
#ifdef UNICODE
			mbstowcs_s(NULL, sztBuff, item->m_szcName3, _TRUNCATE);
#else
			_tcscpy_s(sztBuff, item->m_szcName3);
#endif
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 4;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -14;
			}
		}


		//���
		ZeroMemory(sztBuff, sizeof(sztBuff));
		const _TCHAR sztType[][0x20] = { \
			_T("-"),_T("�A�C�e��"),_T("�N�G�X�g"),_T("��"),_T("����"),_T("�h��"),_T("LinkShell"),_T("���Օi"),_T("�N���X�^��"),_T("�ݕ�"),_T("���x�i"),_T("��"),_T("�A�ؔ�"),_T("���炭��"),_T("�}�l�L��"),_T("�o���X�^"),\
			_T("RacingForm"),_T("�`���R�{"),_T("����"),_T("���b��"),_T("�A�T���g"),_T("�{�i���U"),_T("���C�Y�^�r����_M"),_T("���C�Y�^�r����_R"),_T("���C�Y�o�E�`���["),_T("���C�Y���[��"),_T("�G���H���X"),_T("�a�蒠"),_T("���M�I��"),_T("�~�[�u���E�o���[�Y"),\
		};
		if (item->m_Type < ARRAYSIZE(sztType)) {
			_tcscpy_s(sztBuff, sztType[item->m_Type]);
		}
		else {
			wsprintf(sztBuff, _T("�s��(0x%X)"), item->m_Type);
		}
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 5;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -15;
		}

		//����
		ZeroMemory(sztBuff, sizeof(sztBuff));
		const _TCHAR sztElement[][0x20] = { _T("��"),_T("�X"),_T("��"),_T("�y"),_T("��"),_T("��"),_T("��"),_T("��") };
		//const _TCHAR sztElement[][0x10] = {_T("Fire"),_T("Ice"),_T("Air"),_T("Earth"),_T("Thunder"),_T("Water"),_T("Light"),_T("Dark")};
		if (item->m_Element < 8) {
			_tcscpy_s(sztBuff, sztElement[item->m_Element]);
		}
		else if (item->m_Element == 0xFF) {
			wsprintf(sztBuff, _T("-"));//Undecided
		}
		else {
			wsprintf(sztBuff, _T(""));
		}
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 6;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -16;
		}

		//���[��
		wsprintf(sztBuff, _T("%d"), item->m_StorageSlots);
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 7;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -17;
		}

		//�X�^�b�N��
		wsprintf(sztBuff, _T("%d"), item->m_StackSize);
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 8;//��
		lvi.pszText = sztBuff;
		if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
			return -21;
		}

		//Rare
		if (item->m_Rare) {
			_tcscpy_s(sztBuff, _T("Rare"));
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 9;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -22;
			}
		}

		//Ex
		if (item->m_Ex) {
			_tcscpy_s(sztBuff, _T("Ex"));
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 10;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -23;
			}
		}

		//Text
		if (strlen(item->m_szcText)) {
			//0x0A��'/'�ɒu��
			for (int i = 0; i<(int)strlen(item->m_szcText); i++) {
				if (item->m_szcText[i] == 0x0A) {
					item->m_szcText[i] = '/';
				}
			}
#ifdef UNICODE
			errno_t err = mbstowcs_s(NULL, sztBuff, item->m_szcText, _TRUNCATE);
			//���s���鎖������̂ŏo�����MBS�ŃR���p�C��
#else
			_tcscpy_s(sztBuff, item->m_szcText);
#endif
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 11;//��
			lvi.pszText = sztBuff;
			if (FALSE == ListView_SetItem(this->_hWnd, &lvi)) {
				return -24;
			}
		}
		//}

	}

	return 1;
}


//int CLASS_LIST::SaveFileWeaponArmor(_TCHAR*)
//���X�g���t�@�C���ɕۑ�
//�Ԃ�l�F����1�A�L�����Z��0�A���s-1�ȉ�
int CLASS_LIST::SaveFileWeaponArmor(_TCHAR* sztFileName)
{
	_TCHAR sztFullPathName[MAX_PATH];
	GetFullPathName(sztFileName, ARRAYSIZE(sztFullPathName), sztFullPathName, NULL);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = this->_hWnd;
	ofn.lpstrFile = sztFullPathName;
	ofn.nMaxFile = sizeof(sztFullPathName) / sizeof(_TCHAR);
	ofn.lpstrFileTitle = sztFileName;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrFilter = _T("Text (*.txt)\0*.txt\0All (*.*)\0*.*\0\0");
	ofn.nFilterIndex = 0;
	ofn.lpstrDefExt = _T("txt");

	if (GetSaveFileName(&ofn) == 0) {//���s��
		if (CommDlgExtendedError() == 0) {//0�̎��̓L�����Z��
			return 0;
		}
		else {
			return -10;
		}
	}

	HANDLE hFile = CreateFile(sztFullPathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//1.0a�ŏC��
	if (hFile == INVALID_HANDLE_VALUE)return -1;

	int nItemCount = ListView_GetItemCount(this->_hWnd);
	if (nItemCount <= 0)return -2;

	_TCHAR sztID[0x10];
	_TCHAR sztRID[0x10];
	_TCHAR sztName1[0x50];
	_TCHAR sztName2[0x50];
	_TCHAR sztName3[0x50];
	_TCHAR sztSlot[0x10];
	_TCHAR sztRace[0x10];
	_TCHAR sztJob[0x50];
	_TCHAR sztDamage[0x10];
	_TCHAR sztDeley[0x10];
	_TCHAR sztLevel[0x10];
	_TCHAR sztStack[0x10];
	_TCHAR sztRare[0x10];
	_TCHAR sztEx[0x10];
	_TCHAR sztText[0x200];
	DWORD dwWriteSize;
	_TCHAR sztBuff[0x1000];

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;

	for (int nRow = 0; nRow<nItemCount; nRow++) {
		lvi.iItem = nRow;
		lvi.iSubItem = 0;
		lvi.cchTextMax = ARRAYSIZE(sztID);
		lvi.pszText = sztID;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 1;
		lvi.cchTextMax = ARRAYSIZE(sztRID);
		lvi.pszText = sztRID;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 2;
		lvi.cchTextMax = ARRAYSIZE(sztName1);
		lvi.pszText = sztName1;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 3;
		lvi.cchTextMax = ARRAYSIZE(sztName2);
		lvi.pszText = sztName2;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 4;
		lvi.cchTextMax = ARRAYSIZE(sztName3);
		lvi.pszText = sztName3;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 5;
		lvi.cchTextMax = ARRAYSIZE(sztSlot);
		lvi.pszText = sztSlot;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 6;
		lvi.cchTextMax = ARRAYSIZE(sztRace);
		lvi.pszText = sztRace;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 7;
		lvi.cchTextMax = ARRAYSIZE(sztJob);
		lvi.pszText = sztJob;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 8;
		lvi.cchTextMax = ARRAYSIZE(sztDamage);
		lvi.pszText = sztDamage;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 9;
		lvi.cchTextMax = ARRAYSIZE(sztDeley);
		lvi.pszText = sztDeley;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 10;
		lvi.cchTextMax = ARRAYSIZE(sztLevel);
		lvi.pszText = sztLevel;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 11;
		lvi.cchTextMax = ARRAYSIZE(sztStack);
		lvi.pszText = sztStack;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 12;
		lvi.cchTextMax = ARRAYSIZE(sztRare);
		lvi.pszText = sztRare;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 13;
		lvi.cchTextMax = ARRAYSIZE(sztEx);
		lvi.pszText = sztEx;
		ListView_GetItem(this->_hWnd, &lvi);

		lvi.iSubItem = 14;
		lvi.cchTextMax = ARRAYSIZE(sztText);
		lvi.pszText = sztText;
		ListView_GetItem(this->_hWnd, &lvi);

		if (_tcslen(sztJob) > 1) {
			//ID,���\�[�XID,Name1,Name2,Name3,���,�푰����,�W���u,D�l/���^�C�v,�Ԋu,���x��,�X�^�b�N��,���A,EX,������
			wsprintf(sztBuff,
				_T("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n"),
				sztID, sztRID, sztName1, sztName2, sztName3, sztSlot, sztRace, sztJob, sztDamage, sztDeley, sztLevel, sztStack, sztRare, sztEx, sztText
			);
			WriteFile(hFile, sztBuff, _tcslen(sztBuff), &dwWriteSize, NULL);
		}
	}

	CloseHandle(hFile);
	return 1;
}


//int CLAS_LIST::SaveFile(_TCHAR*)
//���X�g���t�@�C���ɕۑ�(���@�p)
//�Ԃ�l�F����1�A�L�����Z��0�A���s-1�ȉ�
int CLASS_LIST::SaveFile(_TCHAR* sztFileName)
{
	_TCHAR sztFullPathName[MAX_PATH];
	GetFullPathName(sztFileName, ARRAYSIZE(sztFullPathName), sztFullPathName, NULL);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = this->_hWnd;
	ofn.lpstrFile = sztFullPathName;
	ofn.nMaxFile = sizeof(sztFullPathName) / sizeof(_TCHAR);
	ofn.lpstrFileTitle = sztFileName;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrFilter = _T("Text (*.txt)\0*.txt\0All (*.*)\0*.*\0\0");
	ofn.nFilterIndex = 0;

	if (GetSaveFileName(&ofn) == 0) {//���s��
		if (CommDlgExtendedError() == 0) {//0�̎��̓L�����Z��
			return 0;
		}
		else {
			return -10;
		}
	}

	HANDLE hFile = CreateFile(sztFullPathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)return -1;

	int nItemCount = ListView_GetItemCount(this->_hWnd);
	if (nItemCount <= 0)return -2;
	int nColumnCount = Header_GetItemCount(ListView_GetHeader(this->_hWnd));
	if (nColumnCount == 0)return -3;

	DWORD dwWriteSize;
	_TCHAR sztBuff[0x1000];

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;

	for (int nRow = 0; nRow<nItemCount; nRow++) {
		ZeroMemory(sztBuff, sizeof(sztBuff));
		for (int nCol = 0; nCol<nColumnCount; nCol++) {
			if (_tcslen(sztBuff) != 0)_tcscat_s(sztBuff, _T(","));
			lvi.iItem = nRow;
			lvi.iSubItem = nCol;
			lvi.cchTextMax = ARRAYSIZE(sztBuff) - _tcslen(sztBuff);
			lvi.pszText = &sztBuff[_tcslen(sztBuff)];
			ListView_GetItem(this->_hWnd, &lvi);
		}
		_tcscat_s(sztBuff, _T("\r\n"));
		WriteFile(hFile, sztBuff, _tcslen(sztBuff), &dwWriteSize, NULL);
	}

	CloseHandle(hFile);
	return 1;
}

//int CLASS_MAGIC::Get(int)
//�t�@�C���������Ǎ���
//�Ԃ�l�F����1�A����0�A���s-1�ȉ�
int CLASS_MAGIC::Get(int nPos)
{
	BYTE byDataName[SIZEOF_MAGIC_NAME_STRUCT];//�Ǎ��݃f�[�^
	BYTE byDataText[SIZEOF_MAGIC_TEXT_STRUCT];//�Ǎ��݃f�[�^
	BYTE byDataDetails[SIZEOF_MAGIC_DETAILS_STRUCT];//�Ǎ��݃f�[�^
	DWORD dwReadSize;//�Ǎ��݃T�C�Y
	BYTE bBuff;

	this->Clear();

	if (SetFilePointer(this->_hFileName, nPos * SIZEOF_MAGIC_NAME_STRUCT + MAGIC_NAME_OFFSET, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)return -1;
	if (ReadFile(this->_hFileName, byDataName, sizeof(byDataName), &dwReadSize, NULL) == false)return -2;
	else if (dwReadSize == 0)return 0;
	if (dwReadSize != SIZEOF_MAGIC_NAME_STRUCT)return -3;

	if (SetFilePointer(this->_hFileText, nPos * SIZEOF_MAGIC_TEXT_STRUCT + MAGIC_TEXT_OFFSET, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)return -1;
	if (ReadFile(this->_hFileText, byDataText, sizeof(byDataText), &dwReadSize, NULL) == false)return -2;
	if (dwReadSize != SIZEOF_MAGIC_TEXT_STRUCT)return -3;

	//if(SetFilePointer(this->_hFileDetails,nPos * SIZEOF_MAGIC_DETAILS_STRUCT + MAGIC_DETAILS_OFFSET,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)return -1;
	//Details��20121109���݃��C���N���C�A���g�ƃe�X�g�T�[�o�ňʒu���قȂ�̂�mgc_���T�[�`���� 1.2�ŕύX
	DWORD dwMagicDetailsOffset = MAGIC_DETAILS_OFFSET - 0x10;
	for (int i = 0;; i++) {
		dwMagicDetailsOffset = MAGIC_DETAILS_OFFSET - 0x10 + i * 0x10;
		if (SetFilePointer(this->_hFileDetails, dwMagicDetailsOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			return -10;
		}
		else {
			BYTE byData_mgc_[5] = "";
			if (ReadFile(this->_hFileDetails, byData_mgc_, 4, &dwReadSize, NULL) == false) {
				return -11;
			}
			if (dwReadSize != 4) {
				return -12;
			}
			else if (strncmp((const char*)byData_mgc_, "mgc_", 4) == 0) {
				dwMagicDetailsOffset += 0x10;
				break;
			}
		}
	}
	if (SetFilePointer(this->_hFileDetails, nPos * SIZEOF_MAGIC_DETAILS_STRUCT + dwMagicDetailsOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)return -1;
	//
	if (ReadFile(this->_hFileDetails, byDataDetails, sizeof(byDataDetails), &dwReadSize, NULL) == false)return -2;
	if (dwReadSize != SIZEOF_MAGIC_DETAILS_STRUCT)return -3;

	//�R�R����f�[�^�𐶐�
	this->m_Index = nPos;
	int nOffset;
	memcpy(&nOffset, &byDataName[0x04], 4);
	wsprintf(this->m_szcName, _T("%s"), &byDataName[nOffset + 0x1C]);
	memcpy(&nOffset, &byDataText[0x04], 4);
	wsprintf(this->m_szcText, _T("%s"), &byDataText[nOffset + 0x1C]);

	this->_Decrypt(byDataDetails);//����
	memcpy(&(this->m_Index), &byDataDetails[0x00], 2);
	memcpy(&(this->m_Type), &byDataDetails[0x02], 2);
	memcpy(&(this->m_Element), &byDataDetails[0x04], 2);
	memcpy(&(this->m_Target), &byDataDetails[0x06], 2);
	memcpy(&(this->m_Skill), &byDataDetails[0x08], 2);
	memcpy(&(this->m_Mp), &byDataDetails[0x0A], 2);
	memcpy(&bBuff, &byDataDetails[0x0C], 1);
	this->m_Cast = (float)((float)bBuff / 4);
	memcpy(&bBuff, &byDataDetails[0x0D], 1);
	this->m_Recast = (float)((float)bBuff / 4);
	memcpy(&(this->m_Level), &byDataDetails[0x0e], 24);
	memcpy(&(this->m_ID), &byDataDetails[0x26], 2);
#ifdef DEBUG
	memcpy(&bBuff, &byDataDetails[0x0C], 1);
	this->m_Cast = (float)((float)bBuff);
	memcpy(&bBuff, &byDataDetails[0x0D], 1);
	this->m_Recast = (float)((float)bBuff);
	if (this->m_Index == 277) {
		//DebugBreak();
	}
	memcpy(&(this->m_Unknown), &byDataDetails[0x29], 7);
#endif

	// Block Layout:
	// 000-001 U16 Index
	// 002-003 U16 Magic Type (1/2/3/4/5/6 - White/Black/Summon/Ninja/Bard/Blue)
	// 004-005 U16 Element
	// 006-007 U16 Valid Targets
	// 008-009 U16 Skill
	// 00a-00b U16 MP Cost
	// 00c-00c U8  Cast Time (1/4 second)
	// 00d-00d U8  Recast Delay (1/4 second)
	// 00e-025 U8  Level required (1 byte per job, 0xff if not learnable; first is for the NUL job, so always 0xff; only 24 slots despite 32 possible job flags)
	// 026-027 U16 ID (0 for "unused" spells; starts out equal to the index, but doesn't stay that way)
	// 028-028 U8  List Icon ID (not sure what this is an index of, but it seems to match differences in item icon)
	// 029-029 U8  Unknown #1
	// 02a-02b U8  Unknown #2
	// 02c-02d U8  Unknown #3
	// 02e-02f U8  Unknown #4
	// 030-03e U8  Padding (NULs)
	// 03f-03f U8  End marker (0xff)

	return 1;
}
