DWORD GetDllModuleAddress(int);
int GetProcess(PROCESSENTRY32*,int);
int GetModule(MODULEENTRY32*,PROCESSENTRY32);
int CheckProcessID(DWORD);
int MakeListPID(HWND);
HWND GetHWNDFromPID(DWORD);
int GetPCNameFromPid(DWORD dwPID,_TCHAR *szPCName,int nSize);//0.37‚Å’Ç‰Á
int GetPCNameFromHandle(HANDLE hPol,DWORD dwDllAddr,int pos,_TCHAR* szPCName,int nSize);//0.37‚Å’Ç‰Á
int SetInitPID(HWND,_TCHAR*);//0.43‚Å’Ç‰Á