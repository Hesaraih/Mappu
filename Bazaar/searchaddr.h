int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern);
DWORD SearchAddress(BYTE *base,const BYTE *pattern);

#define SEARCH_BLOCK	0x1000
#define SEARCH_RANGE	0x300000
#define PATTERN_MAX_RANGE	100
#define NOWSTA_HOSEI	(-0x06)//�J�n�ʒu�������ɈႤ�݂����Ȃ̂ŕ␳

//�T�[�`���@
//�����݂݃n���P�[���Ńn�[�h�E�F�A�u���[�N�|�C���g���I�t�Z�b�g�ɑ΂��Ĕ��s
//EIP�t�߂��t�A�Z���u�����ČŒ�Ǝv����}�V����𒊏o
//�s��́u.�v�I�t�Z�b�g�͢x�
#define constNpcmapPattern	"83EC0C568BF18B46048B0485xxxxxxxx85C0"
//Nowsta��NOWSTA_HOSEI�����{
#define constNowstaPattern	"56578BF58D....8.............8D....xxxxxxxx"
#define constBazaarPattern	"485683F8058BF10F87........FF2485........66837C240C11757CA1xxxxxxxx"
