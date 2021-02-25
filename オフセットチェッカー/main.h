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
	//以下0.03で追加
	DWORD ItemInfo;
	DWORD MenuHelp;
	DWORD MenuName;
	DWORD Inventory;
	DWORD ItemNameList;
	//以下0.04で追加
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
	//0.05で以下を追加
	DWORD CursorPos;
	DWORD Order;
	DWORD SendBox;
	DWORD Sender;
	DWORD SendTos;
	DWORD InputMoney;
	DWORD Money;//0.05aで追加
	DWORD LogAddr;
	DWORD NpcMemRadian;
	DWORD NpcMemChangedId;
	DWORD NpcMemFixedId;
	DWORD NpcMemName;
	DWORD NpcMemViewAddress;
	DWORD NpcMemDistance;
	DWORD NpcMemHP;
	DWORD NpcMemGraphicsId;//0.05aで追加
	DWORD NpcMemNpcType;
	DWORD NpcMemView;
	DWORD NpcMemBazaar;
	DWORD NpcMemInRange;
	DWORD NpcMemState;
	DWORD NpcMemLastHateId;
	//追加
	BYTE NpcMemLockOn;
	BYTE NpcMemSubTarget;
#ifdef HOOK_INFO
	DWORD Ffhook_log;//ffhook.dllのlogフックアドレス
	DWORD Ffhook_cmd;//ffhook.dllのcmdフックアドレス
	BYTE Ffxihook_log[0x20];//ffxihook_log.dllのパターン
	BYTE Ffxihook_cmd[0x20];//ffxihook_cmd.dllのパターン
#endif
	DWORD Camera;//0.06で追加
	//泥男
	DWORD ArgusInitCode; //INITCODE
	DWORD OwnPosition; //OwnPosition
	DWORD Delivery; //デリバリー
	DWORD Wait0DeliveryBox; //宅配
	DWORD Wait0MogDeliveryBox; //モグポスト
	DWORD Wait0SalesStatus; //預託確認
	DWORD Wait0Bid; //入札
	DWORD Wait0Synthesis1; //合成１
	DWORD Wait0Synthesis2; //合成２
	DWORD WallThrough; //壁抜け
	DWORD AuctionHouseArea; //競売エリア
	DWORD WideCamera; //広域化
	DWORD MeripoMenu; //メリポ
	DWORD AuctionHouseMenu; //競売メニュ
	DWORD ViewMogHouse; //モグハ確認
	DWORD ViewMogHouseMenu; //モ確メニュ
	DWORD ChatLogColumn; //行数変更
	DWORD CampaignInfo; //カンパニエ
	DWORD OwnAreaID; //OwnAreaID(眼・アトバン:AREACHGも一緒)
	WORD Bazaar2Meripo; //バザー⇒メリポ
	WORD Meripo2Bazaar; //メリポ⇒バザー
	DWORD Flee; //FLEE
	BYTE FleeDefault1; //FLEEデフォルト(1Byte目)
	BYTE FleeDefault2; //FLEEデフォルト(2Byte目)
	BYTE FleeDefault3; //FLEEデフォルト(3Byte目)
	BYTE FleeDefault4; //FLEEデフォルト(4Byte目)
	BYTE FleeDefault5; //FLEEデフォルト(5Byte目)
	BYTE FleeDefault6; //FLEEデフォルト(6Byte目)
	DWORD CharaThrough; //人抜け
	BYTE CharaThroughDefault1; //人抜けデフォルト(1Byte目)
	BYTE CharaThroughDefault2; //人抜けデフォルト(2Byte目)
	BYTE CharaThroughDefault3; //人抜けデフォルト(3Byte目)
	BYTE CharaThroughDefault4; //人抜けデフォルト(4Byte目)
	BYTE CharaThroughDefault5; //人抜けデフォルト(5Byte目)
	DWORD PlusValue; //プラス値
	//泥道
	DWORD BackGroundFlag; //BGF
	//ぱぺすて
	DWORD MainPuppet; //MAINPUP
	DWORD PuppetTP; //PUPTP
	DWORD AbilityTable; //アビテーブル
	//CAST
	DWORD AbilityRecast; //アビリキャスト
	DWORD BlueMagicHold; //青魔拘束
	DWORD MeripoValue; //CASTのメリポ
	DWORD LinkShellName; //リンクシェル名
	//そのほか
	DWORD UtsusemiRecast; //空蝉リキャスト
	DWORD Alliance1; //アラ１
	DWORD Alliance2; //アラ２
	DWORD PositionAddress; //FFXiChocoboの<PositionAddress>
	DWORD CharList; //FFXiLoginPointのCHARLIST
	DWORD XiNPC; //XiNPCのXINPC
	//ChocoboCheckerPlus
	DWORD OtherPC_Bazaar;
	DWORD Paddock;
};


LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
void ShowVersion(HWND hWnd);
int GetFileName(HWND hWnd,_TCHAR* szFileName,int nFileNameSize);
int GetTimeStamp_Update(_TCHAR *sztFileName,SYSTEMTIME *SysTimeUpdate);//0.05で追加
int Save(_TCHAR *sztFileName,HWND hWnd,HWND hEdit);//0.05で追加


#ifdef MAINCPP
FFXIOFFSET g_Offset;

#else
extern FFXIOFFSET g_Offset;

#endif