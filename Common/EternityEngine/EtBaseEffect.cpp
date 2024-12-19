#include "StdAfx.h"
#include "EtBaseEffect.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CEtBaseEffect, 400 )

CEtBaseEffect::CEtBaseEffect(void)
{
	m_BlendOP = BLENDOP_ADD;	
	m_SourceBlend = BLEND_SRCALPHA;
	m_DestBlend = BLEND_INVSRCALPHA;
}

CEtBaseEffect::~CEtBaseEffect(void)
{
	m_MeshStream.Clear();
	SAFE_RELEASE_SPTR( m_hMaterial );
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CEtBaseEffect::RenderEffectList( float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		GetItem( i )->Render( fElapsedTime );
	}
}
