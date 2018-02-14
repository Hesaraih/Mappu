#include <tchar.h>
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi")

class CLASS_ITEM
{
#define SIZEOF_ITEM_STRUCT	0xC00//polアイテム情報の構造体サイズ
private:
	_TCHAR _sztDatPath[MAX_PATH];
	int _nFileSize;
	HANDLE _hFile;
	int _Decrypt(BYTE*);

public:
	//int m_nWeaponArmor;
	int m_nItemType;//1.3で武具以外にも対応のため名称変更
					//ID,リソースID,Name1,Name2,Name3,種類,種族性別,ジョブ,D値/盾タイプ,間隔,レベル,スタック数,レア,EX,説明文
	DWORD m_ID;
	WORD m_ResourceID;
	char m_szcName1[0x100];
	char m_szcName2[0x100];
	char m_szcName3[0x100];
	WORD m_Slot;
	WORD m_Skill;
	WORD m_Race;
	DWORD m_Job;
	WORD m_Shild;
	WORD m_Damage;
	WORD m_Delay;
	BYTE m_Rare;
	BYTE m_Ex;
	WORD m_Level;
	WORD m_StackSize;
	char m_szcText[0x200];
	WORD m_Type;
	WORD m_Element;//1.3で追加
	DWORD m_StorageSlots;//1.3で追加

						 //CLASS_ITEM(_TCHAR *sztDatPath,int nWeaPonArmor){
	CLASS_ITEM(_TCHAR *sztDatPath, int nType) {//1.3で変更
		_tcscpy_s(_sztDatPath, sztDatPath);
		this->_hFile = CreateFile(_sztDatPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		this->_nFileSize = GetFileSize(this->_hFile, NULL);
		//this->m_nWeaponArmor = nWeaPonArmor;
		this->m_nItemType = nType;
	}
	~CLASS_ITEM() {
		if (this->_hFile) {
			CloseHandle(this->_hFile);
		}
	}
	int IsItemDat();
	int Get(int);
	void Clear() {
		m_ID = 0;
		m_ResourceID = 0;
		ZeroMemory(m_szcName1, sizeof(m_szcName1));
		ZeroMemory(m_szcName2, sizeof(m_szcName2));
		ZeroMemory(m_szcName3, sizeof(m_szcName3));
		m_Slot = 0;
		m_Skill = 0;
		m_Race = 0;
		m_Job = 0;
		m_Shild = 0;
		m_Damage = 0;
		m_Delay = 0;
		m_Rare = 0;
		m_Ex = 0;
		m_Level = 0;
		m_StackSize = 0;
		ZeroMemory(m_szcText, sizeof(m_szcText));
	}
};

class CLASS_MAGIC
{
#define MAGIC_NAME_OFFSET		0x40//魔法情報の開始位置
#define MAGIC_TEXT_OFFSET		0x40//魔法情報の開始位置
#ifdef TEST1
#define MAGIC_DETAILS_OFFSET	0x1040//魔法情報の開始位置
#else
#define MAGIC_DETAILS_OFFSET	0x30//魔法情報の開始位置
#endif
#define SIZEOF_MAGIC_NAME_STRUCT	0x8C//pol魔法情報の構造体サイズ
#define SIZEOF_MAGIC_TEXT_STRUCT	0x100//pol魔法情報の構造体サイズ
#define SIZEOF_MAGIC_DETAILS_STRUCT	0x40//pol魔法情報の構造体サイズ
private:
	_TCHAR _sztDatPathName[MAX_PATH];
	_TCHAR _sztDatPathText[MAX_PATH];
	_TCHAR _sztDatPathDetails[MAX_PATH];
	int _nFileSizeName;
	int _nFileSizeText;
	int _nFileSizeDetails;
	HANDLE _hFileName;
	HANDLE _hFileText;
	HANDLE _hFileDetails;
	int _CountBits(BYTE B) {
		int Count = 0;
		while (B != 0)
		{
			if ((B & 0x01) != 0)
				++Count;
			B >>= 1;
		}
		return Count;
	};
	int _Decrypt(BYTE* byData) {
		BYTE save3 = byData[2], save12 = byData[11], save13 = byData[12];
		int nShiftSize;
		int BitCount = _CountBits(byData[2]) - _CountBits(byData[11]) + _CountBits(byData[12]);
		switch (abs(BitCount) % 5) {
		case 0:nShiftSize = 7; break;
		case 1:nShiftSize = 1; break;
		case 2:nShiftSize = 6; break;
		case 3:nShiftSize = 2; break;
		case 4:nShiftSize = 5; break;
		}
		for (int i = 0; i<SIZEOF_MAGIC_DETAILS_STRUCT; i++) {
			BYTE B = byData[i];
			byData[i] = (BYTE)((B >> nShiftSize) | (B << (8 - nShiftSize)));
		}
		byData[2] = save3; byData[11] = save12; byData[12] = save13;
		return 1;
	};

public:
	//_T("Index"),_T("ID"),_T("名前"),_T("Text"),_T("MP"),_T("属性"),_T("種類"),_T("キャスト"),_T("リキャスト"),_T("ジョブ")
	WORD m_Index;
	DWORD m_ID;
	_TCHAR m_szcName[0x100];
	_TCHAR m_szcText[0x200];
	WORD m_Mp;
	WORD m_Element;
	WORD m_Target;
	WORD m_Type;
	WORD m_Skill;
	float m_Cast;
	float m_Recast;
	_TCHAR m_szcJob[0x200];
	BYTE m_Level[24];
#ifdef DEBUG
	BYTE m_Unknown[0x10];
#endif

	CLASS_MAGIC(_TCHAR *sztDatPathName, _TCHAR *sztDatPathText, _TCHAR *sztDatPathDetails) {
		_tcscpy_s(_sztDatPathName, sztDatPathName);
		this->_hFileName = CreateFile(_sztDatPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		this->_nFileSizeName = GetFileSize(this->_hFileName, NULL);
		_tcscpy_s(_sztDatPathText, sztDatPathText);
		this->_hFileText = CreateFile(sztDatPathText, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		this->_nFileSizeText = GetFileSize(this->_hFileText, NULL);
		_tcscpy_s(_sztDatPathDetails, sztDatPathDetails);
		this->_hFileDetails = CreateFile(_sztDatPathDetails, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		this->_nFileSizeDetails = GetFileSize(this->_hFileDetails, NULL);
	}
	~CLASS_MAGIC() {
		if (this->_hFileName) {
			CloseHandle(this->_hFileName);
		}
		if (this->_hFileText) {
			CloseHandle(this->_hFileText);
		}
		if (this->_hFileDetails) {
			CloseHandle(this->_hFileDetails);
		}
	}
	int Get(int);
	void Clear() {
		m_ID = 0;
		ZeroMemory(m_szcName, sizeof(m_szcName));
		ZeroMemory(m_szcText, sizeof(m_szcText));
		m_Mp = 0;
		m_Element = 0;
		m_Target = 0;
		m_Type = 0;
		m_Skill = 0;
		m_Cast = 0;
		m_Recast = 0;
		ZeroMemory(m_szcJob, sizeof(m_szcJob));
		ZeroMemory(m_Level, sizeof(m_Level));
	}
};

class CLASS_PATH
{
#define MAX_KEY_NUM	9//iniのWEAPON_?を読込む最大数

public:
	_TCHAR m_sztJobs[24][0x10];//ジョブ名
	_TCHAR sztIni[MAX_PATH];//.iniのパス
	struct {
		_TCHAR sztPol[MAX_PATH];
		struct {
			_TCHAR ja[MAX_PATH];
			_TCHAR en[MAX_PATH];
			_TCHAR fr[MAX_PATH];
			_TCHAR de[MAX_PATH];
		}sztWeapon[MAX_KEY_NUM], sztArmor[MAX_KEY_NUM], sztMagic[3], sztItem[MAX_KEY_NUM];//1.3でsztItemを追加
	}Client[2];//0:MainClient,1:TestClient

			   //コンストラクタ
	CLASS_PATH() {
		ZeroMemory(Client, sizeof(Client));
		GetModuleFileName(NULL, sztIni, sizeof(sztIni));
		PathRenameExtension(sztIni, _T(".ini"));
	}
	int GetPath();
};


class CLASS_TREE
{
#define LPARAM_WEAPON	(0x00000001)
#define LPARAM_ARMOR	(0x00000002)
#define LPARAM_ITEM		(0x00000003)//1.3で追加
#define LPARAM_MAGIC	(0x00000010)
private:
	HWND _hWnd;
public:
	int Init(HWND, CLASS_PATH*);
};


class CLASS_LIST
{
private:
	HWND _hWnd;
public:
	int Init(HWND);
	int Add(CLASS_PATH*, CLASS_ITEM*);
	int AddItem(CLASS_PATH*, CLASS_ITEM*);
	int Add(CLASS_PATH*, CLASS_MAGIC*);
	int Clear();
	int SaveFileWeaponArmor(_TCHAR*);
	int SaveFile(_TCHAR*);
};
