#pragma once

class CDnGameTask;

class CDnStageClearImp {
public:
	CDnStageClearImp( CDnGameTask *pTask ) { m_pGameTask = pTask; }
	virtual ~CDnStageClearImp() {}

protected:
	CDnGameTask *m_pGameTask;

public:
	virtual void OnCalcClearProcess( void *pData ) {}
	virtual void OnCalcClearRewardItemProcess( void *pData ) {}
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	virtual void OnCalcClearRewardBoxProcess( void *pData ) {}
#endif
};