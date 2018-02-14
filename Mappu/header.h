#define WM_TASKTRAY	(WM_USER+1)

#define ID_TASKTRAY	1000
#define ID_TIMER	2000

#define UP	1
#define DOWN	2

//�ǉ������PID�̃��j���[�ʒu
#define MENU_OTHER_POS		7//IDR_MENU_MAP�́u���v�̈ʒu�B���j���[��ǉ������Ƃ��ύX���K�v //1.40�ŕύX
#define MENU_PROCESS_ID_POS	3//IDR_MENU_MAP�́u�����Ď��Ώہv�̈ʒu 0.50�ŕύX

//NPCLIST�쐬�̍ő吔
#define MAX_SEARCH	0x400

//Map.ini�̃T�uID�̍ő吔
#define MAP_SUB_ID_MAX 0x20

struct FFXIOFFSET {//0.17�ŕύX 0.36�ō\���̖��ǉ�
	BYTE Auto;//0.36�Œǉ�
	DWORD NpcMap;
	DWORD NowSta;
	DWORD Target;//0.17�Œǉ�
	DWORD NpcList;//0.28�Œǉ�
	DWORD Weather;
};

//NPC���̃������}�b�v
struct NPC_MEM_MAP{//0.49�Œǉ�
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
	WORD NpcType;//0.53�Œǉ�
	WORD View;
	WORD Inrange;
	WORD State;
	WORD LastHateId;
};
//NPCMAP:NPC���A�h���X���i�[����Ă���z��̐擪�A�h���X
//#define NPC_MEM_POS_X			0x04
//#define NPC_MEM_POS_Y			0x08
//#define NPC_MEM_POS_Z			0x0C
//#define NPC_MEM_RADIAN		0x18//x����ł�(float)���W�A��
//#define NPC_MEM_CHANGEDID		0x74//changedID
//#define NPC_MEM_FIXEDID		0x78//fixedID
//#define NPC_MEM_NAME			0x7C
//#define NPC_MEM_VIEW_ADDRESS	0xA0//�\���p�A�h���X���L�ځH
//#define NPC_MEM_DISTANCE		0xD8//�����ƑΏۂ̋���
//#define NPC_MEM_HPP			0xEC
//#define NPC_MEM_NPCTYPE		0xFC//0.53�Œǉ��@NPC=0x04(�͈͓�)/0x40(�͈͊O)�AENEMY=0x24(�͈͓�)�A0x60(�͈͊O�A����)
//#define NPC_MEM_VIEW			0xFD//v0.42�Œǉ��@�G�F�͈͓�0x40�A�͈͊O0x40�A���S0xC5�A����0xC4/0xC5/0xC1�ˏ��ł�0xC*�A�����鎞��0x4*
//#define NPC_MEM_INRANGE		0x141//�͈͓��G0x0F�APC0x1F�ANPC0x57(0x1F��)�B�͈͊Oor����0x00 //0.48�ŕύX(20110712VU)
//#define NPC_MEM_STATE			0x180//�t���[0x00�A�퓬��0x01�A���S0x03 //0.48�ŕύX(20110712VU)
//#define NPC_MEM_LASTHATEID	0x184//�Ō�Ƀw�C�g�s������PC�̌Œ�ID //0.48�ŕύX(20110712VU)
#define NPC_MEM_SIZE	0x2F0//��������ǂݍ��ރT�C�Y //0.49�ŕύX

//NOWSTA�̃������}�b�v
//NOWSTA�̃I�t�Z�b�g
//���O�C�����Ă���L�������Ŏn�܂�T�C�Y0x58�o�C�g�̍\���̔z��̃A�h���X
//struct{
//-0x06(0x00)	DWORD �s��;//(�����{���͂�������\���̃X�^�[�g)
//-0x02(0x04)	BYTE PartyPos?;//�����o�[�̏���?
//-0x01(0x05)	BYTE PartyPos?;//�����o�[�̏���?
//+0x00(0x06)	char name[0x12];//���O�C�����Ă��鎩���̖��O
//+0x12(0x18)	DWORD fixed_id;//�����̌Œ�ID
//+0x16(0x1C)	DWORD change_id;//�����̕ϓ�ID
//+0x1A(0x20)	DWORD �s��;
//+0x1E(0x24)	DWORD HP;
//+0x22(0x28)	DWORD MP;
//+0x26(0x2C)	DWORD TP;
//+0x2A(0x30)	BYTE HPP;
//+0x2B(0x31)	BYTE MPP;
//+0x2C(0x32)	WORD AreaID;
//+0x2E(0x34)	DWORD �s��;//Fill NULL?
//+0x32(0x38)	DWORD �s��;
//+0x36(0x3C)	DWORD �s��;//Fill NULL?
//+0x3A(0x40)	DWORD �s��;//Fill NULL?
//+0x3E(0x44)	DWORD �s��;//Fill NULL?
//+0x42(0x48)	DWORD �s��;//Fill NULL?
//+0x46(0x4C)	DWORD �s��;//Fill NULL?
//+0x4A(0x50)	DWORD fixed_id;//�����̌Œ�ID
//
//+0x72(0x78)	BYTE HPP;
//+0x73(0x79)	BYTE MPP;
//+0x74(0x7A)	BYTE Party;//�p�[�e�B�ɓ����Ă����0x01�������0x00
//+0x75(0x7B)	BYTE �s��;
//}NOWSTA[18](20110418����)
//NOWSTA[0]�������ANOWSTA[1-5]���p�[�e�B�ANOWSTA[6-17]���A��?
//(FFXiMain.dll�̐擪�A�h���X+NPCMAP+4Byte*change_id)�Ŏ����̏��
#define NOWSTA_HOSEI		(-0x06)//�J�n�ʒu�������ɈႤ�݂����Ȃ̂ŕ␳
#define NOWSTA_SIZE			0x7C
#define NOWSTA_MEM_NAME		0x06//���g�p
#define NOWSTA_MEM_FIXEDID	0x18//���g�p
#define NOWSTA_MEM_CHANGEID	0x1C
#define NOWSTA_MEM_HP		0x24//���g�p
#define NOWSTA_MEM_MP		0x28//���g�p
#define NOWSTA_MEM_TP		0x2C//���g�p
#define NOWSTA_MEM_HPP		0x30//���g�p
#define NOWSTA_MEM_MPP		0x31//���g�p
#define NOWSTA_MEM_AREAID	0x32
#define NOWSTA_MEM_READER	0x38//���g�p
#define NOWSTA_MEM_PARTY	0x7A

//NPCLIST�̃������}�b�v
//NPCLIST:none�Ŏn�܂��Ă���T�C�Y0x1C�o�C�g�̍\���̔z��̃A�h���X���L�ڂ���Ă���A�h���X
//+0h����0x18��NPC���A�I�[������\0�܂܂Ȃ��ꍇ�L��
//struct{
//	char name[0x1C];
//	DWORD id;
//  NpcID = id&0x0FFF;
//  AreaID = (id>>12)&0xFFFF;
//}*NPCLIST;
#define NPCLIST_ID		0x1C
#define NPCLIST_SIZE	0x20

//������
#define STRING_POL		_T("pol.exe")
#define STRING_DLL		_T("FFXiMain.dll")
#define STRING_CLASS	_T("FFXiClass")
#define STRING_INI		_T("Mappu.ini")

//�˒�����
#define MAX_DISTANCE_ABL		(float)18.0
#define MAX_DISTANCE_MGC		(float)21.5
#define MAX_DISTANCE_RNG		(float)25.0
#define MAX_DISTANCE_WINDSONG	(float)10.0
#define MAX_DISTANCE_DETECTION	(float)20.0

//���\�[�X�̈ꕔ
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

//�t���O����
//#define NPC_CORPSE(x) (x.hpp == 0 && x.viewAddress == 0)
#define NPC_CORPSE(x) (((x.view)&0xF0) >= 0xC0)//0.42�ŕύX
#define NPC_WAR(x) ((x.state != 0 && x.hpp != 0) || (x.hpp == 0 && x.viewAddress != 0))
#define NPC_POP(x) (x.state == 0 && x.hpp != 0)


//��`����ĂȂ���Β�`
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
	_TCHAR name[0x20];//�ő��0x18+'\0'
	DWORD viewAddress;
	BYTE npctype;//0.53�Œǉ�
	BYTE view;
	BYTE hpp;
	BYTE state;
	BYTE inrange;
	float distance_2;//�����̂Q��l 0.57�Œǉ�
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
	BYTE own_trace_line;//0.55�Œǉ�
	BYTE own_trace_point;//0.55�Œǉ�
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
	BYTE popsound[3];//0.49�ŕύX 0:Normal 1:Ex 2:Sp
	BYTE statesound[3];//0.49�ŕύX
	BYTE weather_icon;
	BYTE weather_sound;

	BYTE alpha_map;
	BYTE alpha_npclist;
	BYTE transparent;

	BYTE radar;
	BYTE center_own;
	BYTE image_reverse;
	BYTE hide_height_diff;//0.56�Œǉ�

	BYTE always_update;
	BYTE file_autoload;
	BYTE hide_toolbar;//0.50�Œǉ�
	BYTE hide_buttonbox;//0.50�Œǉ�
	struct{
		int id;
		int name;
		int timer;
		int state;
		int type;
		int ym;//0.57�Œǉ�
		int note;
	}column;//0.50�Œǉ�
	BYTE list_control;//1.40�Œǉ�
};

struct MAP_INFORMATION{
	float x;
	float z;
	float zoom;//�{��
	WORD AreaID;//�G���AID
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

struct POSXYZR{//0.55�Œǉ�
	float x;
	float y;
	float z;
	float radian;
	BYTE flag;
};

struct LINK{//0.59�Œǉ�
	_TCHAR exe[MAX_PATH];
	_TCHAR cmd[0x20];
	_TCHAR link[4][0x20];
	_TCHAR url[4][MAX_PATH];
};

const _TCHAR szColumn[][0x10]={_T("ID"),_T("Name"),_T("State"),_T("Timer"),_T("Type"),_T("Ym"),_T("Note")};//0.57a�Œǉ�

const _TCHAR szVK[][10]={
	_T(""),_T("LButton"),_T("RButton"),_T("?"),_T("MButton"),_T("?"),_T("?"),_T("?"),_T("BkSpace"),_T("Tab"),_T("-"),_T("-"),_T("Clear"),_T("Enter"),_T("-"),_T("-"),
	_T("Shift"),_T("Ctrl"),_T("Alt"),_T("Pause"),_T("CpsLck"),_T("?"),_T("-"),_T("?"),_T("?"),_T("?"),_T("-"),_T("Esc"),_T("?"),_T("?"),_T("?"),_T("?"),
	_T("Space"),_T("PgUp"),_T("PgDn"),_T("End"),_T("Home"),_T("��"),_T("��"),_T("��"),_T("��"),_T("Select"),_T("Print"),_T("?"),_T("PrScn"),_T("Ins"),_T("Del"),_T("?"),
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