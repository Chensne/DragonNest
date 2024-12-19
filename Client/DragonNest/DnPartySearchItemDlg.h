#pragma once
#include "EtUIDialog.h"

class CDnPartySearchItemDlg : public CEtUIDialog
{
public:
	CDnPartySearchItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPartySearchItemDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	//	note by kallsite : ListBoxEx에서 Child UI를 지원하게 되면 셋팅 제거
	void SetInfo(const std::wstring& name, int nMapId, CEtUIDialog* pParent);
	int GetMapIdForMSG() const { return m_nMapIdForMSG; }

protected:
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	CEtUIStatic*		m_pStaticName;
#ifdef PRE_PARTY_DB
#else
	CEtUICheckBox*		m_pCheckBox;
#endif
	int					m_nMapIdForMSG;
	CEtUIDialog*		m_pPartyItemDialog;
};
