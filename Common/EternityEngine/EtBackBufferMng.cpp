#include "StdAfx.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtBackBufferMng g_EtBackBufferMng;

CEtBackBufferMng::CEtBackBufferMng(void)
{
}

CEtBackBufferMng::~CEtBackBufferMng(void)
{
	Clear();
}

void CEtBackBufferMng::Clear()
{
	SAFE_RELEASE_SPTR( m_hBackBuffer );

	for( int i = 0; i < (int)m_UserBackBuffers.size(); i++ ) {
		SAFE_RELEASE_SPTR( m_UserBackBuffers[i] );
	}
	m_UserBackBuffers.clear();
}

void CEtBackBufferMng::DumpBackBuffer()
{
	if( !m_hBackBuffer )
	{
		CreateBackBuffer();
	}
	GetEtDevice()->DumpBackBuffer( ( EtSurface * )m_hBackBuffer->GetSurfaceLevel() );
}

void CEtBackBufferMng::DumpUserBackBuffer( int nIndex )
{
	GetEtDevice()->DumpBackBuffer( ( EtSurface * )m_UserBackBuffers[ nIndex ]->GetSurfaceLevel() );
}

void CEtBackBufferMng::CreateBackBuffer()
{
	m_hBackBuffer = CEtTexture::CreateRenderTargetTexture( -1, -1 );
	m_hBackBuffer->SetFileName( "EtBackBuffer" );
}

int CEtBackBufferMng::CreateUserBackBuffer()
{
	EtTextureHandle hBackBuffer = CEtTexture::CreateRenderTargetTexture( -1, -1 );
	hBackBuffer->SetFileName( "EtUserBackBuffer" );
	int nSlotIndex = -1;
	for( int i = 0; i < (int)m_UserBackBuffers.size(); i++ ) {
		if( !m_UserBackBuffers[i] ) {
			nSlotIndex = i;
			break;
		}
	}
	if( nSlotIndex == -1 ) {
		m_UserBackBuffers.push_back( hBackBuffer );
		if( m_UserBackBuffers.size() > 10 ) {
			OutputDebug("[warning] too many user back buffer created.\n");
		}
		nSlotIndex = (int)m_UserBackBuffers.size()-1;
	}
	else {
		m_UserBackBuffers[ nSlotIndex ] = hBackBuffer;
	}

	return nSlotIndex;
}

void CEtBackBufferMng::ReleaseUserBackBuffer( int nIndex )
{
	SAFE_RELEASE_SPTR( m_UserBackBuffers[nIndex] );
}

int CEtBackBufferMng::GetBackBufferIndex()
{
	if( !m_hBackBuffer )
	{
		CreateBackBuffer();
	}
	return m_hBackBuffer->GetMyIndex(); 
}

EtTextureHandle CEtBackBufferMng::GetBackBuffer()
{
	if( !m_hBackBuffer )
	{
		CreateBackBuffer();
	}
	return m_hBackBuffer; 
}

EtTextureHandle CEtBackBufferMng::SetBackBuffer( EtTextureHandle hNewBackBuffer )
{
	EtTextureHandle hOldTexture = GetBackBuffer();
	m_hBackBuffer = hNewBackBuffer;
	return hOldTexture;
}

EtTextureHandle CEtBackBufferMng::GetUserBackBuffer( int nIndex )
{
	return m_UserBackBuffers[ nIndex ]; 
}
