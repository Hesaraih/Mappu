#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "..\ffhook\ffhook.h"
#pragma comment(lib, "ffhook")

int main(int argc, char *argv[])
{
	DWORD	pid;
	BYTE	attr, fcut;
	char	str[1024];
	int		n, index[32], cnt, len;
	FILE	*fp;

	// POE.EXEプロセス検索
	ZeroMemory(index, sizeof(index));

	printf("hook test -- qで終了\n");

	cnt = 0;
	fcut = 0;

	fopen_s(&fp, "log.txt", "w");

	while (1) {
		if (_kbhit()) {
			if (_getch() == 'q')
				break;
		}

		for (n = 0; n < 4; n++) {
			if (index[n] <= 0) {
				pid = FF11_GetProcessId(n);
				if (pid == 0)
					continue;
				if (FF11_GetBaseAddress(pid))
					index[n] = FF11_Inject(pid);
				if (index[n] <= 0)
					continue;
				FF11_SetCutString(index[n], "0x01", "ほげほげ");
				FF11_SetCutString(index[n], "0xce", "^げぶほ");
				FF11_SetCutString(index[n], 0, "ごぼごぼ");
				FF11_PutCmd(index[n], "/bow");
			}

			if (FF11_Rdy(index[n]) < 0) {
				index[n] = 0;
				continue;
			}

			len =  FF11_GetLog(index[n], &attr, str, 1024, &fcut);
			if (len > 0) {
				printf("index = %d attr = %02x %s\n", index[n], (int)attr, str);
				fprintf(fp, "%02x:%s\n", (int)attr, str);
			}

			len =  FF11_GetCmd(index[n], str, 1024);
			if (len > 0) {
				if (!strcmp(str, "/logcut")) {
					if (fcut ^= 1) {
						FF11_PutCmd(index[n], "/echo ログカット ON");
					} else {
						FF11_PutCmd(index[n], "/echo ログカット OFF");
					}
					FF11_LogCut(index[n], fcut);
				}
				printf("index = %d cmd %s\n", index[n], str);
				fprintf(fp, "cmd:%s\n", str);
			}
		}
		Sleep(1);
	}

	printf("eject\n");
	FF11_Eject(0);
	fclose(fp);

	printf("何かキーを押してください\n");
	_getch();

	return 0;
}
