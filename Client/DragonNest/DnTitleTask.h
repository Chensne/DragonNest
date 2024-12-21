#pragma once

#include "Task.h"
#include "InputReceiver.h"
#include "EtBVLEngine.h"
#include "DnLoadingTask.h"

class CDnLoginTask;
class CDnTitleTask : public CTask, public CInputReceiver, public CDnLoadingStateMachine
{
public:
	CDnTitleTask();
	virtual ~CDnTitleTask();

protected:
	DnCameraHandle m_hCamera;

	boost::shared_ptr<CEtBVLEngine> m_pBVLEngine;
	int m_nLogoStep;
	bool m_bPlayMovie;

	static char *s_szMovieName[];

	bool m_bFinishPreoadLogin;

	CDnLoginTask *m_pLoginTask;

	// 하운드로딩바
	int m_nHoundLoading;
	int m_nForceAddHoundLoadingValue;
	DWORD m_dwForceAddTime;
	float m_fForceAddTimeDelta;

protected:
	//	static UINT __stdcall ProcessLoadThread( void *pParam );
	static bool __stdcall OnLoadLoginTasktCallback( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime );
	void PreLoadLoginTask();

	void ProcessHoundLoading( LOCAL_TIME LocalTime, float fDelta );

public:
	bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool RenderScreen( LOCAL_TIME LocalTime, float fDelta );

	void CheckPointHoundLoading( int nValue );
	void ForceAddHoundLoading( int nValue, DWORD dwTime );
};