#include "stdafx.h"
#include <wx/wx.h>
#include "cwxAddActorDlg.h"



cwxAddActorDlg::cwxAddActorDlg( wxWindow* pParent, wxWindowID id, const wxString& title, const wxPoint& pos /* = wxDefaultPosition */, 
							   const wxSize& size /* = wxDefaultSize */, long style /* = wxDEFAULT_DIALOG_STYLE  */) : wxDialog( pParent, id, title, pos, size, style )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pBtnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pTextActorPostfix = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(200, 20) );
	m_pOKButton = new wxButton( this, wxID_OK, wxT("OK") );
	m_pCancelButton = new wxButton( this, wxID_CANCEL, wxT("Cancel") );

	m_pBtnSizer->Add( m_pOKButton, 0, wxALL|wxALIGN_CENTER, 5 );
	m_pBtnSizer->Add( m_pCancelButton, 0, wxALL|wxALIGN_CENTER, 5 );

	m_pTopSizer->Add( m_pTextActorPostfix, 0, wxALL|wxALIGN_CENTER|wxEXPAND, 5 );;
	m_pTopSizer->Add( m_pBtnSizer, 0, wxALIGN_RIGHT, 5 );

	SetSizer( m_pTopSizer );
	m_pTopSizer->SetSizeHints( this );
	m_pTopSizer->Fit( this );
}

cwxAddActorDlg::~cwxAddActorDlg(void)
{

}


void cwxAddActorDlg::GetInputtedString( wxString& strInputted )
{
	strInputted = m_pTextActorPostfix->GetLineText( 0 );
}