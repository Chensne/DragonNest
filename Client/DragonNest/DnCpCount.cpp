#include "StdAfx.h"
#include "DnCpCount.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpCount::CDnCpCount(CEtUIDialog *pParent)
	: CDnCountCtl( pParent )
	, m_MinusSignIdx(11)
{
	m_fFontSize = 1.0f;
	m_StaticIndex = -1;
	m_bCustomColor = false;
	m_CustomColor = 0xffffffff;
}

CDnCpCount::~CDnCpCount(void)
{
}

void CDnCpCount::Render( float fElapsedTime )
{
	SUIElement *pElement(NULL);
	SUICoord uiCoord;
	float fWidth(0.0f);
	int nIndex(-1);

	UI_CONTROL_STATE CurrentState = UI_STATE_NORMAL;
	if( !IsShow() ) CurrentState = UI_STATE_HIDDEN;
	m_Color.Blend( CurrentState, fElapsedTime, 0.8f );

	DWORD dwColor = m_Color.dwCurrentColor;
	if (m_bCustomColor)
		dwColor = m_CustomColor;

	EtColor Color(dwColor);
	Color.a = Color.a * (m_cAlpha/255.f);
	dwColor = Color;

	for( int i=(int)m_vecValueIndex.size()-1; i>=0 ; i-- )
	{
		nIndex = m_vecValueIndex[i];
		pElement = GetElement(nIndex);
		if( pElement == NULL ) continue;

		uiCoord = m_Property.UICoord;
		uiCoord.fX += fWidth;
		
		uiCoord.SetSize( pElement->fTextureWidth * m_fFontSize, pElement->fTextureHeight * m_fFontSize );
		fWidth += pElement->fTextureWidth * m_fFontSize;

		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, uiCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, dwColor, uiCoord );
	}

	pElement = GetElement(m_StaticIndex);
	if( pElement == NULL ) return;

	uiCoord = m_Property.UICoord;
	uiCoord.fX += fWidth;

	uiCoord.SetSize( pElement->fTextureWidth * m_fFontSize, pElement->fTextureHeight * m_fFontSize );

	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, uiCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, dwColor, uiCoord );
}

float CDnCpCount::GetControlWidth()
{
	float fWidth = CDnCountCtl::GetControlWidth();

	SUIElement *pElement = GetElement(m_StaticIndex);
	if (pElement)
		fWidth += pElement->fTextureWidth;

	return fWidth;
}

float CDnCpCount::GetControlHeight()
{
	float fHeight = CDnCountCtl::GetControlHeight();

	SUIElement *pElement = GetElement(m_StaticIndex);
	if( pElement && fHeight < pElement->fTextureHeight )
		fHeight = pElement->fTextureHeight;

	return fHeight;
}

void CDnCpCount::SetValue( int nValue )
{
	bool bMinus = (nValue < 0);
	int valueSet = bMinus ? -nValue : nValue;

	CDnCountCtl::SetValue( valueSet );

	if (bMinus)
		m_vecValueIndex.push_back( m_MinusSignIdx );
}
