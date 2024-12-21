#pragma once

#include "EtUIDialog.h"
#include "DnBlindDlg.h"


class CDnNotifyArrowDialog : public CEtUIDialog, public CDnBlindCallBack
{
public:
	CDnNotifyArrowDialog( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNotifyArrowDialog();

	void SetDirection(EtVector3 vTarget, LOCAL_TIME nTime);
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Render( float fElapsedTime );

	virtual void OnBlindOpen() {}
	virtual void OnBlindOpened() {}
	virtual void OnBlindClose() {}
	virtual void OnBlindClosed();

private:
	EtVector3		m_vTargetPos;
	LOCAL_TIME		m_nViewTime;
	LOCAL_TIME		m_nSetTime;
	CEtUIStatic*	m_pStaticArrow;
};