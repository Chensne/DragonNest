#include "StdAfx.h"
#include "DnCountCtl.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCountCtl::CDnCountCtl(CEtUIDialog *pParent)
	: CEtUIControl( pParent )
	, m_nValue(0)
{
	m_fZValue = 0.f;
	m_fScale = 1.f;
	m_fFontSize = 0.8f;	
	m_bUseCustomColor = false;
	m_bPlus = false;
	m_bMinus = false;
	m_cAlpha = 255;
	m_bRightAlign = false;
	m_bChangeFont = false;
}

CDnCountCtl::~CDnCountCtl(void)
{
}

void CDnCountCtl::Render( float fElapsedTime )
{
	SUIElement *pElement(NULL);	
	float fWidth(0.0f);
	int nIndex(-1);

	UI_CONTROL_STATE CurrentState = UI_STATE_NORMAL;
	if( !IsShow() )
	{
		CurrentState = UI_STATE_HIDDEN;
		m_Color.Blend( CurrentState, fElapsedTime, 0.0f );
	}
	else
		m_Color.Blend( CurrentState, fElapsedTime, 0.8f );

	DWORD dwColor = m_Color.dwCurrentColor;

	if( m_bUseCustomColor ) {
		EtColor Color(dwColor);
		EtColor CustomColor(m_Property.CustomProperty.dwColor);
		CustomColor.a *= Color.a * (m_cAlpha/255.f);
		dwColor = CustomColor;
	}

	if( m_bRightAlign ) {
		for( int i=0; i<(int)m_vecValueIndex.size() ; i++ )
		{
			nIndex = m_vecValueIndex[i];
			pElement = GetElement(nIndex);
			if( pElement == NULL ) continue;

			SUICoord uiCoord;
			GetUICoord(uiCoord);		// added by kalliste 090824
			uiCoord.fX -= fWidth;

			uiCoord.SetSize( pElement->fTextureWidth * m_fFontSize * m_fScale, pElement->fTextureHeight * m_fFontSize * m_fScale);
			fWidth += pElement->fTextureWidth * m_fFontSize * m_fScale;

			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, uiCoord, 0.f, m_fZValue );		
			else
				m_pParent->DrawSprite( pElement->UVCoord, dwColor, uiCoord, 0.f, m_fZValue );		
		}
	}
	else {
		for( int i=(int)m_vecValueIndex.size()-1; i>=0 ; i-- )
		{
			nIndex = m_vecValueIndex[i];
			pElement = GetElement(nIndex);
			if( pElement == NULL ) continue;

			SUICoord uiCoord;
			GetUICoord(uiCoord);		// added by kalliste 090824
			uiCoord.fX += fWidth;
			
			uiCoord.SetSize( pElement->fTextureWidth * m_fFontSize * m_fScale, pElement->fTextureHeight * m_fFontSize * m_fScale);
			fWidth += pElement->fTextureWidth * m_fFontSize * m_fScale;
			
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, uiCoord, 0.f, m_fZValue );
			else
				m_pParent->DrawSprite( pElement->UVCoord, dwColor, uiCoord, 0.f, m_fZValue );		
		}
	}
}

void CDnCountCtl::Init()
{
	m_nValue = 0;	
	m_vecValueIndex.clear();
	m_bPlus = false;
	m_bMinus = false;
	m_Color.dwCurrentColor = m_Color.dwColor[ UI_STATE_NORMAL ];
	m_fZValue = 0.f;
	m_fScale = 1.f;
	m_fFontSize = 0.8f;	
	m_bUseCustomColor = false;	
	m_cAlpha = 255;
}

void CDnCountCtl::SetValue( int nValue )
{
	m_nValue = nValue;
	m_vecValueIndex.clear();
	
	if( nValue < 0 && m_bMinus )
		m_nValue *= -1;
	do 
	{
		if( m_bChangeFont )
		{
			const int ADDFONTSTARTINDEX = 29;
			m_vecValueIndex.push_back( ADDFONTSTARTINDEX + (m_nValue % 10) );
		}
		else
			m_vecValueIndex.push_back( m_nValue % 10 );
		
		m_nValue = m_nValue / 10;

	} while( m_nValue );

	if( m_bPlus ) 
	{
		m_vecValueIndex.push_back( 13 ); // add PLUS
	}
	else if( m_bMinus )
	{
		m_vecValueIndex.push_back( 12 ); // add Minus
	}
}

float CDnCountCtl::GetControlWidth()
{
	SUIElement *pElement(NULL);
	int nIndex(-1);
	float fWidth(0.0f);

	for( int i=0; i<(int)m_vecValueIndex.size(); i++ )
	{
		nIndex = m_vecValueIndex[i];
		pElement = GetElement(nIndex);
		if( pElement == NULL ) continue;
		
		fWidth += pElement->fTextureWidth * m_fFontSize * m_fScale;		
	}

	return fWidth;
}

float CDnCountCtl::GetControlHeight()
{
	SUIElement *pElement(NULL);
	int nIndex(-1);
	float fHeight(0.0f);

	for( int i=0; i<(int)m_vecValueIndex.size(); i++ )
	{
		nIndex = m_vecValueIndex[i];
		pElement = GetElement(nIndex);
		if( pElement == NULL ) continue;	

		if( fHeight < (pElement->fTextureHeight * m_fFontSize) )
		{
			fHeight = pElement->fTextureHeight * m_fFontSize *m_fScale;
		}		
	}

	return fHeight;
}


BYTE CDnCountCtl::GetAlpha()
{	
	int nAlpha = (m_bUseCustomColor ? m_cAlpha : 255 ) * (m_Color.dwCurrentColor>>24) / 255;
	return nAlpha;
}
