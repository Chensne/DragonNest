#include "StdAfx.h"
#include "DnIntegrateQuestNReputationDlg.h"
#include "DnQuestTabDlg.h"
#include "DnNpcReputationTabDlg.h"
#include "DnMainDlg.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
CDnIntegrateQuestNReputationDlg::CDnIntegrateQuestNReputationDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
,m_pButtonQuestDlg(NULL)
,m_pButtonReputationDlg(NULL)
,m_pQuestDlg(NULL)
,m_pReputationDlg(NULL)
,m_SelectedTabIndex(0)
{
}

CDnIntegrateQuestNReputationDlg::~CDnIntegrateQuestNReputationDlg()
{
}

void CDnIntegrateQuestNReputationDlg::Initialize( bool bShow )
{
	CEtUITabDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Tab_QuestBaseTabDlg.ui" ).c_str(), bShow );
}

void CDnIntegrateQuestNReputationDlg::InitialUpdate()
{
	CEtUITabDialog::InitialUpdate();

	m_pButtonQuestDlg = GetControl<CEtUIRadioButton>( "ID_RBT_QUEST" );
	m_pButtonQuestDlg->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 401));//"퀘스트" 401
	m_pQuestDlg = new CDnQuestTabDlg( UI_TYPE_CHILD, this );
	m_pQuestDlg->Initialize( false );
	AddTabDialog( m_pButtonQuestDlg, m_pQuestDlg );

	m_pButtonReputationDlg = GetControl<CEtUIRadioButton>( "ID_RBT_REPUTE" );
	m_pButtonReputationDlg->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3201)); //"호감도" 3201
	m_pReputationDlg = new CDnNpcReputationTabDlg( UI_TYPE_CHILD, this );
	m_pReputationDlg->Initialize( false );
	AddTabDialog( m_pButtonReputationDlg, m_pReputationDlg );

	m_pTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	
	//타임 이벤트 설정...
	SetCheckedTab( m_pButtonQuestDlg->GetTabID() );
}

void CDnIntegrateQuestNReputationDlg::Show( bool bShow )
{
	//bShow가 true일때만 호출 하도록 함. 마우스 커서 참조 카운트 엉킬 수 있음.
	if (bShow == true)
		SetCheckedTab(m_SelectedTabIndex);
	else
	{
		CDnMainDlg* pMainDlg = GetInterface().GetMainBarDialog();
		if (pMainDlg)
		{
			pMainDlg->ToggleMenuButton(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG, false);
			pMainDlg->ToggleMenuButton(CDnMainMenuDlg::QUEST_DIALOG, false);
		}
	}


	if( m_bShow == bShow )
		return;
	
	CEtUITabDialog::Show( bShow );
}

bool CDnIntegrateQuestNReputationDlg::IsEventTabShow(DWORD tabIndex)
{
	if (m_bShow == false)
		return false;

	return (tabIndex == GetCurrentTabID());
}

void CDnIntegrateQuestNReputationDlg::SetEnableEventTab(bool bEnable, DWORD tabIndex)
{
	switch(tabIndex)
	{
	case QUEST_TAB:
		{
			m_pButtonQuestDlg->Enable(bEnable);
		}
		break;
	case REPUTATION_TAB:
		{
			m_pButtonReputationDlg->Enable(bEnable);
		}
		break;
	}
}

void CDnIntegrateQuestNReputationDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg/* = 0*/ )
{
	SetCmdControlName( pControl->GetControlName() );

	switch(nCommand)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl("ID_BT_CLOSE") ) 
			{
				Show( false );
			}
		}
		break;

	case EVENT_RADIOBUTTON_CHANGED:
		{
			CDnMainDlg* pMainDlg = GetInterface().GetMainBarDialog();
			if (pMainDlg == NULL)
				break;

			if (IsCmdControl("ID_RBT_REPUTE"))
			{
				m_pTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3201)); // UISTRING : 호감도
				pMainDlg->ToggleMenuButton(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG, true);
				pMainDlg->ToggleMenuButton(CDnMainMenuDlg::QUEST_DIALOG, false);
			}
			else if (IsCmdControl("ID_RBT_QUEST"))
			{
				m_pTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 401)); // UISTRING : 퀘스트
				pMainDlg->ToggleMenuButton(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG, false);
				pMainDlg->ToggleMenuButton(CDnMainMenuDlg::QUEST_DIALOG, true);
			}
			else
			{
				m_pTitle->ClearText();
			}
		}
		break;
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnIntegrateQuestNReputationDlg::ShowQuestFromNotifier( int nChapterIndex, int nQuestIndex, bool bMainQuest )
{
	if (m_pQuestDlg)
		m_pQuestDlg->ShowQuestFromNotifier(nChapterIndex, nQuestIndex, bMainQuest);
}

#ifdef PRE_ADD_REPUTATION_EXPOSURE
void CDnIntegrateQuestNReputationDlg::SetReputeNpcUnionPage(int nNpcID)
{
	m_pReputationDlg->OnSelectNpcInUnionDlg(nNpcID);
}
#endif // PRE_ADD_REPUTATION_EXPOSURE


void CDnIntegrateQuestNReputationDlg::SetCheckDailyTab()
{
	if( m_pQuestDlg )
		m_pQuestDlg->SetCheckDailyTab();
}

#endif // PRE_ADD_INTEGERATE_QUEST_REPUT