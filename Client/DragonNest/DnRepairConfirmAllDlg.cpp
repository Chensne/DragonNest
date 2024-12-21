#include "StdAfx.h"
#include "DnRepairConfirmAllDlg.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnRepStoreBenefitTooltipDlg.h"
#include "DnActor.h"
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnRepairConfirmAllDlg::CDnRepairConfirmAllDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pGold(NULL)
, m_pSilver(NULL)
, m_pBronze(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
, m_pReputationBenefit( NULL )
, m_pStoreBenefitTooltip( NULL )
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
{
}

CDnRepairConfirmAllDlg::~CDnRepairConfirmAllDlg(void)
{
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	SAFE_DELETE( m_pStoreBenefitTooltip );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
}

void CDnRepairConfirmAllDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RepairConfirmAllDlg.ui" ).c_str(), bShow );
}

void CDnRepairConfirmAllDlg::InitialUpdate()
{
	m_pGold = GetControl<CEtUIStatic>("ID_GOLD"); 
	m_pSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pBronze = GetControl<CEtUIStatic>("ID_BRONZE");

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	m_pReputationBenefit = GetControl<CEtUIStatic>( "ID_ICON_REPUTE" );
	m_pReputationBenefit->Show( false );

	m_pStoreBenefitTooltip = new CDnRepStoreBenefitTooltipDlg( UI_TYPE_CHILD, this );
	m_pStoreBenefitTooltip->Initialize( false );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );
}

void CDnRepairConfirmAllDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();

		m_pGold->SetIntToText( 0 );
		m_pSilver->SetIntToText( 0 );
		m_pBronze->SetIntToText( 0 );
	}
}

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
void CDnRepairConfirmAllDlg::ShowReputationBenefit( bool bShow )
{
	m_pReputationBenefit->Show( bShow );
}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

void CDnRepairConfirmAllDlg::SetPrice( int nPrice )
{
	int nGold = nPrice/10000;
	int nSilver = (nPrice%10000)/100;
	int nBronze = nPrice%100;

	m_pGold->SetIntToText( nGold );
	m_pSilver->SetIntToText( nSilver );
	m_pBronze->SetIntToText( nBronze );
}

bool CDnRepairConfirmAllDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	
	switch( uMsg )
	{
		case WM_MOUSEMOVE:
			{
				CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
				if( !pQuestTask ) 
					return false;

				CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
				if( !pReputationRepos )
					return false;

				float fMouseX = 0;
				float fMouseY = 0;
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				bool bMouseEnter = false;
				float fReputationValue = 0.0f;
				float fReputationValueMax = 100.0f;

				ShowChildDialog( m_pStoreBenefitTooltip, false );
				if( m_pReputationBenefit->IsShow() )
				{
					if( m_pReputationBenefit->IsInside( fMouseX, fMouseY ) )
						bMouseEnter = true;

					if( !CDnMouseCursor::GetInstance().IsShowCursor() )
						bMouseEnter = false;				

					if( bMouseEnter )
					{
						if( CDnActor::s_hLocalActor )
						{
							int iBenefitValue = pQuestTask->GetStoreBenefitValue( NpcReputation::StoreBenefit::RepairFeeDiscount );

							wchar_t awcBuffer[ 256 ] = { 0 };
							//swprintf_s( awcBuffer, L"호감도 효과 발동중\n수리비용 %d%% 할인 (UIString 에 없는 임시 텍스트)", iBenefitValue );
							swprintf_s( awcBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3216 ), iBenefitValue );
							m_pStoreBenefitTooltip->SetText( awcBuffer );
							ShowChildDialog( m_pStoreBenefitTooltip, true );

							SUICoord dlgCoord;
							m_pStoreBenefitTooltip->GetDlgCoord( dlgCoord );
							dlgCoord.fX = fMouseX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
							dlgCoord.fY = fMouseY + GetYCoord() - dlgCoord.fHeight - 0.004f;
							m_pStoreBenefitTooltip->SetDlgCoord( dlgCoord );
						}
					}
				}
			}
			break;
	}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	return bRet;
}