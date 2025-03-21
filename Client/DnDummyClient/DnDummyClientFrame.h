#pragma once

class DnDummyInfoListCtrl;
class DnDummySettingPropertyPanel;

const int UI_UPDATE_TIME = 1000;
class DnDummyClientFrame : public wxFrame
{

public:
	DnDummyClientFrame(wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

	virtual ~DnDummyClientFrame();
	
	void DoUpdate();
	void AddLog(const TCHAR* szString);

	void OnMenuConnect( wxCommandEvent& MenuEvent );
	void OnMenuDisconnect( wxCommandEvent& MenuEvent );
	void OnMenuAutoConnect( wxCommandEvent& MenuEvent );
	void OnMenuStopAutoConnect( wxCommandEvent& MenuEvent );
	void OnMenuSaveInfomation( wxCommandEvent& MenuEvent );
	void OnMenuRetryConnect( wxCommandEvent& MenuEvent );
	void OnCloseWindow( wxCloseEvent& MenuEvent );
	
	void OnTimer(wxTimerEvent& event);
	void OnIdle( wxIdleEvent& IdleEvent );

private:
	bool _CreateWindow();
	void _MakeDummyFile();
	void _MakeDummyNexonFile(int nSplit = 0);


private:

	wxAuiManager					m_AuiManager;
	wxTextCtrl*						m_pLogTextCtrl;
	DnDummyInfoListCtrl*			m_pDummyInfoListCtrl;
	DnDummySettingPropertyPanel*	m_pDummySettingPropertyPanel;
	wxTimer			m_Timer;
	bool m_bAutoConnect;
	int m_nCurAutoIndex;
	int m_nAutoConnectBegin;
	int m_nAutoConnectEnd;
	DWORD m_nLastAutoConnectTime;
	DWORD m_nLastUpdateTime;

	wxToolBar* tb1;
	DECLARE_EVENT_TABLE()
};

void Log(const TCHAR* szString);
void ClearLog();