#include <Windows.h>
#include <tchar.h>
#include "ffhook.h"
#pragma comment(lib,"ffhook_healer.lib")

static int ffhook_index;


int ffhook_init(DWORD pid)
{
	ffhook_index = FF11_Inject(pid);
	if (ffhook_index < 0)return -1;

	return 0;
}

int ffhook_end()
{
	FF11_Eject(ffhook_index);
	ffhook_index = 0;

	return 0;
}

int ffhook_putcmd(_TCHAR *cmd)
{
	FF11_PutCmd(ffhook_index, cmd);

	return 0;
}

int ffhook_getlog(BYTE *attr, char *cmd, int size)
{
	if (ffhook_index == 0)return 0;
	return FF11_GetLogA(ffhook_index, attr, cmd, size, NULL);
}

int ffhook_getcmd(_TCHAR *cmd, int size)
{
	if (ffhook_index == 0)return 0;
	return FF11_GetCmdW(ffhook_index, cmd, size);
}