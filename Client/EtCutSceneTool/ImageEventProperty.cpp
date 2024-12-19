#include "StdAfx.h"
#include "ImageEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"


CImageEventProperty::CImageEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Fade Event Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Fade In/Out Property") );

	wxArrayString FadeArray;
	FadeArray.Add( wxT("Fade In") );
	FadeArray.Add( wxT("Fade Out") );
	m_aPGID[ FADE_KIND ] = m_pPropGrid->Append( wxEnumProperty( wxT("Kind"), wxPG_LABEL, FadeArray ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0 ) );
	m_aPGID[ IMAGE_FILE_NAME ] = m_pPropGrid->Append( wxFileProperty( wxT("Import File Name"), wxT("") ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_WILDCARD, wxT("dds Image File(*.dds)|*.dds") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_DIALOG_TITLE, wxT("Select Image File") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_SHOW_FULL_PATH, 0 );
	m_aPGID[ LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->SetPropertyValue( m_aPGID[ FADE_KIND ], 0 );
}

CImageEventProperty::~CImageEventProperty(void)
{
}


void CImageEventProperty::_UpdateProp( int iEventInfoID )
{

}

void CImageEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{

}

void CImageEventProperty::Initialize( void )
{

}

void CImageEventProperty::OnShow( void )
{

}

void CImageEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{

}

void CImageEventProperty::CommandPerformed( ICommand* pCommand )
{

}

void CImageEventProperty::ClearTempData( void )
{

}