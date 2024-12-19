#pragma once

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "DnCustomDlg.h"

using namespace std;
class CDnCharTalismanInfoDlg : public CDnCustomDlg
{
public:
	enum{
		FLOAT_TYPE,
		DOUBLE_FLOAT_TYPE,
		INT_TYPE,
		DOUBLE_INT_TYPE
	};

	CDnCharTalismanInfoDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharTalismanInfoDlg();

	virtual void SetText();
	virtual void SetText(CDnPlayerActor* pActor);
	void AddStateText( int nStringID, int eType, float fFirstData = 0.0f, float fSecondData = 0.0f );
	void SetItemStateText( CDnState *pState );

protected:
	CEtUITextBox * m_pTextBox;

	vector< pair<int, int> >					m_vIntTypeList;
	vector< pair<int, pair<int, int> > >		m_vDoubleIntTypeList;
	vector< pair<int, float> >					m_vFloatTypeList;
	vector< pair<int, pair<float, float> > >	m_vDoubleFloatTypeList;

protected:
	void SetStatsInfo();
	void SetStatsInfo(CDnPlayerActor* pActor);
	void SetMagicChanceInfo();
	void SetGoldChanceInfo();
	
	void SetSkillInfo();
	void SetSpecialSkillInfo();

	void AddDescription( int nItemID );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};

#endif // PRE_ADD_TALISMAN_SYSTEM