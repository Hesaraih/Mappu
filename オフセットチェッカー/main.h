struct FFXIOFFSET{
	DWORD dwDllAddress;
	DWORD Npcmap;
	DWORD Map;
	DWORD NowSta;
	DWORD Recast;
	DWORD Cast;
	DWORD Viewstyle;
	DWORD Npclist;
	DWORD Activityinfo;
	DWORD Target;
	DWORD Weather;
	DWORD Bazaar;
	//�ȉ�0.03�Œǉ�
	DWORD ItemInfo;
	DWORD MenuHelp;
	DWORD MenuName;
	DWORD Inventory;
	DWORD ItemNameList;
	//�ȉ�0.04�Œǉ�
	DWORD Login;
	DWORD Loginfo;
	DWORD Logserialno;
	DWORD PartyInfo;
	DWORD Status;
	DWORD Server;
	DWORD Pos;
	DWORD Pcmap;
	DWORD Npcmap2;
	DWORD Fish;
	DWORD Jobinfo;
	DWORD Maxhpmpinfo;
	DWORD XIACE_Iteminfo;
	DWORD Inventorycount;
	DWORD Loggedin;
	DWORD Equipinfo;
	DWORD Inventorymax;
	//0.05�ňȉ���ǉ�
	DWORD CursorPos;
	DWORD Order;
	DWORD SendBox;
	DWORD Sender;
	DWORD SendTos;
	DWORD InputMoney;
	DWORD Money;//0.05a�Œǉ�
	DWORD LogAddr;
	DWORD NpcMemRadian;
	DWORD NpcMemChangedId;
	DWORD NpcMemFixedId;
	DWORD NpcMemName;
	DWORD NpcMemViewAddress;
	DWORD NpcMemDistance;
	DWORD NpcMemHP;
	DWORD NpcMemGraphicsId;//0.05a�Œǉ�
	DWORD NpcMemNpcType;
	DWORD NpcMemView;
	DWORD NpcMemBazaar;
	DWORD NpcMemInRange;
	DWORD NpcMemState;
	DWORD NpcMemLastHateId;
	//�ǉ�
	BYTE NpcMemLockOn;
	BYTE NpcMemSubTarget;
#ifdef HOOK_INFO
	DWORD Ffhook_log;//ffhook.dll��log�t�b�N�A�h���X
	DWORD Ffhook_cmd;//ffhook.dll��cmd�t�b�N�A�h���X
	BYTE Ffxihook_log[0x20];//ffxihook_log.dll�̃p�^�[��
	BYTE Ffxihook_cmd[0x20];//ffxihook_cmd.dll�̃p�^�[��
#endif
	DWORD Camera;//0.06�Œǉ�
	//�D�j
	DWORD ArgusInitCode; //INITCODE
	DWORD OwnPosition; //OwnPosition
	DWORD Delivery; //�f���o���[
	DWORD Wait0DeliveryBox; //��z
	DWORD Wait0MogDeliveryBox; //���O�|�X�g
	DWORD Wait0SalesStatus; //�a���m�F
	DWORD Wait0Bid; //���D
	DWORD Wait0Synthesis1; //�����P
	DWORD Wait0Synthesis2; //�����Q
	DWORD WallThrough; //�ǔ���
	DWORD AuctionHouseArea; //�����G���A
	DWORD WideCamera; //�L�扻
	DWORD MeripoMenu; //�����|
	DWORD AuctionHouseMenu; //�������j��
	DWORD ViewMogHouse; //���O�n�m�F
	DWORD ViewMogHouseMenu; //���m���j��
	DWORD ChatLogColumn; //�s���ύX
	DWORD CampaignInfo; //�J���p�j�G
	DWORD OwnAreaID; //OwnAreaID(��E�A�g�o��:AREACHG���ꏏ)
	WORD Bazaar2Meripo; //�o�U�[�˃����|
	WORD Meripo2Bazaar; //�����|�˃o�U�[
	DWORD Flee; //FLEE
	BYTE FleeDefault1; //FLEE�f�t�H���g(1Byte��)
	BYTE FleeDefault2; //FLEE�f�t�H���g(2Byte��)
	BYTE FleeDefault3; //FLEE�f�t�H���g(3Byte��)
	BYTE FleeDefault4; //FLEE�f�t�H���g(4Byte��)
	BYTE FleeDefault5; //FLEE�f�t�H���g(5Byte��)
	BYTE FleeDefault6; //FLEE�f�t�H���g(6Byte��)
	DWORD CharaThrough; //�l����
	BYTE CharaThroughDefault1; //�l�����f�t�H���g(1Byte��)
	BYTE CharaThroughDefault2; //�l�����f�t�H���g(2Byte��)
	BYTE CharaThroughDefault3; //�l�����f�t�H���g(3Byte��)
	BYTE CharaThroughDefault4; //�l�����f�t�H���g(4Byte��)
	BYTE CharaThroughDefault5; //�l�����f�t�H���g(5Byte��)
	DWORD PlusValue; //�v���X�l
	//�D��
	DWORD BackGroundFlag; //BGF
	//�ς؂���
	DWORD MainPuppet; //MAINPUP
	DWORD PuppetTP; //PUPTP
	DWORD AbilityTable; //�A�r�e�[�u��
	//CAST
	DWORD AbilityRecast; //�A�r���L���X�g
	DWORD BlueMagicHold; //���S��
	DWORD MeripoValue; //CAST�̃����|
	DWORD LinkShellName; //�����N�V�F����
	//���̂ق�
	DWORD UtsusemiRecast; //��䃊�L���X�g
	DWORD Alliance1; //�A���P
	DWORD Alliance2; //�A���Q
	DWORD PositionAddress; //FFXiChocobo��<PositionAddress>
	DWORD CharList; //FFXiLoginPoint��CHARLIST
	DWORD XiNPC; //XiNPC��XINPC
	//ChocoboCheckerPlus
	DWORD OtherPC_Bazaar;
	DWORD Paddock;
};


LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
void ShowVersion(HWND hWnd);
int GetFileName(HWND hWnd,_TCHAR* szFileName,int nFileNameSize);
int GetTimeStamp_Update(_TCHAR *sztFileName,SYSTEMTIME *SysTimeUpdate);//0.05�Œǉ�
int Save(_TCHAR *sztFileName,HWND hWnd,HWND hEdit);//0.05�Œǉ�


#ifdef MAINCPP
FFXIOFFSET g_Offset;

#else
extern FFXIOFFSET g_Offset;

#endif