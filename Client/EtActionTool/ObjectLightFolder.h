#pragma once

#include "ActionBase.h"
class CObjectLightFolder : public CActionBase
{
public:
	CObjectLightFolder();
	virtual ~CObjectLightFolder();

protected:

public:
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );

	void SaveLightSetting( CString szStr );
	void LoadLightSetting( CString szStr );


};