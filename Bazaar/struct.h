//�L�����N�^�[���\���o����
struct INFORMATION{
	WORD id;
	float x;
	float y;
	float z;
	_TCHAR name[0x20];//�ő��0x18+'\0'
	BYTE inrange;
	float distance_2;//�����̂Q��l
	BYTE bazaar;
	int code;
};

//�L�����N�^�[���̍\���̃I�t�Z�b�g
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

//wanted�\����
struct WANTED{
	BYTE use;
	_TCHAR szName[0x30];
	int price;
};


struct BAZAAR_DATA{//0.09�Œǉ�
	WORD id;
	float fx;
	float fz;
	_TCHAR szTName[0x20];
};