int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern);
DWORD SearchAddress(BYTE *base,const BYTE *pattern);

#define SEARCH_BLOCK	0x1000
#define SEARCH_RANGE	0x300000
#define PATTERN_MAX_RANGE	100
#define NOWSTA_HOSEI	(-0x06)//開始位置が微妙に違うみたいなので補正

//サーチ方法
//うさみみハリケーンでハードウェアブレークポイントをオフセットに対して発行
//EIP付近を逆アセンブリして固定と思われるマシン語を抽出
//不定は「.」オフセットは｢x｣
#define constNpcmapPattern	"83EC0C568BF18B46048B0485xxxxxxxx85C0"
//NowstaはNOWSTA_HOSEIを実施
#define constNowstaPattern	"56578BF58D....8.............8D....xxxxxxxx"
#define constBazaarPattern	"485683F8058BF10F87........FF2485........66837C240C11757CA1xxxxxxxx"
