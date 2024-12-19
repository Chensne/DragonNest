#include "StdAfx.h"
#include "DnDurabilityGaugeMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDurabilityGaugeMng::CDnDurabilityGaugeMng(void)
{
	Clear();
}

CDnDurabilityGaugeMng::~CDnDurabilityGaugeMng(void)
{
	Clear();
}

void CDnDurabilityGaugeMng::ShowDurability(DnActorHandle hActor, int nStateBlowID, float fDurability, bool bShow)
{
	CDnDurabilityDlg *pDlg = NULL;
	std::list<CDnDurabilityDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		if( (*iter)->IsSameInfo(hActor, nStateBlowID))
		{
			pDlg = (*iter);
			//(*iter)->SetDurability(fDurability);
			//(*iter)->Show(bShow);
			break;;
		}
	}
	
	if (NULL == pDlg)
	{
		pDlg = m_GaugeDlgMemPool.Allocate();
		pDlg->Initialize( false );
		m_listGaugeDlg.push_back( pDlg );
		pDlg->SetActorInfo(hActor, nStateBlowID);
	}
	
	pDlg->SetDurability(fDurability);
	pDlg->Show(bShow);
}

void CDnDurabilityGaugeMng::DeleteActor(DnActorHandle hActor)
{
	std::list<CDnDurabilityDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		if( (*iter)->GetActor() == hActor )
		{
			(*iter)->Show(false);
			m_GaugeDlgMemPool.Deallocate( (*iter) );
			m_listGaugeDlg.erase(iter);
			return;
		}
	}
}

void CDnDurabilityGaugeMng::Process( float fElapsedTime )
{
	std::list<CDnDurabilityDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); )
	{
		if( false == (*iter)->IsShow() ) 
		{
			m_GaugeDlgMemPool.Deallocate( (*iter) );
			iter = m_listGaugeDlg.erase(iter);
			continue;
		}

		++iter;
	}
}

void CDnDurabilityGaugeMng::Clear()
{
	std::list<CDnDurabilityDlg*>::iterator iter = m_listGaugeDlg.begin();
	for( ; iter != m_listGaugeDlg.end(); ++iter )
	{
		(*iter)->Show( false );
		m_GaugeDlgMemPool.Deallocate( (*iter) );
	}

	m_listGaugeDlg.clear();

}