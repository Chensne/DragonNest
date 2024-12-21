#include "StdAfx.h"
#include "DnCharSelectDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnMessageBox.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "LoginSendPacket.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"

#ifdef PRE_MOD_SELECT_CHAR
#include "DnCharSelectListDlg.h"
#include "DnCharLevelLimitInfoDlg.h"
#else // PRE_MOD_SELECT_CHAR
#include "DnCharSelectTitleDlg.h"
#include "DnCharSelectArrowLeftDlg.h"
#include "DnCharSelectArrowRightDlg.h"
#endif // PRE_MOD_SELECT_CHAR
#include "DnAuthTask.h"

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


#ifdef PRE_MOD_SELECT_CHAR

CDnCharSelectDlg::CDnCharSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_fCharSelectDescDelta( 10.0f )
, m_pStaticCharSelectDesc( NULL )
, m_pCharSelectCloseDlg( NULL )
, m_pCharSelectListDlg( NULL )
, m_pCharLevelLimitInfoDlg( NULL )
{
}

CDnCharSelectDlg::~CDnCharSelectDlg(void)
{
	SAFE_DELETE( m_pCharSelectCloseDlg );
	SAFE_DELETE( m_pCharSelectListDlg );
	SAFE_DELETE( m_pCharLevelLimitInfoDlg );
}

void CDnCharSelectDlg::Initialize( bool bShow )
{

#ifdef _ADD_NEWLOGINUI
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charselectstartdlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_StartDlg.ui" ).c_str(), bShow );
#endif
}

void CDnCharSelectDlg::InitialUpdate()
{
	m_pCharSelectListDlg = new CDnCharSelectListDlg( UI_TYPE_CHILD, this );
	m_pCharSelectListDlg->Initialize( true );
	m_pCharSelectCloseDlg = new CDnCharSelectCloseDlg( UI_TYPE_CHILD, this );
	m_pCharSelectCloseDlg->Initialize( true );
	m_pCharLevelLimitInfoDlg = new CDnCharLevelLimitInfoDlg( UI_TYPE_CHILD, this );
	m_pCharLevelLimitInfoDlg->Initialize( false );
	m_pStaticCharSelectDesc = GetControl<CEtUIStatic>( "ID_TEXT_NOTICE" );
}

void CDnCharSelectDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( pTask && pTask->IsCompleteCharList() )
		{
			if( pTask->IsEmptySelectActorSlot() )
			{
				pTask->ChangeState( CDnLoginTask::CharCreate_SelectClass );
				Show( false );
			}
		}

		m_fCharSelectDescDelta -= fElapsedTime;
		if( m_fCharSelectDescDelta <= 0.f ) m_fCharSelectDescDelta = 0.f;
		float fAlpha = ( m_fCharSelectDescDelta > 1.f ) ? 1.f : m_fCharSelectDescDelta;
		DWORD dwColor = D3DCOLOR_ARGB( (BYTE)(fAlpha * 255), 255, 255, 255 );
		DWORD dwShadowColor = D3DCOLOR_ARGB( (BYTE)(fAlpha * 255), 0, 0, 0 );
		m_pStaticCharSelectDesc->SetTextureColor( dwColor );
		m_pStaticCharSelectDesc->SetTextColor( dwColor );
		m_pStaticCharSelectDesc->SetShadowColor( dwShadowColor );
		if (m_fCharSelectDescDelta == 0.f)
		{
			if (m_pStaticCharSelectDesc->IsShow())
				m_pStaticCharSelectDesc->Show(false);
		}
	}
}

#if defined(PRE_ADD_DWC)
bool CDnCharSelectDlg::CheckUserAccountID(eAccountLevel AccountLv)
{
	CDnLoginTask* pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
	if(pLoginTask == NULL)
		return false;

	std::vector<TCharListData> vCharList = pLoginTask->GetMyCharListData();
	if(vCharList.empty())
		return false;

	CDnCharSelectDlg* pDlg = GetInterface().GetCharSelectDlg();
	if(pDlg == NULL)
		return false;

	CDnCharSelectListDlg* pCharSelectDlg = pDlg->GetCharSelectListDlg();
	if(pCharSelectDlg == NULL)
		return false;

	int nIndex = pCharSelectDlg->GetSelectCharIndex();
	if( nIndex >= 0 && nIndex < (int)vCharList.size())
	{
		if(vCharList[nIndex].cAccountLevel == AccountLv)
		{							
			return true;
		}
	}

	return false;
}
#endif

void CDnCharSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( !pTask ) return;

		if (IsCmdControl("ID_BT_START"))
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
	}
}

void CDnCharSelectDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
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

void CDnCharSelectDlg::Show( bool bShow )
{
	if( bShow == m_bShow ) return;

	if( bShow )
	{
		m_fCharSelectDescDelta = 10.f;

#ifdef PRE_ADD_COMEBACK
		CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
		if( pTask && pTask->GetComeback() )
		{			
			if( pTask->GetComebackMessage() )
			{
				pTask->EndComebackMessage();
#ifdef PRE_ADD_NEWCOMEBACK
				GetInterface().OpenComebackMsgDlg( true );
#else
				GetInterface().MiddleMessageBox( 7738, MB_OK ); // "왜 이제 오셨나요! 오랜만입니다. 가장 처음 접속한 캐릭터에게 귀환자 보상이 지급됩니다."
#endif // PRE_ADD_NEWCOMEBACK
			}
		}
#endif // PRE_ADD_COMEBACK
	}

	GetInterface().ShowCharOldGestureDlg(bShow, false);
	GetInterface().ShowCharRotateDlg( bShow );

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
	GetInterface().ShowPreviewCostumeDlg( bShow );
#endif

	CEtUIDialog::Show( bShow );
}

#if defined(PRE_ADD_23829)
void CDnCharSelectDlg::Show2ndPassCreateButton(bool bShow)
{
	if( m_pCharSelectListDlg )
		m_pCharSelectListDlg->Show2ndPassCreateButton( bShow );
}
#endif // PRE_ADD_23829

void CDnCharSelectDlg::SetCharacterList( std::vector<TCharListData>& vecCharListData, int nMaxCharCount )
{

#ifdef PRE_ADD_GAMEQUIT_REWARD
	CTask *pTask = CTaskManager::GetInstance().GetTask( "BridgeTask" );
	if( pTask )
	{
		CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)pTask;

		if( pBridgeTask->GetNewbieLoginSequence() )
		{
			// 재접속보상수령가능.
			if( pBridgeTask->GetAccountRegTime() && pBridgeTask->GetNewbieReward() )
			{

			}

			// 재접속보상 수령불가.
			else if( pBridgeTask->GetAccountRegTime() && pBridgeTask->GetNewbieReward() == false )
			{
				if( !vecCharListData.empty() )
				{
					TCharListData & charData = vecCharListData[0];

					int nClass = CommonUtil::GetClassByJob( (int)charData.cJob );
					if( nClass > 0 )
						GetInterface().ShowGameQuitNextTimeRewardDlg( true, GameQuitReward::RewardType::eType::NewbieReward, true, (int)charData.cLevel, nClass );
				}
			}
		}
	}
	
#endif // PRE_ADD_GAMEQUIT_REWARD

	if( m_pCharSelectListDlg )
		m_pCharSelectListDlg->SetCharacterList( vecCharListData, nMaxCharCount );

	int nCountLevelLimitCharacter = 0;
	int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	std::vector<TCharListData>::iterator iter = vecCharListData.begin();
	for( ; iter != vecCharListData.end(); iter++ )
	{
		if( (*iter).cLevel >= nLevelLimit && !(*iter).bDeleteFlag )
		{
			nCountLevelLimitCharacter++;
		}
	}

	if( m_pCharLevelLimitInfoDlg )
	{
		if( nCountLevelLimitCharacter > 0 )
		{
			m_pCharLevelLimitInfoDlg->SetLevelLimitCount( nCountLevelLimitCharacter );
			m_pCharLevelLimitInfoDlg->Show( true );
		}
		else
		{
			m_pCharLevelLimitInfoDlg->Show( false );
		}
	}
}

void CDnCharSelectDlg::SelectCharIndex( int nSelectIndex )
{
	if( m_pCharSelectListDlg )
		m_pCharSelectListDlg->SelectCharIndex( nSelectIndex );
}


///////////////////////////////////////////////////////////////////////////
// Cloase Dialog

CDnCharSelectCloseDlg::CDnCharSelectCloseDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharSelectCloseDlg::~CDnCharSelectCloseDlg(void)
{
}

void CDnCharSelectCloseDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "charselectclosedlg.ui" ).c_str(), bShow ); //rlkt_test CharCreate_CloseDlg.ui
}

void CDnCharSelectCloseDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BACK" ) )
		{
			SendBackButton();
			pLoginTask->ChangeState( CDnLoginTask::ServerList );
			return;
		}
		else if( IsCmdControl( "ID_EXIT" ) )
		{
// #69613 - 메세지변경 처리 제거.
//#ifdef PRE_ADD_NEWCOMEBACK
//			CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
//			if( pTask->GetComeback() )
//				GetInterface().MessageBox( 4948, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() ); // "게임에 접속을 하지 않고 종료하면, 재접속 시에는 귀환자 보상을 받을 수 없습니다. 종료하시겠습니까?"
//			else
//				GetInterface().MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() );
//#else
			GetInterface().MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() );
//#endif // PRE_ADD_NEWCOMEBACK
			return;
		}
	}
}


#else // PRE_MOD_SELECT_CHAR


CDnCharSelectDlg::CDnCharSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pCharSelectTitleDlg( NULL )
, m_pCharSelectArrowLeftDlg( NULL )
, m_pCharSelectArrowRightDlg( NULL )
, m_fCharSelectDescDelta( 10.0f )
, m_pStaticCharSelectDesc( NULL )
{
}

CDnCharSelectDlg::~CDnCharSelectDlg(void)
{
	SAFE_DELETE( m_pCharSelectTitleDlg );
	SAFE_DELETE( m_pCharSelectArrowLeftDlg );
	SAFE_DELETE( m_pCharSelectArrowRightDlg );
}

void CDnCharSelectDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharSelect.ui" ).c_str(), bShow );
}

void CDnCharSelectDlg::InitialUpdate()
{
	m_pCharSelectTitleDlg = new CDnCharSelectTitleDlg( UI_TYPE_CHILD, this );
	m_pCharSelectTitleDlg->Initialize( true );
	bIsDeleteChracter = false;
	m_pCharSelectArrowLeftDlg = new CDnCharSelectArrowLeftDlg( UI_TYPE_CHILD, this );
	m_pCharSelectArrowLeftDlg->Initialize( true );
	m_pCharSelectArrowLeftDlg->Show( false );
	m_pCharSelectArrowRightDlg = new CDnCharSelectArrowRightDlg( UI_TYPE_CHILD, this );
	m_pCharSelectArrowRightDlg->Initialize( true );
	m_pCharSelectArrowRightDlg->Show( false );
	m_pStaticCharSelectDesc = GetControl<CEtUIStatic>("ID_STATIC0");

	//#if defined(PRE_ADD_23829)
	// 기본적으로 이 버튼은 Hide시켜 놓는다. [2010/12/17 semozz]
	CEtUIButton *pButton = GetControl<CEtUIButton>("ID_2NDPASS_CREATE");
	if (pButton)
	{
		pButton->Show(false);
	}
	//#endif // PRE_ADD_23829
}

void CDnCharSelectDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( pTask && pTask->IsCompleteCharList() )
		{
			if( pTask->IsEmptySelectActorSlot() )
			{
				pTask->ChangeState( CDnLoginTask::CharCreate_SelectClass );
				Show( false );
			}
		}

		if( bIsDeleteChracter ) 
		{
			bIsDeleteChracter = false; 
			//2차 인증이 있으면 
			CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask( "AuthTask" );
			if( pAuthTask )
			{
				if( pAuthTask->GetSecondAuthPW() )
				{
					GetInterface().OpenSecurityCheckDlg( CDnInterface::InterfaceTypeEnum::Login , 1 );
					return; 
				}
				else 
				{
					CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
					if( pTask ) {
						int nValue[4] = { 0, };
						pTask->SetAuthPassword( 0, nValue );
						pTask->DeleteCharacter();
					}
				}
			}


		}
		m_fCharSelectDescDelta -= fElapsedTime;
		if( m_fCharSelectDescDelta <= 0.f ) m_fCharSelectDescDelta = 0.f;
		float fAlpha = ( m_fCharSelectDescDelta > 1.f ) ? 1.f : m_fCharSelectDescDelta;
		DWORD dwColor = D3DCOLOR_ARGB( (BYTE)(fAlpha * 255), 255, 255, 255 );
		DWORD dwShadowColor = D3DCOLOR_ARGB( (BYTE)(fAlpha * 255), 0, 0, 0 );
		m_pStaticCharSelectDesc->SetTextureColor( dwColor );
		m_pStaticCharSelectDesc->SetTextColor( dwColor );
		m_pStaticCharSelectDesc->SetShadowColor( dwShadowColor );
	}
}

void CDnCharSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( !pTask ) return;

		if( IsCmdControl("ID_CONNECT" ) || IsCmdControl("ID_BT_START") ) 
		{
			if( pTask->GetSelectActorSlot() == -1 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_11, MB_OK, MESSAGEBOX_11, pTask );
				return;
			}

#ifdef PRE_ADD_COMEBACK
			CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
			if( pTask && pTask->GetComeback() )
			{
#ifdef PRE_ADD_NEWCOMEBACK
				CDnCharSelectDlg * pDlg = GetInterface().GetCharSelectDlg();
				if( pDlg )
				{
					CDnCharSelectListDlg * pDlgList = pDlg->GetCharSelectListDlg();					
					if( pDlgList && pDlgList->ValidReward() )
					{
						GetInterface().SelectedCharIndex( pDlgList->GetSelectCharIndex() );
						GetInterface().OpenComebackRewardDlg( true );
					}
					else
					{
						GetInterface().MessageBox( 7741, MB_OKCANCEL, 7741, this ); // "해당 캐릭터는 귀환자 보상 지급 대상이 아니기 때문에, 보상을 지급 받을 수 없습니다."
					}					
				}
                       
#else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7740 ), MB_OKCANCEL, 1, this ); // "정말 이 캐릭터로 지급 받으시겠습니까? (선택하시면 수정이 불가능합니다. 신중하게 선택해주세요.)"						
#endif // PRE_ADD_NEWCOMEBACK
			}
			else
				GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
#else
			GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
#endif // PRE_ADD_COMEBACK



			return;
		}

		if( IsCmdControl("ID_CREATE_CHAR" ) ) 
		{
			pTask->ChangeState( CDnLoginTask::CharCreate_SelectClass );
			return;
		}

		if( IsCmdControl("ID_DELETE_CHAR" ) ) 
		{
			if( pTask->GetSelectActorSlot() == -1 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_11, MB_OK, MESSAGEBOX_11, pTask );
				return;
			}

			GetInterface().MessageBox( MESSAGEBOX_6, MB_YESNO, MESSAGEBOX_6, this, false, true );
			return;
		}

		if( IsCmdControl("ID_BACK" ) )
		{
			SendBackButton();
			pTask->ChangeState( CDnLoginTask::ServerList );
			return;
		}

		if( IsCmdControl("ID_EXIT" ) )
		{
			GetInterface().MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() );
			return;
		}
#if defined(PRE_ADD_23829)
		if( IsCmdControl("ID_2NDPASS_CREATE" ) ) 
		{
			GetInterface().OpenSecurityCreateDlg();
			return;
		}
#endif
	}
}


void CDnCharSelectDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
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

	case MESSAGEBOX_6:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_YES" ) )
			{
				bIsDeleteChracter = true; 
			}
		}
		break;
	}
}

void CDnCharSelectDlg::Show( bool bShow )
{
	if( bShow == m_bShow ) return;

	if( m_bShow ) {
		m_fCharSelectDescDelta = 10.f;
	}

	if( bShow )
	{
#ifdef PRE_ADD_COMEBACK
		CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
		if( pTask && pTask->GetComeback() )
		{
			if( pTask->GetComebackMessage() )
			{
				pTask->EndComebackMessage();
				GetInterface().MiddleMessageBox( 7738, MB_OK ); // "왜 이제 오셨나요! 오랜만입니다. 가장 처음 접속한 캐릭터에게 귀환자 보상이 지급됩니다."
			}
		}
#endif
	}

	CEtUIDialog::Show( bShow );
}

#if defined(PRE_ADD_23829)
void CDnCharSelectDlg::Show2ndPassCreateButton(bool bShow)
{
	CEtUIButton *pButton = GetControl<CEtUIButton>("ID_2NDPASS_CREATE");
	if (pButton)
	{
		pButton->Show(bShow);
	}
}
#endif

#endif // PRE_MOD_SELECT_CHAR