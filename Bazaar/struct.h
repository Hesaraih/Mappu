//キャラクター情報構抽出造体
struct INFORMATION{
	WORD id;
	float x;
	float y;
	float z;
	_TCHAR name[0x20];//最大は0x18+'\0'
	BYTE inrange;
	float distance_2;//距離の２乗値
	BYTE bazaar;
	int code;
};

//キャラクター情報の構造体オフセット
struct NPC_MEM_MAP{
	WORD POS_X;
	WORD POS_Y;
	WORD POS_Z;
	WORD ChangedId;
	WORD Name;
	WORD Distance;
	WORD Inrange;
	WORD Bazaar;
};

//wanted構造体
struct WANTED{
	BYTE use;
	_TCHAR szName[0x30];
	int price;
};


struct BAZAAR_DATA{//0.09で追加
	WORD id;
	float fx;
	float fz;
	_TCHAR szTName[0x20];
};