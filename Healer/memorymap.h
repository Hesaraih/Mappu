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
//+0x2C(0x32)	BYTE AreaID;
//+0x2D(0x33)	BYTE 不明;//0x00?
//+0x2E(0x34)	DWORD 不明;//Fill NULL?
//+0x32(0x38)	DWORD 不明;
//+0x36(0x3C)	DWORD 不明;//Fill NULL?
//+0x3A(0x40)	DWORD 不明;//Fill NULL?
//+0x3E(0x44)	DWORD 不明;//Fill NULL?
//+0x42(0x48)	DWORD 不明;//Fill NULL?
//+0x46(0x4C)	DWORD 不明;//Fill NULL?
//+0x4A(0x50)	DWORD fixed_id;//自分の固定ID
//+0x4E(0x54)	BYTE HPP;
//+0x4F(0x55)	BYTE MPP;
//+0x50(0x56)	BYTE Party;//パーティに入っていると0x01抜けると0x00
//+0x51(0x57)	BYTE 不明;
//}NOWSTA[18](20110418現在)
//NOWSTA[0]が自分、NOWSTA[1-5]がパーティ、NOWSTA[6-17]がアラ?
//(FFXiMain.dllの先頭アドレス+NPCMAP+4Byte*change_id)で自分の情報
#define NOWSTA_HOSEI		(-0x06)//開始位置が微妙に違うみたいなので補正
#define NOWSTA_SIZE			0x7C//0.50cで変更
#define NOWSTA_MEM_NAME		0x06
#define NOWSTA_MEM_FIXEDID	0x18//未使用
#define NOWSTA_MEM_CHANGEID	0x1C
#define NOWSTA_MEM_HP		0x24
#define NOWSTA_MEM_MP		0x28
#define NOWSTA_MEM_TP		0x2C
#define NOWSTA_MEM_HPP		0x30
#define NOWSTA_MEM_MPP		0x31
#define NOWSTA_MEM_AREAID	0x32
#define NOWSTA_MEM_READER	0x38//未使用
#define NOWSTA_MEM_PARTY	0x7A//0.50cで変更



//NPC情報のメモリマップ
//NPCMAP:NPC情報アドレスが格納されている配列の先頭アドレス
//#define NPC_MEM_POS_X	0x04
//#define NPC_MEM_POS_Y	0x08
//#define NPC_MEM_POS_Z	0x0C
//#define NPC_MEM_RADIAN	0x18//x軸基準での(float)ラジアン
//#define NPC_MEM_CHANGEDID	0x74//changedID
//#define NPC_MEM_FIXEDID	0x78//fixedID
//#define NPC_MEM_NAME	0x7C
//#define NPC_MEM_DISTANCE	0xAC//自分と対象の距離
//#define NPC_MEM_HPP	0xC4
//#define NPC_MEM_VIEW	0xFD//v0.42で追加　敵：範囲内0x40、範囲外0x40、死亡0xC5、消滅0xC4/0xC5/0xC1⇒消滅は0xC*、見える時は0x4*
//#define NPC_MEM_INRANGE	0x115//範囲内敵0x0F、PC0x1F、NPC0x57(0x1F等)。範囲外or自分0x00
//#define NPC_MEM_STATE	0x13C//フリー0x00、戦闘中0x01、死亡0x03
//#define NPC_MEM_SIZE	0x160//メモリを読み込むサイズ
#define NPC_MEM_SIZE	0x200//メモリを読み込むサイズ



struct PARTYINFORMATION{
	WORD id;
	float x;
	float y;
	float z;
	float radian;
	_TCHAR name[0x20];//最大は0x18+'\0'
	BYTE hpp;
	BYTE mpp;
	DWORD hp;
	DWORD mp;
	DWORD tp;
	BYTE areaid;
	int code;
	int priority;
	int listnum;
	BYTE inrange;
	float distance;//0.17で追加
	DWORD fixedid;//0.17で追加
	BYTE state;//0.28で追加
};


struct INPUTKEYSTATE{//0.28で追加
	BYTE numpad[10];//数字
	BYTE key[26];//文字
};

struct FFXIOFFSET{//0.17で変更 0.36で構造体名追加
	BYTE Auto;//0.36で追加
	DWORD NpcMap;
	DWORD NowSta;
	DWORD Recast;
	DWORD Cast;
	DWORD ViewPoint;//=ViewStyle
	DWORD Target;//0.17で追加
	DWORD Map;//0.21で追加
	DWORD NpcList;//0.28で追加
	DWORD ActivityInfo;//0.28で追加
	DWORD PosX;//NPCMAP先メモリマップでPOS_X(Mappuでも使用) 0.43hで追加
	DWORD PosY;//NPCMAP先メモリマップでPOS_Y(Mappuでも使用) 0.43hで追加
	DWORD PosZ;//NPCMAP先メモリマップでPOS_Z(Mappuでも使用) 0.43hで追加
	DWORD Radian;//NPCMAP先メモリマップでRADIAN(Mappuでも使用) 0.43hで追加
	DWORD Name;//NPCMAP先メモリマップでNAME(Mappuでも使用) 0.43hで追加
	DWORD FixedId;//NPCMAP先メモリマップでFIXEDID(Mappuでも使用) 0.43hで追加
	DWORD Hpp;//NPCMAP先メモリマップでHP(Mappuでも使用) 0.43hで追加
	DWORD Inrange;//NPCMAP先メモリマップでINRANGE(Mappuでも使用) 0.16で追加
	DWORD Distance;//NPCMAP先メモリマップでDISTANCE(Mappuでも使用) 0.17で追加
	DWORD State;//NPCMAP先メモリマップでSTATE(Mappuでも使用) 0.28で追加
};