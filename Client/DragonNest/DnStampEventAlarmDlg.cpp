#include "Stdafx.h"

#ifdef PRE_ADD_STAMPSYSTEM

#include "GameOption.h"

#include "DnInterface.h"
#include "DnStampEventDlg.h"

#include "DnStampEventAlarmDlg.h"


CDnStampEventAlarmDlg::CDnStampEventAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, false )
,m_pStaticText(NULL)
{
}

void CDnStampEventAlarmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_DayCheckSumInfoDlg.ui" ).c_str(), bShow );
	this->SetAllignType(AT_HORI_CENTER, AT_VERT_TOP);//rlkt!
}


void CDnStampEventAlarmDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_MSG");
}

void CDnStampEventAlarmDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) 
		return;

	if( bShow )
	{
		// 알림이 그만보기.
		CDnStampEventDlg * pStampEventDlg = GetInterface().GetStampEventDlg();
		if( pStampEventDlg && CGameOption::GetInstance().m_nShowStampAlram != -1 && CGameOption::GetInstance().m_nShowStampAlram == pStampEventDlg->GetToday() )
			bShow = false;
	}
	else
	{

	}

	CEtUIDialog::Show( bShow );
}



void CDnStampEventAlarmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_SUMINFO" ) )
		{
			Show( false );
			GetInterface().ShowStampDlg();
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


// 알람상태설정.
void CDnStampEventAlarmDlg::SetAlarmState( bool bOne, bool bForce )
{
	// 만렙이 아닌경우 보이지 않는다.
	bool bMaxLevel = false;
	if( !CDnActor::s_hLocalActor )
		return;
	int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	if( CDnActor::s_hLocalActor->GetLevel() < nLimitLevel )
		return;
	

	bool bComplete = false;
	CDnStampEventDlg * pStampEventDlg = GetInterface().GetStampEventDlg();
	if( pStampEventDlg == NULL )
		return;

	// 알림이 그만보기.
	if( CGameOption::GetInstance().m_nShowStampAlram != -1 && CGameOption::GetInstance().m_nShowStampAlram == pStampEventDlg->GetToday() )
		return;

	if( pStampEventDlg )
	{
		if( pStampEventDlg->IsExistStamp() == false )
		{
			this->Show( false );
			return;
		}

		int strID = 8326;
		int res = pStampEventDlg->IsComplete();
		switch( res )
		{
		// 완료한게 없다.
		case 0 :
			strID = 8326; // "도전과제 진행중입니다"
			break;

		// 한개이상 완료.
		case 1 :
			strID = 8321; // "도전과제를 완료하였습니다"
			break;

		// 모두완료.
		case 2 :
			strID = 8322; // "도전과제를 모두 완료하였습니다"
			bComplete = true;
			break;
		}

		if( !bOne && res == 1 )
			strID = 8326; // "도전과제 진행중입니다"	

		m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strID ) );
	}

	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
	{
		if( bForce && bComplete )
			this->Show( false );
		else
			this->Show( true );
	}
	else
		this->Show( false );

}

#endif // PRE_ADD_STAMPSYSTEM