#pragma once

#ifdef MTA_EXPORTS
#define MTA_EXPORT __declspec(dllexport)
#else
#define MTA_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

class MTA_EXPORT MTA
{
private:
	HANDLE m_hPol;
	DWORD m_Pid;
	DWORD m_OffsetNpcmap;
	DWORD m_OffsetTarget;
	DWORD m_NpcMemChangedId;
	DWORD m_NpcMemFixedId;
	DWORD m_NpcMemViewAddress;
	//BYTE m_OverWriteData[0x14];//\‘¢‘Ì‚É•ÏX
	struct{
		DWORD dwId;
		DWORD dwFixedId;
		DWORD dwTargetNpcmapAddress;
		DWORD dwViewAddress;
		BYTE byte01;
		BYTE byte00;
		WORD wKey;
	}m_OverWriteData;
	WORD m_GetKey();
	DWORD m_GetDllAddress();
	DWORD m_DllAddress;
	int m_GetIniFileIntHex(LPCTSTR sztSection, LPCTSTR sztKey);
public:
	MTA(DWORD dwPid,DWORD dwOffsetTarget,DWORD dwOffsetNpcmap);
	MTA(DWORD dwPid);
	~MTA();
	void SetTarget(DWORD dwId);
	void UnSetTarget();
};

#ifdef MTA_EXPORTS
MTA_EXPORT void* __stdcall mta_Init(DWORD dwProcessId,DWORD dwOffsetTarget,DWORD dwOffsetNpcmap){return new MTA(dwProcessId,dwOffsetTarget,dwOffsetNpcmap);}
MTA_EXPORT void* __stdcall mta_InitFromIniFile(DWORD dwProcessId){return new MTA(dwProcessId);}
MTA_EXPORT void __stdcall mta_Delete(void* mta){delete mta;}
MTA_EXPORT void __stdcall mta_SetTarget(void* mta,DWORD dwId){((MTA*)mta)->SetTarget(dwId);}
MTA_EXPORT void __stdcall mta_UnSetTarget(void* mta){((MTA*)mta)->UnSetTarget();}
#endif

#ifdef __cplusplus
}
#endif
