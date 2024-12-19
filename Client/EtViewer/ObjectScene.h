#pragma once

#include "ObjectBase.h"
class CObjectScene : public CObjectBase
{
public:
	CObjectScene();
	virtual ~CObjectScene();

protected:

public:
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void Activate();
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual CString GetClassName() { return CString("Scene"); }

	virtual void Show( bool bShow );

	virtual void ExportObject( FILE *fp, int &nCount );
	virtual void ImportObject( FILE *fp );
};