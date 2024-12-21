#include "StdAfx.h"
#include "DnNestInfoListItem.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnNestInfoListItem::CDnNestInfoListItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pNestName(NULL)
, m_pNestClearInfo(NULL)
, m_pNestEventIcon(NULL)
, m_pTooltipControl(NULL)
{
	memset(&m_NestInfo,0,sizeof(m_NestInfo));
}

CDnNestInfoListItem::~CDnNestInfoListItem(void)
{

}

void CDnNestInfoListItem::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyNestInfoListDlg.ui" ).c_str(), bShow );

}

void CDnNestInfoListItem::InitialUpdate()
{
	m_pNestName = GetControl<CEtUIStatic>( "ID_TEXT_NESTNAME" );
	m_pNestClearInfo = GetControl<CEtUIStatic>( "ID_TEXT_CLEAR" );
	m_pNestEventIcon = GetControl<CEtUIStatic>( "ID_STATIC_EVENT" );

	m_pTooltipControl = GetControl<CEtUIStatic>( "ID_STATIC_TOOLTIP" );
}

void CDnNestInfoListItem::SetInfo( TCustomEventInfo& pInfo, int nActivate, int uiStringID )
{
	m_NestInfo = pInfo;

	m_pNestName->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, uiStringID));

	int nStateClearUIStirngID = m_NestInfo.cFlag == 1 ? 1000005225 : 93;
	m_pNestClearInfo->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nStateClearUIStirngID) );
	
	//이벤트 대상인지 확인...
	bool isValidEvent = nActivate == 1;
	m_pNestEventIcon->Show(isValidEvent);

	//#48529
	//"이벤트 대상입니다" 툴팁 표시는 클리어가 아니어야 한다..
	if (isValidEvent && m_NestInfo.cFlag != 1)
		m_pTooltipControl->SetTooltipText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3381));
	else
		m_pTooltipControl->SetTooltipText(L"");

}

void CDnNestInfoListItem::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnNestInfoListItem::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}

void CDnNestInfoListItem::SetElementDialogShowState( bool bShow )
{
	CEtUIDialog::SetElementDialogShowState( bShow );
}



