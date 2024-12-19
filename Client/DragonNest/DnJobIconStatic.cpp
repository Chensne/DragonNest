#include "StdAfx.h"
#include "DnJobIconStatic.h"
#include "DnCustomControlCommon.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnJobIconStatic::CDnJobIconStatic( CEtUIDialog *pParent )
: CEtUIStatic( pParent )
{
	m_bUseSmallSizeIcon = false;
	m_nJobIconID = -1;
}

CDnJobIconStatic::~CDnJobIconStatic(void)
{
	SAFE_RELEASE_SPTR( m_hNormalSizeIcon );
	SAFE_RELEASE_SPTR( m_hSmallSizeIcon );
}

void CDnJobIconStatic::Initialize( SUIControlProperty *pProperty )
{
	m_hNormalSizeIcon = LoadResource( CEtResourceMng::GetInstance().GetFullName( "JobIcon_Main.dds" ).c_str(), RT_TEXTURE );
	m_hSmallSizeIcon = LoadResource( CEtResourceMng::GetInstance().GetFullName( "JobIcon_Pvp.dds" ).c_str(), RT_TEXTURE );
	CEtUIStatic::Initialize( pProperty );
}

void CDnJobIconStatic::Render( float fElapsedTime )
{
	if( !IsShow() ) return;

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	if( m_nJobIconID != -1 && m_hNormalSizeIcon && m_hSmallSizeIcon )
	{
		D3DXCOLOR textureColor( pElement->TextureColor.dwCurrentColor );

		if( !m_bUseSmallSizeIcon )
			m_pParent->DrawSprite( m_hNormalSizeIcon, m_IconUV, textureColor, m_Property.UICoord );
		else
			m_pParent->DrawSprite( m_hSmallSizeIcon, m_IconUV, textureColor, m_Property.UICoord );
	}
	//CEtUIStatic::Render( fElapsedTime );
}

void CDnJobIconStatic::SetIconID( int nJobID, bool bUseSmallSizeIcon ) 
{
	m_nJobIconID = -1;
	DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TJOB);
	if( pSox && pSox->IsExistItem( nJobID ) )
		m_nJobIconID = pSox->GetFieldFromLablePtr( nJobID, "_JobIcon" )->GetInteger();
	m_bUseSmallSizeIcon = bUseSmallSizeIcon;

	if( m_nJobIconID != -1 )
	{
		if( !m_bUseSmallSizeIcon )
			CalcButtonUV( m_nJobIconID, m_hNormalSizeIcon, m_IconUV, JOB_NORMALSIZE_ICON_XSIZE, JOB_NORMALSIZE_ICON_YSIZE );
		else
			CalcButtonUV( m_nJobIconID, m_hSmallSizeIcon, m_IconUV, JOB_SMALLSIZE_ICON_XSIZE, JOB_SMALLSIZE_ICON_YSIZE );
	}
}