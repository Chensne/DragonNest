#include "stdafx.h"
#include <wx/wx.h>
#include "cwxHistoryPanel.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"
#include "CmdProcessor.h"
#include "CommandSet.h"


BEGIN_EVENT_TABLE( cwxHistoryPanel, wxPanel )
	EVT_LISTBOX( LIST_HISTORY_ID, cwxHistoryPanel::OnSelChangeHistory )
END_EVENT_TABLE()


cwxHistoryPanel::cwxHistoryPanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pHistoryListBox = new wxListBox( this, LIST_HISTORY_ID );
	m_pTopSizer->Add( m_pHistoryListBox, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 3 );

	this->SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	// 마우스 휠 메시지는 3D 패널로 가도록 한다.
	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>( TOOL_DATA.GetMainFrame() );
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pHistoryListBox );
}

cwxHistoryPanel::~cwxHistoryPanel(void)
{
	
}


void cwxHistoryPanel::_UpdateHistorySelection( void )
{
	CCmdProcessor* pCmdProcessor = TOOL_DATA.GetCmdProcessor();
	int iNumCmdHistory = pCmdProcessor->GetNumCmdHistory();
	int iNowCmdIndex = pCmdProcessor->GetNowCmdHistoryIndex();
	if( -1 != iNowCmdIndex )
		m_pHistoryListBox->SetSelection( iNumCmdHistory-iNowCmdIndex-1 );
	else
		m_pHistoryListBox->SetSelection( -1 );
}


void cwxHistoryPanel::OnSelChangeHistory( wxCommandEvent& ListEvent )
{
	int iSelection = ListEvent.GetSelection();

	// 작업 히스토리가 갱신 되었으므로 모든 뷰들을 갱신.
	TOOL_DATA.UpdateToThisHistory( iSelection );
	_UpdateHistorySelection();
}


void cwxHistoryPanel::CommandPerformed( ICommand* pCommand )
{
	CCmdProcessor* pCmdProcessor = TOOL_DATA.GetCmdProcessor();
	if( pCmdProcessor->IsCommandUpdate() )
	{
		m_pHistoryListBox->Hide();
		m_pHistoryListBox->Clear();

		int iNumCmdHistory = pCmdProcessor->GetNumCmdHistory();
		for( int iCommand = 0; iCommand < iNumCmdHistory; ++iCommand )
		{
			pCommand = pCmdProcessor->GetCmdHistory( iCommand );
			m_pHistoryListBox->Insert( pCommand->GetDesc(), 0 );
		}
	
		m_pHistoryListBox->Show();

		_UpdateHistorySelection();
	}
}