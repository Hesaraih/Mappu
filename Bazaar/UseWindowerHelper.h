#include "WindowerHelper.h"

#ifdef USEWINDOWERHELPER_CPP
CKeyboardHelper* pKeyboardHelper;

static void* (__stdcall *DLL_CreateKeyboardHelper)(char*);
static void (__stdcall *DLL_DeleteKeyboardHelper)(void*);
void (__stdcall *DLL_CKHSetKey)(void*,unsigned char,bool);
void (__stdcall *DLL_CKHSendString)(void*,char*);


#else
extern CKeyboardHelper* pKeyboardHelper;
extern void (__stdcall *DLL_CKHSetKey)(void*,unsigned char,bool);
extern void (__stdcall *DLL_CKHSendString)(void*,char*);

#endif


int InitWindowerHelper(HWND);
int CloseWindowerHelper();
