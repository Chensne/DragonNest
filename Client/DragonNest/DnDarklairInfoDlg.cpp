#include "StdAfx.h"
#include "DnDarklairInfoDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDarklairInfoDlg::CDnDarklairInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticMapName( NULL )
, m_pStaticRound( NULL )
, m_pStaticBoss( NULL )
, m_pStaticFinal( NULL )
, m_pStaticMovePosition( NULL )
, m_bMoveOffset( false )
, m_pStaticBonus( NULL )
{
}

CDnDarklairInfoDlg::~CDnDarklairInfoDlg()
{
}

void CDnDarklairInfoDlg::InitialUpdate()
{
	m_pStaticMapName = GetControl<CEtUIStatic>( "ID_STATIC_MAPNAME" );
	m_pStaticRound = GetControl<CEtUIStatic>( "ID_STATIC0" );
	m_pStaticBoss = GetControl<CEtUIStatic>( "ID_STATIC2" );
	m_pStaticFinal = GetControl<CEtUIStatic>( "ID_TEXT_FINAL" );
	m_pStaticBonus = GetControl<CEtUIStatic>( "ID_STATIC3" );
	m_pStaticBonus->Show( false );
	m_pStaticRound->Show( false );
	m_pStaticBoss->Show( false );
	m_pStaticFinal->Show( false );

	m_uiDefaultCoord[0] = m_pStaticMapName->GetUICoord();
	m_uiDefaultCoord[1] = GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord();
	m_uiDefaultCoord[2] = GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord();

	m_pStaticMovePosition = GetControl<CEtUIStatic>("ID_MOVE_POSITION");
	m_pStaticMovePosition->Show( false );
	m_uiMovePositionOffset.fX = m_pStaticMovePosition->GetUICoord().fX - GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fX;
	m_uiMovePositionOffset.fY = m_pStaticMovePosition->GetUICoord().fY - GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fY;
}

void CDnDarklairInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DarkRareInfoDlg.ui" ).c_str(), bShow );
}

void CDnDarklairInfoDlg::RefreshInfo()
{
	// 새로 위치 정할 경우 혹시 이미 오프셋처리가 되어있다면 해제한다.
	if( m_bMoveOffset ) MoveOffset( false );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox ) return;

	int nID = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_MapNameID" )->GetInteger();

	m_pStaticMapName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ) );

	// 적용 전 초기화
	m_pStaticMapName->SetUICoord( m_uiDefaultCoord[0] );
	GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->SetUICoord( m_uiDefaultCoord[1] );
	GetControl<CEtUIStatic>("ID_STATIC1")->SetUICoord( m_uiDefaultCoord[2] );

	// 길이가 길어질 경우 해당 컨트롤의 가로크기를 키우고, 필요한 나머지 컨트롤들을 왼쪽으로 이동시킨다.
	// 사이즈 변경시 다른 컨트롤들도 같이 이동시켜야하기 때문에, 이 기능을 엔진단에 넣기엔 애매하다.
	SUICoord uiCoord, textCoord;
	m_pStaticMapName->GetUICoord(uiCoord);
	SUIElement *pElement = m_pStaticMapName->GetElement(0);
	CalcTextRect( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ), pElement, textCoord );
	if( uiCoord.fWidth < textCoord.fWidth+24.0f/DEFAULT_UI_SCREEN_WIDTH )
	{
		float fDiff = textCoord.fWidth+24.0f/DEFAULT_UI_SCREEN_WIDTH - uiCoord.fWidth;
		GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->SetPosition( GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord().fX - fDiff, GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord().fY );
		GetControl<CEtUIStatic>("ID_STATIC1")->SetPosition( GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fX - fDiff, GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fY );
		m_pStaticMapName->SetPosition( uiCoord.fX - fDiff, uiCoord.fY );
		m_pStaticMapName->SetSize( uiCoord.fWidth + fDiff, uiCoord.fHeight );
	}

	m_pStaticRound->Show( false );
	m_pStaticBoss->Show( false );
	m_pStaticFinal->Show( false );
}

void CDnDarklairInfoDlg::RefreshRound( int nRound, bool bBoss, bool bFinal )
{
	m_pStaticRound->Show( false );
	m_pStaticBoss->Show( false );
	m_pStaticFinal->Show( false );

	if( nRound == 0 ) return;
	if( bFinal ) {
		m_pStaticFinal->Show( true );
	}
	else if( bBoss ) {
		WCHAR wszStr[64];
		swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2388 ), nRound );
		m_pStaticBoss->SetText( wszStr );
		m_pStaticBoss->Show( true );
	}
	else {
		WCHAR wszStr[64];
		swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2387 ), nRound );
		m_pStaticRound->SetText( wszStr );
		m_pStaticRound->Show( true );
	}
}

void CDnDarklairInfoDlg::MoveOffset( bool bMove )
{
	if( m_bMoveOffset == bMove )
		return;

	m_bMoveOffset = bMove;

	int nControlCount = GetControlCount();
	for( int i = 0; i < nControlCount; ++i )
	{
		CEtUIControl *pControl = GetControl(i);

		if( m_bMoveOffset )
			pControl->SetPosition( pControl->GetUICoord().fX + m_uiMovePositionOffset.fX, pControl->GetUICoord().fY + m_uiMovePositionOffset.fY );
		else
			pControl->SetPosition( pControl->GetUICoord().fX - m_uiMovePositionOffset.fX, pControl->GetUICoord().fY - m_uiMovePositionOffset.fY );
	}
}