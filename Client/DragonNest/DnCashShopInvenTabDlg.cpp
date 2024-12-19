#include "StdAfx.h"
#include "DnCashShopInvenTabDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnInvenCashDlg.h"
#include "DnInvenSlotDlg.h"
/////////////////////////////////////////

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopInvenTabDlg::CDnCashShopInvenTabDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
, m_OrigHoriType( AT_HORI_NONE )
, m_OrigVertType( AT_VERT_NONE )
{
}

CDnCashShopInvenTabDlg::~CDnCashShopInvenTabDlg(void)
{
}

void CDnCashShopInvenTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSInvenTab.ui" ).c_str(), bShow );
}

void CDnCashShopInvenTabDlg::InitialUpdate()
{
	GetControl<CEtUIRadioButton>("ID_TAB_BASIC01")->Show( false );
	GetControl<CEtUIRadioButton>("ID_TAB_BASIC02")->Show( false );
	GetControl<CEtUIRadioButton>("ID_TAB_BASIC03")->Show( false );
	GetControl<CEtUIRadioButton>("ID_TAB_BASIC04")->Show( false );
	GetControl<CEtUIRadioButton>("ID_TAB_BASIC05")->Show( false );

	CEtUIRadioButton * pRad = GetControl<CEtUIRadioButton>("ID_TAB_BASIC06");
	if( pRad ) pRad->Show( false );


}

void CDnCashShopInvenTabDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON0"))
			Show(false);
	}
	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopInvenTabDlg::Show( bool bShow )
{
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		SetPositionCashShopInven(true);
	}
	else
	{
		SetPositionCashShopInven(false);
	}

	CEtUIDialog::Show(bShow);
}

void CDnCashShopInvenTabDlg::Render(float fElapsedTime)
{
	if( !IsAllowRender() ) return;

	// #42600 채팅룸 인벤과 마찬가지 처리.
	if( m_listChildDialog.size() > 0 && m_listChildDialog.front() != NULL && m_listChildDialog.front()->IsShow() )
		m_listChildDialog.front()->LoadDialogTexture();

	CEtUIDialog::Render(fElapsedTime);
}

void CDnCashShopInvenTabDlg::SetPositionCashShopInven( bool bCashShop )
{
	// 이 상수값은 직접 수정해야한다.
	const float fOffsetX = -119.0f / DEFAULT_UI_SCREEN_WIDTH;
	const float fOffsetY = 60.0f / DEFAULT_UI_SCREEN_HEIGHT;

	if( bCashShop )
	{
		// 강제로 Render를 호출하는 거로는 MsgProc처리가 복잡해져서 이렇게 임시로 Child 등록하는 방법을 택했다.
		// 보여질때 AllignType이랑 DlgCoord만 바꾸고 차일드로 연결했다가 다시 풀어주기만 하면 끝이다.
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			CDnInvenCashDlg *pCashInvenDlg = pInvenDlg->GetCashInvenDlg();
			if( pCashInvenDlg )
			{
				std::vector<CDnInvenSlotDlg *> &pVecInvenSlotDlg = pCashInvenDlg->GetInvenSlotDlgList();

				// 현재 다이얼로그의 정렬 속성 구하고,
				UIAllignHoriType HoriType;
				UIAllignVertType VertType;
				GetAllignType( HoriType, VertType );

				// 원본 구해두고,
				pCashInvenDlg->GetAllignType( m_OrigHoriType, m_OrigVertType );

				// 이동
				pCashInvenDlg->SetAllignType( HoriType, VertType );

#ifdef PRE_ADD_CASHINVENTAB
				pCashInvenDlg->SetAllignTypeCashTab( HoriType, VertType );
#endif // PRE_ADD_CASHINVENTAB		

				pCashInvenDlg->MoveDialog( fOffsetX, fOffsetY );
				for( int i = 0; i < (int)pVecInvenSlotDlg.size(); ++i )
				{
					pVecInvenSlotDlg[i]->SetAllignType( HoriType, VertType );
					//pVecInvenSlotDlg[i]->MoveDialog( fOffsetX , fOffsetY );	// Move의 경우 파생받아서 자식 슬롯페이지 모두에다가 적용시키므로 따로 안해도 된다.
				}

				// 임시 차일드 설정.
				AddChildDialog( pCashInvenDlg );
				ShowChildDialog( pCashInvenDlg, true );
			}
		}
	}
	else
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			CDnInvenCashDlg *pCashInvenDlg = pInvenDlg->GetCashInvenDlg();
			if( pCashInvenDlg && pCashInvenDlg->IsShow() )
			{
				std::vector<CDnInvenSlotDlg *> &pVecInvenSlotDlg = pCashInvenDlg->GetInvenSlotDlgList();

				// 이동
				pCashInvenDlg->SetAllignType( m_OrigHoriType, m_OrigVertType );

#ifdef PRE_ADD_CASHINVENTAB
				pCashInvenDlg->SetAllignTypeCashTab( m_OrigHoriType, m_OrigVertType );
#endif // PRE_ADD_CASHINVENTAB	

				pCashInvenDlg->MoveDialog( -fOffsetX, -fOffsetY );
				for( int i = 0; i < (int)pVecInvenSlotDlg.size(); ++i )
				{
					pVecInvenSlotDlg[i]->SetAllignType( m_OrigHoriType, m_OrigVertType );
					//pVecInvenSlotDlg[i]->MoveDialog( -fOffsetX , -fOffsetY );	// Move의 경우 파생받아서 자식 슬롯페이지 모두에다가 적용시키므로 따로 안해도 된다.
				}

				// 차일드 해제
				pCashInvenDlg->ReleaseNewGain();
				pCashInvenDlg->Show( false );
				m_listChildDialog.clear();
			}
		}
	}
}