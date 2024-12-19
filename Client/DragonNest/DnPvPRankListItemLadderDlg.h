#pragma once

#ifdef PRE_ADD_PVP_RANKING

#include "DnPvPRankListItemDlg.h"


// CDnPvPRankListItemDlg 과 동일하나  .ui 파일이 다름.
// CEtUITreeCtl 에 추가되는 ItemDlg 로써  TreeControl 에서 생성, 초기화를 처리하기때문에 
// 상태에따라 .ui 파일을 변경할 수가 없어서 클래스를 추가로 만듬.
class CDnPvPRankListItemLadderDlg : public CDnPvPRankListItemDlg
{
public:

	CDnPvPRankListItemLadderDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnPvPRankListItemLadderDlg(){}

	virtual void Initialize( bool bShow );
};


#endif // PRE_ADD_PVP_RANKING