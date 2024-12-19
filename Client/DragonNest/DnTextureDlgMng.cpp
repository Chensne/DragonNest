#include "StdAfx.h"
#include "DnTextureDlgMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTextureDlgMng::CDnTextureDlgMng(void)
{
}

CDnTextureDlgMng::~CDnTextureDlgMng(void)
{
}

DWORD CDnTextureDlgMng::ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall )
{
	CDnTextureDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetTexture( hTextureHandle, fX, fY, fDelayTime );
	pDlg->SetCallback( pCall );
	pDlg->Show( true );

	SDlgSlotInfo sSlotInfo = m_dequeEmptySlot.front();
	m_dequeEmptySlot.pop_front();

	if( dwSetID > 0 )
	{
		sSlotInfo.dwSetID = dwSetID + USER_ID_OFFSET;
	}

	sSlotInfo.pDlg = pDlg;

	m_dequeUseSlot.push_front( sSlotInfo );
	
	return sSlotInfo.dwID;
}

DWORD CDnTextureDlgMng::ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall )
{
	CDnTextureDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetTexture( hTextureHandle, fX, fY, nPos, fDelayTime );
	pDlg->SetCallback( pCall );
	pDlg->Show( true );

	SDlgSlotInfo sSlotInfo = m_dequeEmptySlot.front();
	m_dequeEmptySlot.pop_front();

	if( dwSetID > 0 )
	{
		sSlotInfo.dwSetID = dwSetID + USER_ID_OFFSET;
	}

	sSlotInfo.pDlg = pDlg;

	m_dequeUseSlot.push_front( sSlotInfo );

	return sSlotInfo.dwID;
}

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
DWORD CDnTextureDlgMng::ShowTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID, bool bAutoCloseDialog, CEtUICallback *pCall)
{
	CDnTextureDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetTexture( hTextureHandle, fX, fY, nPos, bAutoCloseDialog, fDelayTime);
	pDlg->SetCallback( pCall );
	pDlg->Show( true );

	SDlgSlotInfo sSlotInfo = m_dequeEmptySlot.front();
	m_dequeEmptySlot.pop_front();

	if( dwSetID > 0 )
	{
		sSlotInfo.dwSetID = dwSetID + USER_ID_OFFSET;
	}

	sSlotInfo.pDlg = pDlg;

	m_dequeUseSlot.push_front( sSlotInfo );

	return sSlotInfo.dwID;
}
#endif