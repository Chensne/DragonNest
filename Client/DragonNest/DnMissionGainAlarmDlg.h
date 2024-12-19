#pragma once
#include "DnCustomDlg.h"
#include "DnMissionTask.h"

class CDnItem;

class CDnMissionGainAlarmDlg : public CDnCustomDlg
{
public:
	CDnMissionGainAlarmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMissionGainAlarmDlg(void);

protected:
	CEtUIStatic *m_pDescription;

	float m_fShowTime;
	float m_fShowRatio;
	float m_fElapsedTime;
	float m_fConst;

protected:
	void SetFadeTime( float fFadeTime );
	void SetText( LPCWSTR wszMsg, DWORD dwColor );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void SetMissionAlarm( CDnMissionTask::MissionInfoStruct *pInfo, float fFadeTime );
};
