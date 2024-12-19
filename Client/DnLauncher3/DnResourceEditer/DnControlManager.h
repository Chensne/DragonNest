#pragma once


enum
{
	_BUTTON = 0,
	_TEXT,
	_PROGRESS_BAR,
	_CHECK_BOX,

	_CTRL_MAX
};

struct stNewControlData
{
	CWnd*		m_WndPT;
	RECT		m_RectPos;
	CString		m_szName;
	CString		m_szResourceFileName;
	CString		m_szOnlyFileName;
	int			m_nType;
	int			m_nEnumValue;

	stNewControlData()
	: m_WndPT(NULL), m_nType(0), m_nEnumValue(0)
	{
		m_szResourceFileName.Empty();
		m_szName.Empty();
		m_szOnlyFileName.Empty();
	}

	bool operator ==(const CString szData)
	{
		if(szData == m_szName)
		{
			return true;
		}
		return false;
	}
};


#define _CTRL_IT		std::vector<stNewControlData>::iterator


class DnControlManager
{
private:
	DnControlManager();
	~DnControlManager();

	CString							m_szSaveString; // 변경된 에디트 텍스트 저장 버퍼
	CTreeCtrl*						m_pTreeList;
	std::vector<char*>				m_vecTrashPointerList;

public:
	CWnd*							m_pCurrentControl;	// 현재 선택된 컨트롤.
	
	HTREEITEM						m_hBtnItem;
	HTREEITEM						m_hTextItem;
	HTREEITEM						m_hProgressItem;
	HTREEITEM						m_hCheckBoxItem;
	HTREEITEM						m_hRootItem;

	RECT							m_RectData;
	CString							m_RootPath;

	// dynamic Control Object 관련..!!!
	stNewControlData				m_DynamicControlData;
	std::vector<stNewControlData>	m_vecCtrlData;

	// 잠금 컨트롤 오브젝트
	std::vector<int>				m_vecLockObjects;

	static DnControlManager& GetInstance()
	{
		static DnControlManager obj;
		return obj;
	}

	void			SetEditString( CString sz )			{ m_szSaveString = sz; }
	CString			GetEditString()						{ return m_szSaveString; }
	void			SetTreeListPointer(CTreeCtrl* pt)	{ m_pTreeList = pt;		 }
	void			SetCurrentControl(CWnd* p)			{ m_pCurrentControl = p; }
	
	int				GetEnum(CString szString);
	CWnd*			GetCtrl(CString szString);
	bool			DeleteCtrl(CString szName, HTREEITEM treeItem);
	CWnd*			CheckCollision(POINT pt, int& nEnum);

	HBITMAP			GetBitmap(_CTRL_IT it);
	HBITMAP			MakeBitmap(CString &szFilePath);

	HRESULT			Save();
	HRESULT			Load();

	bool			SetResourceFilePath(CString szIndex , CString szFilePath);
	bool			ReleaseResourceFilePath(CString szIndex);

	bool			SetCheckBox(CString szIndex, int nCheck);
	bool			SetStaticText(CString szIndex , CString szStaticText);
	bool			SetControlPosition(WCHAR* szRectPos, WCHAR* szImgPath, WCHAR* szName, int nType, int nEnum);
	
	char*			StringToChar(CString& str);

	//_CTRL_IT		GetVecIterator(CString szString);
	bool			GetVecIterator(CString szString, _CTRL_IT &it);
	HTREEITEM		GetParentItem(int nType);

	void			CreateControlData(CDialog* pParent);
	void			ResetControlData();

	void			SetLockObject(int nEnumID);
	void			SetUnLockObject(int nEnumID);
	bool			IsLockObject(int nEnumID);
};