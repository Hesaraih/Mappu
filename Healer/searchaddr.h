int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern);
DWORD SearchAddress(BYTE *base,const BYTE *pattern);


#define SEARCH_BLOCK	0x1000
#define SEARCH_RANGE	0x300000
#define PATTERN_MAX_RANGE	100
#define NOWSTA_HOSEI		(-0x06)//�J�n�ʒu�������ɈႤ�݂����Ȃ̂ŕ␳
#define ACTIVITY_INFO_HOSEI	(0x08)//���ڃT�[�`�s�̈׋߂��Ƃ��납��Q��

//�T�[�`���@
//�����݂݃n���P�[���Ńn�[�h�E�F�A�u���[�N�|�C���g���I�t�Z�b�g�ɑ΂��Ĕ��s
//EIP�t�߂��t�A�Z���u�����ČŒ�Ǝv����}�V����𒊏o
//�s��́u.�v�I�t�Z�b�g�͢x�

#define constNpcmapPattern			"83EC0C568BF18B46048B0485xxxxxxxx85C0"
#define constMapPattern				"895E38C786........FFFFFFFF895E3C891Dxxxxxxxx8BCE899E"
//Nowsta��NOWSTA_HOSEI�����{
const BYTE constNowstaPattern[PATTERN_MAX_RANGE]       = "56578BF58D....8.............8D....xxxxxxxx";
#define constRecastPattern			"56BExxxxxxxx66833E007E13E8"
#define constCastPattern			"8BF083C41085F674..A1xxxxxxxx85C0"
#define constVeiwstylePattern		"1Dxxxxxxxx881D........881D..................8B0D"
#define constNpclistPattern			"33C05685D2577E198B3Dxxxxxxxx8B7424"
//Activityinfo�͋߂��l����+0x08���Ă��邾��(�Q�l�l)
#define constActivityinfoPattern	"C646....8B4424..85C0758FA0xxxxxxxxBB"
#define constTargetPattern			"4424....5250E8........8B0Dxxxxxxxx"

