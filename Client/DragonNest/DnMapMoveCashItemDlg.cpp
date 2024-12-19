#include "StdAfx.h"
#include "DnMapMoveCashItemDlg.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnCommonTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMapMoveCashItemDlg::CDnMapMoveCashItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true ),
m_iSelectedMapRadioBtn( -1 ),
m_pStaticMapName( NULL ),
m_pStaticSummary( NULL ),
m_pOKBtn( NULL ),
m_pCancelBtn( NULL )
{
	memset(&m_apMovableMapInfo,0,sizeof(m_apMovableMapInfo));
}

CDnMapMoveCashItemDlg::~CDnMapMoveCashItemDlg(void)
{

}

void CDnMapMoveCashItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MapMoveDlg.ui" ).c_str(), bShow );
}

void CDnMapMoveCashItemDlg::InitialUpdate()
{
	char szTemp[32]={0};
	char* aMapImageControlNames [] =
	{
		"ID_STATIC_PT",
		"ID_STATIC_MR",
		"ID_STATIC_CG",
		"ID_STATIC_SH",
		"ID_STATIC_LM",
		"ID_STATIC_COLO",
		"ID_STATIC_FARM"
	};

	struct S_MAP_DESC
	{
		int iMapID;
		int iToolTipUIStringID;
		int iMapExplanationStringID;
	};

	// UI 로 마을이 아예 박혀 있으므로 코드도 따라아갸 한다. 
	// 추후에 마을 입장에 대한 레벨 정보가 서버로부터 리스트로 오는데 해당 리스트와 비교하는 근거자료도 된다.
	S_MAP_DESC aMapTableID [] = 
	{
		{1, 2422, 2414},			// 프레리 타운
		{5, 2423, 2415},			// 마나릿지
		{8, 2424, 2416},			// 캐더락 관문
		{11, 2425, 2417},			// 세인트 헤븐
		{15, 2426, 2418},			// 로터스 마쉬
		{40, 2429, 2419},			// 콜로세움
		{15107, 2427, 2421},		// 농장 마을
	};

	bool bMapNamesArrayIsValid = ( _countof(aMapImageControlNames) == NUM_MOVABLE_MAP) &&
								 ( _countof(aMapTableID) == NUM_MOVABLE_MAP) ;
	_ASSERT( bMapNamesArrayIsValid );
	if( bMapNamesArrayIsValid )
	{	
		for( int i = 0; i < NUM_MOVABLE_MAP; ++i )
		{
			sprintf_s( szTemp, _countof(szTemp), "ID_RBT%d", i );
			m_apMovableMapInfo[ i ].pRadioButton = GetControl<CEtUIRadioButton>( szTemp );
			m_apMovableMapInfo[ i ].pRadioButton->SetButtonID( i );
			m_apMovableMapInfo[ i ].pMapImage = GetControl<CEtUIStatic>( aMapImageControlNames[ i ] );
			m_apMovableMapInfo[ i ].iMapID = aMapTableID[ i ].iMapID;
			m_apMovableMapInfo[ i ].iToolTipUIString = aMapTableID[ i ].iToolTipUIStringID;
			m_apMovableMapInfo[ i ].iMapExplanationStringID = aMapTableID[ i ].iMapExplanationStringID;
		}
	}

	m_pStaticMapName = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );
	m_pStaticSummary = GetControl<CEtUIStatic>( "ID_TEXT_SUMMARY" );

	m_pOKBtn = GetControl<CEtUIButton>( "ID_OK" );
	m_pCancelBtn = GetControl<CEtUIButton>( "ID_CANCEL" );

	m_SmartMove.SetControl( m_pCancelBtn );
}

void CDnMapMoveCashItemDlg::SetVillageList( const WarpVillage::WarpVillageInfo* pVillageList, int iListCount )
{
	for( int k = 0; k < NUM_MOVABLE_MAP; ++k )
	{
		m_apMovableMapInfo[ k ].iLevelLimit = 0;
	}

	for( int i = 0; i < iListCount; ++i )
	{
		for( int k = 0; k < NUM_MOVABLE_MAP; ++k )
		{
			if( pVillageList[ k ].nMapIndex == m_apMovableMapInfo[ i ].iMapID )
			{
				m_apMovableMapInfo[ i ].iLevelLimit = pVillageList[ k ].btLimitLevel;
			}
		}
	}
}

void CDnMapMoveCashItemDlg::Show( bool bShow ) 
{ 
	bool bPrevShow = m_bShow;
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();

		// 레벨 제한 툴팁 문자열 셋팅
		wchar_t acBuffer[ 256 ] = { 0 };
		for( int i = 0; i < NUM_MOVABLE_MAP; ++i )
		{
			if( 0 < m_apMovableMapInfo[ i ].iLevelLimit )
			{
				swprintf_s( acBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_apMovableMapInfo[ i ].iToolTipUIString ), 
						   m_apMovableMapInfo[ i ].iLevelLimit );
				m_apMovableMapInfo[ i ].pRadioButton->SetTooltipText( acBuffer );
			}
			else
			{
				m_apMovableMapInfo[ i ].pRadioButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_apMovableMapInfo[ i ].iToolTipUIString ) );
			}
		}

		// 첫번째 맵 자동 선택
		int iInitialSelectedIndex = 0;

		// 하지만 현재 내가 있는 맵 번호와 동일하다면 다른 맵 선택.
		if( m_apMovableMapInfo[ iInitialSelectedIndex ].iMapID == CGlobalInfo::GetInstance().m_nCurrentMapIndex )
		{
			iInitialSelectedIndex = (iInitialSelectedIndex + 1) % NUM_MOVABLE_MAP;
		}

		// 현재 맵은 선택 불가.
		int iDisabledRadioBtnIndex = -1;
		for( int i = 0; i < NUM_MOVABLE_MAP; ++i )
		{
			m_apMovableMapInfo[ i ].pRadioButton->Enable( true );
			if( m_apMovableMapInfo[ i ].iMapID == CGlobalInfo::GetInstance().m_nCurrentMapIndex )
				iDisabledRadioBtnIndex = i;
		}
		if( -1 < iDisabledRadioBtnIndex )
			m_apMovableMapInfo[ iDisabledRadioBtnIndex ].pRadioButton->Enable( false );

		SelectMap( iInitialSelectedIndex );

		// 라디오 버튼도 디폴트로 선택된 맵으로 선택.
		m_apMovableMapInfo[ iInitialSelectedIndex ].pRadioButton->SetChecked( true );

		// 전에 사용되어서 disable 된 상태의 버튼을 enable 로 복구.
		m_pOKBtn->Enable( true );
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}

	// #48810 열려 있다 닫히는 경우 window state none 을 서버로 보내준다.
	if( true == bPrevShow && false == m_bShow )
	{
		CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
		if( pTask )			
			pTask->SendWindowState( WINDOW_NONE );
	}
}

void CDnMapMoveCashItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( EVENT_BUTTON_CLICKED == nCommand )
	{
		if( pControl == m_pOKBtn )
		{
			// 서버로 마을 이동 패킷 보냄.
			if( 0 <= m_iSelectedMapRadioBtn && m_iSelectedMapRadioBtn < NUM_MOVABLE_MAP &&
				m_pUsedItem )
			{
				GetItemTask().RequestUseWarpVillageCashItem( m_apMovableMapInfo[ m_iSelectedMapRadioBtn ].iMapID, m_pUsedItem->GetSerialID() );

				// 버튼은 disable 처리.
				m_pOKBtn->Enable( false );
			}
		}
		else
		if( pControl == m_pCancelBtn || (strcmp(pControl->GetControlName(), "ID_BUTTON_CLOSE") == 0) )
		{
			Show(false);
		}
	}
	else
	if( EVENT_RADIOBUTTON_CHANGED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_RBT" ) )
		{
			int iButtonID = static_cast<CEtUIRadioButton*>( pControl )->GetButtonID();
			SelectMap( iButtonID );
		}
		else
		{
			m_iSelectedMapRadioBtn = -1;
		}
	}
}

void CDnMapMoveCashItemDlg::SelectMap( int iIndex )
{
	if( iIndex < NUM_MOVABLE_MAP )
	{
		m_iSelectedMapRadioBtn = iIndex;

		// 그림도 같이 바꿔줌.
		for( int i = 0; i < NUM_MOVABLE_MAP; ++i )
		{
			m_apMovableMapInfo[ i ].pMapImage->Show( false );
		}

		m_apMovableMapInfo[ iIndex ].pMapImage->Show( true );

		// 맵 이름 문자열 셋팅
		m_pStaticMapName->SetText( m_apMovableMapInfo[ iIndex ].pRadioButton->GetText() );
		m_pStaticSummary->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_apMovableMapInfo[iIndex].iMapExplanationStringID ) );
	}
}

void CDnMapMoveCashItemDlg::ResetOKBtn()
{
	if( m_bShow )
	{
		if( m_pOKBtn )
			m_pOKBtn->Enable( true );
	}
}


bool CDnMapMoveCashItemDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMapMoveCashItemDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	//if( nCommand == EVENT_BUTTON_CLICKED )
	//{
	//	if( nID == MESSAGEBOX_OPENBOX )
	//	{
	//		if( IsCmdControl( "ID_YES" ) )
	//		{
	//			RequestUseCharmItem();
	//		}
	//	}
	//}
}

