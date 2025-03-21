#include "StdAfx.h"
#include "DnItemCompoundTabDlg.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnItemCompoundMixDlg.h"
#include "DnMainMenuDlg.h"
#include "DnMainFrame.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnItemCompoundTabDlg::CDnItemCompoundTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pTabButtonWeapon(NULL)
, m_pTabButtonParts(NULL)
, m_pTabButtonAccessory(NULL)
, m_pTabButtonNormal(NULL)
, m_nCurTab(0)
, m_pComboBoxJob(NULL)
, m_pComboBoxType(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_pStaticSelect(NULL)
, m_nCurSelect(-1)
, m_pButtonPagePrev(NULL)
, m_pButtonPageNext(NULL)
, m_pStaticPage(NULL)
, m_nCurPage(0)
, m_nMaxPage(0)
, m_nCurCompoundListID(-1)
, m_pCompoundMixDlg(NULL)
, m_bOK(false)
, m_pGold(NULL)
, m_pSilver(NULL)
, m_pBronze(NULL)
{
	for( int i = 0; i < NUM_NEED_ITEM; ++i )
	{
		m_pNeedItem[i] = NULL;
		m_pNeedItemSlotButton[i] = NULL;
	}
}

CDnItemCompoundTabDlg::~CDnItemCompoundTabDlg(void)
{
	for( int i = 0; i < NUM_NEED_ITEM; ++i )
		SAFE_DELETE(m_pNeedItem[i]);
	SAFE_DELETE(m_pCompoundMixDlg);
}

void CDnItemCompoundTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompoundTabDlg.ui" ).c_str(), bShow );
}

void CDnItemCompoundTabDlg::InitialUpdate()
{
	m_pTabButtonWeapon = GetControl<CEtUIRadioButton>("ID_TAB_WEAPON");
	m_pTabButtonParts = GetControl<CEtUIRadioButton>("ID_TAB_PARTS");
	m_pTabButtonAccessory = GetControl<CEtUIRadioButton>("ID_TAB_ACCESSORY");
	m_pTabButtonNormal = GetControl<CEtUIRadioButton>("ID_TAB_NORMAL");

	m_pComboBoxJob = GetControl<CEtUIComboBox>("ID_COMBOBOX_JOB");

	// 타입에 들어가는 내용은 탭마다 달라 여기서 넣지 않는다.
	m_pComboBoxType = GetControl<CEtUIComboBox>("ID_COMBOBOX_TYPE");

	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");

	char szControlName[32];
	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_SLOT%d", i);
		m_sCompoundItem[i].m_pItemSlotButton = GetControl<CDnItemSlotButton>(szControlName);
		m_sCompoundItem[i].m_pItemSlotButton->SetSlotType( ST_ITEM_COMPOUND );

		sprintf_s(szControlName, _countof(szControlName), "ID_BASE%d", i);
		m_sCompoundItem[i].m_pStaticBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_NAME%d", i);
		m_sCompoundItem[i].m_pStaticName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_LEVEL%d", i);
		m_sCompoundItem[i].m_pStaticLevelText = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_LEVEL%d", i);
		m_sCompoundItem[i].m_pStaticLevel = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_TYPE%d", i);
		m_sCompoundItem[i].m_pStaticType = GetControl<CEtUIStatic>(szControlName);
	}

	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; ++i )
		m_sCompoundItem[i].Clear();

	m_pStaticSelect = GetControl<CEtUIStatic>("ID_STATIC_SELECT");
	m_pStaticSelect->Show(false);

	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_PAGE_PREV");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_PAGE_NEXT");
	m_pStaticPage = GetControl<CEtUIStatic>("ID_PAGE_NUM");
	m_nCurPage = 0;

	for( int i = 0; i < NUM_NEED_ITEM; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "%s%d", "ID_ITEM_SLOT", i);
		m_pNeedItemSlotButton[i] = GetControl<CDnItemSlotButton>(szControlName);
		m_pNeedItemSlotButton[i]->SetSlotType( ST_ITEM_COMPOUND_JEWEL );
	}
	m_pGold = GetControl<CEtUIStatic>("ID_GOLD"); 
	m_pSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_dwColorGold = m_pGold->GetTextColor();
	m_dwColorSilver = m_pSilver->GetTextColor();
	m_dwColorBronze = m_pBronze->GetTextColor();

	m_pTabButtonWeapon->SetChecked(true);
	m_nCurTab = 0;

	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 46 ), NULL, 1 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 47 ), NULL, 2 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 49 ), NULL, 3 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 48 ), NULL, 4 );
#ifdef PRE_ADD_ACADEMIC
#ifndef PRE_REMOVE_ACADEMIC
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 305 ), NULL, 5 );
#endif
#endif
#if defined( PRE_ADD_KALI ) && !defined( PRE_REMOVE_KALI )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 306 ), NULL, 6 );
#endif
//[OK]
#if defined( PRE_ADD_ASSASSIN ) && !defined( PRE_REMOVE_ASSASSIN )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 307 ), NULL, 7 );
#endif
#if defined( PRE_ADD_LENCEA ) 
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 308 ), NULL, 8 );
#endif
#if defined( PRE_ADD_MACHINA ) 
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 376 ), NULL, 9 );
#endif

	// Child_modal의 경우 인벤토리쪽 입력을 막을 수 없어서, 그리고 인벤토리보다 먼저 렌더링된다는 점에서 별로다.
	// 그래서 상점판매확인창에도 Modal로 띄운 듯.
	m_pCompoundMixDlg = new CDnItemCompoundMixDlg( UI_TYPE_MODAL, NULL, COMPOUND_CONFIRM_DIALOG, this );
	m_pCompoundMixDlg->Initialize( false );
}

void CDnItemCompoundTabDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_nCurSelect = -1;
		CheckUsableTab();
		m_nCurPage = 0;
		m_pComboBoxJob->SetSelectedByValue( CDnActor::s_hLocalActor->GetClassID() );
		UpdateList();
	}
	else
	{
		ResetList();
		GetInterface().CloseBlind();
		m_pButtonOK->Enable( false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnItemCompoundTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_PAGE_PREV" ) )
		{
			PrevPage();
			UpdateCompoundSlot();
			return;
		}
		else if( IsCmdControl("ID_PAGE_NEXT" ) )
		{
			NextPage();
			UpdateCompoundSlot();
			return;
		}
		else if( IsCmdControl("ID_CANCEL") )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0 );
			return;
		}
		else if( IsCmdControl("ID_OK") )
		{
			if( m_pButtonOK->IsEnable() )
				ShowConfirmDlg( m_sCompoundItem[m_nCurSelect].m_nCompoundID, m_bOK );
			return;
		}
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( IsCmdControl("ID_TAB_WEAPON") )
		{
			m_nCurTab = 0;
			UpdateList();
			return;
		}
		else if( IsCmdControl("ID_TAB_PARTS") )
		{
			m_nCurTab = 1;
			UpdateList();
			return;
		}
		else if( IsCmdControl("ID_TAB_ACCESSORY") )
		{
			m_nCurTab = 2;
			UpdateList();
			return;
		}
		else if( IsCmdControl("ID_TAB_NORMAL") )
		{
			m_nCurTab = 3;
			UpdateList();
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED ) 
	{
		if( IsCmdControl("ID_COMBOBOX_JOB") )
		{
			UpdateList();
			
		}
		else if( IsCmdControl("ID_COMBOBOX_TYPE") )
		{
			UpdateCompoundList();
			UpdatePage();
			UpdateCompoundSlot();
		}
		// 콤보박스에 가있는 포커스를 풀어야 콤보박스 변경 후 focus::HandleMouse 함수로 휠메세지가 가지 않게된다.
		focus::ReleaseControl();
		return;
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnItemCompoundTabDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	if( m_pComboBoxJob && m_pComboBoxType &&
		!m_pComboBoxJob->IsOpenedDropDownBox() && !m_pComboBoxType->IsOpenedDropDownBox() )
	{
		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				POINT MousePoint;
				float fMouseX, fMouseY;

				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				int nFindSelect = FindInsideItem( fMouseX, fMouseY );
				if( nFindSelect != -1 )
				{
					if( m_sCompoundItem[nFindSelect].m_pStaticBase->IsShow() )
					{
						m_nCurSelect = nFindSelect;
						UpdateSelectBar();
						return true;
					}
					else
					{
						for( int i = 0; i < NUM_NEED_ITEM; ++i )
							m_pNeedItemSlotButton[i]->ResetSlot();

						m_pGold->SetIntToText( 0 );
						m_pSilver->SetIntToText( 0 );
						m_pBronze->SetIntToText( 0 );
						m_pGold->SetTextColor( m_dwColorGold );
						m_pSilver->SetTextColor( m_dwColorSilver );
						m_pBronze->SetTextColor( m_dwColorBronze );

						m_pStaticSelect->Show(false);
						m_nCurSelect = -1;
						m_pButtonOK->Enable( false );
					}
				}
			}
			break;
		case WM_LBUTTONDBLCLK:
			if( !IsMouseInDlg() ) break;
			ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_OK"), 0 );
			return true;
		case WM_MOUSEWHEEL:
			{
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );

				ScreenToClient( hWnd, &MousePoint );
				lParam = MAKELPARAM( MousePoint.x, MousePoint.y );

				if( IsMouseInDlg() )
				{
					UINT uLines;
					SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
					int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
					if( nScrollAmount > 0 )
					{
						if( m_nCurPage > 0 )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_PAGE_PREV"), 0 );
					}
					else if( nScrollAmount < 0 )
					{
						if( m_nCurPage < m_nMaxPage )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_PAGE_NEXT"), 0 );
					}

					// 마우스무브 메세지로 툴팁이 갱신되게 한다.
					CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
					return true;
				}
			}
			break;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnItemCompoundTabDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID ) 
		{
		case COMPOUND_CONFIRM_DIALOG:
			{
				if( IsCmdControl("ID_OK") )
				{
					if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ) )
					{
						GetInterface().MessageBox( 7012, MB_OK );
						return;
					}

					// 서버에다 등록 요청 후 해당 패킷 받으면 연출동영상 띄우는거다.
					OutputDebug( "아이템조합 요청 패킷 전송\n" );
					GetItemTask().RequestItemCompoundOpen( m_sCompoundItem[m_nCurSelect].m_nCompoundID, m_vNeedItemInfo );
					m_pCompoundMixDlg->EnableButton( false );
//					m_pCompoundMixDlg->Show( false );
				}
				else if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_CLOSE_DIALOG") )
				{
					m_pCompoundMixDlg->Show( false );
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
					GetItemTask().RequestItemCompound( m_sCompoundItem[m_nCurSelect].m_nCompoundID, m_vNeedItemInfo );
					OutputDebug( "아이템조합 실제요청 패킷 전송\n" );
				}
				else if( IsCmdControl("ID_BUTTON_CANCEL") )
				{
					// 취소 패킷을 보낸다.
					// Send...
					OutputDebug( "문장보옥 조합 중 캔슬 패킷 전송\n" );
					GetItemTask().RequestItemCompoundCancel();
				}
			}
			break;
		}
	}
}

int CDnItemCompoundTabDlg::FindInsideItem( float fX, float fY )
{
	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; i++ )
	{
		if( m_sCompoundItem[i].IsInsideItem( fX, fY ) )
		{
			return i;
		}
	}

	return -1;
}

void CDnItemCompoundTabDlg::UpdateSelectBar()
{
	SUICoord uiCoordBase, uiCoordSelect;
	m_sCompoundItem[m_nCurSelect].m_pStaticBase->GetUICoord(uiCoordBase);
	m_pStaticSelect->GetUICoord(uiCoordSelect);
	uiCoordSelect.fX = uiCoordBase.fX - 0.0039f;
	uiCoordSelect.fY = uiCoordBase.fY- 0.005f;
	m_pStaticSelect->SetUICoord(uiCoordSelect);
	m_pStaticSelect->Show(true);

	CheckCompound();
}

void CDnItemCompoundTabDlg::SetCompoundListID( int nListID )
{
	m_nCurCompoundListID = nListID;
	m_vecCompoundID.clear();

	// 테이블에서 읽어온다.
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMCOMPOUNDSHOP );
	if( !pSox ) return;

	vector<int> vCompoundShotItemIDList;
	int nNum = pSox->GetItemIDListFromField( "_ListID", nListID, vCompoundShotItemIDList );
	if( nNum <= 0 )
	{
		ASSERT(0&&"조합NPC의 인덱스로 얻은 조합ID의 총 갯수가 0이하입니다. 데이터가 이상합니다.");
	}

	for( int i = 0; i < (int)vCompoundShotItemIDList.size(); ++i )
		m_vecCompoundID.push_back( pSox->GetFieldFromLablePtr( vCompoundShotItemIDList[i], "_ItemCompoundID" )->GetInteger() );
}

void CDnItemCompoundTabDlg::ResetList()
{
	m_nCurCompoundListID = -1;
	m_vecCompoundID.clear();
	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; ++i )
		m_sCompoundItem[i].Clear();

	for( int i = 0; i < NUM_NEED_ITEM; ++i )
		m_pNeedItemSlotButton[i]->ResetSlot();

	m_pGold->SetIntToText( 0 );
	m_pSilver->SetIntToText( 0 );
	m_pBronze->SetIntToText( 0 );
	m_pGold->SetTextColor( m_dwColorGold );
	m_pSilver->SetTextColor( m_dwColorSilver );
	m_pBronze->SetTextColor( m_dwColorBronze );

	m_nCurSelect = -1;
	m_pStaticSelect->Show( false );
}

void CDnItemCompoundTabDlg::CheckUsableTab()
{
	// #20161 이슈 관련.
	// 원래는 각 탭에 들어가는 아이템을 검사해서 탭에 아이템이 없을경우 하이드 시키고,
	// 이후 마지막 하나의 탭만 남을 경우 해당 탭을 선택한 후 탭들을 다 하이드 시킨다.
	bool bTab[4] = {0,};
	for( int i = 0; i < (int)m_vecCompoundID.size(); ++i )
	{
		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( m_vecCompoundID[i], &CompoundInfo );
		int nItemID = CompoundInfo.aiSuccessItemID[0];

		// 이 아이템번호로 아이템을 만들기엔 CreateItem에 너무 많은게 들어있어서 직접 데이터를 얻어오기로 하겠다.
		// 먼저 탭으로 골라낸다.
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return;
		if( !pSox->IsExistItem( nItemID ) ) continue;
		eItemTypeEnum ItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();

		if( ItemType == ITEMTYPE_WEAPON ) bTab[0] = true;
		else if( ItemType == ITEMTYPE_PARTS )
		{
			DNTableFileFormat* pSoxParts = GetDNTable( CDnTableDB::TPARTS );
			if( !pSoxParts ) return;
			if( !pSoxParts->IsExistItem( nItemID ) ) continue;
			CDnParts::PartsTypeEnum PartsType = (CDnParts::PartsTypeEnum)pSoxParts->GetFieldFromLablePtr( nItemID, "_Parts" )->GetInteger();
			if( PartsType >= CDnParts::Helmet && PartsType <= CDnParts::Foot ) bTab[1] = true;
			if( PartsType >= CDnParts::Necklace && PartsType <= CDnParts::Ring ) bTab[2] = true;
		}
		else if( 1 )
		{
			// 여기까지 들어오면 일반탭에 아이템 있다는 것이다. 타입 검사하지 않는다. CDnItem::Normal아닌것도 제작할 수 있기 때문.
			bTab[3] = true;
		}
	}

	int nSum = 0;
	int nLastIndex = -1;
	for( int i = 0; i < sizeof(bTab); ++i )
	{
		nSum += (int)bTab[i];
		if( bTab[i] ) nLastIndex = i;
	}

	if( nSum == 1 )
	{
		m_pTabButtonWeapon->Show( false );
		m_pTabButtonParts->Show( false );
		m_pTabButtonAccessory->Show( false );
		m_pTabButtonNormal->Show( false );

		switch( nLastIndex )
		{
		case 0:	m_pTabButtonWeapon->SetChecked( true );		break;
		case 1:	m_pTabButtonParts->SetChecked( true );		break;
		case 2:	m_pTabButtonAccessory->SetChecked( true );	break;
		case 3:	m_pTabButtonNormal->SetChecked( true );		break;
		}
		m_nCurTab = nLastIndex;
	}
	else
	{
		// 두개 이상의 탭에 조합목록이 들어있을땐 기본대로 한다.(만약 아무것도 조합목록이 없더라도 이렇게 한다.)
		m_pTabButtonWeapon->Show( true );
		m_pTabButtonParts->Show( true );
		m_pTabButtonAccessory->Show( true );
		m_pTabButtonNormal->Show( true );

		m_pTabButtonWeapon->SetChecked( true );
		m_nCurTab = 0;
	}
}

void CDnItemCompoundTabDlg::UpdateList()
{
	UpdateComboBoxType();
	UpdateCompoundList();
	UpdatePage();
	UpdateCompoundSlot();
}

void CDnItemCompoundTabDlg::UpdateComboBoxType()
{
	if( !m_pComboBoxType ) return;

	m_pComboBoxType->RemoveAllItems();

	// 현재 선택된 탭과, 콤보박스 직업에 따라 내용이 바뀐다.
	std::vector<int> vecType;
	if( m_nCurTab == 0 )
	{
		SComboBoxItem *pItem = m_pComboBoxJob->GetSelectedItem();
		if( !pItem ) return;

		switch( pItem->nValue )
		{
		case 1:
			vecType.push_back(CDnWeapon::Sword);
			vecType.push_back(CDnWeapon::Axe);
			vecType.push_back(CDnWeapon::Hammer);
			vecType.push_back(CDnWeapon::Gauntlet);
			break;
		case 2:
			vecType.push_back(CDnWeapon::SmallBow);
			vecType.push_back(CDnWeapon::BigBow);
			vecType.push_back(CDnWeapon::CrossBow);
			vecType.push_back(CDnWeapon::Arrow);
			break;
		case 3:
			vecType.push_back(CDnWeapon::Staff);
			vecType.push_back(CDnWeapon::Orb);
			vecType.push_back(CDnWeapon::Puppet);
			vecType.push_back(CDnWeapon::Book);
			break;
		case 4:
			vecType.push_back(CDnWeapon::Mace);
			vecType.push_back(CDnWeapon::Flail);
			vecType.push_back(CDnWeapon::Wand);
			vecType.push_back(CDnWeapon::Shield);
			break;
		case 5:
			vecType.push_back(CDnWeapon::Cannon);
			vecType.push_back(CDnWeapon::BubbleGun);
			vecType.push_back(CDnWeapon::Glove);
			break;
		case 6:
			vecType.push_back(CDnWeapon::Fan);
			vecType.push_back(CDnWeapon::Chakram);
			vecType.push_back(CDnWeapon::Charm);
			break;
		case 7:
			vecType.push_back(CDnWeapon::Scimiter);
			vecType.push_back(CDnWeapon::Dagger);
			vecType.push_back(CDnWeapon::Crook);
			break;
		case 8:
			vecType.push_back(CDnWeapon::Spear);
			vecType.push_back(CDnWeapon::Bracelet);
			break;
		case 9:
			vecType.push_back(CDnWeapon::KnuckleGear);
			vecType.push_back(CDnWeapon::Claw);
			break;
		}
#pragma message(" ------> DNItemCompoundTabDLG 589 Add other items !")
		m_pComboBoxType->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 79 ), NULL, 99 );
		for( int i = 0; i < (int)vecType.size(); ++i )
			m_pComboBoxType->AddItem( DN_INTERFACE::STRING::ITEM::GetEquipString( (CDnWeapon::EquipTypeEnum)vecType[i]), NULL, vecType[i] );
	}
	else if( m_nCurTab == 1 )
	{
		SComboBoxItem *pItem = m_pComboBoxJob->GetSelectedItem();
		if( !pItem ) return;

		// 모두보기면, Type도 모두 보여줄까.
		switch( pItem->nValue )
		{
		case 1:
		case 2:
		case 3:
		case 4:
			vecType.push_back(CDnParts::Helmet);
			vecType.push_back(CDnParts::Body);
			vecType.push_back(CDnParts::Leg);
			vecType.push_back(CDnParts::Hand);
			vecType.push_back(CDnParts::Foot);
			break;
		}

		m_pComboBoxType->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 79 ), NULL, 99 );
		for( int i = 0; i < (int)vecType.size(); ++i )
			m_pComboBoxType->AddItem( DN_INTERFACE::STRING::ITEM::GetPartsString( (CDnParts::PartsTypeEnum)vecType[i]), NULL, vecType[i] );
	}
	else if( m_nCurTab == 2 )
	{
		SComboBoxItem *pItem = m_pComboBoxJob->GetSelectedItem();
		if( !pItem ) return;

		// 모두보기면, Type도 모두 보여줄까.
		switch( pItem->nValue )
		{
		case 1:
		case 2:
		case 3:
		case 4:
			vecType.push_back(CDnParts::Necklace);
			vecType.push_back(CDnParts::Earring);
			vecType.push_back(CDnParts::Ring);
			break;
		}

		m_pComboBoxType->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 79 ), NULL, 99 );
		for( int i = 0; i < (int)vecType.size(); ++i )
			m_pComboBoxType->AddItem( DN_INTERFACE::STRING::ITEM::GetPartsString( (CDnParts::PartsTypeEnum)vecType[i]), NULL, vecType[i] );
	}
	else if( m_nCurTab == 3 )
	{
		SComboBoxItem *pItem = m_pComboBoxJob->GetSelectedItem();
		if( !pItem ) return;

		// 모두보기면, Type도 모두 보여줄까.
		switch( pItem->nValue )
		{
		case 1:
		case 2:
		case 3:
		case 4:
			vecType.push_back(ITEMRANK_D);
			vecType.push_back(ITEMRANK_C);
			vecType.push_back(ITEMRANK_B);
			vecType.push_back(ITEMRANK_A);
			vecType.push_back(ITEMRANK_S);
			break;
		}

		m_pComboBoxType->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 79 ), NULL, 99 );
		for( int i = 0; i < (int)vecType.size(); ++i )
			m_pComboBoxType->AddItem( DN_INTERFACE::STRING::ITEM::GetRankString( (eItemRank)vecType[i]), NULL, vecType[i] );
	}

	m_pComboBoxType->SetSelectedByIndex(0);
}

static bool CompareCompoundInfo( int nCompoundID1, int nCompoundID2 )
{
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo1, CompoundInfo2;
	GetItemTask().GetCompoundInfo( nCompoundID1, &CompoundInfo1 );
	GetItemTask().GetCompoundInfo( nCompoundID2, &CompoundInfo2 );
	int nResultItemID1 = CompoundInfo1.aiSuccessItemID[0];
	int nResultItemID2 = CompoundInfo2.aiSuccessItemID[0];

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( nResultItemID1 ) ) return false;
	if( !pSox->IsExistItem( nResultItemID2 ) ) return false;

	int nLevelLimit1 = pSox->GetFieldFromLablePtr( nResultItemID1, "_LevelLimit" )->GetInteger();
	int nLevelLimit2 = pSox->GetFieldFromLablePtr( nResultItemID2, "_LevelLimit" )->GetInteger();

	if( nLevelLimit1 < nLevelLimit2 ) return true;
	else if( nLevelLimit1 > nLevelLimit2 ) return false;

	// 레벨 제한이 같다면, 원래대로 
	if( CompoundInfo1.iCompoundTableID < CompoundInfo2.iCompoundTableID ) return true;
	else if( CompoundInfo1.iCompoundTableID > CompoundInfo2.iCompoundTableID ) return false;

	return false;
}

void CDnItemCompoundTabDlg::UpdateCompoundList()
{
	m_vecCompoundIDinList.clear();

	// 현재 선택된 탭과, 직업과 타입에 따라 출력할 조합 리스트를 골라낸다.
	for( int i = 0; i < (int)m_vecCompoundID.size(); ++i )
	{
		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( m_vecCompoundID[i], &CompoundInfo );
		int nItemID = CompoundInfo.aiSuccessItemID[0];

		// 이 아이템번호로 아이템을 만들기엔 CreateItem에 너무 많은게 들어있어서 직접 데이터를 얻어오기로 하겠다.
		// 먼저 탭으로 골라낸다.
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return;
		if( !pSox->IsExistItem( nItemID ) ) continue;
		eItemTypeEnum ItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();

		if( m_nCurTab == 0 && ItemType != ITEMTYPE_WEAPON ) continue;
		else if( m_nCurTab == 1 && ItemType != ITEMTYPE_PARTS ) continue;
		else if( m_nCurTab == 2 && ItemType != ITEMTYPE_PARTS ) continue;	// 장신구도 Parts에 속한다.
		else if( m_nCurTab == 3 && (ItemType == ITEMTYPE_WEAPON || ItemType == ITEMTYPE_PARTS) ) continue;

		// 직업을 골라낸다.
		{
			std::vector<int> vecPermitJobList;
			char *szPermitStr = pSox->GetFieldFromLablePtr( nItemID, "_NeedJobClass" )->GetString();
			int nJobIndex;
			for( int i=0;; i++ ) {
				//char *pStr = _GetSubStrByCount( i, szPermitStr, ';' );
				std::string strValue = _GetSubStrByCountSafe( i, szPermitStr, ';' );
				if( strValue.size() == 0 ) break;
				nJobIndex = atoi(strValue.c_str());
				if( nJobIndex < 1 ) continue;
				vecPermitJobList.push_back( nJobIndex );
			}
			int nCurJob;
			if( m_pComboBoxJob->GetSelectedValue(nCurJob) )
			{
				if( (int)vecPermitJobList.size() == 1 && nCurJob != vecPermitJobList[0] )
					continue;
			}
		}
		// 타입을 골라낸다.
		int nCurType;
		if( m_nCurTab == 0 )
		{
			// 무기면, 무기쪽에서 검사
			DNTableFileFormat* pSoxWeapon = GetDNTable( CDnTableDB::TWEAPON );
			if( !pSoxWeapon ) return;
			if( !pSoxWeapon->IsExistItem( nItemID ) ) continue;
			CDnWeapon::EquipTypeEnum EquipType = (CDnWeapon::EquipTypeEnum)pSoxWeapon->GetFieldFromLablePtr( nItemID, "_EquipType" )->GetInteger();
			if( m_pComboBoxType->GetSelectedValue(nCurType) )
			{
				if( nCurType != 99 && nCurType != EquipType )
					continue;
			}
		}
		else if( m_nCurTab == 1 )
		{
			DNTableFileFormat* pSoxParts = GetDNTable( CDnTableDB::TPARTS );
			if( !pSoxParts ) return;
			if( !pSoxParts->IsExistItem( nItemID ) ) continue;
			CDnParts::PartsTypeEnum PartsType = (CDnParts::PartsTypeEnum)pSoxParts->GetFieldFromLablePtr( nItemID, "_Parts" )->GetInteger();
			if( m_pComboBoxType->GetSelectedValue(nCurType) )
			{
				if( PartsType < CDnParts::Helmet || PartsType > CDnParts::Foot )
					continue;
				if( nCurType != 99 && nCurType != PartsType )
					continue;
			}
		}
		else if( m_nCurTab == 2 )
		{
			DNTableFileFormat* pSoxParts = GetDNTable( CDnTableDB::TPARTS );
			if( !pSoxParts ) return;
			if( !pSoxParts->IsExistItem( nItemID ) ) continue;
			CDnParts::PartsTypeEnum PartsType = (CDnParts::PartsTypeEnum)pSoxParts->GetFieldFromLablePtr( nItemID, "_Parts" )->GetInteger();
			if( m_pComboBoxType->GetSelectedValue(nCurType) )
			{
				if( PartsType < CDnParts::Necklace || PartsType > CDnParts::Ring )
					continue;
				if( nCurType != 99 && nCurType != PartsType )
					continue;
			}
		}
		else if( m_nCurTab == 3 )
		{
			DNTableFileFormat* pSoxParts = GetDNTable( CDnTableDB::TITEM );
			if( !pSoxParts ) return;
			if( !pSoxParts->IsExistItem( nItemID ) ) continue;
			eItemRank Rank = (eItemRank)pSox->GetFieldFromLablePtr( nItemID, "_Rank" )->GetInteger();
			if( m_pComboBoxType->GetSelectedValue(nCurType) )
			{
				if( nCurType != 99 && nCurType != Rank )
					continue;
			}
		}
		else
		{
			// 그 외 이상한 데이터들은 그냥 패스.
			continue;
		}

		// 모든 검사에 통과한 것들은..
		m_vecCompoundIDinList.push_back(m_vecCompoundID[i]);
	}

	// 리스트 정렬. 낮은 레벨 순으로.
	std::sort( m_vecCompoundIDinList.begin(), m_vecCompoundIDinList.end(), CompareCompoundInfo );

	// 리스트가 갱신되었으니 페이지를 초기화한다.
	m_nCurPage = 0;

	// 맥스 페이지 설정하고(페이지가 한개면 0, 두개면 1인 최고 인덱스 저장이다.)
	m_nMaxPage = ((int)m_vecCompoundIDinList.size()-1) / NUM_MAX_COMPOUND_ITEM;
}

void CDnItemCompoundTabDlg::UpdatePage()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_nCurPage+1, m_nMaxPage+1 );
	m_pStaticPage->SetText( wszPage );

	if( m_nCurPage == m_nMaxPage )
		m_pButtonPageNext->Enable(false);
	else
		m_pButtonPageNext->Enable(true);

	if( m_nCurPage == 0 )
		m_pButtonPagePrev->Enable(false);
	else
		m_pButtonPagePrev->Enable(true);
}

void CDnItemCompoundTabDlg::UpdateCompoundSlot()
{
	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; ++i )
		m_sCompoundItem[i].Clear();

	for( int i = 0; i < NUM_NEED_ITEM; ++i )
		m_pNeedItemSlotButton[i]->ResetSlot();

	m_pGold->SetIntToText( 0 );
	m_pSilver->SetIntToText( 0 );
	m_pBronze->SetIntToText( 0 );
	m_pGold->SetTextColor( m_dwColorGold );
	m_pSilver->SetTextColor( m_dwColorSilver );
	m_pBronze->SetTextColor( m_dwColorBronze );

	int nOffset = m_nCurPage * NUM_MAX_COMPOUND_ITEM;

	// 필요한 것들만 보여준다.
	for( int i = nOffset; i < (int)m_vecCompoundIDinList.size(); ++i )
	{
		if( i >= nOffset + NUM_MAX_COMPOUND_ITEM )
			break;

		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( m_vecCompoundIDinList[i], &CompoundInfo );
		if( CompoundInfo.aiSuccessItemID[0] > 0 )
			m_sCompoundItem[i-nOffset].SetInfo( CompoundInfo.iCompoundTableID, CompoundInfo.aiSuccessItemID[0] );
	}

	UpdateCompoundableSlot();

	// 슬롯이 업데이트 되면, 선택란도 초기화한다.
	m_nCurSelect = -1;
	m_pStaticSelect->Show( false );
	m_pButtonOK->Enable( false );
}

void CDnItemCompoundTabDlg::UpdateCompoundableSlot()
{
	// 제작가능한지 살핀다.
	for( int i = 0; i < NUM_MAX_COMPOUND_ITEM; ++i )
	{
		if( m_sCompoundItem[i].m_nCompoundID && !m_sCompoundItem[i].m_pItemSlotButton->IsEmptySlot() )
		{
			if( CheckCompound( m_sCompoundItem[i].m_nCompoundID ) )
			{
				m_sCompoundItem[i].m_pItemSlotButton->SetRegist( false );
				m_sCompoundItem[i].m_pStaticLevelText->Enable( true );
				m_sCompoundItem[i].m_pStaticLevel->Enable( true );
			}
			else
			{
				m_sCompoundItem[i].m_pItemSlotButton->SetRegist( true );
				m_sCompoundItem[i].m_pStaticLevelText->Enable( false );
				m_sCompoundItem[i].m_pStaticLevel->Enable( false );
			}
		}
	}
}

void CDnItemCompoundTabDlg::PrevPage()
{
	--m_nCurPage;
	if( m_nCurPage < 0 )
		m_nCurPage = 0;

	UpdatePage();
}

void CDnItemCompoundTabDlg::NextPage()
{
	++m_nCurPage;
	if( m_nCurPage > m_nMaxPage )
		m_nCurPage = m_nMaxPage;

	UpdatePage();
}

void CDnItemCompoundTabDlg::CheckCompound()
{
	m_bOK = false;
	m_pButtonOK->Enable( true );
	if( 0 <= m_nCurSelect && m_nCurSelect < NUM_MAX_COMPOUND_ITEM )
	{
		int nCompoundID = m_sCompoundItem[m_nCurSelect].m_nCompoundID;
		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

		// 수수료 보여주기.
		int nGold = (int)(CompoundInfo.iCost/10000);
		int nSilver = (int)((CompoundInfo.iCost%10000)/100);
		int nBronze = (int)(CompoundInfo.iCost%100);
		m_pGold->SetIntToText( nGold );
		m_pSilver->SetIntToText( nSilver );
		m_pBronze->SetIntToText( nBronze );

		// 수수료를 얻어와 확인.
		bool bEnoughCoin = false;
		if( GetItemTask().GetCoin() >= CompoundInfo.iCost )
		{
			bEnoughCoin = true;
			m_pGold->SetTextColor( m_dwColorGold );
			m_pSilver->SetTextColor( m_dwColorSilver );
			m_pBronze->SetTextColor( m_dwColorBronze );
		}
		else
		{
			if( CompoundInfo.iCost >= 10000 ) m_pGold->SetTextColor( 0xFFFF0000, true );
			if( CompoundInfo.iCost >= 100 ) m_pSilver->SetTextColor( 0xFFFF0000, true );
			m_pBronze->SetTextColor( 0xFFFF0000, true );
		}

		bool bNeedItemResult = true;

		// 먼저 설정된 필요아이템 슬롯 초기화.
		for( int i = 0; i < NUM_NEED_ITEM; ++i )
			m_pNeedItemSlotButton[i]->ResetSlot();

		// 서버에 전송할 필요아이템정보 구조체를 설정해둔다.
		m_vNeedItemInfo.clear();
		for( int i = 0; i < NUM_NEED_ITEM; ++i )
		{
			if( CompoundInfo.aiItemID[i] > 0 && CompoundInfo.aiItemCount[i] > 0 )
			{
				CDnItemTask::ItemCompoundInfo Info;
				Info.iItemID = CompoundInfo.aiItemID[ i ];
				Info.iUseCount = CompoundInfo.aiItemCount[ i ];
				m_vNeedItemInfo.push_back(Info);

				// 추가로 인벤토리를 보면서 종류과 갯수 확인.
				int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[i] );

				// 재료템 슬롯 설정.
				SetNeedItemSlot( i, CompoundInfo.aiItemID[i], CompoundInfo.aiItemCount[i], nCurItemCount );

				// 어느 한 종류라도 없으면 false된다.
				if( nCurItemCount < CompoundInfo.aiItemCount[i] )
					bNeedItemResult = false;
			}
		}

		if( bEnoughCoin && bNeedItemResult )
			m_bOK = true;
	}
}

bool CDnItemCompoundTabDlg::CheckCompound( int nCompoundID )
{
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	bool bEnoughCoin = false;
	if( GetItemTask().GetCoin() >= CompoundInfo.iCost )
		bEnoughCoin = true;

	bool bNeedItemResult = true;
	for( int i = 0; i < NUM_NEED_ITEM; ++i )
	{
		if( CompoundInfo.aiItemID[i] > 0 && CompoundInfo.aiItemCount[i] > 0 )
		{
			int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[i] );
			if( nCurItemCount < CompoundInfo.aiItemCount[i] )
				bNeedItemResult = false;
		}
	}

	if( bEnoughCoin && bNeedItemResult )
		return true;

	return false;
}

void CDnItemCompoundTabDlg::SetNeedItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount )
{
	if( nItemID == 0 )
		return;

	SAFE_DELETE( m_pNeedItem[nSlotIndex] );

	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;
	m_pNeedItem[nSlotIndex] = GetItemTask().CreateItem( itemInfo );
	m_pNeedItemSlotButton[nSlotIndex]->SetItem(m_pNeedItem[nSlotIndex], CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_pNeedItemSlotButton[nSlotIndex]->SetJewelCount( nNeedItemCount, nCurItemCount );

	if( nCurItemCount < nNeedItemCount )
		m_pNeedItemSlotButton[nSlotIndex]->SetRegist( true );
	else
		m_pNeedItemSlotButton[nSlotIndex]->SetRegist( false );
}

void CDnItemCompoundTabDlg::ShowConfirmDlg( int nCompoundID, bool bOK )
{
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );
	m_pCompoundMixDlg->SetCompoundInfo( CompoundInfo.aiItemID, CompoundInfo.aiItemCount );
	m_pCompoundMixDlg->Show( true );

	m_pCompoundMixDlg->SetPrice( (int)CompoundInfo.iCost );

	CDnSlotButton *pDragButton = (CDnSlotButton *)drag::GetControl();
	if( pDragButton ) {
		pDragButton->DisableSplitMode(true);
		drag::ReleaseControl();
	}

	int nProb = (int)(CompoundInfo.fSuccessPossibility * 100.0f);
	if( CompoundInfo.fSuccessPossibility * 100.0f - (float)nProb > 0.999 ) nProb += 1;
	m_pCompoundMixDlg->SetProb( nProb );
	m_pCompoundMixDlg->SetOK( bOK );
}

void CDnItemCompoundTabDlg::OnRecvItemCompoundOpen()
{
	// 조합하려는 아이템에 따라 적절한 시간을 구해온 후
	int nCompoundID = m_sCompoundItem[m_nCurSelect].m_nCompoundID;
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	GetInterface().OpenMovieProcessDlg(
		"ItemCombine.avi", CompoundInfo.fTimeRequired / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8019 ), COMPOUND_MOVIE_DIALOG, this );

	m_pCompoundMixDlg->EnableButton( true );
	m_pCompoundMixDlg->Show( false );
}

void CDnItemCompoundTabDlg::OnRecvItemCompound()
{
	// 기본스텝으로 돌아간다.
	m_nCurSelect = -1;
	m_pStaticSelect->Show( false );
	m_pButtonOK->Enable( false );

	for( int i = 0; i < NUM_NEED_ITEM; ++i )
		m_pNeedItemSlotButton[i]->ResetSlot();

	m_pGold->SetIntToText( 0 );
	m_pSilver->SetIntToText( 0 );
	m_pBronze->SetIntToText( 0 );

	// 제작 후 아이템과 돈이 줄었을테니 만들수있는지 다시 확인한다.
	UpdateCompoundableSlot();

	m_bOK = false;
}