#pragma once
#include <wx/wx.h>

class cwxRTCutSceneStatusBar : public wxStatusBar
{
public:
	enum
	{
		POSITION_STATIC = 1000,
		ASSIGN_BUTTON,
	};

	enum
	{
		Field_Text,
		Field_Position,
		Field_AssignButton,
		Field_Max,
	};

private:
	wxStaticText* m_pPositionStatic;
	wxButton* m_pAssignButton;

public:
	cwxRTCutSceneStatusBar( wxWindow* pParent );
	virtual ~cwxRTCutSceneStatusBar(void);

	void OnSize( wxSizeEvent& Event );
	void OnAssignButton( wxCommandEvent& Event );
	
	void SetPositionText( const wxString& Position ) { m_pPositionStatic->SetLabel(Position); };

	DECLARE_EVENT_TABLE()
};
