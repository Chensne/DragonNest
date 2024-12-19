#pragma once
#include "EtUIDialog.h"
#ifdef PRE_ADD_VIP
#include "DnItemTask.h"
#endif

class CDnPopupFatigueDlg : public CEtUIDialog
{
public:
	CDnPopupFatigueDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPopupFatigueDlg(void);

protected:
	CEtUIStatic *m_pStatic1;
	CEtUIStatic *m_pStatic2;
	CEtUIStatic *m_pStatic3;
#ifdef PRE_ADD_VIP
	CEtUIStatic *m_pStaticVIP;
#endif
	CEtUIStatic *m_pStaticEvent0;
	CEtUIStatic *m_pStaticEvent1;
	CEtUIStatic *m_pStaticEvent2;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetFatigue( int nFatigue, int nWeekFatigue, int nPCRoomFatigue, int nEventFatigue );
#ifdef PRE_ADD_VIP
	void SetFatigue(CDnItemTask::eFatigueType type, int value);
#endif
};