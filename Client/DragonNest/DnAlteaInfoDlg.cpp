#include "stdafx.h"
#include "DnAlteaInfoDlg.h"
#include "DnAlteaTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ALTEIAWORLD_EXPLORE )

CDnAlteaInfoDlg::CDnAlteaInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{

}

CDnAlteaInfoDlg::~CDnAlteaInfoDlg(void)
{
}

void CDnAlteaInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AlteaInfoDlg.ui" ).c_str(), bShow );
}

void CDnAlteaInfoDlg::InitialUpdate()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TALTEIAPREVIEWREWARD );
	if( NULL == pSox )
		return;

	std::vector<int> vecPreviewReward;
	const int nItemCount = pSox->GetItemCount();
	for( int itr = 0; itr < nItemCount; ++itr )
	{
		int nItemID = pSox->GetItemID( itr );
		vecPreviewReward.push_back( pSox->GetFieldFromLablePtr( nItemID, "_ItemID" )->GetInteger() );
	}

	for( DWORD itr = 0; itr < vecPreviewReward.size(); ++itr )
	{
		if( m_vecPreviewItemSlotBtn.size() <= itr )
			break;

		CDnItem * pItem = CDnItem::CreateItem( vecPreviewReward[itr], 0 );
		m_vecPreviewItemSlotBtn[itr]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	}
}

void CDnAlteaInfoDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BT_ITEM"))
		return;

	CDnItemSlotButton * pItemSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	m_vecPreviewItemSlotBtn.push_back( pItemSlotBtn );
}

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )