extern HANDLE g_polHandle;//pol�̃n���h��
extern DWORD g_DllAddress;//FFXiMain.dll�̃A�h���X
extern DWORD g_DllSize;//DLL�̃T�C�Y
extern HWND g_hList;//IDC_LIST�̃n���h��
//extern WNDPROC WndProcMainClass;//�T�u�N���X����WNDPROC

//�e��I�t�Z�b�g

//MAP:AreaID
//extern DWORD g_OffsetMap;

//NPCMAP:NPC���A�h���X(�|�C���^)���i�[����Ă���z��̐擪�A�h���X
//struct{
//	DWORD �s��;
//	float x;
//	float y;
//	float z;
//	....
//	float radian;
//	....
//	DWORD View
//	....
//	BYTE hpp;
//	....
//	BYTE inrange;
//	....
//	BYTE state;
//	....
//}*dwNPCINFO[id];
//&dwNPCINFO[0] = NPCMAP+DLL
//extern DWORD g_OffsetNpcMap;

//NPCLIST:none�Ŏn�܂��Ă���T�C�Y0x1C�o�C�g�̍\���̔z��̃A�h���X(�|�C���^)���L�ڂ���Ă���A�h���X
//+0h����0x18��NPC���A�I�[������\0�܂܂Ȃ��ꍇ�L��
//struct{
//	char name[0x18];
//	DWORD id;// NpcID = id&0x0FFF;AreaID = (id>>12)&0xFF;
//}*NPCLIST;
//extern DWORD g_OffsetNpcList;

//NOWSTA�̃I�t�Z�b�g
//���O�C�����Ă���L�������Ŏn�܂�A�h���X
//struct{
//	char name[0x12];//���O�C�����Ă��鎩���̖��O
//	DWORD fixed_id;//�����̌Œ�ID
//	WORD change_id;//�����̕ϓ�ID
//	�ȍ~�s��;
//}NOWSTA;
//(FFXiMain.dll�̐擪�A�h���X+NPCMAP+4Byte*change_id)�Ŏ����̏��
//extern DWORD g_OffsetNowSta;

//TARGET�̃I�t�Z�b�g
//extern DWORD g_OffsetTarget;

//WEATGER�̃I�t�Z�b�g
//00=��
//01=����
//02=��
//03=��
//04=�M�g
//05=�ܔM�g
//06=�J
//07=�X�R�[��
//08=���o
//09=����
//0A=��
//0B=�\��
//0C=��
//0D=����
//0E=��
//0F=���J
//10=�I�[����
//11=����
//12=�d��
//13=���H
//extern DWORD g_OffsetWeather;

extern DWORD g_DLLFileSize;
extern DWORD g_DLLFileSizeCmp;
extern struct FFXIOFFSET g_Offset;
extern struct NPC_MEM_MAP NPC_MEM;//0.49�Œǉ�

//map.ini�̃p�X
extern _TCHAR g_MapPath[MAX_PATH + 1];

//NPCLIST�̃p�X
extern _TCHAR g_NpcListPath[MAX_PATH + 1];

//�E�B���h�E�̈ʒu
extern int g_MapPosX;
extern int g_MapPosY;
extern int g_MapSizeX;
extern int g_MapSizeY;
extern int g_NpcListPosX;
extern int g_NpcListPosY;
extern int g_NpcListSizeX;
extern int g_NpcListSizeY;
extern float g_Zoom;//�\���{��
extern float g_ZoomMouseDelta;//�����̍����l
extern float g_Zoom_01;//0.1�{�}�b�v�̃f�t�H���g�{���i�t�B�[���h�j
extern float g_Zoom_02;//0.2�{�}�b�v�̃f�t�H���g�{���i�_���W�����j
extern float g_Zoom_04;//0.4�{�}�b�v�̃f�t�H���g�{���i�X�j
extern int g_Sort[5];//�\�[�g���[�J������]
extern int g_Timer;//�^�C�}�[ms
extern _TCHAR g_WaveFileName[12][3][MAX_PATH];//0.47��3��ނɕύX
extern _TCHAR g_WeatherWaveFileName[MAX_PATH];

//�O���[�o���G���[�R�[�h
extern int g_ErrorCode;

//�t�H���g�T�C�Y
extern FONT g_Font;
extern FONT g_FontEx;
extern FONT g_FontSp;//0.47�Œǉ�
extern FONT g_FontSystem;
extern FONT g_FontTarget;
extern FONT g_FontPC;

//NPCLIST�̓G���G���AID
extern WORD g_ListAreaID;

//�Ď�����v���Z�XID
extern DWORD g_ProcessID;

//���ߗ�
//extern int g_AlphaMAP;
//extern int g_AlphaNPCLIST;

//�F���i�[
extern COLORREF g_ColorVisible;
extern COLORREF g_ColorAbility;
extern COLORREF g_ColorMagic;
extern COLORREF g_ColorRange;
extern COLORREF g_ColorWindSong;
extern COLORREF g_ColorDetection;
extern COLORREF g_ColorMapName;
extern COLORREF g_ColorZoom;
extern COLORREF g_ColorMyPosition;
extern COLORREF g_Colorlt_Pos_gt;
extern COLORREF g_ColorDistance;
extern COLORREF g_ColorNPC_Pop[3];//0.47��3�v�f�ɕύX
extern COLORREF g_ColorNPC_Share[3];
extern COLORREF g_ColorNPC_War[3];
extern COLORREF g_ColorNPC_Corpse[3];
extern COLORREF g_ColorNPC_Out[3];
extern COLORREF g_ColorOWN;
extern COLORREF g_ColorPARTY;
extern COLORREF g_ColorALIANCE;//0.51�Œǉ�
extern COLORREF g_ColorPC;
extern COLORREF g_ColorPET;
extern COLORREF g_ColorTarget;
extern COLORREF g_ColorTransparent;

//�e�|�C���g�T�C�Y
extern float g_PointSizeNPC;
extern float g_PointSizeOWN;
extern float g_PointSizeTRACE;//0.55�Œǉ�
extern float g_PointSizePARTY;
extern float g_PointSizePC;
extern float g_PointSizePET;
extern float g_PointSizeTARGET;
extern int g_TargetHighlight;

//�G���A������NPCLIST���X�V���S�Ă�NPC�Ƀ`�F�b�N������
//ALWAYS_UPDATE_AND_SELECT_ALL_CHECK
//extern int g_AlwaysUpdate;

//FILE_AUTOLOAD
//extern int g_FileAutoLoad;

//map.ini���˂��܌݊��ŗ��p����(62_c=��62_12.gif�ŊJ��)
extern int g_HexToDec;

//�}�b�v�摜�𔽓]�\������
//extern int g_ImgReverse;

//���[�_�[���[�h
//extern int g_RadarMode;

//���ߐF���[�h
//extern int g_TransparentMode;

//MAP�𒆐S�ɂ���
//extern int g_CenterOWN;
extern int g_CenterMAP_X;
extern int g_CenterMAP_Z;

//�V���[�g�J�b�g�L�[�̐ݒ�
extern int g_KeyZoomPlus;
extern int g_KeyZoomPlus_1;
extern int g_KeyZoomMinus;
extern int g_KeyZoomMinus_1;

//�I�����_�C�A���O�{�b�N�X��\��������
extern int g_ExitDialogBox;

//State��Corpse/-��Pop�ƂȂ������ATimer�̕\�����Ԃ��N���A����
extern int g_ClearTime;

//�͈͊O�ɂȂ������ACorpse(War)�Ȃ�͈͊O�̐F�ɂ��Ȃ�
extern int g_NotChangeColorCorpse;
extern int g_NotChangeColorWar;

//NPCLIST�̕����w�i�F��L�� 0.48�Œǉ�
extern int g_NPCLIST_BkColor;

//�B�����x�� 0.56�Œǉ�
extern int g_height_diff;

//MENU_FLAG 0.46�Œǉ�
extern MENU_FLAG g_Menu;
//Link 0.59�Œǉ�
extern LINK g_Link;