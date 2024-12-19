#include "StdAfx.h"
#include "EtUITextBoxData.h"
#include "EtTexture.h"
#include "EternityEngine.h"
#include "EtResourceMng.h"

// Note : Class Word
//
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CWord::CWord()
{
}

CWord::~CWord()
{
}

void CWord::UpdatePos( float fX, float fY )
{
	m_sProperty.uiCoord.fX = fX;
	m_sProperty.uiCoord.fY = fY;
}

void CWord::Clear()
{
	m_sProperty.Clear();
	m_strWord.clear();
	m_strWordWithTag.clear();
}

/////////////////////////////////////////////////////////////////


// Note : Class Line
//
CLine::CLine()
	: m_fLineSpace(0.0f)
	, m_bSelected(false)
	, m_bMouseInLine(false)
	, m_uiCoord( 0.f, 0.f, 0.f, 0.f )
{
}

CLine::~CLine()
{
	SAFE_DELETE_VEC( m_vecWord );
}

void CLine::SetLineSpace( float fLineSpace )
{
	m_uiCoord.fHeight -= GetLineSpace();
	this->m_fLineSpace = fLineSpace;
	m_uiCoord.fHeight += GetLineSpace();
}

void CLine::UpdatePos( float fX, float fY )
{
	m_uiCoord.fX = fX;
	m_uiCoord.fY = fY;

	fY += GetLineSpace();

	for( int i=0; i<(int)m_vecWord.size(); i++ )
	{
		if( m_vecWord[i].m_sProperty.dwFormat & UITEXT_RIGHT )
		{
			// 버그 발견.
			// Word의 Pos를 정할때 라인 좌상단 기준에서 fX를 구해야하는게 아니라,
			// 부모 다이얼로그 좌상단 기준에서 fX를 구해야하는 것이다.
			// (이건 모든 컨트롤의 fX를 정할때 적용되는 사실이다.)
			// 결국 부모 다이얼로그의 DrawDlgText를 사용해 렌더링되기 때문이다.
			//
			// 그렇기 때문에 Word의 fX를 정할때는 라인의 fX만큼 오프셋 처리를 해줘야한다.
			//
			// 우측정렬일때는 Append(해당줄에 추가)나, Add(다음줄에 추가)나 상관하지 않고 결정한다.
			m_vecWord[i].UpdatePos( m_uiCoord.fX + m_uiCoord.fWidth - m_vecWord[i].m_sProperty.uiCoord.fWidth, fY );
			// Note : 오른쪽 정렬이라는 것은 마지막 Word를 뜻한다.
			return;
		}
		else if( m_vecWord[i].m_sProperty.dwFormat & UITEXT_CENTER )
		{
			// Append에 CENTER붙이면 라인의 센터에 둬야하나, 남은 공간의 센터에 둬야하나.
			// 만약 라인의 센터에 둔다면 Append에서 CENTER자체를 받게 해둔 거 자체가 오류일테니,
			// 남는 공간에서 CENTER에 해야겠다.
			float fEmptyWidth = m_uiCoord.fWidth - (fX - m_uiCoord.fX);
			float fWordWidth = m_vecWord[i].m_sProperty.uiCoord.fWidth;

			// RIGHT와 달리 fX에다가 추가해야 좌측에 뭔가 적혔을때의 너비를 기억하게 된다.
			m_vecWord[i].UpdatePos( fX + (fEmptyWidth - fWordWidth)*0.5f, fY );
		}
		else
		{
			m_vecWord[i].UpdatePos( fX, fY );
		}

		fX = m_vecWord[i].m_sProperty.uiCoord.Right();
	}
}

std::wstring CLine::GetText()
{
	std::wstring strTemp;

	for( int i=0; i<(int)m_vecWord.size(); i++ )
	{
		strTemp += m_vecWord[i].m_strWord;
	}

	return strTemp;
}

CImageLine::CImageLine()
{
	m_hTexture;
	std::string m_szFileName;
}

CImageLine::~CImageLine()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CImageLine::SetImage( WCHAR *wszFileName, SUICoord &Coord )
{
	ToMultiString( wszFileName, m_szFileName );
	SetImage( (char*)m_szFileName.c_str(), Coord );
}

void CImageLine::SetImage( char *szFileName, SUICoord &Coord )
{
	m_szFileName = szFileName;
	m_hTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	m_uiUV = Coord;
}

EtTextureHandle CImageLine::GetTexture()
{
	return m_hTexture;
}

