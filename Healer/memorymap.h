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
//+0x2C(0x32)	BYTE AreaID;
//+0x2D(0x33)	BYTE �s��;//0x00?
//+0x2E(0x34)	DWORD �s��;//Fill NULL?
//+0x32(0x38)	DWORD �s��;
//+0x36(0x3C)	DWORD �s��;//Fill NULL?
//+0x3A(0x40)	DWORD �s��;//Fill NULL?
//+0x3E(0x44)	DWORD �s��;//Fill NULL?
//+0x42(0x48)	DWORD �s��;//Fill NULL?
//+0x46(0x4C)	DWORD �s��;//Fill NULL?
//+0x4A(0x50)	DWORD fixed_id;//�����̌Œ�ID
//+0x4E(0x54)	BYTE HPP;
//+0x4F(0x55)	BYTE MPP;
//+0x50(0x56)	BYTE Party;//�p�[�e�B�ɓ����Ă����0x01�������0x00
//+0x51(0x57)	BYTE �s��;
//}NOWSTA[18](20110418����)
//NOWSTA[0]�������ANOWSTA[1-5]���p�[�e�B�ANOWSTA[6-17]���A��?
//(FFXiMain.dll�̐擪�A�h���X+NPCMAP+4Byte*change_id)�Ŏ����̏��
#define NOWSTA_HOSEI		(-0x06)//�J�n�ʒu�������ɈႤ�݂����Ȃ̂ŕ␳
#define NOWSTA_SIZE			0x7C//0.50c�ŕύX
#define NOWSTA_MEM_NAME		0x06
#define NOWSTA_MEM_FIXEDID	0x18//���g�p
#define NOWSTA_MEM_CHANGEID	0x1C
#define NOWSTA_MEM_HP		0x24
#define NOWSTA_MEM_MP		0x28
#define NOWSTA_MEM_TP		0x2C
#define NOWSTA_MEM_HPP		0x30
#define NOWSTA_MEM_MPP		0x31
#define NOWSTA_MEM_AREAID	0x32
#define NOWSTA_MEM_READER	0x38//���g�p
#define NOWSTA_MEM_PARTY	0x7A//0.50c�ŕύX



//NPC���̃������}�b�v
//NPCMAP:NPC���A�h���X���i�[����Ă���z��̐擪�A�h���X
//#define NPC_MEM_POS_X	0x04
//#define NPC_MEM_POS_Y	0x08
//#define NPC_MEM_POS_Z	0x0C
//#define NPC_MEM_RADIAN	0x18//x����ł�(float)���W�A��
//#define NPC_MEM_CHANGEDID	0x74//changedID
//#define NPC_MEM_FIXEDID	0x78//fixedID
//#define NPC_MEM_NAME	0x7C
//#define NPC_MEM_DISTANCE	0xAC//�����ƑΏۂ̋���
//#define NPC_MEM_HPP	0xC4
//#define NPC_MEM_VIEW	0xFD//v0.42�Œǉ��@�G�F�͈͓�0x40�A�͈͊O0x40�A���S0xC5�A����0xC4/0xC5/0xC1�ˏ��ł�0xC*�A�����鎞��0x4*
//#define NPC_MEM_INRANGE	0x115//�͈͓��G0x0F�APC0x1F�ANPC0x57(0x1F��)�B�͈͊Oor����0x00
//#define NPC_MEM_STATE	0x13C//�t���[0x00�A�퓬��0x01�A���S0x03
//#define NPC_MEM_SIZE	0x160//��������ǂݍ��ރT�C�Y
#define NPC_MEM_SIZE	0x200//��������ǂݍ��ރT�C�Y



struct PARTYINFORMATION{
	WORD id;
	float x;
	float y;
	float z;
	float radian;
	_TCHAR name[0x20];//�ő��0x18+'\0'
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
	float distance;//0.17�Œǉ�
	DWORD fixedid;//0.17�Œǉ�
	BYTE state;//0.28�Œǉ�
};


struct INPUTKEYSTATE{//0.28�Œǉ�
	BYTE numpad[10];//����
	BYTE key[26];//����
};

struct FFXIOFFSET{//0.17�ŕύX 0.36�ō\���̖��ǉ�
	BYTE Auto;//0.36�Œǉ�
	DWORD NpcMap;
	DWORD NowSta;
	DWORD Recast;
	DWORD Cast;
	DWORD ViewPoint;//=ViewStyle
	DWORD Target;//0.17�Œǉ�
	DWORD Map;//0.21�Œǉ�
	DWORD NpcList;//0.28�Œǉ�
	DWORD ActivityInfo;//0.28�Œǉ�
	DWORD PosX;//NPCMAP�惁�����}�b�v��POS_X(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD PosY;//NPCMAP�惁�����}�b�v��POS_Y(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD PosZ;//NPCMAP�惁�����}�b�v��POS_Z(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD Radian;//NPCMAP�惁�����}�b�v��RADIAN(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD Name;//NPCMAP�惁�����}�b�v��NAME(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD FixedId;//NPCMAP�惁�����}�b�v��FIXEDID(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD Hpp;//NPCMAP�惁�����}�b�v��HP(Mappu�ł��g�p) 0.43h�Œǉ�
	DWORD Inrange;//NPCMAP�惁�����}�b�v��INRANGE(Mappu�ł��g�p) 0.16�Œǉ�
	DWORD Distance;//NPCMAP�惁�����}�b�v��DISTANCE(Mappu�ł��g�p) 0.17�Œǉ�
	DWORD State;//NPCMAP�惁�����}�b�v��STATE(Mappu�ł��g�p) 0.28�Œǉ�
};