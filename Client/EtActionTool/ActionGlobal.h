#pragma once

#include "Timer.h"
#include "UnionValueProperty.h"
#include "PropertyGridCreator.h"
#include "ActionBase.h"

class CActionGlobal : public CActionBase {
public:
	CActionGlobal();
	virtual ~CActionGlobal();

protected:

public:
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual CString GetClassName() { return CString("GlobalObject"); }
};