#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <psapi.h>

#define MTA_EXPORTS
#include "mta.h"

#define STRING_POL	_T("pol.exe")
#define STRING_DLL	_T("FFXiMain.dll")

#define CHANGEDID		0x74
#define FIXEDID			0x78
#define VIEW_ADDRESS	0xA8//←UNKNOWNADDRESSにしてたけどオフセットチェッカーでいうVIEW_ADDRESSと同じだったっていう

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


MTA::MTA(DWORD dwProcessId, DWORD dwOffsetTarget, DWORD dwOffsetNpcmap)
{
	m_hPol = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessId);
	m_Pid = dwProcessId;
	m_DllAddress = m_GetDllAddress();
	m_OffsetTarget = dwOffsetTarget;
	m_OffsetNpcmap = dwOffsetNpcmap;
	m_NpcMemChangedId = CHANGEDID;
	m_NpcMemFixedId = FIXEDID;
	m_NpcMemViewAddress = VIEW_ADDRESS;
	ZeroMemory(&m_OverWriteData, sizeof(m_OverWriteData));
}

MTA::MTA(DWORD dwProcessId)
{
	m_hPol = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessId);
	m_Pid = dwProcessId;
	m_DllAddress = m_GetDllAddress();
	//mta.iniから取得
	m_OffsetTarget = m_GetIniFileIntHex(_T("OFFSET"), _T("TARGET"));
	m_OffsetNpcmap = m_GetIniFileIntHex(_T("OFFSET"), _T("NPCMAP"));
	m_NpcMemChangedId = m_GetIniFileIntHex(_T("NPC_MEM"), _T("CHANGEDID"));
	m_NpcMemFixedId = m_GetIniFileIntHex(_T("NPC_MEM"), _T("FIXEDID"));;
	m_NpcMemViewAddress = m_GetIniFileIntHex(_T("NPC_MEM"), _T("VIEW_ADDRESS"));;
	ZeroMemory(&m_OverWriteData, sizeof(m_OverWriteData));
}

MTA::~MTA()
{
	CloseHandle(m_hPol);
}

int MTA::m_GetIniFileIntHex(LPCTSTR sztSection, LPCTSTR sztKey)
{
	_TCHAR sztData[10];
	_TCHAR sztIniFilePath[MAX_PATH];

	ZeroMemory(sztData, sizeof(sztData));
	GetFullPathName(_T("mta.ini"), ARRAYSIZE(sztIniFilePath), sztIniFilePath, NULL);
	GetPrivateProfileString(sztSection, sztKey, 0, sztData, ARRAYSIZE(sztData), sztIniFilePath);
	return _tcstoul(sztData, NULL, 16);
}

void MTA::SetTarget(DWORD dwId)
{
	ZeroMemory(&m_OverWriteData, sizeof(m_OverWriteData));
	SIZE_T readsize = 0;
	DWORD dwAddress = 0;

	ReadProcessMemory(m_hPol, (LPCVOID)(m_DllAddress + m_OffsetNpcmap + dwId * 4), &m_OverWriteData.dwTargetNpcmapAddress, 4, &readsize);
	m_OverWriteData.dwId = dwId;
	ReadProcessMemory(m_hPol, (LPCVOID)(m_OverWriteData.dwTargetNpcmapAddress + FIXEDID), &m_OverWriteData.dwFixedId, 4, &readsize);
	ReadProcessMemory(m_hPol, (LPCVOID)(m_OverWriteData.dwTargetNpcmapAddress + VIEW_ADDRESS), &m_OverWriteData.dwViewAddress, 4, &readsize);
	m_OverWriteData.byte01 = 0x01;
	m_OverWriteData.byte00 = 0x00;
	m_OverWriteData.wKey = m_GetKey();

	ReadProcessMemory(m_hPol, (LPCVOID)(m_DllAddress + m_OffsetTarget), &dwAddress, 4, &readsize);

	SIZE_T writesize = 0;
	WriteProcessMemory(m_hPol, (LPVOID)dwAddress, &m_OverWriteData, sizeof(m_OverWriteData), &writesize);
}

void MTA::UnSetTarget()
{
	ZeroMemory(&m_OverWriteData, sizeof(m_OverWriteData));
	DWORD dwAddress = 0;

	SIZE_T readsize = 0;
	ReadProcessMemory(m_hPol, (LPCVOID)(m_DllAddress + m_OffsetTarget), &dwAddress, 4, &readsize);

	SIZE_T writesize = 0;
	WriteProcessMemory(m_hPol, (LPVOID)dwAddress, &m_OverWriteData, sizeof(m_OverWriteData), &writesize);
}

DWORD MTA::m_GetDllAddress()
{
	MODULEENTRY32 mo32;

	int iFlag = 0;
	//モジュールのスナップショットを作成
	HANDLE hModuleSnapshot;
	hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_Pid);

	if (hModuleSnapshot == NULL) {
		return 0;
	}

	//FFXiMain.dllのモジュールの情報を取得
	ZeroMemory(&mo32, sizeof(MODULEENTRY32));
	mo32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hModuleSnapshot, &mo32)) {
		do {
			if (_tcscmp(STRING_DLL, mo32.szModule) == 0) {
				iFlag = 1;
				break;
			}
		} while (Module32Next(hModuleSnapshot, &mo32));
	}
	CloseHandle(hModuleSnapshot);

	if (iFlag == 0) {
		return 0;
	}

	return (DWORD)mo32.modBaseAddr;
}

WORD MTA::m_GetKey()
{
	BYTE* ECX = (BYTE*)&m_OverWriteData;
	union {
	public:
		WORD X;
		DWORD EX;
	}A, D;
	//MOV DX,[ECX+4]
	//MOV EAX,ECX
	//SAR EAX,3
	//ADD DX,DX
	//ADD AX,DX
	//MOV DX,[ECX]
	//IMUL DX,DX,3
	//ADD AX,DX
	memcpy(&D.X, ECX + 4, 2);//MOV DX,[ECX+4]

	SIZE_T readsize = 0;
	ReadProcessMemory(m_hPol, (LPCVOID)((DWORD)m_DllAddress + m_OffsetTarget), &A.EX, 4, &readsize);//MOV EAX,ECX

	for (int i = 0; i<3; i++) {//SAR EAX,3
		A.EX = (A.EX >> 1) | (A.EX & 0x80000000);
	}

	D.X += D.X;//ADD DX,DX

	A.X += D.X;//ADD AX,DX

	memcpy(&D.X, ECX, 2);//MOV DX,[ECX]

	D.X = D.X * 3;//IMUL DX,DX,3

	A.X += D.X;//ADD AX,DX

	return A.X;
}