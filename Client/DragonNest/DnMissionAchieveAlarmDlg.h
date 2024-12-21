#pragma once
#include "DnCustomDlg.h"
#include "DnMissionTask.h"

class CDnItem;

class CDnMissionAchieveAlarmDlg : public CEtUIDialog
{
public:
	CDnMissionAchieveAlarmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMissionAchieveAlarmDlg(void);

	enum
	{
#if defined(PRE_ADD_ACTIVEMISSION)
		NUM_ACHIEVEICON = 5,
#else //
		NUM_ACHIEVEICON = 4,	// ÇöÀç 4°³
#endif // #if defined(PRE_ADD_ACTIVEMISSION)
	};

protected:
	CEtUIStatic *m_pTitle[NUM_ACHIEVEICON];
	CEtUIStatic *m_pSubTitle[NUM_ACHIEVEICON];
	CEtUIStatic *m_pAchieveIcon[NUM_ACHIEVEICON];

	float m_fShowTime;
	float m_fShowRatio;
	float m_fElapsedTime;
	float m_fConst;

protected:
	void SetFadeTime( float fFadeTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void SetMissionAlarm( CDnMissionTask::MissionInfoStruct *pInfo, float fFadeTime );
};
