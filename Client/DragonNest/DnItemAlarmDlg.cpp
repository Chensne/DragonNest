#include "StdAfx.h"
#include "DnItemAlarmDlg.h"
#include "DnSkill.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnItemAlarmDlg::CDnItemAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pButtonItem(NULL)
	, m_pButtonSkill(NULL)
	, m_pStaticText(NULL)
	, m_pButtonSymbol(NULL)
	, m_fElapsedTime(-1.0f)
	, m_fConst(1.0f)
	, m_fShowTime(0.0f)
	, m_fShowRatio(0.8f)
	, m_pItem(NULL)
	, m_fIconHeightDelta( 0.0f )
{
	memset( m_pButtonPlant, NULL, sizeof(CDnItemSlotButton *)*PLANT_MAX );
	memset( m_pStaticPlant, NULL, sizeof(CEtUIStatic *)*PLANT_MAX );
	memset( m_pPlantItems, NULL, sizeof(CDnItem *)*PLANT_MAX );
}

CDnItemAlarmDlg::~CDnItemAlarmDlg(void)
{
	SAFE_DELETE(m_pItem);

	for( int itr = 0; itr < PLANT_MAX; ++itr )
		SAFE_DELETE( m_pPlantItems[itr] );
}

void CDnItemAlarmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemAlarmDlg.ui" ).c_str(), bShow );
	SetFadeMode( CEtUIDialog::CancelRender );
}

void CDnItemAlarmDlg::InitialUpdate()
{
	m_pButtonItem = GetControl<CDnItemSlotButton>("ID_BUTTON_ITEM");
	m_pButtonItem->SetRenderCount( false );

	m_pButtonSymbol = GetControl<CDnItemSlotButton>("ID_BUTTON_SYMBOL");
	m_pButtonSymbol->SetRenderCount( false );

	m_pButtonSkill = GetControl<CDnSkillSlotButton>("ID_BUTTON_SKILL");
	m_pStaticText = GetControl<CEtUIStatic>("ID_STATIC_TEXT");

	m_pButtonPlant[0] = GetControl<CDnItemSlotButton>("ID_BUTTON_SYMBOL1");
	m_pButtonPlant[0]->SetRenderCount( false );
	m_pButtonPlant[1] = GetControl<CDnItemSlotButton>("ID_BUTTON_SYMBOL2");
	m_pButtonPlant[1]->SetRenderCount( false );
	m_pButtonPlant[2] = GetControl<CDnItemSlotButton>("ID_BUTTON_SYMBOL3");
	m_pButtonPlant[2]->SetRenderCount( false );

	m_pStaticPlant[0] = GetControl<CEtUIStatic>("ID_STATIC_TEXT1");
	m_pStaticPlant[1] = GetControl<CEtUIStatic>("ID_STATIC_TEXT2");
	m_pStaticPlant[2] = GetControl<CEtUIStatic>("ID_STATIC_TEXT3");

	m_ButtonCoord[0] = m_pButtonPlant[0]->GetProperty()->UICoord;
	m_ButtonCoord[1] = m_pButtonPlant[2]->GetProperty()->UICoord;
	m_TextCoord[0] = m_pStaticPlant[0]->GetProperty()->UICoord;
	m_TextCoord[1] = m_pStaticPlant[2]->GetProperty()->UICoord;
	m_fIconHeightDelta = m_pButtonPlant[0]->GetProperty()->UICoord.fHeight/2.0f;
}

void CDnItemAlarmDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( m_fElapsedTime < 0.0f )
	{
		if( IsShow() ) Show( false );
		return;
	}

	if( m_fShowTime > 0.0f )
	{
		m_fShowTime -= fElapsedTime;
		return;
	}
	else
	{
		Show( false );
	}

	EtColor CurColor;
	EtColorLerp( &CurColor, &EtColor(0xFFFFFFFF), &EtColor((DWORD)0x00000000), 1.0f-(1/powf(m_fConst,2)*powf(m_fElapsedTime,2) ) );

	m_pButtonItem->SetIconColor( CurColor );
	m_pButtonSymbol->SetIconColor( CurColor );
	m_pButtonSkill->SetIconColor( CurColor );

	m_pButtonPlant[0]->SetIconColor( CurColor );
	m_pButtonPlant[1]->SetIconColor( CurColor );
	m_pButtonPlant[2]->SetIconColor( CurColor );

	m_fElapsedTime -= fElapsedTime;
}

void CDnItemAlarmDlg::Show( bool bShow )
{ 
	if( bShow )
	{
	}
	else
	{
		m_pStaticText->Show(false);

		m_pButtonSymbol->Show(false);
		m_pButtonItem->Show(false);
		m_pButtonSkill->Show(false);

		for( int itr = 0; itr < PLANT_MAX; ++itr )
		{
			m_pButtonPlant[itr]->Show( false );
			m_pStaticPlant[itr]->Show( false );
		}
	}

	CEtUIDialog::Show( bShow );
}
void CDnItemAlarmDlg::SetItemAlarm( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime )
{
	ResetControl();
	// pItem에 NULL오면 Msg만 출력하겠다는 의미다.
	if( pItem )
	{
		// 인벤에 있는 아이템 포인터가 올 경우가 많은데,
		// 이때 인벤에 있는 해당 아이템을 옆으로 이동시키면, 포인터가 NULL이 되면서 보였던 아이템슬롯이 안보이게 된다.
		// 그래서 인자로 받은 CDnItem 의 정보로 새 아이템하나를 임시로 만들어 보여주는 방법으로 바꾼다.

		SAFE_DELETE( m_pItem );
		TItemInfo itemInfo;
		pItem->GetTItemInfo(itemInfo);

		itemInfo.Item.bSoulbound = true;
		// 신규 생성되는 아이템을 인벤에서 찾게되는데< 기존에 템이있으면 찾은템과 제작템이 다른상황에 놓인다 >
		// 봉인되어있는 경우가 있기때문에 봉인을 풀어준다.

		m_pItem = GetItemTask().CreateItem( itemInfo );

		m_pButtonItem->SetItem((MIInventoryItem*)m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pButtonItem->SetIconColor( 0xFFFFFFFF );
		m_pButtonItem->Show(true);
	}

	wchar_t szTemp[128] = {0};

	if( nCount > 1 )
	{
		swprintf_s( szTemp, _countof(szTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 80 ), wszMsg, nCount );
	}
	else
	{
		swprintf_s( szTemp, _countof(szTemp), L"%s", wszMsg );
	}

	SetText( szTemp, dwColor );
	SetFadeTime( fFadeTime );
}

void CDnItemAlarmDlg::SetSkillAlarm( LPCWSTR wszMsg, DnSkillHandle hSkill, DWORD dwColor, float fFadeTime )
{
	ResetControl();

	m_pButtonSkill->SetItem((MIInventoryItem*)(CDnSkill*)hSkill, CDnSlotButton::NO_COUNTABLE_RENDER);
	m_pButtonSkill->SetIconColor( 0xFFFFFFFF );
	m_pButtonSkill->Show(true);

	wchar_t szTemp[128] = {0};
	swprintf_s( szTemp, _countof(szTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 81 ), wszMsg );

	SetText( szTemp, dwColor );
	SetFadeTime( fFadeTime );
}

void CDnItemAlarmDlg::SetSymbolAlarm( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime )
{
	ResetControl();

	m_pButtonSymbol->SetItem((MIInventoryItem*)pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_pButtonSymbol->SetIconColor( 0xFFFFFFFF );
	m_pButtonSymbol->Show(true);

	wchar_t szTemp[128] = {0};
	swprintf_s( szTemp, _countof(szTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 80 ), wszMsg, nCount );

	SetText( szTemp, dwColor );
	SetFadeTime( fFadeTime );
}

void CDnItemAlarmDlg::SetPlantAlarm( std::vector<CDnItem *> & vPlant, DWORD dwColor, float fFadeTime )
{
	ResetControl();

	for( int itr = 0; itr < PLANT_MAX; ++itr )
		SAFE_DELETE( m_pPlantItems[itr] );

	int nCount = (int)vPlant.size();

	if( 1 == nCount )
	{
		SetPlant( 1, vPlant[0], dwColor );
	}
	else if( 2 == nCount )
	{
		SetPlant( 0, vPlant[0], dwColor );
		SetPlant( 2, vPlant[1], dwColor );

		m_pButtonPlant[0]->GetProperty()->UICoord.fY = m_ButtonCoord[0].fY + m_fIconHeightDelta;
		m_pButtonPlant[2]->GetProperty()->UICoord.fY = m_ButtonCoord[1].fY - m_fIconHeightDelta;
		m_pStaticPlant[0]->GetProperty()->UICoord.fY = m_TextCoord[0].fY + m_fIconHeightDelta;
		m_pStaticPlant[2]->GetProperty()->UICoord.fY = m_TextCoord[1].fY - m_fIconHeightDelta;
		m_pStaticPlant[0]->UpdateRects();
		m_pStaticPlant[2]->UpdateRects();
	}
	else if( 3 == nCount )
	{
		SetPlant( 0, vPlant[0], dwColor );
		SetPlant( 1, vPlant[1], dwColor );
		SetPlant( 2, vPlant[2], dwColor );
		
		m_pButtonPlant[0]->GetProperty()->UICoord.fY = m_ButtonCoord[0].fY;
		m_pButtonPlant[2]->GetProperty()->UICoord.fY = m_ButtonCoord[1].fY;
		m_pStaticPlant[0]->GetProperty()->UICoord.fY = m_TextCoord[0].fY;
		m_pStaticPlant[2]->GetProperty()->UICoord.fY = m_TextCoord[1].fY;
		m_pStaticPlant[0]->UpdateRects();
		m_pStaticPlant[2]->UpdateRects();
	}

	SetFadeTime( fFadeTime );
}

void CDnItemAlarmDlg::SetPlant( int nIndex, CDnItem * pItem, DWORD dwColor )
{
	m_pPlantItems[nIndex] = pItem;

	m_pButtonPlant[nIndex]->SetItem( (MIInventoryItem *)pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	m_pButtonPlant[nIndex]->SetIconColor( 0xFFFFFFFF );
	m_pButtonPlant[nIndex]->Show( true );

	wchar_t szTemp[128] = {0};

	swprintf_s( szTemp, _countof(szTemp), L"%s", pItem->GetName() );

	m_pStaticPlant[nIndex]->SetText( szTemp );
	m_pStaticPlant[nIndex]->SetTextColor( dwColor );
	m_pStaticPlant[nIndex]->Show( true );
}

void CDnItemAlarmDlg::SetText( LPCWSTR wszMsg, DWORD dwColor )
{
	m_pStaticText->SetText( wszMsg );
	m_pStaticText->SetTextColor( dwColor );
	m_pStaticText->Show(true);
}

void CDnItemAlarmDlg::SetFadeTime( float fFadeTime )
{
	m_fShowTime = fFadeTime * m_fShowRatio;
	m_fElapsedTime = fFadeTime - m_fShowTime;
	m_fConst = m_fElapsedTime;
}

void CDnItemAlarmDlg::ResetControl()
{
	m_pStaticText->ClearText();
	m_pButtonSymbol->ResetSlot();
	m_pButtonItem->ResetSlot();
	m_pButtonSkill->ResetSlot();

	for( int itr = 0; itr < PLANT_MAX; ++itr )
	{
		m_pStaticPlant[itr]->ClearText();
		m_pButtonPlant[itr]->ResetSlot();
	}
}