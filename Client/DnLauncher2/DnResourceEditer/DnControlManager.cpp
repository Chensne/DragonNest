#include "stdafx.h"
#include "DnControlManager.h"


DnControlManager::DnControlManager()
: m_pTreeList(NULL), m_pCurrentControl(NULL)
{
	m_szSaveString.Empty();
	m_vecCtrlData.clear();
	m_vecTrashPointerList.clear();
}

DnControlManager::~DnControlManager()
{
	if(!m_vecTrashPointerList.empty())
	{
		std::vector<char*>::iterator it = m_vecTrashPointerList.begin();

		for( ; it != m_vecTrashPointerList.end() ; ++it )
		{
			delete [] ((*it));
			(*it) = NULL;
		}
	}

	if(!m_vecCtrlData.empty())
	{
		std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

		for( ; it != m_vecCtrlData.end() ; ++it )
		{
			SAFE_DELETE((*it).m_WndPT);
		}
	}
}

CWnd* DnControlManager::GetCtrl(CString szString)
{
	CWnd* res = NULL;
	
	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();
	
	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).m_szName.Compare(szString) == 0 )
		{
			res = (*it).m_WndPT;
			break;
		}
	}

	return res;
}
int DnControlManager::GetEnum(CString szString)
{
	int nEnum = -1;

	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).m_szName.Compare(szString) == 0 )
		{
			nEnum = (*it).m_nEnumValue;
			break;
		}
	}

	return nEnum;
}

CWnd* DnControlManager::CheckCollision(POINT pt, int& nEnum)
{
	if( m_vecCtrlData.empty() )
	{
		// 벡터가 비었으면 리턴..
		return NULL;
	}

	RECT	controlRect;
	CWnd*	control = NULL;
	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		control = (*it).m_WndPT;

		if(control != NULL)
		{
			control->GetWindowRect(&controlRect);
			if( PtInRect(&controlRect, pt) )
			{
				nEnum = (*it).m_nEnumValue;
				return control;
			}
		}
	}

	return NULL;
}

HBITMAP DnControlManager::MakeBitmap(CString &szFilePath)
{
	TCHAR szFilter[] = _T("Image Files (*.bmp, *.jpg, *.png) | *.bmp;*.jpg;*.png||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if( dlg.DoModal() == IDOK )
	{
		CxImage xImage;	
		CString szPath		= dlg.GetPathName();
		CString szFileName	= dlg.GetFileName();
		szFilePath = dlg.GetFileName();
		

		// xml에는 이미지의 경로가 "파일이름"만 저장이 되어있다. 
		// 경로는 직접 적어줘야한다. 컴퓨터마다 리소스 폴더의 위치가 틀리기 떄문에.. 
		// 우선 기준점이 되는 이미지의 경로를 이용해서, 지금 불러올 이미지가 test인지, kor인지 확인용으로 저장.
		m_RootPath = szPath;
		
		//--------------------------------------------------
		// bmp파일을 불러왔다면 따로처리한다.
		// CxImage에서 bmp가 로딩이 안된다. 왜이러나.....
		//--------------------------------------------------
		int nFindResult = szFileName.Find(L"bmp");
		if(nFindResult != -1)
		{
			HBITMAP bimtmap = (HBITMAP)LoadImage(NULL , szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			return bimtmap;
		}

		//--------------------------------------------------
		// 나머지 이미지 처리.
		//--------------------------------------------------
		if( !xImage.Load(szPath) )
		{
			return NULL;
		}
		HBITMAP hBmp = xImage.MakeBitmap();
		
		return hBmp;
	}

	return NULL;
}

HRESULT DnControlManager::Save()
{
	TCHAR szFilter[]		= _T("XML Files (*.xml, *.XML) | *.xml;*.XML ||");
	TCHAR szDefaultName[]	= _T("ControlPositionList.xml");
	
	CFileDialog dlg(FALSE, NULL, szDefaultName, OFN_OVERWRITEPROMPT, szFilter);
	
	CString szPathName;
	CString szFileName;

	if( dlg.DoModal() == IDOK )
	{
		szPathName = dlg.GetPathName();
		szFileName = dlg.GetFileName();

		CXMLCreater aParser;
		char		szValue[100]		= { 0 , };
		char		szRectbuff[100]		= { 0 , };
		char		szImageResource[100]= { 0 , };
		char		szName[100]			= { 0 , };

		// 파일 이름 설정.
		int nFindSpot = szPathName.Find(L".");
		if(nFindSpot == -1)
		{
			// 확장자가 없다.
			szPathName+=".xml";
		}
		
		aParser.Init(StringToChar(szFileName), StringToChar(szPathName));
		aParser.AddElement("document",true);

		
		std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();
		for( ; it != m_vecCtrlData.end() ; ++it )
		{
			WINDOWPLACEMENT place;
			(*it).m_WndPT->GetWindowPlacement(&place);

			sprintf(szRectbuff, "%d.%d.%d.%d",	place.rcNormalPosition.left , place.rcNormalPosition.top,
												place.rcNormalPosition.right, place.rcNormalPosition.bottom );

			aParser.AddElement("ContrlListData");
			
			aParser.AddAttribute("RectPos",	szRectbuff);
			aParser.AddAttribute("ImgPath",	StringToChar((*it).m_szResourceFileName));
			aParser.AddAttribute("Type",	(*it).m_nType);
			aParser.AddAttribute("Name",	StringToChar((*it).m_szName));
			aParser.AddAttribute("Enum",	(*it).m_nEnumValue);
		}
		aParser.Create();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT DnControlManager::Load()
{
	// 로딩하기전에 리셋.
	ResetControlData();

	TCHAR szFilter[]		= _T("XML Files (*.xml) | *.XML||");
	TCHAR szDefaultName[]	= _T("ControlPositionList.xml");
	CFileDialog dlg(TRUE, NULL, szDefaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, szFilter);
	if( dlg.DoModal() == IDCANCEL )
	{
		return NULL;
	}

	CString szFullPath	= dlg.GetPathName();
	CString szFileName	= dlg.GetFileName();

	CXMLParser aParser;
	{
		USES_CONVERSION;
		
		WCHAR szRectPos[1024]	= { '\0', };
		WCHAR szImgPath[1024]	= { '\0', };
		WCHAR szType[1024]		= { '\0', };
		WCHAR szName[1024]		= { '\0', };
		WCHAR szEnum[1024]		= { '\0', };

		if( !aParser.Open(StringToChar(szFullPath)) )
		{
			return E_FAIL;
		}

		if( aParser.FirstChildElement("document",true) )
		{
			do
			{
				if( aParser.FirstChildElement("ContrlListData",false) )
				{
					do 
					{
						::wcsncpy_s( szRectPos, _countof(szRectPos),aParser.GetAttribute("RectPos"),_countof(szRectPos) );
						::wcsncpy_s( szImgPath, _countof(szImgPath),aParser.GetAttribute("ImgPath"),_countof(szImgPath) );
						::wcsncpy_s( szType,	_countof(szType),	aParser.GetAttribute("Type"),	_countof(szType) );
						::wcsncpy_s( szName,	_countof(szName),	aParser.GetAttribute("Name"),	_countof(szName) );
						::wcsncpy_s( szEnum,	_countof(szEnum),	aParser.GetAttribute("Enum"),	_countof(szEnum) );
						
						// 컨트롤 벡터 위치 설정.
						if( !SetControlPosition(szRectPos, szImgPath, szName, _wtoi(szType), _wtoi(szEnum)))
						{
							return E_FAIL;
						}
					} while(aParser.NextSiblingElement("ContrlListData"));
				}
			} while(aParser.NextSiblingElement("document"));

		} // end of document
	}

	m_pTreeList->SortChildren(m_hBtnItem);
	m_pTreeList->SortChildren(m_hTextItem);
	m_pTreeList->SortChildren(m_hProgressItem);
	m_pTreeList->SortChildren(m_hCheckBoxItem);

	return S_OK;
}

bool DnControlManager::SetControlPosition(WCHAR* szRectPos, WCHAR* szImgPath, WCHAR* szName, int nType, int nEnum)
{
	//-----------------------------------
	// 1. Rect값 분리하기.
	//-----------------------------------
	RECT				rect;
	std::vector<int>	vecRectPosition;
	WCHAR*				szResult = wcstok(szRectPos, L".");
	do 
	{
		vecRectPosition.push_back(_wtoi(szResult));
		szResult = wcstok(NULL, L".");
	}while (szResult != NULL);
	SetRect(&rect, vecRectPosition[0],vecRectPosition[1],vecRectPosition[2],vecRectPosition[3]);

	// 리소스 정보 셋팅.
	stNewControlData tempData;
	tempData.m_RectPos				= rect;
	tempData.m_nType				= nType;
	tempData.m_szName				= szName;
	tempData.m_nEnumValue			= nEnum;

	// 경로 지정하기.
	if( wcslen(szImgPath) )
	{
		// 이미지 경로 설정.
		CHAR DumpFileName[MAX_PATH];
		::GetModuleFileNameA(NULL, DumpFileName, MAX_PATH);

		CString tempStr = m_RootPath;
		tempStr.MakeLower();

		CT2CA pszConvert(tempStr);
		std::string szPath = pszConvert;

		int nOffset = szPath.rfind("\\");
		if(std::string::npos != nOffset)
		{
			szPath.erase(nOffset);

			USES_CONVERSION;

			std::wstring wszPath( A2W(szPath.c_str()) );

			WCHAR szTempPath[_MAX_PATH] = { 0 , };
			wsprintfW(szTempPath, _T("\\%s"), szImgPath);
			wszPath += szTempPath;

			tempData.m_szResourceFileName = wszPath.c_str();
		}
	}
	else
		tempData.m_szResourceFileName = szImgPath;
	//tempData.m_szResourceFileName = szImgPath;

	m_vecCtrlData.push_back(tempData);


	// 트리 컨트롤에 인서트.
	HTREEITEM hItem = GetParentItem(nType);
	HTREEITEM CurrentItem = m_pTreeList->InsertItem(tempData.m_szName, hItem, TVI_LAST);
	m_pTreeList->SetCheck(CurrentItem, true);
	m_pTreeList->Expand(hItem, TVE_EXPAND);

	return true;
}

char* DnControlManager::StringToChar( CString& str )
{
	long len = str.GetLength();
	len = len * 2;
	char* szTemp = new char[len+1];

	memset(szTemp, 0, len + 1);
	USES_CONVERSION;
	strcpy(szTemp, T2A(str));

	m_vecTrashPointerList.push_back(szTemp);

	return szTemp;
}

bool DnControlManager::SetResourceFilePath( CString szIndex , CString szFilePath )
{
	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).m_szName.Compare(szIndex) == 0 )
		{
			(*it).m_szResourceFileName = szFilePath;

			return true;
		}
	}

	return true;
}

bool DnControlManager::SetStaticText( CString szIndex , CString szStaticText )
{
	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).m_szName.Compare(szIndex) == 0 )
		{
			(*it).m_szName = szStaticText;

			return true;
		}
	}

	return false;
}

bool DnControlManager::ReleaseResourceFilePath( CString szIndex )
{
	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).m_szName.Compare(szIndex) == 0 )
		{
			(*it).m_szResourceFileName = "";

			return true;
		}
	}

	return false;
}

bool DnControlManager::SetCheckBox( CString szIndex, int nCheck)
{
	/*std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		if( (*it).szName.Compare(szIndex) == 0 )
		{
			(*it).nIsVisible = nCheck;
			return true;
		}
	}*/

	return false;
}

HBITMAP DnControlManager::GetBitmap( _CTRL_IT it )
{
	HBITMAP HBitmap = NULL;

	if(! (*it).m_szResourceFileName.IsEmpty() )
	{
		CString szFilePath( (*it).m_szResourceFileName );

		
		int nFindResult = szFilePath.Find(L"bmp");
		if(nFindResult != -1)
		{
			// 확장자가 비트맵이면
			HBitmap = (HBITMAP)LoadImage(NULL , szFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else
		{
			// 다른 확장자들.
			CxImage xImage;
			if( !xImage.Load(szFilePath) )
			{
				szFilePath.Format(_T("파일로드에 실패했습니다.\n%s"), szFilePath);
				AfxMessageBox(szFilePath);
				return NULL;
			}

			HBitmap = xImage.MakeBitmap();
		}

		BITMAP bmp = { 0 };
		GetObject( HBitmap, sizeof(BITMAP), &bmp );
		CButton* pBtn = static_cast<CButton*> ((*it).m_WndPT);
	}

	return HBitmap;
}

//_CTRL_IT DnControlManager::GetVecIterator( CString szString )
bool DnControlManager::GetVecIterator( CString szString, _CTRL_IT &it)
{
	it = std::find(m_vecCtrlData.begin(),m_vecCtrlData.end(),szString);
	if( it != m_vecCtrlData.end() )
	{
		return true;
	}

	return false;
}

HTREEITEM DnControlManager::GetParentItem(int nType)
{
	switch(nType)
	{
	case _TEXT:
		return m_hTextItem;

	case _BUTTON:
		return m_hBtnItem;

	case _PROGRESS_BAR:
		return m_hProgressItem;

	case _CHECK_BOX:
		return m_hCheckBoxItem;

	default:
		break;
	}

	return m_hRootItem;
}


void DnControlManager::CreateControlData(CDialog* pParent)
{
	HBITMAP hBitmap;
	CButton *pButton;
	CStatic *pText;
	CButton *pCheckBox;

	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();
	for( ; it != m_vecCtrlData.end() ; ++it )
	{
		switch((*it).m_nType)
		{
		case _BUTTON:
			pButton = new CButton;
			pButton->Create((*it).m_szName, WS_VISIBLE | WS_CHILD | LBS_NOTIFY | BS_BITMAP , (*it).m_RectPos, pParent, 0);
			pButton->EnableWindow(false);
			pButton->ShowWindow(SW_SHOW);
			hBitmap = GetBitmap(it);
			if(hBitmap)
			{
				pButton->SetBitmap(hBitmap);
			}
			(*it).m_WndPT = pButton;
			break;

		case _TEXT:
			pText = new CStatic;
			pText->Create((*it).m_szName, WS_VISIBLE | SS_LEFTNOWORDWRAP , (*it).m_RectPos, pParent, 0);
			pText->EnableWindow(TRUE);
			pText->ShowWindow(SW_SHOW);
			(*it).m_WndPT = pText;
			break;

		case _PROGRESS_BAR:
			pButton = new CButton;
			pButton->Create((*it).m_szName, WS_VISIBLE | WS_CHILD | LBS_NOTIFY, (*it).m_RectPos, pParent, 0);
			pButton->EnableWindow(false);
			pButton->ShowWindow(SW_SHOW);
			(*it).m_WndPT = pButton;
			break;

		case _CHECK_BOX:
			pCheckBox = new CButton;
			pCheckBox->Create(L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_LEFTTEXT , (*it).m_RectPos, pParent, 0);
			pCheckBox->EnableWindow(false);
			pCheckBox->ShowWindow(SW_SHOW);
			(*it).m_WndPT = pCheckBox;
			break;
		}			
	}
}

void DnControlManager::ResetControlData()
{
	BOOL bFlag		= m_pTreeList->DeleteAllItems();
	m_hRootItem		= m_pTreeList->InsertItem(_T("컨트롤 목록"), TVI_ROOT, TVI_LAST);
	
	m_hBtnItem		= m_pTreeList->InsertItem(_T("BUTTONS"),		m_hRootItem, TVI_LAST);
	m_hTextItem		= m_pTreeList->InsertItem(_T("STATIC_TEXT"),	m_hRootItem, TVI_LAST);
	m_hProgressItem	= m_pTreeList->InsertItem(_T("PROGRESSBAR"),	m_hRootItem, TVI_LAST);
	m_hCheckBoxItem	= m_pTreeList->InsertItem(_T("CHECK_BOX"),	m_hRootItem, TVI_LAST);

	m_pTreeList->Expand(m_hRootItem,	TVE_EXPAND);
	m_pTreeList->Expand(m_hBtnItem,		TVE_EXPAND);
	m_pTreeList->Expand(m_hTextItem,	TVE_EXPAND);
	m_pTreeList->Expand(m_hProgressItem,TVE_EXPAND);
	m_pTreeList->Expand(m_hCheckBoxItem,TVE_EXPAND);


	if(!m_vecTrashPointerList.empty())
	{
		std::vector<char*>::iterator it = m_vecTrashPointerList.begin();

		for( ; it != m_vecTrashPointerList.end() ; ++it )
		{
			delete [] ((*it));
			(*it) = NULL;
		}
	}
	m_vecTrashPointerList.erase(m_vecTrashPointerList.begin(), m_vecTrashPointerList.end());

	
	if(!m_vecCtrlData.empty())
	{
		std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();

		for( ; it != m_vecCtrlData.end() ; ++it )
		{
			SAFE_DELETE((*it).m_WndPT);
		}
	}
	m_vecCtrlData.erase(m_vecCtrlData.begin(), m_vecCtrlData.end());
}

bool DnControlManager::DeleteCtrl(CString szName, HTREEITEM treeItem)
{
	if(m_vecCtrlData.empty())
	{
		return false;
	}

	std::vector<stNewControlData>::iterator it = m_vecCtrlData.begin();
	for( ; it != m_vecCtrlData.end() ; )
	{
		if( (*it).m_szName.Compare(szName) == 0 )
		{
			SAFE_DELETE((*it).m_WndPT);			// 컨트롤 삭제.
			m_pTreeList->DeleteItem(treeItem);	// 리스트 컨트롤 삭제.
			it = m_vecCtrlData.erase(it);		// 벡터 삭제.
			
			return true;
		}
		else
		{
			++it;
		}
	}

	return false;
}

bool DnControlManager::IsLockObject(int nEnumID)
{
	if(!m_vecLockObjects.empty() || nEnumID != -1)
	{
		std::vector<int>::iterator it = m_vecLockObjects.begin();
		for( ; it != m_vecLockObjects.end() ; ++it )
		{
			if( (*it) == nEnumID )
				return true;
		}
	}

	return false;
}

void DnControlManager::SetLockObject(int nEnumID)
{
	// 중복 pushback  방지.
	bool bIsOverLap = false;
	
	std::vector<int>::iterator it = m_vecLockObjects.begin();
	for( ; it != m_vecLockObjects.end() ; ++it )
	{
		if( (*it) == nEnumID )
			bIsOverLap = true;
	}
	
	if(!bIsOverLap)
		m_vecLockObjects.push_back(nEnumID);
}

void DnControlManager::SetUnLockObject(int nEnumID)
{
	if(!m_vecLockObjects.empty())
	{
		std::vector<int>::iterator it = remove(m_vecLockObjects.begin(), m_vecLockObjects.end(), nEnumID);
		m_vecLockObjects.erase(it);
	}
}