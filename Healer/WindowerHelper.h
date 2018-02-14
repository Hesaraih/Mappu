#pragma once

#ifdef WINDOWERHELPER_EXPORTS
#define WINDOWERHELPER_API __declspec(dllexport)
#else
#define WINDOWERHELPER_API __declspec(dllimport)
#endif

#include <queue>
#include <windows.h>

using namespace std;

/* Text MMF */
struct MMFTextCommand
{
	DWORD Command;
	char Alias[256];
	char Param1[256];
	char Param2[256];
	char Param3[256];
	char Param4[256];
	char Text[2048];
};

struct MMFTextControlMapping
{
	unsigned long NumberOfCommands;
	bool Handled;
	MMFTextCommand Commands[32];
};

/* Keyboard MMF */
struct MMFKeyboardCommand
{
	DWORD Command;
	char Param1[8];
	char Param2[8];
	char Param3[8];
	char Param4[8];
	char Text[2048];
};

struct MMFKeyboardControlMapping
{
	unsigned long NumberOfCommands;
	bool Handled;
	MMFKeyboardCommand Commands[32];
};

/* Console MMF */
struct MMFConsoleArg
{
	char Value[2048];
};

struct MMFConsoleCommand
{
	int ArgCount;
	MMFConsoleArg Args[8];
};

struct MMFConsoleControlMapping
{
	unsigned long LastUpdate;
	MMFConsoleCommand Command;
};

/*class WINDOWERHELPER_API CTextHelper {
	public:
		CTextHelper(char* name);
		virtual ~CTextHelper();
		
		void CreateTextObject(char* name);
		void DeleteTextObject(char* name);
		void SetText(char* name, char* text);
		void SetVisibility(char* name, bool visible);
		void SetFont(char* name, char* font, int height);
		void SetColor(char* name, unsigned char a, unsigned char r, unsigned char g, unsigned char b);
		void SetLocation(char* name, float x, float y);
		void SetBold(char* name, bool bold);
		void SetItalic(char* name, bool italic);
		void SetBGColor(char* name, unsigned char a, unsigned char r, unsigned char g, unsigned char b);
		void SetBGBorderSize(char* name, float pixels);
		void SetBGVisibility(char* name, bool visible);		
		void SetRightJustified(char* name, bool justified);

		void FlushCommands();

	private:
		HANDLE m_MapFile;
		LPVOID m_MapAddress;
		MMFTextControlMapping* m_TCM;

		queue<MMFTextCommand> m_QueuedCommands;
};*/

class WINDOWERHELPER_API CKeyboardHelper {
	public:
		CKeyboardHelper(char* name);
		virtual ~CKeyboardHelper();

		void SetKey(unsigned char key, bool down);
		void BlockInput(bool block);
		void SendString(char* string);

	private:
		HANDLE m_MapFile;
		LPVOID m_MapAddress;
		MMFKeyboardControlMapping* m_KCM;
};

/*class WINDOWERHELPER_API CConsoleHelper {
	public:
		CConsoleHelper(char* name);
		virtual ~CConsoleHelper();

		bool IsNewCommand();
		int GetArgCount();
		void GetArg(int index, char* buffer);

	private:
		unsigned long m_LastReadCommand;
		HANDLE m_MapFile;
		LPVOID m_MapAddress;
		MMFConsoleControlMapping* m_CCM;
};*/

///* TEXT HELPER STATIC FUNCTIONS */
//void* CreateTextHelper(char* name) { return new CTextHelper(name); }
//void DeleteTextHelper(void* helper) { delete helper; }
//
//void CTHCreateTextObject(void* helper, char* name) { ((CTextHelper*)helper)->CreateTextObject(name); }
//void CTHDeleteTextObject(void* helper, char* name) { ((CTextHelper*)helper)->DeleteTextObject(name); }
//
//void CTHSetText(void* helper, char* name, char* text) { ((CTextHelper*)helper)->SetText(name, text); }
//void CTHSetVisibility(void* helper, char* name, bool visible) { ((CTextHelper*)helper)->SetVisibility(name, visible); }
//void CTHSetFont(void* helper, char* name, char* font, int height) { ((CTextHelper*)helper)->SetFont(name, font, height); };
//void CTHSetColor(void* helper, char* name, unsigned char a, unsigned char r, unsigned char g, unsigned char b) { ((CTextHelper*)helper)->SetColor(name, a, r, g, b); }
//void CTHSetLocation(void* helper, char* name, float x, float y) { ((CTextHelper*)helper)->SetLocation(name, x, y); }
//void CTHSetBold(void* helper, char* name, bool bold) { ((CTextHelper*)helper)->SetBold(name, bold); }
//void CTHSetItalic(void* helper, char* name, bool italic) { ((CTextHelper*)helper)->SetBold(name, italic); }
//void CTHSetBGColor(void* helper, char* name, unsigned char a, unsigned char r, unsigned char g, unsigned char b) { ((CTextHelper*)helper)->SetBGColor(name, a, r, g, b); }
//void CTHSetBGBorderSize(void* helper, char* name, float pixels) { ((CTextHelper*)helper)->SetBGBorderSize(name, pixels); }
//void CTHSetBGVisibility(void* helper, char* name, bool visible) { ((CTextHelper*)helper)->SetBGVisibility(name, visible); }
//void CTHSetRightJustified(void* helper, char* name, bool justified) { ((CTextHelper*)helper)->SetRightJustified(name, justified); }
//void CTHFlushCommands(void* helper) { ((CTextHelper*)helper)->FlushCommands(); }
//
///* KEYBOARD HELPER STATIC FUNCTIONS */
//void* CreateKeyboardHelper(char* name) { return new CKeyboardHelper(name); }
//void DeleteKeyboardHelper(void* helper) { delete helper; }
//
//void CKHSetKey(void* helper, unsigned char key, bool down) { ((CKeyboardHelper*)helper)->SetKey(key, down); }
//void CKHBlockInput(void* helper, bool block) { ((CKeyboardHelper*)helper)->BlockInput(block); }
//void CKHSendString(void* helper, char* string) { ((CKeyboardHelper*)helper)->SendString(string); }
//
///* CONSOLE HELPER STATIC FUNCTIONS */
//void* CreateConsoleHelper(char* name) { return new CConsoleHelper(name); }
//void DeleteConsoleHelper(void* helper) { delete helper; }
//
//bool CCHIsNewCommand(void* helper) { return ((CConsoleHelper*)helper)->IsNewCommand(); }
//int CCHGetArgCount(void* helper) { return ((CConsoleHelper*)helper)->GetArgCount(); }
//void CCHGetArg(void* helper, int index, char* buffer) { return ((CConsoleHelper*)helper)->GetArg(index, buffer); }

#define ESCAPE 0x01
#define NUM1 0x02
#define NUM2 0x03
#define NUM3 0x04
#define NUM4 0x05
#define NUM5 0x06
#define NUM6 0x07
#define NUM7 0x08
#define NUM8 0x09
#define NUM9 0x0A
#define NUM0 0x0B

#define NPNUM0 0x52
#define NPNUM1 0x4F
#define NPNUM2 0x50
#define NPNUM3 0x51
#define NPNUM4 0x4B
#define NPNUM5 0x5C
#define NPNUM6 0x4D
#define NPNUM7 0x47
#define NPNUM8 0x48
#define NPNUM9 0x49

#define NPMINUS 0x4A
#define NPADD 0x4E
#define NPPERIOD 0x53
#define NPRETURN 0x9C
#define NPFWDSLASH 0xB5

#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define F4 0x3E
#define F5 0x3F
#define F6 0x40
#define F7 0x41
#define F8 0x42
#define F9 0x43
#define F10 0x44
#define F11 0x57
#define F12 0x58

#define MINUS 0x0C
#define EQUALS 0x0D
#define BACKSP 0x0E
#define TAB 0x0F
 