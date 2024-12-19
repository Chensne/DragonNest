#pragma once

#include "Singleton.h"
#include "EtActionBase.h"
#include "CriticalSection.h"

class CEtActionCoreMng : public CSingleton< CEtActionCoreMng >
{
public:
	CEtActionCoreMng();
	virtual ~CEtActionCoreMng();

	struct ActionHeader {
		char szHeaderString[32];
		int nVersion;
		int nActionCount;
	};

	struct ActionContainerStruct : public TBoostMemoryPool< ActionContainerStruct >
	{
		int nRefCount;
		std::string szFileName;
		std::vector<CEtActionBase::ActionElementStruct *> pVecActionElementList;
		std::map<std::string, int> MapSearch;
		std::vector<std::string> szVecUsingElementList;

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		void (__cdecl *pCheckInitSignalFunc)( CEtActionBase::ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
#endif
		void (__cdecl *pCheckPreSignalFunc)( CEtActionBase::ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
		void (__cdecl *pCheckPostSignalFunc)( CEtActionBase::ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
	};

protected:
	std::map<std::string, ActionContainerStruct*> m_szMapContainerList;
	std::vector<ActionContainerStruct*> m_pVecWaitDeleteList;
	CSyncLock m_SyncLock;
	bool m_bUseDynamic;

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	bool m_bProcessOptimize;
	float m_fOptimizeCheckTime;
	std::vector<int> m_vecOptimizeSignalIndex;
#endif

protected:
	void ReleaseContainer( ActionContainerStruct *pStruct );
	std::vector<CEtActionBase::ActionElementStruct *> *GetAction( const char *szFileName, CEtActionBase *pAction );

public:
	bool Initialize( bool bUseDynamic = true );
	void Finalize();
	bool IsActionObject( const char *szFullPathName );
	std::vector<CEtActionBase::ActionElementStruct *> *LoadAction( const char *szFileName, CEtActionBase *pAction );
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionMulti( const char *szFileName, CEtActionBase *pAction );
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionMulti( CStream *pStream, CEtActionBase *pAction );
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionBasic( const char *szFileName, CEtActionBase *pAction );
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE

	
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionBasicMultiVersion( CStream *pStream, CEtActionBase *pAction, std::vector<std::string> *szVecNameList = NULL, std::vector<DWORD> *nVecOffsetList = NULL, std::vector<DWORD> *nActionVersion = NULL );
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionBasic( CStream *pStream, CEtActionBase *pAction, std::vector<std::string> *szVecNameList = NULL, std::vector<DWORD> *nVecOffsetList = NULL );
#else
	std::vector<CEtActionBase::ActionElementStruct *> *LoadActionBasic( CStream *pStream, CEtActionBase *pAction );
#endif

	void RemoveAction( const char *szFileName, CEtActionBase *pAction );

	ActionContainerStruct *GetContainer( const char *szFileName );

	void FlushWaitDelete();
	int GetWaitDeleteCount() { return (DWORD)m_pVecWaitDeleteList.size(); }
	int GetActionCount() { return (DWORD)m_szMapContainerList.size(); }

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	void ProcessOptimize( float fDelta );
	void EnableProcessOptimize( bool bEnable ) { m_bProcessOptimize = bEnable; }
	bool IsEnableProcessOptimize() { return m_bProcessOptimize; }
	void SetOptimizeSignalIndexArray( std::vector<int> &SignalIndexArray ) { m_vecOptimizeSignalIndex = SignalIndexArray; }
	bool IsOptimizeSignal( int nSignalIndex );
#endif

//#if defined(_RDEBUG) || defined(_DEBUG)
	void ReleaseAllContainer();
//#endif		//#if defined(_RDEBUG) || defined(_DEBUG)
};

extern CEtActionCoreMng g_ActionCoreMng;