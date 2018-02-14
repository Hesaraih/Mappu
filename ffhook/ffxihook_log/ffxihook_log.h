#pragma once

class IFFXiHooks
{
public:
IFFXiHooks(void) {}
virtual ~IFFXiHooks(void) {}
virtual void Destroy(void) = 0;

virtual bool Open(void) = 0;
virtual void Close(void) = 0;
virtual bool IsReady(void) = 0;

virtual bool Sync(void) = 0;
virtual bool NumOfLines(DWORD &rNumOfLines) = 0;
virtual bool NextLine(DWORD &rNextLine) = 0;
virtual bool Generation(DWORD &rGeneration) = 0;
virtual bool Header(DWORD &rNumOfLines, DWORD &rNextLine, DWORD &rGeneration) = 0;

virtual LPCSTR Get(void) = 0;
virtual LPCSTR Get(__int64 &rTime) = 0;
virtual LPCSTR Get(BYTE &rAttribute) = 0;
virtual LPCSTR Get(__int64 &rTime, BYTE &rAttribute) = 0;

virtual int Get(LPWSTR lpBuffer, int size) = 0;
virtual int Get(LPWSTR lpBuffer, int size, __int64 &rTime) = 0;
virtual int Get(LPWSTR lpBuffer, int size, BYTE &rAttribute) = 0;
virtual int Get(LPWSTR lpBuffer, int size, __int64 &rTime, BYTE &rAttribute) = 0;

virtual LPCSTR Get(size_t index) = 0;
virtual LPCSTR Get(size_t index, __int64 &rTime) = 0;
virtual LPCSTR Get(size_t index, BYTE &rAttribute) = 0;
virtual LPCSTR Get(size_t index, __int64 &rTime, BYTE &rAttribute) = 0;

virtual int Get(size_t index, LPWSTR lpBuffer, int size) = 0;
virtual int Get(size_t index, LPWSTR lpBuffer, int size, __int64 &rTime) = 0;
virtual int Get(size_t index, LPWSTR lpBuffer, int size, BYTE &rAttribute) = 0;
virtual int Get(size_t index, LPWSTR lpBuffer, int size, __int64 &rTime, BYTE &rAttribute) = 0;
};


/*
typedef bool (__stdcall *LPF_SERVER_LOG)(LPVOID, LPCSTR, BYTE, DWORD, DWORD);
typedef bool (_stdcall *LPF_CLIENTPROC)(LPCSTR, DWORD, DWORD, LPVOID, LPVOID);

#ifdef FFXIHOOKS_EXPORTS
#define DLLEXPORT(type) __declspec(dllexport) type __stdcall
#else
#define DLLEXPORT(type) __declspec(dllimport) type __stdcall
#endif

DLLEXPORT(bool) FFXiHookLog_Inject(DWORD pid);
DLLEXPORT(bool) FFXiHookLog_Eject(DWORD pid);
DLLEXPORT(bool) FFXiHookLog_RegClient(LPF_CLIENTPROC client);
DLLEXPORT(void) FFXiHookLog_UnregClient(LPF_CLIENTPROC client);
DLLEXPORT(IFFXiHooks*) FFXiHookLog_Interface(void);
*/

