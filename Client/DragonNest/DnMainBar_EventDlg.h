#pragma once

#include "EtUIDialog.h"
#define RIGHT_DEGREE_MAX	0.750491f
#define	LEFT_DEGREE_MAX		-3.893829f
class CDnMainBar_EventDlg : public CEtUIDialog
{
public:
	enum EventButtons{
		ID_BT_SECRET,
		ID_BT_CHALLENGE,
		ID_BT_DNGIRL,
		ID_BT_ALTEA,
		ID_BT_DRAGON,
		ID_BT_FISHING
	};

	CDnMainBar_EventDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMainBar_EventDlg(void);

protected:

	EtTextureHandle m_hBackLight;
	CEtUIStatic *m_pStaticBackLight;
	EtTextureHandle m_hBackBlack;
	CEtUIStatic *m_pStaticBackBlack;

	bool isShow;

	//Altea
	CEtUIButton * m_pAlteaDiceIcon;
	bool m_bAlteaDiceShow;
	bool m_bAlteaDiceAlarm;
	float m_fAlteaDiceAlarmTime;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	void SetAlteaDiceAlarm();
	void UpdateAlteaDice( float fElapsedTime );
	void SetAlteaDiceIcon();
};
