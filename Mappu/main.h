extern HANDLE g_polHandle;//polのハンドル
extern DWORD g_DllAddress;//FFXiMain.dllのアドレス
extern DWORD g_DllSize;//DLLのサイズ
extern HWND g_hList;//IDC_LISTのハンドル
//extern WNDPROC WndProcMainClass;//サブクラス元のWNDPROC

//各種オフセット

//MAP:AreaID
//extern DWORD g_OffsetMap;

//NPCMAP:NPC情報アドレス(ポインタ)が格納されている配列の先頭アドレス
//struct{
//	DWORD 不明;
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

//NPCLIST:noneで始まっているサイズ0x1Cバイトの構造体配列のアドレス(ポインタ)が記載されているアドレス
//+0hから0x18にNPC名、終端文字列\0含まない場合有り
//struct{
//	char name[0x18];
//	DWORD id;// NpcID = id&0x0FFF;AreaID = (id>>12)&0xFF;
//}*NPCLIST;
//extern DWORD g_OffsetNpcList;

//NOWSTAのオフセット
//ログインしているキャラ名で始まるアドレス
//struct{
//	char name[0x12];//ログインしている自分の名前
//	DWORD fixed_id;//自分の固定ID
//	WORD change_id;//自分の変動ID
//	以降不明;
//}NOWSTA;
//(FFXiMain.dllの先頭アドレス+NPCMAP+4Byte*change_id)で自分の情報
//extern DWORD g_OffsetNowSta;

//TARGETのオフセット
//extern DWORD g_OffsetTarget;

//WEATGERのオフセット
//00=晴
//01=快晴
//02=曇
//03=霧
//04=熱波
//05=灼熱波
//06=雨
//07=スコール
//08=砂塵
//09=砂嵐
//0A=風
//0B=暴風
//0C=雪
//0D=吹雪
//0E=雷
//0F=雷雨
//10=オーロラ
//11=白夜
//12=妖霧
//13=日食
//extern DWORD g_OffsetWeather;

extern DWORD g_DLLFileSize;
extern DWORD g_DLLFileSizeCmp;
extern struct FFXIOFFSET g_Offset;
extern struct NPC_MEM_MAP NPC_MEM;//0.49で追加

//map.iniのパス
extern _TCHAR g_MapPath[MAX_PATH + 1];

//NPCLISTのパス
extern _TCHAR g_NpcListPath[MAX_PATH + 1];

//ウィンドウの位置
extern int g_MapPosX;
extern int g_MapPosY;
extern int g_MapSizeX;
extern int g_MapSizeY;
extern int g_NpcListPosX;
extern int g_NpcListPosY;
extern int g_NpcListSizeX;
extern int g_NpcListSizeY;
extern float g_Zoom;//表示倍率
extern float g_ZoomMouseDelta;//増減の差分値
extern float g_Zoom_01;//0.1倍マップのデフォルト倍率（フィールド）
extern float g_Zoom_02;//0.2倍マップのデフォルト倍率（ダンジョン）
extern float g_Zoom_04;//0.4倍マップのデフォルト倍率（街）
extern int g_Sort[5];//ソート情報[カラム数]
extern int g_Timer;//タイマーms
extern _TCHAR g_WaveFileName[12][3][MAX_PATH];//0.47で3種類に変更
extern _TCHAR g_WeatherWaveFileName[MAX_PATH];

//グローバルエラーコード
extern int g_ErrorCode;

//フォントサイズ
extern FONT g_Font;
extern FONT g_FontEx;
extern FONT g_FontSp;//0.47で追加
extern FONT g_FontSystem;
extern FONT g_FontTarget;
extern FONT g_FontPC;

//NPCLISTの敵情報エリアID
extern WORD g_ListAreaID;

//監視するプロセスID
extern DWORD g_ProcessID;

//透過率
//extern int g_AlphaMAP;
//extern int g_AlphaNPCLIST;

//色情報格納
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
extern COLORREF g_ColorNPC_Pop[3];//0.47で3要素に変更
extern COLORREF g_ColorNPC_Share[3];
extern COLORREF g_ColorNPC_War[3];
extern COLORREF g_ColorNPC_Corpse[3];
extern COLORREF g_ColorNPC_Out[3];
extern COLORREF g_ColorOWN;
extern COLORREF g_ColorPARTY;
extern COLORREF g_ColorALIANCE;//0.51で追加
extern COLORREF g_ColorPC;
extern COLORREF g_ColorPET;
extern COLORREF g_ColorTarget;
extern COLORREF g_ColorTransparent;

//各ポイントサイズ
extern float g_PointSizeNPC;
extern float g_PointSizeOWN;
extern float g_PointSizeTRACE;//0.55で追加
extern float g_PointSizePARTY;
extern float g_PointSizePC;
extern float g_PointSizePET;
extern float g_PointSizeTARGET;
extern int g_TargetHighlight;

//エリアした時NPCLISTを更新＆全てのNPCにチェックを入れる
//ALWAYS_UPDATE_AND_SELECT_ALL_CHECK
//extern int g_AlwaysUpdate;

//FILE_AUTOLOAD
//extern int g_FileAutoLoad;

//map.iniをねこま互換で利用する(62_c=を62_12.gifで開く)
extern int g_HexToDec;

//マップ画像を反転表示する
//extern int g_ImgReverse;

//レーダーモード
//extern int g_RadarMode;

//透過色モード
//extern int g_TransparentMode;

//MAPを中心にする
//extern int g_CenterOWN;
extern int g_CenterMAP_X;
extern int g_CenterMAP_Z;

//ショートカットキーの設定
extern int g_KeyZoomPlus;
extern int g_KeyZoomPlus_1;
extern int g_KeyZoomMinus;
extern int g_KeyZoomMinus_1;

//終了時ダイアログボックスを表示させる
extern int g_ExitDialogBox;

//StateがCorpse/-→Popとなった時、Timerの表示時間をクリアする
extern int g_ClearTime;

//範囲外になった時、Corpse(War)なら範囲外の色にしない
extern int g_NotChangeColorCorpse;
extern int g_NotChangeColorWar;

//NPCLISTの文字背景色を有効 0.48で追加
extern int g_NPCLIST_BkColor;

//隠す高度差 0.56で追加
extern int g_height_diff;

//MENU_FLAG 0.46で追加
extern MENU_FLAG g_Menu;
//Link 0.59で追加
extern LINK g_Link;