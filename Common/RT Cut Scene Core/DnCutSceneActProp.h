#pragma once
#include "DnCutSceneWorldProp.h"
#include "DnCutSceneActionBase.h"
#include "IDnCutSceneRenderObject.h"
#include "StringUtil.h"


class CDnCutSceneActProp : public CDnCutSceneWorldProp,
						   public IDnCutSceneRenderObject,
						   public CDnCutSceneActionBase
{
public:
	string m_strAniName;
	string m_strActName;
	string m_strDefaultActionName;

	bool m_bDestroy;
	CDnCutSceneActProp* m_pActionObject;

protected:
	bool _CreateObject( void );

public:
	CDnCutSceneActProp(void);
	virtual ~CDnCutSceneActProp(void);

	// from CDnCutSceneWorldProp
	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	bool InitializeTable( DNTableFileFormat*  pPropTable, int iTableID );
	void Process( LOCAL_TIME LocalTime, float fDelta );

	// from IDnCutSceneRenderObject
	int GetAniIndex( const char* pAniName );

	bool IsDestroy( void ) { return m_bDestroy; };

	void OnResetCutScene( void );
	void CheckActionObjectSignal( ActionElementStruct* pElement, CEtActionSignal* pSignal );

	virtual void OnSignal( int iSignalType, int iSignalArrayIndex, void* pSignalData, LOCAL_TIME LocalTime, LOCAL_TIME StartTime, LOCAL_TIME EndTime );
	CDnCutSceneActProp* OnCreateActionObject( const char* pSkinName, const char* pAniName, const char* pActionName );
};
