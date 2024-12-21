#pragma once

#include "Task.h"
#include "DnCamera.h"
#include "CriticalSection.h"
#include "Singleton.h"
#include "EtUIDialog.h"
#include "DnHangCrashTask.h"

class CDnMapNameDlg  : public CEtUIDialog
{
public:
	CDnMapNameDlg()
	: CEtUIDialog( UI_TYPE_SELF, NULL, -1, NULL), m_pMapName( NULL ), m_fMinHeight( 0.0f ) {}
	virtual ~CDnMapNameDlg(void) {}

protected:
	CEtUIStatic *m_pMapName;
	float m_fMinHeight;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Render( float fElapsedTime );

public:
	void SetMapName( std::wstring wszMapName );
};

class CDnLoadingStateMachine;
class CDnLoadingTask : public CTask, public CSingleton<CDnLoadingTask>, public CDnHangMonitorObject
{
public:
	CDnLoadingTask();
	virtual ~CDnLoadingTask();

	enum LoadThreadLayer {
		CreateActor,
		CreateNpc,
		ReleaseActor,
		ReleaseNpc,
		LoadThreadLayer_Amount,
	};

	CSyncLock m_Lock;
	CSyncLock m_MachineLock;
protected:
	CDnLoadingStateMachine *m_pMachine;
	EtCameraHandle m_hCamera;
	EtTextureHandle m_hTexture;
	bool m_bEnableBackgroundLoad;
	bool m_bPauseRenderScreen;
	CSyncLock m_RenderLock;	
	SUICoord m_TodayRect;	
	std::wstring m_strTodayTip;
	bool m_bBusy;
	bool m_bPauseThreadLoad;
	bool m_bLastShowCursor;
	CDnMapNameDlg *m_pMapName;

	// 잡다 로딩 화면에 필요한 것들
	int m_nLoadStartTime;

#ifdef PRE_FIX_LOADINGSCREEN
	bool m_bLoadingScreenRendered;
#endif // #ifdef PRE_FIX_LOADINGSCREEN

	struct LoadPacketStruct {
		int nMainCmd;
		int nSubCmd;
		int nSize;
		char *pData;
	};
	struct LoadStruct {
		void *pThis;
		bool (__stdcall *pFunc)( void *pThis, void *pParam, int nParamSize, LOCAL_TIME LocalTime );
		int (__stdcall *pGetFunc)( void *pParam, int nParamSize );
		bool (__stdcall *pCheckFunc)( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nParamSize );
		bool (__stdcall *pSkipFunc)( int nMainCmd, int nSubCmd );
		void *pParam;
		int nSize;
		std::vector<LoadPacketStruct *> pVecPacketList;
		LOCAL_TIME LocalTime;
	};
	std::vector<LoadStruct *> m_pVecLoadList[LoadThreadLayer_Amount];

	HANDLE	m_hWaitComplete;

	EtTextureHandle m_hLoadingAniTexture;
	int m_nLoadingAniFrameCount;
	int m_nLoadingAniDelay;
	int m_nLoadingAniSpeed;
protected:
	int GetTodayTipStringID( int nTableID, int nLevel );
	void ParseIndex( std::string &szStr, std::vector<int> &nVecResult );

public:
	void EnableBackgroundLoading( bool bEnable, bool bForceClear = false );
	bool IsEnableBackgroundLoading() { return m_bEnableBackgroundLoad; }
	void PauseRenderScreen( bool bPause );

	bool Initialize( CDnLoadingStateMachine *pMachine, int nMapIndex = 0, int nStageConstructionLevel = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void InsertLoadObject( bool (__stdcall *Func)( void *pThis, void *pParam, int nParamSize, LOCAL_TIME LocalTime ), int (__stdcall *GetFunc)( void *pParam, int nParamSize ), bool (__stdcall *CheckFunc)( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nParamSize ), bool (__stdcall *SkipFunc)( int nMainCmd, int nSubCmd ), void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime, int nLayerIndex = 0 );
	bool InsertLoadPacket( void *pThis, int nMainCmd, int nSubCmd, void *pParam, int nSize );
	bool CheckSameLoadObject( void *pParam, int nSize, int nLayerIndex = 0 );

	virtual bool IsBusy() {return m_bBusy;}
	bool IsLoading();

	void WaitForComplete();

	CDnLoadingStateMachine *GetStateMachine() { return m_pMachine; }
	void ResetStateMachine() { m_pMachine = NULL; }
};


class CDnLoadingStateMachine {
public:
	CDnLoadingStateMachine() {}
	virtual ~CDnLoadingStateMachine() {
		if( CDnLoadingTask::IsActive() && CDnLoadingTask::GetInstance().GetStateMachine() == this )
			CDnLoadingTask::GetInstance().ResetStateMachine();
	}

protected:

public:
	virtual bool RenderScreen( LOCAL_TIME LocalTime, float fDelta ) { return false; }
};