#pragma once
#include "EtUIDialog.h"

class CDnOptionBase
{
public:
	CDnOptionBase() {}
	virtual ~CDnOptionBase() {} 

public:	
	virtual bool IsChanged() = 0;
	virtual void ExportSetting() = 0;
	virtual void ImportSetting() = 0;
	virtual void ProcessCombo(const int index) {}
	virtual void CancelOption() {}
};

class CDnOptionDlg : public CEtUIDialog, public CDnOptionBase
{
public:
	CDnOptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnOptionDlg();

	struct SInputItem
	{
		BYTE m_cKey;
		BYTE m_cKeySecond;
		CEtUIStatic *m_pStaticBox;
		CEtUIButton *m_pButtonBox;

		SInputItem()
			: m_cKey(0)
			, m_cKeySecond(0)
			, m_pStaticBox(NULL)
			, m_pButtonBox(NULL)
		{
		}
	};

public:
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg ) override;
	virtual void Show( bool bShow ) override;	
};
