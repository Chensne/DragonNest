#include "StdAfx.h"
#include "DnMissionIconStatic.h"
#include "DnCustomControlCommon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnMissionIconStatic::CDnMissionIconStatic( CEtUIDialog *pParent )
: CEtUIStatic( pParent )
{
	m_nIconID = -1;
}

CDnMissionIconStatic::~CDnMissionIconStatic(void)
{
	SAFE_RELEASE_SPTR(m_hCurrentIcon);
	SAFE_RELEASE_SPTRVEC(m_hVecIconList);
}

void CDnMissionIconStatic::Initialize( SUIControlProperty *pProperty )
{
	EtTextureHandle hTexture;
	CFileNameString szFileName;
	bool bExistFile;
	char szStr[32];
	for( int i=0; i<99; i++ ) {
		sprintf_s( szStr, "MissionIcon%02d.dds", i + 1 );
		szFileName = CEtResourceMng::GetInstance().GetFullName( szStr, &bExistFile );
		if( !bExistFile ) break;
		hTexture = LoadResource( szFileName.c_str(), RT_TEXTURE );
		m_hVecIconList.push_back( hTexture );
	}

	CEtUIStatic::Initialize( pProperty );
}

void CDnMissionIconStatic::Process( float fElapsedTime )
{
	CEtUIStatic::Process( fElapsedTime );
}

void CDnMissionIconStatic::Render( float fElapsedTime )
{
	if( !IsShow() ) return;

	SUIElement *pElement(NULL);
	if( m_nIconID != -1 && m_hCurrentIcon ) {
		m_pParent->DrawSprite( m_hCurrentIcon, m_IconUV, ( m_bEnable == true ) ? 0xFFFFFFFF : 0xff808080, m_Property.UICoord );
	}

//	CEtUIStatic::Render( fElapsedTime );
}

void CDnMissionIconStatic::SetIconID( int nID ) 
{ 
	m_nIconID = nID;

	SAFE_RELEASE_SPTR( m_hCurrentIcon );
	if( m_nIconID != -1 ) {
		int nIndex = m_nIconID / MISSION_TEXTURE_ICON_COUNT;
		int nIconIndex = m_nIconID % MISSION_TEXTURE_ICON_COUNT;

		if( nIndex < 0 || nIndex >= (int)m_hVecIconList.size() ) return;

		m_hCurrentIcon = m_hVecIconList[nIndex];
		if( m_hCurrentIcon ) m_hCurrentIcon->AddRef();

		CalcButtonUV( nIconIndex, m_hCurrentIcon, m_IconUV, ITEMSLOT_ICON_XSIZE, ITEMSLOT_ICON_YSIZE );
	}
}