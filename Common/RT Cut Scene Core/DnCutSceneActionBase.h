#pragma once

#include "SignalHeader.h"
#include "EtActionBase.h"


class IDnCutSceneRenderObject;
class IResourcePathFinder;
class CDnCutSceneActor;

class CDnCutSceneActionBase : public CEtActionBase
{
protected:
	IDnCutSceneRenderObject*		m_pEntity;
	IResourcePathFinder*			m_pPathFinder;

	vector<int>						m_vlAniIndexList;
	float							m_fFrame;
	int								m_iActionIndex;
	string							m_strActionName;
	LOCAL_TIME						m_LocalTime;

	float							m_fLastProcessSignalFrame;
	set<CEtActionSignal*>			m_setProcessedSignals;

public:
	CDnCutSceneActionBase( void );
	virtual ~CDnCutSceneActionBase( void );

	void Initialize( IDnCutSceneRenderObject* pEntity );
	void SetResPathFinder( IResourcePathFinder* pResPathFinder );
	IResourcePathFinder *GetResPathFinder() { return m_pPathFinder; }

	// from CEtActionBase
	bool LoadAction( const char* szFullPathName );
	void FreeAction( void );
	static void CheckPreSignal( ActionElementStruct* pElement, int nElementIndex, CEtActionSignal* pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	static void CheckPostSignal( ActionElementStruct* pElement, int nElementIndex, CEtActionSignal* pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	//////

	void ProcessSignal( ActionElementStruct* pActionElement, LOCAL_TIME LocalTime, float fFrame, float fPrevFrame );
	virtual void OnSignal( int iSignalType, int iSignalArrayIndex, void* pSignalData, LOCAL_TIME LocalTime, LOCAL_TIME StartTime, LOCAL_TIME EndTime ) {};

	void CacheAniIndex( void );
	const char* GetCurrentAction( void );
	void SetAction( int iAnindex );
	void SetAction( const char* pActionName, float fFrame, float fBlendFrame );
	void OnChangeAction( void );

	int GetCurrentActionIndex( void ) { return m_iActionIndex; };
};
