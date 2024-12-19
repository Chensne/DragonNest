#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;
class CDnEventTodayDlg : public CEtUIDialog
{
public:
	CDnEventTodayDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnEventTodayDlg(void);

protected:
	CEtUIStatic*		m_pEventExplain;

	CEtUICheckBox*		m_pEventShow;

	CEtUIButton*		m_pButtonClose;

	TCHAR				m_szEventToday[2048];

	bool				m_bEventToday;
	bool				m_bIsImage;

	EtTextureHandle m_hTexture;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );

	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
	void UpdateContent( int iSlotIndex, int iStartJobDegree, int iEndJobDegree, int iRewardSP );
	
	void ReadEventToday();
	void WriteEventToday( bool nCheck = true );
	bool CheckEventToday();
	void ProcessParse(TCHAR * szData, int & nItr);

	bool GetShowEvent()	{ return m_bEventToday; }

#ifdef PRE_ADD_START_POPUP_QUEUE
	void DisplayEventPopUp (const char* szUrlAddress);
#else
	void DisplayEventPopUp (char* szUrlAddress);
#endif
};
