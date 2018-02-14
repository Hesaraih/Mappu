//���j�R�[�h�ŃR���p�C��
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "header.h"
#include "searchaddr.h"

extern FFXIOFFSET g_Offset;
extern NPC_MEM_MAP NPC_MEM;


int GetOffsetAddressAll(HANDLE hProcess, DWORD dwDllAddress)
{
	int ret = 0;

	g_Offset.NpcMap = GetOffsetAddress(hProcess, dwDllAddress, constNpcmapPattern);
	g_Offset.NowSta = GetOffsetAddress(hProcess, dwDllAddress, constNowstaPattern);
	if (g_Offset.NowSta) {
		g_Offset.NowSta -= NOWSTA_HOSEI;
	}
	g_Offset.NpcList = GetOffsetAddress(hProcess, dwDllAddress, constNpclistPattern);
	g_Offset.Target = GetOffsetAddress(hProcess, dwDllAddress, constTargetPattern);
	g_Offset.Weather = GetOffsetAddress(hProcess, dwDllAddress, constWeatherPattern);


	//NPC_MEM 0.05�Œǉ�
	NPC_MEM.Radian = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemRadianPattern) & 0xFF);
	NPC_MEM.ChangedId = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemChangedIdPattern) & 0xFF);
	NPC_MEM.FixedId = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemFixedIdPattern) & 0xFF);
	NPC_MEM.Name = (WORD)(GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNamePattern) & 0xFF);//0.05a�ŕύX
	NPC_MEM.ViewAddress = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemViewAddressPattern);
	NPC_MEM.Distance = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemDistancePattern);
	NPC_MEM.Hpp = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemHpPattern);
	NPC_MEM.NpcType = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNpcTypePattern);
	if (NPC_MEM.NpcType) {
		NPC_MEM.View = (WORD)(NPC_MEM.NpcType + 1);
	}
	NPC_MEM.Inrange = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemInRangePattern);
	NPC_MEM.State = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemStatePattern);
	NPC_MEM.LastHateId = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constNpcMemLastHateIdPattern);

	//�r�b�g�t���O 0.03��ret++�ɕύX
	if (g_Offset.NpcMap == 0) { ret++; }
	if (g_Offset.NowSta == 0) { ret++; }
	if (g_Offset.NpcList == 0) { ret++; }
	if (g_Offset.Target == 0) { ret++; }
	if (g_Offset.Weather == 0) { ret++; }

	//0.05�Œǉ�
	if (NPC_MEM.Radian == 0) { ret++; }
	if (NPC_MEM.ChangedId == 0) { ret++; }
	if (NPC_MEM.FixedId == 0) { ret++; }
	if (NPC_MEM.Name == 0) { ret++; }
	if (NPC_MEM.ViewAddress == 0) { ret++; }
	if (NPC_MEM.Distance == 0) { ret++; }
	if (NPC_MEM.Hpp == 0) { ret++; }
	if (NPC_MEM.NpcType == 0) { ret++; }
	if (NPC_MEM.View == 0) { ret++; }
	if (NPC_MEM.Inrange == 0) { ret++; }
	if (NPC_MEM.State == 0) { ret++; }
	if (NPC_MEM.LastHateId == 0) { ret++; }
	return ret;
}

/// <summary>
/// �I�t�Z�b�g�擾
/// </summary>
/// <param name="hProcess"></param>
/// <param name="dwDllAddress">FFXiMain.dll�̃A�h���X(�x�[�X�A�h���X)</param>
/// <param name="pattern">�o�C�g�p�^�[��</param>
/// <param name="dwStartAddress">�T�[�`�J�n�A�h���X�i���w�莞��dwDllAddress�Ɠ���)</param>
/// <param name="dwSearchRange">���g�p</param>
/// <returns></returns>
DWORD GetOffsetAddress(HANDLE hProcess, DWORD dwDllAddress, const BYTE* pattern, DWORD dwStartAddress, DWORD dwSearchRange)//0.05�ŕύX
{
	DWORD dwAddress;
	DWORD dwOffset = 0;
	BYTE buf[SEARCH_BLOCK + PATTERN_MAX_RANGE];
	if (dwStartAddress == 0) {
		dwStartAddress = dwDllAddress;//�T�[�`�J�n�A�h���X��FFXiMain.dll�̃A�h���X�ɂ��� 0.05�Œǉ�
	}

	for (int i = 0; i <= (int)SEARCH_RANGE / SEARCH_BLOCK; i++) {//0.05�ŕύX
		dwAddress = dwStartAddress + SEARCH_BLOCK * i;//0.05�ŕύX
		if (0 == ReadProcessMemory(hProcess, (LPCVOID)dwAddress, buf, SEARCH_BLOCK + PATTERN_MAX_RANGE, NULL)) {
			break;
		}

		if ((dwOffset = SearchAddress(buf, pattern, dwAddress, dwDllAddress)) != 0) {
			break;
		}
	}

	return dwOffset;
}

/// <summary>
/// �p�^�[������
/// </summary>
/// <param name="base"></param>
/// <param name="pattern"></param>
/// <param name="dwReadAddress">�J�n�A�h���X</param>
/// <param name="dwDllAddress">FFXiMain.dll�̃A�h���X(�x�[�X�A�h���X)</param>
/// <returns></returns>
DWORD SearchAddress(BYTE *base, const BYTE *pattern, DWORD dwReadAddress, DWORD dwDllAddress)
{
	BYTE	dat = 0, ptrn[256], mask[256], *p1, *p2;
	char	c, toggle = 0;
	int		n, cnt = 0, fp;
	DWORD	dwAddress = 0;

	//�����p�^�[���W�J
	ZeroMemory(ptrn, sizeof(ptrn));
	ZeroMemory(mask, sizeof(mask));

	toggle = 0;
	cnt = 0;

	for (n = 0; n < 100; n++) {
		c = (char)tolower(pattern[n]);
		if (c == ' ') {
			continue;
		}
		if (c == 0) { //������̏I�[�R�[�h
			mask[cnt] = 1;
			break;
		}
		if (c == '.') {
			mask[cnt] = 1;		//�u.�v�}�X�N�p�^�[��
		}
		else if (c == 'x') {
			mask[cnt] = 2;		//�ux�v�A�h���X��(�x�[�X�A�h���X+�I�t�Z�b�g�l)
		}
		else if (c == 'p') {//0.05���痘�p��
			mask[cnt] = 3;		// �up�v�I�t�Z�b�g��
		}
		else if (c == 'z') {
			mask[cnt] = 4;		// �uz�v�I�t�Z�b�g��
		}
		else if ('0' <= c && c <= '9') {
			dat |= (c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			dat |= (c - 'a' + 10);
		}
		else {
			return false;
		}

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		}
		else {
			dat <<= 4;
		}
		toggle ^= 1; //���4bit,����4bit�̐؂�ւ��t���O
	}

	// �ŏ��ɗL���ȃp�^�[���̈ʒu������(.���΂�)
	for (n = 0; n < cnt; n++) {
		if (mask[n] == 0) {
			//�u.�v�̂Ƃ��X�L�b�v
			break;
		}
	}

	fp = n;

	for (p1 = base; p1 < base + SEARCH_BLOCK; p1++) {
		if (*p1 != ptrn[fp]) {
			continue;
		}
		p2 = p1 + 1;
		for (n = fp + 1; n < cnt; n++, p2++) {
			if (mask[n]) {
				continue;
			}
			else if (*p2 != ptrn[n]) {
				break;
			}
		}
		if (n == cnt) {
			//��v����FFXiMain.dll�擪����̍����A�h���X(xp�����݂��Ȃ��p�^�[���͂��̒l���g����)
			dwAddress = (DWORD)(p1 - fp - base) + dwReadAddress - dwDllAddress;
			for (n = 0; n < cnt; n++) {
				if (mask[n] == 2) {//x:�A�h���X��
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					dwAddress -= dwDllAddress;//�������Ńx�[�X�A�h���X������
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//p�ɑΉ�
				if (mask[n] == 3) {//p:�I�t�Z�b�g��
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//z�ɑΉ�
				if (mask[n] == 4) {//z:byte�l
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(BYTE));
					break;
				}
			}
			break;
		}
	}

	return dwAddress;
}