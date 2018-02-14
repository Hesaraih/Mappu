#include <commctrl.h>

int ListInit(HWND);
int CALLBACK CmpProc(LPARAM,LPARAM,LPARAM);
int ListDeletePC(HWND,_TCHAR*);
int ListReadFileCSV(HWND);
int ListWriteFileCSV(HWND,int);
int ListWriteFileHTML(HWND,int);
int ListSendOpenBazaar(HWND);
int ListOpenFFXIAH(HWND);
int ListResetLparam(HWND);
void ListSort(HWND,int);
int ListMakeListUp(HWND);
int ListChangeHeaderWidth(HWND,int,int);
int ListGetHeaderWidth(HWND,int);
void ReplaceString(_TCHAR*,_TCHAR,_TCHAR);


#define SORT_UP		0
#define SORT_DOWN	1

#ifdef LIST_CPP
static HWND hList;
static HWND hList_ListUp;
BYTE Sort[]={SORT_DOWN,SORT_DOWN,SORT_DOWN,SORT_DOWN,SORT_DOWN};
const static _TCHAR szColumn[][0x10]={_T("ñºëO"),_T("ïiñº"),_T("âøäi"),_T("å¬êî"),_T("îıçl")};
static WNDPROC WndProcMainClass;
LRESULT CALLBACK WndProcSubClass(HWND,UINT,WPARAM,LPARAM);

#else
extern BYTE Sort[];

#endif