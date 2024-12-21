#pragma once

#include "ObjectBase.h"

class CObjectSubmesh : public CObjectBase
{
public:
	CObjectSubmesh();
	virtual ~CObjectSubmesh();

protected:
	CString m_szEffectName;
	DWORD m_dwVertexCount;
	int m_nSubMeshIndex;

	int m_nTextureCount;
	std::vector<SCustomParam*> m_VecEffectList;
	std::vector<CString> m_VecEffectSemanticList;


public:
	virtual void Activate();
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual CString GetClassName() { return CString("SubMesh"); }
};