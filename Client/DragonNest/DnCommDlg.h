#pragma once
#include "EtUIDialog.h"

class CDnCommDlg : public CEtUIDialog
{
public:
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	enum eCommDlgType
	{
		eCommDlg_TYPE_NPCTALK,
		eCommDlg_TYPE_COMMON,
	};
#endif
	CDnCommDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCommDlg(void);

protected:
	float m_fDelayTime;
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	eCommDlgType m_DlgType;
	bool m_bAutoCloseDialog;
#endif

public:
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	void SetDelayTime( float fDelayTime ) { m_fDelayTime = fDelayTime; m_bAutoCloseDialog = false; }
	bool IsAutoCloseDialog() { return m_bAutoCloseDialog; }
	void SetDlgType(eCommDlgType e) { m_DlgType = e; }
	eCommDlgType GetDlgType() { return m_DlgType; }
#else
	void SetDelayTime( float fDelayTime ) { m_fDelayTime = fDelayTime; }
#endif
	float GetDelayTime() { return m_fDelayTime; }
	void  CallBackFunc( DWORD dwID, DWORD dwSetID );
};