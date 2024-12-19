#include "StdAfx.h"
#include "DnTextBoxDlgMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTextBoxDlgMng::CDnTextBoxDlgMng(void)
{
}

CDnTextBoxDlgMng::~CDnTextBoxDlgMng(void)
{
}

DWORD CDnTextBoxDlgMng::ShowTextWindow( LPCWSTR wszStr, float fX, float fY, float fWidth, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall )
{
	CDnTextBoxDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetText( wszStr, fX, fY, fWidth, fDelayTime );
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

DWORD CDnTextBoxDlgMng::ShowTextWindow( LPCWSTR wszStr, float fX, float fY, int nPos, int nFormat, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall )
{
	CDnTextBoxDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetText( wszStr, fX, fY, nPos, nFormat, fDelayTime );
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

DWORD CDnTextBoxDlgMng::ShowCountDownWindow( int nCountDown, float fX, float fY, int nPos, int nFormat, float fDelayTime, DWORD dwSetID, CEtUICallback *pCall )
{
	CDnTextBoxDlg *pDlg = m_DlgMemPool.Allocate();
	pDlg->Initialize( false );
	m_listDlg.push_back( pDlg );
	pDlg->SetCountDown( nCountDown, fX, fY, nPos, nFormat, fDelayTime );
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