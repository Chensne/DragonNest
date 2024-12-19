#include "StdAfx.h"
#include "EtUIDialog.h"
#include "EtUIProgressBar.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIProgressBar::CEtUIProgressBar( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_fProgress = 50.0f;
	m_fProgressSmooth = 50.0f;
	m_bSmooth = false;
	m_dwTick = 0;
	m_bImmediateChangeNeeded = false;
	m_nProgressIndex = 0;
	m_nProgressCount = 1;
	m_bUseLazy = false;
	m_bUseLayer = false;
	m_bUseProgressIndex = false;
	m_nLightShadeElementIndex = 0;
	m_nDarkShadeElementIndex = 0;
	m_nGaugeEndElementIndex = 0;
	m_bReverse = false;
}

CEtUIProgressBar::~CEtUIProgressBar(void)
{
}

void CEtUIProgressBar::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );
	UpdateRects();
}

void CEtUIProgressBar::UpdateRects()
{
	float fHoriBorder, fVertBorder;

	fHoriBorder = m_Property.UICoord.fWidth * m_Property.ProgressBarProperty.fHoriBorder;
	fVertBorder = m_Property.UICoord.fHeight * m_Property.ProgressBarProperty.fVertBorder;
	m_ProgressCoord.SetCoord( m_Property.UICoord.fX + fHoriBorder, 
							  m_Property.UICoord.fY + fVertBorder,
							  m_Property.UICoord.fWidth - fHoriBorder * 2, 
							  m_Property.UICoord.fHeight - fVertBorder * 2 );
}

void CEtUIProgressBar::Render( float fElapsedTime )
{
	if( !IsShow() ) {
		return;
	}
	SUIElement *pElement(NULL);
	SUICoord ProgressCoord, UVCoord;

	EtTextureHandle hTexture = m_pParent->GetUITexture();
	if( m_bExistTemplateTexture )
		hTexture = m_Template.m_hTemplateTexture;

	pElement = GetElement(0);
	UVCoord = pElement->UVCoord;
	if( m_bExistTemplateTexture )
		UVCoord = pElement->TemplateUVCoord;
	m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, m_Property.UICoord, 0.f, m_fZValue );

	if( m_bSmooth ) {
		if( !m_bUseLazy || GetTickCount() - m_dwTick > 1000 ) {
			m_fProgress += (m_fProgressSmooth - m_fProgress ) * min(1.0f, ( fElapsedTime * 7.f ));
		}
	}
	if( m_fProgress <= 0.0f )
		return;

	int nTotalLayerCount = (int)m_nVecLayerElementList.size();
	int nCurrentLayer = (int)( ( m_fProgress * 0.01f ) * nTotalLayerCount );

	if( nCurrentLayer < 0 ) nCurrentLayer = 0;
	else if( nCurrentLayer >= (int)m_nVecLayerElementList.size() ) nCurrentLayer = (int)m_nVecLayerElementList.size() - 1;

	int nProgressIndex = ( m_bUseLayer ) ? m_nVecLayerElementList[nCurrentLayer] : 0;
	int nPrevProgressIndex = ( m_bUseLayer ) ? ( ( nCurrentLayer > 0 ) ? m_nVecLayerElementList[nCurrentLayer - 1] : -1 ) : 0;

	float fProgress = m_fProgress;
	float fProgressSmooth = m_fProgressSmooth;
	if( m_bUseLayer ) {
		float fTemp = 100.0f / nTotalLayerCount;
		int nTemp2 = (int)(m_fProgress / fTemp);
		fProgress = ( m_fProgress < 100.f ) ? ( m_fProgress - (float)( nTemp2 * fTemp ) ) * (float)nTotalLayerCount : 100.f;
		nTemp2 = (int)(m_fProgressSmooth / fTemp);
		fProgressSmooth = ( m_fProgressSmooth < 100.f ) ? ( m_fProgressSmooth - nTemp2 * fTemp ) * (float)nTotalLayerCount : 100.f;
	}

	if( m_bUseLayer && nPrevProgressIndex >= 0 ) {
		pElement = GetElement( 1 + nPrevProgressIndex );
		UVCoord = pElement->UVCoord;
		if( m_bExistTemplateTexture )
			UVCoord = pElement->TemplateUVCoord;
		ProgressCoord = m_ProgressCoord;
		m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
	}
	
	if(m_bUseProgressIndex)
	{
		pElement = GetElement( 1 + m_nProgressIndex);

		if(pElement)
		{
			UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			UVCoord.fWidth *= (fProgress * 0.01f);
			ProgressCoord = m_ProgressCoord;
			ProgressCoord.fWidth *= (fProgress * 0.01f);
			m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
		}
	}
	else
	{	
		//--- ProgressBar 반전 추가 이전소스.
		/*pElement = GetElement( 1 + nProgressIndex );
		UVCoord = pElement->UVCoord;
		UVCoord.fWidth *= (fProgress * 0.01f);
		ProgressCoord = m_ProgressCoord;
		ProgressCoord.fWidth *= (fProgress * 0.01f);
		m_pParent->DrawSprite( UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );*/
		//  ---//

		
		pElement = GetElement( 1 + nProgressIndex );
		UVCoord = pElement->UVCoord;
		if( m_bExistTemplateTexture )
			UVCoord = pElement->TemplateUVCoord;
		UVCoord.fWidth *= (fProgress * 0.01f);
		ProgressCoord = m_ProgressCoord;
		ProgressCoord.fWidth *= (fProgress * 0.01f);
		
		// ProgressBar 반전.
		if( m_bReverse )
		{			
			if( m_bExistTemplateTexture )
				UVCoord.fX = UVCoord.fX + pElement->TemplateUVCoord.fWidth - UVCoord.fWidth;			
			else
				UVCoord.fX = UVCoord.fX + pElement->UVCoord.fWidth - UVCoord.fWidth;			
			ProgressCoord.fX = ProgressCoord.fX + m_ProgressCoord.fWidth - ProgressCoord.fWidth;			
		}
		m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
	}

	if( m_nLightShadeElementIndex > 0 ) {
		pElement = GetElement( m_nLightShadeElementIndex );
		if( pElement ) {
			UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			ProgressCoord = m_ProgressCoord;
			ProgressCoord.fWidth *= (fProgress * 0.01f);
			m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
		}
	}
	if( m_nDarkShadeElementIndex > 0 ) {
		pElement = GetElement( m_nDarkShadeElementIndex );
		if( pElement ) {
			UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			ProgressCoord = m_ProgressCoord;
			ProgressCoord.fX += (ProgressCoord.fWidth * (fProgress * 0.01f));
			ProgressCoord.fWidth *= ((100.0f-fProgress) * 0.01f);
			m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
		}
	}

	if( m_nGaugeEndElementIndex > 0 ) {
		pElement = GetElement( m_nGaugeEndElementIndex );
		if( pElement && fProgress < 100.0f ) {
			SUICoord StarCoord;
			ProgressCoord = m_ProgressCoord;
			ProgressCoord.fWidth *= (fProgress * 0.01f);
			StarCoord.SetPosition( ProgressCoord.Right()-(pElement->fTextureWidth/2.0f), ProgressCoord.fY-((pElement->fTextureHeight-ProgressCoord.fHeight)/2.0f) );
			StarCoord.SetSize( pElement->fTextureWidth, pElement->fTextureHeight );
			UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, StarCoord );
		}
	}

	if( m_bUseLazy ) {
		pElement = GetElement( 2 + nProgressIndex );
		if( pElement ) {
			UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			ProgressCoord = m_ProgressCoord;
			if( m_bSmooth ) {
				UVCoord.fWidth *= (fProgressSmooth * 0.01f);	
				ProgressCoord.fWidth *= (fProgressSmooth * 0.01f);
			}
			else {
				UVCoord.fWidth *= (fProgress * 0.01f);	
				ProgressCoord.fWidth *= (fProgress * 0.01f);
			}
			m_pParent->DrawSprite( hTexture, UVCoord, 0xffffffff, ProgressCoord, 0.f, m_fZValue );
		}
	}
}

void CEtUIProgressBar::SetProgress( float fProgress )
{ 
	float fClampProgress = max( min( 100.f, fProgress), 0.f);
	if( m_bSmooth && !m_bImmediateChangeNeeded ) {
		if( m_fProgressSmooth != fClampProgress )
		{
			m_dwTick = GetTickCount();
		}
		m_fProgressSmooth = fClampProgress;		
	}
	else {
		m_fProgressSmooth = m_fProgress = fClampProgress;
		m_bImmediateChangeNeeded = false;
	}
}

void CEtUIProgressBar::UseShadeProcessBar( int nLightElementIndex, int nDarkElementIndex )
{
	m_nLightShadeElementIndex = nLightElementIndex;
	m_nDarkShadeElementIndex = nDarkElementIndex;
}

void CEtUIProgressBar::UseGaugeEndElement( int nGaugeEndElementIndex )
{
	m_nGaugeEndElementIndex = nGaugeEndElementIndex;
}

void CEtUIProgressBar::UseLayer(bool bLayer, int nLayerElementCount, ... )
{
	m_bUseLayer = bLayer;
	m_nVecLayerElementList.clear();

	if( bLayer ) {
		va_list list;
		va_start( list, nLayerElementCount );

		for( int i=0; i<nLayerElementCount; i++ ) {
			int nValue = va_arg( list, int );
			m_nVecLayerElementList.push_back( nValue );
		}
		va_end( list );
	}
}