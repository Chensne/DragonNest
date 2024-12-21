#include "StdAfx.h"
#include "UIToolTemplate.h"
#include "RenderBase.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIToolTemplate::CUIToolTemplate(void)
{
	m_fDefaultWidth = 0.0f;
	m_fDefaultHeight = 0.0f;
	m_pExternControlTemplate = NULL;
	m_bChanged = false;
	memset( &m_ControlInfo, 0, sizeof( SUIControlInfo ) );
}

CUIToolTemplate::~CUIToolTemplate(void)
{
	if( m_bChanged )
	{
		char szString[ 1024 ];

		sprintf_s( szString, 1024, "%s Not Saved... Save?", m_Template.m_szTemplateName.c_str() );
		if( MessageBox( NULL, szString, "", MB_OKCANCEL ) == IDOK )
		{
			char szFileName[ _MAX_PATH ];

			sprintf_s( szFileName, _MAX_PATH, "%s\\%s.uit", CMainFrame::GetUITemplateFolder(), m_Template.m_szTemplateName.c_str() );

			CFileStream Stream( szFileName, CFileStream::OPEN_WRITE );
			if( !Save( Stream ) )
			{
				sprintf_s( szString, 1024, "%s.uit File Save Failed", m_Template.m_szTemplateName.c_str() );
				MessageBox( NULL, szString, "", MB_OK );
			}

			if( m_Template.m_hTemplateTexture )
			{
				sprintf_s( szFileName, _MAX_PATH, "%s\\UITemplateTexture\\UIT_%s.dds", CMainFrame::GetResourceFolder(), m_Template.m_szTemplateName.c_str() );
				D3DXSaveTextureToFile( szFileName, D3DXIFF_DDS, m_Template.m_hTemplateTexture->GetTexturePtr(), NULL );
			}
		}
	}
}

void CUIToolTemplate::AddUIElement( const char *pTextureName, const int nFontSetIndex, const char *pElementName, SUIElement &Element )
{
	m_vecTextureName.push_back( pTextureName );
	m_vecFontSetIndex.push_back( nFontSetIndex );
	m_vecElementName.push_back( pElementName );
	m_Template.m_vecElement.push_back( Element );
}

void CUIToolTemplate::DelUIElement( CString &strElementName )
{
	//for( int i=0; i<(int)m_vecElementName.size(); i++ )
	//{
	//	if( strElementName.Compare(m_vecElementName[i].c_str()) == 0 )
	//	{
	//		std::vector< std::string >::iterator iter = m_vecTextureName.begin();
	//		for(int j=0;j<i;j++ ) iter++;
	//		m_vecTextureName.erase(iter);

	//		iter = m_vecFontName.begin();
	//		for(int j=0;j<i;j++ ) iter++;
	//		m_vecFontName.erase(iter);

	//		iter = m_vecElementName.begin();
	//		for(int j=0;j<i;j++ ) iter++;
	//		m_vecElementName.erase(iter);

	//		std::vector< SUIElement >::iterator iter1 = m_Template.m_vecElement.begin();
	//		for(int j=0;j<i;j++ ) iter1++;
	//		m_Template.m_vecElement.erase(iter1);
	//	}
	//}
}

bool CUIToolTemplate::Load( CStream &Stream )
{
	if( !Stream.IsValid() )
		return false;

	char szReserved[ UIT_HEADER_RESERVED ]={0};
	SUITemplateFileHeader Header;

	Stream.ReadBuffer( &Header, sizeof( SUITemplateFileHeader ) );
	Stream.ReadBuffer( szReserved, UIT_HEADER_RESERVED );

	switch( Header.nVersion )
	{
	case UIT_FILE_VERSION_01: Load_01( Stream ); break;
	case UIT_FILE_VERSION_02: Load_02( Stream ); break;
	case UIT_FILE_VERSION:
		{
			Stream >> m_UIType;
			Stream >> m_vecTextureName;
			Stream >> m_vecFontSetIndex;
			Stream >> m_vecElementName;
			Stream >> m_fDefaultWidth;
			Stream >> m_fDefaultHeight;

			m_ControlInfo.Load( Stream, m_UIType );
			m_Template.Load( Stream );

			Stream >> m_szExternalControlName;
		}
		break;
	default:
		CDebugSet::ToLogFile( "CUIToolTemplate::Load, Invalid UITool Template Version(%d)!", Header.nVersion );
		break;
	}

	return true;
}

bool CUIToolTemplate::Save( CStream &Stream )
{
	if( !Stream.IsValid() )
		return false;

	char szReserved[ UIT_HEADER_RESERVED ]={0};
	SUITemplateFileHeader Header;

	Stream.WriteBuffer( &Header, sizeof( SUITemplateFileHeader ) );
	Stream.WriteBuffer( szReserved, UIT_HEADER_RESERVED );

	// eye-ad�� �����ϴ� ��� ��ε� �� R�� �ٲٱ�.
	//for(int i = 0; i < m_vecTextureName.size(); ++i)
	//{
	//	std::string szName = m_vecTextureName[i];

	//	std::string strTemp(szName);
	//	std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

	//	int index = strTemp.find_first_of("\\\\eye-ad\\tooldata\\uitemplate\\");
	//	if( index == 0 )
	//	{
	//		szName = szName.substr(strlen("\\\\eye-ad\\tooldata\\uitemplate\\"), m_vecTextureName[i].size());
	//		//m_vecTextureName[i] = szName;
	//		m_vecTextureName[i] = "R:\\Plan\\4.WorkSheet\\UITemplate\\" + szName;
	//	}
	//	else if( index == std::string::npos )
	//	{
	//		// �ƴѰ͵� �ֳ�����.
	//		int i = 0;
	//		++i;
	//	}
	//}

	// �ٽ� ��θ� �ٲ����. R:\Plan\4.WorkSheet ���� R:\Gameres\WorkSheet ��.
	//for(int i = 0; i < m_vecTextureName.size(); ++i)
	//{
	//	std::string szName = m_vecTextureName[i];

	//	std::string strTemp(szName);
	//	std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

	//	int index = strTemp.find_first_of("R:\\Plan\\4.WorkSheet\\UITemplate\\");
	//	if( index == 0 )
	//	{
	//		szName = szName.substr(strlen("R:\\Plan\\4.WorkSheet\\UITemplate\\"), m_vecTextureName[i].size());
	//		//m_vecTextureName[i] = szName;
	//		m_vecTextureName[i] = "R:\\Gameres\\WorkSheet\\UITemplate\\" + szName;
	//	}
	//	else if( index == std::string::npos )
	//	{
	//		// �ƴѰ͵� �ֳ�����.
	//		int i = 0;
	//		++i;
	//	}
	//}

	Stream << m_UIType;
	Stream << m_vecTextureName;
	Stream << m_vecFontSetIndex;
	Stream << m_vecElementName;
	Stream << m_fDefaultWidth;
	Stream << m_fDefaultHeight;

	m_ControlInfo.Save( Stream, m_UIType );
	m_Template.Save( Stream );

	if( m_pExternControlTemplate )
	{
		Stream << m_szExternalControlName;
	}
	else
	{
		Stream << 0;
	}

	return true;
}

bool CUIToolTemplate::Load_01( CStream &Stream )
{
	Stream >> m_UIType;
	Stream >> m_vecTextureName;
	int nVecSize = Stream.SeekCur_VecString<char>();
	Stream >> m_vecElementName;
	Stream >> m_fDefaultWidth;
	Stream >> m_fDefaultHeight;

	m_ControlInfo.Load( Stream, m_UIType );
	m_Template.Load( Stream );

	Stream >> m_szExternalControlName;

	// Note : ���� �߰��� ���� �ʱ�ȭ
	m_vecFontSetIndex.resize(nVecSize);

	return true;
}

bool CUIToolTemplate::Load_02( CStream &Stream )
{
	Stream >> m_UIType;
	Stream >> m_vecTextureName;
	int nVecSize = Stream.SeekCur_VecString<char>();
	Stream >> m_vecElementName;
	Stream >> m_fDefaultWidth;
	Stream >> m_fDefaultHeight;

	m_ControlInfo.Load( Stream, m_UIType );
	m_Template.Load( Stream );

	Stream >> m_szExternalControlName;

	// Note : ���� �߰��� ���� �ʱ�ȭ
	m_vecFontSetIndex.resize(nVecSize);

	return true;
}