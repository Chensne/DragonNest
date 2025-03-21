#pragma once

#include "ObjectBase.h"
class CObjectLightFolder : public CObjectBase
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

	virtual void ExportObject( FILE *fp, int &nCount );
	virtual void ImportObject( FILE *fp );

};