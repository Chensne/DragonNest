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
			// ���� �߰�.
			// Word�� Pos�� ���Ҷ� ���� �»�� ���ؿ��� fX�� ���ؾ��ϴ°� �ƴ϶�,
			// �θ� ���̾�α� �»�� ���ؿ��� fX�� ���ؾ��ϴ� ���̴�.
			// (�̰� ��� ��Ʈ���� fX�� ���Ҷ� ����Ǵ� ����̴�.)
			// �ᱹ �θ� ���̾�α��� DrawDlgText�� ����� �������Ǳ� �����̴�.
			//
			// �׷��� ������ Word�� fX�� ���Ҷ��� ������ fX��ŭ ������ ó���� ������Ѵ�.
			//
			// ���������϶��� Append(�ش��ٿ� �߰�)��, Add(�����ٿ� �߰�)�� ������� �ʰ� �����Ѵ�.
			m_vecWord[i].UpdatePos( m_uiCoord.fX + m_uiCoord.fWidth - m_vecWord[i].m_sProperty.uiCoord.fWidth, fY );
			// Note : ������ �����̶�� ���� ������ Word�� ���Ѵ�.
			return;
		}
		else if( m_vecWord[i].m_sProperty.dwFormat & UITEXT_CENTER )
		{
			// Append�� CENTER���̸� ������ ���Ϳ� �־��ϳ�, ���� ������ ���Ϳ� �־��ϳ�.
			// ���� ������ ���Ϳ� �дٸ� Append���� CENTER��ü�� �ް� �ص� �� ��ü�� �������״�,
			// ���� �������� CENTER�� �ؾ߰ڴ�.
			float fEmptyWidth = m_uiCoord.fWidth - (fX - m_uiCoord.fX);
			float fWordWidth = m_vecWord[i].m_sProperty.uiCoord.fWidth;

			// RIGHT�� �޸� fX���ٰ� �߰��ؾ� ������ ���� ���������� �ʺ� ����ϰ� �ȴ�.
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

