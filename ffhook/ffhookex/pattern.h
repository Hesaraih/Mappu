#pragma once
// アタッチするアドレス検索用パターン
#define MAX_PATTERN		256

#if LOG_HOOK == 1
static PIHOOK	pILOG_HOOK = NULL;
TCHAR pattern_log[MAX_PATTERN] = _T("E8........8B470C8B5424184089470C84DB742E");
void (__stdcall *pNextLogHook)(char*,BYTE *,int,int);
#endif

#if CMD_HOOK == 1 || CMD_PUT == 1
static PIHOOK	pICMD_HOOK = NULL;
TCHAR pattern_cmd[MAX_PATTERN] = _T("............8B8C24080400005556578D84241002000068000200005033ED51");
void (*pNextCmdHook)(char*, int);
#endif

#if KILLEFFECT == 1
void (*pKillEffect)(int);
TCHAR pattern_killeffect[] = _T("6A006A0068F1000000E8........83C40C85C0743456668B7424086A006A08");
#endif

#if MERGEITEM == 1
void (*pMergeItem)(int no);
TCHAR pattern_mergeitem[] = _T("A1........85C074126A006A006A3AE8........83C40C85C07503");
#endif

#if MACROEXE == 1
void (__stdcall *pMacro)(int tyo, int no);
TCHAR pattern_macroexe[] = _T("..........563C018BF175188A5C24108B4E040FBEC350E8........");
#endif

TCHAR pattern_job[] = _T("F605..........74..8A0Dxxxxxxxx80F9");

TCHAR pattern_ptinfo[] = _T("84C974098D80xxxxxxxxC2040033C0C2..00");
#define ADJUST_PTINFO		6

TCHAR pattern_abitab[] = _T("33C0A0........8A0445xxxxxxxxC390E8");

TCHAR pattern_magtab[] = _T("57B9........33C0BFxxxxxxxxF3AB66AB5FC3");

TCHAR pattern_aomahold[] = _T("7D..66833Dxxxxxxxx007E095F5EB8");

TCHAR pattern_npcmap[] = _T("8B7C2424BExxxxxxxx8B0E85C90F84........8B81");

TCHAR pattern_targetinfo[] = _T("8B0Dxxxxxxxx3BCB74..E8........8B15........385A..74..8B0D");
#define OFFSET_LASTST	0x005c
#define OFFSET_STARGET	0x0078

TCHAR pattern_weather[] = _T("A1xxxxxxxx25........50E8........83C40484C075..8B0D");

TCHAR pattern_lsname[] = _T("8B442404506A1568xxxxxxxxE8........83C40CB8........C3");

TCHAR pattern_flist[] = _T("C744241C........BB........A1xxxxxxxx8DB406pppppppp5655E8........83C4");

TCHAR pattern_meripo[] = _T("A0........24..F6D81BC025xxxxxxxxC3");

TCHAR pattern_icon[] = _T("6A006A085066897004E8........83C40CB8xxxxxxxx663930");

TCHAR pattern_bnum[] = _T("8B0Dxxxxxxxx6A016A00E8........A1........8B0D........50E8");

TCHAR pattern_act[] = _T("B9xxxxxxxxE8........0FBF4C24..84C074..33C0894424");
#define ADJUST_ACT			0x68

TCHAR pattern_login[] = _T("8B442404680000803FA3xxxxxxxxE8........680000803F");

TCHAR pattern_item[] = _T("578B3Dxxxxxxxx6A01B9........8B470850E8........8BCFE8");

TCHAR pattern_equip[] = _T("566A0168........57893DxxxxxxxxE8........56E8........83C414C605");

TCHAR pattern_item_adj[] = _T("8D9481xxxxxxxxB9........33C08BFA........");

TCHAR pattern_bag_adj[] = _T("8B4C24..8A8408xxxxxxxxC3");

TCHAR pattern_equip_adj[] = _T("8A48..8A58..8BD18B4C24..889CD1xxxxxxxx33D2");

TCHAR pattern_ownpos[] = _T("D866..D905xxxxxxxxD866..D9C0D8C9D9C2D8CB");

TCHAR pattern_choose[] = _T("3B05xxxxxxxx75..8B116A..FF52..EB..8B01");
#define ADJUST_CHOOSE		0x4c

TCHAR pattern_lockon[] = _T("516A..8B1050FF523C8B4424..8B4C24..3BC875218B15xxxxxxxx8917");
#define ADJUST_LOCKON		0x2a

TCHAR pattern_petid[] = _T("000000BFxxxxxxxxF3..5FB0015EC381E1");
#define ADJUST_PETID		0x08

char Addres[][32] = {
	"ベースアドレス",
	"ジョブ情報",
	"パーティ情報",
	"NPCMAP",
	"TARGETINFO",
	"アビテーブル",
	"魔法リキャスト",
	"青魔拘束",
	"天候情報",
	"リンクシェル名",
	"アイコンテーブル",
	"メリポ",
	"アクション",
	"ターゲットアイテム",
	"フレンドリスト",
	"ログインフラグ",
	"持ち物数",
	"アイテムベース",
	"装備情報",
	"アイテム情報",
	"エリチェン情報",
	"かばん",
	"OWNPOS",
	"選択",
	"キャスト",
	"ペットID",
	"ロックオン",
	"終端"
};
