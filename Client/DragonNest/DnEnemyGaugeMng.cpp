#include "StdAfx.h"
#include "DnEnemyGaugeMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnEnemyGaugeMng::CDnEnemyGaugeMng(void)
{
	Clear();
}

CDnEnemyGaugeMng::~CDnEnemyGaugeMng(void)
{
	Clear();
}

void CDnEnemyGaugeMng::SetEnenmy( DnActorHandle hActor )
{
	std::list<CDnEnemyGaugeDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		if( (*iter)->GetEnemy() == hActor )
		{
			(*iter)->ResetDelayTime();
			return;
		}
	}
	
	CDnEnemyGaugeDlg *pDlg = m_GaugeDlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listGaugeDlg.push_back( pDlg );
	pDlg->SetEnemy( hActor );
	pDlg->Show( true );
}

void CDnEnemyGaugeMng::DeleteEnemy( DnActorHandle hActor )
{
	std::list<CDnEnemyGaugeDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		if( (*iter)->GetEnemy() == hActor )
		{
			(*iter)->Show( false );
			m_GaugeDlgMemPool.Deallocate( (*iter) );
			m_listGaugeDlg.erase(iter);
			return;
		}
	}
}

void CDnEnemyGaugeMng::DeletePlayer( DnActorHandle hActor )
{
	for(UINT i = 0 ; i < m_vecPlayerGaugeDlg.size(); i++ )
	{
		if(m_vecPlayerGaugeDlg[i]->GetEnemy() == hActor )
		{
			SAFE_DELETE( m_vecPlayerGaugeDlg[i] );
			m_vecPlayerGaugeDlg.erase( m_vecPlayerGaugeDlg.begin()+i);
			break;
		}		
	}	
}

void CDnEnemyGaugeMng::SetPlayer( DnActorHandle hActor )
{
	if( !hActor )
		return;

	for(UINT i = 0 ; i < m_vecPlayerGaugeDlg.size(); i++ )
	{
		if(m_vecPlayerGaugeDlg[i]->GetEnemy() == hActor )
		{
			return;
		}
		
	}

	CDnPlayerGaugeDlg *pDlg = new CDnPlayerGaugeDlg();//m_GaugePLayerDlgMemPool.Allocate();
	pDlg->Initialize( false );
	pDlg->SetActor( hActor );
	pDlg->Show( true );
	m_vecPlayerGaugeDlg.push_back( pDlg );
	
}

void CDnEnemyGaugeMng::Process( float fElapsedTime )
{
	std::list<CDnEnemyGaugeDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); )
	{
		if( (*iter)->GetDelayTime() <= 0.0f || ( (*iter)->GetEnemy() && !(*iter)->GetEnemy()->IsShowExposureInfo() ) )
		{
			(*iter)->Show( false );
			m_GaugeDlgMemPool.Deallocate( (*iter) );
			iter = m_listGaugeDlg.erase(iter);
			continue;
		}

		++iter;
	}

	for(UINT i = 0 ; i < m_vecPlayerGaugeDlg.size(); i++ )
	{
		if(m_vecPlayerGaugeDlg[i]->GetDelayTime() <= 0.0f || !(m_vecPlayerGaugeDlg[i]->GetEnemy()))
		{
			SAFE_DELETE( m_vecPlayerGaugeDlg[i] );
			m_vecPlayerGaugeDlg.erase(m_vecPlayerGaugeDlg.begin()+i);
			--i;
		}
	}
}

void CDnEnemyGaugeMng::Clear()
{
	std::list<CDnEnemyGaugeDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		(*iter)->Show( false );
		m_GaugeDlgMemPool.Deallocate( (*iter) );
	}

	m_listGaugeDlg.clear();

	for(UINT i = 0 ; i < m_vecPlayerGaugeDlg.size(); i++ )
	{
		SAFE_DELETE( m_vecPlayerGaugeDlg[i] );
	}
	m_vecPlayerGaugeDlg.clear();
}

#ifdef PRE_ADD_SHOW_MONACTION
bool CDnEnemyGaugeMng::IsEnemyGaugeInList(DnActorHandle hActor) const
{
	std::list<CDnEnemyGaugeDlg*>::const_iterator iter = m_listGaugeDlg.begin();
	for (; iter != m_listGaugeDlg.end(); ++iter)
	{
		const CDnEnemyGaugeDlg* pDlg = (*iter);
		if (pDlg && pDlg->GetEnemy() == hActor)
			return pDlg->IsShow();
	}

	return false;
}
#endif
