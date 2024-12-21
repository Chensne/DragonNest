#include "StdAfx.h"
#include "DnHelpAlarmDlg.h"
#include "DnItemTask.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int NPC_FACE_TEXTURE_SIZE = 64;

CDnHelpAlarmDlg::CDnHelpAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
, m_fElapsedTime(-1.0f)
, m_fConst(1.0f)
, m_fShowTime(0.0f)
, m_fShowRatio(0.8f)
{
	m_pStaticDesc = NULL;
	m_pSlotItem = NULL;
	m_pTextureNpc = NULL;
	m_pItem = NULL;
}

CDnHelpAlarmDlg::~CDnHelpAlarmDlg(void)
{
	SAFE_RELEASE_SPTR( m_hNpcFaceTexture );
	SAFE_DELETE( m_pItem );
}

void CDnHelpAlarmDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "HelpAlarmDlg.ui" ).c_str(), bShow );
	SetFadeMode( CEtUIDialog::CancelRender );
}

void CDnHelpAlarmDlg::InitialUpdate()
{
	m_pStaticDesc = GetControl<CEtUIStatic>("ID_STATIC_TEXT");
	m_pSlotItem = GetControl<CDnItemSlotButton>("ID_BUTTON_ITEM");
	m_pTextureNpc = GetControl<CEtUITextureControl>("ID_TEXTUREL_NPC");

	GetControl<CEtUIButton>("ID_BUTTON_SYMBOL")->Show(false);
	GetControl<CEtUIButton>("ID_BUTTON_SKILL")->Show(false);

	SAFE_RELEASE_SPTR( m_hNpcFaceTexture );
	m_hNpcFaceTexture = LoadResource( "Repute_NPC.dds", RT_TEXTURE, true );
}

void CDnHelpAlarmDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );

	if( m_fElapsedTime < 0.0f )
	{
		if( IsShow() ) Show( false );
		return;
	}

	if( m_fShowTime > 0.0f )
	{
		fElapsedTime = min( fElapsedTime, 0.5f );	// ������ ó�� �����Ҷ� �ε������� fDelta�� 2.5�� �̻� �ɶ��� �ִ�. �׷��� ������ fDelta�� �����.
		m_fShowTime -= fElapsedTime;
		return;
	}
	else
	{
		Show( false );
	}

	m_fElapsedTime -= fElapsedTime;
}

void CDnHelpAlarmDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow ) return;

	if( bShow ) {
		if( false == CGameOption::GetInstance().bSimpleGuide )
			return;
	}
	else {
		SAFE_DELETE( m_pItem );
		m_pSlotItem->ResetSlot();
	}

	CDnCustomDlg::Show( bShow );
}

void CDnHelpAlarmDlg::SetAlarm( int nHelpTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUIDEPOPUP );
	if( !pSox->IsExistItem( nHelpTableID ) ) return;

	SAFE_DELETE( m_pItem );
	m_pSlotItem->ResetSlot();

	int nType = pSox->GetFieldFromLablePtr( nHelpTableID, "_Type" )->GetInteger();
	int nDescription = pSox->GetFieldFromLablePtr( nHelpTableID, "_DescriptionID" )->GetInteger();
	int nImageIndex = pSox->GetFieldFromLablePtr( nHelpTableID, "_ImgIndex" )->GetInteger();
	float fFadeTime = pSox->GetFieldFromLablePtr( nHelpTableID, "_ImgTime" )->GetFloat();

	m_pStaticDesc->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nDescription ) );
	switch( nType ) {
		case 1:	// Npc
			{
				m_pSlotItem->Show( false );
				m_pTextureNpc->Show( true );

				int nX, nY;
				CalcButtonUV( nImageIndex, m_hNpcFaceTexture, nX, nY, NPC_FACE_TEXTURE_SIZE, NPC_FACE_TEXTURE_SIZE );
				m_pTextureNpc->SetTexture( m_hNpcFaceTexture, nX, nY, NPC_FACE_TEXTURE_SIZE, NPC_FACE_TEXTURE_SIZE );
			}
			break;
		case 2: // Item
			{
				m_pSlotItem->Show( true );
				m_pTextureNpc->Show( false );

				TItemInfo itemInfo;
				if( CDnItem::MakeItemInfo( nImageIndex, 1, itemInfo ) == false ) return;
				m_pItem = GetItemTask().CreateItem( itemInfo );

				// ������ �缳��. �������� �ƽ�ġ�� �� �� ������ ���⼭ �̷��� �ƽ�ġ�� �����Ѵ�.
				if( m_pItem->GetItemType() == ITEMTYPE_WEAPON ) {
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(m_pItem);
					if( pWeapon ) pWeapon->SetDurability( pWeapon->GetMaxDurability() );
				}
				else if( m_pItem->GetItemType() == ITEMTYPE_PARTS ) {
					CDnParts *pParts = dynamic_cast<CDnParts *>(m_pItem);
					if( pParts ) pParts->SetDurability( pParts->GetMaxDurability() );
				}

				m_pSlotItem->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
				m_pSlotItem->SetShowTooltip( false );
			}
			break;
	}

	SetFadeTime( fFadeTime );
}

void CDnHelpAlarmDlg::SetFadeTime( float fFadeTime )
{
	m_fShowTime = fFadeTime * m_fShowRatio;
	m_fElapsedTime = fFadeTime - m_fShowTime;
	m_fConst = m_fElapsedTime;
}

void CDnHelpAlarmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl( "ID_BT_CLOSE" ) ) {
			Show( false );
		}
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnHelpAlarmDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN ) {
		if( wParam == 0x4A ) {
			int asdf = 0;
		}
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}