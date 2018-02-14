#include "stdafx.h"
#include "..\ffhook_replo\ffhook_replo.h"
#pragma comment(lib, "ffhook_replo")

#define TITLE_BAR _T("FFXiLog")
#define FILE_NAME _T("%s\\FFXiLog_%04d-%02d-%02d_%02d-%02d-%02d.html")

#define HTML_HEADER _T("<html><head><title>FFXiLog</title></head><body bgcolor=\"#000020\" text=\"#ffffff\">\n<br>\n")
#define HTML_FOOTER _T("</body></html>\n")
#define HTML_BODY   _T("<font color=\"#%s\"><b><!--%s--> %s</b></font><br>\n<!--TIME: %d,%d,%d,%d,%d,%d, -->\n")

typedef struct {
	char	attr[53];
	char	data[512];
	time_t	time;
} LOG;

typedef struct {
	DWORD	pid;
	int		index;
	char	name[17];
	char	file[256];
	FILE	*fp;
	DWORD	count;
} POL;

POL	pols[6];

void init(POL *pol);
void end(void);
BOOL new_file(POL *pol);
void put_text(LOG *log, char *name);
void put_html(POL *pol, LOG *log);
void html_header(FILE *fp);
void html_footer(FILE *fp);
void html_body(FILE *fp, LOG *log);

int _tmain(int argc, _TCHAR* argv[])
{
	int n, len = 0;
	char name[17], title[64*7], pc[64];
	LOG log;
	BOOL update;

	atexit(end);

	strncpy_s(title, TITLE_BAR, _TRUNCATE);
	SetConsoleTitle(title);

	printf_s("FFXiLog start -- qÇ≈èIóπ\n");

	while (1) {
		if (_kbhit()) {
			if (_getch() == 'q')
				break;
		}
		strncpy_s(title, TITLE_BAR, _TRUNCATE);

		for (n = 0; n < 6; n++) {
			if (pols[n].index <= 0) {
				pols[n].pid = FF11_GetProcessId(n);
				if (pols[n].pid == 0)
					continue;
				if (FF11_GetBaseAddress(pols[n].pid))
					pols[n].index = FF11_Inject(pols[n].pid);
				if (pols[n].index <= 0)
					continue;
			}

			if (FF11_Rdy(pols[n].index) < 0) {
				init(&pols[n]);
				//pols[n].index = 0;
				continue;
			}

			FF11_GetPCname(pols[n].index, name);
			if (strcmp(pols[n].name, name) != 0) {
				if (pols[n].fp != NULL) {
					html_footer(pols[n].fp);
					fclose(pols[n].fp);
					pols[n].fp = NULL;
				}
				strncpy_s(pols[n].name, name, _TRUNCATE);
			}
			
			update = FALSE;
			while (len = FF11_GetReplo(pols[n].index, log.attr, log.data, 512, &log.time)) {
				update = TRUE;

				put_text(&log, pols[n].name);
				put_html(&pols[n], &log);
				
				if (strcmp(log.data, "@") == 0 || strcmp(log.data, "Åó") == 0) {
					if (strncmp(log.attr, "ce", 2) == 0) {
						if (pols[n].fp != NULL) {
							html_footer(pols[n].fp);
							fclose(pols[n].fp);
							pols[n].fp = NULL;
						}
					}
				}
			}

			if (update && pols[n].fp != NULL) fflush(pols[n].fp);

			if (pols[n].name) {
				sprintf_s(pc, " %s:%d", pols[n].name, pols[n].count);
				strcat_s(title, pc);
			}
		}
		
		SetConsoleTitle(title);
		Sleep(100);
	}

	end();

	printf_s("âΩÇ©ÉLÅ[ÇâüÇµÇƒÇ≠ÇæÇ≥Ç¢\n");
	_getch();
	return 0;
}

BOOL strtrim(char *buf, char *sea, int range)
{
    char *hit;
    size_t sealen;
	errno_t err;

	sealen = strlen(sea);

	if (sealen == 0) return FALSE;
	if ((hit = strstr(buf, sea)) == NULL) return FALSE;

	err = memmove_s(
		hit,
		strnlen_s(hit, strlen(buf)),
		hit + sealen + range,
		strnlen_s(hit, strlen(buf)));
	if (err) return FALSE;

	return TRUE;
}

BOOL strrep(char *buf, char *sea, char *rep)
{
    char *hit;
    size_t sealen, replen;
	errno_t err;

	sealen = strlen(sea);
	replen = strlen(rep);

	if (sealen == 0) return FALSE;
	if ((hit = strstr(buf, sea)) == NULL) return FALSE;

	err = memmove_s(
		hit + replen,
		strnlen_s(hit, strlen(buf)) + replen,
		hit + sealen,
		strlen(buf) - (hit + sealen - buf) + 1);
	if (err) return FALSE;

	err = memcpy_s(hit, strlen(hit) + replen, rep, replen);
	if (err) return FALSE;

	return TRUE;
}

void put_text(LOG *log, char *name)
{
	char text[512];

	strncpy_s(text, log->data, _TRUNCATE);

	while (strtrim(text, "\x1e\x03", 0));
	while (strtrim(text, "\x1e\x02", 0));
	while (strtrim(text, "\x1e\x01", 0));
	while (strrep(text, "\x1e\x27", "Åy"));
	while (strrep(text, "\x1e\x28", "Åz"));
	while (strrep(text, "Åè", "Å_"));

	printf_s("%s|%s\n", name, text);
}

BOOL new_file(POL *pol)
{
	time_t timer;
	tm t;

	if (pol->name) {
		timer = time(NULL);
		localtime_s(&t, &timer);
		sprintf_s(pol->file, FILE_NAME, pol->name,
			t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

		pol->fp = _fsopen(pol->file, "w", _SH_DENYWR);
		if (pol->fp == NULL) {
			if (_mkdir(pol->name) == 0) {
				pol->fp = _fsopen(pol->file, "w", _SH_DENYWR);
				if (pol->fp != NULL) return TRUE;
			}
		} else {
			return TRUE;
		}
	}

	return FALSE;
}

void html_header(FILE *fp)
{
	if (fp == NULL) return;

	fprintf_s(fp, HTML_HEADER);
	fflush(fp);
}

void html_footer(FILE *fp)
{
	if (fp == NULL) return;

	fprintf_s(fp, HTML_FOOTER);
	fflush(fp);
}

void html_body(FILE *fp, LOG *log)
{
	char text[1000];
	char color[7];
	tm t;

	if (fp == NULL) return;

	strncpy_s(color, &log->attr[11], _TRUNCATE);

	strncpy_s(text, log->data, _TRUNCATE);
	
	while (strrep(text, "<", "&lt;"));
	while (strrep(text, ">", "&gt;"));
	
	while (strrep(text, "  ", "Å@"));
	while (strrep(text, "Åè", "Å_"));
	while (strrep(text, "\x5c", "&#x20E5;"));

	while (strrep(text, "\x1e\x03", "<font color=\"6060c0\">"));
	while (strrep(text, "\x1e\x02", "<font color=\"40e040\">"));
	while (strrep(text, "\x1e\x01", "</font>"));
	while (strrep(text, "\x1e\x27", "<font color=\"208020\">Åy</font>"));
	while (strrep(text, "\x1e\x28", "<font color=\"802020\">Åz</font>"));

	localtime_s(&t, &log->time);

	fprintf_s(fp, HTML_BODY, color, log->attr, text,
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	//fflush(fp);
}

void put_html(POL *pol, LOG *log)
{
	if (pol->fp == NULL) {
		if (!new_file(pol)) return;
		pol->count = 0;
		html_header(pol->fp);
	}
	pol->count++;
	html_body(pol->fp, log);
}

void init(POL *pol)
{
	pol->pid = 0;
	pol->index = 0;
	if (pol->fp != NULL) {
		html_footer(pol->fp);
		fclose(pol->fp);
		pol->fp = NULL;
	}
}

void end(void)
{
	FF11_Eject(0);
	for (int i = 0; i < 6; i++) {
		if (pols[i].fp != NULL) {
			html_footer(pols[i].fp);
			fclose(pols[i].fp);
			pols[i].fp = NULL;
		}
	}
	_fcloseall();
}