#include "StdAfx.h"
#include "DnEventDungeonEnterLevelDlg.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnWorld.h"


CDnEventDungeonEnterLevelDlg::CDnEventDungeonEnterLevelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnDungeonEnterLevelDlg( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnEventDungeonEnterLevelDlg::~CDnEventDungeonEnterLevelDlg()
{
}

void CDnEventDungeonEnterLevelDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonEventEnterLevelDlg.ui" ).c_str(), bShow );
}

void CDnEventDungeonEnterLevelDlg::InitialUpdate()
{
	//CDnDungeonEnterLevelDlg::InitialUpdate();
	m_pStaticTimer = GetControl<CEtUIStatic>("ID_STATIC_TIMER");
	m_pButtonEnter = GetControl<CEtUIButton>("ID_BUTTON_ENTER");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pTextureDungeonArea = GetControl<CEtUITextureControl>("ID_TEXTUREL_AREA");

	m_pSelectName = GetControl<CEtUIStatic>("ID_SELECT_NAME");
	m_pSelectLevel = GetControl<CEtUIStatic>("ID_SELECT_LEVEL");
	m_pRecommLevel = GetControl<CEtUIStatic>("ID_RECOMM_LEVEL");
	m_pRecommCount = GetControl<CEtUIStatic>("ID_RECOMM_COUNT");

	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pNeedItemStatic = GetControl<CEtUIStatic>("ID_ITEMSLOT");
	m_pNeedItemStatic->Show( false );
	m_pNeedItemCount = GetControl<CEtUIStatic>("ID_STATIC_COUNT");

	m_pMaxUsableCoin = GetControl<CEtUIStatic>("ID_REBIRTH_LIMIT");
}

void CDnEventDungeonEnterLevelDlg::SetDungeonDesc( const wchar_t *wszStr )
{
	m_pSelectLevel->SetText( wszStr );
}

void CDnEventDungeonEnterLevelDlg::UpdateDungeonInfo( CDnDungeonEnterDlg::SDungeonGateInfo *pInfo )
{
	CDnDungeonEnterLevelDlg::UpdateDungeonInfo( pInfo );

	WCHAR wszStr[256];
	swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2412 ), pInfo->m_nMinLevel, pInfo->m_nMaxLevel );
	m_pRecommLevel->SetText( wszStr );

	swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2366 ), pInfo->m_nMaxPartyCount );
	m_pRecommCount->SetText( wszStr );
}

void CDnEventDungeonEnterLevelDlg::SetSelectDungeonInfo( int nMapIndex, int nDifficult )
{
	CDnDungeonEnterLevelDlg::SetSelectDungeonInfo( nMapIndex, nDifficult );

	CDnGameTask *pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstance().GetTask( "GameTask" ));
	if( !pGameTask ) return;
	int nReadyGateIndex = pGameTask->GetReadyGateIndex();
	if( !CDnWorld::IsActive() ) return;
	CDnWorld::DungeonGateStruct *pGateStruct = (CDnWorld::DungeonGateStruct *)CDnWorld::GetInstance().GetGateStruct( nReadyGateIndex );
	if( !pGateStruct ) return;

	for( DWORD i=0; i<pGateStruct->pVecMapList.size(); i++ ) {
		CDnWorld::DungeonGateStruct *pChildGate = (CDnWorld::DungeonGateStruct *)pGateStruct->pVecMapList[i];
		if( pChildGate->nMapIndex == nMapIndex ) {
			WCHAR wszStr[256];
			WCHAR *wszDesc = L"";

			m_pSelectLevel->SetText( (WCHAR *)pChildGate->szDesc.c_str() );

			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2412 ), pChildGate->nMinLevel, pChildGate->nMaxLevel );
			m_pRecommLevel->SetText( wszStr );

			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2366 ), pChildGate->nMaxPartyCount );
			m_pRecommCount->SetText( wszStr );

			break;
		}
	}

}