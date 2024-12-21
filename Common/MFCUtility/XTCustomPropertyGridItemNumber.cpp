#include "StdAfx.h"
#include "Common/XTPVC80Helpers.h"

#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"
#include "XTCustomPropertyGridItemNumber.h"

IMPLEMENT_DYNAMIC(CXTCustomPropertyGridItemNumber, CXTPPropertyGridItem)

CXTCustomPropertyGridItemNumber::CXTCustomPropertyGridItemNumber(const CString& strCaption, long nValue, long* pBindNumber )
: CXTPPropertyGridItemNumber( strCaption, nValue, pBindNumber )
{
}

CXTCustomPropertyGridItemNumber::CXTCustomPropertyGridItemNumber(UINT nID, long nValue, long* pBindNumber )
: CXTPPropertyGridItemNumber( nID, nValue, pBindNumber )
{
}

void CXTCustomPropertyGridItemNumber::OnInplaceButtonDown()
{
	m_pGrid->GetParent()->GetParent()->SendMessage( UM_PROPERTYGRID_ONCUSTOM_DIALOG, (WPARAM)0, (LPARAM)this );
}