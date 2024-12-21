#include "stdafx.h"
#include "IPropertyDelegate.h"



IPropertyDelegate::IPropertyDelegate( wxWindow* pParent, int id ) : m_pPropGrid( NULL )
{
	m_pPropGrid = new wxPropertyGrid( pParent, id, wxDefaultPosition, wxDefaultSize, 
									  wxPG_BOLD_MODIFIED | wxPG_TOOLTIPS | wxTAB_TRAVERSAL | wxEXPAND );
}

IPropertyDelegate::~IPropertyDelegate(void)
{
	m_pPropGrid->Clear();
}


void IPropertyDelegate::Show( bool bShow )
{
	m_pPropGrid->Show( bShow );
	m_pPropGrid->Enable( true );

	if( bShow )
		OnShow();
}
