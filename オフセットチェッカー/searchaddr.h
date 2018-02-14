#define SEARCH_BLOCK				0x1000
#define SEARCH_RANGE				0x400000
#define PATTERN_MAX_RANGE			100
#define NOWSTA_HOSEI				0x06	//0.05で追加
#define NOWSTA_MEM_CHANGEID			0x16	//0.05で追加
#define ACTIVITY_INFO_HOSEI			0x08	//0.05で追加
#define NPCMAP2_HOSEI				0x1C00	//0.05で追加
#define INVENTORY_INFO_HOSEI		0x6C	//0.05で追加変更(以前までは0x622C)
#define FISH_INFO_HOSEI				0x18
#define XIACE_ITEM_INFO_HOSEI		0x16C	//0.08で変更
#define NPC_MEM_BAZAAR_HOSEI		0x09
#define MONEY_HOSEI					0x04	//0.05aで追加
#define SendBox_HOSEI				0xF8D0	//位置からマイナス
#define OwnPosition_HOSEI			0x04
#define Delivery_HOSEI				0x04
#define Wait0MogDeliveryBox_HOSEI	0x06
#define Wait0SalesStatus_HOSEI		0x04
#define Wait0Synthesis1_HOSEI		0x04
#define Wait0Synthesis2_HOSEI		0x03
#define AuctionHouseArea_HOSEI		0x10
#define WideCamera_HOSEI			0x01
#define MeripoMenu_HOSEI			0x23
#define AuctionHouseMenu_HOSEI		0x08
#define ViewMogHouse_HOSEI			0x18
#define ViewMogHouseMenu_HOSEI		0x0C
#define ChatLogColumn_HOSEI			0x0C
#define Bazaar2Meripo_HOSEI			0x0C
#define Meripo2Bazaar_HOSEI			0x0C
#define Flee_HOSEI					0x04
#define CharaThrough_HOSEI			0x04
#define PlusValue_HOSEI				0x2E
#define BackGroundFlag_HOSEI		0x1D
#define MainPuppet_HOSEI			0x4938
#define AbilityRecast_HOSEI			0x7C
#define UtsusemiRecast_HOSEI		0x2A4	//2Btye*338(空蝉の術：壱のID)
#define Alliance1_HOSEI				0x210
#define Alliance2_HOSEI				0x420
#define CharList_HOSEI				0x08
#define NpcMemLockOn_HOSEI			0x04
#define NpcMemSubTarget_HOSEI		0x14
#define XiNPC_HOSEI					0x04

int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern,DWORD dwStartAddress = 0,DWORD dwSearchRange = SEARCH_RANGE);//0.05で変更
DWORD SearchAddress(BYTE *base,const BYTE *pattern,DWORD dwReadAddress,DWORD dwDllAddress);


/*
サーチ方法
うさみみハリケーンでハードウェアブレークポイントをオフセットに対して発行
EIP付近を逆アセンブリして固定と思われるマシン語を抽出
不定は「.」オフセットは｢x｣

便利なツール・ソフトを作るスレに投稿した文
---ここから---
オフセットチェッカーは、
VUがあったとしても、ある関数のオフセット部にアクセスする機械語列(バイナリ列)が変更されていない。という条件の下で機械語列からオフセットを抽出しています
※VUでバイナリ列に変更があり利用できなくなる可能性もありますので、過去のdllでも動作する最適なバイナリ列を準備する必要があります(それでも今後変更になる可能性はあります)

実際にサーチしているのはGetOffsetAddress()の部分です
1)プロセスにロードされたFFXiMain.dllをバイナリ列サーチする(「.」はアドレス値など不定となるもの、「x」はオフセット+ベースアドレスの値)
2)1)でマッチしたxの場所をdwOffsetにコピーしてベースアドレスを引く

この1)2)の動作を各オフセットに対して繰り返しているだけです

バイナリ列の準備方法は(うさみみハリケーンの場合)
0)うさみみハリケーンのプロセス選択をpol.exeに設定
1)FFXiMain.dllのベースアドレス+現在のOFFSET値にハードウェアブレークポイントを設定する
1.1)デバッグ→ブレークポイントの挿入/削除→デバッギー操作内のアタッチ
1.2)ブレークポイントの挿入/削除→ブレークポイント設定(アドレスは16進数で入力)内のハードウェアブレークポイントにベースアドレス+オフセット値の入力
1.3)アクセスの種類を最適な物に設定
1.4)設定する
2)出力されたデータの中から適当なものを一つ選択し、EIPと書かれている数値をメモする
※出力されたデータは設定したアドレス(ベースアドレス+オフセット値)に対してアクセスがあった時のメモリの状態ですEIPはその実行アドレスを指します
3)2)でメモしたEIP付近を逆アセンブルする
3.1)メモしたEIPのアドレスに移動する
3.2)EIP付近の前後0x20を選択状態にする
3.3)デバッグ→選択範囲を逆アセンブル→逆アセンブル実行
4)バイナリ列を作成する
4.1)逆アセンブルコードの列の中からベースアドレス+オフセット値の値がある付近を見つける
4.2)その前後を適当な長さとなるようにHexダンプ値をメモしていく
4.3)メモしたHexダンプ値で不定となりそうな物を「.」に変える(アドレス値とか)
4.4)ベースアドレス+オフセット値(リトルエンディアンです)を「x」に変える
5)作成したバイナリ列をサーチしてみる

2)でヒットするデータは多いので2)～5)を数回繰り返す必要があります
バイナリ列は変更されたり、たまたま一致しただけだったりする可能性があるので過去のFFXiMain.dllやテストサーバーでもサーチできるか確認した方が良いです
---ここまで---

追記0.04
直接サーチ出来ないところは2回に分けて計算したりして求めています
スマートな方法が解らない為、詳しい方は教えてくれると助かります
*/


//0.05bでsearchaddr.cから移動
const BYTE constNpcmapPattern[PATTERN_MAX_RANGE]       = "83EC0C568BF18B46048B0485xxxxxxxx85C0";//85C00F84........B9xxxxxxxx894C24
const BYTE constMapPattern[PATTERN_MAX_RANGE]          = "895E38C786........FFFFFFFF895E3C891Dxxxxxxxx8BCE899E";
//NowstaはNOWSTA_HOSEIを実施
const BYTE constNowstaPattern[PATTERN_MAX_RANGE]       = "56578BF58D....8.............8D....xxxxxxxx";//変更
const BYTE constRecastPattern[PATTERN_MAX_RANGE]       = "56BExxxxxxxx66833E007E13E8";
const BYTE constCastPattern[PATTERN_MAX_RANGE]         = "8BF083C41085F674..A1xxxxxxxx85C0";//変更
const BYTE constVeiwstylePattern[PATTERN_MAX_RANGE]    = "1Dxxxxxxxx881D........881D..................8B0D";//変更
const BYTE constNpclistPattern[PATTERN_MAX_RANGE]      = "33C05685D2577E198B3Dxxxxxxxx8B7424";
//Activityinfoは近い値から+0x08しているだけ(参考値)
const BYTE constActivityinfoPattern[PATTERN_MAX_RANGE] = "C646....8B4424..85C0758FA0xxxxxxxxBB";//変更
const BYTE constTargetPattern[PATTERN_MAX_RANGE]       = "4424....5250E8........8B0Dxxxxxxxx";//変更
const BYTE constWeatherPattern[PATTERN_MAX_RANGE]      = "8BC883C40485C90F84........66A1xxxxxxxx663D";
const BYTE constBazaarPattern[PATTERN_MAX_RANGE]       = "485683F8058BF10F87........FF2485........66837C240C11757CA1xxxxxxxx";

//倉庫番
const BYTE constItemInfoPattern[PATTERN_MAX_RANGE]     = "......EB09..................A1xxxxxxxx85C0";//変更? "894E..EB098B15........8956..A1xxxxxxxx85C0";
const BYTE constMenuHelpPattern[PATTERN_MAX_RANGE]     = "E8........84C00F84........A1xxxxxxxx8D48..E8........84C0";
const BYTE constMenuNamePattern[PATTERN_MAX_RANGE]     = "85C0............................8B..xxxxxxxx6A02";//変更
//Inventoryは直接サーチできず[Inventory1] + Inventory2 + INVENTORY_INFO_HOSEI
const BYTE constInventory1Pattern[PATTERN_MAX_RANGE]   = "33D0F6C20374............................8B0Dpppppppp8B..";//変更
const BYTE constInventory2Pattern[PATTERN_MAX_RANGE]   = "33D0F6C20374............................8B0D........8B..pppppppp";//変更
const BYTE constItemNameListPattern[PATTERN_MAX_RANGE] = "8B5424..85D2741833C9B8xxxxxxxx3910";

//RealTimeLogGets
const BYTE constLoginPattern[PATTERN_MAX_RANGE]        = "8B0D........E8........A1xxxxxxxx85C07420";
const BYTE constLogInfoPattern[PATTERN_MAX_RANGE]      = "030000................8B0DxxxxxxxxE8";//変更
const BYTE constLogSerialNoPattern[PATTERN_MAX_RANGE]  = "74..8B0C85........518B0DxxxxxxxxE8";//変更
//PT情報は直接サーチ出来ず[Partyinfo1] + Partyinfo2
const BYTE constPartyinfo1Pattern[PATTERN_MAX_RANGE]   = "8B3490B9........33C08BFE8935pppppppp6A";//Money1と同じ
const BYTE constPartyinfo2Pattern[PATTERN_MAX_RANGE]   = "57B9........33C08DBApppppppp68";//0.05aで追加
const BYTE constStatusPattern[PATTERN_MAX_RANGE]       = "33DBF3ABB91000000083C8FFBFxxxxxxxx891D";
//サーバーは直接サーチ出来ず[Server1] + Server2
const BYTE constServer1Pattern[PATTERN_MAX_RANGE]      = "8B0Dpppppppp5733FF8B91........8D04D5";//0.05でpに変更
const BYTE constServer2Pattern[PATTERN_MAX_RANGE]      = "2BC28D14808D8491pppppppp81C1";//0.05でpに変更
//Pos(位置)は直接サーチ出来ず[Pos1] + Pos2
const BYTE constPos1Pattern[PATTERN_MAX_RANGE]         = "8B54240CBFppppppppB9........33C0893D";//0.05でpに変更
const BYTE constPos2Pattern[PATTERN_MAX_RANGE]         = "8B4C2404D94424088988pppppppp8B15";//0.05でpに変更
const BYTE constPcmapPattern[PATTERN_MAX_RANGE]        = "85C00F84........B9xxxxxxxx894C24";
//NPCMAP2は直接サーチ出来ずNpcmap + 0x1C00
//const BYTE constNpcmap2Pattern[PATTERN_MAX_RANGE]        = "";

//XIACE
const BYTE constFishPattern[PATTERN_MAX_RANGE]         = "8986........C605........00............C705xxxxxxxx";//変更
const BYTE constJobInfoPattern[PATTERN_MAX_RANGE]      = "E8........83C4..33C950A1xxxxxxxx8ACC";//変更
const BYTE constMaxHpMpInfoPattern[PATTERN_MAX_RANGE]  = "51..........50E8........8B0Dxxxxxxxx";//変更
//XIACE用ITEM_INFOは倉庫番のItemInfoにXIACE_ITEM_INFO_HOSEIを足した名称は同じであるが別もの(パターンは違うところから取得)
const BYTE constXIACEItemInfoPattern[PATTERN_MAX_RANGE] = "33C9B8xxxxxxxx391074..05........";
const BYTE constXIACEItemInfoPattern2[PATTERN_MAX_RANGE] = "33C9B8........391074..05pppppppp";//0.08で追加 20130709VU:ITEM_INFOの構造体サイズが欲しいときはコレを使う0x148→0x16c
const BYTE constInventoryCountPattern[PATTERN_MAX_RANGE] = "83F86074..83F870............E8........8B0Dxxxxxxxx";//変更
//EQUIP_INFOは直接サーチ出来ず[EquipInfo1] + EquipInfo2
const BYTE constEquipInfo1Pattern[PATTERN_MAX_RANGE]    = "A1pppppppp8D88........8988";//0.05でpに変更
const BYTE constEquipInfo2Pattern[PATTERN_MAX_RANGE]    = "A1........8D88pppppppp8988";//0.05でpに変更
//Inventorymaxは直接サーチ出来ず[Inventorymax1] + Inventorymax2
const BYTE constInventorymax1Pattern[PATTERN_MAX_RANGE] = "90A1pppppppp85C075030CFFC3";//0.05でpに変更
const BYTE constInventorymax2Pattern[PATTERN_MAX_RANGE] = "C38B4C2404......ppppppppC390";//変更
//LOGGED_INは正規の位置不明の為IN=0、OUT≠0となるところをでっちあげ
const BYTE constLoggedinPattern[PATTERN_MAX_RANGE]     = "EB06891Dxxxxxxxx576A3CE8";//変更

//FFXI小道具//0.05で追加
const BYTE constCursorPosPattern[PATTERN_MAX_RANGE]		= "8B51..85D274..3B05xxxxxxxx75..";
const BYTE constOrderPattern[PATTERN_MAX_RANGE]			= "6A006A006A00E8........A1xxxxxxxx6A00";//変更? "EB06891D........391Dxxxxxxxx75226A70";
const BYTE constSenderPattern[PATTERN_MAX_RANGE]		= "6A008B0DxxxxxxxxE8BF120000";
const BYTE constSendTosPattern[PATTERN_MAX_RANGE]		= "8B0Dxxxxxxxx85C974..52E8DDE00100C3";
const BYTE constInputMoneyPattern[PATTERN_MAX_RANGE]	= "8935xxxxxxxxEB06891D........391D........75376A14";
//8935:MOV xx,ESI  EB06:JMP SHORT 891D:MOV ..,EBX 391D:CMP ..,EBX 7537:JNZ SHORT 6A14:PUSH 14
//Money(所持金)は[Money1] + Money2 + MONEY_HOSEI
const BYTE constMoney1Pattern[PATTERN_MAX_RANGE]		= "8B3490B9........33C08BFE8935pppppppp6A";//Partyinfo1と同じ
const BYTE constMoney2Pattern[PATTERN_MAX_RANGE]		= "8B15........57B9........33C08DBAppppppppBA";

//Fish改
const BYTE constLogAddrPattern[PATTERN_MAX_RANGE]		= "BDxxxxxxxx8B4C24..8D54244C............528D4C24";//変更

//NPC_MEM
const BYTE constNpcMemRadianPattern[PATTERN_MAX_RANGE]	= "50408951..8B50..8951..8B50..8951pp8B50..8951..8B40..8941..C3";
const BYTE constNpcMemChangedIdPattern[PATTERN_MAX_RANGE]	= "8179pp........C0C3..90";
const BYTE constNpcMemFixedIdPattern[PATTERN_MAX_RANGE]	= "C1E9..F6C1..74..8B50pp8B4E..3BD175";
const BYTE constNpcMemNamePattern[PATTERN_MAX_RANGE] = "568B7424..578D78ppB9........F3A55FC680";
const BYTE constNpcMemViewAddressPattern[PATTERN_MAX_RANGE] = "3BD175..8B80pppppppp5E83C4..C3";
const BYTE constNpcMemDistancePattern[PATTERN_MAX_RANGE] = "D99EppppppppDDD8DDD8E8";
const BYTE constNpcMemHpPattern[PATTERN_MAX_RANGE] = "8B4E..8B57..8A87pppppppp3BCA884424..74";
const BYTE constNpcMemGraphicsIdPattern[PATTERN_MAX_RANGE] = "663988pppppppp74..8088..........668988";
const BYTE constNpcMemNpcTypePattern[PATTERN_MAX_RANGE] = "85C074..8B88ppppppppC1E9..F6C1..74";
//const BYTE constNpcMemViewPattern[PATTERN_MAX_RANGE] = "";
const BYTE constNpcMemBazaarPattern[PATTERN_MAX_RANGE] = "8.C00F84........8B8.ppppppppC1E9..F6C1..0F8.";//変更
const BYTE constNpcMemInRangePattern[PATTERN_MAX_RANGE] = "C681pppppppp00668B56..8B0495........8B88";
const BYTE constNpcMemStatePattern[PATTERN_MAX_RANGE] = "C2....8B86pppppppp8BCE....";//変更?  "FF2495........8B88pppppppp83C1..83F9..77";
const BYTE constNpcMemLastHateIdPattern[PATTERN_MAX_RANGE] = "A80174..8B86pppppppp85C07428";//変更? "F686..........0F84........8B86pppppppp85C00F84";
//LockOnとSubTargetはターゲット構造体を基準に検索する
const BYTE constNpcMemLockOnSearchPattern[PATTERN_MAX_RANGE] = "FFFFFFFF";

#ifdef HOOK_INFO
//ffhookの置換アドレス
const BYTE constFfhook_logPattern[PATTERN_MAX_RANGE]   = "E8........8B470C8B5424184089470C84DB742E";
const BYTE constFfhook_cmdPattern[PATTERN_MAX_RANGE]   = "............8B8C24080400005556578D84241002000068000200005033ED51";//変更
#endif

const BYTE constCameraPattern[PATTERN_MAX_RANGE]       = "89.424..E8........8B0D........68xxxxxxxx6A";//変更

//泥男
const BYTE constArgusInitCodePattern[PATTERN_MAX_RANGE] = "C64107048B510433C081E2000000FF89"; //INITCODE
const BYTE constOwnPositionPattern[PATTERN_MAX_RANGE] = "83EC10568BF1B9xxxxxxxx"; //OwnPosition
const BYTE constDeliveryPattern[PATTERN_MAX_RANGE] = "C40484C075078B44240CF7D8C3A1"; //+0x4 デリバリー
const BYTE constWait0DeliveryBoxPattern[PATTERN_MAX_RANGE] = "75508B0D"; //宅配
const BYTE constWait0MogDeliveryBoxPattern[PATTERN_MAX_RANGE] = "5EC3807E0C0175378B0D"; //+0x6 モグポスト
const BYTE constWait0SalesStatusPattern[PATTERN_MAX_RANGE] = "46063C0175218B0D"; //+0x4 預託確認
const BYTE constWait0BidPattern[PATTERN_MAX_RANGE] = "81C2B40000006689"; //入札
const BYTE constWait0Synthesis1Pattern[PATTERN_MAX_RANGE] = "000085C97403"; //+0x4 合成１
const BYTE constWait0Synthesis2Pattern[PATTERN_MAX_RANGE] = "FF84C075178B0D"; //+0x3 合成2
const BYTE constWallThroughPattern[PATTERN_MAX_RANGE] = "0F84D30300008A86"; //壁抜け
const BYTE constWideCameraPattern[PATTERN_MAX_RANGE] = "1000007243"; //+0x1 広域化
const BYTE constMeripoMenuPattern[PATTERN_MAX_RANGE] = "EEFF........6A0068........E8........83C4........EEFF........6A......"; //+0x23 メリポ
const BYTE constAuctionHouseMenuPattern[PATTERN_MAX_RANGE] = "2573203a20000000"; //+0x8 競売メニュ
const BYTE constViewMogHousePattern[PATTERN_MAX_RANGE] = "E8........E8........E8................6A..6A..68"; //+0x18 モグハ確認
const BYTE constViewMogHouseMenuPattern[PATTERN_MAX_RANGE] = "636F6D6D656E752000000000"; //+0xC モ確メニュ
const BYTE constChatLogColumnPattern[PATTERN_MAX_RANGE] = "........9380000005022000"; //+0xC 行数変更
const BYTE constCampaignInfoPattern[PATTERN_MAX_RANGE] = "48..88....83C0203D..........EDB8xxxxxxxx"; //カンパニエ
const BYTE constOwnAreaIDPattern[PATTERN_MAX_RANGE] = "7428568B35xxxxxxxx"; //OwnAreaID
const BYTE constBazaar2MeripoPattern[PATTERN_MAX_RANGE] = "6D6572697463617400000000"; //+0xC &0xFFFF バザー⇒メリポ
const BYTE constMeripo2BazaarPattern[PATTERN_MAX_RANGE] = "6D63726D656E752000000000"; //+0xC &0xFFFF メリポ⇒バザー
const BYTE constFleePattern[PATTERN_MAX_RANGE] = "00008A561D"; //-0x4 FLEE
const BYTE constFleeDefaultPattern1[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0zz99580100008A561D"; //FLEEデフォルト(1Byte目)
const BYTE constFleeDefaultPattern2[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D9zz580100008A561D"; //FLEEデフォルト(2Byte目)
const BYTE constFleeDefaultPattern3[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D999zz0100008A561D"; //FLEEデフォルト(3Byte目)
const BYTE constFleeDefaultPattern4[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D99958zz00008A561D"; //FLEEデフォルト(4Byte目)
const BYTE constFleeDefaultPattern5[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D9995801zz008A561D"; //FLEEデフォルト(5Byte目)
const BYTE constFleeDefaultPattern6[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D999580100zz8A561D"; //FLEEデフォルト(6Byte目)
const BYTE constCharaThroughPattern[PATTERN_MAX_RANGE] = "FFD9442440D8D983C414DFE0"; //-0x4 人抜け
const BYTE constCharaThroughDefaultPattern1[PATTERN_MAX_RANGE] = "8D..2428....pp........D9442440D8D983C414DFE0"; //人抜けデフォルト(1Byte目)
const BYTE constCharaThroughDefaultPattern2[PATTERN_MAX_RANGE] = "8D..2428......pp......D9442440D8D983C414DFE0"; //人抜けデフォルト(2Byte目)
const BYTE constCharaThroughDefaultPattern3[PATTERN_MAX_RANGE] = "8D..2428........pp....D9442440D8D983C414DFE0"; //人抜けデフォルト(3Byte目)
const BYTE constCharaThroughDefaultPattern4[PATTERN_MAX_RANGE] = "8D..2428..........pp..D9442440D8D983C414DFE0"; //人抜けデフォルト(4Byte目)
const BYTE constCharaThroughDefaultPattern5[PATTERN_MAX_RANGE] = "8D..2428............ppD9442440D8D983C414DFE0"; //人抜けデフォルト(5Byte目)

//泥道
const BYTE constBackGroundFlagSearchPattern[PATTERN_MAX_RANGE] = "8A44240484C0A2xxxxxxxx"; //-0x1 VIEWSTYLE
// BGFはconstBackGroundFlagSearchPattern -0x1Dでもいける
const BYTE constBackGroundFlagPattern[PATTERN_MAX_RANGE] = "890Dxxxxxxxx8B56048915........8B4608A3........8B4E0C"; //BGF

//ぱぺすて
const BYTE constPuppetTPPattern[PATTERN_MAX_RANGE] = "8B4E74890Dxxxxxxxx"; //PUPTP
const BYTE constAbilityTablePattern[PATTERN_MAX_RANGE] = "000000535657B9xxxxxxxx"; //constAbilityTable

//CAST
//アビリキャストはconstAbilityTablePattern +0x7c
const BYTE constBlueMagicHoldPattern[PATTERN_MAX_RANGE] = "8D0480C1E00266A3xxxxxxxx"; //青魔拘束
const BYTE constMeripoValuePattern[PATTERN_MAX_RANGE] = "2402F6D81BC025xxxxxxxx"; //メリポ
const BYTE constLinkShellNamePattern[PATTERN_MAX_RANGE] = "84C07415BFxxxxxxxx"; //リンクシェル名

//チョコボの宝探し補助ツール(FFXiChocobo)
const BYTE constPositionAddressPattern[PATTERN_MAX_RANGE] = "D8660CD905xxxxxxxx"; //<PositionAddress>

//XiNPC(NPCをIDでターゲットする)
const BYTE constXiNPCPattern[PATTERN_MAX_RANGE] = "04....3C01740B3C02C6..............09C6"; //XiNPC
