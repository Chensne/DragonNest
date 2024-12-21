#include "StdAfx.h"
#include "EtDevice.h"
#include "EtOcclusionQuery.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtOcclusionQuery::CEtOcclusionQuery(void)
{
	m_pQuery = NULL;
	m_bHasPrev = false;
	m_bCreateFail = false;
	m_bAsyncType = false;
}

CEtOcclusionQuery::~CEtOcclusionQuery(void)
{
	SAFE_RELEASE( m_pQuery );
}

void CEtOcclusionQuery::Begin()
{
	if( !m_bCreateFail && m_pQuery == NULL )
	{
		( ( LPDIRECT3DDEVICE9 )GetEtDevice()->GetDevicePtr() )->CreateQuery ( D3DQUERYTYPE_OCCLUSION, &m_pQuery );
		ADD_D3D_RES( m_pQuery );
		if( m_pQuery == NULL ) {
			m_bCreateFail = true;
		}
	}
	if( !m_bCreateFail ) 
	{
		m_pQuery->Issue (D3DISSUE_BEGIN);
	}
	m_bHasPrev = true;
}

void CEtOcclusionQuery::End()
{
	if( !m_bCreateFail ) 
	{
		m_pQuery->Issue( D3DISSUE_END );
	}
}

bool CEtOcclusionQuery::GetResult( DWORD &dwResult )
{
	dwResult = 0;
	if( m_bHasPrev == false )
	{
		return false;
	}
	if( m_bCreateFail ) 
	{
		return false;
	}

	bool bRet = true;

	if( m_bAsyncType )
	{
		if( m_pQuery->GetData( (void * )&dwResult, sizeof( DWORD ), 0 ) == S_FALSE )
		{
			bRet = false;
		}
	}
	else
	{
		int nCount = 0;
		while( m_pQuery->GetData( (void * )&dwResult, sizeof( DWORD ), D3DGETDATA_FLUSH ) == S_FALSE )
		{
			nCount++;
			if( nCount > 30 )
			{
				bRet = false;
				break;
			}
		}
	}
	m_bHasPrev = false;

	return bRet;
}

void CEtOcclusionQuery::OnLostDevice()
{
	SAFE_RELEASE( m_pQuery );
	m_bHasPrev = false;
}

void CEtOcclusionQuery::OnResetDevice()
{
}
