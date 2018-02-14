#include <tlhelp32.h>

DWORD GetDllModuleAddress(int Num,int *pId,HANDLE *polHandle);
int GetProcess(PROCESSENTRY32 *pe32,int iProcessCounter);
int GetModule(MODULEENTRY32 *mo32,PROCESSENTRY32 pe32);
