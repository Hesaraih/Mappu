#define WM_TASKTRAY	(WM_USER+1)

#define ID_TASKTRAY	1000
#define ID_TIMER	2000

#define UP	1
#define DOWN	2

//追加されるPIDのメニュー位置
#define MENU_OTHER_POS		7//IDR_MENU_MAPの「他」の位置。メニューを追加したとき変更が必要 //1.40で変更
#define MENU_PROCESS_ID_POS	3//IDR_MENU_MAPの「他→監視対象」の位置 0.50で変更

//NPCLIST作成の最大数
#define MAX_SEARCH	0x400

//Map.iniのサブIDの最大数
#define MAP_SUB_ID_MAX 0x20

struct FFXIOFFSET {//0.17で変更 0.36で構造体名追加
	BYTE Auto;//0.36で追加
	DWORD NpcMap;
	DWORD NowSta;
	DWORD Target;//0.17で追加
	DWORD NpcList;//0.28で追加
	DWORD Weather;
};

//NPC情報のメモリマップ
struct NPC_MEM_MAP{//0.49で追加
	WORD POS_X;
	WORD POS_Y;
	WORD POS_Z;
	WORD Radian;
	WORD ChangedId;
	WORD FixedId;
	WORD Name;
	WORD ViewAddress;
	WORD Distance;
	WORD Hpp;
	WORD NpcType;//0.53で追加
	WORD View;
	WORD Inrange;
	WORD State;
	WORD LastHateId;
};
//NPCMAP:NPC情報アドレスが格納されている配列の先頭アドレス
//#define NPC_MEM_POS_X			0x04
//#define NPC_MEM_POS_Y			0x08
//#define NPC_MEM_POS_Z			0x0C
//#define NPC_MEM_RADIAN		0x18//x軸基準での(float)ラジアン
//#define NPC_MEM_CHANGEDID		0x74//changedID
//#define NPC_MEM_FIXEDID		0x78//fixedID
//#define NPC_MEM_NAME			0x7C
//#define NPC_MEM_VIEW_ADDRESS	0xA0//表示用アドレスが記載？
//#define NPC_MEM_DISTANCE		0xD8//自分と対象の距離
//#define NPC_MEM_HPP			0xEC
//#define NPC_MEM_NPCTYPE		0xFC//0.53で追加　NPC=0x04(範囲内)/0x40(範囲外)、ENEMY=0x24(範囲内)、0x60(範囲外、消滅)
//#define NPC_MEM_VIEW			0xFD//v0.42で追加　敵：範囲内0x40、範囲外0x40、死亡0xC5、消滅0xC4/0xC5/0xC1⇒消滅は0xC*、見える時は0x4*
//#define NPC_MEM_INRANGE		0x141//範囲内敵0x0F、PC0x1F、NPC0x57(0x1F等)。範囲外or自分0x00 //0.48で変更(20110712VU)
//#define NPC_MEM_STATE			0x180//フリー0x00、戦闘中0x01、死亡0x03 //0.48で変更(20110712VU)
//#define NPC_MEM_LASTHATEID	0x184//最後にヘイト行動したPCの固定ID //0.48で変更(20110712VU)
#define NPC_MEM_SIZE	0x2F0//メモリを読み込むサイズ //0.49で変更

//NOWSTAのメモリマップ
//NOWSTAのオフセット
//ログインしているキャラ名で始まるサイズ0x58バイトの構造体配列のアドレス
//struct{
//-0x06(0x00)	DWORD 不明;//(多分本来はここから構造体スタート)
//-0x02(0x04)	BYTE PartyPos?;//メンバーの順番?
//-0x01(0x05)	BYTE PartyPos?;//メンバーの順番?
//+0x00(0x06)	char name[0x12];//ログインしている自分の名前
//+0x12(0x18)	DWORD fixed_id;//自分の固定ID
//+0x16(0x1C)	DWORD change_id;//自分の変動ID
//+0x1A(0x20)	DWORD 不明;
//+0x1E(0x24)	DWORD HP;
//+0x22(0x28)	DWORD MP;
//+0x26(0x2C)	DWORD TP;
//+0x2A(0x30)	BYTE HPP;
//+0x2B(0x31)	BYTE MPP;
//+0x2C(0x32)	WORD AreaID;
//+0x2E(0x34)	DWORD 不明;//Fill NULL?
//+0x32(0x38)	DWORD 不明;
//+0x36(0x3C)	DWORD 不明;//Fill NULL?
//+0x3A(0x40)	DWORD 不明;//Fill NULL?
//+0x3E(0x44)	DWORD 不明;//Fill NULL?
//+0x42(0x48)	DWORD 不明;//Fill NULL?
//+0x46(0x4C)	DWORD 不明;//Fill NULL?
//+0x4A(0x50)	DWORD fixed_id;//自分の固定ID
//
//+0x72(0x78)	BYTE HPP;
//+0x73(0x79)	BYTE MPP;
//+0x74(0x7A)	BYTE Party;//パーティに入っていると0x01抜けると0x00
//+0x75(0x7B)	BYTE 不明;
//}NOWSTA[18](20110418現在)
//NOWSTA[0]が自分、NOWSTA[1-5]がパーティ、NOWSTA[6-17]がアラ?
//(FFXiMain.dllの先頭アドレス+NPCMAP+4Byte*change_id)で自分の情報
#define NOWSTA_HOSEI		(-0x06)//開始位置が微妙に違うみたいなので補正
#define NOWSTA_SIZE			0x7C
#define NOWSTA_MEM_NAME		0x06//未使用
#define NOWSTA_MEM_FIXEDID	0x18//未使用
#define NOWSTA_MEM_CHANGEID	0x1C
#define NOWSTA_MEM_HP		0x24//未使用
#define NOWSTA_MEM_MP		0x28//未使用
#define NOWSTA_MEM_TP		0x2C//未使用
#define NOWSTA_MEM_HPP		0x30//未使用
#define NOWSTA_MEM_MPP		0x31//未使用
#define NOWSTA_MEM_AREAID	0x32
#define NOWSTA_MEM_READER	0x38//未使用
#define NOWSTA_MEM_PARTY	0x7A

//NPCLISTのメモリマップ
//NPCLIST:noneで始まっているサイズ0x1Cバイトの構造体配列のアドレスが記載されているアドレス
//+0hから0x18にNPC名、終端文字列\0含まない場合有り
//struct{
//	char name[0x1C];
//	DWORD id;
//  NpcID = id&0x0FFF;
//  AreaID = (id>>12)&0xFFFF;
//}*NPCLIST;
#define NPCLIST_ID		0x1C
#define NPCLIST_SIZE	0x20

//文字列
#define STRING_POL		_T("pol.exe")
#define STRING_DLL		_T("FFXiMain.dll")
#define STRING_CLASS	_T("FFXiClass")
#define STRING_INI		_T("Mappu.ini")

//射程距離
#define MAX_DISTANCE_ABL		(float)18.0
#define MAX_DISTANCE_MGC		(float)21.5
#define MAX_DISTANCE_RNG		(float)25.0
#define MAX_DISTANCE_WINDSONG	(float)10.0
#define MAX_DISTANCE_DETECTION	(float)20.0

//リソースの一部
#define IDM_SELECT_PID1		(IDM_SELECT_PID + 1)
#define IDM_SELECT_PID2		(IDM_SELECT_PID + 2)
#define IDM_SELECT_PID3		(IDM_SELECT_PID + 3)
#define IDM_SELECT_PID4		(IDM_SELECT_PID + 4)
#define IDM_SELECT_PID5		(IDM_SELECT_PID + 5)
#define IDM_SELECT_PID6		(IDM_SELECT_PID + 6)
#define IDM_SELECT_PID7		(IDM_SELECT_PID + 7)
#define IDM_SELECT_PID8		(IDM_SELECT_PID + 8)
#define IDM_SELECT_PID9		(IDM_SELECT_PID + 9)
#define IDM_SELECT_PID10	(IDM_SELECT_PID + 10)
#define IDM_SELECT_PID11	(IDM_SELECT_PID + 11)
#define IDM_SELECT_PID12	(IDM_SELECT_PID + 12)
#define IDM_SELECT_PID13	(IDM_SELECT_PID + 13)
#define IDM_SELECT_PID14	(IDM_SELECT_PID + 14)
#define IDM_SELECT_PID15	(IDM_SELECT_PID + 15)
#define IDM_SELECT_PID16	(IDM_SELECT_PID + 16)

//フラグ判定
//#define NPC_CORPSE(x) (x.hpp == 0 && x.viewAddress == 0)
#define NPC_CORPSE(x) (((x.view)&0xF0) >= 0xC0)//0.42で変更
#define NPC_WAR(x) ((x.state != 0 && x.hpp != 0) || (x.hpp == 0 && x.viewAddress != 0))
#define NPC_POP(x) (x.state == 0 && x.hpp != 0)


//定義されてなければ定義
#ifndef WS_EX_COMPOSITED
#define WS_EX_COMPOSITED	0x2000000
#endif

#ifdef UNICODE
#ifndef _T
#define _T(x) L##x
#endif
#endif

#ifndef UNICODE
#ifndef _T
#define _T(x) x
#endif
#endif

#ifdef UNICODE

#ifdef strcpy
#undef strcpy
#endif

#ifdef strcmp
#undef strcmp
#endif

#define atoi	_wtoi
#define atof	_wtof
#define fgets	fgetws
#define fopen	_wfopen
#define isalpha	iswalpha
#define setlocale	_wsetlocale
#define strcmp	wcscmp
#define strcpy	wcscpy
#define strlen	wcslen
#define strncmp	wcsncmp
#define strtoul		wcstoul
#define sprintf	swprintf
#define sscanf	swscanf
#define tolower	towlower
#endif


struct INFORMATION{
	WORD id;
	DWORD fixedid;
	DWORD lasthateid;
	float x;
	float y;
	float z;
	float radian;
	//float distance;
	_TCHAR name[0x20];//最大は0x18+'\0'
	DWORD viewAddress;
	BYTE npctype;//0.53で追加
	BYTE view;
	BYTE hpp;
	BYTE state;
	BYTE inrange;
	float distance_2;//距離の２乗値 0.57で追加
	int code;
};

struct MENU_FLAG{
	BYTE default_zoomset;
	BYTE npc_name;
	BYTE npc_hpp;
	BYTE npc_id;
	BYTE npc_timer;
	BYTE npc_onlyvisible;
	BYTE npc_name_ex;
	BYTE npc_hpp_ex;
	BYTE npc_id_ex;
	BYTE npc_timer_ex;
	BYTE npc_onlyvisible_ex;
	BYTE npc_name_sp;
	BYTE npc_hpp_sp;
	BYTE npc_id_sp;
	BYTE npc_timer_sp;
	BYTE npc_onlyvisible_sp;
	BYTE own_direction;
	BYTE own_position;
	BYTE own_lt_pos_gt;
	BYTE own_trace_line;//0.55で追加
	BYTE own_trace_point;//0.55で追加
	BYTE party_position;
	BYTE party_name;
	BYTE pc_position;
	BYTE pc_name;
	BYTE pet_position;
	BYTE pet_name;
	BYTE target_name;
	BYTE target_hpp;
	BYTE target_id;
	BYTE circle_visible;
	BYTE circle_ability;
	BYTE circle_magic;
	BYTE circle_range;
	BYTE circle_windsong;
	BYTE circle_detection;
	BYTE popsound[3];//0.49で変更 0:Normal 1:Ex 2:Sp
	BYTE statesound[3];//0.49で変更
	BYTE weather_icon;
	BYTE weather_sound;

	BYTE alpha_map;
	BYTE alpha_npclist;
	BYTE transparent;

	BYTE radar;
	BYTE center_own;
	BYTE image_reverse;
	BYTE hide_height_diff;//0.56で追加

	BYTE always_update;
	BYTE file_autoload;
	BYTE hide_toolbar;//0.50で追加
	BYTE hide_buttonbox;//0.50で追加
	struct{
		int id;
		int name;
		int timer;
		int state;
		int type;
		int ym;//0.57で追加
		int note;
	}column;//0.50で追加
	BYTE list_control;//1.40で追加
};

struct MAP_INFORMATION{
	float x;
	float z;
	float zoom;//倍率
	WORD AreaID;//エリアID
	BYTE SubID;
	_TCHAR AreaName[MAX_PATH+1];
	int code;
};

struct FONT{
	HFONT hFont;
	int nSize;
	int nBold;
	_TCHAR szType[0x20];
	int nEffect;//0=OFF,1=DropShadow,2=Edge
	int nDS_x;
	int nDS_z;
	COLORREF ColorEffect;
};

struct POSXYZR{//0.55で追加
	float x;
	float y;
	float z;
	float radian;
	BYTE flag;
};

struct LINK{//0.59で追加
	_TCHAR exe[MAX_PATH];
	_TCHAR cmd[0x20];
	_TCHAR link[4][0x20];
	_TCHAR url[4][MAX_PATH];
};

const _TCHAR szColumn[][0x10]={_T("ID"),_T("Name"),_T("State"),_T("Timer"),_T("Type"),_T("Ym"),_T("Note")};//0.57aで追加

const _TCHAR szVK[][10]={
	_T(""),_T("LButton"),_T("RButton"),_T("?"),_T("MButton"),_T("?"),_T("?"),_T("?"),_T("BkSpace"),_T("Tab"),_T("-"),_T("-"),_T("Clear"),_T("Enter"),_T("-"),_T("-"),
	_T("Shift"),_T("Ctrl"),_T("Alt"),_T("Pause"),_T("CpsLck"),_T("?"),_T("-"),_T("?"),_T("?"),_T("?"),_T("-"),_T("Esc"),_T("?"),_T("?"),_T("?"),_T("?"),
	_T("Space"),_T("PgUp"),_T("PgDn"),_T("End"),_T("Home"),_T("←"),_T("↑"),_T("→"),_T("↓"),_T("Select"),_T("Print"),_T("?"),_T("PrScn"),_T("Ins"),_T("Del"),_T("?"),
	_T("0"),_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),
	_T(""),_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F"),_T("G"),_T("H"),_T("I"),_T("J"),_T("K"),_T("L"),_T("M"),_T("N"),_T("O"),
	_T("P"),_T("Q"),_T("R"),_T("S"),_T("T"),_T("U"),_T("V"),_T("W"),_T("X"),_T("Y"),_T("Z"),_T("?"),_T("?"),_T("?"),_T("?"),_T("?"),
	_T("0"),_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),_T("*"),_T("+"),_T("Enter"),_T("-"),_T("."),_T("/"),
	_T("F1"),_T("F2"),_T("F3"),_T("F4"),_T("F5"),_T("F6"),_T("F7"),_T("F8"),_T("F9"),_T("F10"),_T("F11"),_T("F12"),_T("F13"),_T("F14"),_T("F15"),_T("F16"),
	_T("F17"),_T("F18"),_T("F19"),_T("F20"),_T("F21"),_T("F22"),_T("F23"),_T("F24"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-"),_T("-")
};

#ifdef DEBUG//
#define GETLASTERROR {\
	LPVOID lpMsgBuf;\
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)&lpMsgBuf,0,NULL);\
	MessageBox(NULL,(LPTSTR)lpMsgBuf,_T("Error Message"),MB_OK|MB_ICONERROR);\
	LocalFree(lpMsgBuf);}
#else
#define GETLASTERROR
#endif//