#include "StdAfx.h"
#include "DnItemUnSealDlg.h"
#include "ItemSendPacket.h"
#include "DnInterface.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnItemUnSealDlg::CDnItemUnSealDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pItemSlotBtn	= NULL;
	m_pItemName		= NULL;
	m_pCountStatic	= NULL;

	m_pOkBtn		= NULL;
	m_pCancelBtn	= NULL;
	m_pItem			= NULL;

	m_SlotIndex		= -1;
	m_biSerial		= 0;

	m_bRequestingUnseal = false;
	m_nUnSealSoundIdx	= -1;

	m_pSlotButton = NULL;
}

CDnItemUnSealDlg::~CDnItemUnSealDlg(void)
{
	CEtSoundEngine::GetInstance().RemoveSound( m_nUnSealSoundIdx );
}

void CDnItemUnSealDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("InvenItemGetDlg.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10042 );
	if( strlen( szFileName ) > 0 )
		m_nUnSealSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnItemUnSealDlg::InitialUpdate()
{
	m_pItemName		= GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pCountStatic	= GetControl<CEtUIStatic>("ID_TEXT_ACCOUNT");

	m_pOkBtn		= GetControl<CEtUIButton>("ID_OK");
	m_pCancelBtn	= GetControl<CEtUIButton>("ID_CANCEL");
}

void CDnItemUnSealDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_ITEM"))
		return;

	m_pItemSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	m_pItemSlotBtn->SetSlotType(ST_INVENTORY);
	m_pItemSlotBtn->SetSlotIndex(0);
}

bool CDnItemUnSealDlg::SetInfo(const CDnItem *pItem, CDnSlotButton *pSlotButton)
{
	if (pItem == NULL)
		return false;

	m_SlotIndex = pItem->GetSlotIndex();
	m_biSerial = pItem->GetSerialID();
	m_pSlotButton = pSlotButton;
	if( m_pSlotButton ) m_pSlotButton->SetRegist( true );

	SAFE_DELETE(m_pItem);
	TItemInfo itemInfo;
	((CDnItem*)pItem)->GetTItemInfo(itemInfo);
	m_pItem = GetItemTask().CreateItem(itemInfo);
	if (m_pItem)
		m_pItemSlotBtn->SetItem(m_pItem, m_pItem->GetOverlapCount());

	m_pItemName->SetText(m_pItem->GetName());
	std::wstring str;
	str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4123), pItem->GetSealCount());
	m_pCountStatic->SetText(str.c_str());	// UISTRING : %dȸ

	return true;
}

void CDnItemUnSealDlg::Show( bool bShow )
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		if (m_SlotIndex < 0)
		{
			_ASSERT(0);
			return;
		}
		m_bRequestingUnseal = false;
		m_pOkBtn->Enable(true);

		SetRenderPriority(this, true);
	}
	else
	{
		m_SlotIndex = -1;
		m_biSerial = 0;
		if( m_pSlotButton ) m_pSlotButton->SetRegist( false );
	}

	CDnCustomDlg::Show(bShow);
}

void CDnItemUnSealDlg::Process( float fElapsedTime )
{
	if (m_bRequestingUnseal)
		m_pOkBtn->Enable(false);

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnItemUnSealDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_OK"))
		{
			if (m_SlotIndex < 0)
			{
				_ASSERT(0);
				return;
			}

			if (m_bRequestingUnseal)
				return;

			SendUnSeal(m_SlotIndex, m_biSerial);
			m_bRequestingUnseal = true;
			return;
		}
		else if (IsCmdControl("ID_CANCEL") || IsCmdControl("ID_CLOSE"))
		{
			if (m_bRequestingUnseal == false)
				Show(false);
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}
void CDnItemUnSealDlg::OnRecvUnSealItem(int nResult)
{
	if (nResult == ERROR_NONE)
	{
		if (m_nUnSealSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nUnSealSoundIdx, false );
	}
}