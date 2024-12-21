#include "StdAfx.h"
#include "DnRebirthCaptionDlg.h"
#include "ItemSendPacket.h"
#include "DnInterface.h"
#include "InputWrapper.h"
#include "DnItemTask.h"
//#include "DnInventory.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnWorld.h"
#include "DnLocalPlayerActor.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRebirthCaptionDlg::CDnRebirthCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_Mode = Normal;
}

CDnRebirthCaptionDlg::~CDnRebirthCaptionDlg(void)
{
}

void CDnRebirthCaptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RebirthCaptionDlg.ui" ).c_str(), bShow );
}

void CDnRebirthCaptionDlg::InitialUpdate()
{
	// 재로딩이 있을때마다 키설정값을 다시 불러온다.
	SetRebirthCaptionHotKey( _ToVK(g_WrappingKeyData[IW_REBIRTH]) );
}

void CDnRebirthCaptionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_ADD_INSTANT_CASH_BUY
		if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_DEATH, this );
			return;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
#ifdef PRE_FIX_TRY_REBIRTH_BEFORE_SYNC
		CDnGameTask *pGameTask = static_cast<CDnGameTask *>(CTaskManager::GetInstance().GetTask("GameTask"));
		if (pGameTask && pGameTask->IsSyncComplete())
		{
#ifdef PRE_ADD_INSTANT_CASH_BUY
			bool bPartyRestore = false;
			if( CDnActor::s_hLocalActor )
			{
				CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if( pPlayer && pPlayer->GetCurrentGhostType() == Ghost::Type::PartyRestore )
					bPartyRestore = true;
			}

			if( bPartyRestore || CDnItemTask::GetInstance().IsCanRebirth() )
			{
				Show( false );
				SendRebirthCoin();
				GetInterface().CloseRebirthFailDlg();
			}
#else // PRE_ADD_INSTANT_CASH_BUY
			Show( false );
			SendRebirthCoin();
			GetInterface().CloseRebirthFailDlg();
#endif // PRE_ADD_INSTANT_CASH_BUY
		}
#else
		Show( false );
		SendRebirthCoin();
		GetInterface().CloseRebirthFailDlg();
#endif
	}
}

void CDnRebirthCaptionDlg::SetRebirthCaptionHotKey( BYTE cVK )
{
	// 핫키를 임의로 바꿀 수 있는 기능이 없어서, 우선은 이런 식으로 처리한다.
	GetControl("ID_BTN_REBIRTH")->SetHotKey( cVK );

	WCHAR wszKey[8] = {0,};
	swprintf_s( wszKey, _countof(wszKey), L"%s", g_szKeyString[_ToDIK(cVK)] );

	WCHAR wszText[128] = {0,};

	//bool isSeaDragonMode = false;
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 908 ), wszKey );

	if( m_Mode == DragonNest )
	{
		CDnItem * pItem = CDnItemTask::GetInstance().FindRebirthItem( 0 );

		if( NULL == pItem )
			return;

		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000100 ), wszKey, pItem->GetName(), CDnItemTask::GetInstance().GetSpecialRebirthItemCount() );
	}

#ifdef PRE_ADD_INSTANT_CASH_BUY
	else if( !CDnItemTask::GetInstance().IsCanRebirth() && CDnItemTask::GetInstance().IsCanRebirthIfHaveCoin() )
	{
		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 917 ), wszKey );
	}
#endif // PRE_ADD_INSTANT_CASH_BUY
	if (CDnActor::s_hLocalActor)
	{
		CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		if (pPlayer && pPlayer->GetCurrentGhostType() == Ghost::Type::PartyRestore)
			swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3448 ), wszKey );
	}	
	GetControl("ID_BUTTON_REBIRTH")->SetText( wszText );
}

void CDnRebirthCaptionDlg::Show(bool bShow)
{
	if (bShow)
	{
#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_DEATH );
		if( nShowType == INSTANT_BUY_HIDE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
		}
		else if( nShowType == INSTANT_BUY_SHOW )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( true );
		}
		else if( nShowType == INSTANT_BUY_DISABLE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( false );
		}

		if( CDnItemTask::GetInstance().GetMaxUsableRebirthCoin() == 0 ||
			!CDnItemTask::GetInstance().IsCanRebirthIfHaveCoin() )
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );

		if (CDnActor::s_hLocalActor)
		{
			CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
			if( pPlayer && pPlayer->GetCurrentGhostType() == Ghost::Type::PartyRestore )
				GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY

		// 씨드래곤 맵상에서는 부활 아이템이 있어야 함 [2010/12/21 semozz]
		if (CDnWorld::GetInstance().GetDragonNestType() > eDragonNestType::None)
		{
			int nGameMode = CDnItemTask::GetInstance().GetAllowGameMode();
			if (CDnActor::s_hLocalActor)
			{
				CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if (pPlayer && pPlayer->GetCurrentGhostType() == Ghost::Type::PartyRestore )
				{
					SetMode( DragonNest );
					SetRebirthCaptionHotKey( _ToVK(g_WrappingKeyData[IW_REBIRTH]) );

					__super::Show(bShow);
					return;
				}
			}

			if( CDnItemTask::GetInstance().GetUsableRebirthCoin() > 0 || CommonUtil::IsInfiniteRebirthOnDungeon() )
			{
				if( 0 < CDnItemTask::GetInstance().GetRebirthCoinCount() )
					SetMode( Normal );
				else
					SetMode( DragonNest );
			}
			else if( NULL != CDnItemTask::GetInstance().FindRebirthItem(nGameMode) )
			{
				SetMode( DragonNest );
			}
			else
			{
				__super::Show(false);
				return;
			}
		}
		else
			SetMode(Normal);

		SetRebirthCaptionHotKey( _ToVK(g_WrappingKeyData[IW_REBIRTH]) );
	}

	__super::Show(bShow);
}

#ifdef PRE_ADD_INSTANT_CASH_BUY
void CDnRebirthCaptionDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( pControl == NULL && nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
		WCHAR wszText[128] = {0,};
		BYTE cVK = GetControl("ID_BTN_REBIRTH")->GetHotKey();

		WCHAR wszKey[8] = {0,};
		swprintf_s( wszKey, _countof(wszKey), L"%s", g_szKeyString[_ToDIK(cVK)] );

		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 908 ), wszKey );
		GetControl("ID_BUTTON_REBIRTH")->SetText( wszText );

		return;
	}
}
#endif // PRE_ADD_INSTANT_CASH_BUY