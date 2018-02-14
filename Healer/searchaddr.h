int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern);
DWORD SearchAddress(BYTE *base,const BYTE *pattern);


#define SEARCH_BLOCK	0x1000
#define SEARCH_RANGE	0x300000
#define PATTERN_MAX_RANGE	100
#define NOWSTA_HOSEI		(-0x06)//開始位置が微妙に違うみたいなので補正
#define ACTIVITY_INFO_HOSEI	(0x08)//直接サーチ不可の為近いところから参照

//サーチ方法
//うさみみハリケーンでハードウェアブレークポイントをオフセットに対して発行
//EIP付近を逆アセンブリして固定と思われるマシン語を抽出
//不定は「.」オフセットは｢x｣

#define constNpcmapPattern			"83EC0C568BF18B46048B0485xxxxxxxx85C0"
#define constMapPattern				"895E38C786........FFFFFFFF895E3C891Dxxxxxxxx8BCE899E"
//NowstaはNOWSTA_HOSEIを実施
const BYTE constNowstaPattern[PATTERN_MAX_RANGE]       = "56578BF58D....8.............8D....xxxxxxxx";
#define constRecastPattern			"56BExxxxxxxx66833E007E13E8"
#define constCastPattern			"8BF083C41085F674..A1xxxxxxxx85C0"
#define constVeiwstylePattern		"1Dxxxxxxxx881D........881D..................8B0D"
#define constNpclistPattern			"33C05685D2577E198B3Dxxxxxxxx8B7424"
//Activityinfoは近い値から+0x08しているだけ(参考値)
#define constActivityinfoPattern	"C646....8B4424..85C0758FA0xxxxxxxxBB"
#define constTargetPattern			"4424....5250E8........8B0Dxxxxxxxx"

