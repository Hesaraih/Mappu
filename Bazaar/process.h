//#include <psapi.h>
#include <tlhelp32.h>

DWORD GetDllModuleAddress(int);
int GetProcess(PROCESSENTRY32*,int);
int GetModule(MODULEENTRY32*,PROCESSENTRY32);
int ChangePID(HWND,int);
