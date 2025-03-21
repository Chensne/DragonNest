#include "StdAfx.h"
#include "DnTooltipDlg.h"
#include "DnItemSlotButton.h"
#include "DnSkillSlotButton.h"
#include "DnInterfaceString.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnActor.h"
#include "DnTradeTask.h"
#include "DnTableDB.h"
#include "DnCharStatusDlg.h"
#include "DnStoreTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnInspectPlayerDlg.h"
#include "DnCashShopTask.h"
#include "DnPartyTask.h"
#include "DnPlayerActor.h"
#include "DNGestureTask.h"
#include "DnSkillTask.h"
#include "DnCommonUtil.h"
#include "DnAppellationTask.h"
#include "SecondarySkill.h"
#include "SecondarySkillRepository.h"
#include "DnPvPGameTask.h"
#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnSecondarySkillRecipeItem.h"
#endif // PRE_ADD_SECONDARY_SKILL
#include "DnQuestTask.h"
#include "DnVehicleTask.h"
#include "DnPetTask.h"
#include "DnGuildWarTask.h"
#include "DnGuildTask.h"
#include "DnUIString.h"
#include "DnStoreRepurchaseSystem.h"
#include "SyncTimer.h"

#ifdef PRE_ADD_48682
#include "EtUIXML.h"
#endif

#ifdef PRE_ADD_BESTFRIEND
#include "DnInspectPlateDlg.h"
#endif

#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
#include "DnQuickSlotButton.h"
#endif

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
#include "DnItemCompoundNotifyDlg.h"
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#include "DnBlow.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_MOD_ITEM_COMPOUND_TOOLTIP
#include "DnPotentialJewel.h"
#endif

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemTooltipDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

using namespace DN_INTERFACE::STRING;

CDnTooltipDlg::CDnTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextBox(NULL)
	, m_pTextBox2(NULL)
	, m_pLineVertical2(NULL)
	, m_pTextBox3(NULL)
	, m_pLineVertical3(NULL)
	, m_dwMousePos(MOUSE_DEFAULT)
	, m_pStaticName(NULL)
	, m_pSrcItem(NULL)
	, m_pDestItem(NULL)
	, m_pDestItem2(NULL)
	, m_itemSlotType(ST_ITEM_NONE)
	, m_pStaticBase(NULL)
	, m_pStaticBaseEquip(NULL)
	, m_Type(TOOLTIP_NORMAL)
	, m_pCompareTooltipDlg(NULL)
	, m_pCompareTooltipDlg2(NULL)
	, m_bTooltipSwitch(false)
	, m_pTargetDiffItem(NULL)
	, m_pStaticPointBox(NULL)
	, m_pOverlapCompareItem(NULL)

#ifdef PRE_ADD_CASHREMOVE
	,m_tBegin(0)
	,m_nCashRemoveSerial(0)
#endif // PRE_ADD_CASHREMOVE

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
	,m_nStoreSaleItemPeriod(-1)
#endif // PRE_ADD_COMBINEDSHOP_PERIOD
	
#ifdef PRE_ADD_COMPARETOOLTIP
	,m_bShiftPress(false) // #69087
	,m_nShowRingSlotIndex(-1)
	,m_bTwoRing(false)
#endif // PRE_ADD_COMPARETOOLTIP

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	, m_bSwitchOverlapToolTip(false)
#endif //PRE_ADD_OVERLAP_SETEFFECT

//#ifdef PRE_ADD_MAINQUEST_UI
//	, m_bShowMainQuestToolTip(false)
//#endif

#ifdef PRE_ADD_DRAGON_GEM
	, m_pDragonGemTooltipDlg(NULL)
#endif
{
	for( int i = 0; i < LINE_MAX; ++i )
		m_pStaticLine[i] = NULL;
	m_SrcItemType = MIInventoryItem::Item;
	m_fUpdateOrigPosX = 0.0f;
	m_fUpdateOrigPosY = 0.0f;
	m_nCurLineIndex = 0;
}

CDnTooltipDlg::~CDnTooltipDlg(void)
{
#ifdef PRE_ADD_48682
	m_vAllowMapBitFlag.clear(); // EAllowMapTypes 값 배열. 
	m_mapStrMapType.clear(); // EAllowMapTypes 와 매칭되는 텍스트 배열.
#endif

#ifdef PRE_ADD_DRAGON_GEM
	SAFE_DELETE(m_pDragonGemTooltipDlg);
#endif

	if( GetDialogType() == UI_TYPE_TOP_MSG || GetDialogType() == UI_TYPE_BOTTOM_MSG )
	{
		// 구조가 Tooltip - compare - compare2 이므로, 마지막부터 지워줘야한다.
		SAFE_DELETE( m_pCompareTooltipDlg2 );
		SAFE_DELETE( m_pCompareTooltipDlg );
	}
	SAFE_RELEASE_SPTR( m_hSlotButton );
}

void CDnTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "TooltipDlg.ui" ).c_str(), bShow );
}

void CDnTooltipDlg::InitialUpdate()
{
	m_pTextBox  = GetControl<CEtUITextBox>("ID_TEXTBOX");
	m_pTextBox2 = GetControl<CEtUITextBox>("ID_TEXTBOX2");
	m_pTextBox3 = GetControl<CEtUITextBox>("ID_TEXTBOX3");
	m_pLineVertical2 = GetControl<CEtUIStatic>("ID_LINE_VERTICAL2");
	m_pLineVertical3 = GetControl<CEtUIStatic>("ID_LINE_VERTICAL3");
	m_pStaticName = GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pStaticBase = GetControl<CEtUIStatic>("ID_STATIC_BASE");
	m_pStaticBaseEquip = GetControl<CEtUIStatic>("ID_STATIC_EQUIP");
	m_pStaticBaseEquip->Show(false);
	m_pTextBox->GetUICoord( m_TextBoxDefaultCoord );

	GetDlgCoord( m_uiDefaultCoord );
	// 툴팁이 더 길어져서 2페이지 하단을 넘는다면 3페이지용 컨트롤을 만들어서 연결해주면 된다.
	m_pTextBox->SetNextPageTextBox( m_pTextBox2, CEtUITextBox::NextPage_DlgScreenHeight, 2 );
	m_pLineVertical2->Show( false );
	m_pTextBox2->SetNextPageTextBox( m_pTextBox3, CEtUITextBox::NextPage_DlgScreenHeight, 2 );
	m_pLineVertical3->Show( false );

	char szName[32];
	for( int i = 0; i < LINE_MAX; ++i )
	{
		sprintf_s(szName, _countof(szName), "%s%d", "ID_LINE", i);
		m_pStaticLine[i] = GetControl<CEtUIStatic>(szName);
		m_pStaticLine[i]->Show( false );
	}
	m_pStaticLine[0]->GetUICoord( m_LineBaseCoord );

	if (GetDialogType() != UI_TYPE_CHILD)
	{
		if( m_pCompareTooltipDlg == NULL)
		{
			m_pCompareTooltipDlg = new CDnTooltipDlg( UI_TYPE_CHILD, this );
			m_pCompareTooltipDlg->Initialize( false );
		}

		if( !m_pCompareTooltipDlg2 )
		{
			m_pCompareTooltipDlg2 = new CDnTooltipDlg( UI_TYPE_CHILD, m_pCompareTooltipDlg );
			m_pCompareTooltipDlg2->Initialize( false );
		}
	}

	m_pStaticPointBox = GetControl<CEtUIStatic>("ID_STATIC_POINTBOX");
	m_pStaticPointBox->ForceBlend();


#ifdef PRE_ADD_48682
	// EAllowMapTypes 와 매칭되는 텍스트 배열설정.	
	m_vAllowMapBitFlag.reserve( EAllowMapTypesSIZE ); // EAllowMapTypes 비트값 배열.
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_None        ); 
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_Village     );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_WorldMap    );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_Dungeon     );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_PVP         );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_DarkLair    );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_Nest        );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_SDragonNest );	
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_Guild       );
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_NestHell    );	
	m_vAllowMapBitFlag.push_back( (UINT)EAllowMapTypes::EAllowMapTypes_GDragonNest );
		
	// 텍스트 Index :                       '',   마을,월드맵,던전,   PVP,       다크레어, 네스트, 씨드래곤네스트, 길드전,   네스트헬, 지드래곤네스트 
	int arrStrIdx[ EAllowMapTypesSIZE ] = { 9102, 1212, 1304, 7110, 1000040965, 1000003489, 1372,   1000018496,  1000021681, 1000040966,  10046 }; 

	// "[사용가능지역]"	
	m_strAllowZone.append( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2214 ) ); 	

	// Key : BitFlag,  Value : 맵타입 텍스트.
	for( int i=0; i<EAllowMapTypesSIZE; ++i )
	{
		m_mapStrMapType.insert( std::map< UINT, std::wstring >::value_type( m_vAllowMapBitFlag[i], 
			std::wstring( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, arrStrIdx[ i ] ) ) ) );
	}
#endif

#ifdef PRE_ADD_DRAGON_GEM
	m_pDragonGemTooltipDlg = new CDnDragonGemTooltipDlg(UI_TYPE_CHILD, this);
	m_pDragonGemTooltipDlg->Initialize(false);
#endif
}

void CDnTooltipDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if (m_hSlotButton != NULL)
		{
			if( m_hSlotButton->IsDragMode() )
			{
				HideTooltip();
				return;
			}

			if( !m_hSlotButton->IsMouseEnter() )
			{
//#ifdef PRE_ADD_MAINQUEST_UI
//				if(m_bShowMainQuestToolTip == false)
//					HideTooltip();
//#else
//				HideTooltip();
//#endif
				HideTooltip();
				
				return;
			}

			CEtUIDialog *pDlg = m_hSlotButton->GetParent();
			if( pDlg && !pDlg->IsShow() )
			{
				// 확장형 리스트박스에 들어가는 엘리먼트 아이템 다이얼로그의 경우엔 Hide하지 않는다. 였는데,
				// 확장형 리스트박스 아이템 다이얼로그를 UI_TYPE_SELF로 생성하면서
				// 이렇게 예외처리 하지 않아도 툴팁이 안없어지게 되었지만,
				// 스킬 컨텐츠 다이얼로그의 경우 UI_TYPE_CHILD로 생성하는거라 아직 이 처리가 필요하다. 그래서 남겨둔다.
				if( pDlg->IsElementDialog() ) return;

				HideTooltip();
				return;
			}
		}
		else
		{
			if (m_Type == TOOLTIP_NAMELINK || m_Type == TOOLTIP_MANUALCONTROL)
				return;

			if( GetDialogType() != UI_TYPE_CHILD ) {
				HideTooltip();
				return;
			}
		}

		// 시간이 흐를때마다 툴팁에 바로 반영
		if( m_pSrcItem )
		{
			if( MIInventoryItem::Item == m_pSrcItem->GetType() )
			{
				// 장착한 상태에서 툴팁을 볼 때만.
				if( ST_CHARSTATUS == m_itemSlotType )
				{
					CDnItem* pItem = static_cast<CDnItem*>(m_pSrcItem);
					if( ( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) )
					{
						SetTooltipText( m_pSrcItem, false, false );
					}
				}

#ifdef PRE_ADD_CASHREMOVE 

				// 캐시제거대기아이템의 대기시간출력.
				else if( ST_INVENTORY_CASHREMOVE == m_itemSlotType )
				{
					CDnItem* pItem = static_cast<CDnItem*>(m_pSrcItem);

					// 해당아이템이 인벤으로 들어온 시간.
					if( pItem->GetSerialID() != m_nCashRemoveSerial )
					{
						m_nCashRemoveSerial = pItem->GetSerialID();
						m_tBegin = GetItemTask().GetTimeBySN( m_nCashRemoveSerial );						
					}

					if( pItem->IsCashRemoveItem() && pItem->GetExpireTime() > 0 )
						SetTooltipText( m_pSrcItem, false, false );
				}
#endif
			}
		}
	}
}

bool CDnTooltipDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( m_bShow == false )
		return false;

#ifdef PRE_ADD_COMPARETOOLTIP
	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			PressShift( ( GetKeyState( VK_LSHIFT) & 0x8000 ) != 0 );
		}
		break;

	case WM_KEYUP :
		{
			PressShift( false );
		}
		break;
	}
#endif

	if( m_Type != TOOLTIP_NAMELINK )
	{
		// #31481 스킬 레벨 업 시에 기존 스킬의 툴팁이 떠 있는 경우 안전 코드 추가. (서버 렉이 걸린 경우)
		if( m_hSlotButton && m_hSlotButton->GetItem() == m_pSrcItem )
		{
			if( CDnSkillTask::IsActive() && m_pSrcItem && GetSkillTask().IsGuildWarSkill( m_pSrcItem->GetClassID() ) )
				return false;
			if( m_Type == TOOLTIP_NORMAL && uMsg == WM_MOUSEWHEEL && m_pSrcItem && m_pSrcItem->GetType() == MIInventoryItem::Skill )
			{
				m_bTooltipSwitch ^= true;
				ClearText();
				ClearDivisionLine();
				SetSkillText( m_pSrcItem );
				UpdateTooltip( m_fUpdateOrigPosX, m_fUpdateOrigPosY );
				return true;
			}

#ifdef PRE_ADD_PREFIXSKILL_PVP
			if( ((m_hSlotButton->GetSlotType() == ST_CHARSTATUS && (uMsg == WM_MOUSEWHEEL || uMsg == WM_MBUTTONDOWN)) || (m_hSlotButton->GetSlotType() == ST_INVENTORY && uMsg == WM_MBUTTONDOWN) ) 
				&& m_Type == TOOLTIP_NORMAL && m_pSrcItem && m_pSrcItem->GetType() == MIInventoryItem::Item )
			{
				CDnItem *pItem = static_cast<CDnItem*>(m_pSrcItem);
				if( pItem->HasPrefixSkill() )
				{
					m_bTooltipSwitch ^= true;
					ClearText();
					ClearDivisionLine();
					SetItemText( m_pSrcItem , false );

#ifdef PRE_FIX_CREATE_ITEM_TOOLTIP_POS
					if( !GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_DIALOG) || 
						!GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG) )
					{
						UpdateTooltip( m_fUpdateOrigPosX, m_fUpdateOrigPosY );
					}
#else
					UpdateTooltip( m_fUpdateOrigPosX, m_fUpdateOrigPosY );
#endif

					if( m_pDestItem && m_pDestItem->GetType() == MIInventoryItem::Item )
					{
						CDnItem *pDestItem = static_cast<CDnItem*>(m_pDestItem);

						if( pDestItem->HasPrefixSkill() )
						{
							if( m_pCompareTooltipDlg && m_pCompareTooltipDlg->IsShow() )
							{
								int nItemType = pDestItem->GetItemType();
								int nSlotIndex =  pDestItem->GetSlotIndex();

								ShowCompareTooltip(false , (eItemTypeEnum)nItemType , nSlotIndex , m_pDestItem , m_pCompareTooltipDlg );
							}
						}
					}
				}

#ifndef _FINAL_BUILD
				if( uMsg == WM_MBUTTONDOWN )
				{
					WCHAR wszID[256] = {0,};
					swprintf_s( wszID, L"%d", pItem->GetClassID() );
					CEtUILineEditBox::CopyStringToClipboard( wszID );
				}
#endif
				return true;
			}
#endif

#ifndef _FINAL_BUILD
			if( m_Type == TOOLTIP_NORMAL && m_pSrcItem && m_pSrcItem->GetType() == MIInventoryItem::Item )
			{
				if( uMsg == WM_MBUTTONDOWN )
				{
					WCHAR wszID[256] = {0,};
					CDnItem *pItem = static_cast<CDnItem*>(m_pSrcItem);
					swprintf_s( wszID, L"%d", pItem->GetClassID() );
					CEtUILineEditBox::CopyStringToClipboard( wszID );
					return true;
				}
				else if( uMsg == WM_RBUTTONDOWN )
				{
					if( GetAsyncKeyState( VK_RSHIFT ) )
					{
						WCHAR wszSerial[256] = {0,};
						CDnItem *pItem = static_cast<CDnItem*>(m_pSrcItem);
						INT64 biSerial = pItem->GetSerialID();
						swprintf_s( wszSerial, L"%I64d", biSerial );
						CEtUILineEditBox::CopyStringToClipboard( wszSerial );
						return true;
					}
				}
			}
#endif
		}
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		{
			HideTooltip();
		}
		break;

	case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
				HideTooltip();
		}
		break;

	}

	return false;
}

void CDnTooltipDlg::UpdateTooltip( float fX, float fY )
{
	UpdateTooltipNextPage();

	m_fUpdateOrigPosX = fX;
	m_fUpdateOrigPosY = fY;

	static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH), fYBGap(4.0f/DEFAULT_UI_SCREEN_HEIGHT);
	SUICoord sDlgCoord;
	CEtUIDialog *pDialog;

	float fParentX, fParentY;
	fParentX = fParentY = 0.f;

	GetDlgCoord( sDlgCoord );
	if (m_hSlotButton != NULL)
	{
		pDialog = m_hSlotButton->GetParent();
		fParentX = pDialog->GetXCoord();
		fParentY = pDialog->GetYCoord();
	}
	sDlgCoord.fX = fX + fParentX;
	sDlgCoord.fY = fY + fParentY;

	// 기본은 커서 우하단에 뜨는 건데,
	// 만약 우측을 넘어서거나, 하단을 넘어가면 좌측 혹은 상단으로 올리는 코드다.
	// 그런데 생각해보면, 텍스트가 길어서 커서상단으로 올린 후 또 다시 화면의 위를 넘어가는지 체크 후 내릴거면,
	// 커서 상단으로 넘길 필요가 있나하는 생각이 든다.(좌측으로 넘기는건 그대로 하겠다.)
	//
	// 툴팁 위치에 대한 수정 내역이 사라지면 아래 주석도 지우겠다.

	if( (sDlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
	{
		m_dwMousePos ^= MOUSE_RIGHT;
		m_dwMousePos |= MOUSE_LEFT;
	}

	//if( (sDlgCoord.Bottom()+fYBGap) > GetScreenHeightRatio() )
	//{
	//	m_dwMousePos ^= MOUSE_BOTTOM;
	//	m_dwMousePos |= MOUSE_TOP;
	//}

	if( m_dwMousePos & MOUSE_LEFT )
	{
		sDlgCoord.fX -= sDlgCoord.fWidth;
		//sDlgCoord.fX -= fXLGap;
	}
	else if( m_dwMousePos & MOUSE_RIGHT )
	{
		//sDlgCoord.fX += fXRGap;
	}

	//if( m_dwMousePos & MOUSE_TOP )
	//{
	//	sDlgCoord.fY -= sDlgCoord.fHeight;
	//	//sDlgCoord.fY -= fYTGap;
	//}
	//else if( m_dwMousePos & MOUSE_BOTTOM )
	//{
	//	//sDlgCoord.fY += fYBGap;
	//}

	// 우하단 밖으로 나가지 않도록.
	if( (sDlgCoord.Bottom()+fYBGap) > GetScreenHeightRatio() )
	{
		sDlgCoord.fY -= (sDlgCoord.Bottom()+fYBGap - GetScreenHeightRatio());
	}
	if( (sDlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
	{
		sDlgCoord.fX -= (sDlgCoord.Right()+fXRGap - GetScreenWidthRatio());
	}

	SetDlgCoord( sDlgCoord );

	m_dwMousePos = MOUSE_DEFAULT;
}

void CDnTooltipDlg::UpdateTooltipNextPage()
{
	SUICoord sDlgCoord;
	GetDlgCoord( sDlgCoord );

	m_pLineVertical2->Show( false );
	m_pLineVertical3->Show( false );

	if( m_pTextBox->GetUsedNextPage() == 0 )
	{
		sDlgCoord.fWidth = m_uiDefaultCoord.fWidth;
	}
	if( m_pTextBox->GetUsedNextPage() >= 1 )
	{
		m_pLineVertical2->SetSize( m_pLineVertical2->GetUICoord().fWidth, sDlgCoord.fHeight - 40.0f/DEFAULT_UI_SCREEN_HEIGHT );
		m_pLineVertical2->Show( true );
		sDlgCoord.fWidth = m_uiDefaultCoord.fWidth*1.9f;
	}
	if( m_pTextBox->GetUsedNextPage() >= 2 )
	{
		m_pLineVertical3->SetSize( m_pLineVertical3->GetUICoord().fWidth, sDlgCoord.fHeight - 40.0f/DEFAULT_UI_SCREEN_HEIGHT );
		m_pLineVertical3->Show( true );
		sDlgCoord.fWidth = m_uiDefaultCoord.fWidth*2.9f;
	}

	SetDlgCoord( sDlgCoord );

	m_pStaticBase->SetSize( sDlgCoord.fWidth, sDlgCoord.fHeight );
}

void CDnTooltipDlg::ShowTooltip(CDnItem* pSrcItem, MIInventoryItem::InvenItemTypeEnum itemType, ITEM_SLOT_TYPE slotType, float fX, float fY, bool bNameLink, bool bAutoCompare, CDnItem *pTargetDiffItem)
{
	if( !CDnActor::s_hLocalActor || pSrcItem == NULL ) return;

	m_SrcItemType = itemType;
	m_pSrcItem = pSrcItem;

	switch( m_SrcItemType ) {
		case MIInventoryItem::Item:
		case MIInventoryItem::Skill:
			if( !m_pSrcItem ) return;
			if( !m_pSrcItem->IsEnableTooltip() ) return;
			break;
	}

	m_itemSlotType = slotType;

	m_pDestItem = NULL;
	m_pDestItem2 = NULL;
	m_pOverlapCompareItem = NULL;

	HideCompareTooltip( m_pCompareTooltipDlg );
	HideCompareTooltip( m_pCompareTooltipDlg2 );
	SetTargetDiffItem( pTargetDiffItem );

	switch( m_SrcItemType ) {
		case MIInventoryItem::Item:
		case MIInventoryItem::Skill:
			SetTooltipText( pSrcItem, ( pTargetDiffItem ) ? true : false, bNameLink );
			break;
		case MIInventoryItem::Gesture:
			// DnGestureButton의 경우엔 MIInventoryItem* m_pItem 멤버가 항상 NULL이다.
			// DnSkill, DnItem과 달리 제스처는 따로 객체형태로 표현되지 않기 때문.
			// 그래서 이렇게 DnSlotButton의 m_nItemID를 얻어다가 설정해야한다.
			ClearText();
			ClearDivisionLine();
			SetGestureText(m_SrcItemType);
			break;
		case MIInventoryItem::SecondarySkill:
			ClearText();
			ClearDivisionLine();
			SetLifeSkillText(m_SrcItemType);
			break;
	}

	UpdateTooltip( fX, fY );

	Show( true );

#ifdef PRE_ADD_DRAGON_GEM
	if(pSrcItem && m_pDragonGemTooltipDlg)
	{
		if( m_SrcItemType == MIInventoryItem::InvenItemTypeEnum::Item && pSrcItem->IsDragonGemApplicable())
		{
			m_pDragonGemTooltipDlg->ShowDragonGemToolTip(pSrcItem);
		}
	}
#endif

	if( slotType == ST_ITEM_NONE )	return;
	if( slotType == ST_QUICKSLOT)	return;
	if( slotType == ST_CHARSTATUS ) return;
	if( slotType == ST_SKILL)		return;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	if( slotType == ST_ITEM_NOTIFY_COMPOUND ) return;
#endif
#ifdef PRE_ADD_TALISMAN_SYSTEM
	if( slotType == ST_TALISMAN )   return;
#endif


	// Note : 이 아이템이 인벤토리에 있고 만약 현재 장착되어 있는 장비랑 비교할 수 있다면 비교 창을 띄운다.
	//
	if( bAutoCompare && pSrcItem && (pSrcItem->GetType() == MIInventoryItem::Item) )
	{
		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
		std::vector<int> nVecJobList;
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if( pSrcItem->IsNeedJob() == false || pSrcItem->IsPermitPlayer( nVecJobList ) )
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if( pCharStatusDlg )
			{
				eItemTypeEnum eType = pSrcItem->GetItemType();
				if (pSrcItem->IsCashItem())
				{
					int nSlotIndex = pCharStatusDlg->GetCashEquipIndex( pSrcItem );
					if( nSlotIndex >= 0 )
					{
#ifdef PRE_ADD_COMPARETOOLTIP
						m_bTwoRing = false;
						if( nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2 )
						{						
							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING1 ) != NULL )
								nRing = 1;
							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING2 ) != NULL )
								nRing = (nRing == 1 ? 3 : 2);


							// #69087		
							// 둘다착용시 Alt 로 변경.
							int _slotIdx = CASHEQUIP_RING1;							
							if( nRing == 3 )
							{
								m_bTwoRing = true;
								_slotIdx = m_bShiftPress ? CASHEQUIP_RING2 : CASHEQUIP_RING1;
							}
							else if( nRing == 2 )
								_slotIdx = CASHEQUIP_RING2;
							m_nShowRingSlotIndex = _slotIdx;
							ShowCompareTooltip( true, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == CASHEQUIP_RING2 ? CDnParts::PartsTypeEnum::CashRing2 : -1 ), bNameLink);
						}
						else
						{
							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg, -1, bNameLink);
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}

#else
						// 마우스 오버한 아이템이 반지면,
						if( nSlotIndex == CASHEQUIP_RING1 )
						{	
							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
							if( ShowCompareTooltip( true, eType, CASHEQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg, bNameLink) )
							{
								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2, bNameLink);
							}
							else
							{
								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg, bNameLink );
								HideCompareTooltip( m_pCompareTooltipDlg2 );
							}
						}
						else
						{
							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg, bNameLink );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#endif // PRE_ADD_COMPARETOOLTIP

					}
				}
				else
				{
					int nSlotIndex = pCharStatusDlg->GetEquipIndex( pSrcItem );
					if( nSlotIndex >= 0 )
					{
#ifdef PRE_ADD_COMPARETOOLTIP
						m_bTwoRing = false;
						//if( ( nSlotIndex == EQUIP_RING1 && eType != ITEMTYPE_GLYPH ) || nSlotIndex == EQUIP_RING2 )
						if( eType != ITEMTYPE_GLYPH && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ) )
						{
							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
							if( pCharStatusDlg->GetEquipItem( EQUIP_RING1 ) != NULL )
								nRing = 1;
							if( pCharStatusDlg->GetEquipItem( EQUIP_RING2 ) != NULL )
								nRing = (nRing == 1 ? 3 : 2);

							// #69087		
							// 둘다착용시 Alt 로 변경.
							int _slotIdx = EQUIP_RING1;							
							if( nRing == 3 )
							{
								_slotIdx = m_bShiftPress ? EQUIP_RING2 : EQUIP_RING1;
								m_bTwoRing = true;
							}
							else if( nRing == 2 )
								_slotIdx = EQUIP_RING2;
							m_nShowRingSlotIndex = _slotIdx;
							ShowCompareTooltip( false, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == EQUIP_RING2 ? CDnParts::PartsTypeEnum::Ring2 : -1 ), bNameLink);
						}
						else
						{
							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg, -1, bNameLink);
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#else

						// 마우스 오버한 아이템이 반지면,
						//if( nSlotIndex == EQUIP_RING1 )
						if( nSlotIndex == EQUIP_RING1 && 
							eType != ITEMTYPE_GLYPH ) // #48430 아래코드는 EQUIP_RING2(10) 툴팁을 찍게되어있는데 
																	// 10번은 문장에서 '의지' 에 해당한다. 따라서 '의지' 문장의 툴팁이
																	// 무조건 출력되어 [문장] 의 경우 예외처리 한다.
						{							
							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
							if( ShowCompareTooltip( false, eType, EQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg, bNameLink ) )
							{
								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2, bNameLink);
							}
							else
							{
								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg, bNameLink );
								HideCompareTooltip( m_pCompareTooltipDlg2 );
							}
						}
						else
						{
							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg, bNameLink );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#endif // PRE_ADD_COMPARETOOLTIP

					}
				}
			}
		}
	}

	if (m_Type == TOOLTIP_NAMELINK)
	{
		SUICoord currentCoord, compare1, compare2;
		GetDlgCoord(currentCoord);
		if (m_pCompareTooltipDlg->IsShow())
			m_pCompareTooltipDlg->GetDlgCoord(compare1);
		if (m_pCompareTooltipDlg2->IsShow())
			m_pCompareTooltipDlg2->GetDlgCoord(compare2);

		// align center
		float curWholeWidth = currentCoord.fWidth + compare1.fWidth + compare2.fWidth;
		float temp = min(currentCoord.fX, ((compare1.fX != 0.f) ? compare1.fX : currentCoord.fX));
		float curMostLeft = min(temp, ((compare2.fX != 0.f) ? compare2.fX : temp));
		float curCenter = curMostLeft + (curWholeWidth * 0.5f);
		float gapBetweenCenters = fX - curCenter;
		currentCoord.fX += gapBetweenCenters;
		compare1.fX += gapBetweenCenters;
		compare2.fX += gapBetweenCenters;

		// align bottom - fY is bottom
		temp = max(currentCoord.fHeight, ((compare1.fHeight != 0.f) ? compare1.fHeight : currentCoord.fHeight));
		float curMaxHeight = max(temp, (compare2.fHeight != 0.f) ? compare2.fHeight : currentCoord.fHeight) ;
		if (curMaxHeight > fY)
			fY = curMaxHeight;
		//float curBottomY = currentCoord.fY + curMaxHeight;
		float newYPos = fY - curMaxHeight;
		currentCoord.fY = compare1.fY = compare2.fY = newYPos;

		SetDlgCoord(currentCoord);
		if (m_pCompareTooltipDlg->IsShow())
			m_pCompareTooltipDlg->SetDlgCoord(compare1);
		if (m_pCompareTooltipDlg2->IsShow())
			m_pCompareTooltipDlg2->SetDlgCoord(compare2);

		// 네임링크 툴팁이랑 살펴보기랑 위치가 겹쳐서 하이드 시킨다.
		GetInterface().CloseInspectPlayerInfoDialog();
	}
}

void CDnTooltipDlg::SetTargetDiffItem( CDnItem* pItem )
{
	m_pTargetDiffItem = pItem;
}

void CDnTooltipDlg::ShowTooltip( CDnSlotButton *pControl, float fX, float fY, bool bAutoCompare, CDnItem *pTargetDiffItem )
{
	ASSERT(pControl);
	if(!pControl) return;

#ifdef PRE_ADD_COMPARETOOLTIP
	m_ringCompare.SetData( fX, fY, bAutoCompare );	
#endif

	// ComeBack - 로그인창에서 툴팁표시.	
	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
		if( !CDnActor::s_hLocalActor ) return;
	// if( !CDnActor::s_hLocalActor ) return;
	m_hSlotButton = pControl->GetMySmartPtr();
	m_pSrcItem = m_hSlotButton->GetItem();

	switch( pControl->GetItemType() ) {
		case MIInventoryItem::Item:
		case MIInventoryItem::Skill:
			if( !m_pSrcItem ) return;
			if( !m_pSrcItem->IsEnableTooltip() ) return;
			break;
	}

	m_itemSlotType = m_hSlotButton->GetSlotType();

	m_pDestItem = NULL;
	m_pDestItem2 = NULL;
	m_pOverlapCompareItem = NULL;

	HideCompareTooltip( m_pCompareTooltipDlg );
	HideCompareTooltip( m_pCompareTooltipDlg2 );
	SetTargetDiffItem( pTargetDiffItem );

	switch( pControl->GetItemType() ) {
		case MIInventoryItem::Item:
		case MIInventoryItem::Skill:
			SetTooltipText( m_pSrcItem, ( pTargetDiffItem ) ? true : false );
			break;
		case MIInventoryItem::Gesture:
			// DnGestureButton의 경우엔 MIInventoryItem* m_pItem 멤버가 항상 NULL이다.
			// DnSkill, DnItem과 달리 제스처는 따로 객체형태로 표현되지 않기 때문.
			// 그래서 이렇게 DnSlotButton의 m_nItemID를 얻어다가 설정해야한다.
			ClearText();
			ClearDivisionLine();
			SetGestureText( pControl->GetItemID() );
			break;
		case MIInventoryItem::SecondarySkill:
			ClearText();
			ClearDivisionLine();
			SetLifeSkillText( pControl->GetItemID() );
			break;
	}
	
	UpdateTooltip( fX, fY );

	Show( true );

#ifdef PRE_ADD_DRAGON_GEM
	CDnItem* pTempItem = static_cast<CDnItem*>(pControl->GetItem());
	if(pTempItem && m_pDragonGemTooltipDlg) // 테스트용으로 탈리스만으로 나오게 했음
	{
		if( pControl->GetItemType() == MIInventoryItem::Item && pTempItem->IsDragonGemApplicable() )
		{
			m_pDragonGemTooltipDlg->ShowDragonGemToolTip(pTempItem);
		}
	}
#endif

	// 비교 툴팁이 나오면 안되는것들
	if( m_hSlotButton->GetSlotType() == ST_ITEM_NONE )	return;
	if( m_hSlotButton->GetSlotType() == ST_QUICKSLOT)	return;
	if( m_hSlotButton->GetSlotType() == ST_CHARSTATUS )	return;
	if( m_hSlotButton->GetSlotType() == ST_SKILL)		return;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	if( m_hSlotButton->GetSlotType() == ST_ITEM_NOTIFY_COMPOUND ) return;
#endif
#ifdef PRE_ADD_TALISMAN_SYSTEM
	if( m_hSlotButton->GetSlotType() == ST_TALISMAN )   return;
#endif

	// Note : 이 아이템이 인벤토리에 있고 만약 현재 장착되어 있는 장비랑 비교할 수 있다면 비교 창을 띄운다.
	//
	MIInventoryItem* pItem = m_hSlotButton->GetItem();
	CDnItem* pSrcItem = (CDnItem *)m_hSlotButton->GetItem();
	if( bAutoCompare && pItem && (pItem->GetType() == MIInventoryItem::Item) )
	{
		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
		std::vector<int> nVecJobList;
		if( CDnActor::s_hLocalActor )
			((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		// ((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if( ((CDnItem*)pItem)->IsNeedJob() == false || ((CDnItem*)pItem)->IsPermitPlayer( nVecJobList ) )
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if( pCharStatusDlg )
			{
				int nSlotIndex = (((CDnItem*)pItem)->IsCashItem()) ? pCharStatusDlg->GetCashEquipIndex( (CDnItem*)pItem ) : pCharStatusDlg->GetEquipIndex( (CDnItem*)pItem );
				if( nSlotIndex >= 0 )
				{
					eItemTypeEnum eType = pSrcItem->GetItemType();
					if (((CDnItem*)pItem)->IsCashItem())
					{
#ifdef PRE_ADD_COMPARETOOLTIP
						m_bTwoRing = false;
						if( nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2 )
						{
							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING1 ) != NULL )
								nRing = 1;
							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING2 ) != NULL )
								nRing = (nRing == 1 ? 3 : 2);


							// #69087		
							// 둘다착용시 Alt 로 변경.
							int _slotIdx = CASHEQUIP_RING1;							
							if( nRing == 3 )
							{
								_slotIdx = m_bShiftPress ? CASHEQUIP_RING2 : CASHEQUIP_RING1;
								m_bTwoRing = true;
							}
							else if( nRing == 2 )
								_slotIdx = CASHEQUIP_RING2;
							m_nShowRingSlotIndex = _slotIdx;
							ShowCompareTooltip( true, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == CASHEQUIP_RING2 ? CDnParts::PartsTypeEnum::CashRing2 : -1 )  );
						}
						else
						{
							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}

#else
						if( nSlotIndex == CASHEQUIP_RING1 )
						{	
							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
							if( ShowCompareTooltip( true, eType, CASHEQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
							{
								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
							}
							else
							{
								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
								HideCompareTooltip( m_pCompareTooltipDlg2 );
							}							
						}
						else
						{
							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#endif // PRE_ADD_COMPARETOOLTIP

					}
					else
					{
#ifdef PRE_ADD_COMPARETOOLTIP
						m_bTwoRing = false;
						//if( ( nSlotIndex == EQUIP_RING1 && eType != ITEMTYPE_GLYPH ) || nSlotIndex == EQUIP_RING2 )
						if( eType != ITEMTYPE_GLYPH && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ) )
						{
							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
							if( pCharStatusDlg->GetEquipItem( EQUIP_RING1 ) != NULL )
								nRing = 1;
							if( pCharStatusDlg->GetEquipItem( EQUIP_RING2 ) != NULL )
								nRing = (nRing == 1 ? 3 : 2);

							// #69087		
							// 둘다착용시 Alt 로 변경.
							int _slotIdx = EQUIP_RING1;							
							if( nRing == 3 )
							{
								_slotIdx = m_bShiftPress ? EQUIP_RING2 : EQUIP_RING1;
								m_bTwoRing = true;
							}
							else if( nRing == 2 )
								_slotIdx = EQUIP_RING2;
							m_nShowRingSlotIndex = _slotIdx;
							ShowCompareTooltip( false, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == EQUIP_RING2 ? CDnParts::PartsTypeEnum::Ring2 : -1 )  );
						}
						else
						{
							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#else

						//if( nSlotIndex == EQUIP_RING1 )
						if( nSlotIndex == EQUIP_RING1 && 
							eType != ITEMTYPE_GLYPH ) // #48430 아래코드는 EQUIP_RING2(10) 툴팁을 찍게되어있는데 
							// 10번은 문장에서 '의지' 에 해당한다. 따라서 '의지' 문장의 툴팁이
							// 무조건 출력되어 [문장] 의 경우 예외처리 한다.
						{
							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
							if( ShowCompareTooltip( false, eType, EQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
							{
								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
							}
							else
							{
								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
								HideCompareTooltip( m_pCompareTooltipDlg2 );
							}							
						}
						else
						{
							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
#endif // PRE_ADD_COMPARETOOLTIP
					}

#if 0
					// 마우스 오버한 아이템이 반지면,
					if( nSlotIndex == EQUIP_RING1 )
					{
						// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
						if( ShowCompareTooltip( true, EQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
						{
							// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
							ShowCompareTooltip( true, EQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
						}
						else
						{
							// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
							ShowCompareTooltip( true, EQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
							HideCompareTooltip( m_pCompareTooltipDlg2 );
						}
					}
					else
					{
						ShowCompareTooltip( true, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
						HideCompareTooltip( m_pCompareTooltipDlg2 );
					}
#endif // 0

					return;
				}
			}
		}
	}
}

void CDnTooltipDlg::OnChangeTooltip()
{
	if( !m_pSrcItem )
		return;

	SetTooltipText( m_pSrcItem, false );

	if( m_pDestItem )
	{
		m_pCompareTooltipDlg->SetTooltipText( m_pDestItem, true );
	}

	if( m_pDestItem2 )
	{
		m_pCompareTooltipDlg2->SetTooltipText( m_pDestItem2, true );
	}
}

void CDnTooltipDlg::RefreshTooltip( CEtUIControl *pControl )
{
	CDnSlotButton *pButton = static_cast<CDnSlotButton*>(pControl);
	if( !pButton->GetItem() ) return;

	SetTooltipText( pButton->GetItem(), false );
}

#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
void CDnTooltipDlg::RefreshTooltip( CDnSkill *pSkill )
{
	if(pSkill)
	{
		SetTooltipText( pSkill, false );
	}
}
#endif

void CDnTooltipDlg::HideTooltip()
{
	ShowCompareLight( false );
	Show( false );
	m_hSlotButton.Identity();
	ClearDivisionLine();
	ResetToolTipSwitch();
#ifdef PRE_ADD_DRAGON_GEM
	if(m_pDragonGemTooltipDlg) m_pDragonGemTooltipDlg->CloseDragonGemToolTip();
#endif
}

void CDnTooltipDlg::HideCompareTooltip( CDnTooltipDlg *pDialog )
{
	if( pDialog )
	{
		CEtUIDialog *pParentDlg = pDialog->GetParentDialog();
		if( pParentDlg ) pParentDlg->ShowChildDialog( pDialog, false );
	}
}

void CDnTooltipDlg::Show( bool bShow )
{ 
	// Note : 툴팁은 포커스가 필요없기 때문에 Dialog의 Show를 재정의 했다.
	//
	m_bShow = bShow;

	if( bShow )
	{
		if( !m_bLoadedTexture ) LoadDialogTexture();
		//SetFadeMode(true); // Note : 이건 아닌데...-_-;;
		SetFadeIn();
	}
	else
	{
		SetFadeOut();

		if( GetDialogType() == UI_TYPE_TOP ||  GetDialogType() == UI_TYPE_TOP_MSG )
		{
			HideCompareTooltip( m_pCompareTooltipDlg );
			HideCompareTooltip( m_pCompareTooltipDlg2 );
		}

		ClearText();

#ifdef PRE_FIX_SOURCEITEM_TOOLTIP
		m_pSrcItem = NULL;
		m_pDestItem = NULL;
		m_pDestItem2 = NULL;
#endif

#ifdef PRE_ADD_OVERLAP_SETEFFECT
		m_bSwitchOverlapToolTip = false;
#endif
	}

	if( m_bShow != bShow )
		ResetToolTipSwitch();

	m_pStaticName->Show(bShow);
	m_pTextBox->Show(bShow);
}

void CDnTooltipDlg::ClearText()
{
	m_pStaticName->ClearText();
	m_pTextBox->ClearText();
	m_pStaticPointBox->Show( false );
}

#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
bool CDnTooltipDlg::IsCashRewardItem(const MIInventoryItem* pItem, bool bDontCareSN) const
{
	if (pItem == NULL)
	{
		_ASSERT(0);
		return false;
	}

	bool bCashItemCheck = pItem->IsCashItem();
	if (bCashItemCheck == false && bDontCareSN)
		bCashItemCheck = CDnItem::IsCashItem(pItem->GetClassID());

	if (bCashItemCheck && (m_itemSlotType == ST_QUEST || m_itemSlotType == ST_NPCREWARD || m_itemSlotType == ST_MISSIONREWARD))
		return true;

	return false;
}
#endif

#ifdef PRE_ADD_OVERLAP_SETEFFECT
bool CDnTooltipDlg::IsCashPartsItem(MIInventoryItem* pItem)
{
	// 캐시 파츠 / 무기 인지 체크하는 함수
	int  nSetItemID  = 0;
	bool bIsParts    = false;
	CDnItem* pDnItem = static_cast<CDnItem*>(pItem);
	if(!pDnItem) 
		return false;

	switch(pDnItem->GetItemType())
	{
	case ITEMTYPE_WEAPON:
		{
			CDnWeapon* pWeapon = dynamic_cast<CDnWeapon*>(pItem);
			if(pWeapon) nSetItemID	= pWeapon->GetSetItemID();
			bIsParts = true;
		}
		break;

	case ITEMTYPE_PARTS:
	case ITEMTYPE_PARTS_RING:
		{
			CDnParts* pParts = dynamic_cast<CDnParts*>(pItem);
			if(pParts) nSetItemID = pParts->GetSetItemID();
			bIsParts = true;
		}
		break;
	}
	
	if(pDnItem->IsCashItem() && bIsParts)// && nSetItemID > 0)
	{
		return true;
	}

	return false;
}
#endif // PRE_ADD_OVERLAP_SETEFFECT

void CDnTooltipDlg::SetTooltipText( MIInventoryItem *pItem, bool bCompare, bool bNameLink )
{
	ASSERT(pItem&&"CDnTooltipDlg::SetTooltipText, pItem==NULL!");
	ClearText();
	ClearDivisionLine();

	m_pOverlapCompareItem = pItem;

	bool bOnlyOnceCallFunction = false; // SetInteractionInfo() 이 함수가 두번 불리는걸 방지하는 변수.
	switch( pItem->GetType() ) 
	{
		case MIInventoryItem::Item:
#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
			{
				if (CDnCashShopTask::IsActive() && GetCashShopTask().IsUsingCashShopTooltip(pItem->GetCashItemSN(), m_itemSlotType) && (GetDialogType() != UI_TYPE_CHILD))
					SetCashShopItemText(pItem, bCompare);
				else if (IsCashRewardItem(pItem, false))
					SetCashShopItemText(pItem, bCompare, true);
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
				else if( m_itemSlotType == ST_ITEM_NOTIFY_COMPOUND )
					SetNotifyCompoundItemText( pItem );
#endif
				else
				{
					SetItemText( pItem, bCompare, bNameLink );
				}
			}
#else // PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
			if (CDnCashShopTask::IsActive() && GetCashShopTask().IsUsingCashShopTooltip(pItem->GetCashItemSN()) && (GetDialogType() != UI_TYPE_CHILD))
				SetCashShopItemText(pItem, bCompare);
#ifdef PRE_ADD_INSTANT_CASH_BUY
			else if (pItem->IsCashItem() && (m_itemSlotType == ST_QUEST || m_itemSlotType == ST_NPCREWARD || m_itemSlotType == ST_MISSIONREWARD || m_itemSlotType == ST_CASHINSTANCE_ITEM))
#else // PRE_ADD_INSTANT_CASH_BUY
			else if (pItem->IsCashItem() && (m_itemSlotType == ST_QUEST || m_itemSlotType == ST_NPCREWARD || m_itemSlotType == ST_MISSIONREWARD))
#endif // PRE_ADD_INSTANT_CASH_BUY
				SetCashShopItemText(pItem, bCompare, true);
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
			else if( m_itemSlotType == ST_ITEM_NOTIFY_COMPOUND )
				SetNotifyCompoundItemText( pItem );
#endif
			else
			{
				SetItemText( pItem, bCompare, bNameLink );
			}
#endif // PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
		

#ifdef PRE_ADD_OVERLAP_SETEFFECT
			{
				// 캐시 "Parts" 아이템은 무조건 SetInteractionInfo()를 보여주도록 변경
				// "인벤"이거나 "캐릭터 상태창"이거나, 이런 예외상황이 너무 많아서, 애초에 윗단에서 한번 실행.
				if(IsCashPartsItem(pItem) == true)
				{
					if (GetInterface().IsSkipInteraction() == false) {					
						SetInteractionInfo(pItem);
						bOnlyOnceCallFunction = true;
					}
				}
			}
#endif // PRE_ADD_OVERLAP_SETEFFECT


#if defined(PRE_ADD_63603)
			//인벤토리인 경우만 표시 되고, 나머지는 표시 안됨..
			{
				switch(m_itemSlotType)
				{
				case ST_INVENTORY:
				case ST_INVENTORY_CASH:
				case ST_INVENTORY_VEHICLE:
				case ST_INVENTORY_CASHREMOVE:
					{
						if (GetInterface().IsSkipInteraction() == false) {
							if(bOnlyOnceCallFunction == false)
								SetInteractionInfo(pItem);
						}
					}
					break;

#ifdef PRE_ADD_EQUIPLOCK
				case ST_CHARSTATUS:
					{
						if (GetInterface().IsSkipInteraction() == false)
						{
							if (pItem->GetType() != MIInventoryItem::Item)
								return;
							CDnItem* pDnItem = static_cast<CDnItem*>(pItem);
							if (pDnItem == NULL)
								break;

							const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
							EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pDnItem);
							if (status == EquipItemLock::Lock)
							{
								m_pTextBox->AddText( L"" );
								AddDivisionLine();

								m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8387)); // UISTRING : 마우스 우클릭 : 잠금 해제
								break;
							}
						}
					}
					break;
#endif
				}					
			}
#endif // PRE_ADD_63603
			break;
		
		case MIInventoryItem::Skill:
			if( CDnSkillTask::IsActive() && GetSkillTask().IsGuildWarSkill( pItem->GetClassID() ) )
				SetGuildWarSkillText( pItem );
			else
				SetSkillText( pItem );
			break;
		
		case MIInventoryItem::Gesture:
			ASSERT(0&&"제스처의 경우 이 함수까지 호출될 수 없습니다. 상위에서 처리되야 정상입니다.");
			break;
		
		default:
			ASSERT(0&&"CDnTooltipDlg::SetTooltipText, Unknow Inventory Type!");
			break;
	}

}


bool CDnTooltipDlg::SetItemText_NeedJob(const CDnItem* pItem)
{
	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};	// 보조
	DWORD dwFontColor = textcolor::WHITE;

	if( pItem->IsNeedJob() )
	{
		swprintf_s( szTemp, 256, L"%s ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2242 ) );
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );

		int nNeedJobCount = pItem->GetNeedJobCount();

		// 필요직업 중 하나라도 있으면 해당 장비는 사용가능한거라 한다. 그래서 하나라도 있으면 하얀색 처리.
		swprintf_s( szTemp, 256, L"" );
		for( int i = 0; i < nNeedJobCount; ++i ) {
			int nJobID = pItem->GetNeedJob(i);
			swprintf_s( szTempSub1, 256, L"%s", DN_INTERFACE::STRING::GetJobString( nJobID ) );

			if( szTemp[0] == L'\0' )
				swprintf_s( szTemp, 256, L"%s", szTempSub1 );
			else
				swprintf_s( szTemp, 256, L"%s, %s", szTemp, szTempSub1 );
		}

		std::vector<int> nVecJobList;
		if( CDnActor::s_hLocalActor )
			((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		// ((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if( !((CDnItem*)pItem)->IsPermitPlayer( nVecJobList ) )
			dwFontColor = textcolor::RED;
		m_pTextBox->AppendText( szTemp, dwFontColor );

		return true;
	}

	return false;
}

bool CDnTooltipDlg::SetItemText_DetailType(const CDnItem* pItem)
{
	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};	// 보조
	DWORD dwFontColor = textcolor::WHITE;

	bool bRet = false;

	if( pItem->GetItemType() == ITEMTYPE_WEAPON )
	{
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2243 ), textcolor::FONT_ORANGE );
		const CDnWeapon *pWeapon = dynamic_cast<const CDnWeapon *>(pItem);
		if( pWeapon )
		{
			if( pItem->IsCashItem() ) {
				swprintf_s( szTemp, 256, L" %s", ITEM::GetCashEquipString( pWeapon->GetEquipType() ) );
			}
			else swprintf_s( szTemp, 256, L" %s", ITEM::GetEquipString( pWeapon->GetEquipType() ) );
		}

		// #69626 [귀환자보상] 여신의 대행자 아이템 : 툴팁 문구의 타입 설명이 누락되어있음.
		// - 아이템타입이 ITEMTYPE_WEAPON 이지만, 아직 CreateWeapon() 으로 생성되지 않은상태라서 (인벤에 들어올때 CDnWeapon으로 생성된다. ) 
		//   CDnWeapon으로의 캐스팅이 실패한 경우이다.
		//   그래서 ClassID로 Weapon Table 에서 장착타입을 찾아 타입을 설정한다.
		else
		{			
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
			if( !pSox ) return false;
			if( !pSox->IsExistItem( pItem->GetClassID() ) )
				return false;	

			DNTableCell * pCell = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_EquipType" );
			if( pCell )
			{
				CDnWeapon::EquipTypeEnum equipType = (CDnWeapon::EquipTypeEnum)pCell->GetInteger();
				if( pItem->IsCashItem() )
					swprintf_s( szTemp, 256, L" %s", ITEM::GetCashEquipString( equipType ) );
				else 
					swprintf_s( szTemp, 256, L" %s", ITEM::GetEquipString( equipType ) );
			}
		}

		m_pTextBox->AppendText( szTemp, dwFontColor );
		bRet = true;
	}
	else if( pItem->GetItemType() == ITEMTYPE_PARTS )
	{
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2243 ), textcolor::FONT_ORANGE );
		CDnParts *pParts = dynamic_cast<CDnParts *>((CDnItem*)pItem);
		if( pParts )
		{
			if( pItem->IsCashItem() ) {
				swprintf_s( szTemp, 256, L" %s", ITEM::GetCashPartsString( pParts->GetPartsType() ) );
				if( pParts->IsExistSubParts() ) {
					for( int i=0; i<pParts->GetSubPartsCount(); i++ ) {
						CDnParts::PartsTypeEnum SubParts = pParts->GetSubPartsIndex(i);
						wcscat( szTemp, L", " );
						wcscat( szTemp, ITEM::GetCashPartsString(SubParts) );
					}
				}
			}
			else swprintf_s( szTemp, 256, L" %s", ITEM::GetPartsString( pParts->GetPartsType() ) );
		}

		// 우선 Weapon 만 처리함. 필요할때 주석제거하세요~. << 하지만 한가지 문제가 SubParts 의 경우에는 처리할 수 가 없다는 것이다. >>
		//// #69626 [귀환자보상] 여신의 대행자 아이템 : 툴팁 문구의 타입 설명이 누락되어있음.
		//// 위의 Weapon 의 경우와 같다.
		//else
		//{
		//	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
		//	if( !pSox ) return false;
		//	if( !pSox->IsExistItem( pItem->GetClassID() ) )
		//		return false;
		//	
		//	DNTableCell * pCell = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_Parts" );
		//	if( pCell )
		//	{
		//		CDnParts::PartsTypeEnum equipType = (CDnParts::PartsTypeEnum)pCell->GetInteger();

		//		if( pItem->IsCashItem() )
		//			swprintf_s( szTemp, 256, L" %s", ITEM::GetCashPartsString( equipType ) );
		//		else
		//			swprintf_s( szTemp, 256, L" %s", ITEM::GetPartsString( equipType ) );
		//	}

		//}

		m_pTextBox->AppendText( szTemp, dwFontColor );
		bRet = true;
	}

	return bRet;
}

bool CDnTooltipDlg::SetItemText_Period(const CDnItem* pItem)
{
	if ( pItem == NULL )
		return false;

#if defined(PRE_ADD_CASHREMOVE)
	if (pItem->IsEternityItem() && false == pItem->IsCashRemoveItem() )
#else
	if (pItem->IsEternityItem() )
#endif	// #if defined(PRE_ADD_CASHREMOVE)
	{
		if( pItem->IsCashItem() == false || pItem->GetSerialID() <= 0 )
			return false;

		if (GetCashShopTask().IsCostumeItem(pItem->GetClassID()))
		{
			std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4759);	// UISTRING : 서비스 기간 내 무제한
			m_pTextBox->AddText(str.c_str(), textcolor::WHITE);
			return true;
		}

		return false;
	}

	WCHAR dateString[256] = {0,};
	const time_t* pExpireDate = pItem->GetExpireDate();
	if (pExpireDate == NULL)
	{
#ifdef _RDEBUG
		std::wstring result = L"[DEBUG] 기간 오류!!";
		m_pTextBox->AddText(result.c_str(), textcolor::ORANGERED);
		return true;
#endif
		return false;
	}

	tm Day;
	if( !DnLocalTime_s(&Day, pExpireDate) ) return false;
	//아래 년도 제한 확인 코드를 여기서 검사 하도록함..
	if ((Day.tm_year + 1900) >= 2020 || (Day.tm_year + 1900) < 2010)	// temp by kalliste //edited by ralukat
		return false;

	CommonUtil::GetCashRemainDate(*pExpireDate, dateString, 256);
	bool bRet = false;

	std::wstring stringProcessed;
	if (CommonUtil::ConvertTimeFormatString(dateString, stringProcessed) != false)
	{
		std::wstring result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2115 ), stringProcessed.c_str()); // UISTRING : %s 까지 사용
		m_pTextBox->AddText(result.c_str(), textcolor::ORANGERED);
#ifdef PRE_ADD_PET_EXTEND_PERIOD
		if( pItem->GetItemType() == ITEMTYPE_PET )	// 기간 만료된 펫에 툴팁 추가
		{
			TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
			if( pPetCompact )
			{
				const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
				if( !pItem->IsEternityItem() && pNowTime >= pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )
				{
					m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9280 ), textcolor::ORANGE );
				}
			}
		}
#endif // PRE_ADD_PET_EXTEND_PERIOD
		bRet = true;
	}
	else
	{
		_ASSERT(0);
		m_pTextBox->AddText(L"ERROR", textcolor::RED);
		bRet = true;
	}
	
	return bRet;
}

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
bool CDnTooltipDlg::SetCashItemText_StorePeriod(const CDnItem* pItem)
{
	if ( pItem == NULL )
		return false;

	DNTableFileFormat* pTable = GetDNTable( CDnTableDB::TCOMBINEDSHOP );
	if( !pTable )
		return false;
	
	DNTableCell * pCell = NULL;
	for( int i = 0; i<pTable->GetItemCount(); ++i )
	{	
		int nTableID = pTable->GetItemID( i );

		// ItemIndex.
		pCell = pTable->GetFieldFromLablePtr( nTableID, "_itemindex" );
		if( pCell && pCell->GetInteger() == pItem->GetClassID() )
		{	
			// SlotIndex.
			pCell = pTable->GetFieldFromLablePtr( nTableID, "_ListID" );
			if( pCell && pCell->GetInteger() == pItem->GetSlotIndex() )
			{
				// Period.			
				pCell = pTable->GetFieldFromLablePtr( nTableID, "_Period" );
				if( pCell )
				{
					m_nStoreSaleItemPeriod = pCell->GetInteger();
					break;
				}
			}
		}		
	}

	if( m_nStoreSaleItemPeriod == -1 || m_nStoreSaleItemPeriod == 0 )
		return false;

	std::wstring strPeriod;

	// 기간제아이템.
	if( m_nStoreSaleItemPeriod != 0 )
		strPeriod = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1785 ), m_nStoreSaleItemPeriod );// "사용 기간 : (%d일)" 

	m_pTextBox->AddText( strPeriod.c_str() );

	return true;
}

void CDnTooltipDlg::SetCashItemText_StoreReversion(const CDnItem* pItem)
{
	std::wstring strTrade;

	// 거래가능.
	if( pItem->GetReversion() == ITEMREVERSION_TRADE )
	{	
		// 무제한.
		if( m_nStoreSaleItemPeriod == 0 )
		{				
			// 일본은 거래횟수 제한이 없다.
			if( CDnCashShopTask::GetInstance().IsCashTradeCountRestrict() == false )
			{
				strTrade.assign( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2116) ); // "(거래소 등록 가능)"
			}
			
			else
			{
				if( pItem->GetSealCount() > 0 )
					strTrade = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2118), pItem->GetSealCount() ); // "(거래소등록가능: %d회)"
				else
					strTrade.assign( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113) );	// "(거래불가)"
			}
		}

		// 기간제.
		else
			strTrade.assign( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113) );	// "(거래불가)"
	}

	// 거래불가.
	else
		strTrade.assign( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113) );	// "(거래불가)"

#if defined _US || defined _SG || defined _TH
	m_pTextBox->AddText( strTrade.c_str(), textcolor::WHITE );
#else
	m_pTextBox->AppendText( strTrade.c_str(), textcolor::WHITE, UITEXT_RIGHT );
#endif

}
#endif // PRE_ADD_COMBINEDSHOP_PERIOD


bool CDnTooltipDlg::SetItemText_UnableDisjointOrEnchant(CDnItem* pItem)
{
	if (pItem == NULL)
		return false;

	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};	// 보조
	wchar_t szTempSub2[256]={0};	// 보조
	szTempSub1[0] = szTempSub2[0] = '\0';

	// 분해불가를 붙여야한다면,(분해불가 체크를 해야하는 아이템이라면,)
	bool bNeedDisjointMsg = false;
	if( pItem->GetItemType() == ITEMTYPE_WEAPON || pItem->GetItemType() == ITEMTYPE_PARTS ) bNeedDisjointMsg = true;
	if( bNeedDisjointMsg && !pItem->CanDisjoint() ) {
		swprintf_s( szTempSub1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2295 ) );
	}

	// 강화불가를 붙여야한다면,(강화불가 체크를 해야하는 아이템이라면,)
	bool bNeedEnchantMsg = false;
	if( pItem->GetItemType() == ITEMTYPE_WEAPON ) bNeedEnchantMsg = true;
	if( pItem->GetItemType() == ITEMTYPE_PARTS ) {
		CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
		if( pParts && pParts->GetPartsType()>=CDnParts::Helmet && pParts->GetPartsType()<=CDnParts::Ring2 )
			bNeedEnchantMsg = true;

		if (pItem->IsCashItem())
		{
			if (pParts && pParts->GetPartsType() >= CDnParts::CashHelmet && pParts->GetPartsType() <= CDnParts::CashFairy)
				bNeedEnchantMsg = true;
		}
	}
	if( bNeedEnchantMsg ) {
		if( !pItem->GetEnchantID() || pItem->GetEnchantLevel() >= pItem->GetMaxEnchantLevel() )
			swprintf_s( szTempSub2, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2294 ) );
	}

	if( szTempSub1[0] != '\0' && szTempSub2[0] != '\0' )
		swprintf_s( szTemp, 256, L"%s, %s", szTempSub1, szTempSub2 );
	else
		swprintf_s( szTemp, 256, L"%s%s", szTempSub1, szTempSub2 );

	if( szTemp[0] != '\0' )
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );

	return (szTemp[0] != '\0');
}

#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
bool CDnTooltipDlg::SetItemText_UnableConstumeMix( const CDnItem* pItem )
{
	if( pItem != NULL && ! pItem->IsCashItem() && pItem->GetItemType() != ITEMTYPE_PARTS ) return false;
	if( !CDnItemTask::IsActive() ) return false;

	wchar_t szTemp[256]={0};
#ifdef PRE_ADD_COSRANDMIX
	int nRes = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr().GetRandomMixStuffItemRank( pItem->GetClassID() );
	if( nRes == -1 ) return false;

	bool bCostumMixItem = ( nRes > 0 ? true : false );

	if ( pItem->IsEternityItem() == false || pItem->GetLookItemID() != 0 )
		bCostumMixItem = false;

	if( bCostumMixItem )
		swprintf_s( szTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6008 ) );
	else 
		swprintf_s( szTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6009 ) );

	if( szTemp[0] != '\0' )
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	
#endif
	return (szTemp[0] != '\0');
}

bool CDnTooltipDlg::SetItemText_UnableDesignMix( const CDnItem* pItem ) 
{
	if( pItem != NULL && ! pItem->IsCashItem() && pItem->GetItemType() != ITEMTYPE_PARTS ) return false;
	if( !CDnItemTask::IsActive() ) return false;
	
	wchar_t szTemp[256]={0};

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGACHAINFO );
	if( ! pSox )
		return false;

	DNTableCell* pField = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_UseCompound");
	if(pField == NULL)
		return false;

	bool bDesignMixItem = (pField->GetInteger()) ? true : false;
	if (bDesignMixItem == false || pItem->IsEternityItem() == false || pItem->GetLookItemID() != 0)
		swprintf_s( szTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6011 ) );
	else 
		swprintf_s( szTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6010 ) );

	if( szTemp[0] != '\0' )
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	
	return (szTemp[0] != '\0');	
}
#endif


bool CDnTooltipDlg::SetItemText_Reversion(const CDnItem* pItem)
{
	if( pItem->IsInfoItem() )
		return false;

#if defined (_KR)
	if( pItem->IsCashItem() && CDnCashShopTask::IsActive() )	// 한국 - 캐시템 중 페키지는 표시하지 않음
	{
		const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo( pItem->GetCashItemSN() );
		if( pInfo && pInfo->type == eCSType_ePackage )
			return true;
	}
#endif // _KR

	if( pItem->IsCashItem() || 
		pItem->GetItemType() == ITEMTYPE_WEAPON || 
		pItem->GetItemType() == ITEMTYPE_PARTS || 
		pItem->GetItemType() == ITEMTYPE_GLYPH || 
		pItem->GetItemType() == ITEMTYPE_POTENTIAL_JEWEL ||
		pItem->GetItemType() == ITEMTYPE_SEALABLENORMAL ||
		( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) )
	{
		switch( pItem->GetReversion() ) {
			case ITEMREVERSION_BELONG:
				{
					std::wstring result;
					int count = pItem->GetSealCount();
					if (pItem->IsCashItem())
						count = 0;

					if (count > 0)
						result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2112), count); // UISTRING : (재밀봉 %d회)
					else if (count == 0)
					{
						if (pItem->IsSoulbBound() == false)
							result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4133);	 // UISTRING : (재봉인 불가)
						else
						{
							result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	 // UISTRING : (거래 불가)
						}
					}
					else
						result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4134);	 // UISTRING : (재봉인 제한없음)

#if defined _US || defined _SG || defined _TH
					m_pTextBox->AddText(result.c_str(), textcolor::WHITE);
#else
					m_pTextBox->AppendText(result.c_str(), textcolor::WHITE, UITEXT_RIGHT);
#endif
					return true;
				}
				break;
			case ITEMREVERSION_TRADE:
				{
					std::wstring result;
					if ( CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsCashTradeCountRestrict() == false)
					{
						if (pItem->GetLookItemID() != ITEMCLSID_NONE)
						{
							result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	// UISTRING : (거래불가)
						}
						else
						{
							result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2116);	 // UISTRING : (거래소 등록 가능)
						}
					}
					else
					{
#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
						bool bUsingCashShopTooltip = false;
						bUsingCashShopTooltip = ((CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsUsingCashShopTooltip(pItem->GetCashItemSN(), m_itemSlotType))
							|| IsCashRewardItem(pItem, false));
						if (bUsingCashShopTooltip)
#else
						if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsUsingCashShopTooltip(pItem->GetCashItemSN()))
#endif
						{
#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
							int number = 2116; // UISTRING : (거래소 등록 가능)
							if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsCostumeItem(pItem->GetClassID()))
							{
								number = 2119; // UISTRING : (무제한은거래가능)
							}
							else
							{
								if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsItemPermanent(pItem->GetCashItemSN()) == false)
									number = 2113; // UISTRING : (거래불가)
							}
#else
							int number = (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsCostumeItem(pItem->GetClassID())) ? 2119 : 2116; // UISTRING : (무제한은거래가능) / (거래소 등록 가능)
#endif
							result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, number);
						}
						else
						{
							if (pItem->GetLookItemID() != ITEMCLSID_NONE)
							{
								result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	// UISTRING : (거래불가)
							}
							else
							{
	#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
								bool bCheckAndPresentMarket = true;
								if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsOpenCashShopRecvGiftDlg())
								{
									bool bPeriodItem = false;
									const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(pItem->GetCashItemSN());
									if (pInfo)
										bPeriodItem = (pInfo->period > 0) ? true : false;

									if (pItem->IsEternityItem() == false || bPeriodItem)
									{
										bCheckAndPresentMarket = false;
										result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	// UISTRING : (거래불가)
									}
								}

								if (bCheckAndPresentMarket)
								{
									int count = pItem->GetSealCount();

									if (count > 0)
									{
										result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2118), count);	// UISTRING : (거래소등록가능: %d회)
									}
									else if (count == 0)
									{
										result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	// UISTRING : (거래불가)
									}
									else
									{
										result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2116);	 // UISTRING : (거래소 등록 가능)
									}
								}
	#else // PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
								int count = pItem->GetSealCount();
								if (count > 0)
								{
									result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2118), count);	// UISTRING : (거래소등록가능: %d회)
								}
								else if (count == 0)
								{
									result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2113);	// UISTRING : (거래불가)
								}
								else
								{
									result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2116);	 // UISTRING : (거래소 등록 가능)
								}
	#endif // PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
							}
						}
					}
#if defined _US || defined _SG || defined _TH
					m_pTextBox->AddText(result.c_str(), textcolor::WHITE);
#else
					m_pTextBox->AppendText(result.c_str(), textcolor::WHITE, UITEXT_RIGHT);
#endif
					return true;
				}
				break;
		}
	}

	return false;
}

wstring CDnTooltipDlg::GetItemName( CDnItem *pItem )
{
	wstring wszName;

	switch( pItem->GetItemType() ) {
		case ITEMTYPE_PERIOD_APPELLATION: // #53324 기간제칭호 명칭안나오는 문제.
		case ITEMTYPE_APPELLATION:
			{
				wchar_t szTemp[256]={0};
				swprintf_s( szTemp, L"%s : %s", pItem->GetName(), CDnAppellationTask::GetAppellationName( pItem->GetTypeParam(0) - 1 ).c_str() );
				wszName = szTemp;
			}
			break;
		case ITEMTYPE_PET:
			{
				// ComeBack - 로그인창에서 툴팁표시.	
				if( GetInterface().GetInterfaceType() == CDnInterface::Login )
					wszName = pItem->GetName();
				else
				{
					if( CDnVehicleTask::IsActive() && CDnPetTask::IsActive() )
					{
						wszName = pItem->GetName();
						TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
						if( pPetCompact )
						{
							if( wcslen( pPetCompact->wszNickName ) > 0 )	// 펫 이름이 있으면 펫 이름을 사용
							{
								wszName = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9246), 
									GetPetTask().GetPetLevel( pPetCompact->Vehicle[Pet::Slot::Body].nSerial ), 
									pPetCompact->wszNickName );
							}
						}
					}
				}
			}
			break;
		default:
			{
				wchar_t szTemp[256]={0};
				wchar_t szTempSub1[256]={0};	// 보조

				if( pItem->GetEnchantLevel() != 0 )
					swprintf_s( szTempSub1, 256, L"+%d ", pItem->GetEnchantLevel() );
#ifndef _FINAL_BUILD
				if( GetAsyncKeyState( VK_LSHIFT ) )
					swprintf_s( szTemp, 256, L"%s[ID:%d]", szTempSub1, pItem->GetClassID() );
				else if( GetAsyncKeyState( VK_RSHIFT ) )
					swprintf_s( szTemp, 256, L"%s[RightShift+우클릭=Serial복사]", szTempSub1 );
				else
					swprintf_s( szTemp, 256, L"%s%s", szTempSub1, pItem->GetName() );
#else
				swprintf_s( szTemp, 256, L"%s%s", szTempSub1, pItem->GetName() );
#endif
				wszName = szTemp;
			}
			break;
	}

	return wszName;
}

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
void CDnTooltipDlg::SetNotifyCompoundItemText( MIInventoryItem *pInvenItem )
{
	CDnItem *pItem = static_cast<CDnItem*>(pInvenItem);
	if( !pItem ) return;
	if( !CDnActor::s_hLocalActor ) return;

	wstring wszItemName = GetItemName( pItem );
	m_pStaticName->SetTextColor( ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
	m_pStaticName->SetText( wszItemName );
	
	m_pTextBox->AddText( L"" );
	AddDivisionLine();

	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( CDnItemCompoundNotifyDlg::GetCompoundIndex() , &CompoundInfo );

	bool bPossible = true;
	for( int k=0; k<5; k++ ) 
	{
		if( CompoundInfo.aiItemID[k] > 0 && CompoundInfo.aiItemCount[k] > 0 )
		{
			int nCurItemCount = 0;
			if( CompoundInfo.abResultItemIsNeedItem[k] )
			{
				nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[k] , CompoundInfo.iCompoundPreliminaryID );
			}
			else
			{
				nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[k] );
			}

			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( pSox && pSox->IsExistItem(CompoundInfo.aiItemID[k]) )
			{
				m_pTextBox->AddText( L"\n" );

				eItemRank eType = (eItemRank)pSox->GetFieldFromLablePtr( CompoundInfo.aiItemID[k], "_Rank" )->GetInteger();
				m_pTextBox->AppendText( CDnItem::GetItemFullName( CompoundInfo.aiItemID[k] , CompoundInfo.iCompoundPreliminaryID ).c_str() , ITEM::RANK_2_COLOR( eType ) );

				DWORD dwFontColor;
				dwFontColor = nCurItemCount >= CompoundInfo.aiItemCount[k] ? textcolor::PALEGOLDENROD : textcolor::ORANGERED;
				m_pTextBox->AppendText( L"  (" , textcolor::PALEGOLDENROD );
				m_pTextBox->AppendText( FormatW( L"%d" ,nCurItemCount > CompoundInfo.aiItemCount[k] ? CompoundInfo.aiItemCount[k] : nCurItemCount ).c_str() , dwFontColor );
				m_pTextBox->AppendText( FormatW( L"/%d)" , CompoundInfo.aiItemCount[k] ).c_str() ,textcolor::PALEGOLDENROD );
			}
		}
	}

	m_pTextBox->AddText( L"" );
	AddDivisionLine();

	std::wstring wstCost;

	INT64 nCoin = CompoundInfo.iCost;
	INT64 nGold = nCoin/10000;
	INT64 nSilver = (nCoin%10000)/100;
	INT64 nBronze = nCoin%100;

	wstCost += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 751 );
	wstCost += FormatW(  L"  : %I64d%s" , nGold , GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 507 ) );
	wstCost += FormatW(  L" %I64d%s" , nSilver , GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 508 ) );
	wstCost += FormatW(  L" %I64d%s" , nBronze , GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 509 ) );
	m_pTextBox->AddText( wstCost.c_str() );

}
#endif

void CDnTooltipDlg::SetItemText( MIInventoryItem *pInvenItem, bool bCompare, bool bNameLink )
{
	ASSERT(pInvenItem&&"CDnTooltipDlg::SetItemText, pInvenItem==NULL!");

	if( pInvenItem->GetType() != MIInventoryItem::Item )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetItemText, 아이템이 아닙니다." );
		return;
	}

	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};
	DWORD dwFontColor;

	// 기본 폰트컬러 색상. 이제는 다르게 하지 않는것으로 변경됨.
	//dwFontColor = bCompare ? textcolor::DARKGRAY : textcolor::WHITE;
	dwFontColor = bCompare ? textcolor::WHITE : textcolor::WHITE;

	CDnItem *pItem = static_cast<CDnItem*>(pInvenItem);
	if( !pItem ) return;

	// ComeBack - 로그인창에서 툴팁표시.	
	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
		if( !CDnActor::s_hLocalActor ) return;
	//if( !CDnActor::s_hLocalActor ) return;

	wstring wszItemName = GetItemName( pItem );

	// 강화 이름
	m_pStaticName->SetTextColor( ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
	if( bCompare )
	{
		if( m_pTargetDiffItem )
			wszItemName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8066 );
		else {
			SUICoord NameCoord, LineCoord;
			CalcTextRect( L" ", m_pStaticName->GetElement(0), LineCoord );
			CalcTextRect( wszItemName.c_str(), m_pStaticName->GetElement(0), NameCoord );
			if( NameCoord.fWidth < m_TextBoxDefaultCoord.fWidth && NameCoord.fHeight < LineCoord.fHeight * 2.0f )
				wszItemName += '\n';


#ifdef PRE_ADD_COMPARETOOLTIP
			 // 반지비교툴팁 - 현재 보이는 툴팁을 구분.			
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			int nSlotIndex = -1;
			if( pItem->IsCashItem() )
				nSlotIndex = pCharStatusDlg->GetCashEquipIndex( pItem );
			else
				nSlotIndex = pCharStatusDlg->GetEquipIndex( pItem );
			
			//if( !bNameLink && 
			//	pItem->GetType() == MIInventoryItem::Item &&
			//	( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ||
			//	( pItem->IsCashItem() && (nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2) ) )
			//	)

			if( !CDnCashShopTask::GetInstance().IsOpenCashShop() && !bNameLink && 
				pItem->GetType() == MIInventoryItem::Item && pItem->GetItemType() == ITEMTYPE_PARTS &&
				( ( pItem->IsCashItem() && (nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2) ) ||
				  ( !pItem->IsCashItem() && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ) ) )
			  )
			{				
				int strID = 2219; // "(반지1 장착중)"

				// 캐시.
				if( pItem->IsCashItem() && nSlotIndex == CASHEQUIP_RING2 )
					strID = 2237; // "(반지2 장착중)"
				// 일반.
				else if( nSlotIndex == EQUIP_RING2 )
					strID = 2237; // "(반지2 장착중)"				

				wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strID );				
			}
			else
				wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2259 );

#else
			wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2259 );
#endif // PRE_ADD_COMPARETOOLTIP

		}
	}

#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	int nItemCategoryType = pItem->GetCategoryType();
	if (nItemCategoryType != 0)
	{
		int descIndex = CItemCategoryInfo::GetInstance().GetDescIndex(nItemCategoryType);
		if (descIndex > 0)
		{
#ifdef PRE_MOD_ITEM_COMPOUND_TOOLTIP
			if( pItem->GetItemType() == ITEMTYPE_POTENTIAL_JEWEL )
			{
				CDnPotentialJewel* pPotentialItem = static_cast<CDnPotentialJewel*>(pItem);
				if( pPotentialItem && pPotentialItem->IsSuffix() )	
					descIndex = 1000047767;		// MID ( 접미사 ) 
			}
#endif
			wszItemName += FormatW(L"\n%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, descIndex));
		}
	}
#endif

#ifdef PRE_ADD_BESTFRIEND
	
	bool bDestroyFriend = false;
	// 절친반지 이름처리
	//if( pItem->GetItemType() != ITEMTYPE_GIFTBOX && pItem->GetTypeParam(0) == 10 )
	if( pItem->GetItemType() == ITEMTYPE_PARTS && 
		pItem->GetTypeParam(0) == 10 && pItem->GetTypeParam(1) == 10 )
	{
		wchar_t strBF[256];
		CDnInspectPlayerDlg * pInspect = GetInterface().GetInspectDialog();

		if( bNameLink )
		{			
			m_pStaticName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4350 ) );
		}
	
		else
		{
			//살펴보기 : 다른플레이어의 반지.
			if( pInspect && pInspect->IsShow() && pItem->GetSerialID() == pInspect->GetBFserial() )
				//if( pInspect && pInspect->IsShow() )
			{
				TCHAR * pBFname = pInspect->GetBestfriendName();
				if( pBFname )
				{
					swprintf_s( strBF, 256, wszItemName.c_str(), pBFname );
					m_pStaticName->SetText( strBF );
				}						
			}

			// 내반지.
			else if( pItem->GetSerialID() == GetInterface().GetSerialBF() ) 
			{	
				swprintf_s( strBF, 256, wszItemName.c_str(), GetInterface().GetNameBF() );
				m_pStaticName->SetText( strBF );		
			}

			// "파괴된절친반지"
			else
			{	
				bDestroyFriend = true;
				m_pStaticName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4349 ) );		
			}
		}
	}
		
	//
	else
		m_pStaticName->SetText( wszItemName.c_str() );

#else 
	m_pStaticName->SetText( wszItemName.c_str() );
#endif

	{
		// 이름 긴 아이템을 강화한 상태에서 장착중까지 붙으면 너무 길어진다.
		// 아직은 3줄까지는 안되지만, 나중에 3줄이상 가게되면,
		// 텍스트박스만 내리고, 다이얼로그 크기도 바꾸고 Name스태틱컨트롤 위치도 조정해야한다.
	}

	{
#ifdef PRE_ADD_SECONDARY_SKILL
		if( pItem->GetItemType() == ITEMTYPE_SECONDARYSKILL_RECIPE )
		{
			CDnSecondarySkillRecipeItem *pRecipeItem = dynamic_cast<CDnSecondarySkillRecipeItem *>(pItem);
			if( pRecipeItem && pRecipeItem->GetDurability() > 0 )
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7506 ), textcolor::RED, UITEXT_CENTER );
		}
#endif // PRE_ADD_SECONDARY_SKILL

		// 귀속 표시, 파괴불가
		{
			SetItemText_ExchangeAttribute(pItem, bNameLink);
		}
		SetItemText_CostumeDesignMix(pItem);

		if( ITEMTYPE_GLYPH == pItem->GetItemType() )
		{
			int nCashIndex = GetItemTask().GetEquipCashGlyphIndex( pItem );

			if( -1 != nCashIndex && !GetItemTask().IsCashGlyphOpen( nCashIndex ) )
				m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9032 ), textcolor::RED );
		}

#ifdef PRE_ADD_TALISMAN_SYSTEM
		if( ITEMTYPE_TALISMAN == pItem->GetItemType() )
		{
			if( GetItemTask().IsCompareCashTalismanSlotID(pItem) && !GetItemTask().IsOpenCashTalismanSlot() )
			{
				m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9032 ), textcolor::RED );
			}
		}
#endif

		m_pTextBox->AddText( L"" );
		AddDivisionLine();

#ifdef PRE_ADD_EQUIPLOCK
		SetItemText_LockState(pItem);
#endif

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
		if (m_itemSlotType != ST_INSPECT )
		{
			// 아이템의 기간처리.
			if( SetItemText_Period(pItem) )
			{
				m_pTextBox->AddText(L"");
				AddDivisionLine();
			}


			// 상점이 열려있을때 상점에서 설정한 캐시아이템기간처리.
			m_nStoreSaleItemPeriod = -1; // // 기간초기화.
			if( pItem->IsCashItem() &&
				m_itemSlotType == ST_STORE && 				
				GetInterface().IsOpenStore() && 
				SetCashItemText_StorePeriod(pItem) )
			{
				m_pTextBox->AddText(L"");
				AddDivisionLine();
			}
			
		}
#else
		if (m_itemSlotType != ST_INSPECT && SetItemText_Period(pItem))
		{
			m_pTextBox->AddText(L"");
			AddDivisionLine();
		}
#endif // PRE_ADD_COMBINEDSHOP_PERIOD


		// 제한 레벨
		{
			if( pItem->GetLevelLimit() > 1 )
			{
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2240 ), textcolor::FONT_ORANGE );

#ifdef PRE_ADD_SECONDARY_SKILL
				if( pItem->GetItemType() == ITEMTYPE_COOKING )
				{
					wchar_t szTemp1[256]={0};
					wchar_t szTemp2[256]={0};
					int nUseLevelLimit = 0;
					swprintf_s( szTemp1, 256, L" %d %s", pItem->GetLevelLimit(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );

					DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TITEM );
					if( pSox )
					{
						nUseLevelLimit = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_UseLevelLimit" )->GetInteger();
						if( nUseLevelLimit > 0 )
						{
							swprintf_s( szTemp2, 256, L" %d %s", nUseLevelLimit, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7522 ) );
							swprintf_s( szTemp, 256, L" %s %s %s", szTemp1, TILDE, szTemp2 );
						}
						else
							swprintf_s( szTemp, 256, L" %s", szTemp1 );
					}

					if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() < pItem->GetLevelLimit() || ( nUseLevelLimit > 0 && nUseLevelLimit < CDnActor::s_hLocalActor->GetLevel() ) )
						dwFontColor = textcolor::RED;
				}
				else
				{
					swprintf_s( szTemp, 256, L" %d %s", pItem->GetLevelLimit(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
					if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() < pItem->GetLevelLimit() )
					{
						dwFontColor = textcolor::RED;
					}
				}
#else // PRE_ADD_SECONDARY_SKILL
				swprintf_s( szTemp, 256, L" %d %s", pItem->GetLevelLimit(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
				if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() < pItem->GetLevelLimit() )
				{
					dwFontColor = textcolor::RED;
				}
#endif // PRE_ADD_SECONDARY_SKILL
				m_pTextBox->AppendText( szTemp, dwFontColor );
				//dwFontColor = bCompare ? textcolor::DARKGRAY : textcolor::WHITE;
				dwFontColor = bCompare ? textcolor::WHITE : textcolor::WHITE;
			}
		}

		// 직업 제한
		{
			SetItemText_NeedJob(pItem);
		}

		// 아이템 세부 타입
		{
			SetItemText_DetailType(pItem);
		}
#if defined(PRE_ADD_SECONDARY_SKILL)
		if( pItem->GetItemType() == ITEMTYPE_SEED )
		{
			SetItemText_SecondarySkill( SecondarySkill::SubType::CultivationSkill, pItem );
		}
		if( pItem->GetItemType() == ITEMTYPE_SECONDARYSKILL_RECIPE )
		{
			SetItemText_SecondarySkill( SecondarySkill::SubType::CookingSkill, pItem );
		}
		if( pItem->GetItemType() == ITEMTYPE_FISHINGROD )
		{
			SetItemText_SecondarySkill( SecondarySkill::SubType::FishingSkill, pItem );
		}
#endif // PRE_ADD_SECONDARY_SKILL
		// 아이템 등급
		{
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2252 ), textcolor::FONT_ORANGE );
			swprintf_s( szTemp, 256, L" %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ITEM::RANK_2_INDEX( pItem->GetItemRank() ) ) );
			m_pTextBox->AppendText( szTemp, dwFontColor );
		}

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
		if ( pItem->IsCashItem() )
		{			
			if( m_nStoreSaleItemPeriod != -1 )
				SetCashItemText_StoreReversion(pItem);
		
			else
				SetItemText_Reversion(pItem);
		}
#else
		if (pItem->IsCashItem())
			SetItemText_Reversion(pItem);
#endif // PRE_ADD_COMBINEDSHOP_PERIOD

		// 스킬북인 경우 지금 배운건지 아닌건지 확인...
		if( pItem->GetItemType() == ITEMTYPE_SKILLBOOK )
		{
			int iSkillBookItemID = pItem->GetClassID();
			//if( GetSkillTask().FindSkillBySkillBookItemID( iSkillBookItemID ) )
			if( GetSkillTask().FindZeroLevelSkillBySkillBookItemID( iSkillBookItemID ) || 
				GetSkillTask().FindSkillBySkillBookItemID( iSkillBookItemID ) )
			{
				// 이미 배움
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1746 ), textcolor::RED );
			}
			else
			{
				// 아직 배우지 않음.
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1747 ), textcolor::WHITE );
			}
		}

		// 재사용 시간. 스킬 가진 문장일때 뜨면 안되니 아이템 타입 체크한다.
		{
			if( pItem->GetSkillID() > 0 && ( pItem->GetItemType() == ITEMTYPE_NORMAL || pItem->GetItemType() == ITEMTYPE_COOKING ) && pItem->GetDelayTime() > 0 )
			{
				m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_COOLTIME ), textcolor::FONT_ORANGE );
				int nSec = (int)(pItem->GetDelayTime());
				swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2312 ), nSec );
				swprintf_s( szTemp, 256, L" %s", szTempSub1 );
				m_pTextBox->AppendText( szTemp, dwFontColor );
			}
		}

		// 내구도.
#ifdef PRE_MOD_INSPECT_DURABILITY
		if( m_itemSlotType != ST_INSPECT )
#endif
		{
			// 노랑 : 255, 212, 5
			// 주황 : 255, 141, 10
			// 빨강 : 230, 52, 34
			DWORD dwRepairYellow = 0xFFFFD405;
			DWORD dwRepairOrange = 0xFFFF8D0A;
			DWORD dwRepairRed = 0xFFE63422;
			DWORD dwRepairColor = 0xFFFFFFFF;

			if( pItem->GetItemType() == ITEMTYPE_WEAPON )
			{
				CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
				if( pWeapon && pWeapon->GetMaxDurability() > 0 )
				{
					m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DURABLE ), textcolor::FONT_ORANGE );
					swprintf_s( szTemp, 256, L" %d/%d", (int)( 0.99f + ( pWeapon->GetDurability()/100.0f )) , (int)( 0.99f + ( pWeapon->GetMaxDurability()/100.0f ) ) );

					float fRate = (float)pWeapon->GetDurability() / pWeapon->GetMaxDurability();
					if( fRate == 0.0f ) dwRepairColor = dwRepairRed;
					else if( fRate <= 0.1f ) dwRepairColor = dwRepairOrange;
					else if( fRate <= 0.2f ) dwRepairColor = dwRepairYellow;
					else dwRepairColor = dwFontColor;
					m_pTextBox->AppendText( szTemp, dwRepairColor );
				}
			}
			else if( pItem->GetItemType() == ITEMTYPE_PARTS )
			{
				CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
				if( pParts && pParts->GetMaxDurability() > 0 )
				{
					int nPartsMin = CDnParts::Helmet;
					int nPartsMax = CDnParts::Foot;
					if( pParts->IsCashItem() ) {
						nPartsMin = CDnParts::CashHelmet;
						nPartsMax = CDnParts::CashFoot;
					}

					if( pParts->GetPartsType()>=nPartsMin && pParts->GetPartsType()<=nPartsMax )
					{
						m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DURABLE ), textcolor::FONT_ORANGE );
						swprintf_s( szTemp, 256, L" %d/%d", (int)( 0.99f + ( pParts->GetDurability()/100.0f )) , (int)( 0.99f + ( pParts->GetMaxDurability()/100.0f ) ) );

						float fRate = (float)pParts->GetDurability() / pParts->GetMaxDurability();
						if( fRate == 0.0f ) dwRepairColor = dwRepairRed;
						else if( fRate <= 0.1f ) dwRepairColor = dwRepairOrange;
						else if( fRate <= 0.2f ) dwRepairColor = dwRepairYellow;
						else dwRepairColor = dwFontColor;
						m_pTextBox->AppendText( szTemp, dwRepairColor );
					}

				}
			}

#ifdef PRE_MOD_INSPECT_DURABILITY
#else
			if (pItem->IsCashItem() == false)
				SetItemText_Reversion(pItem);
#endif
		}
#ifdef PRE_MOD_INSPECT_DURABILITY
		if (pItem->IsCashItem() == false)
			SetItemText_Reversion(pItem);
#endif

		// 착용조건 PVP등급
		{
			if( pItem->GetNeedPvpRank() > 0 )
			{
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2296 ), textcolor::FONT_ORANGE );

				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
				int nUIString = 0;
				if( pSox && pSox->IsExistItem(pItem->GetNeedPvpRank()) )
					nUIString = pSox->GetFieldFromLablePtr( pItem->GetNeedPvpRank(), "PvPRankUIString" )->GetInteger();
				swprintf_s( szTemp, 256, L" %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ) );

				TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
				if( pPvPInfo->cLevel < pItem->GetNeedPvpRank() )
					dwFontColor = textcolor::RED;
				m_pTextBox->AppendText( szTemp, dwFontColor );
				dwFontColor = bCompare ? textcolor::WHITE : textcolor::WHITE;
			}
		}

		// 인증여부
		{
			if( pItem->IsNeedAuthentication() )
			{
				m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_JUDGMENT ) );
			}
		}

		// 분해불가, 강화불가
		{
			SetItemText_UnableDisjointOrEnchant(pItem);
		}
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
		// 코스튬 합성 가능, 불가능 
		{	
			SetItemText_UnableConstumeMix( pItem ); 		
		}
		// 디자인 합성 가능, 불가능 
		{
			SetItemText_UnableDesignMix( pItem );
		}
#endif 

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		// 공백구분 처리 위해 사용.
		std::wstring wszLastLine;

		// 우선 구분선 대신 공백으로..
		m_pTextBox->AddText( L"" );
		AddDivisionLine();

#ifdef PRE_ADD_SALE_COUPON
		if( pItem->GetItemType() == ITEMTYPE_SALE_COUPON )
		{
			SetItemText_CouponSaleItemList( pItem );
		}
#endif // PRE_ADD_SALE_COUPON

		if(pItem->GetItemType() == ITEMTYPE_VEHICLE || pItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
		{
			DNTableFileFormat* pItemTable = GetDNTable( CDnTableDB::TITEM);
			int nVehicleActorSpeed =  pItemTable->GetFieldFromLablePtr( pItem->GetClassID(), "_TypeParam2" )->GetInteger();
			WCHAR wszMsg[256] = {0,};
			wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9215 ), nVehicleActorSpeed );
			m_pTextBox->AddText(wszMsg);
			m_pTextBox->AddText( L"" );
			AddDivisionLine();
		}

#ifdef PRE_ADD_SERVER_WAREHOUSE
	#ifdef PRE_REMOVE_SERVER_WAREHOUSE
	#else
		if (GetInterface().IsOpenStorage())
		{
			if (pItem->IsTradable(true) == false)
				m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1633), textcolor::RED); // UISTRING : 서버창고 이용불가				
			else
				m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1623), textcolor::YELLOW); // UISTRING : 서버창고 이용가능
			
			m_pTextBox->AddText( L"" );
			AddDivisionLine();
		}
	#endif // PRE_REMOVE_SERVER_WAREHOUSE
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)
		// 탈리스만 슬롯의 효율 설명
		if(m_hSlotButton && m_hSlotButton->GetSlotType() == ST_TALISMAN)
		{
			int nSlotIndex = m_hSlotButton->GetSlotIndex();
			float fRatio = GetItemTask().GetTalismanSlotRatio(nSlotIndex);
			
			WCHAR wszTemp[256] = { 0 , };
			wsprintf(wszTemp, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8366), (int)(fRatio*100));
			m_pTextBox->AddText(wszTemp, textcolor::YELLOW);
		}
#endif

		// 기본 능력치
		std::vector<CDnItem*> compareItemList;
		if (bCompare == false)
		{
			if (pItem->GetItemType() == ITEMTYPE_PARTS && m_itemSlotType != ST_CHARSTATUS)
			{
				CDnParts *pParts = static_cast<CDnParts *>(pItem);
				if (pParts)
				{
					if ((pParts->IsCashItem() && pParts->GetPartsType() == CDnParts::CashRing) ||
						(pParts->IsCashItem() == false && pParts->GetPartsType() == CDnParts::Ring))
					{
						GetCompareDestItem_Ring(compareItemList, pItem);
					}
				}
			}

			if (compareItemList.empty())
			{
				CDnItem* pSrcItemForState = GetCompareDestItem(pItem);
				if (pSrcItemForState == NULL)
					compareItemList.clear();
				else
					compareItemList.push_back(pSrcItemForState);
			}
		}
		if( m_pTargetDiffItem ) {
			compareItemList.clear();
			compareItemList.push_back( m_pTargetDiffItem );
		}

#ifdef PRE_ADD_BESTFRIEND
		// 파괴된 절친반지 에는 능력치를 표시하지 않는다.
		if( !bDestroyFriend )
			SetItemStateText( pItem, dwFontColor, NULL, &compareItemList);
#else
		SetItemStateText( pItem, dwFontColor, NULL, &compareItemList);
#endif

		// 보물상자의 경우엔 특별처리한다.
#if defined( PRE_ADD_EASYGAMECASH )
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
		if( pItem->GetItemType() == ITEMTYPE_CHARM || pItem->GetItemType() == ITEMTYPE_ALLGIVECHARM
												   || pItem->GetItemType() == ITEMTYPE_CHARMRANDOM)
		{
			SetCharmItemText( pItem );
			SetPackageItemText( pItem );
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
			SetRandomCharmItemText( pItem );
#endif
				
		}
#else
		if( pItem->GetItemType() == ITEMTYPE_CHARM || pItem->GetItemType() == ITEMTYPE_ALLGIVECHARM)
		{
			SetCharmItemText( pItem );
			SetPackageItemText( pItem );
		}
#endif 
#else
		if( pItem->GetItemType() == ITEMTYPE_CHARM )
			SetCharmItemText( pItem );
#endif
		if( pItem->GetItemType() == ITEMTYPE_APPELLATION ||
			pItem->GetItemType() == ITEMTYPE_PERIOD_APPELLATION ) // #53324 기간제칭호 명칭안나오는 문제와 연계( 능력치 표시 ).
			SetAppellationItemText( pItem );

		//접두어 시스템 정보 표시
		int nSkillID = -1;
		int nSkillLevel = 0;
		if (pItem->HasPrefixSkill(nSkillID, nSkillLevel))
		{
			SetItemPrefixSystemInfo(pItem, nSkillID, nSkillLevel);
		}
#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
		else
		{
			bool bSetCompoundEnableTooltip = false;

			if (CDnItem::IsCashItem(pItem->GetClassID()) == false)
			{
				if (pItem->GetItemType() == ITEMTYPE_WEAPON)
				{
					bSetCompoundEnableTooltip = true;
				}
				else if (pItem->GetItemType() == ITEMTYPE_PARTS)
				{
					CDnParts::PartsTypeEnum partsType = CDnParts::GetPartsType(pItem->GetClassID());
					switch(partsType)
					{
					case CDnParts::Helmet:
					case CDnParts::Body:
					case CDnParts::Leg:
					case CDnParts::Hand:
					case CDnParts::Foot:
						{
							bSetCompoundEnableTooltip = true;
						}
						break;

					default:
						{
							bSetCompoundEnableTooltip = false;
						}
						break;
					}
				}
			}

			if (bSetCompoundEnableTooltip && CDnItemTask::IsActive())
			{
				if (GetItemTask().IsEnableCompoundItem(pItem->GetClassID()))
				{
					wszLastLine = m_pTextBox->GetLastLineText();
					if (wszLastLine.size())
					{
						m_pTextBox->AddText(L"");
						AddDivisionLine();
					}

					SetCompoundEnableInfo();
				}
			}
		}
#endif

		// 강화 능력
		if( pItem->GetEnchantLevel() != 0 )
		{
			wszLastLine = m_pTextBox->GetLastLineText();
			if( wszLastLine.size() )
			{
				m_pTextBox->AddText( L"" );
				AddDivisionLine();
			}

			swprintf_s( szTemp, 256, L"[%s]", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8004 ) );
			m_pTextBox->AddText( szTemp, dwFontColor );
			SetItemStateText(pItem->GetEnchantState(), textcolor::FONT_GREEN, NULL, &compareItemList, eSCC_ENCHANT);
		}
#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
		else if (pItem->GetEnchantID() && CDnItem::IsCashItem(pItem->GetClassID()) == false)
		{
			wszLastLine = m_pTextBox->GetLastLineText();
			if (wszLastLine.size())
			{
				m_pTextBox->AddText(L"");
				AddDivisionLine();
			}

			SetEnchantEnableInfo();
		}
#endif

		// 잠재
		if( pItem->GetPotentialIndex() != 0 )
		{
			wszLastLine = m_pTextBox->GetLastLineText();
			if( wszLastLine.size() )
			{
				m_pTextBox->AddText( L"" );
				AddDivisionLine();
			}

			swprintf_s( szTemp, 256, L"[%s]", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8005 ) );
			m_pTextBox->AddText( szTemp, dwFontColor );
			SetItemStateText( pItem->GetPotentialState(), textcolor::DODGERBLUE, NULL, &compareItemList, eSCC_POTENTIAL );

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
			if( pItem->IsPotentialExtractableItem() )
			{
				int nPotentialMaxMoveCount = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PotentialMovealbleCount );
				int nRemain = nPotentialMaxMoveCount - pItem->GetPotentialMoveCount();
				if( nRemain < 0 ) nRemain = 0;
				swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8089 ), nRemain );
			}
			else
			{
				swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8090 ) );
			}
			m_pTextBox->AddText( szTemp, textcolor::DODGERBLUE );
#endif
		}
#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
		else
		{
			if (CDnItem::IsCashItem(pItem->GetClassID()) == false)
			{
				if (pItem->GetItemType() == ITEMTYPE_PARTS || pItem->GetItemType() == ITEMTYPE_WEAPON)
				{
					if (pItem->GetTypeParam(1) != 0)
					{
						wszLastLine = m_pTextBox->GetLastLineText();
						if (wszLastLine.size())
						{
							m_pTextBox->AddText(L"");
							AddDivisionLine();
						}

						SetPotentialEnableInfo();
					}
				}
			}
		}
#endif

		// 문장 등의 스킬습득
		/*문장수정예정
		if( pItem->GetSkillID() > 0 && pItem->GetItemType() == CDnItem::Parts )
		{
			CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
			if( pParts )
			{
				if( pParts->GetPartsType() == CDnParts::Crest )
				{
					wszLastLine = m_pTextBox->GetLastLineText();
					if( wszLastLine.size() )
					{
						m_pTextBox->AddText( L"" );
						AddDivisionLine();
					}

					swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2283 ) );
					m_pTextBox->AddText( szTemp, dwFontColor );

					DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );

					// skill table
					int nSkillStringID = 0;
					nSkillStringID = pSkillTable->GetFieldFromLablePtr( pItem->GetSkillID(), "_NameID" )->GetInteger();
					if( nSkillStringID < 0 ) ASSERT(0&&"문장의 스킬이름 스트링 아이디가 이상합니다.");

					swprintf_s( szTemp, 256, L"%s(LV %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSkillStringID ), pItem->GetSkillLevel() );
					m_pTextBox->AddText( szTemp, textcolor::YELLOW );
				}
			}
		}
		*/

		// 아이템제작용 옵션
		bool bOptionList = false;
		CEtUIDialog *pItemCompoundDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_DIALOG );
		CEtUIDialog *pItemCompound2Dlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG );
		CEtUIDialog *pStoreDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
		if( pItemCompoundDlg && pItemCompoundDlg->IsShow() && m_itemSlotType == ST_ITEM_COMPOUND ) bOptionList = true;
		if( pItemCompound2Dlg && pItemCompound2Dlg->IsShow() && m_itemSlotType == ST_ITEM_COMPOUND ) bOptionList = true;
		if( pStoreDlg && pStoreDlg->IsShow() && m_itemSlotType == ST_STORE )
		{
			bOptionList = true;
			CDnStoreTabDlg* pStoreTabDlg = static_cast<CDnStoreTabDlg*>(pStoreDlg);
			if (pStoreTabDlg && pStoreTabDlg->IsShowRepurchasableTab())
				bOptionList = false;
		}
		if( bOptionList )
		{
			if( pItem->GetItemType() == ITEMTYPE_WEAPON || pItem->GetItemType() == ITEMTYPE_PARTS )
			{
				int nTypeParam = pItem->GetTypeParam();
				DNTableFileFormat* pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
				if( pPotentialSox && nTypeParam > 0 )
				{
					std::vector<int> nVecItemID;
					pPotentialSox->GetItemIDListFromField( "_PotentialID", nTypeParam, nVecItemID );

					// 접두사 부여 옵션은 제외하고 랜덤옵션만 나오게 수정한다.
					for( std::vector<int>::iterator iter = nVecItemID.begin(); iter != nVecItemID.end(); )
					{
						int nPotenialItemID = (int)(*iter);
						float fRatio = pPotentialSox->GetFieldFromLablePtr( nPotenialItemID, "_PotentialRatio" )->GetFloat() * 100.f;
						if( fRatio == 0.0f ) iter = nVecItemID.erase( iter );
						else ++iter;
					}

					// 50렙 제작 방식이 바뀌게되면서 랜덤옵션 하나만 가지고 있으면서, 포텐샬능력치는 없는 기본템이 추가되었다고 합니다.
					// 그래서 이럴땐 아이템 제작옵션 문구만 뜨고 내용은 하나도 없어서 버그처럼 보인다고 해서,
					// 아래와 같이 미리 한번 검사해보고 보여주는 방법으로 수정해둡니다.
					bool bRandomOption = false;
					for( int i = 0; i < (int)nVecItemID.size(); ++i )
					{
						int nItemID = nVecItemID[i];
						char szLabel[32];
						for( int i=0; i<16; i++ )
						{
							sprintf_s( szLabel, "_State%d", i + 1 );
							int nStateType = pPotentialSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
							if( nStateType == -1 ) break;
							bRandomOption = true;
							break;	// 하나라도 있으면 있는거니 break
						}
						if( bRandomOption ) break;
					}

					// 랜덤 옵션
					if( bRandomOption )
					{
						wszLastLine = m_pTextBox->GetLastLineText();
						if( wszLastLine.size() )
						{
							m_pTextBox->AddText( L"" );
							AddDivisionLine();
						}

						if( m_itemSlotType != ST_STORE )
						{
							swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2297 ) );
							m_pTextBox->AddText( szTemp, textcolor::DARKVIOLET );
						}

						for( int i = 0; i < (int)nVecItemID.size(); ++i )
						{
							// 각각의 옵션마다 작은 구분선으로 구분
							m_pTextBox->AddText( L"" );
							AddDivisionLine( 6, true );

							if( m_itemSlotType == ST_STORE && i == 0 )
							{
								m_pTextBox->RemoveLastLine();
								RemoveLastDivisionLine();
							}

							int nItemID = nVecItemID[i];
							char szLabel[32];
							static CDnState s_OptionState;
							s_OptionState.ResetState();
							for( int i=0; i<16; i++ ) {
								sprintf_s( szLabel, "_State%d", i + 1 );
								int nStateType = pPotentialSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
								if( nStateType == -1 ) break;

								sprintf_s( szLabel, "_State%dValue", i + 1 );
								char *szValue = pPotentialSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();

								CDnItem::CalcStateValue( &s_OptionState, nStateType, szValue, szValue, false );
							}
							s_OptionState.CalcValueType();
							if( m_itemSlotType != ST_STORE )
								SetItemStateText( &s_OptionState, dwFontColor, L"   ", &compareItemList, eSCC_NORMAL );
							else
								SetItemStateText( &s_OptionState, dwFontColor, NULL, &compareItemList, eSCC_NORMAL );
						}
					}
				}
			}
		}
	
		// 셋트아이템
		SetItemSetText( pItem, dwFontColor, bNameLink ); // 기존의 툴팁 세트 아이템 설명

		// 아이템 설명
		if( !bCompare || pItem->GetItemType() == ITEMTYPE_GLYPH
#if defined(PRE_ADD_TALISMAN_SYSTEM)
			|| pItem->GetItemType() == ITEMTYPE_TALISMAN
#endif
			)
		{
			std::wstring strTemp = pItem->GetDescription();
			if( !strTemp.empty() )
			{
				wszLastLine = m_pTextBox->GetLastLineText();
				if( wszLastLine.size() )
				{
					m_pTextBox->AddText( L"" );
					AddDivisionLine();
				}
				if( pItem->GetItemType() == ITEMTYPE_SOURCE )
				{
					UpdateSourceItemDescAndDurationTime( pItem );
				}
				else
				{
					m_pTextBox->AddColorText( pItem->GetDescription() );
				}
			}

#ifdef PRE_ADD_ITEM_GAINTABLE
			std::string rawString;
			CommonUtil::GetItemDescriptionInGainTable(rawString, pItem->GetClassID());
			if (rawString.empty() == false)
			{
				std::wstring converted;
				MakeUIStringItemGainDescription(converted, rawString);
				if (converted.empty() == false)
				{
					converted = FormatW(L"\n%s", converted.c_str());
					m_pTextBox->AddColorText(converted.c_str());
				}
			}
#endif
		}

#ifdef PRE_ADD_NEW_MONEY_SEED
		if( pItem->GetItemType() == ITEMTYPE_NEWMONEY_SEED )
		{
			std::wstring strTemp = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4979 ), pItem->GetTypeParam( 0 ) );
			if( !strTemp.empty() )
			{
				wszLastLine = m_pTextBox->GetLastLineText();
				if( wszLastLine.size() )
				{
					m_pTextBox->AddText( L"" );
					AddDivisionLine();
				}
				m_pTextBox->AddColorText( strTemp.c_str() );
			}
		}
#endif // PRE_ADD_NEW_MONEY_SEED

		// 수확된 농작물, 농장 현황판에 수확대기중인 것은 현재 남은 시간을 보여준다.
		if( ST_STORAGE_FARM == m_itemSlotType )
		{
			WCHAR strTemp[256];

			swprintf_s( strTemp, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7454 ), m_hSlotButton->GetExpire() );	////마감시간 제대로 계산 되면 수정해야함

			m_pTextBox->AddColorText( strTemp, textcolor::RED );	
		}

		// 마지막 라인이 비어있는 라인이라면 삭제
		wszLastLine = m_pTextBox->GetLastLineText();
		if( !wszLastLine.size() )
		{
			m_pTextBox->RemoveLastLine();
			RemoveLastDivisionLine();
		}
	}

#ifdef PRE_ADD_48682
	// 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화. // 아이템가격보다 위로 해달라는 기획요청.
	if( pItem->GetItemType() == ITEMTYPE_NORMAL &&
		pItem->GetSkillApplyType() == CDnItem::ItemSkillApplyType::ApplySkill )
		SetItemText_AllowMapType( pItem );
#endif

	// Note : 아이템 판매 가격
	CDnStoreTabDlg *pStoreDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
	if( pStoreDlg && pStoreDlg->IsShow() )
	{
		int nAmount(0);
		std::wstring strCoin;

		if( m_itemSlotType == ST_STORE )
		{
			nAmount = pItem->GetItemAmount() * pItem->GetOverlapCount();

			bool bStorePriceProcessed = false;
			if (pStoreDlg->IsShowRepurchasableTab())
			{
				SetItemText_RepurchaseSellPrice(pItem);
				bStorePriceProcessed = true;
			}

			if (bStorePriceProcessed == false)
			{
				if( Shop::Type::Combined <= pStoreDlg->GetStoreType() )
				{
					SetItemText_CombinedShop( pItem, pStoreDlg->GetCurrentTabID() );
				}
				else
				{
					if( nAmount > 0 || (pItem->GetNeedBuyItem() && pItem->GetNeedBuyItemCount()) || pItem->GetNeedBuyLadderPoint() || pItem->GetNeedBuyGuildWarPoint() || pItem->GetNeedBuyUnionPoint() )
					{
						std::wstring wszLastLine;
						wszLastLine = m_pTextBox->GetLastLineText();
						if( wszLastLine.size() )
						{
							m_pTextBox->AddText( L"" );
							AddDivisionLine();
						}

						if( pItem->GetNeedBuyItem() && pItem->GetNeedBuyItemCount() ) {
							std::wstring wszNeedItemName = CDnItem::GetItemFullName( pItem->GetNeedBuyItem() );
							swprintf_s( szTemp, 256, L"%s : %s %d%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1802 ), wszNeedItemName.c_str(), pItem->GetNeedBuyItemCount(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1728 ) );

							SUICoord StrCoord;
							CalcTextRect( szTemp, m_pTextBox->GetElement(0), StrCoord );
							if( StrCoord.fWidth > m_TextBoxDefaultCoord.fWidth )
							{
								swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1802 ) );
								m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
								swprintf_s( szTemp, 256, L"%s %d%s", wszNeedItemName.c_str(), pItem->GetNeedBuyItemCount(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1728 ) );
								m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
							}
							else
							{
								m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
							}
						}
						else if(pItem->GetNeedBuyLadderPoint())
						{
							swprintf_s( szTemp, 256, L"%s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2257 ), pItem->GetNeedBuyLadderPoint() , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126176 ));
							m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
						}
						else if(pItem->GetNeedBuyGuildWarPoint())
						{
							swprintf_s( szTemp, 256, L"%s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2257 ), pItem->GetNeedBuyGuildWarPoint() , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126084 ));
							m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
						}
						else if (pItem->GetNeedBuyUnionPoint())
						{
							swprintf_s( szTemp, 256, L"%s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2257 ), pItem->GetNeedBuyUnionPoint() , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3273 ));
							m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
						}

						// 자신의 인벤에 없는 새로운 종류의 중첩을 여러개 사고 툴팁이 표시될때,
						// 아래 GetOverlapCount의 값으로 산만큼의 수량이 들어가서 가격이 높아보이는 버그가 있었다.
						// 인벤에 새 아이템을 만들때 생성된 아이템의 포인터가 RefreshTooltip함수쪽으로 들어가면서 생긴 버그여서
						// 아이템을 만들때 RefreshTooltip를 호출하지 않도록 처리했다.
						//char strTemp[128];
						//sprintf(strTemp, "GetItemAmount : %d\n", pItem->GetItemAmount());
						//OutputDebug(strTemp);
						//sprintf(strTemp, "GetOverlapCount : %d\n", pItem->GetOverlapCount());
						//OutputDebug(strTemp);
						if( nAmount ) {
							DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount, strCoin );
							swprintf_s( szTemp, 256, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2257 ), strCoin.c_str() );
							m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );

							int storeBenefitAmount = nAmount;
							bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::BuyingPriceDiscount, storeBenefitAmount );
							if( bAvailBenefit )
							{
								DN_INTERFACE::UTIL::GetMoneyFormatUseStr( storeBenefitAmount, strCoin );
								swprintf_s( szTemp, 256, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3292 ), strCoin.c_str() ); // UISTRING : 멤버십 적용가
								m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
							}
						}
					}
				}
			}
		}
		else if( m_itemSlotType == ST_INVENTORY )
		{
			nAmount = pItem->GetItemSellAmount() * pItem->GetOverlapCount();
			if( nAmount > 0 )
			{
				std::wstring wszLastLine;
				wszLastLine = m_pTextBox->GetLastLineText();
				if( wszLastLine.size() )
				{
					m_pTextBox->AddText( L"" );
					AddDivisionLine();
				}
				bool bIsHaveMemberShip = GetQuestTask().CheckAndCalcStoreBenefit(NpcReputation::StoreBenefit::SellingPriceUp, nAmount);
				DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount, strCoin );
				swprintf_s( szTemp, 256, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2258 ), strCoin.c_str() );
				m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT );
#ifdef PRE_MOD_ALWAY_SEE_SELL_PRICE_TOOLTIP
				if(bIsHaveMemberShip)
					m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1767), textcolor::WHITE, UITEXT_RIGHT); // 멤버쉽 적용가
#endif
			}
#ifdef PRE_MOD_ALWAY_SEE_SELL_PRICE_TOOLTIP
			else
			{
				m_pTextBox->AddText( L"" );
				AddDivisionLine();
				m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2262), textcolor::FONT_ORANGE, UITEXT_RIGHT); // 판매불가
			}
#endif
		}
	}
#if defined(PRE_MOD_ALWAY_SEE_SELL_PRICE_TOOLTIP)
	else
	{	// Store가 열려있지 않을때만 처리한다.
		if( m_itemSlotType == ST_INVENTORY )//|| m_itemSlotType == ST_INVENTORY_CASH )
		{
			int nAmount(0);
			std::wstring strCoin;
			std::wstring wszLastLine;

			wszLastLine = m_pTextBox->GetLastLineText();
			if( !wszLastLine.empty() )
			{
				m_pTextBox->AddText(L"");
				AddDivisionLine();
			}

			nAmount = pItem->GetItemSellAmount() * pItem->GetOverlapCount();
			if(nAmount > 0)
			{
				bool bIsHaveMemberShip = GetQuestTask().CheckAndCalcStoreBenefit(NpcReputation::StoreBenefit::SellingPriceUp, nAmount);
				DN_INTERFACE::UTIL::GetMoneyFormatUseStr(nAmount, strCoin);
				swprintf_s( szTemp, 256, L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2258 ), strCoin.c_str() );
				m_pTextBox->AddText(szTemp, textcolor::FONT_ORANGE, UITEXT_RIGHT);
			
				if(bIsHaveMemberShip)
					m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1767), textcolor::WHITE, UITEXT_RIGHT); // 멤버쉽 적용가
			}
			else
			{
				m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2262), textcolor::FONT_ORANGE, UITEXT_RIGHT); // 판매불가
			}
		}
	}
#endif // PRE_MOD_ALWAY_SEE_SELL_PRICE_TOOLTIP
	SetSkillLevelUpItemInfo(pItem);

#ifdef PRE_ADD_CASHREMOVE
	// 인벤토리 캐시삭제탭에 속한 아이템의 툴팁에 삭제대기시간 출력.
	SetRemoveCashItemExpireTime( pItem );	
#endif
}

#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
void CDnTooltipDlg::SetEnchantEnableInfo()
{
	std::wstring temp = FormatW(L"[%s]", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8004)); // UISTRING : 강화능력
	m_pTextBox->AddText(temp.c_str());

	std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2216); // UISTRING : #w마을 대장장이에게서 #y[아이템 강화]#w로 강화 능력을 추가할 수 있다.
	if (str.empty() == false)
		m_pTextBox->AddColorText(str.c_str());
}

void CDnTooltipDlg::SetCompoundEnableInfo()
{
	std::wstring temp = FormatW(L"[%s]", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000023207)); // UISTRING : 장비능력
	m_pTextBox->AddText(temp.c_str(), descritioncolor::VIOLET);

	std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2215); // UISTRING : #w마을 대장장이에게서 #y[아이템 제작]#w으로 접미사 능력을 추가할 수 있다.
	if (str.empty() == false)
		m_pTextBox->AddColorText(str.c_str());
}

void CDnTooltipDlg::SetPotentialEnableInfo()
{
	std::wstring temp = FormatW(L"[%s]", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8005)); // UISTRING : 잠재능력
	m_pTextBox->AddText(temp.c_str());

	std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2217); // UISTRING : #y[코드 아이템]#w으로 잠재력 능력을 추가할 수 있다.
	if (str.empty() == false)
		m_pTextBox->AddColorText(str.c_str());
}
#endif

#ifdef PRE_ADD_CASHREMOVE
// 인벤토리 캐시삭제탭에 속한 아이템의 툴팁에 삭제대기시간 출력.
void CDnTooltipDlg::SetRemoveCashItemExpireTime( CDnItem * pItem )
{
	if( pItem->IsCashRemoveItem() )
	{
		__time64_t exTime = pItem->GetExpireTime();
		if( exTime )
		{	
			m_pTextBox->AppendText( L"\n" );
			
			std::wstring strUIString( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4886 ) );
			strUIString.append( L"\n\n" );

			wchar_t strTime[128] = {0,};
			__time64_t pastTime = CSyncTimer::GetInstance().GetCurTime() - m_tBegin; // 경과시간.
			exTime -= pastTime; // 남은대기시간.
			if( exTime > 0 )
			{				
				int _min = (int)((exTime % 3600) / 60);
				int _hour = (int)((exTime % 86400) / 3600);
				int _day = (int)(exTime / 86400);
				
				swprintf_s( strTime, 128, strUIString.c_str(), _day, _hour, _min );
			}
			else
			{
				swprintf_s( strTime, 128, strUIString.c_str(), 0, 0, 0 );				
			}

			m_pTextBox->AppendText( strTime, descritioncolor::RED );
		}

		// 공통툴팁.		
		m_pTextBox->AppendText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4887 ), descritioncolor::RED );
	}
}
#endif

void CDnTooltipDlg::SetItemStateText( CDnState *pState, DWORD dwFontColor, LPCWSTR szPrefix, std::vector<CDnItem*>* pCompareItemList, eStateCompareCategory cat )
{
	wchar_t szTemp[256]={0};
	std::wstring szCompareTemp;
	int idx = 0;
	CDnState* pCompareState = NULL; // temp by kalliste
	std::vector<CDnState*> compareStateList;
	if (pCompareItemList)
	{
		std::vector<CDnItem*>& compareItemList = *pCompareItemList;
		if (cat == eSCC_NORMAL)
		{
			for (idx = 0; idx < (int)pCompareItemList->size(); ++idx)
			{
				compareStateList.push_back(compareItemList[idx]);
			}
		}
		else if (cat == eSCC_ENCHANT)
		{
			for (idx = 0; idx < (int)pCompareItemList->size(); ++idx)
				compareStateList.push_back(compareItemList[idx]->GetEnchantState());
		}
		else if (cat == eSCC_POTENTIAL)
		{
			for (idx = 0; idx < (int)pCompareItemList->size(); ++idx)
				compareStateList.push_back(compareItemList[idx]->GetPotentialState());
		}
	}

	if( !pState )
	{
		ASSERT(pState&&"State가 없음");
		return;
	}

	//	m_pTextBox->AppendText(result.c_str(), textcolor::WHITE, UITEXT_RIGHT);

	//state 32
	if (pState->GetCashAttackPMin() > 0 && pState->GetCashAttackPMax() > 0)
	{
		swprintf_s(szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5018), pState->GetCashAttackPMin(), TILDE, pState->GetCashAttackPMax());
		SetStateText(szTemp, dwFontColor, szPrefix);

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCashAttackPMin() > 0 || pCompareState->GetCashAttackPMax() > 0)
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<int>(
						pCompareState->GetCashAttackPMin(),
						pState->GetCashAttackPMin(),
						pCompareState->GetCashAttackPMax(),
						pState->GetCashAttackPMax()));
			}
		}
	}

	//state 33
	if (pState->GetCashAttackMMin() > 0 && pState->GetCashAttackMMax() > 0)
	{
		swprintf_s(szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5019), pState->GetCashAttackMMin(), TILDE, pState->GetCashAttackMMax());
		SetStateText(szTemp, dwFontColor, szPrefix);

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCashAttackMMin() > 0 || pCompareState->GetCashAttackMMax() > 0)
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<int>(
						pCompareState->GetCashAttackMMin(),
						pState->GetCashAttackMMin(),
						pCompareState->GetCashAttackMMax(),
						pState->GetCashAttackMMax()));
			}
		}
	}

	// 공격력/방어력
	if( pState->GetAttackPMin() > 0 || pState->GetAttackPMax() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pState->GetAttackPMin(), TILDE, pState->GetAttackPMax() );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAttackPMin() > 0 || pCompareState->GetAttackPMax() > 0)
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetAttackPMin(),
						pState->GetAttackPMin(), 
						pCompareState->GetAttackPMax(), 
						pState->GetAttackPMax()));
			}
		}
	}
	if( pState->GetAttackMMin() > 0 || pState->GetAttackMMax() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pState->GetAttackMMin(), TILDE, pState->GetAttackMMax() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAttackMMin() > 0 || pCompareState->GetAttackMMax() > 0)
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetAttackMMin(),
																				pState->GetAttackMMin(), 
																				pCompareState->GetAttackMMax(),
																				pState->GetAttackMMax()));
			}
		}
	}

	if( pState->GetDefenseP() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pState->GetDefenseP() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetDefenseP() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetDefenseP(), pState->GetDefenseP()));
			}
		}
	}
	if( pState->GetDefenseM() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pState->GetDefenseM() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetDefenseM() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetDefenseM(), pState->GetDefenseM()));
			}
		}
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pState->GetStrength() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pState->GetStrength() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStrength() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStrength(), pState->GetStrength()));
			}
		}
	}
	if( pState->GetAgility() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pState->GetAgility() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAgility() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetAgility(), pState->GetAgility()));
			}
		}
	}
	if( pState->GetIntelligence() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pState->GetIntelligence() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetIntelligence() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetIntelligence(), pState->GetIntelligence()));
			}
		}
	}
	if( pState->GetStamina() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pState->GetStamina() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStamina() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStamina(), pState->GetStamina()));
			}
		}
	}
	if( pState->GetSpirit() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pState->GetSpirit() );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSpirit() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetSpirit(), pState->GetSpirit()));
			}
		}
	}

	int nTemp(0);

	// 내부 스탯 증가
	nTemp = pState->GetStiff();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStiff() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStiff(), nTemp));
			}
		}
	}

	nTemp = pState->GetStiffResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStiffResistance() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStiffResistance(), nTemp));
			}
		}
	}

	nTemp = pState->GetStun();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStun() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStun(), nTemp));
			}
		}
	}

	nTemp = pState->GetStunResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStunResistance() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetStunResistance(), nTemp));
			}
		}
	}

	nTemp = pState->GetCritical();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCritical() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetCritical(), nTemp));
			}
		}
	}

	nTemp = pState->GetCriticalResistance();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCriticalResistance() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetCriticalResistance(), nTemp));
			}
		}
	}


	nTemp = pState->GetMoveSpeed();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMoveSpeed() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetMoveSpeed(), nTemp));
			}
		}
	}

	nTemp = pState->GetSafeZoneMoveSpeed();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSafeZoneMoveSpeed() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetSafeZoneMoveSpeed(), nTemp));
			}
		}
	}

	nTemp = (int)pState->GetMaxHP();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );

		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMaxHP() > 0){
					int nMaxHP = (int)pCompareState->GetMaxHP();
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(nMaxHP, nTemp));
				}
			}
		}
	}

	nTemp = pState->GetMaxSP();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMaxSP() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetMaxSP(), nTemp));
			}
		}
	}

	nTemp = pState->GetRecoverySP();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetRecoverySP() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetRecoverySP(), nTemp));
			}
		}
	}

	// 슈퍼아머, 파이널데미지
	nTemp = pState->GetSuperAmmor();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSuperAmmor() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetSuperAmmor(), nTemp));
			}
		}
	}

	nTemp = pState->GetFinalDamage();
	if( nTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), nTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetFinalDamage() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<int>(pCompareState->GetFinalDamage(), nTemp));
			}
		}
	}

	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력
	if( pState->GetAttackPMinRatio() > 0 || pState->GetAttackPMaxRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%%s%.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pState->GetAttackPMinRatio() * 100.0f, TILDE, pState->GetAttackPMaxRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAttackPMinRatio() > 0 || pCompareState->GetAttackPMaxRatio() > 0)
				{
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetAttackPMinRatio() * 100.0f,
																					pState->GetAttackPMinRatio() * 100.0f, 
																					pCompareState->GetAttackPMaxRatio() * 100.0f, 
																					pState->GetAttackPMaxRatio() * 100.0f), true, 1);
				}
			}
		}
	}
	if( pState->GetAttackMMinRatio() > 0 || pState->GetAttackMMaxRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%%s%.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pState->GetAttackMMinRatio() * 100.0f, TILDE, pState->GetAttackMMaxRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAttackMMinRatio() > 0 || pCompareState->GetAttackMMaxRatio() > 0)
				{
					SetStateCompareText(eSCT_RANGE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetAttackMMinRatio() * 100.0f,
																					pState->GetAttackMMinRatio() * 100.0f, 
																					pCompareState->GetAttackMMaxRatio() * 100.0f, 
																					pState->GetAttackMMaxRatio() * 100.0f), true, 1);
				}
			}
		}
	}

	if( pState->GetDefensePRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pState->GetDefensePRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetDefensePRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetDefensePRatio() * 100.0f, pState->GetDefensePRatio() * 100.0f), true, 1);
			}
		}
	}
	if( pState->GetDefenseMRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pState->GetDefenseMRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetDefenseMRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetDefenseMRatio() * 100.0f, pState->GetDefenseMRatio() * 100.0f), true, 1);
			}
		}
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pState->GetStrengthRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pState->GetStrengthRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStrengthRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStrengthRatio() * 100.0f, pState->GetStrengthRatio() * 100.0f), true, 2);
			}
		}
	}
	if( pState->GetAgilityRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pState->GetAgilityRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAgilityRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetAgilityRatio() * 100.0f, pState->GetAgilityRatio() * 100.0f), true, 2);
			}
		}
	}
	if( pState->GetIntelligenceRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pState->GetIntelligenceRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetIntelligenceRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetIntelligenceRatio() * 100.0f, pState->GetIntelligenceRatio() * 100.0f), true, 2);
			}
		}
	}
	if( pState->GetStaminaRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pState->GetStaminaRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStaminaRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStaminaRatio() * 100.0f, pState->GetStaminaRatio() * 100.0f), true, 2);
			}
		}
	}
	if( pState->GetSpiritRatio() > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pState->GetSpiritRatio() * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSpiritRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetSpiritRatio() * 100.0f, pState->GetSpiritRatio() * 100.0f), true, 2);
			}
		}
	}

	float fTemp(0);

	// 내부 스탯 증가
	fTemp = pState->GetStiffRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStiffRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStiffRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetStiffResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStiffResistanceRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStiffResistanceRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetStunRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStunRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStunRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetStunResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetStunResistanceRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetStunResistanceRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetCriticalRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCriticalRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetCriticalRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetCriticalResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetCriticalResistanceRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetCriticalResistanceRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetMoveSpeedRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMoveSpeedRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetMoveSpeedRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetSafeZoneMoveSpeedRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSafeZoneMoveSpeedRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetSafeZoneMoveSpeedRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetMaxHPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMaxHPRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetMaxHPRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetMaxSPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetMaxSPRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetMaxSPRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetRecoverySPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetRecoverySPRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetRecoverySPRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	// 슈퍼아머, 파이널데미지
	fTemp = pState->GetSuperAmmorRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetSuperAmmorRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetSuperAmmorRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}

	fTemp = pState->GetFinalDamageRatio();
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), fTemp * 100.0f );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetFinalDamageRatio() > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetFinalDamageRatio() * 100.f, fTemp * 100.0f), true, 2);
			}
		}
	}



	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	//
	fTemp = ( pState->GetElementAttack( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_A ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementAttack( CDnState::Light ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementAttack( CDnState::Light ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementAttack( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_A ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementAttack( CDnState::Dark ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementAttack( CDnState::Dark ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementAttack( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_A ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementAttack( CDnState::Fire ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementAttack( CDnState::Fire ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementAttack( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_A ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementAttack( CDnState::Ice ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementAttack( CDnState::Ice ) * 100.f, fTemp), true, 2);
			}
		}
	}


	fTemp = ( pState->GetElementDefense( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_R ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementDefense( CDnState::Light ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementDefense( CDnState::Light ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementDefense( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_R ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementDefense( CDnState::Dark ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementDefense( CDnState::Dark ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementDefense( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_R ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementDefense( CDnState::Fire ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementDefense( CDnState::Fire ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = ( pState->GetElementDefense( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s : %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_R ), fTemp );
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetElementDefense( CDnState::Ice ) > 0)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetElementDefense( CDnState::Ice ) * 100.f, fTemp), true, 2);
			}
		}
	}

	fTemp = (pState->GetAddExp() * 100.0f);
	if( fTemp > 0 )
	{
		swprintf_s( szTemp, 256, L"%s %.2f%%", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ADDEXP ), fTemp );	// UIString 필요.
		SetStateText( szTemp, dwFontColor, szPrefix );
		szCompareTemp = szTemp;
		for (idx = 0; idx < (int)compareStateList.size(); ++idx)
		{
			CDnState* pCompareState = compareStateList[idx];
			if (pCompareState)
			{
				if (pCompareState->GetAddExp() > 0.0f)
					SetStateCompareText(eSCT_VALUE, szCompareTemp, SStateCompareUnit<float>(pCompareState->GetAddExp() * 100.0f, fTemp), true, 2);
			}
		}
	}




}

void CDnTooltipDlg::SetItemText_CostumeDesignMix(CDnItem* pItem)
{
	if (pItem->GetLookItemID() != ITEMCLSID_NONE)
	{
 		m_pTextBox->AddText( L"" );
 		AddDivisionLine();

		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130333), textcolor::RED, UITEXT_CENTER); // UISTRING : (합성된 아이템)
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130334), textcolor::RED);	// UISTRING : ※합성된 아이템은 디자인이 다를 수 있습니다.
	}
}

// 귀속, 파괴불가, 캐시 표시
void CDnTooltipDlg::SetItemText_ExchangeAttribute(CDnItem* pItem, bool bNameLink)
{
	enum eItemTextExAttrType
	{
		eITET_REVERSION,
		eITET_DESTRUCTION,
		eITET_CASH,
		eITET_MAX
	};
	std::wstring result;
	std::wstring temp[eITET_MAX];

	//szTempSub1[0] = szTempSub2[0] = '\0';

	if( pItem->GetReversion() != CDnItem::NoReversion && bNameLink == false)
	{
		if (pItem->IsSoulbBound())
		{
			CDnItem::ItemReversionEnum type = pItem->GetReversion();
			temp[eITET_REVERSION] = ITEM::GET_REVERSION_TYPE(type);
		}
		else
		{
			//if (pItem->GetSealCount() != 0)

			temp[eITET_REVERSION] = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4130);	// UISTRING : 봉인

			if(pItem->GetItemType() == ITEMTYPE_VEHICLE || pItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE || pItem->GetItemType() == ITEMTYPE_VEHICLEPARTS)
			{
				CDnItem::ItemReversionEnum type = pItem->GetReversion();
				temp[eITET_REVERSION] = ITEM::GET_REVERSION_TYPE(type);
			}
		}
	}

	if( !pItem->IsCanDestruction() )
	{
#if defined(PRE_ADD_CASHREMOVE)

		/*if( false == pItem->IsCashItem() 
			|| ( eItemTypeEnum::ITEMTYPE_VEHICLE == pItem->GetItemType() || eItemTypeEnum::ITEMTYPE_PET == pItem->GetItemType() ) )
			temp[eITET_DESTRUCTION] = ITEM::GET_STATIC(emTOOLTIP_STATIC::ITEM_UNDESTRUCTION);*/

		// #54067 [캐시삭제] 삭제 불가능한 캐시아이템 파괴불가 툴팁 추가 요청.
		bool bRes = false;
		int itemType = pItem->GetItemType();
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCASHERASABLETYPE );
		DNTableCell * pCell = NULL;
		if( pTable )
		{
			int size = pTable->GetItemCount();
			for( int i=0; i<size; ++i )
			{
				int idx = pTable->GetItemID( i );
				pCell = pTable->GetFieldFromLablePtr( idx, "_Type" );
				if( pCell )
				{ 
					if( itemType == pCell->GetInteger() )
					{
						bRes = true;
						break;
					}
				}
			}
		}

		if( bRes == false )
			temp[eITET_DESTRUCTION] = ITEM::GET_STATIC(emTOOLTIP_STATIC::ITEM_UNDESTRUCTION);

#else
		temp[eITET_DESTRUCTION] = ITEM::GET_STATIC(emTOOLTIP_STATIC::ITEM_UNDESTRUCTION);
#endif	// #ifdef PRE_ADD_CASHREMOVE
	}

	if (pItem->IsCashItem())
		temp[eITET_CASH] = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2114);	// UISTRING : 캐시

	int i = 0;
	for (; i < eITET_MAX; ++i)
	{
		if (temp[i].empty() == false)
		{
			if (result.empty() == false)
				result += L", ";
			result += temp[i];
		}
	}

	if (result.empty() == false)
		m_pTextBox->AddText( result.c_str(), textcolor::YELLOW, UITEXT_CENTER );
}

bool CDnTooltipDlg::SetItemText_CashAttribute(CDnItem* pItem, const SCashShopItemInfo& info)
{
	bool bAdded = false;
	std::wstring result;

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( info.bCashUsable == false )
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4969 ); // UISTRING : 캐쉬 구매 불가
		m_pTextBox->AddText( result.c_str(), textcolor::RED, UITEXT_NONE );
		bAdded = true;
	}
#endif // PRE_ADD_NEW_MONEY_SEED

	if (info.bReserveUsable == false)
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4716); // UISTRING : 적립금 구매 불가
		m_pTextBox->AddText(result.c_str(), textcolor::RED, UITEXT_NONE);
		bAdded = true;
	}

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( info.bSeedUsable == false )
	{
		result = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4970 ); // UISTRING : 시드 구매 불가
		m_pTextBox->AddText( result.c_str(), textcolor::RED, UITEXT_NONE );
		bAdded = true;
	}
#endif // PRE_ADD_NEW_MONEY_SEED

	if (info.bGiftUsable == false)
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4715); // UISTRING : 선물 불가
		m_pTextBox->AddText(result.c_str(), textcolor::RED, UITEXT_NONE);
		bAdded = true;
	}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) &&
		info.bRefundable == false)
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4805); // UISTRING : 청약철회 불가
		m_pTextBox->AddText(result.c_str(), textcolor::RED, UITEXT_NONE);
		bAdded = true;
	}
#endif

#ifdef PRE_ADD_CASHSHOP_CREDIT
	if( info.bCreditAble )
	{
		DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TCASHCOMMODITY);
		if( pSox )
		{
			int nNxCreditUsableMinLevel = pSox->GetFieldFromLablePtr( info.id, "_CreditAbleLV" )->GetInteger();
			result = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4841), nNxCreditUsableMinLevel );
			m_pTextBox->AddText( result.c_str(), textcolor::RED, UITEXT_NONE );
			bAdded = true;
		}
	}
#endif // PRE_ADD_CASHSHOP_CREDIT

	if (info.bLimit)
	{
		if(lstrlenW(GetCashShopTask().GetCashLimitItemStartDate(info.sn)) && lstrlenW(GetCashShopTask().GetCashLimitItemEndDate(info.sn))) // 기간이 제대로 설정되어있는지 확인.
		{
			result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4714), GetCashShopTask().GetCashLimitItemStartDate(info.sn), GetCashShopTask().GetCashLimitItemEndDate(info.sn)); // UISTRING : %s ~ %s 한정판매
			m_pTextBox->AddText(result.c_str(), textcolor::WHITE, UITEXT_NONE);
			bAdded = true;
		}
		if(GetCashShopTask().GetCashLimitBuyAbleCount(info.sn) > 0)
		{
			result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4846), GetCashShopTask().GetCashLimitBuyAbleCount(info.sn) ); // BuyAbleCount
			m_pTextBox->AddText(result.c_str(), textcolor::ORANGERED, UITEXT_NONE);
			bAdded = true;
		}
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	int nMaxCount = GetCashShopTask().GetCashLimitItemMaxCount( info.sn );
	if( info.bLimit && nMaxCount > 0 )
	{
		result = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4898), nMaxCount ); // UISTRING : 총 판매수량 : %d 개
		m_pTextBox->AddText( result.c_str(), textcolor::WHITE, UITEXT_NONE );
		bAdded = true;
	}
#endif // PRE_ADD_LIMITED_CASHITEM

	return bAdded;
}

void CDnTooltipDlg::SetItemText_ItemAttribute(CDnItem* pItem, bool bCompare)
{
	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};	// 보조
	DWORD dwFontColor = textcolor::WHITE;

	if (pItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	// 제한 레벨
	if( pItem->GetLevelLimit() > 1 )
	{
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2240 ), textcolor::FONT_ORANGE );

		swprintf_s( szTemp, 256, L" %d %s", pItem->GetLevelLimit(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );

		if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() < pItem->GetLevelLimit() )
		{
			dwFontColor = textcolor::RED;
		}

		m_pTextBox->AppendText( szTemp, dwFontColor );
		dwFontColor = bCompare ? textcolor::WHITE : textcolor::WHITE;
	}

	// 직업 제한
	SetItemText_NeedJob(pItem);

	// 아이템 세부 타입
	SetItemText_DetailType(pItem);
 
	// 아이템 등급
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2252 ), textcolor::FONT_ORANGE );
	swprintf_s( szTemp, 256, L" %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ITEM::RANK_2_INDEX( pItem->GetItemRank() ) ) );
	m_pTextBox->AppendText( szTemp, dwFontColor );

	SetItemText_Reversion(pItem);

	// 스킬북인 경우 지금 배운건지 아닌건지 확인...
	if( pItem->GetItemType() == ITEMTYPE_SKILLBOOK )
	{
		int iSkillBookItemID = pItem->GetClassID();
		//if( GetSkillTask().FindSkillBySkillBookItemID( iSkillBookItemID ) )
		if( GetSkillTask().FindZeroLevelSkillBySkillBookItemID( iSkillBookItemID ) || 
			GetSkillTask().FindSkillBySkillBookItemID( iSkillBookItemID ) )
		{
			// 이미 배움
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1746 ), textcolor::RED );
		}
		else
		{
			// 아직 배우지 않음.
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1747 ), textcolor::WHITE );
		}
	}

	// 재사용 시간. 스킬 가진 문장일때 뜨면 안되니 아이템 타입 체크한다.
	{
		if( pItem->GetSkillID() > 0 && ( pItem->GetItemType() == ITEMTYPE_NORMAL || pItem->GetItemType() == ITEMTYPE_COOKING ) && pItem->GetDelayTime() > 0 )
		{
			m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_COOLTIME ), textcolor::FONT_ORANGE );
			int nSec = (int)(pItem->GetDelayTime());
			swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2312 ), nSec );
			swprintf_s( szTemp, 256, L" %s", szTempSub1 );
			m_pTextBox->AppendText( szTemp, dwFontColor );
		}
	}

	SetItemText_UnableDisjointOrEnchant(pItem);
}

bool CDnTooltipDlg::SetItemText_CashItemAbility(CDnItem* pItem, const SCashShopItemInfo& info)
{
#ifdef PRE_ADD_VIP
	if (info.type != eCSType_Costume && info.type != eCSType_Term)
#else
	if (info.type != eCSType_Costume)
#endif
		return false;

	const std::vector<ITEMCLSID>& abilList = info.abilityList;
	if (abilList.empty())
		return false;

	std::vector<std::wstring> abilityStringList;
	std::vector<ITEMCLSID>::const_iterator iter = abilList.begin();
	for (; iter != abilList.end(); ++iter)
	{
		ITEMCLSID id = *iter;
		std::wstring strAbility = GetCashShopTask().GetAbilityString( id );
		if( strAbility.length() > 0 )
			abilityStringList.push_back( strAbility );
	}

	if (abilityStringList.empty() == false)
	{
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4717), textcolor::WHITE);	// UISTRING : 선택 가능 능력치
		std::vector<std::wstring>::const_iterator strIter = abilityStringList.begin();
		for (; strIter != abilityStringList.end(); ++strIter)
		{
			const std::wstring& str = *strIter;
			m_pTextBox->AddText(str.c_str());
		}

		return true;
	}

	return false;
}

void CDnTooltipDlg::SetItemText_CashItemDescription(CDnItem* pItem, const SCashShopItemInfo& info)
{
	std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, info.descId);
	if (str.empty() == false)
		m_pTextBox->AddColorText(str.c_str());	// 컬러 텍스트 지원하게 바꿔둘게요;


	//일단 여기에서는 아이템이 옵션 정보를 가지고 있지 않다.
	//캐시아이템에서 포텐션ID가 설정되어 있으면 레벨업 아이템? 으로 판단 하도록...
	//캐시아이템에서 포텐션ID가 설정된 경우가 있음..ㅠㅠ
	//포텐셜 리스트를 얻어서 실제 스킬레벨업 정보인지 확인한다..

	DNTableFileFormat* pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	DNTableFileFormat*  pSkillTable = GetTableDB().GetTable( CDnTableDB::TSKILL );
	if (pPotentialSox == NULL || pSkillTable == NULL)
		return;

	int nTypeParam = pItem->GetTypeParam(0);
	std::vector<int> nVecItemID;
	pPotentialSox->GetItemIDListFromField( "_PotentialID", nTypeParam, nVecItemID );

	if (nVecItemID.empty() == true)
		return;

	//선택 가능한 스킬 리스트 정보 추가
	std::vector<int>::const_iterator iter = nVecItemID.begin();
	
	int itemId = *iter;
	int nSkillID = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillID" )->GetInteger();
	int nSkillLevel = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillLevel" )->GetInteger();
	int nSkillUsingType = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillUsingType" )->GetInteger();

	//실제 스킬레벨업 아이템이 아니면 스킵..
	if (nSkillUsingType != CDnItem::ItemSkillApplyType::SkillLevelUp)
		return;

	m_pTextBox->AddText(L"");
	AddDivisionLine();

	//중복안됨 설명 추가
	m_pTextBox->AddColorText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4861), textcolor::RED);
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
bool CDnTooltipDlg::SetItemText_CashPackageItemContentsDesc(CDnItem* pItem, const SCashShopItemInfo& info)
{
	std::wstring pkgString;

	const std::vector<CASHITEM_SN>* pList = GetCashShopTask().GetInPackageItemList(info.sn);
	if (pList && pList->empty() == false)
	{
		std::vector<CASHITEM_SN>::const_iterator iter = pList->begin();
		for (; iter != pList->end(); ++iter)
		{
			const CASHITEM_SN& curItemSN = *iter;
			if (curItemSN > 0)
			{
				const SCashShopItemInfo* pCurInfo = GetCashShopTask().GetItemInfo(curItemSN);
				if (pCurInfo)
				{
					std::wstring dayStr, countStr;
					countStr = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625 ), pCurInfo->count); // UISTRING : %d 개
					if (pCurInfo->period > 0)
					{
						dayStr = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613 ), pCurInfo->period); // UISTRING : (%d일)
						pkgString = FormatW(L"%s%s", pCurInfo->nameString.c_str(), dayStr.c_str());
					}
					else
					{
						pkgString = FormatW(L"%s", pCurInfo->nameString.c_str());
					}
					m_pTextBox->AddText(pkgString.c_str());

					m_pTextBox->AppendText(countStr.c_str(), textcolor::WHITE, UITEXT_RIGHT);
				}
			}
		}
	}

	return true;
}
#endif

#if defined(PRE_ADD_SECONDARY_SKILL)
bool CDnTooltipDlg::SetItemText_SecondarySkill( SecondarySkill::SubType::eType eSecondarySkillSubType, const CDnItem* pItem )
{
	std::wstring wszString;
	WCHAR wszTemp[256];

	DNTableFileFormat* pSoxFile = NULL;
	int nSecondarySkillNameID = 0;

	switch( eSecondarySkillSubType )
	{
		case SecondarySkill::SubType::CultivationSkill:
			{
				pSoxFile = GetDNTable( CDnTableDB::TFARMCULTIVATE );
				nSecondarySkillNameID = 7301;
			}
			break;
		case SecondarySkill::SubType::CookingSkill:
			{
				pSoxFile = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
				nSecondarySkillNameID = 7300;
			}
			break;
		case SecondarySkill::SubType::FishingSkill:
			{
				pSoxFile = GetDNTable( CDnTableDB::TFISHINGROD );
				nSecondarySkillNameID = 7302;
			}
			break;
	}

	if( pSoxFile == NULL )
		return false;

	int nGrade = pSoxFile->GetFieldFromLablePtr( pItem->GetClassID(), "_SecondarySkillClass" )->GetInteger();
	int nLevel = pSoxFile->GetFieldFromLablePtr( pItem->GetClassID(), "_SecondarySkillLevel" )->GetInteger();

	//제한스킬
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7464 ), textcolor::FONT_ORANGE );
	swprintf_s( wszTemp, 256, L" : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSecondarySkillNameID ) );
	m_pTextBox->AppendText( wszTemp );

	//레벨제한
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2240 ), textcolor::FONT_ORANGE );		
	if( SecondarySkill::Grade::Beginning == nGrade )	//초급
		swprintf_s( wszTemp, 256, L" %s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7303 ), nLevel, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
	if( SecondarySkill::Grade::Intermediate == nGrade )	//중급
		swprintf_s( wszTemp, 256, L" %s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7304 ), nLevel, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
	if( SecondarySkill::Grade::Advanced == nGrade )		//고급
		swprintf_s( wszTemp, 256, L" %s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7305 ), nLevel, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );

	CSecondarySkill * pSecondarySkill = GetSkillTask().GetSecondarySkill( eSecondarySkillSubType );
	bool bEnablePlant = false;

	if( pSecondarySkill )
	{
		if( pSecondarySkill->GetGrade() < nGrade )
			bEnablePlant = false;
		else if( pSecondarySkill->GetGrade() == nGrade && pSecondarySkill->GetLevel() < nLevel )
			bEnablePlant = false;
		else
			bEnablePlant = true;
	}

	if( bEnablePlant )
		m_pTextBox->AppendText( wszTemp );
	else
		m_pTextBox->AppendText( wszTemp, textcolor::RED );

	if( eSecondarySkillSubType ==  SecondarySkill::SubType::CultivationSkill )	//타입
	{
		swprintf_s( wszTemp, 256, L" : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000018649 ) );	//  : 씨앗
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7009 ), textcolor::FONT_ORANGE );	// 타입
		m_pTextBox->AppendText( wszTemp );
	}

	return true;
}
#endif	//#if defined(PRE_ADD_SECONDARY_SKILL)

void CDnTooltipDlg::SetCashShopItemText(MIInventoryItem *pInvenItem, bool bCompare, bool bCashRewardItem)
{
	if( !CDnCashShopTask::IsActive() )
		return;

	ASSERT(pInvenItem&&"CDnTooltipDlg::SetItemText, pInvenItem==NULL!");

	if( pInvenItem->GetType() != MIInventoryItem::Item )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetItemText, 아이템이 아닙니다." );
		return;
	}

	CDnItem *pItem = static_cast<CDnItem*>(pInvenItem);
	if( !pItem ) return;

	if (pItem->IsCashItem() == false)
		return;
	const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(pItem->GetCashItemSN());
	if (pInfo == NULL)
		return;

	if( !CDnActor::s_hLocalActor ) return;

	//	Item Name
	wstring wszItemName = GetItemName( pItem );
	m_pStaticName->SetTextColor( ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
	m_pStaticName->SetText( wszItemName.c_str() );

	SetItemText_ExchangeAttribute(pItem);

	m_pTextBox->AddText(L"");
	AddDivisionLine();

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	bool bRefundSlot = (m_hSlotButton && m_hSlotButton->GetSlotType() == ST_INVENTORY_CASHSHOP_REFUND);
	const SRefundCashItemInfo* pRefundInfo = bRefundSlot ? CDnCashShopTask::GetInstance().GetRefundItemInfo(pItem->GetSerialID()) : NULL;

	if (bRefundSlot && pRefundInfo)
	{
		std::wstring refndCondstr;
		if (CDnCashShopTask::GetInstance().IsItemPermanent(pItem->GetCashItemSN()) == false)
		{
			refndCondstr = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4801 ), pInfo->period);	// UISTRING : 아이템 이동 이후 %d일간 사용
			m_pTextBox->AddText(refndCondstr.c_str());
		}

		if (pInfo->bReserveOffer)
		{
#ifdef PRE_MOD_PETAL_WRITE
			refndCondstr = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030008 ), DN_INTERFACE::UTIL::GetAddCommaString( pInfo->ReserveAmount ).c_str() ); // UISTRING : 아이템 이동 이후 %s 페탈 적립
#else // PRE_MOD_PETAL_WRITE
			refndCondstr = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4802 ), pInfo->ReserveAmount ); // UISTRING : 아이템 이동 이후 %d 페탈 적립
#endif // PRE_MOD_PETAL_WRITE
			m_pTextBox->AddText(refndCondstr.c_str());
		}

		m_pTextBox->AddText(pRefundInfo->paidDateStr.c_str());

		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}
	else
	{
		if (bCashRewardItem == false && SetItemText_CashAttribute(pItem, *pInfo))
		{
			m_pTextBox->AddText(L"");
			AddDivisionLine();
		}
	}
#else
	if (bCashRewardItem == false && SetItemText_CashAttribute(pItem, *pInfo))
	{
		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}
#endif

	SetItemText_ItemAttribute(pItem, bCompare);

	m_pTextBox->AddText(L"");
	AddDivisionLine();

	if (pInfo->type == eCSType_ePackage)
	{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
		m_pStaticName->SetText(pInfo->nameString.c_str());
		if (bRefundSlot && pRefundInfo && pRefundInfo->bPackage)
		{
			std::wstring tempStr;

			const std::vector<SRefundCashItemUnit>& contentsList = pRefundInfo->packageContents;
			std::vector<SRefundCashItemUnit>::const_iterator contentsIter = contentsList.begin();
			for (; contentsIter != contentsList.end(); ++contentsIter)
			{
				const SRefundCashItemUnit& unit = *contentsIter;
				const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(unit.sn);
				if (pInfo)
				{
// 					std::wstring date = (pInfo->period < 0) ? GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4705) :
// 							FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4640), pInfo->period); // UISTRING : 무제한 / UISTRING : %d일
// 					tempStr = FormatW(L"%s(%s)", pInfo->nameString.c_str(), date.c_str());
					m_pTextBox->AddText(pInfo->nameString.c_str());

					tempStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4625), pInfo->count);	// UISTRING : %d 개
					m_pTextBox->AppendText(tempStr.c_str(), textcolor::WHITE, UITEXT_RIGHT);

					std::wstring strAbility = GetCashShopTask().GetAbilityString( unit.ability );
					if( strAbility.length() > 0 )
					{
						tempStr = FormatW( L"-%s", strAbility.c_str() );
						m_pTextBox->AddText( tempStr.c_str() );
					}
				}
			}
		}
		else
		{
			SetItemText_CashPackageItemContentsDesc(pItem, *pInfo);
		}
		m_pTextBox->AddText(L"");
		AddDivisionLine();
#else
		std::wstring pkgString;
		m_pStaticName->SetText(pInfo->nameString.c_str());

		const std::vector<CASHITEM_SN>* pList = GetCashShopTask().GetInPackageItemList(pInfo->sn);
		if (pList && pList->empty() == false)
		{
			std::vector<CASHITEM_SN>::const_iterator iter = pList->begin();
			for (; iter != pList->end(); ++iter)
			{
				const CASHITEM_SN& curItemSN = *iter;
				if (curItemSN > 0)
				{
					const SCashShopItemInfo* pCurInfo = GetCashShopTask().GetItemInfo(curItemSN);
					if (pCurInfo)
					{
						std::wstring dayStr, countStr;
						countStr = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625 ), pCurInfo->count); // UISTRING : %d 개
						if (pCurInfo->period > 0)
						{
							dayStr = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613 ), pCurInfo->period); // UISTRING : (%d일)
							pkgString = FormatW(L"%s%s", pCurInfo->nameString.c_str(), dayStr.c_str());
						}
						else
						{
							pkgString = FormatW(L"%s", pCurInfo->nameString.c_str());
						}
						m_pTextBox->AddText(pkgString.c_str());

						m_pTextBox->AppendText(countStr.c_str(), textcolor::WHITE, UITEXT_RIGHT);
					}
				}
			}
		}

		m_pTextBox->AddText(L"");
		AddDivisionLine();

		if (bCashRewardItem == false && pInfo->price > 0 && pInfo->priceFix > 0)
		{
#ifdef PRE_MOD_PETAL_WRITE
			pkgString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030010 ), DN_INTERFACE::UTIL::GetAddCommaString( pInfo->priceFix ).c_str() );	// UISTRING : 상품가격 %s원
			m_pTextBox->AddText( pkgString.c_str());

			pkgString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030009 ), DN_INTERFACE::UTIL::GetAddCommaString( pInfo->price ).c_str() );	// UISTRING : 판매가격 %s원
			m_pTextBox->AddText( pkgString.c_str() );
#else // PRE_MOD_PETAL_WRITE
			pkgString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4710 ), pInfo->priceFix );	// UISTRING : 상품가격 %d원
			m_pTextBox->AddText( pkgString.c_str());

			pkgString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4719 ), pInfo->price );	// UISTRING : 판매가격 %d원
			m_pTextBox->AddText( pkgString.c_str() );
#endif // PRE_MOD_PETAL_WRITE
			int saleRate = GetCashShopTask().GetCashShopDiscountRate((float)pInfo->price, (float)pInfo->priceFix, (float)pInfo->ReserveAmount);
			pkgString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4718), saleRate);	// UISTRING : %d%% 할인
			m_pTextBox->AddText(pkgString.c_str(), textcolor::RED);

			m_pTextBox->AddText(L"");
			AddDivisionLine();
		}
#endif // PRE_ADD_CASHSHOP_REFUND_CL

		SetItemText_CashItemDescription(pItem, *pInfo);


#ifdef PRE_ADD_CASHSHOP_ACTOZ
		// 캐시샾에서 청약철회 약관 출력.
		SetItemText_CashRefund( pItem );
#endif // PRE_ADD_CASHSHOP_ACTOZ

		return;
	}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	if (bRefundSlot && pRefundInfo && pRefundInfo->bPackage == false)
	{
		if (pRefundInfo->ability != ITEMCLSID_NONE)
		{
			std::wstring strAbility = GetCashShopTask().GetAbilityString( pRefundInfo->ability );
			if( strAbility.length() > 0 )
			{
				m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4804 ) ); // UISTRING : 선택한 능력치

				std::wstring tempStr = FormatW( L"-%s", strAbility.c_str() );
				m_pTextBox->AddText( tempStr.c_str() );

				m_pTextBox->AddText( L"" );
				AddDivisionLine();
			}
		}
	}
	else
	{
		if (SetItemText_CashItemAbility(pItem, *pInfo))
		{
			m_pTextBox->AddText(L"");
			AddDivisionLine();
		}
	}
#else
	if (SetItemText_CashItemAbility(pItem, *pInfo))
	{
		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}
#endif

	SetItemText_CashItem_SkillLevelupInfo( pItem );

	if( bCashRewardItem )
	{
		SetItemStateText( pItem, textcolor::WHITE );
	}

	// 셋트아이템
	int nSetItemID = 0;
	switch( pItem->GetItemType() ) 
	{
	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
			if (pWeapon)
				nSetItemID = pWeapon->GetSetItemID();
		}
		break;
	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
			if (pParts)
				nSetItemID = pParts->GetSetItemID();
		}
		break;
	case ITEMTYPE_PETPARTS:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
			if( !pSox ) return;
			if( !pSox->IsExistItem( pItem->GetClassID() ) ) return;

			nSetItemID = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_SetItemID" )->GetInteger();
		}
		break;
	}
	const std::vector<ITEMCLSID>& abilList = pInfo->abilityList;
	bool bEqualSetItemID = true;
	if( nSetItemID > 0 && !abilList.empty() )
	{
		for( int i=0; i<static_cast<int>( abilList.size() ); i++ )
		{
			CDnItem* pFindItem = NULL;
			TItemInfo itemInfo;
			if( CDnItem::MakeItemInfo( abilList[i], 1, itemInfo ) )
			{
				pFindItem = GetItemTask().CreateItem( itemInfo );
			}

			if( pFindItem == NULL )
				continue;

			if( pFindItem )
			{
				switch( pFindItem->GetItemType() ) 
				{
				case ITEMTYPE_WEAPON:
					{
						CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pFindItem);
						if( nSetItemID != pWeapon->GetSetItemID() )
							bEqualSetItemID = false;
					}
					break;
				case ITEMTYPE_PARTS:
					{
						CDnParts *pParts = dynamic_cast<CDnParts *>(pFindItem);
						if( nSetItemID != pParts->GetSetItemID() )
							bEqualSetItemID = false;
					}
					break;
				case ITEMTYPE_PETPARTS:
					{
						DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
						if( !pSox ) return;
						if( !pSox->IsExistItem( pFindItem->GetClassID() ) ) return;

						if( nSetItemID != pSox->GetFieldFromLablePtr( pFindItem->GetClassID(), "_SetItemID" )->GetInteger() )
							bEqualSetItemID = false;
					}
					break;
				}

				SAFE_DELETE( pFindItem );
			}

			if( !bEqualSetItemID )
				break;
		}
	}

	if( nSetItemID > 0 )
	{
		if( bEqualSetItemID )
		{
			SetItemSetText( pItem, textcolor::WHITE ); // 캐시샵에서 사용하는 셋트 아이템 툴팁..
		}
		else
		{
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2281 ) );
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4877 ) );
		}

		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}

	SetItemText_CashItemDescription(pItem, *pInfo);


#ifdef PRE_ADD_48682
	// 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화. // 캐시샾에서도 자동툴팁 적용.
	if( pItem->GetItemType() == ITEMTYPE_NORMAL &&
		pItem->GetSkillApplyType() == CDnItem::ItemSkillApplyType::ApplySkill )
		SetItemText_AllowMapType( pItem );
#endif

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	// 캐시샾에서 청약철회 약관 출력.
	SetItemText_CashRefund( pItem );
#endif // PRE_ADD_CASHSHOP_ACTOZ

}

void CDnTooltipDlg::SetStateText( LPCWSTR szText, DWORD dwFontColor, LPCWSTR szPrefix )
{
	if( !szPrefix )
	{
		m_pTextBox->AddText( szText, dwFontColor );
	}
	else
	{
		wchar_t szTemp[256]={0};
		swprintf_s( szTemp, 256, L"%s%s", szPrefix, szText );
		m_pTextBox->AddText( szTemp, dwFontColor );
	}
}

void CDnTooltipDlg::SetItemSetText( CDnItem *pItem, DWORD dwFontColor, bool bNameLink /*= false*/)
{	// 세트 아이템 효과 및 설명 적는곳
	int		nSetItemID = 0;
	bool	bSimpleDescription = false;
	int		nSimpleDescriptionID = 4551;
	bool	bPetParts = false;

	switch( pItem->GetItemType() ) 
	{
		case ITEMTYPE_WEAPON:
			{
				CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
				if( pWeapon )
				{
					nSetItemID = pWeapon->GetSetItemID();
					bSimpleDescription = pWeapon->IsSimpleSetItemDescription();
					nSimpleDescriptionID = pWeapon->GetSimpleSetItemDescriptionID();
				}
			}
			break;
		case ITEMTYPE_PARTS:
			{
				CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
				if( pParts )
				{
					nSetItemID = pParts->GetSetItemID();
					bSimpleDescription = pParts->IsSimpleSetItemDescription();
					nSimpleDescriptionID = pParts->GetSimpleSetItemDescriptionID();
				}
			}
			break;
		case ITEMTYPE_PETPARTS:
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
				if( !pSox ) return;
				if( !pSox->IsExistItem( pItem->GetClassID() ) ) return;

				nSetItemID = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_SetItemID" )->GetInteger();
				bSimpleDescription = ( pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_Grade_SetItem" )->GetInteger() == 1 ) ? true : false;
				nSimpleDescriptionID = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_SetItemTextID" )->GetInteger();
				bPetParts = true;
			}
			break;
	}

	if( !nSetItemID )
		return;

	wchar_t szTemp[256]={0};

	// 세트아이템 전체 개수 구하기
	int nMaxSetItem = 0;
	int nMaxWeaponSetItem = 0;
	int nMaxPartsSetItem = 0;
	vector<int> vWeaponSetItemIDList;
	vector<int> vPartsSetItemIDList;
	vector<int> vSetItemIDList;
	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	pSox->GetItemIDListFromField( "_SetItemID", nSetItemID, vWeaponSetItemIDList );
	nMaxWeaponSetItem = (int)vWeaponSetItemIDList.size();
	
	pSox = GetDNTable( CDnTableDB::TPARTS );
	pSox->GetItemIDListFromField( "_SetItemID", nSetItemID, vPartsSetItemIDList );
	nMaxPartsSetItem = (int)vPartsSetItemIDList.size();
	nMaxSetItem = nMaxWeaponSetItem + nMaxPartsSetItem;
	
	if( bPetParts )
	{
		nMaxSetItem = 2;
	}

	if( nMaxSetItem <= 0 )
	{
		ASSERT(0&&"세트아이템 총 갯수가 0이하입니다. 데이터가 이상합니다.");
	}

	// 세트아이템 리스트 구해놓고
	vSetItemIDList = vPartsSetItemIDList;
	for( int i = 0; i < (int)vWeaponSetItemIDList.size(); ++i )
		vSetItemIDList.push_back(vWeaponSetItemIDList[i]);

	// 장착된 템 중 현재 세트아이템에 해당되는 개수 구한다.
	int nNumEquipedSetItem = 0;
	vector<int> vEquipedItemIDList;

	// 만약 살펴보기 중이면서 기본툴팁 윈도우라면, 상대방 장착 리스트를 얻어온다.
	// 그 외 경우엔 원래대로 자신의 장비를 찍는다.
	//if( GetDialogType() == UI_TYPE_TOP )	// 이렇게 기본툴팁인지 검사해도 된다.
	if( this != m_pCompareTooltipDlg && this != m_pCompareTooltipDlg2 && m_hSlotButton && m_hSlotButton->GetSlotType() == ST_INSPECT )
	{
		if( GetInterface().GetInspectDialog() )
		{
			std::vector<CDnInspectPlayerDlg::SEquipInfo> vecEquipInfo;
			GetInterface().GetInspectDialog()->GetEquipInfo(vecEquipInfo);

			for( int i = 0; i < (int)vecEquipInfo.size(); ++i )
			{
				vEquipedItemIDList.push_back(vecEquipInfo[i].nClassID);
				if( vecEquipInfo[i].nSetItemID == nSetItemID ) ++nNumEquipedSetItem;
			}
		}
	}
	else
	{
		for (int i = 2; i < EQUIPMAX; ++i)
		{
			CDnItem *pEquipedItem = GetItemTask().GetEquipItem(i);
			if( !pEquipedItem ) continue;
			if( pEquipedItem->GetItemType() == ITEMTYPE_WEAPON )
			{
				CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pEquipedItem);
				vEquipedItemIDList.push_back(pWeapon->GetClassID());
				if( pWeapon->GetSetItemID() == nSetItemID ) ++nNumEquipedSetItem;
			}
			else if( pEquipedItem->GetItemType() == ITEMTYPE_PARTS )
			{
				CDnParts *pParts = dynamic_cast<CDnParts *>(pEquipedItem);
				vEquipedItemIDList.push_back(pParts->GetClassID());
				if( pParts->GetSetItemID() == nSetItemID ) ++nNumEquipedSetItem;
			}
		}

		// 캐쉬템도 체크 
		for (int i = 0; i < CASHEQUIPMAX; ++i)
		{
			if(i==CASHEQUIP_EFFECT)
				continue;

			CDnItem *pEquipedItem = GetItemTask().GetCashEquipItem(i);
			if( !pEquipedItem ) continue;
			if( pEquipedItem->GetItemType() == ITEMTYPE_WEAPON )
			{
				CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pEquipedItem);
				vEquipedItemIDList.push_back(pWeapon->GetClassID());
				if( pWeapon->GetSetItemID() == nSetItemID ) ++nNumEquipedSetItem;
			}
			else if( pEquipedItem->GetItemType() == ITEMTYPE_PARTS )
			{
				CDnParts *pParts = dynamic_cast<CDnParts *>(pEquipedItem);
				vEquipedItemIDList.push_back(pParts->GetClassID());
				if( pParts->GetSetItemID() == nSetItemID ) ++nNumEquipedSetItem;
			}
		}

		if( CDnActor::s_hLocalActor && bPetParts && m_itemSlotType == ST_CHARSTATUS )
		{
			CDnPlayerActor* pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();

			if( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nItemID &&
				pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nItemID )
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
				if( !pSox ) return;
				if( !pSox->IsExistItem( pItem->GetClassID() ) ) return;

				int nSetItemID1 = pSox->GetFieldFromLablePtr( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nItemID, "_SetItemID" )->GetInteger();
				int nSetItemID2 = pSox->GetFieldFromLablePtr( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nItemID, "_SetItemID" )->GetInteger();
				if( nSetItemID1 > 0 && nSetItemID2 > 0 && nSetItemID1 == nSetItemID2 )
					nNumEquipedSetItem = 2;
			}
		}
	}

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	// Mid: 세트 아이템
	swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2281 ) );
	m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	//m_pTextBox->AddText( szTemp, dwFontColor );

	// 세트 아이템 "설명" 출력
	if( bSimpleDescription ) 
	{
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSimpleDescriptionID ), dwFontColor );
	}
	else 
	{
		// 세트아이템 리스트
		std::wstring wszItemName;
		for( int i = 0; i < (int)vSetItemIDList.size(); ++i )
		{
			int nItemID = vSetItemIDList[i];
			wszItemName = CDnItem::GetItemFullName( nItemID );

			bool bEquiped = false;
			for( int j = 0; j < (int)vEquipedItemIDList.size(); ++j )
			{
				if( vEquipedItemIDList[j] == nItemID ) 
					bEquiped = true;
			}

			m_pTextBox->AddText( wszItemName.c_str(), bEquiped ? dwFontColor : textcolor::DARK );
		}
	}

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	// 이제 셋트 아이템 효과는 기존의 툴팁에서 사라지고, 새로운 툴팁에 모아서 보여준다.
	// 채팅창에 링크한 툴팁은 기존의 방식으로 보여준다.	
	if(pItem->IsCashItem())
	{
		if(bNameLink == false)
		{
			return;
		}
	}
#endif

	// 세트아이템 "효과"
	m_pTextBox->AddText( L"" );
	swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2282 ) ); // Mid: 세트효과
	m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE);
	//m_pTextBox->AddText( szTemp, dwFontColor );

	pSox = GetDNTable( CDnTableDB::TSETITEM );
	char szLabel[32];
	int  nNumNeedSet;

	// 먼저 필요갯수에 따라 효과들을 묶는다.(물리공격력 Min, Max때문에.)
	// 인덱스는 필요셋갯수 이며 최대치는 10개. 0번인덱스, 1번인덱스는 사용하지 않는다.(1개짜리 세트아이템은 없으니)
	std::vector<int> nVecStateList[11];

	// 세트아이템에 붙는 효과는 최대 12개.
	for( int i=0; i<12; i++ )
	{
		sprintf_s( szLabel, "_NeedSetNum%d", i+1 );
		nNumNeedSet = pSox->GetFieldFromLablePtr( nSetItemID, szLabel )->GetInteger();
		if( nNumNeedSet < 2 ) continue;
		nVecStateList[nNumNeedSet].push_back(i);	// 효과 인덱스 기억.
	}

#ifdef PRE_ADD_COSTUME_SKILL
	DNTableFileFormat *pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	if( !pSkillTable ) return;
	int nSkillNeedCount = pSox->GetFieldFromLablePtr( nSetItemID, "_NeedCSSetNum" )->GetInteger();
	int nSkillID = pSox->GetFieldFromLablePtr( nSetItemID, "_SkillID" )->GetInteger();
#endif

	// 필요셋개수대로 출력
	CDnState SetItemState;
	for( int i=0; i<11; i++ )
	{
		if( i < 2 || nVecStateList[i].size() == 0 ) continue;
		SetItemState.ResetState();

		for( int j=0; j<(int)nVecStateList[i].size(); ++j )
		{
			sprintf_s( szLabel, "_State%d", nVecStateList[i][j]+1 );
			int nStateType = pSox->GetFieldFromLablePtr( nSetItemID, szLabel )->GetInteger();
			sprintf_s( szLabel, "_State%dValue", nVecStateList[i][j]+1 );
			char *szValue = pSox->GetFieldFromLablePtr( nSetItemID, szLabel )->GetString();
			CDnItem::CalcStateValue( &SetItemState, nStateType, szValue, szValue, false );
		}
		swprintf_s( szTemp, 256, L" %d%s ", i, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2298 ) );
		SetItemStateText( &SetItemState, (i <= nNumEquipedSetItem) ? dwFontColor : textcolor::DARK, szTemp );
#ifdef PRE_ADD_COSTUME_SKILL
		if( nSkillID > 0 && i == nSkillNeedCount )
		{
			int nSkillStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
			std::wstring skillName = FormatW( L": %s" , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSkillStringID ) );
			SetStateText( skillName.c_str(), (i <= nNumEquipedSetItem) ? dwFontColor : textcolor::DARK, szTemp );
		}
#endif
	}

#ifdef PRE_ADD_COSTUME_SKILL
	if( nSkillID > 0  )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8357 ) , textcolor::YELLOW );
	}
#endif // PRE_ADD_COSTUME_SKILL

}

void CDnTooltipDlg::SetCharmItemText( CDnItem *pItem )
{
	std::vector<int> vecKeyID;
	pItem->GetCharmItemKeyID( vecKeyID );
	if( vecKeyID.empty() ) return;

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	wchar_t szTemp[256] = {0,};
	swprintf_s( szTemp, 256, L"[%s]", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7055 ) );
	m_pTextBox->AddText( szTemp, textcolor::WHITE );

	for( int i = 0; i < (int)vecKeyID.size(); ++i )
	{
		swprintf_s( szTemp, 256, L" %s(%s)", CDnItem::GetItemFullName( vecKeyID[i] ).c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CDnItem::IsCashItem( vecKeyID[i] ) ? 2114 : 501 ) );
		m_pTextBox->AddText( szTemp, textcolor::WHITE );
	}
}

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
void CDnTooltipDlg::SetRandomCharmItemText( CDnItem* pItem )
{
	// RandomCharmItem 추가 툴팁들은 여기에서..

	if( ITEMTYPE_CHARMRANDOM != pItem->GetItemType() ) 
		return;

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	TItemInfo iteminfo;
	pItem->GetTItemInfo( iteminfo );
	int nItemID = iteminfo.Item.nItemID;
	
	// 아이템 정보를 읽어와서 
	DNTableFileFormat* pSox  = GetDNTable( CDnTableDB::TITEM );
	if( !pSox || !pSox->IsExistItem(nItemID) )
		return;

	WCHAR wszTemp[128] = { 0, };

	int nCharmItemID = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam3" )->GetInteger();

	DNTableFileFormat* pSoxCharm  = GetDNTable( CDnTableDB::TCHARMCOUNT );
	if( !pSoxCharm || !pSoxCharm->IsExistItem(nCharmItemID) )
		return;

	int nMin = pSoxCharm->GetFieldFromLablePtr( nCharmItemID, "_CharmCount_Min" )->GetInteger();
	int nMax = pSoxCharm->GetFieldFromLablePtr( nCharmItemID, "_CharmCount_Max" )->GetInteger();

	swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000048776 ), nMin, nMax );

	m_pTextBox->AddColorText( wszTemp,  textcolor::DODGERBLUE );
}

#endif

void CDnTooltipDlg::SetPackageItemText( CDnItem *pItem )
{
	if( ITEMTYPE_ALLGIVECHARM != pItem->GetItemType() ) 
		return;

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	std::vector<boost::tuple<int, int, int>> vPackageItem;
#ifdef PRE_FIX_MAKECHARMITEM
	GetItemTask().MakeCharmItemList( vPackageItem, 0, 0, 0, pItem );
#else
	GetItemTask().MakeCharmItemList( vPackageItem, 0, 0, pItem );
#endif

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );

	std::vector<int> nVecJobList;
	if( CDnActor::s_hLocalActor )
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
	//((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );

	for( DWORD itr = 0; itr < vPackageItem.size(); ++itr )
	{
		if( false == CDnItem::IsPermitPlayer( vPackageItem[itr].get<0>(), nVecJobList ) )
			continue;

		WCHAR wszCount[64] = {0,};
		swprintf_s( wszCount, _countof(wszCount), L"%d", vPackageItem[itr].get<1>() );

		if( 0 != vPackageItem[itr].get<2>() )
		{
			WCHAR wszDay[64] = {0,};
			WCHAR wszName[256] = {0,};
			swprintf_s( wszDay, _countof(wszDay), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1916), vPackageItem[itr].get<2>() );	// UISTRING : %d일
			swprintf_s( wszName, _countof(wszName), L"%s (%s)", CDnItem::GetItemFullName( vPackageItem[itr].get<0>() ).c_str(), wszDay );

			m_pTextBox->AddText( wszName );
		}
		else
			m_pTextBox->AddText( CDnItem::GetItemFullName( vPackageItem[itr].get<0>() ).c_str() );

		m_pTextBox->AppendText( wszCount, textcolor::WHITE, UITEXT_RIGHT );
	}
}

void CDnTooltipDlg::SetAppellationItemText( CDnItem *pItem )
{
	if( !CDnAppellationTask::IsActive() ) return;
	tstring wszStr = CDnAppellationTask::GetInstance().MakeDescription( pItem->GetTypeParam(0) - 1, CDnAppellationTask::EFFECT_DESC );

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}
	
	m_pTextBox->AddColorText( FormatW( L"[%s]", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000040573 ) ).c_str(), textcolor::DODGERBLUE ); // "칭호능력"
	m_pTextBox->AddColorText( wszStr.c_str(), textcolor::WHITE );
}

void CDnTooltipDlg::SetSkillText( MIInventoryItem *pInvenItem )
{
	ASSERT(pInvenItem&&"CDnTooltipDlg::SetSkillText, pInvenItem==NULL!");

	if( pInvenItem->GetType() != MIInventoryItem::Skill )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetItemText, 스킬이 아닙니다." );
		return;
	}

	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};
	DWORD dwFontColor(textcolor::WHITE);

	CDnSkill *pSkill = static_cast<CDnSkill*>(pInvenItem);
	if( !pSkill ) return;
	if( !CDnActor::s_hLocalActor ) return;

	DNTableFileFormat*  pSkillLevelTable = NULL;
	
	if( !GetSkillTask().IsGuildWarSkill( pSkill->GetClassID() ) )
		pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TSKILLLEVEL );
	else
		pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
	if( !pSkillLevelTable )
		return;

#if defined(PRE_FIX_NEXTSKILLINFO)
#else
	const CDnSkill::SkillInfo *pSkillInfo = pSkill->GetInfo();
#endif // PRE_FIX_NEXTSKILLINFO

	// pvp모드에서 댔을때는 먼저 pvp툴팁이 보여야하므로, 현재 설정된 값을 알아와 설정한다.
	int nCurSelectedLevelDataType = pSkill->GetSelectedLevelDataType();
	bool bPVPTooltip = m_bTooltipSwitch;

	if( GetSkillTask().IsGuildWarSkill( pInvenItem->GetClassID() ) )
		bPVPTooltip = true;

	if( CDnPetTask::IsActive() && GetPetTask().IsPetSkill( pInvenItem->GetClassID() ) )
		bPVPTooltip = false;

	if( bPVPTooltip )
		pSkill->SelectLevelDataType( CDnSkill::PVP, true );
	else
		pSkill->SelectLevelDataType( CDnSkill::PVE, true );

	// 스킬 이름
	if( !bPVPTooltip )
	{
		swprintf_s( szTemp, 256, L"%s", pSkill->GetName() );
		m_pStaticName->SetTextColor( textcolor::WHITE );
	}
	else
	{
		wstring wszSkillName = pSkill->GetName();
		SUICoord NameCoord, LineCoord;
		CalcTextRect( L" ", m_pStaticName->GetElement(0), LineCoord );
		CalcTextRect( wszSkillName.c_str(), m_pStaticName->GetElement(0), NameCoord );
		if( NameCoord.fWidth < m_TextBoxDefaultCoord.fWidth && NameCoord.fHeight < LineCoord.fHeight * 2.0f )
			wszSkillName += '\n';
		wszSkillName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 733 );

		swprintf_s( szTemp, 256, L"%s", wszSkillName.c_str() );
		m_pStaticName->SetTextColor( textcolor::PALETURQUOISE );
	}
	m_pStaticName->SetText( szTemp );

	// 구분선
	m_pTextBox->AddText( L"" );
	AddDivisionLine(0);

	int nLevelUpValue = 0;
	nLevelUpValue = pSkill->GetLevelUpValue();

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	bool bTotalLevelSkill = GetSkillTask().IsTotalLevelSkill(pInvenItem->GetClassID());
	if (bTotalLevelSkill)
	{
		CDnTotalLevelSkillSystem* pTotalLevelSystem = NULL;
		bool isTotaLevelLimit = false;
		bool isCharacterLevelLimit = false;

		int nTotalLevel = 0;
		int nCharacterLevel = 0;
		int nLimitTotalLevel = 0;
		int nLimitCharacterLevel = 0;

		//통합레벨 제한
		CDnPlayerActor* pPlayerActor = NULL;
		if (CDnActor::s_hLocalActor)
			pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

		if (pPlayerActor)
		{
			pTotalLevelSystem = pPlayerActor->GetTotalLevelSkillSystem();

			nCharacterLevel = pPlayerActor->GetLevel();
		}

		if (pTotalLevelSystem)
		{
			nTotalLevel = pTotalLevelSystem->GetTotalLevel();

			CDnTotalLevelSkillSystem::_TotalLevelSkillInfo* pInfo = pTotalLevelSystem->GetTotalLevelSkillInfo(pInvenItem->GetClassID());
			if (pInfo)
			{
				nLimitTotalLevel = pInfo->m_TotalLevelLimit;
				nLimitCharacterLevel = pInfo->m_ActorLevelLimit;
			}
		}

		DWORD dwTextColor = textcolor::WHITE;
		isTotaLevelLimit = nLimitTotalLevel <= nTotalLevel;
		isCharacterLevelLimit = nLimitCharacterLevel <= nCharacterLevel;

		dwTextColor = isTotaLevelLimit ? textcolor::WHITE : textcolor::RED;
		_snwprintf_s(szTemp, _countof(szTemp), _TRUNCATE, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1831 ), nLimitTotalLevel);
		m_pTextBox->AddText( szTemp, dwTextColor );

		dwTextColor = isCharacterLevelLimit ? textcolor::WHITE : textcolor::RED;
		_snwprintf_s(szTemp, _countof(szTemp), _TRUNCATE, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 729 ), nLimitCharacterLevel);
		m_pTextBox->AddText( szTemp, dwTextColor );
	}
	else
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	if (nLevelUpValue == 0)
	{
		// 1814 : 스킬 레벨
#ifdef PRE_MOD_CHANGE_SKILLTOOLTIP
		swprintf_s( szTemp, _countof(szTemp), L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1814 ) );		
#else
		// 62 : 스킬, 64 : 레벨
		swprintf_s( szTemp, _countof(szTemp), L"%s %s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 62 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ) );
#endif
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
		swprintf_s( szTemp, _countof(szTemp), L" %d", pSkill->GetLevel() );
		m_pTextBox->AppendText( szTemp, dwFontColor );
	}
	else
	{
		// 1814 : 스킬 레벨
#ifdef PRE_MOD_CHANGE_SKILLTOOLTIP
		swprintf_s( szTemp, _countof(szTemp), L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1814 ) );
#else
		// 62 : 스킬, 64 : 레벨
		swprintf_s( szTemp, _countof(szTemp), L"%s %s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 62 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ) );
#endif
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
		
		bool isLocked = pSkill->IsLocked();
		bool isAquired = pSkill->IsAcquired();

		if (true == isLocked || false == isAquired)
		{
			swprintf_s( szTemp, 256, L" %d", pSkill->GetLevel() );
			m_pTextBox->AppendText( szTemp, textcolor::WHITE );
	
			//공백 하나 추가 : 스킬 레벨 : 2 (내용)
			//                              - 레벨 숫자 다음 괄호 앞에
			tstring msg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 736 );
			swprintf_s( szTemp, 256, L" %s", msg.c_str() );
			m_pTextBox->AppendText(msg.c_str());
		}
		else
		{
			swprintf_s( szTemp, 256, L" %d", pSkill->GetLevel() );
			m_pTextBox->AppendText( szTemp, textcolor::FONT_GREEN );

			wchar_t szInfo[256]={0};
			//공백 하나 추가 : 스킬 레벨 : 2 (장비착용효과 +1)
			//                              - 레벨 숫자 다음 괄호 앞에
			swprintf_s( szInfo, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 735 ), nLevelUpValue );
			swprintf_s( szTemp, 256, L" %s", szInfo );
			m_pTextBox->AppendText(szTemp);
		}
	}

	//// 최대 레벨 표시
	//if( pSkill->GetLevel() < pSkill->GetMaxLevel() )
	//{
	//	swprintf_s( szTemp, 256, L" (%s %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1138 ), pSkill->GetMaxLevel() );
	//	m_pTextBox->AppendText( szTemp, dwFontColor );
	//}

#if defined(PRE_FIX_NEXTSKILLINFO)
	int iSkillLevelTableID = -1;
	int iMinLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	SKILL_LEVEL_INFO* pLevelTableInfo = NULL;

	if( !GetSkillTask().IsGuildWarSkill( pSkill->GetClassID() ) )
		pLevelTableInfo = GetSkillTask().GetSkillLevelTableIDList(pSkill->GetClassID(), pSkill->GetSelectedLevelDataType());
	else
		pLevelTableInfo = GetGuildWarTask().GetSkillLevelTableIDList(pSkill->GetClassID(), pSkill->GetSelectedLevelDataType());
	
	SKILL_LEVEL_TABLE_IDS::const_iterator findIter = pLevelTableInfo->_SkillLevelTableIDs.find(pSkill->GetLevel());
	if (findIter != pLevelTableInfo->_SkillLevelTableIDs.end())
		iSkillLevelTableID = findIter->second;

	findIter = pLevelTableInfo->_SkillLevelTableIDs.find(pSkill->GetLevel() + 1);
	if (findIter != pLevelTableInfo->_SkillLevelTableIDs.end())
		iNextSkillLevelTableID = findIter->second;

	findIter = pLevelTableInfo->_SkillLevelTableIDs.find(pLevelTableInfo->_MinLevel);
	if (findIter != pLevelTableInfo->_SkillLevelTableIDs.end())
		iMinLevelTableID = findIter->second;

#endif // PRE_FIX_NEXTSKILLINFO

	// 소모 HP, MP
	{
		int nHP = pSkill->GetDecreaseHP();
		int nMP = pSkill->GetDecreaseMP();
		if( 0 < pSkill->GetUsePoint() )
		{
#if defined(PRE_FIX_NEXTSKILLINFO)
			nHP = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseHP" )->GetInteger();
			nMP = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseSP" )->GetInteger();
#else
			nHP = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel()-1, "_DecreaseHP" )->GetInteger();
			nMP = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel()-1, "_DecreaseSP" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO
		}

		if( nHP > 0 )
		{
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
			swprintf_s( szTemp, _countof(szTemp), L"%s %s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1812 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 17 ) ); // UISTRING : 소비, HP
#else
			swprintf_s( szTemp, _countof(szTemp), L"%s HP :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1812 ) );
#endif 
			m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
			swprintf_s( szTemp, _countof(szTemp), L" %d", nHP );
			m_pTextBox->AppendText( szTemp, dwFontColor );
		}
		if( nMP > 0 )
		{
#ifdef _ADD_NEW_MPCONSUME
			swprintf_s(szTemp, _countof(szTemp), L"%s %s :", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1812), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 18)); // UISTRING : 소비, MP
			m_pTextBox->AddText(szTemp, textcolor::FONT_ORANGE);
			swprintf_s(szTemp, _countof(szTemp), L" %d", nMP);
			m_pTextBox->AppendText(szTemp, dwFontColor);
#else
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
			swprintf_s( szTemp, _countof(szTemp), L"%s %s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1812 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 18 ) ); // UISTRING : 소비, MP
#else 
			swprintf_s( szTemp, _countof(szTemp), L"%s MP :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1812 ) ); //base mp
#endif
			m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
			swprintf_s( szTemp, _countof(szTemp), L" %d", nMP );
			m_pTextBox->AppendText( szTemp, dwFontColor );
#define SKILL_TOOLTIP_MPRATE
#ifdef SKILL_TOOLTIP_MPRATE
			float fRate = 0.0f;
			if( pSkill->GetMPConsumeType() == 0.0f )
#if defined(PRE_FIX_NEXTSKILLINFO)
				fRate = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseSP" )->GetInteger() * 0.1f;
#else
				fRate = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel()-1, "_DecreaseSP" )->GetInteger() * 0.1f;
#endif // PRE_FIX_NEXTSKILLINFO
			else
				fRate = pSkill->GetMPConsumeType() * 100.0f;
			swprintf_s( szTemp, 256, L" " );
			m_pTextBox->AppendText( szTemp, dwFontColor );
			swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1819 ), fRate );
			m_pTextBox->AppendText( szTemp, dwFontColor );
#endif
#endif
		}
	}
	
	// 필요 무기 최대 2개로 변경 됨.
	int nRequireWeaponCount = 2;
	bool bRequireWeapon = false;
	for( int i = 0; i < nRequireWeaponCount; ++i ) {
		if( pSkill->GetNeedEquipType(i) != CDnWeapon::EquipTypeEnum_Amount )
			bRequireWeapon = true;
	}

	if( bRequireWeapon )
	{
		swprintf_s( szTemp, 256, L"%s : ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1141 ) );
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );

		// 필요무기 중 하나라도 있으면 해당 스킬은 사용가능한거라 한다. 그래서 하나라도 있으면 하얀색 처리.
		swprintf_s( szTemp, 256, L"" );
		dwFontColor = textcolor::RED;
		for( int i = 0; i < nRequireWeaponCount; ++i ) {
			int nRequireWeaponType = pSkill->GetNeedEquipType(i);
			if( nRequireWeaponType == CDnWeapon::EquipTypeEnum_Amount ) continue;

			swprintf_s( szTempSub1, 256, L"%s", ITEM::GetEquipString( (CDnWeapon::EquipTypeEnum)nRequireWeaponType ) );
			if( (CDnActor::s_hLocalActor->GetWeapon(0) && CDnActor::s_hLocalActor->GetWeapon(0)->GetEquipType() == nRequireWeaponType) ||
				(CDnActor::s_hLocalActor->GetWeapon(1) && CDnActor::s_hLocalActor->GetWeapon(1)->GetEquipType() == nRequireWeaponType) )
				dwFontColor = textcolor::WHITE;

			if( szTemp[0] == L'\0' )
				swprintf_s( szTemp, 256, L"%s", szTempSub1 );
			else
				swprintf_s( szTemp, 256, L"%s, %s", szTemp, szTempSub1 );
		}
		m_pTextBox->AppendText( szTemp, dwFontColor );
		dwFontColor = textcolor::WHITE;
	}

	bool bShowNeedItem = true;
	// 레더모드에서는 스킬 필요아이템을 소모하지않음.
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP) 
	{
		if( ((CDnPvPGameTask*)pGameTask)->IsLadderMode() )
			bShowNeedItem = false;
	}

	// 소비 아이템
	if( pSkill->GetNeedItemID() > 0 && bShowNeedItem)
	{
		swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1142 ) );
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
		swprintf_s( szTemp, 256, L" %s(%d)", SKILL::GET_DECREASE_ITEM(pSkill->GetNeedItemID()), pSkill->GetNeedItemDecreaseCount() );
		int iNumNeedItem = GetItemTask().GetCharInventory().ScanItemFromID( pSkill->GetNeedItemID(), NULL );
		if( iNumNeedItem < pSkill->GetNeedItemDecreaseCount() )
			dwFontColor = textcolor::RED;
		m_pTextBox->AppendText( szTemp, dwFontColor );
		dwFontColor = textcolor::WHITE;
	}

	// 스킬 타입
	swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1139 ) );
	m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	switch( pSkill->GetSkillType() ) {
		case CDnSkill::SkillTypeEnum::Active:
			swprintf_s( szTemp, 256, L" %s", SKILL::GET_DURATION_TYPE(pSkill->GetDurationType()) );
			break;
		default:
			swprintf_s( szTemp, 256, L" %s", SKILL::GET_TYPE(pSkill->GetSkillType()) );
			break;
	}
	m_pTextBox->AppendText( szTemp, dwFontColor );

	//// 타켓 설정
	//swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1140 ) );
	//m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	//swprintf_s( szTemp, 256, L" %s", SKILL::GET_USING_TYPE(pSkill->GetTargetType()) );
	//m_pTextBox->AppendText( szTemp, dwFontColor );

	// 재사용시간. 스킬은 아이템과 달리 소수점 단위도 처리한다.
	if( pSkill->GetDelayTime() > 0 )
	{
		m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_COOLTIME ), textcolor::FONT_ORANGE );
		if( ( pSkill->GetDelayTime() - (int)pSkill->GetDelayTime() ) > 0 )
		{
			// 2.332999 초도 있다.(2.333초로 설정한 듯)
			// 물어보니 소수점 둘째에서 올림 처리로 해달라고 한다.(2.4초로 나오도록.. 차라리 엑셀에서 RoundDown쓰는게 나으려나.)
			float fTempSec = pSkill->GetDelayTime();
			fTempSec *= 10.0f;
			if( fTempSec - (int)fTempSec > 0.0f )
				fTempSec = (int)fTempSec + 1.0f;
			float fSec = fTempSec /= 10.0f;
			swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 115 ), fSec );
		}
		else
		{
			int nSec = (int)(pSkill->GetDelayTime());
			swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2312 ), nSec );
		}
		swprintf_s( szTemp, 256, L" %s", szTempSub1 );
		m_pTextBox->AppendText( szTemp, dwFontColor );
	}

	// 스킬 레벨업 조건 //////////////////////////////////////////////////////////////////////////////////
	// 스킬을 획득한 상태고, 최대 레벨에 도달한 상태면 다음 스킬 레벨업 조건 툴팁은 보여주지 않는다. 

	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#ifdef PRE_FIX_CLIENT_SKILL_MAX
	int nMaxSkillLevel = pSkillTable->GetFieldFromLablePtr( pSkill->GetClassID(), "_MaxLevel" )->GetInteger();
#else
	int nMaxSkillLevel = pSkill->GetMaxLevel();
#endif

	if( false == ((pSkill->IsAcquired()) && ( nMaxSkillLevel == pSkill->GetLevel() )) )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
		swprintf_s( szTemp, 256, L"%s ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 726 ) );
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );

		// 캐릭터 요구 레벨
		dwFontColor = textcolor::WHITE;
		int nLevelLimit = 0;
#if defined(PRE_FIX_NEXTSKILLINFO)
		if( pSkill->IsAcquired() )
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_LevelLimit" )->GetInteger();
		else
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iMinLevelTableID, "_LevelLimit" )->GetInteger();
#else
		if( pSkill->IsAcquired() )
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel(), "_LevelLimit" )->GetInteger();
		else
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset, "_LevelLimit" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

		if( CDnActor::s_hLocalActor->GetLevel() < nLevelLimit )
			dwFontColor = textcolor::RED;
		swprintf_s( szTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 729 ), nLevelLimit );
		m_pTextBox->AddText( szTemp, dwFontColor );

		if( false == pSkill->GetNodeRenderInfo().vlParentSkillIDs.empty() )
		{
			const CDnSkillTreeSystem::S_NODE_RENDER_INFO& NodeRenderInfo = pSkill->GetNodeRenderInfo();
			int iNumParentSkills = (int)NodeRenderInfo.vlParentSkillIDs.size();
			for( int i = 0; i < iNumParentSkills; ++i )
			{
				int iStringID = pSkillTable->GetFieldFromLablePtr( NodeRenderInfo.vlParentSkillIDs.at(i), "_NameID" )->GetInteger();
				tstring strSkillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );

				if( i+1 != iNumParentSkills )
					swprintf_s( szTemp, 256, L"%s %s %d,", strSkillName.c_str(), 
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ),
					NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(i) );
				else
					swprintf_s( szTemp, 256, L"%s %s %d", strSkillName.c_str(), 
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ),
					NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(i) );		// 마지막은 콤마 안 붙임.

				//if( i+1 != iNumParentSkills )
				//	swprintf_s( szTemp, 256, L"%s(%s %d %s),", strSkillName.c_str(), 
				//											  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ),
				//											  NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(i),
				//											  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
				//else
				//	swprintf_s( szTemp, 256, L"%s(%s %d %s)", strSkillName.c_str(), 
				//											GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ),
				//											NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(i),
				//											GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );		// 마지막은 콤마 안 붙임.

				// 배운 상태라면 하얀색. 안 배운 상태라면 빨간색. (레벨까지 포함.)
				DWORD dwFontColor = textcolor::RED;
				DnSkillHandle hParentSkill = CDnActor::s_hLocalActor->FindSkill( NodeRenderInfo.vlParentSkillIDs.at(i) );

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
				DnSkillHandle hReservationSkill = GetSkillTask().GetReservationSkill( NodeRenderInfo.vlParentSkillIDs.at(i) );
				if( hReservationSkill )
					hParentSkill = hReservationSkill;
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

				if( hParentSkill && hParentSkill->GetLevel() >= NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(i) )
					dwFontColor = textcolor::WHITE;

				m_pTextBox->AddText( szTemp, dwFontColor );
			}
		}

		// 필요 SP 표시
		// 최대 레벨이 1인 경우 1렙짜리로 출력해줘야 한다. (#21154)
		int iNowSkillLevel = pSkill->GetLevel();
		if( iNowSkillLevel < pSkill->GetMaxLevel() || 1 == pSkill->GetMaxLevel() )
		{
			DWORD dwTextColor = textcolor::WHITE;
			int iNeedSkillPoint = 0;

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			dwTextColor = textcolor::RED;
			//직업별 sp 
			//현재 스킬의 needspbyjob 값을 얻는다.
			//현재 캐릭터의 jobHisory를 얻는다.
			CDnPlayerActor *pPlayerActor = NULL;
			if (CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->IsPlayerActor())
				pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

			std::vector<int> nJobHistory;
			if (pPlayerActor)
				pPlayerActor->GetJobHistory(nJobHistory);
			int nJobHistoryCount = (int)nJobHistory.size();

			std::vector<int> nNeedSPValues;
			GetSkillTask().GetNeedSPValuesByJob(pSkill->GetClassID(), nNeedSPValues);

			int nListCount = (int)nNeedSPValues.size();
			for (int i = 0; i < nListCount; ++i)
			{
				if (nJobHistoryCount - 1 < i && nNeedSPValues[i] > 0)
				{
					swprintf_s( szTemp, _countof(szTemp), L"%d 차 전직이 필요 합니다.", i );
					m_pTextBox->AddText( szTemp, dwTextColor );
				}
				else
				{
					bool isAvailable = GetSkillTask().IsAvailableSPByJob(nJobHistory, nNeedSPValues, i);
					if (isAvailable == false)
					{
						int iJobID = nJobHistory[i];
						DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );
						std::wstring wstrJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iJobID, "_JobName" )->GetInteger());

						//%s SP 합계 %d 이상
						swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126310 ), wstrJobName.c_str(), nNeedSPValues[i] );
						m_pTextBox->AddText( szTemp, dwTextColor );
					}
				}
			}

#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

			if( pSkill->IsAcquired() )
				iNeedSkillPoint = pSkill->GetNextLevelSkillPoint();
			else
				iNeedSkillPoint = pSkill->GetNeedAcquireSkillPoint();

			if( GetSkillTask().GetSkillPoint() < iNeedSkillPoint )
				dwTextColor = textcolor::RED;
			else
				dwTextColor = textcolor::WHITE;

			swprintf_s( szTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 727 ), iNeedSkillPoint );
			m_pTextBox->AddText( szTemp, dwTextColor );
		}

	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	// 직업 제한 - 기획서에 빼라고 되어있다.
	//if( CDnActor::s_hLocalActor->GetClassID() != pSkill->GetNeedJobClassID() )
	//{
	//	dwFontColor = textcolor::RED;
	//}
	//m_pTextBox->AddText( GetClassString(pSkill->GetNeedJobClassID()), dwFontColor );
	//dwFontColor = textcolor::WHITE;

	// 사거리 - 기획서에 빼라고 되어있다.
	//if( pSkill->GetIncreaseRange() > 0 )
	//{
	//	swprintf_s( szTemp, 256, L"%s %dm %s", SKILL::GET_STATIC(emTOOLTIP_STATIC::SKILL_DISTANCE), pSkill->GetIncreaseRange()/100, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2260 ) );
	//	m_pTextBox->AppendText( szTemp, dwFontColor, UITEXT_RIGHT );
	//}

	// 스킬 효과
	m_pTextBox->AddText( L"" );
	AddDivisionLine();

	SUICoord SkillDescStartCoord, SkillDescEndCoord, PointBoxCoord;
	m_pTextBox->GetLastLineCoord( SkillDescStartCoord );

	m_pTextBox->AddText( SKILL::GET_STATIC(emTOOLTIP_STATIC::SKILL_DESCRIPTION), textcolor::FONT_ORANGE );
	//int nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel()-1, "_LevelLimit" )->GetInteger();
	//if( CDnActor::s_hLocalActor->GetLevel() < nLevelLimit )
	//	dwFontColor = textcolor::RED;
	//swprintf_s( szTemp, 256, L"(%s %d %s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nLevelLimit, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
	//m_pTextBox->AppendText( szTemp, dwFontColor );
	dwFontColor = textcolor::WHITE;

	// 스킬 설명
	m_pTextBox->AddColorText( pSkill->GetDescription() );
	//SetSkillDescriptionText( L"블라블라#r#b#r블라\n#w블라블라블라" );

	m_pTextBox->AddText( L"" );

	m_pTextBox->GetLastLineCoord( SkillDescEndCoord );
	m_pStaticPointBox->Show( true );
	m_pStaticPointBox->GetUICoord( PointBoxCoord );
	PointBoxCoord.fY = SkillDescStartCoord.fY + (float)6/DEFAULT_UI_SCREEN_HEIGHT;
	PointBoxCoord.fHeight = SkillDescEndCoord.fY - SkillDescStartCoord.fY;
	m_pStaticPointBox->SetUICoord( PointBoxCoord );

	if( pSkill->GetLevel() < nMaxSkillLevel )
	{
		// 다음 레벨 스킬 효과
		AddDivisionLine();
		m_pTextBox->AddText( SKILL::GET_STATIC(emTOOLTIP_STATIC::SKILL_NEXT_DESCRIPTION), textcolor::FONT_ORANGE );
		//int nLevelLimit2 = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+pSkill->GetLevel(), "_LevelLimit" )->GetInteger();
		//if( CDnActor::s_hLocalActor->GetLevel() < nLevelLimit2 )
		//	dwFontColor = textcolor::RED;
		//swprintf_s( szTemp, 256, L"(%s %d %s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nLevelLimit2, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
		//m_pTextBox->AppendText( szTemp, dwFontColor );
		
		//// 필요 SP 표시
		//int iNowSkillLevel = pSkill->GetLevel();
		//if( iNowSkillLevel < pSkill->GetMaxLevel() )
		//{
		//	if( pSkill->IsAcquired() )
		//		swprintf_s( szTemp, 256, L"%s: %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 727 ), pSkill->GetNextLevelSkillPoint() );
		//	else
		//		swprintf_s( szTemp, 256, L"%s: %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 727 ), pSkill->GetNeedAcquireSkillPoint() );

		//	m_pTextBox->AddText( szTemp, textcolor::WHITE );
		//}

		dwFontColor = textcolor::WHITE;

		m_pTextBox->AddColorText( pSkill->GetNextExplainString(), textcolor::WHITE, 0, D3DCOLOR_ARGB( 0, 80, 80, 80 ) );
		m_pTextBox->AddText( L"" );
	}

	pSkill->SelectLevelDataType( nCurSelectedLevelDataType );

	if( CDnPetTask::IsActive() && GetPetTask().IsPetSkill( pInvenItem->GetClassID() ) )	// 펫 스킬은 사냥/PVP 변경이 없으므로 아래 문구 출력 하지 않음
		return;

	AddDivisionLine();
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 734 ), textcolor::FONT_GREEN );
}

void CDnTooltipDlg::SetGuildWarSkillText( MIInventoryItem *pInvenItem )
{
	ASSERT( pInvenItem && "CDnTooltipDlg::SetGuildWarSkillText, pInvenItem==NULL!" );

	if( pInvenItem->GetType() != MIInventoryItem::Skill )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetItemText, 스킬이 아닙니다." );
		return;
	}

	wchar_t szTemp[256]={0};
	wchar_t szTempSub1[256]={0};
	DWORD dwFontColor(textcolor::WHITE);

	CDnSkill *pSkill = static_cast<CDnSkill*>(pInvenItem);
	if( !pSkill ) return;
	if( !CDnActor::s_hLocalActor ) return;

	DNTableFileFormat*  pSkillLevelTable = NULL;
	pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );

	if( !pSkillLevelTable )
		return;

	// pvp모드에서 댔을때는 먼저 pvp툴팁이 보여야하므로, 현재 설정된 값을 알아와 설정한다.
	int nCurSelectedLevelDataType = pSkill->GetSelectedLevelDataType();
	pSkill->SelectLevelDataType( CDnSkill::PVP, true );

	// 스킬 이름
	wstring wszSkillName = pSkill->GetName();
	SUICoord NameCoord, LineCoord;
	CalcTextRect( L" ", m_pStaticName->GetElement(0), LineCoord );
	CalcTextRect( wszSkillName.c_str(), m_pStaticName->GetElement(0), NameCoord );
	if( NameCoord.fWidth < m_TextBoxDefaultCoord.fWidth && NameCoord.fHeight < LineCoord.fHeight * 2.0f )
		wszSkillName += '\n';
	wszSkillName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 733 );

	swprintf_s( szTemp, 256, L"%s", wszSkillName.c_str() );
	m_pStaticName->SetTextColor( textcolor::PALETURQUOISE );
	m_pStaticName->SetText( szTemp );

	// 구분선
	m_pTextBox->AddText( L"" );
	AddDivisionLine( 0 );

	// 스킬 타입
	swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1139 ) );
	m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
	switch( pSkill->GetSkillType() ) 
	{
		case CDnSkill::SkillTypeEnum::Active:
			swprintf_s( szTemp, 256, L" %s", SKILL::GET_DURATION_TYPE(pSkill->GetDurationType()) );
			break;
		default:
			swprintf_s( szTemp, 256, L" %s", SKILL::GET_TYPE(pSkill->GetSkillType()) );
			break;
	}
	m_pTextBox->AppendText( szTemp, dwFontColor );

	// 소비 자원
	int nItemID = pSkillLevelTable->GetItemIDFromField( "_SkillIndex", pInvenItem->GetClassID() );
	int nNeedResource = pSkillLevelTable->GetFieldFromLablePtr( nItemID, "_DecreaseTIC" )->GetInteger();

	if( nNeedResource > 0 )
	{
		swprintf_s( szTemp, 256, L"%s :", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000031444 ) );
		m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );
		swprintf_s( szTemp, 256, L" %d", nNeedResource );
		m_pTextBox->AppendText( szTemp, textcolor::WHITE );
	}

	// 재사용시간. 스킬은 아이템과 달리 소수점 단위도 처리한다.
	if( pSkill->GetDelayTime() > 0 )
	{
		m_pTextBox->AddText( ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_COOLTIME ), textcolor::FONT_ORANGE );
		if( ( pSkill->GetDelayTime() - (int)pSkill->GetDelayTime() ) > 0 )
		{
			// 2.332999 초도 있다.(2.333초로 설정한 듯)
			// 물어보니 소수점 둘째에서 올림 처리로 해달라고 한다.(2.4초로 나오도록.. 차라리 엑셀에서 RoundDown쓰는게 나으려나.)
			float fTempSec = pSkill->GetDelayTime();
			fTempSec *= 10.0f;
			if( fTempSec - (int)fTempSec > 0.0f )
				fTempSec = (int)fTempSec + 1.0f;
			float fSec = fTempSec /= 10.0f;
			swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 115 ), fSec );
		}
		else
		{
			int nSec = (int)(pSkill->GetDelayTime());
			swprintf_s( szTempSub1, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2312 ), nSec );
		}
		swprintf_s( szTemp, 256, L" %s", szTempSub1 );
		m_pTextBox->AppendText( szTemp, dwFontColor );
	}

	// 스킬 효과
	m_pTextBox->AddText( L"" );
	AddDivisionLine();

	SUICoord SkillDescStartCoord, SkillDescEndCoord, PointBoxCoord;
	m_pTextBox->GetLastLineCoord( SkillDescStartCoord );

	m_pTextBox->AddText( SKILL::GET_STATIC(emTOOLTIP_STATIC::SKILL_DESCRIPTION), textcolor::FONT_ORANGE );
	dwFontColor = textcolor::WHITE;

	// 스킬 설명
	m_pTextBox->AddColorText( pSkill->GetDescription() );
	m_pTextBox->AddText( L"" );

	m_pTextBox->GetLastLineCoord( SkillDescEndCoord );
	m_pStaticPointBox->Show( true );
	m_pStaticPointBox->GetUICoord( PointBoxCoord );
	PointBoxCoord.fY = SkillDescStartCoord.fY + (float)6/DEFAULT_UI_SCREEN_HEIGHT;
	PointBoxCoord.fHeight = SkillDescEndCoord.fY - SkillDescStartCoord.fY;
	m_pStaticPointBox->SetUICoord( PointBoxCoord );

	if( pSkill->GetLevel() < pSkill->GetMaxLevel() )
	{
		// 다음 레벨 스킬 효과
		AddDivisionLine();
		m_pTextBox->AddText( SKILL::GET_STATIC(emTOOLTIP_STATIC::SKILL_NEXT_DESCRIPTION), textcolor::FONT_ORANGE );
		dwFontColor = textcolor::WHITE;

		m_pTextBox->AddColorText( pSkill->GetNextExplainString(), textcolor::WHITE, 0, D3DCOLOR_ARGB( 0, 80, 80, 80 ) );
		m_pTextBox->AddText( L"" );
	}

	pSkill->SelectLevelDataType( nCurSelectedLevelDataType );
}

void CDnTooltipDlg::SetGestureText( int nGestureID )
{
	if( nGestureID <= 0 )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetItemText, 제스처가 아닙니다." );
		return;
	}

	wchar_t szTemp[256]={0};
	if( !CDnActor::s_hLocalActor ) return;
	if( !CDnGestureTask::IsActive() ) return;

	CDnGestureTask::SGestureInfo *pInfo = GetGestureTask().GetGestureInfoFromID( nGestureID );
	if( !pInfo ) return;

	// 제스처 이름
	swprintf_s( szTemp, 256, L"%s", pInfo->szName.c_str() );
	m_pStaticName->SetTextColor( textcolor::WHITE );
	m_pStaticName->SetText( szTemp );

	// 구분선
	m_pTextBox->AddText( L"" );
	AddDivisionLine(0);

	// 제한 레벨
	if( pInfo->nUnlockLevel > 1 )
	{
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2240 ), textcolor::FONT_ORANGE );
		swprintf_s( szTemp, 256, L" %d %s", pInfo->nUnlockLevel, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1326 ) );
		m_pTextBox->AppendText( szTemp, ( CDnActor::s_hLocalActor->GetLevel() < pInfo->nUnlockLevel ) ? textcolor::RED : textcolor::WHITE );
	}

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	// 설명
	if( !pInfo->szDescription.empty() )
	{
		m_pTextBox->AddColorText( pInfo->szDescription.c_str() );
	}

#if defined(PRE_ADD_GUILD_GESTURE)
	if (pInfo->_Type == CDnGestureTask::eGestureType::GestureType_GuildReward)
	{
		//길드 보상용 제스처인경우 경고 메시지..
		m_pTextBox->AddText( L"" );
		AddDivisionLine();

		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1591 ), textcolor::RED );
	}
#endif // PRE_ADD_GUILD_GESTURE
}

void CDnTooltipDlg::SetLifeSkillText( int nLifeSkillID )
{
	if( nLifeSkillID <= 0 )
	{
		CDebugSet::ToLogFile( "CDnTooltipDlg::SetLifeSkillText, LifeSkill이 아닙니다." );
		return;
	}

#if defined( PRE_ADD_SECONDARY_SKILL )
	DNTableFileFormat* pSoxSkill = GetDNTable( CDnTableDB::TSecondarySkill );
	DNTableFileFormat* pSoxSkillLevel = GetDNTable( CDnTableDB::TSecondarySkillLevel );
	if( pSoxSkill == NULL || pSoxSkillLevel == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillType" );
		return;
	}

	if( !CDnActor::s_hLocalActor ) return;
	if( !CDnSkillTask::IsActive() ) return;
	CSecondarySkillRepository& pLifeSkillRepository = GetSkillTask().GetLifeSkillRepository();
	CSecondarySkill *pSecondarySkill = pLifeSkillRepository.Get( nLifeSkillID );
	if( !pSecondarySkill )
		return;

	std::vector<int> vecID;
	for( int i=0; i<pSoxSkillLevel->GetItemCount(); i++ )
		vecID.push_back( pSoxSkillLevel->GetItemID( i ) );

	int nSkillLevelIndex = 0;
	std::vector<int>::iterator iter = vecID.begin();
	for( ; iter!= vecID.end(); iter++ )
	{
		if( pSecondarySkill->GetSkillID() == pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillID" )->GetInteger() )
		{
			SecondarySkill::Grade::eType eSkillGrade = (SecondarySkill::Grade::eType)pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillClass" )->GetInteger();
			int nSkillLevel = pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillLevel" )->GetInteger();

			if( eSkillGrade == pSecondarySkill->GetGrade() && nSkillLevel == pSecondarySkill->GetLevel() )
			{
				nSkillLevelIndex = (*iter);
				break;
			}
		}
	}

	// 이름
	int nNameIndex = pSoxSkill->GetFieldFromLablePtr( nLifeSkillID, "_SecondarySkillName" )->GetInteger();
	m_pStaticName->SetTextColor( textcolor::WHITE );
	m_pStaticName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nNameIndex ) );

	// 구분선
	m_pTextBox->AddText( L"" );
	AddDivisionLine(0);

	// 레벨
	WCHAR wszGrade[64] = {0,};
	int nGradeStringIndex = 0;
	switch( pSecondarySkill->GetGrade() )
	{
	case SecondarySkill::Grade::Beginning:
		nGradeStringIndex = 7303;
		break;
	case SecondarySkill::Grade::Intermediate:
		nGradeStringIndex = 7304;
		break;
	case SecondarySkill::Grade::Advanced:
		nGradeStringIndex = 7305;
		break;
	}
	swprintf_s( wszGrade, _countof( wszGrade ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nGradeStringIndex ) );
	WCHAR wszLevel[64] = {0,};
	swprintf_s( wszLevel, _countof( wszLevel ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3762 ), pSecondarySkill->GetLevel() );
	WCHAR wszText[128] = {0,};
	swprintf_s( wszText, _countof( wszText ), L"%s %s", wszGrade, wszLevel );
	m_pTextBox->AddText( wszText, textcolor::LIGHTSKYBLUE );

	m_pTextBox->AddText( L"" );
	AddDivisionLine();

	// 설명
	int nDescriptionIndex = pSoxSkillLevel->GetFieldFromLablePtr( nSkillLevelIndex, "_SecondarySkillExplanation" )->GetInteger();
	std::wstring wszDescription = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nDescriptionIndex );
	if( !wszDescription.empty() )
		m_pTextBox->AddColorText( wszDescription.c_str() );
#endif	// #if defined( PRE_ADD_SECONDARY_SKILL )
}

CDnItem* CDnTooltipDlg::GetCompareDestItem(CDnItem* pSrcItem) const
{
	if (m_itemSlotType == ST_CHARSTATUS || !CDnActor::s_hLocalActor )
		return NULL;

	if (pSrcItem && (pSrcItem->GetType() == MIInventoryItem::Item))
	{
		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
		std::vector<int> nVecJobList;
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pSrcItem->IsNeedJob() == false || pSrcItem->IsPermitPlayer( nVecJobList ))
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if (pCharStatusDlg)
			{
				int nSlotIndex = -1;
				if (pSrcItem->IsCashItem())
					nSlotIndex = pCharStatusDlg->GetCashEquipIndex( pSrcItem );
				else
					nSlotIndex = pCharStatusDlg->GetEquipIndex( pSrcItem );

				if (nSlotIndex >= 0)
				{
					eItemTypeEnum nType = pSrcItem->GetItemType();

					MIInventoryItem* pInvenItem = NULL;
					switch( nType )
					{
					case ITEMTYPE_WEAPON:
					case ITEMTYPE_PARTS:
						{
							// 아이템 타입(머리, 갑옷, 장갑, 무기 등등)에 따른 슬롯번호(첫번째 인자)로 장착되어있는 아이템을 얻어온다.
							pInvenItem = (pSrcItem->IsCashItem()) ? pCharStatusDlg->GetCashEquipItem( nSlotIndex ) : pCharStatusDlg->GetEquipItem( nSlotIndex );
							if (pInvenItem && pInvenItem->GetType() == MIInventoryItem::Item)
								return static_cast<CDnItem*>(pInvenItem);
						}
						break;

					case ITEMTYPE_GLYPH :
						{
							pInvenItem = pCharStatusDlg->GetEquipGlyph( nSlotIndex );
							if (pInvenItem && pInvenItem->GetType() == MIInventoryItem::Item)
								return static_cast<CDnItem*>(pInvenItem);
						}
						break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
					case ITEMTYPE_TALISMAN:
						{
							pInvenItem = pCharStatusDlg->GetEquipTalisman(nSlotIndex);
							if(pInvenItem && pInvenItem->GetType() == MIInventoryItem::Item)
								return static_cast<CDnItem*>(pInvenItem);
						}
						break;
#endif
					}
				}
			}
		}
	}

	return NULL;
}

void CDnTooltipDlg::GetCompareDestItem_Ring(std::vector<CDnItem*>& vResults, CDnItem* pSrcItem) const
{
	if( !CDnActor::s_hLocalActor )
		return;

#ifdef PRE_ADD_COMPARETOOLTIP
	// 이젠 하나의 정보만 보여주도록 한다.
	if (pSrcItem && (pSrcItem->GetType() == MIInventoryItem::Item) && (pSrcItem->GetItemType() == ITEMTYPE_PARTS))
	{
		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
		std::vector<int> nVecJobList;
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pSrcItem->IsNeedJob() == false || pSrcItem->IsPermitPlayer( nVecJobList ))
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if (pCharStatusDlg)
			{
				int nSlotIndex = pSrcItem->IsCashItem() ? CASHEQUIP_RING1 : EQUIP_RING1;
				if( m_nShowRingSlotIndex != -1 )
					nSlotIndex = m_nShowRingSlotIndex;
				MIInventoryItem* pInvenItem = NULL;
				pInvenItem = (pSrcItem->IsCashItem()) ? pCharStatusDlg->GetCashEquipItem( nSlotIndex ) : pCharStatusDlg->GetEquipItem( nSlotIndex );
				if (pInvenItem && pInvenItem->GetType() == MIInventoryItem::Item)
					vResults.push_back(static_cast<CDnItem*>(pInvenItem));
			}
		}
	}
#else
	if (pSrcItem && (pSrcItem->GetType() == MIInventoryItem::Item) && (pSrcItem->GetItemType() == ITEMTYPE_PARTS))
	{
		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
		std::vector<int> nVecJobList;
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pSrcItem->IsNeedJob() == false || pSrcItem->IsPermitPlayer( nVecJobList ))
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if (pCharStatusDlg)
			{
				int nSlotIndex = pSrcItem->IsCashItem() ? CASHEQUIP_RING1 : EQUIP_RING1;
				int i = 0;
				for (; i < MAX_RING_PARTS_COUNT; ++i) // 반지는 두개
				{
					MIInventoryItem* pInvenItem = NULL;
					pInvenItem = (pSrcItem->IsCashItem()) ? pCharStatusDlg->GetCashEquipItem( nSlotIndex ) : pCharStatusDlg->GetEquipItem( nSlotIndex );
					if (pInvenItem && pInvenItem->GetType() == MIInventoryItem::Item)
						vResults.push_back(static_cast<CDnItem*>(pInvenItem));
					++nSlotIndex;
				}
			}
		}
	}
#endif // PRE_ADD_COMPARETOOLTIP

}

#if defined(_TH)
#define INCREASE_MARKER L"+"
#define DECREASE_MARKER L"-"
#else
#define INCREASE_MARKER L"▲"
#define DECREASE_MARKER L"▼"
#endif

void CDnTooltipDlg::GetStateCompareTextUnit(std::wstring& out, eStateCompareType type, int diffMin, int diffMax, bool bPercent, int floatUnit) const
{
	if (type == eSCT_VALUE)
	{
		out = FormatW((bPercent ? L" (%s%d%%)" : L"  (%s%d)"), (diffMin >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMin));
	}
	else if (type == eSCT_RANGE)
	{
		std::wstring tempMin = FormatW(L"%s%d", (diffMin >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMin));
		std::wstring tempMax = FormatW(L"%s%d", (diffMax >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMax));

		out = FormatW((bPercent ? L" (%s%%%s%s%%)" : L" (%s%s%s)"), tempMin.c_str(), TILDE, tempMax.c_str());
	}
}

void CDnTooltipDlg::GetStateCompareTextUnit(std::wstring& out, eStateCompareType type, float diffMin, float diffMax, bool bPercent, int floatUnit) const
{
	if (type == eSCT_VALUE)
	{
		std::wstring formatter(L"  (%s");
		formatter += ((floatUnit == 1) ? L"%.1f" : L"%.2f");
		if (bPercent)
			formatter += L"%%";
		formatter += L")";

		out = FormatW( formatter.c_str(), (diffMin >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMin));
	}
	else if (type == eSCT_RANGE)
	{
		std::wstring formatter(L"%s");
		formatter += ((floatUnit == 1) ? L"%.1f" : L"%.2f");
		if (bPercent)
			formatter += L"%%";

		std::wstring tempMin = FormatW( formatter.c_str(), 
										 (diffMin >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMin));
		std::wstring tempMax = FormatW( formatter.c_str(), 
										 (diffMax >= 0) ? INCREASE_MARKER : DECREASE_MARKER, abs(diffMax));

		out = FormatW(L" (%s%s%s)", tempMin.c_str(), TILDE, tempMax.c_str());
	}
}

bool CDnTooltipDlg::IsCompareTextUnitOverLine(const std::wstring& compareText, const WCHAR* preText)
{
	//	note by kalliste : CalcTextRect를 쓰는게 맞겠지만, 툴팁 연산이 빈번하기에 간단하게 연산하기 위해 길이 연산으로 대체.
	//return ((int)compareText.length() + (int)wcslen(preText) > 26);

	std::wstring temp = compareText;
	temp += preText;

	SUICoord fontCoord;
	if (m_pTextBox->GetElement(0))
	{
		CalcTextRect(temp.c_str(), m_pTextBox->GetElement(0), fontCoord, -1);
		return (fontCoord.fWidth > m_pTextBox->GetUICoord().fWidth);
	}

	return false;
}

#ifdef PRE_ADD_COMPARETOOLTIP
bool CDnTooltipDlg::ShowCompareTooltip( bool bCash, eItemTypeEnum nType, int nSlotIndex, MIInventoryItem *&pDestItem, CDnTooltipDlg *pDialog, int forcePartsType, bool bNameLink)
#else
bool CDnTooltipDlg::ShowCompareTooltip( bool bCash, eItemTypeEnum nType, int nSlotIndex, MIInventoryItem *&pDestItem, CDnTooltipDlg *pDialog, bool bNameLink )
#endif // PRE_ADD_COMPARETOOLTIP
{
	CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( !pCharStatusDlg ) return false;

	switch( nType )
	{
	case ITEMTYPE_WEAPON:
	case ITEMTYPE_PARTS :
		{
			// 아이템 타입(머리, 갑옷, 장갑, 무기 등등)에 따른 슬롯번호(첫번째 인자)로 장착되어있는 아이템을 얻어온다.
			pDestItem = (bCash) ? pCharStatusDlg->GetCashEquipItem( nSlotIndex ) : pCharStatusDlg->GetEquipItem( nSlotIndex );

#ifdef PRE_ADD_COMPARETOOLTIP			
			if( pDestItem && forcePartsType != -1 ) // &&	( nSlotIndex == EQUIP_RING2 || nSlotIndex == CASHEQUIP_RING2 ) )
			  ((CDnParts *)pDestItem)->SetPartsType( (CDnParts::PartsTypeEnum)forcePartsType );
#endif // PRE_ADD_COMPARETOOLTIP
		}
		break;

	case ITEMTYPE_GLYPH :
		{
			pDestItem = pCharStatusDlg->GetEquipGlyph( nSlotIndex );
		}
		break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_TALISMAN:
		{
			pDestItem = pCharStatusDlg->GetEquipTalisman( nSlotIndex );
		}
		break;
#endif
	}

	if( pDestItem )
	{
		if( pDestItem->GetType() == MIInventoryItem::Item )
		{
			CDnItem* pItem = static_cast<CDnItem*>(pDestItem);
			if( pItem->HasPrefixSkill() )
				pDialog->SetToolTipSwitch(m_bTooltipSwitch);
		}

		pDialog->SetTooltipText( pDestItem, true, bNameLink );
		pDialog->UpdateTooltipNextPage();

		CEtUIDialog *pParentDlg = pDialog->GetParentDialog();
		if( !pParentDlg ) return false;

		SUICoord sParentDlgCoord, sSelfDlgCoord;
		pParentDlg->GetDlgCoord( sParentDlgCoord );
		pDialog->GetDlgCoord( sSelfDlgCoord );

		// 아이템 슬롯 버튼을 소유한 다이얼로그의 특성에 따라 기준 툴팁의 왼쪽에 렌더링할지 오른쪽에 렌더링할지를 결정한다.
		switch( m_itemSlotType )
		{
		case ST_INVENTORY:
			sParentDlgCoord.fX -= sSelfDlgCoord.fWidth - 0.018f;
			break;
		case ST_QUEST:
		case ST_STORAGE_PLAYER:
		case ST_STORAGE_GUILD:
		case ST_STORE:
		case ST_SKILL_STORE:
		case ST_EXCHANGE:
		case ST_ITEM_DISJOINT:
		case ST_PLATE:
		case ST_ITEM_UPGRADE:
		case ST_ITEM_COMPOUND:
		case ST_MARKET_REGIST:
		case ST_ITEM_COSTUMEMIX_RESULT:
			sParentDlgCoord.fX += sParentDlgCoord.fWidth - 0.018f;	// 우측에다 표시할때는 부모의 Width만큼을 더해야한다.
		    break;
		default:
			sParentDlgCoord.fX -= sSelfDlgCoord.fWidth - 0.018f;
		    break;
		}

		// 비교 툴팁의 x축 이동량은 기억해뒀다가 부모에도 적용해야한다.
		float fDlgXCoordRel = 0.0f;
		pDialog->SetPosition( sParentDlgCoord.fX, sParentDlgCoord.fY );

		{
			pDialog->GetDlgCoord( sSelfDlgCoord );
			if( (sSelfDlgCoord.Bottom()) > GetScreenHeightRatio() )
			{
				sSelfDlgCoord.fY -= (sSelfDlgCoord.Bottom() - GetScreenHeightRatio());
			}
			if( (sSelfDlgCoord.Right()) > GetScreenWidthRatio() )
			{
				fDlgXCoordRel = sSelfDlgCoord.Right() - GetScreenWidthRatio();
				sSelfDlgCoord.fX -= fDlgXCoordRel;
			}
			if( ( sSelfDlgCoord.fX ) < 0.f )
			{
				fDlgXCoordRel = sSelfDlgCoord.fX;
				sSelfDlgCoord.fX -= fDlgXCoordRel;
			}
			pDialog->SetDlgCoord( sSelfDlgCoord );
		}

		// 비교 툴팁이 좌측으로 이동했다면, 부모에도 같은 값을 적용.
		if (fDlgXCoordRel != 0.0f)
		{
			// 두번째 비교 툴팁의 경우 한단계 상위(첫번째 툴팁) 및 두번째 상위(기본 툴팁) 둘다 이동시켜야한다.
			CEtUIDialog *pParentDialog = pDialog;
			while( pParentDialog = pParentDialog->GetParentDialog() )
			{
				pParentDialog->GetDlgCoord( sParentDlgCoord );
				sParentDlgCoord.fX -= fDlgXCoordRel;
				pParentDialog->SetDlgCoord( sParentDlgCoord );
			}
		}

		pDialog->ShowCompareLight( true );
		pParentDlg->ShowChildDialog( pDialog, true );
		return true;
	}

	return false;
}

void CDnTooltipDlg::ShowCompareLight( bool bShow )
{
	if( m_pStaticBaseEquip == NULL )
		return;

	if( bShow )
	{
		SUICoord sDlgCoord;
		GetDlgCoord( sDlgCoord );
		m_pStaticBaseEquip->SetSize( sDlgCoord.fWidth, sDlgCoord.fHeight );
	}

	m_pStaticBaseEquip->Show( bShow );
	m_pStaticBase->Show( !bShow );
}

void CDnTooltipDlg::ClearDivisionLine()
{
	for( int i = 0; i < LINE_MAX; ++i ) {
		m_pStaticLine[i]->SetPosition( m_LineBaseCoord.fX, m_LineBaseCoord.fY );
		m_pStaticLine[i]->Show( false );
	}
	m_nCurLineIndex = 0;
}

void CDnTooltipDlg::AddDivisionLine( int nYOffset, bool bSmallWidth )
{
	if( m_nCurLineIndex >= LINE_MAX )
	{
		ASSERT(0&&"툴팁 구분선이 25개 이상 사용되었습니다.");
		return;
	}

	// 구분선
	SUICoord LastLineCoord;
	m_pTextBox->GetLastLineCoord( LastLineCoord );
	float fNewY = LastLineCoord.fY + (float)nYOffset/DEFAULT_UI_SCREEN_HEIGHT;
	float fNewOffsetX = m_pTextBox->GetUICoord().fX - m_LineBaseCoord.fX;

	// LastLineCoord의 y값 자체가 딱 떨어지는 형태가 아니라 구분선이 번져보인다. 그래서 정수화시킨다.
	fNewY = ((int)(fNewY*DEFAULT_UI_SCREEN_HEIGHT)) / (float)DEFAULT_UI_SCREEN_HEIGHT;

	float fLineWidth = bSmallWidth ? m_LineBaseCoord.fWidth/1.5f : m_LineBaseCoord.fWidth;
	//m_pStaticLine[m_nCurLineIndex]->SetPosition( m_LineBaseCoord.fX, fNewY );
	m_pStaticLine[m_nCurLineIndex]->SetPosition( LastLineCoord.fX-fNewOffsetX, fNewY );
	m_pStaticLine[m_nCurLineIndex]->SetSize( fLineWidth, m_LineBaseCoord.fHeight );
	m_pStaticLine[m_nCurLineIndex]->Show( true );
	++m_nCurLineIndex;
}

void CDnTooltipDlg::RemoveLastDivisionLine()
{
	--m_nCurLineIndex;
	if( m_nCurLineIndex < 0 ) m_nCurLineIndex = 0;
	m_pStaticLine[m_nCurLineIndex]->Show( false );
}

emTOOLTIP_STATIC CDnTooltipDlg::GetToolTipStaticByStateType(int stateType)
{
	switch(stateType)
	{
	case 0:	case 50:		return ITEM_STRENGTH;
	case 1:	case 51:		return ITEM_AGILITY;
	case 2:	case 52:		return ITEM_INTELLIGENCE;
	case 3:	case 53:		return ITEM_STAMINA;
	case 12: case 62:		return ITEM_CRITICAL_P;
	case 13: case 63:		return ITEM_CRITICAL_R;
	case 14: case 64:		return ITEM_STUN_P;
	case 15: case 65:		return ITEM_STUN_R;
	case 10: case 60:		return ITEM_STIFFNESS_P;
	case 11: case 61:		return ITEM_STIFFNESS_R;
	case 25: case 75:		return ITEM_MAXHP;
	case 26: case 76:		return ITEM_MAXSP;
	case 54: case 55:		return ITEM_ATTACK_P;
	case 56: case 57:		return ITEM_ATTACK_M;
	case 58:				return ITEM_DEFENSE_P;
	case 59:				return ITEM_DEFENSE_M;
	case 74:				return ITEM_MOVE;
	case 77:				return ITEM_RECOVERSP;
	case 16:				return ITEM_FIRE_A;
	case 17:				return ITEM_ICE_A;
	case 18:				return ITEM_LIGHT_A;
	case 19:				return ITEM_DARK_A;
	case 20:				return ITEM_FIRE_R;
	case 21:				return ITEM_ICE_R;
	case 22:				return ITEM_LIGHT_R;
	case 23:				return ITEM_DARK_R;
	default:
		{
			_ASSERT(0);
			return STATIC_INVALID;
		}
	}

	return STATIC_INVALID;
}

void CDnTooltipDlg::SetItemPrefixSystemInfo(CDnItem *pItem, int nSkillID, int nSkillLevel)
{
	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	DNTableFileFormat*  pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TSKILLLEVEL );
	if( !pSkillLevelTable )
		return;

	//스킬 레벨 데이터에서 skilID에 해당하는 리스트를 구하고
	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList ) <= 0 ) 
		return;

	//해당 스킬레벨에 맞는 스킬레벨테이블ID를 구한다.
	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();

#ifdef PRE_ADD_PREFIXSKILL_PVP
		int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_ApplyType" )->GetInteger();
		if( iNowLevel == nSkillLevel && ( (m_bTooltipSwitch && iApplyType == CDnSkill::PVP) || (!m_bTooltipSwitch && iApplyType == CDnSkill::PVE ) ) )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
#else
		if( iNowLevel == nSkillLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
#endif

	}

	if (iSkillLevelTableID == -1)
		return;

	int nSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

	std::wstring msg;
	char *szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
	MakeUIStringUseVariableParam( msg, nSkillExplainStringID, szParam );

	m_pTextBox->AddColorText(msg.c_str());
}


void CDnTooltipDlg::SetSkillLevelUpItemInfo(CDnItem *pItem)
{
	if (pItem == NULL)
		return;

	int nSkillID = -1;
	int nLevelUp = 0;
	int nSkillUsingType = 0;

	if (!pItem->HasLevelUpInfo(nSkillID, nLevelUp, nSkillUsingType))
		return;

	if (CDnItem::ItemSkillApplyType::SkillLevelUp != nSkillUsingType)
		return;

	if (nSkillID == 0 || nLevelUp == 0)
		return;

	DNTableFileFormat*  pSkillTable = GetTableDB().GetTable( CDnTableDB::TSKILL );
	if( !pSkillTable )
		return;

	int iStringID = 0;
	iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
	std::wstring skillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );

	int needJobID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NeedJob")->GetInteger();
	DNTableFileFormat*  pJobTable = GetTableDB().GetTable( CDnTableDB::TJOB );
	if( !pJobTable )
		return;

	int iJobNameID = 0;
	if (needJobID != 0)
		iJobNameID = pJobTable->GetFieldFromLablePtr(needJobID, "_JobName")->GetInteger();
		
	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	std::wstring msg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2210);
	m_pTextBox->AddText(msg.c_str());

	//스킬 정보 (덤블링: +1)
	WCHAR wszLevelInfo[64] = {0,};
	swprintf_s( wszLevelInfo, _countof( wszLevelInfo ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2211 ), skillName.c_str(), nLevelUp );

	//최종 메시지 초기화
	msg.clear();

	//직업이 필요한 스킬이라면 필요 직업 정보 설정
	std::wstring needJobName = L"";
	if (iJobNameID != 0)
	{
		needJobName = L"[";
		needJobName += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iJobNameID);
		needJobName += L"] ";

		msg = needJobName;
	}

	msg += wszLevelInfo;
	m_pTextBox->AddText(msg.c_str());
}

void CDnTooltipDlg::SetItemText_CombinedShop( CDnItem * pItem, int nTabID )
{
	CDnTradeItem::TCombinedShopTableData * pData = GetTradeTask().GetTradeItem().GetCombinedStoreItem( nTabID, pItem->GetSlotIndex() );

	if( NULL == pData )
		return;

	std::wstring wszLastLine;
	wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1764 ), EtInterface::textcolor::ORANGE );	// UISTRING : [구입 필요]

#if defined( PRE_ADD_LIMITED_SHOP )
	bool bGuildLimitedItem = false;
#endif
	for( int itr = 0; itr < Shop::Max::PurchaseType; ++itr )
	{
		if( Shop::PurchaseType::Gold == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseGold( pData->PurchaseType[itr].iPurchaseItemValue );
		}
#ifdef PRE_ADD_NEW_MONEY_SEED
		else if( Shop::PurchaseType::Seed == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseSeed( pData->PurchaseType[itr].iPurchaseItemValue );
		}
#endif // PRE_ADD_NEW_MONEY_SEED
		else if( Shop::PurchaseType::ItemID == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseItem( pData->PurchaseType[itr].iPurchaseItemID, pData->PurchaseType[itr].iPurchaseItemValue );
		}
		else if( Shop::PurchaseType::LadderPoint == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseLadderPoint( pData->PurchaseType[itr].iPurchaseItemValue );
		}
		else if( Shop::PurchaseType::GuildPoint == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseGuildPoint( pItem->GetClassID(), pData->PurchaseType[itr].iPurchaseItemValue );
#if defined( PRE_ADD_LIMITED_SHOP )
			bGuildLimitedItem = true;
#endif
		}
		else if( Shop::PurchaseType::UnionPoint == pData->PurchaseType[itr].PurchaseType )
		{
			SetCombinedShop_PurchaseUnionPoint( pData->PurchaseType[itr].iPurchaseItemID ,pData->PurchaseType[itr].iPurchaseItemValue );
		}
	}

#if defined( PRE_ADD_LIMITED_SHOP )
	if(!bGuildLimitedItem)
	{
#if defined( PRE_FIX_74404 )
		SetCombinedShop_PurchaseLimitedShopItem( pData->nShopID, pItem->GetClassID(), pData->nBuyLimitCount, pData->nShopLimitReset);
#else // #if defined( PRE_FIX_74404 )
		SetCombinedShop_PurchaseLimitedShopItem( pItem->GetClassID(), pData->nBuyLimitCount, pData->nShopLimitReset);
#endif // #if defined( PRE_FIX_74404 )
	}
#endif

	if( Shop::PurchaseLimitType::None == pData->PurchaseLimitType )
		return;

	m_pTextBox->AddText( L"" );
	m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1765 ), EtInterface::textcolor::ORANGE );	// UISTRING : [구입 조건]
	if( Shop::PurchaseLimitType::JobID == pData->PurchaseLimitType )
	{
		SetCombinedShop_PurchaseLimitJob( pData->iPuschaseLimitValue );
	}
	else if( Shop::PurchaseLimitType::Level == pData->PurchaseLimitType )
	{
		SetCombinedShop_PurchaseLimitLevel( pData->iPuschaseLimitValue );
	}
	else if( Shop::PurchaseLimitType::PvPRank == pData->PurchaseLimitType )
	{
		SetCombinedShop_PurchaseLimitPvPRank( pData->iPuschaseLimitValue );
	}
	else if( Shop::PurchaseLimitType::GuildLevel == pData->PurchaseLimitType )
	{
		SetCombinedShop_PurchaseLimitGuildLevel( pData->iPuschaseLimitValue );
	}
	else if( Shop::PurchaseLimitType::LadderGradePoint == pData->PurchaseLimitType )
	{
		SetCombinedShop_PurchaseLimitLadderGrade( pData->iPuschaseLimitValue );
	}
}

void CDnTooltipDlg::SetCombinedShop_PurchaseGold( int nMoney, bool bNoBenefit )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	std::wstring wszMoney;
	DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nMoney, wszMoney );
	swprintf_s( wszString, _countof(wszString), L"%s", wszMoney.c_str() );

	int storeBenefitAmount = nMoney;
	bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::BuyingPriceDiscount, storeBenefitAmount );
	if( bNoBenefit == false && bAvailBenefit )
	{
		if( GetItemTask().GetCoin() < storeBenefitAmount )
			dwColor = textcolor::RED;
		else
			dwColor = textcolor::WHITE;

		DN_INTERFACE::UTIL::GetMoneyFormatUseStr( storeBenefitAmount, wszMoney );
		swprintf_s( wszString, _countof(wszString), L"%s", wszMoney.c_str() );
		m_pTextBox->AddText( wszString, dwColor );
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1767 ), dwColor );
	}
	else
	{
		if( GetItemTask().GetCoin() < nMoney )
			dwColor = textcolor::RED;
		else
			dwColor = textcolor::WHITE;

		m_pTextBox->AddText( wszString, dwColor );
	}
}

#ifdef PRE_ADD_NEW_MONEY_SEED
void CDnTooltipDlg::SetCombinedShop_PurchaseSeed( int nSeed )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, 512, L"%d%s", nSeed, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4967 ) );

	if( GetItemTask().GetSeed() < nSeed )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
}
#endif // PRE_ADD_NEW_MONEY_SEED

void CDnTooltipDlg::SetCombinedShop_PurchaseItem( int nItemID, int nCount )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	std::wstring wszNeedItemName = CDnItem::GetItemFullName( nItemID );
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 80 ), wszNeedItemName.c_str(), nCount );	// UISTRING : %s %d개

	const int nInvenCount = GetItemTask().GetCharInventory().GetItemCount( nItemID );

	if( nInvenCount < nCount )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
}

void CDnTooltipDlg::SetCombinedShop_PurchaseLadderPoint( int nPoint )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	nPoint = GetTradeTask().GetTradeItem().CalcPurchaseLadderPoint(nPoint);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1769 ), nPoint );	// UISTRING : %d 래더 포인트

	const int nMyPoint = CDnPartyTask::GetInstance().GetPVPLadderInfo()->Data.iPvPLadderPoint;

	if( nMyPoint < nPoint )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
}

void CDnTooltipDlg::SetCombinedShop_PurchaseLimitLadderGrade( int nGrade )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1776 ), nGrade );	// UISTRING : 래더 평점 %d점 이상

	for( int itr = 0; itr < LadderSystem::MatchType::MaxCount; ++itr )
	{
		const int nCurLadderPoint = CDnPartyTask::GetInstance().GetPVPLadderInfo()->Data.LadderScore[itr].iPvPLadderGradePoint;

		if( nCurLadderPoint < nGrade )
			dwColor = textcolor::RED;
		else
		{
			dwColor = textcolor::WHITE;
			break;
		}
	}
	m_pTextBox->AddText( wszString, dwColor );
}


void CDnTooltipDlg::SetCombinedShop_PurchaseGuildPoint( int nItemID, int nPoint )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1771 ), nPoint );	// UISTRING : %d 길드축제 포인트

	const int nMyPoint = static_cast<int>( CDnGuildWarTask::GetInstance().GetGuildWarFestivalPoint() );

	if( nMyPoint < nPoint )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TCOMBINEDSHOP );
	if( pSox == NULL ) return;

	int nMaxItemCount = 0;
	for( int i=0; i<pSox->GetItemCount(); ++i )
	{
		int nTableID = pSox->GetItemID( i );
		int nTableItemID = pSox->GetFieldFromLablePtr( nTableID, "_itemindex" )->GetInteger();
		int nPurchaseType = pSox->GetFieldFromLablePtr( nTableID, "_PurchaseType1" )->GetInteger();

		if( nPurchaseType == Shop::PurchaseType::GuildPoint && nTableItemID == nItemID )
		{
			nMaxItemCount = pSox->GetFieldFromLablePtr( nTableID, "_buyLimitCount" )->GetInteger();
			break;
		}
	}

	if( nMaxItemCount > 0 )
	{
		m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7617 ), EtInterface::textcolor::ORANGE );	// UISTRING : [아이템 구입 가능 개수]
		int nCurrentItemBuyCount = GetGuildWarTask().GetGuildWarShopItemCurrentBuyCount( nItemID );
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7616 ), nMaxItemCount - nCurrentItemBuyCount, nCurrentItemBuyCount, nMaxItemCount );
		m_pTextBox->AddColorText( wszString, dwColor );
	}
}

void CDnTooltipDlg::SetCombinedShop_PurchaseUnionPoint( int eType, int nPoint )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1770 ), nPoint );	// UISTRING : %d 연합 포인트

#if defined(PRE_UNIONSHOP_RENEWAL)
	if( NpcReputation::UnionType::Etc <= eType || eType < NpcReputation::UnionType::Commercial )
		return;

	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if (pReputationRepos == NULL)
		return;

	int nMyPoint = (int)pReputationRepos->GetUnionPoint( eType );

	if( nMyPoint < nPoint )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
#else
	int nMyPoint(0);
	CDnStoreTabDlg *pStoreDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
	if( pStoreDlg && pStoreDlg->IsShow() )
	{
		const CEtUIRadioButton* pCurBtn = pStoreDlg->GetCurrentTabControl();
		if (pCurBtn)
		{
			const std::wstring& btnText = pCurBtn->GetText();
			DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
			if (pUnionTable == NULL)
				return;

			int count = pUnionTable->GetItemCount();
			int i = 0;
			for (; i < count; ++i)
			{
				int idx = pUnionTable->GetItemID(i);
				int uiStringNum = pUnionTable->GetFieldFromLablePtr(idx, "_TabNameID")->GetInteger();
				const wchar_t* uiString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, uiStringNum);
				if (btnText.compare(uiString) == 0)
				{
					CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
					if( !pQuestTask ) 
						return;

					CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
					if (pReputationRepos == NULL)
						return;

					nMyPoint = (int)pReputationRepos->GetUnionPoint(CommonUtil::ConvertUnionTableIDToUnionType(idx));
					break;
				}
			}
		}

		if( nMyPoint < nPoint )
			dwColor = textcolor::RED;
		else
			dwColor = textcolor::WHITE;

		m_pTextBox->AddText( wszString, dwColor );
	}
#endif	// #if defined(PRE_UNIONSHOP_RENEWAL)
}

#if defined( PRE_ADD_LIMITED_SHOP )
#if defined( PRE_FIX_74404 )
void CDnTooltipDlg::SetCombinedShop_PurchaseLimitedShopItem( int nShopID, int nItemID, int nLimitCount, int nShopLimitReset )
#else // #if defined( PRE_FIX_74404 )
void CDnTooltipDlg::SetCombinedShop_PurchaseLimitedShopItem( int nItemID, int nLimitCount, int nShopLimitReset )
#endif // #if defined( PRE_FIX_74404 )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);
	

	if( nLimitCount > 0 && nShopLimitReset > 0 )
	{
		m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7617 ), EtInterface::textcolor::ORANGE );	// UISTRING : [아이템 구입 가능 개수]
#if defined( PRE_FIX_74404 )
		int nCurrentItemBuyCount = GetItemTask().GetLimitedShopItemCount( nShopID, nItemID );
#else // #if defined( PRE_FIX_74404 )
		int nCurrentItemBuyCount = GetItemTask().GetLimitedShopItemCount( nItemID );
#endif // #if defined( PRE_FIX_74404 )
		if(nShopLimitReset == LimitedShop::LimitedItemType::Day)
			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7621 ), nLimitCount - nCurrentItemBuyCount, nCurrentItemBuyCount, nLimitCount );
		if(nShopLimitReset == LimitedShop::LimitedItemType::Week)
			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7622 ), nLimitCount - nCurrentItemBuyCount, nCurrentItemBuyCount, nLimitCount );
		m_pTextBox->AddColorText( wszString, dwColor );
	}
}
#endif

void CDnTooltipDlg::SetCombinedShop_PurchaseLimitJob( int nJobID )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1772 ), DN_INTERFACE::STRING::GetJobString(nJobID) );	// UISTRING : %s 제한

	if( !CDnActor::s_hLocalActor )
		return;

	const int nMyJobID = CDnActor::s_hLocalActor->OnGetJobClassID();

	if( GetEnableJob( nJobID, nMyJobID ) )
		dwColor = textcolor::WHITE;
	else
		dwColor = textcolor::RED;

	m_pTextBox->AddText( wszString, dwColor );
}

void CDnTooltipDlg::SetCombinedShop_PurchaseLimitLevel( int nLevel )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1773 ), nLevel );	// UISTRING : 레벨 %d 이상

	if( !CDnActor::s_hLocalActor )
		return;

	const int nMyLevel = CDnActor::s_hLocalActor->GetLevel();

	if( nMyLevel < nLevel )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
}

void CDnTooltipDlg::SetCombinedShop_PurchaseLimitPvPRank( int nRank )
{
	WCHAR wszString[512], wszRankName[128];
	DWORD dwColor(textcolor::WHITE);

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
	int nUIString = 0;

	if( pSox && pSox->IsExistItem( nRank ) )
		nUIString = pSox->GetFieldFromLablePtr( nRank, "PvPRankUIString" )->GetInteger();
	swprintf_s( wszRankName, _countof(wszRankName), L" %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ) );

	TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
	if( pPvPInfo->cLevel < nRank )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1774 ), wszRankName );	// UISTRING : 콜로세움 계급 %s 이상
	m_pTextBox->AddText( wszString, dwColor );
}

void CDnTooltipDlg::SetCombinedShop_PurchaseLimitGuildLevel( int nLevel )
{
	WCHAR wszString[512];
	DWORD dwColor(textcolor::WHITE);

	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1775 ), nLevel );	// UISTRING : 길드 레벨 %d 이상

	const int nMyLevel = static_cast<int>( GetGuildTask().GetGuildInfo()->wGuildLevel );

	if( nMyLevel < nLevel )
		dwColor = textcolor::RED;
	else
		dwColor = textcolor::WHITE;

	m_pTextBox->AddText( wszString, dwColor );
}

bool CDnTooltipDlg::GetEnableJob( int nNeedJobID, int nMyJobID )
{
	if( nNeedJobID == nMyJobID )
		return true;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );

	if( !pSox )
		return false;

	std::vector<int> vecParentJobID;
	vecParentJobID.push_back( nNeedJobID );

	while( !vecParentJobID.empty() )
	{
		std::vector<int> vecTemp = vecParentJobID;
		vecParentJobID.clear();

		for( int itr = 0; itr < (int)vecTemp.size(); ++itr )
		{
			for( int jtr = 0; jtr < pSox->GetItemCount(); ++jtr )
			{
				int nParentID = pSox->GetFieldFromLablePtr( jtr, "_ParentJob" )->GetInteger();

				if( vecTemp[itr] == nParentID )
					vecParentJobID.push_back( jtr );
			}
		}

		for( int itr = 0; itr < (int)vecParentJobID.size(); ++itr )
		{
			if( vecParentJobID[itr] == nMyJobID )
				return true;
		}
	}

	return false;
}

bool CDnTooltipDlg::SetItemText_CashItem_SkillLevelupInfo(CDnItem *pItem)
{
	if (pItem == NULL)
		return false;

	//옵션값이 설정이 되어 있으면 옵션 값을 확인한다.
	int nTypeParam = pItem->GetTypeParam(0);
	int nOptionIndex = pItem->GetOptionIndex();

	if (nTypeParam > 0)
	{
		DNTableFileFormat* pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
		DNTableFileFormat*  pSkillTable = GetTableDB().GetTable( CDnTableDB::TSKILL );
		if (pPotentialSox == NULL || pSkillTable == NULL)
			return false;

		std::vector<int> nVecItemID;
		pPotentialSox->GetItemIDListFromField( "_PotentialID", nTypeParam, nVecItemID );

		if (nVecItemID.empty() == true)
			return false;
		
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4859), textcolor::WHITE);	// UISTRING : 스킬 능력치
		
		//선택 가능한 스킬 리스트 정보 추가
		std::vector<int>::const_iterator iter = nVecItemID.begin();
		for( ; iter!=nVecItemID.end(); iter++ )
		{
			int itemId = *iter;
			int nSkillID = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillID" )->GetInteger();
			int nSkillLevel = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillLevel" )->GetInteger();
			int nSkillUsingType = pPotentialSox->GetFieldFromLablePtr( itemId, "_SkillUsingType" )->GetInteger();

			if (nSkillUsingType != CDnItem::ItemSkillApplyType::SkillLevelUp)
				continue;

			int iStringID = 0;
			iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
			std::wstring skillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );

			std::wstring strSkill;
			WCHAR wszLevelInfo[64] = {0,};
			swprintf_s( wszLevelInfo, _countof( wszLevelInfo ), L"%s +%d", skillName.c_str(), nSkillLevel );
			strSkill = wszLevelInfo;

			m_pTextBox->AddText( strSkill.c_str(), textcolor::WHITE );
		}

		m_pTextBox->AddText(L"");
		AddDivisionLine();

		return true;
	}

	return false;
}

void CDnTooltipDlg::UpdateSourceItemDescAndDurationTime( CDnItem * pItem )
{
	const TCHAR* pDescription = pItem->GetSkillDescription();
	if( pDescription )
	{
		m_pTextBox->AddColorText( pDescription );
		m_pTextBox->AddText( L"" );
	}

	// (UIString:5158) 효과유지시간:
	wstring strDurationTimeString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5158 );
	strDurationTimeString.append( L": " );

	// 인벤에 있을 경우 아이템을 사용했을 때 버프가 주어질 시간의 양을 표시.
	if( ST_INVENTORY == m_itemSlotType )
	{
		wstring strDurationTime;
		int iDurationTimeByMS = pItem->GetSkillSEDurationTime();
		CDnItem::MakeItemDurationString(iDurationTimeByMS, strDurationTime);

		m_pTextBox->AddText( (strDurationTimeString + strDurationTime).c_str() );
	}
	else
	if( ST_CHARSTATUS == m_itemSlotType )
	{
		// 이미 사용한 경우 현재 남은 시간 표시
		wstring strDurationTime;
		int iUsedSourceItemDurationTime = GetItemTask().GetUsedSourceItemDurationTime( pItem->GetClassID() );
		CDnItem::MakeItemDurationString( iUsedSourceItemDurationTime, strDurationTime );

		m_pTextBox->AddText( (strDurationTimeString + strDurationTime).c_str() );
	}

	// 근원 아이템 공통 툴팁 출력
	m_pTextBox->AddText( L"" );
	m_pTextBox->AddColorText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5159), textcolor::YELLOW );
}

void CDnTooltipDlg::SetItemText_RepurchaseSellPrice(CDnItem* pItem)
{
	if (pItem == NULL)
	{
#ifdef _WORK
		_ASSERT(0);
		m_pTextBox->AddColorText(L"RSP : NULL Item", textcolor::RED);
#endif
		return;
	}

	CDnStoreTabDlg* pStoreTabDlg = static_cast<CDnStoreTabDlg*>(GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG ));
	if (pStoreTabDlg == NULL || pStoreTabDlg->IsShowRepurchasableTab() == false)
	{
#ifdef _WORK
		_ASSERT(0);
		m_pTextBox->AddColorText(L"RSP : Repurchase Tab ERROR", textcolor::RED);
#endif
		return;
	}

	const CDnStoreRepurchaseSystem& repurchaseSystem = GetTradeTask().GetTradeItem().GetStoreRepurchaseSystem();
	int repurchasePrice = repurchaseSystem.GetRepurchasePrice(*pItem);
	if (repurchasePrice == CDnStoreRepurchaseSystem::eINVALID_PRICE)
	{
#ifdef _WORK
		_ASSERT(0);
		m_pTextBox->AddColorText(L"RSP : Repurchase PRICE ERROR", textcolor::RED);
#endif
		return;
	}

	std::wstring wszLastLine;
	wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1764 ), EtInterface::textcolor::ORANGE );	// UISTRING : [구입 필요]
	SetCombinedShop_PurchaseGold(repurchasePrice, true);
}

#ifdef PRE_ADD_SALE_COUPON

bool CDnTooltipDlg::SetItemText_CouponSaleItemList( const CDnItem* pItem )
{
	if( pItem == NULL )
		return false;

	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039878 ), textcolor::FONT_ORANGE );	// UISTRING : 사용 가능 아이템

	int nCouponID = pItem->GetTypeParam( 0 );

	DNTableFileFormat* pCouponSox = GetDNTable( CDnTableDB::TSALECOUPON );
	if( pCouponSox == NULL )
		return false;

	char szLabel[32]={0};
	wchar_t szTemp[256]={0};

	for( int i=0; i<MAX_COUPON_SALEITEM; i++ )
	{
		if( i+1 < 10 )
			sprintf_s( szLabel, "_UseItem0%d", i+1 );
		else
			sprintf_s( szLabel, "_UseItem%d", i+1 );

		int nSaleItemID = pCouponSox->GetFieldFromLablePtr( nCouponID, szLabel )->GetInteger();

		if( nSaleItemID == 0 )
			continue;

		const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo( nSaleItemID );
		if( pInfo == NULL )
			continue;

		std::wstring strCount = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625 ), pInfo->count );	// UISTRING : %d 개
		std::wstring strPeriod = (pInfo->period <= 0) ? GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4705 ) :
							FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4640 ), pInfo->period );	// UISTRING : 무제한 / UISTRING : %d 일

		swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4865 ), pInfo->nameString.c_str(), strCount.c_str(), strPeriod.c_str() );
		m_pTextBox->AddText( szTemp );
	}

	return true;
}

#endif // PRE_ADD_SALE_COUPON



#ifdef PRE_ADD_48682
// 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화.
void CDnTooltipDlg::SetItemText_AllowMapType( CDnItem * pItem )
{	
	// 컬럼의 비트값을 이용하여 사용가능지역 텍스트를 추가한다.
	std::map< UINT, std::wstring >::iterator it;
	std::wstring strMap;
	DNTableFileFormat * pTable = CDnTableDB::GetInstance().GetTable( CDnTableDB::TableEnum::TITEM );
	if( pTable )
	{
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( pItem->GetClassID(), "_AllowMapTypes" );
		if( pCell )
		{
			UINT nBitFlag = (UINT)pCell->GetInteger();

			// 사용가능.
			if( nBitFlag > 0 )
			{				
				// 0 은 사용불가라서 제외.
				size_t size = m_vAllowMapBitFlag.size();
				for( size_t i=1; i<size; ++i )
				{
					// 비트값 검사.
					UINT flag = m_vAllowMapBitFlag[ i ];
					if( (nBitFlag & flag) != flag )
						continue;

					// 비트값에 해당하는 텍스트.
					it = m_mapStrMapType.find( flag );
					if( it != m_mapStrMapType.end() )
					{						
						if( strMap.empty() )
						{							
							strMap.append( (*it).second );
						}
						// 1개이상 추가. ',' 콤마를 앞에 붙임.
						else
						{
							strMap.append( L", " );
							strMap.append( (*it).second );
						}						
					}
				}//for.

				// 최종 툴팁텍스트 추가.				
				this->m_pTextBox->DoAddText( L"", L"\n"); // #53478 AppendText => DoAddText 로 변경.
				this->m_pTextBox->DoAddText( L"", L"\n"); // #53478 AppendText => DoAddText 로 변경.

				this->m_pTextBox->AppendText( m_strAllowZone.c_str(), descritioncolor::SKY );				
				this->m_pTextBox->DoAddText( L"", L"\n"); // #53478 AppendText => DoAddText 로 변경.

				this->m_pTextBox->AppendText( strMap.c_str(), descritioncolor::GREEN );							
			}

		}

	}// if( pTable )
}
#endif

void CDnTooltipDlg::ResetToolTipSwitch()
{
	bool bEanble = false;

	if( CDnWorld::GetInstance().GetMapSubType() ==  CDnWorld::MapSubTypePvPVillage )
		bEanble = true;

	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) 
	{
		if (  pGameTask->GetGameTaskType() == GameTaskType::PvP )
		{
			bEanble = true;
		}
	}

	m_bTooltipSwitch = bEanble;
}

#if defined(PRE_ADD_63603)
void CDnTooltipDlg::SetInteractionInfo(MIInventoryItem *pInvenItem)
{
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	if( m_itemSlotType == ST_ITEM_NOTIFY_COMPOUND )
		return;
#endif

	if(pInvenItem == NULL)
		return;

	CDnItem* pItem = static_cast<CDnItem*>(pInvenItem);
	eItemTypeEnum itemType = pItem->GetItemType();

#ifdef PRE_ADD_COMPARETOOLTIP
	// 반지의 경우 아이템타입이 PARTS 이므로 강제로 ITEMTYPE_PARTS_RING 으로 변경한다.
	if( itemType == eItemTypeEnum::ITEMTYPE_PARTS )
	{
		CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
		if( pCharStatusDlg )
		{					
			int nSlotIndex = 0;
			
			if( pItem->IsCashItem() ) 
			{
				nSlotIndex = pCharStatusDlg->GetCashEquipIndex( pItem );
				if( nSlotIndex >= 0 && ( nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2 ) )
					itemType = eItemTypeEnum::ITEMTYPE_PARTS_RING;
			}
			else
			{
				nSlotIndex = pCharStatusDlg->GetEquipIndex( pItem );
				if( nSlotIndex >= 0 && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ) )
					itemType = eItemTypeEnum::ITEMTYPE_PARTS_RING;
			}					
		}
	}
#endif // PRE_ADD_COMPARETOOLTIP

	bool bInteractionInfo = true;
	if (itemType == eItemTypeEnum::ITEMTYPE_NORMAL)
	{
		if (pItem->GetSkillID() != 0)
		{
			//마우스 우클리 : 아이템 사용 표시.
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000047585 ) );

			//기본 아이템 타입별 테이블 설정은 표시 하지 않는다.
			bInteractionInfo = false;
		}
	}

#ifdef PRE_ADD_COMPARETOOLTIP
	// 착용반지가 두개이며, Shift를 누르고 있는동안에만 "Shift 키입력 : 다른슬롯 반지비교" 출력하도록한다.
	bool bAdd = true;
	if (itemType == eItemTypeEnum::ITEMTYPE_PARTS_RING && (m_bTwoRing == false || m_bShiftPress))
	{
		bAdd = false;
	}
#endif // PRE_ADD_COMPARETOOLTIP

	DNTableFileFormat* pTable = CDnTableDB::GetInstance().GetTable( CDnTableDB::TableEnum::TITEMUSEINFO );
	if(!pTable) return;

	// 텍스트 추가시에만 디비전추가.
	bool bAddDivision = true;

	if( bInteractionInfo == true
#ifdef PRE_ADD_COMPARETOOLTIP
		&& bAdd
#endif // PRE_ADD_COMPARETOOLTIP
		)
	{
		//아이템 타입별 테이블 설정 된 스트링 표시..		
		vector<int> vlItemuseInfoList;
		pTable->GetItemIDListFromField( "_Type", itemType, vlItemuseInfoList );
		vector<int>::iterator iter = vlItemuseInfoList.begin();
		for( iter; iter != vlItemuseInfoList.end(); )
		{
			int iTableID = *iter;
			int uiStringID = pTable->GetFieldFromLablePtr( iTableID, "_DescriptionID" )->GetInteger();
			
			// 텍스트 추가시에만 디비전추가.
			if( bAddDivision )
			{
				m_pTextBox->AddText( L"" );
				AddDivisionLine();
				bAddDivision = false;
			}

			m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, uiStringID ));

			++iter;
		}
	}

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	if(IsCashPartsItem(pItem) == true)
	{
		itemType = eItemTypeEnum::ITEMTYPE_SETITEM;
		
		int nTableID = pTable->GetItemIDFromField("_Type", itemType);
		if( nTableID > 0) 
		{
			// 텍스트 추가시에만 디비전추가.
			if( bAddDivision )
			{
				m_pTextBox->AddText( L"" );
				AddDivisionLine();
				bAddDivision = false;
			}

			int nMidNumber = pTable->GetFieldFromLablePtr( nTableID, "_DescriptionID" )->GetInteger();			
			m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMidNumber ), textcolor::FONT_GREEN);
		}
	}
#endif // PRE_ADD_OVERLAP_SETEFFECT


	// Shift + 마우스 우클릭: 아이템 나누기 표시
	// 캐쉬 아이템이 아닌 경우만
	if (pItem->GetOverlapCount() > 1 && pItem->IsCashItem() == false)
	{
		// 텍스트 추가시에만 디비전추가.
		if( bAddDivision )
		{
			m_pTextBox->AddText( L"" );
			AddDivisionLine();
			bAddDivision = false;
		}

		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000047586 ) );
	}

}
#endif // PRE_ADD_63603


#ifdef PRE_ADD_COMPARETOOLTIP
// #69087
void CDnTooltipDlg::PressShift( bool b )
{
	if( m_bShiftPress == b )
		return;

	m_bShiftPress = false;
#ifdef PRE_ADD_OVERLAP_SETEFFECT
	m_bSwitchOverlapToolTip = false;
#endif

	// 캐쉬샵무시.
	if( CDnCashShopTask::GetInstance().IsOpenCashShop() )
		return;

	// 캐릭터창 무시.
	if( !m_hSlotButton || (m_hSlotButton && m_hSlotButton->GetSlotType() == ITEM_SLOT_TYPE::ST_CHARSTATUS) )
		return;
	
	MIInventoryItem * miItem = m_hSlotButton->GetItem();
	if( !miItem )
		return;
	
	// 인벤아이템이 아니거나, PARTS가 아니면 무시.
	CDnItem * pSlotItem = (CDnItem *)miItem;
	//if( pSlotItem && ( pSlotItem->GetType() != MIInventoryItem::Item || pSlotItem->GetItemType() != ITEMTYPE_PARTS ) )
	if( pSlotItem && ( pSlotItem->GetType() != MIInventoryItem::Item || ( pSlotItem->GetItemType() != ITEMTYPE_PARTS && pSlotItem->GetItemType() != ITEMTYPE_WEAPON ) ) )
		return;	

	if( m_hSlotButton && m_pSrcItem )
	{
		m_bShiftPress = b;	

		ShowTooltip( m_hSlotButton, m_ringCompare.fX, m_ringCompare.fY, m_ringCompare.bAutoCompare, m_pTargetDiffItem );

		m_bTooltipSwitch ^= true;
		ClearText();
		ClearDivisionLine();		
		SetItemText( m_pSrcItem, false );
		

		// 아이템슬롯이 인벤토리가 아니면 무시.
		if( m_hSlotButton && ( m_hSlotButton->GetSlotType() == ITEM_SLOT_TYPE::ST_INVENTORY || m_hSlotButton->GetSlotType() == ITEM_SLOT_TYPE::ST_INVENTORY_CASH  ) )
		{
			// 살펴보기의 경우 무시.
			CDnInspectPlayerDlg * pInspect = GetInterface().GetInspectDialog();	
			if ( GetInterface().GetMainMenuDialog()->IsOpenDialog(CDnMainMenuDlg::INVENTORY_DIALOG) &&
				GetInterface().IsSkipInteraction() == false && ( pInspect && !pInspect->IsShow() ) )
				SetInteractionInfo( m_pSrcItem );		
		}

		UpdateTooltip( m_ringCompare.fX, m_ringCompare.fY );		

		// 
		if( m_pDestItem && m_pDestItem->GetType() == MIInventoryItem::Item )
		{
			CDnItem *pDestItem = static_cast<CDnItem*>(m_pDestItem);
			
			if( m_pCompareTooltipDlg && m_pCompareTooltipDlg->IsShow() )
			{
				int nItemType = pDestItem->GetItemType();
				int nSlotIndex =  pDestItem->GetSlotIndex();

				ShowCompareTooltip( pDestItem->IsCashItem(), (eItemTypeEnum)nItemType , nSlotIndex , m_pDestItem , m_pCompareTooltipDlg );
			}
		}
	}
}
#endif // PRE_ADD_COMPARETOOLTIP

#ifdef PRE_ADD_CASHSHOP_ACTOZ

// 캐시샾에서 청약철회 약관 출력.
void CDnTooltipDlg::SetItemText_CashRefund( CDnItem * pItem )
{
	if ( m_hSlotButton && m_hSlotButton->GetSlotType() == ST_INVENTORY_CASHSHOP_REFUND )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();

		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4962 ) );	// "해당 아이템은 구매일로부터 관련 법령 및 약관에 따라 7일이 경과하였거나..."
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
		m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4963 ), textcolor::FORESTGREEN ); // "<인벤토리로 이동:마우스 오른쪽 클릭>"		
	}
}

#endif // PRE_ADD_CASHSHOP_ACTOZ


#ifdef PRE_ADD_EQUIPLOCK
void CDnTooltipDlg::SetItemText_LockState(CDnItem* pItem)
{
	if (pItem == NULL)
		return;

	if (m_itemSlotType != ST_CHARSTATUS)
		return;

	eItemTypeEnum itemType = pItem->GetItemType();
	if (CommonUtil::IsLockableEquipItem(itemType) == false)
		return;

	std::wstring result;
	const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
	EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);

	if (status == EquipItemLock::Lock)
	{
		lockMgr.GetStateToolTipString(result, *pItem);
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8385), textcolor::FONT_ORANGE); // UISTRING : 상태:
		result = FormatW(L" %s", result.c_str());
		m_pTextBox->AppendText(result.c_str(), textcolor::RED);

		result.clear();
		lockMgr.GetLockDateToolTipString(result, *pItem);
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8386), textcolor::FONT_ORANGE); // UISTRING : 잠금일:
		result = FormatW(L" %s", result.c_str());
		m_pTextBox->AppendText(result.c_str(), textcolor::WHITE);

		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}
	else if (status == EquipItemLock::RequestUnLock)
	{
		lockMgr.GetStateToolTipString(result, *pItem);
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8385), textcolor::FONT_ORANGE); // UISTRING : 상태:
		result = FormatW(L" %s", result.c_str());
		m_pTextBox->AppendText(result.c_str(), textcolor::RED);

		result.clear();
		std::wstring releaseDate;
		lockMgr.GetUnLockDateToolTipString(result, releaseDate, *pItem);
		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8393), textcolor::FONT_ORANGE); // UISTRING : 해제일:
		result = FormatW(L" %s", result.c_str());
		m_pTextBox->AppendText(result.c_str(), textcolor::WHITE);

		m_pTextBox->AddText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8394), textcolor::FONT_ORANGE); // UISTRING : 해제 완료일:
		result = FormatW(L" %s", result.c_str());
		m_pTextBox->AppendText(releaseDate.c_str(), textcolor::WHITE);

		m_pTextBox->AddText(L"");
		AddDivisionLine();
	}
}
#endif

#ifdef PRE_ADD_OVERLAP_SETEFFECT

void CDnTooltipDlg::SetOnlySetItemText( CDnItem *pItem, DWORD dwFontColor )
{	// 세트 아이템 효과 및 설명 적는곳
	int		nSetItemID = 0;
	bool	bSimpleDescription	 = false;
	int		nSimpleDescriptionID = 4551;
	bool	bPetParts  = false;
	bool	bIsOverlap = false; // 다중 세트효과를 가지고 있는 아이템인가?	

	switch( pItem->GetItemType() ) 
	{
	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
			if( pWeapon )
			{
				nSetItemID = pWeapon->GetSetItemID();
				bSimpleDescription = pWeapon->IsSimpleSetItemDescription();
				nSimpleDescriptionID = pWeapon->GetSimpleSetItemDescriptionID();
				bIsOverlap = pWeapon->GetOverLapSet();
			}
		}
		break;

	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
			if( pParts )
			{
				nSetItemID = pParts->GetSetItemID();
				bSimpleDescription = pParts->IsSimpleSetItemDescription();
				nSimpleDescriptionID = pParts->GetSimpleSetItemDescriptionID();
				bIsOverlap = pParts->GetOverLapSet();
			}
		}
		break;

	case ITEMTYPE_PETPARTS:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
			if( !pSox ) return;
			if( !pSox->IsExistItem( pItem->GetClassID() ) ) return;

			nSetItemID = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_SetItemID" )->GetInteger();
			bSimpleDescription = ( pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_Grade_SetItem" )->GetInteger() == 1 ) ? true : false;
			nSimpleDescriptionID = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_SetItemTextID" )->GetInteger();
			bPetParts = true;
		}
		break;
	}

	if( !nSetItemID )
		return;

	
	DNTableFileFormat* pOverlapSox = GetDNTable(CDnTableDB::TOVERLAPSETEFFECT);
	if(!pOverlapSox) return;

	// Overlap아이템이 아니면, SetItemID는 한개.
	std::vector<int> vOverlapItemIDList;
	if(bIsOverlap == false)
		vOverlapItemIDList.push_back(nSetItemID);
	else
	{
		// Overlap 아이템이면, 지니고 있는 셋트 아이템ID를 모두 저장한다.
		char szTableLable[32] = { 0, };
		for(int i = 0 ; i < OVERLAP_SETITEM_MAX ; ++i) // 최대 10개
		{
			sprintf_s(szTableLable, "_SetItemID%d", i+1);
			int nTempID = pOverlapSox->GetFieldFromLablePtr(nSetItemID, szTableLable)->GetInteger();
			if( nTempID > 0)
				vOverlapItemIDList.push_back(nTempID);
		}		
	}

	// 세트아이템 전체 개수 구하기
	wchar_t szTemp[256]	  = { 0, };
	int nMaxSetItem		  = 0;
	int nMaxWeaponSetItem = 0;
	int nMaxPartsSetItem  = 0;
	vector<int> vWeaponSetItemIDList;
	vector<int> vPartsSetItemIDList;	

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	pSox->GetItemIDListFromField( "_SetItemID", nSetItemID, vWeaponSetItemIDList );
	nMaxWeaponSetItem = (int)vWeaponSetItemIDList.size();

	pSox = GetDNTable( CDnTableDB::TPARTS );
	pSox->GetItemIDListFromField( "_SetItemID", nSetItemID, vPartsSetItemIDList );
	nMaxPartsSetItem = (int)vPartsSetItemIDList.size();
	nMaxSetItem = nMaxWeaponSetItem + nMaxPartsSetItem;

	if( bPetParts )
	{
		nMaxSetItem = 2;
	}

	if( nMaxSetItem <= 0 )
	{
		ASSERT(0&&"세트아이템 총 갯수가 0이하입니다. 데이터가 이상합니다.");
	}

	// 세트아이템 리스트 구해놓고
	vector<int> vSetItemIDList = vPartsSetItemIDList;
	for( int i = 0; i < (int)vWeaponSetItemIDList.size(); ++i )
		vSetItemIDList.push_back(vWeaponSetItemIDList[i]);

	// 장착된 템 중 현재 세트아이템에 해당되는 개수 구한다.
	std::vector<int> vEquipedItemIDList;

	// 장착한 아이템중 동일한 세트 아이템 갯수
	std::vector<int> vNumEquipedSetItem;

	// 만약 살펴보기 중이면서 기본툴팁 윈도우라면, 상대방 장착 리스트를 얻어온다.
	// 그 외 경우엔 원래대로 자신의 장비를 찍는다.
	//if( GetDialogType() == UI_TYPE_TOP )	// 이렇게 기본툴팁인지 검사해도 된다.
	if( this != m_pCompareTooltipDlg && this != m_pCompareTooltipDlg2 && m_hSlotButton && m_hSlotButton->GetSlotType() == ST_INSPECT )
	{
		if( GetInterface().GetInspectDialog() )
		{
			std::vector<CDnInspectPlayerDlg::SEquipInfo> vecEquipInfo;
			GetInterface().GetInspectDialog()->GetEquipInfo(vecEquipInfo);

			for(int i = 0; i < (int)vecEquipInfo.size(); ++i )
				vEquipedItemIDList.push_back(vecEquipInfo[i].nClassID);

			for(int j = 0 ; j < (int)vOverlapItemIDList.size() ; ++j)
			{
				int nCnt = 0;

				for(int i = 0; i < (int)vecEquipInfo.size(); ++i)
				{
					if(vOverlapItemIDList[j] == vecEquipInfo[i].nSetItemID)
					{
						++nCnt;
					}
				}

				if(nCnt > 0)
					vNumEquipedSetItem.push_back(nCnt);
			}
		}
	}
	else
	{
		for( int j = 0 ; j < (int)vOverlapItemIDList.size() ; ++j)
		{
			int nMaxCount  = 0;
			int nSetItemID = vOverlapItemIDList[j];

			// Cash 무기 체크
			for( int i = 2; i < EQUIPMAX; ++i )
			{
				CDnItem *pEquipedItem = GetItemTask().GetEquipItem(i);
				if(!pEquipedItem ) continue;
				
				if( pEquipedItem->GetItemType() == ITEMTYPE_WEAPON )
				{
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pEquipedItem);
					vEquipedItemIDList.push_back(pWeapon->GetClassID());

					std::vector<int> vEquipedItemList;
					GetOverlapID(pWeapon->GetOverLapSet(), pWeapon->GetSetItemID(), vEquipedItemList);
					ComputeEquipedSetItemCount(nSetItemID, vEquipedItemList, nMaxCount);
				}
				else if( pEquipedItem->GetItemType() == ITEMTYPE_PARTS )
				{
					CDnParts *pParts = dynamic_cast<CDnParts *>(pEquipedItem);
					vEquipedItemIDList.push_back(pParts->GetClassID());

					std::vector<int> vEquipedItemList;
					GetOverlapID(pParts->GetOverLapSet(), pParts->GetSetItemID(), vEquipedItemList);
					ComputeEquipedSetItemCount(nSetItemID, vEquipedItemList, nMaxCount);					
				}
			}

			// Cash 장비 체크
			for( int i = 0 ; i < CASHEQUIPMAX ; ++i )
			{
				if(i == CASHEQUIP_EFFECT)
					continue;

				CDnItem *pEquipedItem = GetItemTask().GetCashEquipItem(i);
				if( !pEquipedItem ) continue;
				if( pEquipedItem->GetItemType() == ITEMTYPE_WEAPON )
				{
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pEquipedItem);
					vEquipedItemIDList.push_back(pWeapon->GetClassID());

					std::vector<int> vEquipedItemList;
					GetOverlapID(pWeapon->GetOverLapSet(), pWeapon->GetSetItemID(), vEquipedItemList);
					ComputeEquipedSetItemCount(nSetItemID, vEquipedItemList, nMaxCount);
				}
				else if( pEquipedItem->GetItemType() == ITEMTYPE_PARTS )
				{
					CDnParts *pParts = dynamic_cast<CDnParts *>(pEquipedItem);
					vEquipedItemIDList.push_back(pParts->GetClassID());

					std::vector<int> vEquipedItemList;
					GetOverlapID(pParts->GetOverLapSet(), pParts->GetSetItemID(), vEquipedItemList);
					ComputeEquipedSetItemCount(nSetItemID, vEquipedItemList, nMaxCount);
				}
			}

			//if(nMaxCount > 0)
				vNumEquipedSetItem.push_back(nMaxCount);
		}

		if( CDnActor::s_hLocalActor && bPetParts && m_itemSlotType == ST_CHARSTATUS )
		{
			CDnPlayerActor* pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();

			if( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nItemID &&
				pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nItemID )
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
				if( !pSox ) return;
				if( !pSox->IsExistItem( pItem->GetClassID() ) ) return;

				int nSetItemID1 = pSox->GetFieldFromLablePtr( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nItemID, "_SetItemID" )->GetInteger();
				int nSetItemID2 = pSox->GetFieldFromLablePtr( pActor->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nItemID, "_SetItemID" )->GetInteger();
				if( nSetItemID1 > 0 && nSetItemID2 > 0 && nSetItemID1 == nSetItemID2 )
				{
					vNumEquipedSetItem.clear();
					vNumEquipedSetItem.push_back(2);
					//nNumEquipedSetItem = 2;
				}
			}
		}
	}

	std::wstring wszLastLine = m_pTextBox->GetLastLineText();
	if( wszLastLine.size() )
	{
		m_pTextBox->AddText( L"" );
		AddDivisionLine();
	}

	// Mid: 세트 아이템
	/*swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2281 ) );
	m_pTextBox->AddText( szTemp, textcolor::FONT_ORANGE );*/
	
	// 세트 아이템 "설명" 출력
	//if(bIsOverlap && bSimpleDescription)
	//{
	//	nSimpleDescriptionID = pOverlapSox->
	//}

	//if( bSimpleDescription ) 
	//	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSimpleDescriptionID ), dwFontColor );
	//else 
	//{
	//	// 세트아이템 리스트
	//	std::wstring wszItemName;
	//	for( int i = 0; i < (int)vSetItemIDList.size(); ++i )
	//	{
	//		int nItemID = vSetItemIDList[i];
	//		wszItemName = CDnItem::GetItemFullName( nItemID );

	//		bool bEquiped = false;
	//		for( int j = 0; j < (int)vEquipedItemIDList.size(); ++j )
	//		{
	//			if( vEquipedItemIDList[j] == nItemID ) 
	//				bEquiped = true;
	//		}

	//		m_pTextBox->AddText( wszItemName.c_str(), bEquiped ? dwFontColor : textcolor::DARK );
	//	}
	//}

	pSox = GetDNTable( CDnTableDB::TSETITEM );
	char szLabel[32];
	int  nNumNeedSet;

	// 먼저 필요갯수에 따라 효과들을 묶는다.(물리공격력 Min, Max때문에.)
	// 인덱스는 필요셋갯수 이며 최대치는 10개. 0번인덱스, 1번인덱스는 사용하지 않는다.(1개짜리 세트아이템은 없으니)
	//std::vector<int> nVecStateList[11];

	// Mid: 세트효과 1 ~ 10
	int nMidStringArray[] = { 6006, 6007, 6012, 6013, 6014, 6015, 6016, 6017, 6018, 6019 };	
	int nMidMaxSize		  = sizeof(nMidStringArray) / sizeof(int);
	
	for(int k = 0 ; k < (int)vOverlapItemIDList.size() ; ++k)
	{				
		m_pTextBox->AddText(L"");
		
		// 세트아이템 "효과"
		if(nMidMaxSize > k)
			swprintf_s( szTemp, 256, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMidStringArray[k]) );
		m_pTextBox->AddText(szTemp, textcolor::FONT_ORANGE);

		// 세트아이템에 붙는 효과는 최대 12개.
		std::vector<int> nVecStateList[11];
		for(int i = 0 ; i < 12 ; i++)
		{
			sprintf_s( szLabel, "_NeedSetNum%d", i+1 );
			nNumNeedSet = pSox->GetFieldFromLablePtr( vOverlapItemIDList[k], szLabel )->GetInteger();
			if( nNumNeedSet < 2 ) 
				continue;
			nVecStateList[nNumNeedSet].push_back(i); // 효과 인덱스 기억.
		}

	#ifdef PRE_ADD_COSTUME_SKILL
		DNTableFileFormat *pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		if( !pSkillTable ) return;
		int nSkillNeedCount = pSox->GetFieldFromLablePtr( vOverlapItemIDList[k], "_NeedCSSetNum" )->GetInteger();
		int nSkillID = pSox->GetFieldFromLablePtr( vOverlapItemIDList[k], "_SkillID" )->GetInteger();
	#endif // PRE_ADD_COSTUME_SKILL

		// 필요셋개수대로 출력
		CDnState SetItemState;
		for(int i = 0 ; i < 11 ; ++i)
		{
			if( i < 2 || nVecStateList[i].size() == 0 ) continue;
			SetItemState.ResetState();

			for( int j=0; j<(int)nVecStateList[i].size(); ++j )
			{
				sprintf_s( szLabel, "_State%d", nVecStateList[i][j]+1 );
				int nStateType = pSox->GetFieldFromLablePtr( vOverlapItemIDList[k], szLabel )->GetInteger();
				sprintf_s( szLabel, "_State%dValue", nVecStateList[i][j]+1 );
				char *szValue = pSox->GetFieldFromLablePtr( vOverlapItemIDList[k], szLabel )->GetString();
				CDnItem::CalcStateValue( &SetItemState, nStateType, szValue, szValue, false );
			}
			swprintf_s( szTemp, 256, L" %d%s ", i, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2298 ) ); // Mid: 세트효과
			SetItemStateText( &SetItemState, (i <= vNumEquipedSetItem[k]) ? dwFontColor : textcolor::DARK, szTemp );

	#ifdef PRE_ADD_COSTUME_SKILL
			if( nSkillID > 0 && i == nSkillNeedCount )
			{
				int nSkillStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
				std::wstring skillName = FormatW( L": %s" , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSkillStringID ) );
				SetStateText( skillName.c_str(), (i <= vNumEquipedSetItem[k]) ? dwFontColor : textcolor::DARK, szTemp );
			}
	#endif // PRE_ADD_COSTUME_SKILL

		} // end of for i

	#ifdef PRE_ADD_COSTUME_SKILL
		if(nSkillID > 0)
		{
			m_pTextBox->AddText( L"" );
			AddDivisionLine();
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8357 ) , textcolor::YELLOW );
		}
	#endif // PRE_ADD_COSTUME_SKILL

		m_pTextBox->AddText(L"");

		// 세트 효과 설명 출력
		if(bIsOverlap && bSimpleDescription)
		{			
			sprintf_s(szLabel, 256, "_SetItemTextID%d", k+1);
			nSimpleDescriptionID = pOverlapSox->GetFieldFromLablePtr(nSetItemID, szLabel)->GetInteger();
		}

		if( bSimpleDescription ) 
			m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nSimpleDescriptionID ), dwFontColor );
		else 
		{
			// 세트아이템 리스트
			std::wstring wszItemName;
			for( int i = 0; i < (int)vSetItemIDList.size(); ++i )
			{
				int nItemID = vSetItemIDList[i];
				wszItemName = CDnItem::GetItemFullName( nItemID );

				bool bEquiped = false;
				for( int j = 0; j < (int)vEquipedItemIDList.size(); ++j )
				{
					if( vEquipedItemIDList[j] == nItemID ) 
						bEquiped = true;
				}

				m_pTextBox->AddText( wszItemName.c_str(), bEquiped ? dwFontColor : textcolor::DARK );
			}
		}
		m_pTextBox->AddText(L"");
		AddDivisionLine();

	} // end of for k

} // end of Function

void CDnTooltipDlg::PressAlt(bool bPush)
{
	ChangeOverlapSetItemToolTip(this, false, bPush);
	ChangeCompareOverlapSetItemToolTip(bPush);
}

void CDnTooltipDlg::ChangeCompareOverlapSetItemToolTip(bool bIsKeyPush)
{
	if(m_pCompareTooltipDlg && m_pCompareTooltipDlg->IsShow()) {
		ChangeOverlapSetItemToolTip(m_pCompareTooltipDlg, true, bIsKeyPush);
	}
}

void CDnTooltipDlg::ChangeOverlapSetItemToolTip(CDnTooltipDlg* pToolTip, bool bIsCompare, bool bIsKeyPush)
{
	if(pToolTip == NULL) 
		return;

	// 기존의 툴팁을 전부 지우고, 셋트 아이템만
	MIInventoryItem* pCurrentItem = pToolTip->GetToolTipOverItem(bIsCompare);
	if(pCurrentItem == NULL) 
		return;

	if(pCurrentItem->GetType() != MIInventoryItem::InvenItemTypeEnum::Item)
		return;

	bool bHaveSetItemID = false;
	CDnItem* pCurItem = static_cast<CDnItem*>(pCurrentItem);
	if(pCurItem)
	{
		switch(pCurItem->GetItemType())
		{
		case ITEMTYPE_PARTS:
			{
				CDnParts* pPart = dynamic_cast<CDnParts*>(pCurItem);
				if(pPart) bHaveSetItemID = (pPart->GetSetItemID() > 0);
			}
			break;

		case ITEMTYPE_WEAPON:
			{
				CDnWeapon* pWeapon = dynamic_cast<CDnWeapon*>(pCurItem);
				if(pWeapon) bHaveSetItemID = (pWeapon->GetSetItemID() > 0);
			}
			break;
		}
	}

	// Cash아이템만 툴팁을 바꾼다.
	if(pCurItem->IsCashItem() == false)
		return;

	pToolTip->m_bSwitchOverlapToolTip = bIsKeyPush; // 위치 중요


	eItemTypeEnum eItemType = pCurItem->GetItemType();
	if(bHaveSetItemID == false)
	{
		bool bResult = false;
		// 파츠나, 무기가 아니면 리턴( 일반 아이템: 물약같은 )
		if(eItemType == ITEMTYPE_PARTS || eItemType == ITEMTYPE_WEAPON || eItemType == ITEMTYPE_PARTS_RING)
			bResult = true;

		if(bResult == false)
			return;
	}

	// 클리어
	pToolTip->ClearText();
	pToolTip->ClearDivisionLine();

	CDnItem* pItem = static_cast<CDnItem*>(pCurrentItem);
	if(!pItem) return;

	//-------------------------------------------------------------------------------------
	// false인 경우: 일반툴팁으로 변경한 경우( 세트아이템 툴팁 --> 일반아이템 툴팁 )
	if(pToolTip->m_bSwitchOverlapToolTip == false)
	{
		pToolTip->SetTooltipText(pItem, bIsCompare);
		pToolTip->Update(bIsCompare);
		return;
	}
	//-------------------------------------------------------------------------------------

	// 아이템 이름 설정
	wstring wszItemName = pToolTip->GetItemName( pItem );
	pToolTip->m_pStaticName->SetTextColor( ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
	if( bIsCompare )
	{
		if( pToolTip->m_pTargetDiffItem )
			wszItemName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8066 );
		else 
		{
			SUICoord NameCoord, LineCoord;
			CalcTextRect( L" ", pToolTip->m_pStaticName->GetElement(0), LineCoord );
			CalcTextRect( wszItemName.c_str(), pToolTip->m_pStaticName->GetElement(0), NameCoord );
			if( NameCoord.fWidth < pToolTip->m_TextBoxDefaultCoord.fWidth && NameCoord.fHeight < LineCoord.fHeight * 2.0f )
				wszItemName += '\n';

#ifdef PRE_ADD_COMPARETOOLTIP
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			int nSlotIndex = -1;
			if( pItem->IsCashItem() )
				nSlotIndex = pCharStatusDlg->GetCashEquipIndex( pItem );
			else
				nSlotIndex = pCharStatusDlg->GetEquipIndex( pItem );
		
			bool bCashRingCheck = ((pItem->IsCashItem() && (nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2)) || (!pItem->IsCashItem() && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2)));
			if( !CDnCashShopTask::GetInstance().IsOpenCashShop() 
				&& pItem->GetType() == MIInventoryItem::Item && pItem->GetItemType() == ITEMTYPE_PARTS 
				&& bCashRingCheck )
			{				
				int strID = 2219; // "(반지1 장착중)"

				// 캐시.
				if( pItem->IsCashItem() && nSlotIndex == CASHEQUIP_RING2 )
					strID = 2237; // "(반지2 장착중)"
				// 일반.
				else if( nSlotIndex == EQUIP_RING2 )
					strID = 2237; // "(반지2 장착중)"				
				
				wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strID );
			}
			else
				wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2259 );

#else	// else PRE_ADD_COMPARETOOLTIP
			wszItemName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2259 );
#endif //  PRE_ADD_COMPARETOOLTIP
		}
	}

#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	int nItemCategoryType = pItem->GetCategoryType();
	if (nItemCategoryType != 0)
	{
		int descIndex = CItemCategoryInfo::GetInstance().GetDescIndex(nItemCategoryType);
		if (descIndex > 0)
		{
#ifdef PRE_MOD_ITEM_COMPOUND_TOOLTIP
			if( pItem->GetItemType() == ITEMTYPE_POTENTIAL_JEWEL )
			{
				CDnPotentialJewel* pPotentialItem = static_cast<CDnPotentialJewel*>(pItem);
				if( pPotentialItem && pPotentialItem->IsSuffix() )	
					descIndex = 1000047767;		// MID ( 접미사 ) 
			}
#endif
			wszItemName += FormatW(L"\n%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, descIndex));
		}
	}
#endif

#ifdef PRE_ADD_BESTFRIEND
	bool bDestroyFriend = false;
	// 절친반지 이름처리
	if( pItem->GetItemType() == ITEMTYPE_PARTS &&
		pItem->GetTypeParam(0) == 10 &&
		pItem->GetTypeParam(1) == 10 )
	{
		wchar_t strBF[256];
		CDnInspectPlayerDlg* pInspect = GetInterface().GetInspectDialog();
		if( pInspect && pInspect->IsShow() && pItem->GetSerialID() == pInspect->GetBFserial() )
		{
			// 살펴보기 : 다른플레이어의 반지.
			TCHAR * pBFname = pInspect->GetBestfriendName();
			if( pBFname )
			{
				swprintf_s( strBF, 256, wszItemName.c_str(), pBFname );
				pToolTip->m_pStaticName->SetText( strBF );
			}
		}
		else if( pItem->GetSerialID() == GetInterface().GetSerialBF() ) 
		{
			// 내반지.
			swprintf_s( strBF, 256, wszItemName.c_str(), GetInterface().GetNameBF() );
			pToolTip->m_pStaticName->SetText( strBF );		
		}
		else
		{	
			// "파괴된절친반지"
			bDestroyFriend = true;
			pToolTip->m_pStaticName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4349 ) );		
		}

	}
	else
		pToolTip->m_pStaticName->SetText( wszItemName.c_str() );
#else  // PRE_ADD_BESTFRIEND
	pToolTip->m_pStaticName->SetText( wszItemName.c_str() );
#endif // PRE_ADD_BESTFRIEND
	pToolTip->m_pStaticName->SetText( wszItemName.c_str() );

	////////////////////////////////////////////////////////////////////////////////////////////////

	// 귀속 표시, 파괴불가
	pToolTip->SetItemText_ExchangeAttribute(pItem);

	// 세트효과
	//if(bHaveSetItemID == false && bIsCompare == true) // 셋트 아이템이 아니면서, 비교 툴팁이다.
	if(bHaveSetItemID == false) // 세트 아이템이 아니라면..
	{
		pToolTip->m_pTextBox->AddText(L"");
		pToolTip->AddDivisionLine();
		pToolTip->m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6005), textcolor::FONT_GREEN); // Mid: 비교할 세트 효과가 없습니다.
		pToolTip->m_pTextBox->AddText(L"");
		pToolTip->AddDivisionLine();
	}
	else
		pToolTip->SetOnlySetItemText( pItem, textcolor::WHITE );

	// 아이템 능력치 보기 [ALT키]
	//pToolTip->m_pTextBox->AddText(L"");
	//pToolTip->m_pTextBox->AddText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6004 ), textcolor::FONT_GREEN); // Mid: 아이템 능력치 보기 [Alt키]

	// 업데이트
	pToolTip->Update(bIsCompare);
}

void CDnTooltipDlg::GetOverlapID(bool bIsOverlap, int nOverLapID, std::vector<int>& vIDList)
{
	if(bIsOverlap == false) {
		vIDList.push_back(nOverLapID);
	}
	else 
	{
		char szTableLable[32] = { 0 , };
		DNTableFileFormat* pOverlapSox = GetDNTable(CDnTableDB::TOVERLAPSETEFFECT);
		if(!pOverlapSox) return;

		for(int i = 0 ; i < OVERLAP_SETITEM_MAX ; ++i)
		{
			sprintf_s(szTableLable, "_SetItemID%d", i+1);
			int nTempID = pOverlapSox->GetFieldFromLablePtr(nOverLapID, szTableLable)->GetInteger();
			if( nTempID > 0)
				vIDList.push_back(nTempID);
		}
	}
}

void CDnTooltipDlg::ComputeEquipedSetItemCount(int nMouseOverSetItemID, std::vector<int> vEquipedItemList, int& nMaxCount)
{
	for(int i = 0 ; i < (int)vEquipedItemList.size() ; ++i) {
		if(nMouseOverSetItemID == vEquipedItemList[i]) {
			++nMaxCount;
		}
	}
}

void CDnTooltipDlg::Update(bool bCompare)
{
	if(bCompare)
		UpdateCompareToolTip();
	else
		UpdateTooltip(m_fUpdateOrigPosX, m_fUpdateOrigPosY);
}

MIInventoryItem* CDnTooltipDlg::GetToolTipOverItem(bool bCompare)
{
	if(bCompare == false)
	{
		if(m_pSrcItem)
			return m_pSrcItem;
	}
	else
	{
		if(m_pOverlapCompareItem) 
			return m_pOverlapCompareItem;
	}

	return NULL;
}

void CDnTooltipDlg::UpdateCompareToolTip()
{
	CEtUIDialog *pParentDlg = GetParentDialog();
	if( !pParentDlg ) 
		return;

	SUICoord sParentDlgCoord, sSelfDlgCoord;
	pParentDlg->GetDlgCoord( sParentDlgCoord );
	GetDlgCoord( sSelfDlgCoord );

	// 아이템 슬롯 버튼을 소유한 다이얼로그의 특성에 따라 기준 툴팁의 왼쪽에 렌더링할지 오른쪽에 렌더링할지를 결정한다.
	switch( m_itemSlotType )
	{
	case ST_INVENTORY:
		sParentDlgCoord.fX -= sSelfDlgCoord.fWidth - 0.018f;
		break;
	case ST_QUEST:
	case ST_STORAGE_PLAYER:
	case ST_STORAGE_GUILD:
	case ST_STORE:
	case ST_SKILL_STORE:
	case ST_EXCHANGE:
	case ST_ITEM_DISJOINT:
	case ST_PLATE:
	case ST_ITEM_UPGRADE:
	case ST_ITEM_COMPOUND:
	case ST_MARKET_REGIST:
	case ST_ITEM_COSTUMEMIX_RESULT:
		sParentDlgCoord.fX += sParentDlgCoord.fWidth - 0.018f;	// 우측에다 표시할때는 부모의 Width만큼을 더해야한다.
		break;
	default:
		sParentDlgCoord.fX -= sSelfDlgCoord.fWidth - 0.018f;
		break;
	}

	// 비교 툴팁의 x축 이동량은 기억해뒀다가 부모에도 적용해야한다.
	float fDlgXCoordRel = 0.0f;
	SetPosition( sParentDlgCoord.fX, sParentDlgCoord.fY );

	{
		GetDlgCoord( sSelfDlgCoord );
		if( (sSelfDlgCoord.Bottom()) > GetScreenHeightRatio() )
		{
			sSelfDlgCoord.fY -= (sSelfDlgCoord.Bottom() - GetScreenHeightRatio());
		}
		if( (sSelfDlgCoord.Right()) > GetScreenWidthRatio() )
		{
			fDlgXCoordRel = sSelfDlgCoord.Right() - GetScreenWidthRatio();
			sSelfDlgCoord.fX -= fDlgXCoordRel;
		}
		if( ( sSelfDlgCoord.fX ) < 0.f )
		{
			fDlgXCoordRel = sSelfDlgCoord.fX;
			sSelfDlgCoord.fX -= fDlgXCoordRel;
		}
		SetDlgCoord( sSelfDlgCoord );
	}

	// 비교 툴팁이 좌측으로 이동했다면, 부모에도 같은 값을 적용.
	if (fDlgXCoordRel != 0.0f)
	{
		// 두번째 비교 툴팁의 경우 한단계 상위(첫번째 툴팁) 및 두번째 상위(기본 툴팁) 둘다 이동시켜야한다.
		CEtUIDialog *pParentDialog = this;
		while( pParentDialog = pParentDialog->GetParentDialog() )
		{
			pParentDialog->GetDlgCoord( sParentDlgCoord );
			sParentDlgCoord.fX -= fDlgXCoordRel;
			pParentDialog->SetDlgCoord( sParentDlgCoord );
		}
	}

	ShowCompareLight(true);
}

#endif // PRE_ADD_OVERLAP_SETEFFECT

//#ifdef PRE_ADD_MAINQUEST_UI
//void CDnTooltipDlg::ShowMainQuestTooltip(CDnSlotButton *pControl, float fX, float fY, bool bAutoCompare, CDnItem *pTargetDiffItem)
//{
//	ASSERT(pControl);
//	if(!pControl)
//		return;
//
//	m_bShowMainQuestToolTip = true;
//
//#ifdef PRE_ADD_COMPARETOOLTIP
//	m_ringCompare.SetData( fX, fY, bAutoCompare );	
//#endif
//
//	// ComeBack - 로그인창에서 툴팁표시.	
//	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
//		if( !CDnActor::s_hLocalActor ) return;
//	// if( !CDnActor::s_hLocalActor ) return;
//	m_hSlotButton = pControl->GetMySmartPtr();
//	m_pSrcItem = m_hSlotButton->GetItem();
//
//	switch( pControl->GetItemType() ) {
//		case MIInventoryItem::Item:
//		case MIInventoryItem::Skill:
//			if( !m_pSrcItem ) return;
//			if( !m_pSrcItem->IsEnableTooltip() ) return;
//			break;
//	}
//
//	m_itemSlotType = m_hSlotButton->GetSlotType();
//
//	m_pDestItem = NULL;
//	m_pDestItem2 = NULL;
//	m_pOverlapCompareItem = NULL;
//
//	HideCompareTooltip( m_pCompareTooltipDlg );
//	HideCompareTooltip( m_pCompareTooltipDlg2 );
//	SetTargetDiffItem( pTargetDiffItem );
//
//	switch( pControl->GetItemType() ) {
//		case MIInventoryItem::Item:
//		case MIInventoryItem::Skill:
//			SetTooltipText( m_pSrcItem, ( pTargetDiffItem ) ? true : false );
//			break;
//		case MIInventoryItem::Gesture:
//			// DnGestureButton의 경우엔 MIInventoryItem* m_pItem 멤버가 항상 NULL이다.
//			// DnSkill, DnItem과 달리 제스처는 따로 객체형태로 표현되지 않기 때문.
//			// 그래서 이렇게 DnSlotButton의 m_nItemID를 얻어다가 설정해야한다.
//			ClearText();
//			ClearDivisionLine();
//			SetGestureText( pControl->GetItemID() );
//			break;
//		case MIInventoryItem::SecondarySkill:
//			ClearText();
//			ClearDivisionLine();
//			SetLifeSkillText( pControl->GetItemID() );
//			break;
//	}
//
//	UpdateTooltip( fX, fY );
//
//	Show( true );
//
//	return;
//
//	// 비교 툴팁이 나오면 안되는것들
//	if( m_hSlotButton->GetSlotType() == ST_ITEM_NONE )	return;
//	if( m_hSlotButton->GetSlotType() == ST_QUICKSLOT)	return;
//	if( m_hSlotButton->GetSlotType() == ST_CHARSTATUS )	return;
//	if( m_hSlotButton->GetSlotType() == ST_SKILL)		return;
//#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
//	if( m_hSlotButton->GetSlotType() == ST_ITEM_NOTIFY_COMPOUND ) return;
//#endif
//#ifdef PRE_ADD_TALISMAN_SYSTEM
//	if( m_hSlotButton->GetSlotType() == ST_TALISMAN )   return;
//#endif
//
//	// Note : 이 아이템이 인벤토리에 있고 만약 현재 장착되어 있는 장비랑 비교할 수 있다면 비교 창을 띄운다.
//	//
//	MIInventoryItem* pItem = m_hSlotButton->GetItem();
//	CDnItem* pSrcItem = (CDnItem *)m_hSlotButton->GetItem();
//	if( bAutoCompare && pItem && (pItem->GetType() == MIInventoryItem::Item) )
//	{
//		// 장착되어있는 아이템을 얻어오기 전에, 해당 아이템을 장착할 수 있는지 확인한다.
//		std::vector<int> nVecJobList;
//		if( CDnActor::s_hLocalActor )
//			((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
//		// ((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
//		if( ((CDnItem*)pItem)->IsNeedJob() == false || ((CDnItem*)pItem)->IsPermitPlayer( nVecJobList ) )
//		{
//			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
//			if( pCharStatusDlg )
//			{
//				int nSlotIndex = (((CDnItem*)pItem)->IsCashItem()) ? pCharStatusDlg->GetCashEquipIndex( (CDnItem*)pItem ) : pCharStatusDlg->GetEquipIndex( (CDnItem*)pItem );
//				if( nSlotIndex >= 0 )
//				{
//					eItemTypeEnum eType = pSrcItem->GetItemType();
//					if (((CDnItem*)pItem)->IsCashItem())
//					{
//#ifdef PRE_ADD_COMPARETOOLTIP
//						m_bTwoRing = false;
//						if( nSlotIndex == CASHEQUIP_RING1 || nSlotIndex == CASHEQUIP_RING2 )
//						{
//							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
//							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING1 ) != NULL )
//								nRing = 1;
//							if( pCharStatusDlg->GetCashEquipItem( CASHEQUIP_RING2 ) != NULL )
//								nRing = (nRing == 1 ? 3 : 2);
//
//
//							// #69087		
//							// 둘다착용시 Alt 로 변경.
//							int _slotIdx = CASHEQUIP_RING1;							
//							if( nRing == 3 )
//							{
//								_slotIdx = m_bShiftPress ? CASHEQUIP_RING2 : CASHEQUIP_RING1;
//								m_bTwoRing = true;
//							}
//							else if( nRing == 2 )
//								_slotIdx = CASHEQUIP_RING2;
//							m_nShowRingSlotIndex = _slotIdx;
//							ShowCompareTooltip( true, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == CASHEQUIP_RING2 ? CDnParts::PartsTypeEnum::CashRing2 : -1 )  );
//						}
//						else
//						{
//							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
//							HideCompareTooltip( m_pCompareTooltipDlg2 );
//						}
//
//#else
//						if( nSlotIndex == CASHEQUIP_RING1 )
//						{	
//							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
//							if( ShowCompareTooltip( true, eType, CASHEQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
//							{
//								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
//								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
//							}
//							else
//							{
//								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
//								ShowCompareTooltip( true, eType, CASHEQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
//								HideCompareTooltip( m_pCompareTooltipDlg2 );
//							}							
//						}
//						else
//						{
//							ShowCompareTooltip( true, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
//							HideCompareTooltip( m_pCompareTooltipDlg2 );
//						}
//#endif // PRE_ADD_COMPARETOOLTIP
//
//					}
//					else
//					{
//#ifdef PRE_ADD_COMPARETOOLTIP
//						m_bTwoRing = false;
//						//if( ( nSlotIndex == EQUIP_RING1 && eType != ITEMTYPE_GLYPH ) || nSlotIndex == EQUIP_RING2 )
//						if( eType != ITEMTYPE_GLYPH && ( nSlotIndex == EQUIP_RING1 || nSlotIndex == EQUIP_RING2 ) )
//						{
//							int nRing = 0; // 0(둘다X),  1(1번착용),  2(2번착용),  3(둘다착용)							
//							if( pCharStatusDlg->GetEquipItem( EQUIP_RING1 ) != NULL )
//								nRing = 1;
//							if( pCharStatusDlg->GetEquipItem( EQUIP_RING2 ) != NULL )
//								nRing = (nRing == 1 ? 3 : 2);
//
//							// #69087		
//							// 둘다착용시 Alt 로 변경.
//							int _slotIdx = EQUIP_RING1;							
//							if( nRing == 3 )
//							{
//								_slotIdx = m_bShiftPress ? EQUIP_RING2 : EQUIP_RING1;
//								m_bTwoRing = true;
//							}
//							else if( nRing == 2 )
//								_slotIdx = EQUIP_RING2;
//							m_nShowRingSlotIndex = _slotIdx;
//							ShowCompareTooltip( false, eType, _slotIdx, m_pDestItem, m_pCompareTooltipDlg, ( _slotIdx == EQUIP_RING2 ? CDnParts::PartsTypeEnum::Ring2 : -1 )  );
//						}
//						else
//						{
//							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
//							HideCompareTooltip( m_pCompareTooltipDlg2 );
//						}
//#else
//
//						//if( nSlotIndex == EQUIP_RING1 )
//						if( nSlotIndex == EQUIP_RING1 && 
//							eType != ITEMTYPE_GLYPH ) // #48430 아래코드는 EQUIP_RING2(10) 툴팁을 찍게되어있는데 
//							// 10번은 문장에서 '의지' 에 해당한다. 따라서 '의지' 문장의 툴팁이
//							// 무조건 출력되어 [문장] 의 경우 예외처리 한다.
//						{
//							// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
//							if( ShowCompareTooltip( false, eType, EQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
//							{
//								// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
//								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
//							}
//							else
//							{
//								// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
//								ShowCompareTooltip( false, eType, EQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
//								HideCompareTooltip( m_pCompareTooltipDlg2 );
//							}							
//						}
//						else
//						{
//							ShowCompareTooltip( false, eType, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
//							HideCompareTooltip( m_pCompareTooltipDlg2 );
//						}
//#endif // PRE_ADD_COMPARETOOLTIP
//					}
//
//#if 0
//					// 마우스 오버한 아이템이 반지면,
//					if( nSlotIndex == EQUIP_RING1 )
//					{
//						// 캐릭터창 두번째 슬롯에 있는 반지부터 먼저 기본 툴팁에 붙여 보이게 하고,
//						if( ShowCompareTooltip( true, EQUIP_RING2, m_pDestItem, m_pCompareTooltipDlg ) )
//						{
//							// 첫번재 슬롯에 있는 반지를 옆에다가 붙인다.
//							ShowCompareTooltip( true, EQUIP_RING1, m_pDestItem2, m_pCompareTooltipDlg2 );
//						}
//						else
//						{
//							// 캐릭터창 두번째 슬롯에 반지가 없다면, 그냥 첫번째 슬롯에 있는 반지만 보일 것이다.
//							ShowCompareTooltip( true, EQUIP_RING1, m_pDestItem, m_pCompareTooltipDlg );
//							HideCompareTooltip( m_pCompareTooltipDlg2 );
//						}
//					}
//					else
//					{
//						ShowCompareTooltip( true, nSlotIndex, m_pDestItem, m_pCompareTooltipDlg );
//						HideCompareTooltip( m_pCompareTooltipDlg2 );
//					}
//#endif // 0
//
//					return;
//				}
//			}
//		}
//	}
//}
//
//void CDnTooltipDlg::HideMainQuestTooltip()
//{
//	ShowCompareLight( false );
//	Show( false );
//	m_hSlotButton.Identity();
//	ClearDivisionLine();
//	ResetToolTipSwitch();
//	
//	m_bShowMainQuestToolTip = false;
//}
//#endif

#ifdef PRE_ADD_DRAGON_GEM
void CDnTooltipDlg::ShowDragonGemToolTip()
{
	m_pDragonGemTooltipDlg->SetPosition(m_fUpdateOrigPosX, m_fUpdateOrigPosY);
	m_pDragonGemTooltipDlg->Show(true);
}

void CDnTooltipDlg::CloseDragonGemToolTip()
{
	m_pDragonGemTooltipDlg->Show(false);
}
#endif // PRE_ADD_DRAGON_GEM