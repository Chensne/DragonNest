#pragma once

#ifdef PRE_ADD_DWC
#include "DnCustomDlg.h"

class CDnPvPRankListItemDwcDlg : public CDnCustomDlg
{
public:
	CDnPvPRankListItemDwcDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnPvPRankListItemDwcDlg();

protected:
	enum
	{
		e_Ranking = 0,	// 순위
		e_TeamName,		// 팀 이름
		//e_Class,		// 클래스
		e_Captain,		// 팀장이름
		e_Result,		// 결과
		e_TotalScore,	// 평점

		e_Max,
	};
	CEtUIStatic*		m_pCtrlStatic[e_Max];
	CDnJobIconStatic*	m_pJobIcon;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	
	void SetDWCRankInfo(TDWCRankData* pInfo);
};


#endif // PRE_ADD_DWC