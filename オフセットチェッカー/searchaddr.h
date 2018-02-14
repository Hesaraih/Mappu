#define SEARCH_BLOCK				0x1000
#define SEARCH_RANGE				0x400000
#define PATTERN_MAX_RANGE			100
#define NOWSTA_HOSEI				0x06	//0.05�Œǉ�
#define NOWSTA_MEM_CHANGEID			0x16	//0.05�Œǉ�
#define ACTIVITY_INFO_HOSEI			0x08	//0.05�Œǉ�
#define NPCMAP2_HOSEI				0x1C00	//0.05�Œǉ�
#define INVENTORY_INFO_HOSEI		0x6C	//0.05�Œǉ��ύX(�ȑO�܂ł�0x622C)
#define FISH_INFO_HOSEI				0x18
#define XIACE_ITEM_INFO_HOSEI		0x16C	//0.08�ŕύX
#define NPC_MEM_BAZAAR_HOSEI		0x09
#define MONEY_HOSEI					0x04	//0.05a�Œǉ�
#define SendBox_HOSEI				0xF8D0	//�ʒu����}�C�i�X
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
#define UtsusemiRecast_HOSEI		0x2A4	//2Btye*338(���̏p�F���ID)
#define Alliance1_HOSEI				0x210
#define Alliance2_HOSEI				0x420
#define CharList_HOSEI				0x08
#define NpcMemLockOn_HOSEI			0x04
#define NpcMemSubTarget_HOSEI		0x14
#define XiNPC_HOSEI					0x04

int GetOffsetAddressAll(HANDLE hProcess,DWORD dwDllAddress);
DWORD GetOffsetAddress(HANDLE hProcess,DWORD dwDllAddress,const BYTE *pattern,DWORD dwStartAddress = 0,DWORD dwSearchRange = SEARCH_RANGE);//0.05�ŕύX
DWORD SearchAddress(BYTE *base,const BYTE *pattern,DWORD dwReadAddress,DWORD dwDllAddress);


/*
�T�[�`���@
�����݂݃n���P�[���Ńn�[�h�E�F�A�u���[�N�|�C���g���I�t�Z�b�g�ɑ΂��Ĕ��s
EIP�t�߂��t�A�Z���u�����ČŒ�Ǝv����}�V����𒊏o
�s��́u.�v�I�t�Z�b�g�͢x�

�֗��ȃc�[���E�\�t�g�����X���ɓ��e������
---��������---
�I�t�Z�b�g�`�F�b�J�[�́A
VU���������Ƃ��Ă��A����֐��̃I�t�Z�b�g���ɃA�N�Z�X����@�B���(�o�C�i����)���ύX����Ă��Ȃ��B�Ƃ��������̉��ŋ@�B��񂩂�I�t�Z�b�g�𒊏o���Ă��܂�
��VU�Ńo�C�i����ɕύX�����藘�p�ł��Ȃ��Ȃ�\��������܂��̂ŁA�ߋ���dll�ł����삷��œK�ȃo�C�i�������������K�v������܂�(����ł�����ύX�ɂȂ�\���͂���܂�)

���ۂɃT�[�`���Ă���̂�GetOffsetAddress()�̕����ł�
1)�v���Z�X�Ƀ��[�h���ꂽFFXiMain.dll���o�C�i����T�[�`����(�u.�v�̓A�h���X�l�ȂǕs��ƂȂ���́A�ux�v�̓I�t�Z�b�g+�x�[�X�A�h���X�̒l)
2)1)�Ń}�b�`����x�̏ꏊ��dwOffset�ɃR�s�[���ăx�[�X�A�h���X������

����1)2)�̓�����e�I�t�Z�b�g�ɑ΂��ČJ��Ԃ��Ă��邾���ł�

�o�C�i����̏������@��(�����݂݃n���P�[���̏ꍇ)
0)�����݂݃n���P�[���̃v���Z�X�I����pol.exe�ɐݒ�
1)FFXiMain.dll�̃x�[�X�A�h���X+���݂�OFFSET�l�Ƀn�[�h�E�F�A�u���[�N�|�C���g��ݒ肷��
1.1)�f�o�b�O���u���[�N�|�C���g�̑}��/�폜���f�o�b�M�[������̃A�^�b�`
1.2)�u���[�N�|�C���g�̑}��/�폜���u���[�N�|�C���g�ݒ�(�A�h���X��16�i���œ���)���̃n�[�h�E�F�A�u���[�N�|�C���g�Ƀx�[�X�A�h���X+�I�t�Z�b�g�l�̓���
1.3)�A�N�Z�X�̎�ނ��œK�ȕ��ɐݒ�
1.4)�ݒ肷��
2)�o�͂��ꂽ�f�[�^�̒�����K���Ȃ��̂���I�����AEIP�Ə�����Ă��鐔�l����������
���o�͂��ꂽ�f�[�^�͐ݒ肵���A�h���X(�x�[�X�A�h���X+�I�t�Z�b�g�l)�ɑ΂��ăA�N�Z�X�����������̃������̏�Ԃł�EIP�͂��̎��s�A�h���X���w���܂�
3)2)�Ń�������EIP�t�߂��t�A�Z���u������
3.1)��������EIP�̃A�h���X�Ɉړ�����
3.2)EIP�t�߂̑O��0x20��I����Ԃɂ���
3.3)�f�o�b�O���I��͈͂��t�A�Z���u�����t�A�Z���u�����s
4)�o�C�i������쐬����
4.1)�t�A�Z���u���R�[�h�̗�̒�����x�[�X�A�h���X+�I�t�Z�b�g�l�̒l������t�߂�������
4.2)���̑O���K���Ȓ����ƂȂ�悤��Hex�_���v�l���������Ă���
4.3)��������Hex�_���v�l�ŕs��ƂȂ肻���ȕ����u.�v�ɕς���(�A�h���X�l�Ƃ�)
4.4)�x�[�X�A�h���X+�I�t�Z�b�g�l(���g���G���f�B�A���ł�)���ux�v�ɕς���
5)�쐬�����o�C�i������T�[�`���Ă݂�

2)�Ńq�b�g����f�[�^�͑����̂�2)�`5)�𐔉�J��Ԃ��K�v������܂�
�o�C�i����͕ύX���ꂽ��A���܂��܈�v���������������肷��\��������̂ŉߋ���FFXiMain.dll��e�X�g�T�[�o�[�ł��T�[�`�ł��邩�m�F���������ǂ��ł�
---�����܂�---

�ǋL0.04
���ڃT�[�`�o���Ȃ��Ƃ����2��ɕ����Čv�Z�����肵�ċ��߂Ă��܂�
�X�}�[�g�ȕ��@������Ȃ��ׁA�ڂ������͋����Ă����Ə�����܂�
*/


//0.05b��searchaddr.c����ړ�
const BYTE constNpcmapPattern[PATTERN_MAX_RANGE]       = "83EC0C568BF18B46048B0485xxxxxxxx85C0";//85C00F84........B9xxxxxxxx894C24
const BYTE constMapPattern[PATTERN_MAX_RANGE]          = "895E38C786........FFFFFFFF895E3C891Dxxxxxxxx8BCE899E";
//Nowsta��NOWSTA_HOSEI�����{
const BYTE constNowstaPattern[PATTERN_MAX_RANGE]       = "56578BF58D....8.............8D....xxxxxxxx";//�ύX
const BYTE constRecastPattern[PATTERN_MAX_RANGE]       = "56BExxxxxxxx66833E007E13E8";
const BYTE constCastPattern[PATTERN_MAX_RANGE]         = "8BF083C41085F674..A1xxxxxxxx85C0";//�ύX
const BYTE constVeiwstylePattern[PATTERN_MAX_RANGE]    = "1Dxxxxxxxx881D........881D..................8B0D";//�ύX
const BYTE constNpclistPattern[PATTERN_MAX_RANGE]      = "33C05685D2577E198B3Dxxxxxxxx8B7424";
//Activityinfo�͋߂��l����+0x08���Ă��邾��(�Q�l�l)
const BYTE constActivityinfoPattern[PATTERN_MAX_RANGE] = "C646....8B4424..85C0758FA0xxxxxxxxBB";//�ύX
const BYTE constTargetPattern[PATTERN_MAX_RANGE]       = "4424....5250E8........8B0Dxxxxxxxx";//�ύX
const BYTE constWeatherPattern[PATTERN_MAX_RANGE]      = "8BC883C40485C90F84........66A1xxxxxxxx663D";
const BYTE constBazaarPattern[PATTERN_MAX_RANGE]       = "485683F8058BF10F87........FF2485........66837C240C11757CA1xxxxxxxx";

//�q�ɔ�
const BYTE constItemInfoPattern[PATTERN_MAX_RANGE]     = "......EB09..................A1xxxxxxxx85C0";//�ύX? "894E..EB098B15........8956..A1xxxxxxxx85C0";
const BYTE constMenuHelpPattern[PATTERN_MAX_RANGE]     = "E8........84C00F84........A1xxxxxxxx8D48..E8........84C0";
const BYTE constMenuNamePattern[PATTERN_MAX_RANGE]     = "85C0............................8B..xxxxxxxx6A02";//�ύX
//Inventory�͒��ڃT�[�`�ł���[Inventory1] + Inventory2 + INVENTORY_INFO_HOSEI
const BYTE constInventory1Pattern[PATTERN_MAX_RANGE]   = "33D0F6C20374............................8B0Dpppppppp8B..";//�ύX
const BYTE constInventory2Pattern[PATTERN_MAX_RANGE]   = "33D0F6C20374............................8B0D........8B..pppppppp";//�ύX
const BYTE constItemNameListPattern[PATTERN_MAX_RANGE] = "8B5424..85D2741833C9B8xxxxxxxx3910";

//RealTimeLogGets
const BYTE constLoginPattern[PATTERN_MAX_RANGE]        = "8B0D........E8........A1xxxxxxxx85C07420";
const BYTE constLogInfoPattern[PATTERN_MAX_RANGE]      = "030000................8B0DxxxxxxxxE8";//�ύX
const BYTE constLogSerialNoPattern[PATTERN_MAX_RANGE]  = "74..8B0C85........518B0DxxxxxxxxE8";//�ύX
//PT���͒��ڃT�[�`�o����[Partyinfo1] + Partyinfo2
const BYTE constPartyinfo1Pattern[PATTERN_MAX_RANGE]   = "8B3490B9........33C08BFE8935pppppppp6A";//Money1�Ɠ���
const BYTE constPartyinfo2Pattern[PATTERN_MAX_RANGE]   = "57B9........33C08DBApppppppp68";//0.05a�Œǉ�
const BYTE constStatusPattern[PATTERN_MAX_RANGE]       = "33DBF3ABB91000000083C8FFBFxxxxxxxx891D";
//�T�[�o�[�͒��ڃT�[�`�o����[Server1] + Server2
const BYTE constServer1Pattern[PATTERN_MAX_RANGE]      = "8B0Dpppppppp5733FF8B91........8D04D5";//0.05��p�ɕύX
const BYTE constServer2Pattern[PATTERN_MAX_RANGE]      = "2BC28D14808D8491pppppppp81C1";//0.05��p�ɕύX
//Pos(�ʒu)�͒��ڃT�[�`�o����[Pos1] + Pos2
const BYTE constPos1Pattern[PATTERN_MAX_RANGE]         = "8B54240CBFppppppppB9........33C0893D";//0.05��p�ɕύX
const BYTE constPos2Pattern[PATTERN_MAX_RANGE]         = "8B4C2404D94424088988pppppppp8B15";//0.05��p�ɕύX
const BYTE constPcmapPattern[PATTERN_MAX_RANGE]        = "85C00F84........B9xxxxxxxx894C24";
//NPCMAP2�͒��ڃT�[�`�o����Npcmap + 0x1C00
//const BYTE constNpcmap2Pattern[PATTERN_MAX_RANGE]        = "";

//XIACE
const BYTE constFishPattern[PATTERN_MAX_RANGE]         = "8986........C605........00............C705xxxxxxxx";//�ύX
const BYTE constJobInfoPattern[PATTERN_MAX_RANGE]      = "E8........83C4..33C950A1xxxxxxxx8ACC";//�ύX
const BYTE constMaxHpMpInfoPattern[PATTERN_MAX_RANGE]  = "51..........50E8........8B0Dxxxxxxxx";//�ύX
//XIACE�pITEM_INFO�͑q�ɔԂ�ItemInfo��XIACE_ITEM_INFO_HOSEI�𑫂������͓̂����ł��邪�ʂ���(�p�^�[���͈Ⴄ�Ƃ��납��擾)
const BYTE constXIACEItemInfoPattern[PATTERN_MAX_RANGE] = "33C9B8xxxxxxxx391074..05........";
const BYTE constXIACEItemInfoPattern2[PATTERN_MAX_RANGE] = "33C9B8........391074..05pppppppp";//0.08�Œǉ� 20130709VU:ITEM_INFO�̍\���̃T�C�Y���~�����Ƃ��̓R�����g��0x148��0x16c
const BYTE constInventoryCountPattern[PATTERN_MAX_RANGE] = "83F86074..83F870............E8........8B0Dxxxxxxxx";//�ύX
//EQUIP_INFO�͒��ڃT�[�`�o����[EquipInfo1] + EquipInfo2
const BYTE constEquipInfo1Pattern[PATTERN_MAX_RANGE]    = "A1pppppppp8D88........8988";//0.05��p�ɕύX
const BYTE constEquipInfo2Pattern[PATTERN_MAX_RANGE]    = "A1........8D88pppppppp8988";//0.05��p�ɕύX
//Inventorymax�͒��ڃT�[�`�o����[Inventorymax1] + Inventorymax2
const BYTE constInventorymax1Pattern[PATTERN_MAX_RANGE] = "90A1pppppppp85C075030CFFC3";//0.05��p�ɕύX
const BYTE constInventorymax2Pattern[PATTERN_MAX_RANGE] = "C38B4C2404......ppppppppC390";//�ύX
//LOGGED_IN�͐��K�̈ʒu�s���̈�IN=0�AOUT��0�ƂȂ�Ƃ�����ł�������
const BYTE constLoggedinPattern[PATTERN_MAX_RANGE]     = "EB06891Dxxxxxxxx576A3CE8";//�ύX

//FFXI������//0.05�Œǉ�
const BYTE constCursorPosPattern[PATTERN_MAX_RANGE]		= "8B51..85D274..3B05xxxxxxxx75..";
const BYTE constOrderPattern[PATTERN_MAX_RANGE]			= "6A006A006A00E8........A1xxxxxxxx6A00";//�ύX? "EB06891D........391Dxxxxxxxx75226A70";
const BYTE constSenderPattern[PATTERN_MAX_RANGE]		= "6A008B0DxxxxxxxxE8BF120000";
const BYTE constSendTosPattern[PATTERN_MAX_RANGE]		= "8B0Dxxxxxxxx85C974..52E8DDE00100C3";
const BYTE constInputMoneyPattern[PATTERN_MAX_RANGE]	= "8935xxxxxxxxEB06891D........391D........75376A14";
//8935:MOV xx,ESI  EB06:JMP SHORT 891D:MOV ..,EBX 391D:CMP ..,EBX 7537:JNZ SHORT 6A14:PUSH 14
//Money(������)��[Money1] + Money2 + MONEY_HOSEI
const BYTE constMoney1Pattern[PATTERN_MAX_RANGE]		= "8B3490B9........33C08BFE8935pppppppp6A";//Partyinfo1�Ɠ���
const BYTE constMoney2Pattern[PATTERN_MAX_RANGE]		= "8B15........57B9........33C08DBAppppppppBA";

//Fish��
const BYTE constLogAddrPattern[PATTERN_MAX_RANGE]		= "BDxxxxxxxx8B4C24..8D54244C............528D4C24";//�ύX

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
const BYTE constNpcMemBazaarPattern[PATTERN_MAX_RANGE] = "8.C00F84........8B8.ppppppppC1E9..F6C1..0F8.";//�ύX
const BYTE constNpcMemInRangePattern[PATTERN_MAX_RANGE] = "C681pppppppp00668B56..8B0495........8B88";
const BYTE constNpcMemStatePattern[PATTERN_MAX_RANGE] = "C2....8B86pppppppp8BCE....";//�ύX?  "FF2495........8B88pppppppp83C1..83F9..77";
const BYTE constNpcMemLastHateIdPattern[PATTERN_MAX_RANGE] = "A80174..8B86pppppppp85C07428";//�ύX? "F686..........0F84........8B86pppppppp85C00F84";
//LockOn��SubTarget�̓^�[�Q�b�g�\���̂���Ɍ�������
const BYTE constNpcMemLockOnSearchPattern[PATTERN_MAX_RANGE] = "FFFFFFFF";

#ifdef HOOK_INFO
//ffhook�̒u���A�h���X
const BYTE constFfhook_logPattern[PATTERN_MAX_RANGE]   = "E8........8B470C8B5424184089470C84DB742E";
const BYTE constFfhook_cmdPattern[PATTERN_MAX_RANGE]   = "............8B8C24080400005556578D84241002000068000200005033ED51";//�ύX
#endif

const BYTE constCameraPattern[PATTERN_MAX_RANGE]       = "89.424..E8........8B0D........68xxxxxxxx6A";//�ύX

//�D�j
const BYTE constArgusInitCodePattern[PATTERN_MAX_RANGE] = "C64107048B510433C081E2000000FF89"; //INITCODE
const BYTE constOwnPositionPattern[PATTERN_MAX_RANGE] = "83EC10568BF1B9xxxxxxxx"; //OwnPosition
const BYTE constDeliveryPattern[PATTERN_MAX_RANGE] = "C40484C075078B44240CF7D8C3A1"; //+0x4 �f���o���[
const BYTE constWait0DeliveryBoxPattern[PATTERN_MAX_RANGE] = "75508B0D"; //��z
const BYTE constWait0MogDeliveryBoxPattern[PATTERN_MAX_RANGE] = "5EC3807E0C0175378B0D"; //+0x6 ���O�|�X�g
const BYTE constWait0SalesStatusPattern[PATTERN_MAX_RANGE] = "46063C0175218B0D"; //+0x4 �a���m�F
const BYTE constWait0BidPattern[PATTERN_MAX_RANGE] = "81C2B40000006689"; //���D
const BYTE constWait0Synthesis1Pattern[PATTERN_MAX_RANGE] = "000085C97403"; //+0x4 �����P
const BYTE constWait0Synthesis2Pattern[PATTERN_MAX_RANGE] = "FF84C075178B0D"; //+0x3 ����2
const BYTE constWallThroughPattern[PATTERN_MAX_RANGE] = "0F84D30300008A86"; //�ǔ���
const BYTE constWideCameraPattern[PATTERN_MAX_RANGE] = "1000007243"; //+0x1 �L�扻
const BYTE constMeripoMenuPattern[PATTERN_MAX_RANGE] = "EEFF........6A0068........E8........83C4........EEFF........6A......"; //+0x23 �����|
const BYTE constAuctionHouseMenuPattern[PATTERN_MAX_RANGE] = "2573203a20000000"; //+0x8 �������j��
const BYTE constViewMogHousePattern[PATTERN_MAX_RANGE] = "E8........E8........E8................6A..6A..68"; //+0x18 ���O�n�m�F
const BYTE constViewMogHouseMenuPattern[PATTERN_MAX_RANGE] = "636F6D6D656E752000000000"; //+0xC ���m���j��
const BYTE constChatLogColumnPattern[PATTERN_MAX_RANGE] = "........9380000005022000"; //+0xC �s���ύX
const BYTE constCampaignInfoPattern[PATTERN_MAX_RANGE] = "48..88....83C0203D..........EDB8xxxxxxxx"; //�J���p�j�G
const BYTE constOwnAreaIDPattern[PATTERN_MAX_RANGE] = "7428568B35xxxxxxxx"; //OwnAreaID
const BYTE constBazaar2MeripoPattern[PATTERN_MAX_RANGE] = "6D6572697463617400000000"; //+0xC &0xFFFF �o�U�[�˃����|
const BYTE constMeripo2BazaarPattern[PATTERN_MAX_RANGE] = "6D63726D656E752000000000"; //+0xC &0xFFFF �����|�˃o�U�[
const BYTE constFleePattern[PATTERN_MAX_RANGE] = "00008A561D"; //-0x4 FLEE
const BYTE constFleeDefaultPattern1[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0zz99580100008A561D"; //FLEE�f�t�H���g(1Byte��)
const BYTE constFleeDefaultPattern2[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D9zz580100008A561D"; //FLEE�f�t�H���g(2Byte��)
const BYTE constFleeDefaultPattern3[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D999zz0100008A561D"; //FLEE�f�t�H���g(3Byte��)
const BYTE constFleeDefaultPattern4[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D99958zz00008A561D"; //FLEE�f�t�H���g(4Byte��)
const BYTE constFleeDefaultPattern5[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D9995801zz008A561D"; //FLEE�f�t�H���g(5Byte��)
const BYTE constFleeDefaultPattern6[PATTERN_MAX_RANGE] = "D80D........8B0C8.........33C0D999580100zz8A561D"; //FLEE�f�t�H���g(6Byte��)
const BYTE constCharaThroughPattern[PATTERN_MAX_RANGE] = "FFD9442440D8D983C414DFE0"; //-0x4 �l����
const BYTE constCharaThroughDefaultPattern1[PATTERN_MAX_RANGE] = "8D..2428....pp........D9442440D8D983C414DFE0"; //�l�����f�t�H���g(1Byte��)
const BYTE constCharaThroughDefaultPattern2[PATTERN_MAX_RANGE] = "8D..2428......pp......D9442440D8D983C414DFE0"; //�l�����f�t�H���g(2Byte��)
const BYTE constCharaThroughDefaultPattern3[PATTERN_MAX_RANGE] = "8D..2428........pp....D9442440D8D983C414DFE0"; //�l�����f�t�H���g(3Byte��)
const BYTE constCharaThroughDefaultPattern4[PATTERN_MAX_RANGE] = "8D..2428..........pp..D9442440D8D983C414DFE0"; //�l�����f�t�H���g(4Byte��)
const BYTE constCharaThroughDefaultPattern5[PATTERN_MAX_RANGE] = "8D..2428............ppD9442440D8D983C414DFE0"; //�l�����f�t�H���g(5Byte��)

//�D��
const BYTE constBackGroundFlagSearchPattern[PATTERN_MAX_RANGE] = "8A44240484C0A2xxxxxxxx"; //-0x1 VIEWSTYLE
// BGF��constBackGroundFlagSearchPattern -0x1D�ł�������
const BYTE constBackGroundFlagPattern[PATTERN_MAX_RANGE] = "890Dxxxxxxxx8B56048915........8B4608A3........8B4E0C"; //BGF

//�ς؂���
const BYTE constPuppetTPPattern[PATTERN_MAX_RANGE] = "8B4E74890Dxxxxxxxx"; //PUPTP
const BYTE constAbilityTablePattern[PATTERN_MAX_RANGE] = "000000535657B9xxxxxxxx"; //constAbilityTable

//CAST
//�A�r���L���X�g��constAbilityTablePattern +0x7c
const BYTE constBlueMagicHoldPattern[PATTERN_MAX_RANGE] = "8D0480C1E00266A3xxxxxxxx"; //���S��
const BYTE constMeripoValuePattern[PATTERN_MAX_RANGE] = "2402F6D81BC025xxxxxxxx"; //�����|
const BYTE constLinkShellNamePattern[PATTERN_MAX_RANGE] = "84C07415BFxxxxxxxx"; //�����N�V�F����

//�`���R�{�̕�T���⏕�c�[��(FFXiChocobo)
const BYTE constPositionAddressPattern[PATTERN_MAX_RANGE] = "D8660CD905xxxxxxxx"; //<PositionAddress>

//XiNPC(NPC��ID�Ń^�[�Q�b�g����)
const BYTE constXiNPCPattern[PATTERN_MAX_RANGE] = "04....3C01740B3C02C6..............09C6"; //XiNPC
