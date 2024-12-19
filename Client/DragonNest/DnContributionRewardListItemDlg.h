#pragma once


#ifdef PRE_ADD_DONATION


#include "DnCustomDlg.h"

//------------------------------------------------
// ��νý��� - ��κ����� ItemDlg.

class CDnContributionRewardListItemDlg : public CDnCustomDlg
{

private:

	enum ETypeReward
	{
		MONTH = 0,
		DAY
	};

	CEtUIRadioButton * m_pRadioMonth;
	CEtUIRadioButton * m_pRadioDay;
	CEtUIStatic * m_pStaticRank; // Static ����.
	std::vector< class CDnItemSlotButton * > m_vItemSlots; // ItemSlot ������.
		

public:

	CDnContributionRewardListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnContributionRewardListItemDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();
	
	// Override - CEtUIDialog //	
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	void SetData( const struct SContributionRewardItems & aItems );
};


#endif