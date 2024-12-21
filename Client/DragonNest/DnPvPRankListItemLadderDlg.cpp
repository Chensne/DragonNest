#include "Stdafx.h"

#ifdef PRE_ADD_PVP_RANKING

#include "DnPvPRankListItemLadderDlg.h"

CDnPvPRankListItemLadderDlg::CDnPvPRankListItemLadderDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnPvPRankListItemDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{

}

void CDnPvPRankListItemLadderDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("RankLadderListDlg.ui").c_str(), bShow );	
}



#endif // PRE_ADD_PVP_RANKING