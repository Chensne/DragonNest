#pragma once

#include "ActionBase.h"
#include "EtMatrixEx.h"
class CObjectLightDir : public CActionBase
{
public:
	CObjectLightDir();
	virtual ~CObjectLightDir();

protected:
	EtLightHandle m_LightHandle;
	SLightInfo m_BackupInfo;
	MatrixEx m_matExWorld;
	int m_nMouseFlag;

public:
	virtual void Activate();
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void Show( bool bShow );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime );

	SLightInfo GetLightInfo();
	void SetLightInfo( SLightInfo &Info );

	void RefreshLight();
	
};

