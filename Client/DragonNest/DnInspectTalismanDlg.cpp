#include "stdafx.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnInspectTalismanDlg.h"
#include "DnTableDB.h"
#include "DnInspectPlayerDlg.h"
#include "DnPlayerActor.h"
#include "DnTooltipTalismanDlg.h"

const int	TALISMAN_SLOT_OFFSET = 70;
const int	TALISMAN_SLOT_WIDTH_CNT = 4;

CDnInspectTalismanDlg::CDnInspectTalismanDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pSlotDlg(NULL)
, m_pDetailInfoDlg(NULL)
, m_pDetailInfoButton(NULL)
, m_pScrollBar(NULL)
, m_pListBoxEx(NULL)
, m_fOriginalPosX(.0f)
, m_fOriginalPosY(.0f)
, m_nSlotMaxCount(0)
{
}

CDnInspectTalismanDlg::~CDnInspectTalismanDlg()
{
	SAFE_DELETE(m_pSlotDlg);
	SAFE_DELETE(m_pDetailInfoDlg);
}

void CDnInspectTalismanDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName( "PlayerTalismanDlg.ui" ).c_str(), bShow );
}

void CDnInspectTalismanDlg::InitialUpdate()
{
	m_pDetailInfoDlg = new CDnInspectTalismanInfoDlg(UI_TYPE_CHILD, this);
	m_pDetailInfoDlg->Initialize(false);

	m_pSlotDlg = new CDnInspectTalismanSlotDlg(UI_TYPE_CHILD, this);
	m_pSlotDlg->Initialize(false);

	GetControl<CEtUIButton>("ID_BT_COMPOSE")->Enable(false);
	GetControl<CEtUIButton>("ID_BT_OK")->Enable(false);
	GetControl<CEtUIButton>("ID_BT_SMALLHELP")->Enable(false);
	
	m_pDetailInfoButton = GetControl<CEtUIButton>("ID_BT_INFO");

	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_TALISMAN");
	m_pScrollBar = m_pListBoxEx->GetScrollBar();

	if( DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TTALISMANSLOT) ) 
	{
		for(int i = 0 ; i < pSox->GetItemCount() ; ++i)
		{
			int ID = pSox->GetItemID(i);
			if(pSox->GetFieldFromLablePtr(ID, "_Service")->GetBool())
				++m_nSlotMaxCount;
		}
	}

	// 살펴보기는 모든 슬롯을 오픈한다.
	m_pSlotDlg->SetAllOpenSlot();
}

bool CDnInspectTalismanDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int iScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;

				if( m_pScrollBar->IsEnable() )
				{
					int nPos = m_pScrollBar->GetTrackPos() - iScrollAmount*10;
					m_pScrollBar->SetTrackPos(nPos);
				}
			}
		}
		break;
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnInspectTalismanDlg::SetSlotOffset()
{
	if(m_pSlotDlg)
	{
		SUIDialogInfo info;
		m_pSlotDlg->GetDlgInfo(info);

		if(m_fOriginalPosX == .0f && m_fOriginalPosY == .0f)
		{
			m_fOriginalPosX = info.DlgCoord.fX;
			m_fOriginalPosY = info.DlgCoord.fY;
		}

		const SUICoord& ListBoxCoord = m_pListBoxEx->GetUICoord();
		float fPageSize = ListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height();
		m_pScrollBar->SetPageSize( (int)fPageSize );

		// UI의 최대길이는 Y:720이다.
		// OFFSET(70) * ((테이블의 칼럼갯수 / 4) - 1)
		// ex) 70 * ( 32개 / 4칸 ) - 1;
		//	   70 * 7 = 490;
		float fBGImageHeight = m_pSlotDlg->GetBGImageHeight();
		float fSize = fBGImageHeight / GetScreenHeightRatio() * GetEtDevice()->Height();
		int nEnd = int(fSize * ((m_nSlotMaxCount / TALISMAN_SLOT_WIDTH_CNT) - 1));
		m_pScrollBar->SetTrackRange(0, nEnd);

		int nCurrentPos = m_pScrollBar->GetTrackPos();
		float fNowYOffsetRatio = (float)nCurrentPos / fPageSize;
		float fHeight = ListBoxCoord.fHeight / GetScreenHeightRatio() /* * GetEtDevice()->Height()*/;
		m_pSlotDlg->SetPosition( m_fOriginalPosX, m_fOriginalPosY - fNowYOffsetRatio*fHeight );
	}
}

void CDnInspectTalismanDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render(fElapsedTime);

	if(IsShow() && m_pSlotDlg)
	{
		SetSlotOffset();

		RECT oldRect, newRect;
		GetEtDevice()->GetScissorRect( &oldRect );

		const SUICoord& ContentListBoxCoord = m_pListBoxEx->GetUICoord();
		newRect.left = int((ContentListBoxCoord.fX+GetXCoord()) / GetScreenWidthRatio() * GetEtDevice()->Width());
		newRect.top = int((ContentListBoxCoord.fY+GetYCoord()) / GetScreenHeightRatio() * GetEtDevice()->Height());
		newRect.right = newRect.left + int(ContentListBoxCoord.fWidth / GetScreenWidthRatio() * GetEtDevice()->Width() );
		newRect.bottom = newRect.top + int(ContentListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height() );

		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE);
		GetEtDevice()->SetScissorRect( &newRect );

		m_pSlotDlg->Show( true );
		m_pSlotDlg->Render( fElapsedTime );
		m_pSlotDlg->Show(false);

		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetScissorRect( &oldRect );
		GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE);
	}
}

void CDnInspectTalismanDlg::RefreshTalismanEquipData()
{
	CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
	if( !pParentInspectPlayerDlg ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pParentInspectPlayerDlg->GetSessionID() );
	if( !hActor ) return;

	CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	if( !pActor ) return;

	for( DWORD i=0; i<TALISMAN_MAX; i++ )
	{
		m_pSlotDlg->SetSlotItem(i, pActor->GetTalisman(i) );
	}
}

void CDnInspectTalismanDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	CDnCustomDlg::Show(bShow);

	if(m_pSlotDlg == NULL)
		return;

	if(bShow)
	{
		RefreshTalismanEquipData();

		m_pSlotDlg->SetAcceptInputMsgWhenHide(bShow);
		m_pSlotDlg->SetElementDialogShowState(bShow);
	}
	else
	{
		if(m_pDetailInfoDlg->IsShow()) m_pDetailInfoDlg->Show(false);

		m_pSlotDlg->SetAcceptInputMsgWhenHide(false);
		m_pSlotDlg->SetElementDialogShowState(false);
	}
}

void CDnInspectTalismanDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if(IsCmdControl("ID_BT_INFO"))
		{
			if( !m_pDetailInfoDlg->IsShow() )
			{
				CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
				if( !pParentInspectPlayerDlg ) return;
				DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pParentInspectPlayerDlg->GetSessionID() );
				if( !hActor ) return;
				CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
				if( !pActor ) return;
				m_pDetailInfoDlg->SetText(pActor);
			}
			
			ShowChildDialog(m_pDetailInfoDlg, !m_pDetailInfoDlg->IsShow());
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDnInspectTalismanSlotDlg::CDnInspectTalismanSlotDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCharTalismanListDlg(dialogType, pParentDialog, nID, pCallback)
{
}

CDnInspectTalismanSlotDlg::~CDnInspectTalismanSlotDlg()
{
}

void CDnInspectTalismanSlotDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName( "PlayerTalismanListDlg.ui" ).c_str(), bShow );
	SetElementDialog(true);
}

void CDnInspectTalismanSlotDlg::InitialUpdate()
{
	CDnCharTalismanListDlg::InitialUpdate();
}

bool CDnInspectTalismanSlotDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	 return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnInspectTalismanSlotDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if(bShow)
	{
	}
	else
	{
		m_pToolTipDlg->Show(false);
	}

	CDnCustomDlg::Show(bShow);
}

void CDnInspectTalismanSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName(pControl->GetControlName());

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 상세정보보기 클래스
CDnInspectTalismanInfoDlg::CDnInspectTalismanInfoDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCharTalismanInfoDlg(dialogType, pParentDialog, nID, pCallback)
{
}

CDnInspectTalismanInfoDlg::~CDnInspectTalismanInfoDlg()
{
}

void CDnInspectTalismanInfoDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName( "PlayerTalismanInfoDlg.ui" ).c_str(), bShow );
}

void CDnInspectTalismanInfoDlg::InitialUpdate()
{
	CDnCharTalismanInfoDlg::InitialUpdate();
}

void CDnInspectTalismanInfoDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	CDnCustomDlg::Show(bShow);
}

bool CDnInspectTalismanInfoDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	return CDnCharTalismanInfoDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}
#endif // PRE_ADD_TALISMAN_SYSTEM