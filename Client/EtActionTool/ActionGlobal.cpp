#include "StdAfx.h"
#include "ActionGlobal.h"
#include "RenderBase.h"


CActionGlobal::CActionGlobal()
{
}

CActionGlobal::~CActionGlobal()
{
}

void CActionGlobal::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Grid", CUnionValueProperty::Boolean, "Grid Show/Hide", TRUE },
		{ "Common", "Axis", CUnionValueProperty::Boolean, "Axis Show/Hide", TRUE },
		NULL,
	};
	AddPropertyInfo( Default );
}

void CActionGlobal::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( CRenderBase::GetInstance().IsShowGrid() );
			break;
		case 1:
			pVariable->SetVariable( CRenderBase::GetInstance().IsShowAxis() );
			break;
	}
}

void CActionGlobal::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			CRenderBase::GetInstance().ShowGrid( pVariable->GetVariableBool() );
			break;
		case 1:
			CRenderBase::GetInstance().ShowAxis( pVariable->GetVariableBool() );
			break;
	}
}

void CActionGlobal::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
}
