#pragma once


// 액터를 추가하기 위해 이름을 입력 받는 다이얼로그
class cwxAddActorDlg : public wxDialog
{
private:
	wxTextCtrl*			m_pTextActorPostfix;
	
	wxBoxSizer*			m_pTopSizer;
	wxBoxSizer*			m_pBtnSizer;

	wxButton*			m_pOKButton;
	wxButton*			m_pCancelButton;

public:
	
	cwxAddActorDlg( wxWindow* pParent, wxWindowID id, const wxString& title, 
				    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
				    long style = wxDEFAULT_DIALOG_STYLE );

	virtual ~cwxAddActorDlg(void);

	void GetInputtedString( /*IN OUT*/ wxString& strInputted );
};
