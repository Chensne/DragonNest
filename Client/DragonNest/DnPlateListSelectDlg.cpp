#include "StdAfx.h"
#include "DnPlateListSelectDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnPlateListDlg.h"
#include "DnPlateItemSlotDlg.h"
#include "DnPlateMixViewDlg.h"
#include "DnPlateMainDlg.h"
#include "DnMainMenuDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPlateListSelectDlg::CDnPlateListSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pPlateListDlg(NULL)
, m_pButtonNext(NULL)
, m_pButtonPrev(NULL)
, m_pStaticPage(NULL)
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
, m_dwColorBronze(0)
, m_dwColorGold(0)
, m_dwColorSilver(0)
, m_nCurSelect(0)
{
	memset( m_pNeedRune, NULL, sizeof(m_pNeedRune) );
	memset( m_pPlateInfo, 0, sizeof(m_pPlateInfo));
	memset( m_RuneInfo, 0, sizeof(m_RuneInfo));
	memset( m_pNeedRuneSlotButton, 0, sizeof(m_pNeedRuneSlotButton));
	memset( m_pPlateInfo, 0, sizeof(m_pPlateInfo));
}

CDnPlateListSelectDlg::~CDnPlateListSelectDlg(void)
{
	SAFE_DELETE( m_pPlateListDlg );

	for( int itr = 0; itr < SLOT_NUM; ++itr )
		SAFE_DELETE(m_pNeedRune[itr]);
}

void CDnPlateListSelectDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlateListSelectDlg.ui" ).c_str(), bShow );
}

void CDnPlateListSelectDlg::InitialUpdate()
{
	char szControlName[32];

	for( int i = 0; i < SLOT_NUM; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "%s%d", "ID_ITEM_SLOT", i);
		m_pNeedRuneSlotButton[i] = GetControl<CDnItemSlotButton>(szControlName);
		m_pNeedRuneSlotButton[i]->SetSlotType( ST_ITEM_COMPOUND_JEWEL );
	}

	m_pStaticGold = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pStaticSilver = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pStaticBronze = GetControl<CEtUIStatic>( "ID_BRONZE" );
	m_dwColorGold = m_pStaticGold->GetTextColor();
	m_dwColorSilver = m_pStaticSilver->GetTextColor();
	m_dwColorBronze = m_pStaticBronze->GetTextColor();

	m_pPlateListDlg = new CDnPlateListDlg( UI_TYPE_CHILD, this );
	m_pPlateListDlg->Initialize( false );
}

void CDnPlateListSelectDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	ShowChildDialog(m_pPlateListDlg, bShow);
	if( bShow )
	{
	}
	else
	{
	}

	CEtUIDialog::Show( bShow );
}

void CDnPlateListSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnPlateListSelectDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
		{
			
		}
		break;
	case WM_LBUTTONDBLCLK:
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

// 인덱스에 해당 하는 문장에 필요 룬 들을 셋팅한다.
void CDnPlateListSelectDlg::RefreshPlateList()
{
	for( int itr = 0; itr < SLOT_NUM; ++itr )
	{
		SAFE_DELETE(m_pNeedRune[itr]);
		m_pNeedRuneSlotButton[itr]->ResetSlot();
	}
	m_pStaticGold->SetText( L"" );
	m_pStaticSilver->SetText( L"" );
	m_pStaticBronze->SetText( L"" );
	m_pStaticGold->SetTextColor( m_dwColorGold );
	m_pStaticSilver->SetTextColor( m_dwColorSilver );
	m_pStaticBronze->SetTextColor( m_dwColorBronze );

	int nPlateItemID = ((CDnPlateMainDlg *)m_pParentDialog)->GetPlateItemID();
	const CDnItemCompounder::S_PLATE_INFO* pPlateInfo = GetItemTask().GetPlateInfoByItemID( nPlateItemID );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if( !pSox ) return;

	for( int nIndex = 0; nIndex < NUM_MAX_PLATE_COMPOUND_VARI; ++nIndex )
	{
		//최종 문장의 인덱스를 저장한다.
		m_pPlateInfo[nIndex] = pPlateInfo->aiCompoundTableID[nIndex];

		if( 0 != pPlateInfo->aiCompoundTableID[nIndex] )
		{
			for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
			{
				// 문장 정보를 찾아서 문장에 필요한 룬의 정보를 저장한다.
				if( pSox->GetFieldFromLablePtr(pSox->GetItemID(itr), "_SuccessItemID1")->GetInteger() == pPlateInfo->aiCompoundTableID[nIndex] )
				{
					int nNum = pSox->GetItemID(itr);
					m_RuneInfo[nIndex].m_nID[0] = pSox->GetFieldFromLablePtr( nNum, "_Slot1Id" )->GetInteger();
					m_RuneInfo[nIndex].m_nNum[0] = pSox->GetFieldFromLablePtr( nNum, "_Slot1Num" )->GetInteger();

					m_RuneInfo[nIndex].m_nID[1] = pSox->GetFieldFromLablePtr( nNum, "_Slot2Id" )->GetInteger();
					m_RuneInfo[nIndex].m_nNum[1] = pSox->GetFieldFromLablePtr( nNum, "_Slot2Num" )->GetInteger();

					m_RuneInfo[nIndex].m_nID[2] = pSox->GetFieldFromLablePtr( nNum, "_Slot3Id" )->GetInteger();
					m_RuneInfo[nIndex].m_nNum[2] = pSox->GetFieldFromLablePtr( nNum, "_Slot3Num" )->GetInteger();

					m_RuneInfo[nIndex].m_nID[3] = pSox->GetFieldFromLablePtr( nNum, "_Slot4Id" )->GetInteger();
					m_RuneInfo[nIndex].m_nNum[3] = pSox->GetFieldFromLablePtr( nNum, "_Slot4Num" )->GetInteger();

					m_RuneInfo[nIndex].m_nID[4] = pSox->GetFieldFromLablePtr( nNum, "_Slot5Id" )->GetInteger();
					m_RuneInfo[nIndex].m_nNum[4] = pSox->GetFieldFromLablePtr( nNum, "_Slot5Num" )->GetInteger();
					break;
				}
			}
		}
	}

	m_pPlateListDlg->RefreshPlateList();
}

int CDnPlateListSelectDlg::GetSelectedCompoundID()
{
	return m_pPlateListDlg->GetSelectedCompoundID();
}

void CDnPlateListSelectDlg::SelectPlate( int nIndex )
{
	m_nCurSelect = nIndex;

	TItemInfo itemInfo;
	m_vNeedItemInfo.clear();
	for( int itr = 0; itr < SLOT_NUM; ++itr )
	{
		SAFE_DELETE( m_pNeedRune[itr] );
		m_pNeedRuneSlotButton[itr]->ResetSlot();

		if( CDnItem::MakeItemInfo( m_RuneInfo[nIndex].m_nID[itr], 1, itemInfo ) == false ) continue;

		int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( m_RuneInfo[nIndex].m_nID[itr] );

		m_pNeedRune[itr] = GetItemTask().CreateItem( itemInfo );
		m_pNeedRuneSlotButton[itr]->SetItem(m_pNeedRune[itr], CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pNeedRuneSlotButton[itr]->SetJewelCount( m_RuneInfo[nIndex].m_nNum[itr], nCurItemCount );

		CDnItemTask::ItemCompoundInfo CompoundInfo;
		CompoundInfo.iItemID = m_RuneInfo[nIndex].m_nID[itr];
		CompoundInfo.iUseCount = m_RuneInfo[nIndex].m_nNum[itr];
		m_vNeedItemInfo.push_back(CompoundInfo);

		if( nCurItemCount < m_RuneInfo[nIndex].m_nNum[itr] )
			m_pNeedRuneSlotButton[itr]->SetRegist( true );
		else
			m_pNeedRuneSlotButton[itr]->SetRegist( false );
	}

	int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	int nPrice = (int)CompoundInfo.iCost;
	int nGold = nPrice/10000;
	int nSilver = (nPrice%10000)/100;
	int nBronze = nPrice%100;
	m_pStaticGold->SetIntToText( nGold );
	m_pStaticSilver->SetIntToText( nSilver );
	m_pStaticBronze->SetIntToText( nBronze );
	if( nPrice > GetItemTask().GetCoin() )
	{
		if( nPrice >= 10000 ) m_pStaticGold->SetTextColor( 0xFFFF0000, true );
		if( nPrice >= 100 ) m_pStaticSilver->SetTextColor( 0xFFFF0000, true );
		m_pStaticBronze->SetTextColor( 0xFFFF0000, true );
	}
	else
	{
		m_pStaticGold->SetTextColor( m_dwColorGold );
		m_pStaticSilver->SetTextColor( m_dwColorSilver );
		m_pStaticBronze->SetTextColor( m_dwColorBronze );
	}
}

bool CDnPlateListSelectDlg::EnoughRune()
{
	for( int itr = 0; itr < (int)m_vNeedItemInfo.size(); ++itr )
	{
		if( m_RuneInfo[m_nCurSelect].m_nNum[itr] > GetItemTask().GetCharInventory().GetItemCount( m_vNeedItemInfo[itr].iItemID ) )
			return false;
	}

	return true;
}

bool CDnPlateListSelectDlg::EnoughCoin()
{
	int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	return GetItemTask().GetCoin() >= CompoundInfo.iCost;
}

/*
CDnPlateListSelectDlg::CDnPlateListSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_emStep(stepBase)
, m_pItem(NULL)
, m_pItemSlotButton(NULL)
, m_pStaticFix(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_pStaticBack(NULL)
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
, m_pQuickSlotButton(NULL)
, m_pPlateListDlg(NULL)
, m_pPlateMixViewDlg(NULL)
{
}

CDnPlateListSelectDlg::~CDnPlateListSelectDlg(void)
{
	SAFE_DELETE( m_pPlateListDlg );
	SAFE_DELETE( m_pPlateMixViewDlg );
	SAFE_DELETE( m_pItem );
}

void CDnPlateListSelectDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlateListSelectDlg.ui" ).c_str(), bShow );
}

void CDnPlateListSelectDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM_PLATE");
	m_pStaticFix = GetControl<CEtUIStatic>("ID_STATIC_FIX");
	m_pStaticFix->Show( false );

	m_pButtonOK = GetControl<CEtUIButton>("ID_BUTTON_OK");

	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");

	m_pStaticBack = GetControl<CEtUIStatic>("ID_STATIC_BACK");

	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");

	m_pPlateListDlg = new CDnPlateListDlg( UI_TYPE_CHILD, this );
	m_pPlateListDlg->Initialize( false );

	for( int i = 0; i < 5; ++i )
	{
		CDnPlateItemSlotDlg *pDlg;
		pDlg = new CDnPlateItemSlotDlg( UI_TYPE_CHILD, this );
		pDlg->Init( i+1 );
		m_SlotDlgGroup.AddDialog( i, pDlg );
	}

	m_pPlateMixViewDlg = new CDnPlateMixViewDlg( UI_TYPE_CHILD, this );
	m_pPlateMixViewDlg->Initialize( false );
}

void CDnPlateListSelectDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{

	}
	else
	{
		ChangeStep( stepBase );
		GetInterface().CloseBlind();
	}

	CEtUIDialog::Show( bShow );
}

bool CDnPlateListSelectDlg::IsEnableRegisterPlateItem(CDnSlotButton* pDragButton, CDnItem* pItem) const
{
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3610 ), MB_OK );
		return false;
	}

	// 올린거 또 올리는거면 패스
	if( m_pQuickSlotButton && (pItem == m_pQuickSlotButton->GetItem()) )
	{
		return false;
	}

	// 아이템 종류 검사
	if( pItem->GetItemType() == CDnItem::Quest )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return false;
	}

	// 플레이트 아이템인지 검사.
	if( pItem->GetItemType() != CDnItem::Plate )
	{
		GetInterface().MessageBox( 9008, MB_OK );
		return false;
	}

	return true;
}

void CDnPlateListSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_OK") )
		{
			if( m_emStep == stepListSelect )
			{
				int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
				CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
				GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );
				if( GetItemTask().GetCoin() < CompoundInfo.iCost )
				{
					// 더블클릭해서 들어와 수수료가 부족할땐, 활성화 되지도 않은 버튼인데 메세지가 뜨게 될거다.
					// 이거 별로인거 같아, 그냥 아무일 하지 않고 넘어가기로 하겠다.
					//GetInterface().MessageBox( 4036, MB_OK );
					return;
				}
				
				ChangeStep( stepSlotItem );
			}
			else if( m_emStep == stepSlotItem )
			{
				if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ) )
				{
					GetInterface().MessageBox( 7012, MB_OK );
					return;
				}

				// 조합하시겠습니까? 메세지 후 처리해야한다.
				GetInterface().MessageBox( 9009, MB_YESNO, COMPOUND_CONFIRM_DIALOG, this );
				return;
			}
			return;
		}
		else if( IsCmdControl("ID_BUTTON_CANCEL") )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0 );
			return;
		}
		else if( IsCmdControl("ID_ITEM_PLATE") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				ChangeStep( stepBase );
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if (pItem == NULL || IsEnableRegisterPlateItem(pDragButton, pItem) == false)
				{
					pDragButton->EnableSplitMode(true);
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());

				// 만약 이미 올려진 자리에 다른 아이템을 올리는 거라면,
				if( pPressedItem )
				{
					// 해당 아이템을 창에서 제거한다.
					SAFE_DELETE(m_pItem);
					pPressedButton->ResetSlot();
					m_pQuickSlotButton->SetRegist( false );
					m_pQuickSlotButton = NULL;
				}

				// 플레이트를 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
				//pPressedButton->SetItem( pItem );
				// 이렇게 임시템을 만들어서 넣어준다.
				TItemInfo itemInfo;
				pItem->GetTItemInfo(itemInfo);
				m_pItem = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pQuickSlotButton = (CDnQuickSlotButton *)pDragButton;
				m_pQuickSlotButton->SetRegist( true );

				ChangeStep( stepPlateRegist );
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPlateListSelectDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case COMPOUND_CONFIRM_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_YES") )
			{
				// 서버에다 등록 요청 후 해당 패킷 받으면 연출동영상 띄우는거다.
				OutputDebug( "플레이트 제작 패킷 전송\n" );
				GetItemTask().RequestEmblemCompoundOpen( static_cast<CDnItem*>(m_pItemSlotButton->GetItem()), m_pPlateListDlg->GetSelectedCompoundID(), m_vNeedItemInfo );

				// 임시
				//int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
				//CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
				//GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );
				//GetInterface().OpenMovieProcessDlg(
				//	"PlateCombine.avi", CompoundInfo.fTimeRequired / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9011 ), COMPOUND_MOVIE_DIALOG, this );
			}
		}
		break;
	case COMPOUND_MOVIE_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			// 안보이는 버튼. 프로세스 처리가 끝났음을 알린다.
			if( IsCmdControl("ID_PROCESS_COMPLETE") )
			{
				// 완료 패킷을 보낸다.
				// Send...
				GetItemTask().RequestEmblemCompound( static_cast<CDnItem*>(m_pItemSlotButton->GetItem()), m_pPlateListDlg->GetSelectedCompoundID(), m_vNeedItemInfo );
				OutputDebug( "문장보옥 조합 완성 패킷 전송\n" );

			}
			else if( IsCmdControl("ID_BUTTON_CANCEL") )
			{
				// 취소 패킷을 보낸다.
				// Send...
				OutputDebug( "문장보옥 조합 중 캔슬 패킷 전송\n" );
				GetItemTask().RequestEmblemCompoundCancel();
			}
		}
		break;
	}
}

void CDnPlateListSelectDlg::OnRecvEmblemCompoundOpen()
{
	// 조합하려는 아이템에 따라 적절한 시간을 구해온 후
	int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	GetInterface().OpenMovieProcessDlg(
		"PlateCombine.avi", CompoundInfo.fTimeRequired / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8013 ), COMPOUND_MOVIE_DIALOG, this );

	// 직접 호출하는게 아니라, 다른 곳에서 호출시 this대신하려면,
	// GetInterface().GetMainMenuDialog(CDnMainMenuDlg::PLATE_LIST_SELECT_DIALOG)
}

void CDnPlateListSelectDlg::OnRecvEmblemCompound()
{
	// 문장보옥 조합결과가 실패하던지 성공하던지 기본스텝으로 돌아간다.
	ChangeStep( stepBase );
}

void CDnPlateListSelectDlg::ChangeStep( emPlateStep emStep )
{
	m_emStep = emStep;

	if( m_emStep == stepBase )
	{
		SAFE_DELETE(m_pItem);
		m_pItemSlotButton->ResetSlot();
		m_pStaticFix->Show( false );

		if( m_pQuickSlotButton )
		{
			m_pQuickSlotButton->SetRegist( false );
			m_pQuickSlotButton = NULL;
		}

		m_pStaticBack->Show( true );

		m_pStaticGold->SetIntToText(0);
		m_pStaticSilver->SetIntToText(0);
		m_pStaticBronze->SetIntToText(0);

		m_pButtonOK->Enable( false );
		m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 74 ) );
		m_pButtonCancel->Enable( true );

		ClosePlateList();
		m_SlotDlgGroup.CloseAllDialog();
		ClosePlateMixView();
	}
	else if( m_emStep == stepPlateRegist )
	{
		m_pStaticFix->Show( true );
		m_pStaticBack->Show( false );

		// 수수료 초기화
		m_pStaticGold->SetIntToText(0);
		m_pStaticSilver->SetIntToText(0);
		m_pStaticBronze->SetIntToText(0);

		m_pButtonOK->Enable( false );
		m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 74 ) );

		RefreshPlateList();
		m_SlotDlgGroup.CloseAllDialog();
		ClosePlateMixView();
	}
	else if( m_emStep == stepListSelect )
	{
		int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

		// 수수료를 얻어와서 보여준다. 이게 int형을 넘지는 않을테니.. 강제형변환.
		int nPrice = (int)CompoundInfo.iCost;
		int nGold = nPrice/10000;
		int nSilver = (nPrice%10000)/100;
		int nBronze = nPrice%100;
		m_pStaticGold->SetIntToText( nGold );
		m_pStaticSilver->SetIntToText( nSilver );
		m_pStaticBronze->SetIntToText( nBronze );

		if( GetItemTask().GetCoin() >= CompoundInfo.iCost )
			m_pButtonOK->Enable( true );
		else
			m_pButtonOK->Enable( false );
		m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 74 ) );
	}
	else if( m_emStep == stepSlotItem )
	{
		int nCompoundID = m_pPlateListDlg->GetSelectedCompoundID();
		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

		m_pButtonOK->Enable( false );
		m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9003 ) );

		// NUM_MAX_NEED_ITEM 원래는 이거 써야하는데, 문장보옥은 많아야 5개라 한다.
		int nNumNeedItem;
		for(nNumNeedItem = 0; nNumNeedItem < 5; ++nNumNeedItem)
			if( CompoundInfo.aiItemID[nNumNeedItem] == 0 ) break;

		// 필요 슬롯 갯수에 따라 적절한 다이얼로그를 보여준다.
		ASSERT(nNumNeedItem&&"조합에 필요한 슬롯 갯수가 0개면 잘못된 데이터 아닌가.");

		// 필요 슬롯 갯수에 따라 아이템 슬롯 다이얼로그를 보여준다.
		ClosePlateList();
		m_SlotDlgGroup.ShowDialog(nNumNeedItem-1, true);
		ShowPlateMixView();

		int nProb = (int)(CompoundInfo.fSuccessPossibility * 100.0f);
		if( CompoundInfo.fSuccessPossibility * 100.0f - (float)nProb > 0.999 ) nProb += 1;
		int nResultItemID = 0;
		if( CompoundInfo.aiSuccessTypeOrCount[0] == -1 && CompoundInfo.aiSuccessItemID[0] > 0 )
			nResultItemID = CompoundInfo.aiSuccessItemID[0];
		m_pPlateMixViewDlg->SetInfo( nProb, nResultItemID );
		bool bResult = ((CDnPlateItemSlotDlg *)m_SlotDlgGroup.GetDialog(nNumNeedItem-1))->SetCompoundInfo(CompoundInfo.aiItemID, CompoundInfo.aiItemCount);

		// 아이템슬롯 단계에서 서버에 전송할 필요아이템정보 구조체를 설정해둔다.
		m_vNeedItemInfo.clear();
		for( int i = 0; i < nNumNeedItem; ++i )
		{
			CDnItemTask::ItemCompoundInfo Info;
			Info.iItemID = CompoundInfo.aiItemID[ i ];
			Info.iUseCount = CompoundInfo.aiItemCount[ i ];
			m_vNeedItemInfo.push_back(Info);
		}

		if( bResult )
			m_pButtonOK->Enable( true );
		else
			m_pButtonOK->Enable( false );
	}
}

void CDnPlateListSelectDlg::RefreshPlateList()
{
	if( m_pPlateListDlg )
	{
		if( !m_pPlateListDlg->IsShow() ) ShowChildDialog( m_pPlateListDlg, true );
	}
	m_pPlateListDlg->RefreshPlateList();
}

void CDnPlateListSelectDlg::ClosePlateList()
{
	if( m_pPlateListDlg )
	{
		ShowChildDialog( m_pPlateListDlg, false );
	}
}

void CDnPlateListSelectDlg::ShowPlateMixView()
{
	if( m_pPlateMixViewDlg )
	{
		if( !m_pPlateMixViewDlg->IsShow() ) ShowChildDialog( m_pPlateMixViewDlg, true );
	}
}

void CDnPlateListSelectDlg::ClosePlateMixView()
{
	if( m_pPlateMixViewDlg )
	{
		ShowChildDialog( m_pPlateMixViewDlg, false );
	}
}

void CDnPlateListSelectDlg::SetPlateItem( CDnQuickSlotButton *pPressedButton )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pQuickSlotButton && (pItem == m_pQuickSlotButton->GetItem()) )
	{
		return;
	}

	// 아이템 종류 검사
	if( pItem->GetItemType() == CDnItem::Quest )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return;
	}

	// 플레이트 아이템인지 검사.
	if( pItem->GetItemType() != CDnItem::Plate )
	{
		GetInterface().MessageBox( 9008, MB_OK );
		return;
	}

	// 칸이 하나이므로 더 이상 등록할 수 없습니다.보단 재등록이 낫다.
	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlotButton->GetItem());
	if( pOriginItem )
	{
		// 해당 아이템을 창에서 제거한다.
		SAFE_DELETE(m_pItem);
		m_pItemSlotButton->ResetSlot();
		m_pStaticFix->Show( false );
		m_pQuickSlotButton->SetRegist( false );
		m_pQuickSlotButton = NULL;
	}

	// 플레이트를 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
	//pPressedButton->SetItem( pItem );
	// 이렇게 임시템을 만들어서 넣어준다.
	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);
	m_pItem = GetItemTask().CreateItem(itemInfo);
	m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pQuickSlotButton = (CDnQuickSlotButton *)pPressedButton;
	m_pQuickSlotButton->SetRegist( true );

	ChangeStep( stepPlateRegist );
}

int CDnPlateListSelectDlg::GetPlateItemID()
{
	if( m_emStep != stepBase )
	{
		CDnItem *pPlateItem = static_cast<CDnItem *>(m_pItemSlotButton->GetItem());
		return pPlateItem->GetClassID();
	}
	else return 0;
}*/