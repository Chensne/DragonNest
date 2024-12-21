#include "StdAfx.h"
#include "cwxRTCutSceneStatusBar.h"
#include "ToolData.h"
#include "cwxPropertyPanel.h"
#include "RTCutSceneRenderer.h"


BEGIN_EVENT_TABLE(cwxRTCutSceneStatusBar, wxStatusBar)
	EVT_SIZE( cwxRTCutSceneStatusBar::OnSize )
	EVT_BUTTON( ASSIGN_BUTTON, cwxRTCutSceneStatusBar::OnAssignButton )
END_EVENT_TABLE()


cwxRTCutSceneStatusBar::cwxRTCutSceneStatusBar( wxWindow* pParent ) : wxStatusBar( pParent, wxID_ANY )
{
	static const int widths[ Field_Max ] = {-1, 150, 100};

	SetFieldsCount( Field_Max );
	SetStatusWidths( Field_Max, widths );

	m_pPositionStatic = new wxStaticText( this, POSITION_STATIC, wxT("Position") );
	m_pAssignButton = new wxButton( this, ASSIGN_BUTTON, wxT("Assign") );

	SetMinHeight( 15 );
}

cwxRTCutSceneStatusBar::~cwxRTCutSceneStatusBar(void)
{
}


void cwxRTCutSceneStatusBar::OnSize( wxSizeEvent& Event )
{
	wxRect rect;
	
	GetFieldRect( Field_Position, rect );
	m_pPositionStatic->SetSize( rect.x+2, rect.y+2, rect.width-4, rect.height-4 );

	GetFieldRect( Field_AssignButton, rect );
	m_pAssignButton->SetSize( rect.x+2, rect.y+2, rect.width-4, rect.height-4 );

	Event.Skip();
}


void cwxRTCutSceneStatusBar::OnAssignButton( wxCommandEvent& Event )
{
	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();

	pPropertyPanel->AssignPosition( pRenderer->GetLookAt() );
}