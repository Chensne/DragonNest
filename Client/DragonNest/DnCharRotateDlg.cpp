#include "StdAfx.h"
#include "DnCharRotateDlg.h"
#include "DnLoginTask.h"
#include "DnBridgeTask.h"
#include "TaskManager.h"
#include "DnMessageBox.h"
#include "DnInterface.h"
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef PRE_ADD_GAMEQUIT_REWARD
#include "DnCharSelectDlg.h"
#include "DnCharSelectListDlg.h"
#endif // PRE_ADD_GAMEQUIT_REWARD


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_MOD_SELECT_CHAR

CDnCharRotateDlg::CDnCharRotateDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pButtonRotateLeft( NULL )
, m_pButtonRotateRight( NULL )
, m_pButtonAvatarViewArea( NULL )
#if defined(PRE_ADD_DWC)
, m_pStaticDWCCharTitle(NULL)
#endif
, m_fAvatarViewMouseX( 0.0f )
, m_fAvatarViewMouseY( 0.0f )
{
}

CDnCharRotateDlg::~CDnCharRotateDlg()
{
}

void CDnCharRotateDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_CharPreviewDlg.ui" ).c_str(), bShow );
}

void CDnCharRotateDlg::InitialUpdate()
{
	m_pButtonRotateLeft = GetControl<CEtUIButton>( "ID_BT_LEFT" );
	m_pButtonRotateRight = GetControl<CEtUIButton>( "ID_BT_RIGHT" );
	m_pButtonAvatarViewArea = GetControl<CEtUIButton>( "ID_BT_PREVIEW" );

	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_TEXT_RETURNTITLE");
#ifdef PRE_ADD_NEWCOMEBACK
	m_pStaticComeback = pStatic;
	m_pStaticComeback->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7899 ) ); // "★ 귀환자 캐릭터 ★"
	m_pStaticComeback->Show( false );

	m_crrTextColor = m_TextColor = EtColor( m_pStaticComeback->GetTextColor() );		
	m_sign = 1.0f;
#else
	if( pStatic )
		pStatic->Show( false );
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	m_pStaticDWCCharTitle = GetControl<CEtUIStatic>("ID_TEXT_DWCTITLE");	
	if(m_pStaticDWCCharTitle)
		m_pStaticDWCCharTitle->Show(false);

	m_DWCcrrTextColor = m_DWCTextColor = EtColor( m_pStaticDWCCharTitle->GetTextColor() );		
	m_DWCSign = 1.0f;
#endif // PRE_ADD_DWC
	
}

void CDnCharRotateDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
#ifdef PRE_ADD_NEWCOMEBACK
		m_sign = 1.0f;
		m_crrTextColor = m_TextColor;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
		m_DWCSign = 1.0f;
		m_DWCcrrTextColor = m_DWCTextColor;
#endif // PRE_ADD_NEWCOMEBACK
	}
	else
	{
#ifdef PRE_ADD_NEWCOMEBACK
		m_pStaticComeback->SetTextColor( m_TextColor ); // 원래색상으로 복원.
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
		m_pStaticDWCCharTitle->SetTextColor( m_DWCTextColor ); // 원래색상으로 복원.
#endif // PRE_ADD_NEWCOMEBACK
	}
}

void CDnCharRotateDlg::Process( float fDelta )
{
	CEtUIDialog::Process( fDelta );

	if( !IsShow() ) return;

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	static float fRotScale = 200.f;

	if( m_pButtonRotateLeft->IsPressed() )
		pLoginTask->CharacterAddRotateYaw( -fDelta * fRotScale );
	else if( m_pButtonRotateRight->IsPressed() )
		pLoginTask->CharacterAddRotateYaw( fDelta * fRotScale );
	else if( m_pButtonAvatarViewArea->IsPressed() )
	{
		float fX = m_pButtonAvatarViewArea->GetMouseCoord().fX;
		float fY = m_pButtonAvatarViewArea->GetMouseCoord().fY;
		if( m_fAvatarViewMouseX != 0.f && m_fAvatarViewMouseY != 0.f )
		{
			static float fMoveScale = 500.f;
			float fAddAngle = fMoveScale * sqrtf( (m_fAvatarViewMouseX-fX)*(m_fAvatarViewMouseX-fX)+(m_fAvatarViewMouseY-fY)*(m_fAvatarViewMouseY-fY) ) * (((m_fAvatarViewMouseX-fX)>0.f)? 1.f : -1.f);
			pLoginTask->CharacterAddRotateYaw( -fAddAngle );
		}
		m_fAvatarViewMouseX = fX;
		m_fAvatarViewMouseY = fY;
	}
	else
	{
		m_fAvatarViewMouseX = 0.f;
		m_fAvatarViewMouseY = 0.f;
		focus::SetFocus();
	}

#ifdef PRE_ADD_NEWCOMEBACK	
	
	if( m_pStaticComeback->IsShow() )
	{
		if( m_crrTextColor.a >= 1.0f )
			m_sign = -1.0f;
		else if( m_crrTextColor.a <= 0.0f )
			m_sign = 1.0f;

		m_crrTextColor.a += m_sign * fDelta * 0.5f;

		if( m_crrTextColor.a > 1.0f )
			m_crrTextColor.a = 1.0f;
		else if( m_crrTextColor.a < 0.0f )
			m_crrTextColor.a = 0.0f;

		m_pStaticComeback->SetTextColor( m_crrTextColor );
	}
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC	

	if( m_pStaticDWCCharTitle->IsShow() )
	{
		if( m_DWCcrrTextColor.a >= 1.0f )
			m_DWCSign = -1.0f;
		else if( m_DWCcrrTextColor.a <= 0.0f )
			m_DWCSign = 1.0f;

		m_DWCcrrTextColor.a += m_DWCSign * fDelta * 0.5f;

		if( m_DWCcrrTextColor.a > 1.0f )
			m_DWCcrrTextColor.a = 1.0f;
		else if( m_DWCcrrTextColor.a < 0.0f )
			m_DWCcrrTextColor.a = 0.0f;

		m_pStaticDWCCharTitle->SetTextColor( m_DWCcrrTextColor );
	}
#endif // PRE_ADD_NEWCOMEBACK

}

bool CDnCharRotateDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pTask ) 
		return false;
	
	if( pTask->GetState() == CDnLoginTask::CharSelect && uMsg == WM_LBUTTONDBLCLK )
	{
		if( !IsMouseInDlg() ) return false;
		POINT MousePoint;
		SUICoord uiCoordsBase[2];

		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( m_pButtonAvatarViewArea->IsInside( fMouseX, fMouseY ) ) 
		{
			if( pTask->GetSelectActorSlot() == -1 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_11, MB_OK, MESSAGEBOX_11, pTask );
				return false;
			}

			if( pTask->CheckSelectCharDeleteWait() )
				return false;

#ifdef PRE_ADD_COMEBACK
			CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
	
	#ifdef PRE_ADD_DWC
			if( pTask->GetComeback() && GetDWCTask().IsDWCChar() == false)
	#else
			if( pTask->GetComeback() )
	#endif	// PRE_ADD_DWC
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7740 ), MB_OKCANCEL, 1, this ); // "정말 이 캐릭터로 지급 받으시겠습니까? (선택하시면 수정이 불가능합니다. 신중하게 선택해주세요.)"						
			}
			//else
			//	GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
	#ifdef PRE_ADD_GAMEQUIT_REWARD
			else
			{
				bool bNormalUser = true;
				CTask * _pTask = CTaskManager::GetInstance().GetTask( "BridgeTask" );
				if( pTask )
				{
					CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)_pTask;

					// 재접속보상수령가능.
					CDnCharSelectDlg * pDlg = GetInterface().GetCharSelectDlg();
					if( pDlg && pBridgeTask->GetAccountRegTime() && pBridgeTask->GetNewbieReward() )				
					{
						CDnCharSelectListDlg * pDlgList = pDlg->GetCharSelectListDlg();					
						if( pDlgList )
						{				
							bNormalUser = false;
							GetInterface().ShowGameQuitRewardComboDlg( true );
							GetInterface().SelectedGameQuitRewardCharIndex( pDlgList->GetSelectCharIndex() );
						}					
					}	
				}

		#ifdef PRE_ADD_DWC
				// DWC시즌이 아니라면
				if(GetDWCTask().IsDWCRankSession() == false && GetDWCTask().IsDWCChar())
				{
					// mid: 콜로대회 기간이 아닙니다. 해당 캐릭은 사용할수없습니다.
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120270));
					bNormalUser = false;
				}
		#endif // PRE_ADD_DWC

				// [신규계정 재접속 보상] 도 [귀환자] 도 [DWC캐릭] 도 아닌 일반계정.
				if( bNormalUser )
				{ 
#if defined(PRE_ADD_23829)
					SecondPassCheck(m_pCheckBox ? m_pCheckBox->IsChecked() : false);
#endif // PRE_ADD_23829

#ifdef PRE_MDD_USA_SECURITYNUMPAD
				// 미국의 경우 2차 비밀번호를 강제적으로 입력한다. 
				Show(false);
#else
				CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
				int nValue[4] = { 0, };
				pLoginTask->SetAuthPassword(0, nValue);
				pLoginTask->ChangeState(CDnLoginTask::LoginStateEnum::ChannelList);
				//SelectChannel();

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
				pLoginTask->ResetPreviewCashCostume();
#endif

#ifdef PRE_ADD_SETCHECK
				SetCheckShow(m_pCheckBox->IsChecked());
#endif 

#endif // PRE_MDD_USA_SECURITYNUMPAD
				Show(false);
			}
	#else	// PRE_ADD_GAMEQUIT_REWARD			
			else
				GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
	#endif	// PRE_ADD_GAMEQUIT_REWARD

#else	// PRE_ADD_COMEBACK
			GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
#endif	// PRE_ADD_COMEBACK
			}
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCharRotateDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{	
#ifdef PRE_ADD_COMEBACK
	case 1:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) {
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
				}
			}
		}
		break;
#endif
	default:
		break;
	}
}

#ifdef PRE_ADD_NEWCOMEBACK
void CDnCharRotateDlg::SetComebackUser( bool bComeback )
{
	if( bComeback )
	{
		m_crrTextColor.a = 0.0f;
		m_pStaticComeback->SetTextColor( m_crrTextColor );
	}
	m_pStaticComeback->Show( bComeback );
}
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
void CDnCharRotateDlg::SetDWCCharSelect( bool bSelect )
{
	if(bSelect)
	{
		m_DWCcrrTextColor.a = 0.0f;
		m_pStaticDWCCharTitle->SetTextColor( m_DWCcrrTextColor );
	}
	m_pStaticDWCCharTitle->Show(bSelect);
}
#endif //PRE_ADD_DWC


#endif // PRE_MOD_SELECT_CHAR