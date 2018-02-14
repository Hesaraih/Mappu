#define SEARCH_BLOCK				0x1000
#define SEARCH_RANGE				0x400000
#define PATTERN_MAX_RANGE			100
#define NOWSTA_HOSEI				(-0x06)	//0.05で追加
#define ACTIVITY_INFO_HOSEI			0x08	//0.05で追加

int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern,DWORD dwStartAddress = 0,DWORD dwSearchRange = SEARCH_RANGE);//0.05で変更
DWORD SearchAddress(BYTE *base,const BYTE *pattern,DWORD dwReadAddress,DWORD dwDllAddress);


//0.05bでsearchaddr.cから移動
const BYTE constNpcmapPattern[PATTERN_MAX_RANGE]       = "83EC0C568BF18B46048B0485xxxxxxxx85C0";//85C00F84........B9xxxxxxxx894C24
//NowstaはNOWSTA_HOSEIを実施
const BYTE constNowstaPattern[PATTERN_MAX_RANGE]       = "56578BF58D....8.............8D....xxxxxxxx";//変更
const BYTE constNpclistPattern[PATTERN_MAX_RANGE]      = "33C05685D2577E198B3Dxxxxxxxx8B7424";
const BYTE constTargetPattern[PATTERN_MAX_RANGE]       = "4424....5250E8........8B0Dxxxxxxxx";//変更
const BYTE constWeatherPattern[PATTERN_MAX_RANGE]      = "8BC883C40485C90F84........66A1xxxxxxxx663D";


//NPC_MEM
const BYTE constNpcMemRadianPattern[PATTERN_MAX_RANGE]    = "50408951..8B50..8951..8B50..8951pp8B50..8951..8B40..8941..C3";
const BYTE constNpcMemChangedIdPattern[PATTERN_MAX_RANGE] = "8179pp........C0C3..90";
const BYTE constNpcMemFixedIdPattern[PATTERN_MAX_RANGE] = "C1E9..F6C1..74..8B50pp8B4E..3BD175";
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
