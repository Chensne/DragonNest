#include "StdAfx.h"
#include "EtUIDialog.h"
#include "EtUIControl.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// elkian03 나중에 여기서 빼자
#include "EtUITextUtil.h"


bool CEtUIControl::s_bFocusEditBox = false;

CEtUIControl::CEtUIControl( CEtUIDialog *pParent )
{
	m_bEnable = true;
	m_bShow = true;
	m_bKeyLock = true; 
	m_bFocus = false;
	m_bMouseEnter = false;
	m_bPressed = false;
	m_bRightPressed = false;
	m_fBlendRate = 0.8f;
	m_CurrentState = UI_STATE_NORMAL;
	
	m_pParent = pParent;

	m_Property.nID = -1;
	m_Property.UIType = UI_CONTROL_STATIC;

	m_bFirstRender = true;
	m_fZValue = 0.f;

	m_bExistTemplateTexture = false;
}

CEtUIControl::~CEtUIControl(void)
{
}

bool CEtUIControl::IsInside( float fX, float fY )
{
	return m_Property.UICoord.IsInside(fX, fY);
}

float CEtUIControl::FloatXCoord( int nX )
{
	return nX / m_pParent->GetScreenWidth();
}

float CEtUIControl::FloatYCoord( int nY )
{
	return nY / m_pParent->GetScreenHeight();
}

int CEtUIControl::IntXCoord( float fX )
{
	return ( int )( fX * m_pParent->GetScreenWidth() );
}

int CEtUIControl::IntYCoord( float fY )
{
	return ( int )( fY * m_pParent->GetScreenHeight() );
}

void CEtUIControl::Initialize( SUIControlProperty *pProperty )
{
	if( pProperty )
	{
		m_Property.Assign( pProperty );
		SetTemplate( m_Property.nTemplateIndex );
	}

	switch( m_Property.nInitState )
	{
	case 1:	Show( false );		break;
	case 2:	Enable( false );	break;
	}

	m_BaseDlgCoord = m_Property.UICoord;

	m_bExistTemplateTexture = ( m_Template.m_hTemplateTexture ) ? true : false;

	UpdateRects();
}

void CEtUIControl::SetTemplate( int nTemplateIndex )
{
	CEtUITemplate *pTemplate(NULL);
	pTemplate = m_pParent->GetTemplate( nTemplateIndex );
	if( !pTemplate ) return;

	m_Template = *pTemplate;
}

void CEtUIControl::Render( float fElapsedTime )
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, m_Property.UICoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, m_Property.UICoord );
}

void CEtUIControl::UpdateUICoord( float fX, float fY, float fWidth, float fHeight )
{
	m_Property.UICoord.fX += fX;
	m_Property.UICoord.fY += fY;
	m_Property.UICoord.fWidth += fWidth;
	m_Property.UICoord.fHeight += fHeight;

	UpdateRects();
}

// Note : 아래 함수는 컨트롤들이 처음 랜더 될때 블랜드 상태때문에 번쩍이는 부분을 없애기 위해 호출한다.
//
void CEtUIControl::UpdateBlendRate()
{
	if( m_CurrentState == UI_STATE_PRESSED )
	{
		m_fBlendRate = 0.0f;
	}
	else
	{
		if( m_bFirstRender )
		{
			m_fBlendRate = 0.0f;
			m_bFirstRender = false;
		}
		else
		{
			m_fBlendRate = 0.8f;
		}
	}
}

SUIElement* CEtUIControl::GetElement( int nIndex )
{
	//ASSERT( (nIndex < (int)m_Template.m_vecElement.size()) && "CEtUIControl::GetElement, Invalid Index" );
	if( (nIndex >= (int)m_Template.m_vecElement.size()) || (nIndex < 0) )
	{
		//CDebugSet::ToLogFile( "CEtUIControl::GetElement, ControlName(%s), Invalid Index(%d)", GetControlName(), nIndex );
		return NULL;
	}

	return &m_Template.m_vecElement[nIndex];
}

void CEtUIControl::OnChangeResolution()
{
	UpdateRects();
}

int CEtUIControl::GetTextToInt()
{
	return _wtoi( m_szText.c_str() );
}

void CEtUIControl::SetIntToText( int nValue )
{
	WCHAR wszBuffer[256]={0};
	_itow( nValue, wszBuffer, 10 );
	SetText( wszBuffer );
}

INT64 CEtUIControl::GetTextToInt64()
{
	return _wtoi64( m_szText.c_str() );
}

void CEtUIControl::SetInt64ToText( INT64 nValue )
{
	WCHAR wszBuffer[256]={0};
	_i64tow( nValue, wszBuffer, 10 );
	SetText( wszBuffer );
}

void CEtUIControl::SetText( const std::wstring &strText, bool bAutoFit, float fScaleX, float fScaleY )
{ 
	m_strRenderText = m_szText = strText; 
}

// #ifdef PRE_ADD_CASHSHOP_SUBCAT_COLOR
void CEtUIControl::SetTextColor( const std::wstring StrColor, UI_CONTROL_STATE ControlState, int nElement )
{
	if( StrColor.empty() )
		return;

	DWORD		dwTextColor = 0;

	// ex) StrClolr : "0:r" << 문자열의 2번째 인덱스 
	switch( StrColor[2] )
	{
		case 'r':	dwTextColor = descritioncolor::RED;			break;	// 레드
		case 'g':	dwTextColor = descritioncolor::GREEN;		break;	// 그린
		case 'b':	dwTextColor = descritioncolor::DODGERBLUE;	break;	// 블루
		case 'y':	dwTextColor = descritioncolor::YELLOW1;		break;	// 노란
		case 'e':	dwTextColor = descritioncolor::YELLOW2;		break;	// 노란
		case 'v':	dwTextColor = descritioncolor::VIOLET;		break;	// 보라
		case 's':	dwTextColor = descritioncolor::SKY;			break;	// 하늘
		case 'j':	dwTextColor = descritioncolor::ORANGE;		break;	// 주황
		case 'w':	dwTextColor = descritioncolor::WHITE;		break;	// 흰
		case 'z':	dwTextColor = 0xFFFFD405;					break;	// 캐쉬샵 베이스 노랑 
		default:	dwTextColor = 0xffffffff;					break;	// 인식 안되는녀석은 흰색 
	}
	
	SetTextColor( dwTextColor, ControlState, nElement );
}
void CEtUIControl::SetTextColor( const DWORD TextColor, UI_CONTROL_STATE ControlState, int nElement )
{
	SUIElement* pElement(NULL);
	pElement = GetElement(nElement);

	if( pElement )
	{
		pElement->FontColor.dwColor[ ControlState ] = TextColor;
	}
}

//#endif


void CEtUIControl::FindInputPos( std::vector<EtVector2> &vecPos )
{
	CEtUIDialog *pDialog = GetParent();
	if( !pDialog ) return;

	float fPosX = pDialog->GetXCoord() + GetUICoord().fX + GetUICoord().fWidth / 2.0f;
	float fPosY = pDialog->GetYCoord() + GetUICoord().fY + GetUICoord().fHeight / 2.0f;

	vecPos.push_back( EtVector2(fPosX, fPosY) );
}