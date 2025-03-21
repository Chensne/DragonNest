#pragma once

#include "EtMatrixEx.h"
#include "EtWorldProp.h"
#include "DNTableFile.h"

// 기본적인 Prop을 구현한다.
class CDnCutSceneWorldProp : public CEtWorldProp
{
public:
	//enum PropType
	//{
	//	STATIC,
	//	ACTION,
	//	BROKEN,
	//	OPERATION,
	//	//LIGHT,

	//	PROP_TYPE_COUNT,
	//};

protected:
	EtMatrix			m_matWorld;

protected:
	virtual bool _CreateObject( void );
	virtual void* AllocPropData(int& usingCount);

public:
	MatrixEx		m_matExWorld;

public:
	CDnCutSceneWorldProp(void);
	virtual ~CDnCutSceneWorldProp(void);

	// from CEtWorldProp
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( DNTableFileFormat*  pPropTable, int iTableID );
	void EnableCastShadow( bool bEnable );
	void EnableReceiveShadow( bool bEnable );
	
	void UpdateMatrix( void );

	void Show( bool bShow ) { m_Handle->ShowObject( bShow ); };

	// 상속 받는 녀석들에게 제공. 상속 받은 거 아님.
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {};
};
