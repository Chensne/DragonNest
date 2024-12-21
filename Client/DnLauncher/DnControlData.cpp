#include "stdafx.h"
#include "DnControlData.h"

CDnControlData::CDnControlData()
{
	m_vecCtrlPosData.clear();
	m_szLanguageParam.Empty();
}

CDnControlData::~CDnControlData()
{
}

HRESULT CDnControlData::SetControlDataFromLocalFile( CString szFilePath, CRect* pRect, int nMaxCnt )
{
	char* pBuff = new char[szFilePath.GetLength() + 1];
	if(!pBuff) return E_FAIL;

	strncpy(pBuff, CT2A(szFilePath), szFilePath.GetLength() + 1);
	HRESULT hResult = S_OK;

	// xml파일을 읽어온다.
	if( !LoadXMLFile(pBuff) )
	{
		LogWnd::TraceLog(_T("ControlData XML파일 불러오기 실패!!"));
		hResult = E_FAIL;
	}

	// 컨트롤에 위치를 잡아준다.
	if (hResult == S_OK)
	{
		if( !SetPositionData( pRect, nMaxCnt ) )
		{
			LogWnd::TraceLog(_T("ControlData Position Set Failed"));
			hResult = E_FAIL;
		}
	}

	if (pBuff)
	{
		delete [] pBuff;
	}
	return hResult;
}

HRESULT CDnControlData::SetControlDataFromResource( int nResourceID, LPCTSTR szFileName, CRect* pRect, int nMaxCnt )
{
	// xml파일을 읽어온다.
	if( !LoadResourceFromXMLFile(nResourceID, szFileName) )
	{
		LogWnd::TraceLog(_T("ControlData ResourceXML파일 불러오기 실패!!"));
		return E_FAIL;
	}

	// 컨트롤에 위치를 잡아준다.
	if( !SetPositionData( pRect, nMaxCnt ) )
	{
		LogWnd::TraceLog(_T("ControlData Position Set Failed"));
		return E_FAIL; 
	}

	return S_OK;
}

bool CDnControlData::LoadXMLFile( char* szFilePath )
{
	if( szFilePath == NULL )
	{
		return false;
	}

	// 버퍼 클리어.
	m_vecCtrlPosData.clear();

	CXMLParser aParser;
	{
		USES_CONVERSION;

		RECT RectPos;
		WCHAR szRect[1024]		= { '\0', };
		WCHAR szType[1024]		= { '\0', };
		WCHAR szEnum[1024]		= { '\0', };

		stControlPositionData	tempData;
		if( !aParser.Open(szFilePath) )
		{
			return false;
		}

		if( aParser.FirstChildElement("document",true) )
		{
			do
			{
				if( aParser.FirstChildElement("ContrlListData",false) )
				{
					do 
					{
						::wcsncpy_s( szRect, _countof(szRect),	aParser.GetAttribute("RectPos"),_countof(szRect) );
						::wcsncpy_s( szType, _countof(szType),	aParser.GetAttribute("Type"),	_countof(szType) );
						::wcsncpy_s( szEnum, _countof(szEnum),	aParser.GetAttribute("Enum"),	_countof(szEnum) );

						ParsingRectData(szRect, RectPos);
						tempData.nType	 = _wtoi(szType);
						tempData.nEnum	 = _wtoi(szEnum);
						tempData.RectPos = RectPos;

						m_vecCtrlPosData.push_back(tempData);

					} while(aParser.NextSiblingElement("ContrlListData"));
				}
			} while(aParser.NextSiblingElement("document"));
		} // end of document
	}

	if(m_vecCtrlPosData.empty())
	{
		return false;
	}

	return true;
}

bool CDnControlData::LoadResourceFromXMLFile(int nID, LPCTSTR szFileName)
{
	// 리소스 읽어오기.
	HMODULE hModule = ::GetModuleHandle(NULL);
	HRSRC	hSrc	= ::FindResource( hModule, MAKEINTRESOURCE(nID), _T("XML"));
	HGLOBAL hGlobal = ::LoadResource( hModule, hSrc );
	char*	szRes	= static_cast<char*>(::LockResource(hGlobal));
	LPCTSTR myStr	= szFileName;
	
	CFile file( myStr, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	file.Write((void*)szRes, SizeofResource(AfxGetInstanceHandle(),(HRSRC)hSrc));
	file.Close();
	::FreeResource(hGlobal);
	
	CString strFileName = szFileName;
	char szOpenFileName[_MAX_PATH] = { 0 , };
	strncpy(szOpenFileName , CT2A(strFileName), _MAX_PATH);

	// 버퍼 클리어.
	m_vecCtrlPosData.clear();

	CXMLParser aParser;
	{
		USES_CONVERSION;

		RECT RectPos;
		WCHAR szRect[1024] = { '\0', };
		WCHAR szType[1024] = { '\0', };
		WCHAR szEnum[1024] = { '\0', };

		stControlPositionData tempData;

		if( !aParser.Open(szOpenFileName) )
		{
			ClientDeleteFile(myStr);
			return false;
		}

		if( aParser.FirstChildElement("document",true) )
		{
			do
			{
				if( aParser.FirstChildElement("ContrlListData",false) )
				{
					do 
					{
						::wcsncpy_s( szRect, _countof(szRect),	aParser.GetAttribute("RectPos"),_countof(szRect) );
						::wcsncpy_s( szType, _countof(szType),	aParser.GetAttribute("Type"),	_countof(szType) );
						::wcsncpy_s( szEnum, _countof(szEnum),	aParser.GetAttribute("Enum"),	_countof(szEnum) );

						ParsingRectData(szRect, RectPos);
						tempData.nType	 = _wtoi(szType);
						tempData.nEnum	 = _wtoi(szEnum);
						tempData.RectPos = RectPos;

						m_vecCtrlPosData.push_back(tempData);

					} while(aParser.NextSiblingElement("ContrlListData"));
				}
			} while(aParser.NextSiblingElement("document"));
		} // end of document
	}

	ClientDeleteFile(myStr);

	if(m_vecCtrlPosData.empty())
	{
		return false;
	}

	return true;
}

bool CDnControlData::SetPositionData( CRect* pRt, int nMax )
{
	if( m_vecCtrlPosData.empty())
	{
		return false;
	}

	RECT rect;
	for( int index = 0 ; index < nMax ; ++index )
	{
		rect = GetCtrlRect(index);
		SetRect( pRt[index] , rect.left, rect.top, rect.right, rect.bottom);
	}

	return true;
}

void CDnControlData::ParsingRectData( WCHAR* szPosition, RECT& rt )
{
	std::vector<int> vecRectPosition;

	WCHAR* szResult = wcstok(szPosition, L".");
	do 
	{
		vecRectPosition.push_back(_wtoi(szResult));
		szResult = wcstok(NULL, L".");
	} while (szResult != NULL);

	SetRect(&rt, vecRectPosition[0],vecRectPosition[1],vecRectPosition[2],vecRectPosition[3]);
}

RECT CDnControlData::GetCtrlRect(int enumIndex)
{
	std::vector<stControlPositionData>::iterator it = m_vecCtrlPosData.begin();

	for( ; it != m_vecCtrlPosData.end() ; ++it )
	{
		if( enumIndex == (*it).nEnum )
		{
			return (*it).RectPos;
		}
	}

	CRect rt(0,0,0,0);
	return rt;
}

CString CDnControlData::GetFilePath( WCHAR* szFilePath )
{
	CString szString;
	WCHAR szBuff[_MAX_PATH] = { 0 ,};
	GetModuleFileName(AfxGetInstanceHandle(), szBuff, sizeof(szBuff));
	szString = szBuff;

	int nPos = szString.Find(L"Output");
	if(nPos == -1)
	{
		nPos = szString.Find(L"output");
	}

	szString = szString.Left(nPos);
	szString = szString + (szFilePath);

	return szString;
}

CString CDnControlData::GetMulitiLanguageFilePath()
{
	CString szFinalName;
	if( !m_szLanguageParam.IsEmpty() )
	{
		szFinalName += "ControlPositionList_";
		szFinalName += m_szLanguageParam;
		szFinalName += ".xml";
	}
	else
	{
		szFinalName += "ControlPositionList.xml";
	}
	
	return szFinalName;
}

int	CDnControlData::GetMulitiLanguageResourceID()
{
	int nResultID = -1;

	if( m_szLanguageParam == L"FRA" )
	{
		nResultID = 17003;	
	}
	else if( m_szLanguageParam == L"ESP" )
	{
		nResultID = 17002;
	}
	else if( m_szLanguageParam == L"GER" )
	{
		nResultID = 17004;
	}
	else
	{
		nResultID = 17001;
	}

	return nResultID;
}