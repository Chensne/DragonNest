#include "StdAfx.h"
#include "DnQuestDescriptionDlg.h"
#include "EtUITextBox.h"
#include "DnTableDB.h"
#include "DnQuestTask.h"
#include "TaskManager.h"
#include "DnMainMenuDlg.h"
#include "DnQuestTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnQuestDescriptionDlg::CDnQuestDescriptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextHtmlBoxJournal(NULL)
	, m_pQuestImage(NULL)
	, m_pStaticDescTitle(NULL)
{
}

CDnQuestDescriptionDlg::~CDnQuestDescriptionDlg(void)
{
	SAFE_RELEASE_SPTR( m_hQuestImage );
}

void CDnQuestDescriptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "QuestDescriptionDlg.ui" ).c_str(), bShow );
}

void CDnQuestDescriptionDlg::InitialUpdate()
{
	m_pTextHtmlBoxJournal = GetControl<CEtUIHtmlTextBox>("ID_HTML_JOURNAL");
	m_pQuestImage = GetControl<CEtUITextureControl>("ID_QUEST_IMAGE");
	m_pStaticDescTitle = GetControl<CEtUIStatic>("ID_DESC_TITLE");
	
	m_pTextHtmlBoxJournal->SetDefaultTextColor( textcolor::NPC_SPEECH );
}

void CDnQuestDescriptionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnQuestDescriptionDlg::SetJournalText( int nQuestIndex, const JournalPage *pJournalPage )
{
	if( !pJournalPage )
		return false;

	std::wstring szText;
	szText = pJournalPage->szContents;
	g_DataManager.ReplaceStaticTalkParam(szText);

	if ( m_pStaticDescTitle )
		m_pStaticDescTitle->SetText(pJournalPage->szTitle.c_str());

	// Note : 이미지 셋팅

	std::string szTitleImage;
	if( pJournalPage->szContentsImage.empty() == false  ) { 
		ToMultiString( (WCHAR*)pJournalPage->szContentsImage.c_str(), szTitleImage );
		SAFE_RELEASE_SPTR( m_hQuestImage );
		m_hQuestImage = LoadResource( CEtResourceMng::GetInstance().GetFullName(szTitleImage.c_str()).c_str(), RT_TEXTURE );
		if ( m_hQuestImage ) {
			m_pQuestImage->SetTexture(m_hQuestImage, 0, 0, 256, 128);
			m_pQuestImage->SetManualControlColor( true );
			m_pQuestImage->SetTextureColor( 0x66FFFFFF );
			m_pQuestImage->Show( true );
		}
		else {
			m_pQuestImage->Show( false );
		}
	}
	else {
		m_pQuestImage->Show( false );
	}

	std::vector<boost::tuple<wstring, int, int> > vecString;
	std::wstring szProgress;
	GetQuestTask().GetCurJournalCountingInfoString( nQuestIndex, pJournalPage->nJournalPageIndex, vecString, szProgress );

	RemoveStringW(szText, std::wstring(L"<html>"));
	RemoveStringW(szText, std::wstring(L"</html>"));
	RemoveStringW(szText, std::wstring(L"<HTML>"));
	RemoveStringW(szText, std::wstring(L"</HTML>"));
	
	szText.insert(0, L"<HTML>");
	szText += L"<BR>";
	// 몬스터 관련 정보 표시 
	for( size_t i = 0 ; i < (int)vecString.size() ; i++ )
	{
		std::wstring wsz = vecString[i].get<0>();

		int nCnt = vecString[i].get<1>();
		int nTotalCnt = vecString[i].get<2>();

		if( nCnt >= 0 && nTotalCnt >= 0  ) {
			wsz += FormatW(L" [%d/%d]", nCnt, nTotalCnt );
		}
		else if( nCnt >= 0 && nTotalCnt < 0 ) {
			wsz += FormatW(L" [%d]", nCnt);
		}

		szText += wsz;
		szText += L"<BR>";
	}
	szText += L"</HTML>";

	m_pTextHtmlBoxJournal->ClearText();
	m_pTextHtmlBoxJournal->ReadHtmlString(szText.c_str());

	return true;
}
