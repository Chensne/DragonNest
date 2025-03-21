#include "StdAfx.h"
#include "XTCustomPropertyGridItem.h"
#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"


CXTCustomPropertyGridItem::CXTCustomPropertyGridItem(const CString& strCaption, LPCTSTR strValue, CString* pBindString)
: CXTPPropertyGridItem( strCaption, strValue, pBindString )
{
}


CXTCustomPropertyGridItem::CXTCustomPropertyGridItem(UINT nID, LPCTSTR strValue, CString* pBindString)
: CXTPPropertyGridItem( nID, strValue, pBindString )
{
}


void CXTCustomPropertyGridItem::OnInplaceButtonDown()
{
	m_pGrid->GetParent()->GetParent()->SendMessage( UM_PROPERTYGRID_ONCUSTOM_DIALOG, (WPARAM)0, (LPARAM)this );
}