#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDWCCharCreateDlg.h"
#include "DnDWCCharCreateSelectDlg.h"
#include "DnDWCCharCreateButtonDlg.h"
#include "DnInterface.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnLoginTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDWCCharCreateSelectDlg::CDnDWCCharCreateSelectDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_pComboBox(NULL)
, pParentDlg(NULL)
, m_nCharDBID(0)
, m_nSelectedTabID(0)
, m_nCharJobID(0)
{
	m_vRadioButtonList.reserve(DWC_CREATE_PLAYER_MAX);
	pParentDlg = (CDnDWCCharCreateDlg*)pParentDialog;
}

CDnDWCCharCreateSelectDlg::~CDnDWCCharCreateSelectDlg()
{
}

void CDnDWCCharCreateSelectDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvP_CharCreateSelect.ui" ).c_str(), bShow );
}

void CDnDWCCharCreateSelectDlg::InitialUpdate()
{
	m_vRadioButtonList.clear();
	for(int i = 0 ; i < DWC_CREATE_PLAYER_MAX ; ++i)
	{
		m_vRadioButtonList.push_back( GetControl<CEtUIRadioButton>(FormatA("ID_RBT_%d",i).c_str()) );
	}
	
	m_pComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_CHAR");

	MakeDWCCharCreateData();
}

void CDnDWCCharCreateSelectDlg::MakeDWCCharCreateData()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDWCCREATETABLE );	
	if( pSox == NULL )
		return;

	sDWCCharCreateData D;
	m_vDWCCharCreateDataList.clear();
	for(int i = 0 ; i < pSox->GetItemCount() ; ++i)
	{
		int nID = pSox->GetItemID(i);
		D.nID			= nID;
		D.nClassID		= pSox->GetFieldFromLablePtr(nID, "_ClassID")->GetInteger();
		D.nJobID1		= pSox->GetFieldFromLablePtr(nID, "_Jobcode1")->GetInteger();
		D.nJobID2		= pSox->GetFieldFromLablePtr(nID, "_Jobcode2")->GetInteger();
		D.nClassMidID   = pSox->GetFieldFromLablePtr(nID, "_JobName")->GetInteger();
		D.strClassImageFileName = pSox->GetFieldFromLablePtr(nID, "_ClassImage")->GetString();

		m_vDWCCharCreateDataList.push_back(D);
	}	
}

void CDnDWCCharCreateSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		// 선택한 ID 저장
		CEtUIRadioButton* pBtn = static_cast<CEtUIRadioButton*>(pControl);
		if(!pBtn) return;
		int nTabID = pBtn->GetTabID();
		if( nTabID < 0 || nTabID >= (int)m_vDWCCharCreateDataList.size() ) return;
		
		if(pParentDlg) 
		{
			// 캐릭터 일러스트 변경
			m_nSelectedTabID = nTabID;
			m_nCharJobID	 = m_vDWCCharCreateDataList[nTabID].nJobID1;
			pParentDlg->ChangeCharImage(m_vDWCCharCreateDataList[nTabID].strClassImageFileName, m_vDWCCharCreateDataList[nTabID].nClassMidID);
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		// 콤보 박스 변경
		CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if(pLoginTask)
		{
			std::vector<TCharListData> vCharList = pLoginTask->GetMyCharListData();
			if(!vCharList.empty())
			{
				CEtUIComboBox* pComboBox = static_cast<CEtUIComboBox*>(pControl);
				if(pComboBox) 
				{
					int nSelectedIndex = pComboBox->GetSelectedIndex();
					if(nSelectedIndex < 0 || nSelectedIndex >= (int)vCharList.size() )
						return;

#if defined(PRE_MOD_SELECT_CHAR)
					m_nCharDBID = vCharList[nSelectedIndex].biCharacterDBID;
#else
					m_nCharDBID = nSelectedIndex;
#endif
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDWCCharCreateSelectDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{
		Reset();
		SetCharNameComboBox();
	}

	CEtUIDialog::Show(bShow);
}

void CDnDWCCharCreateSelectDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
}

void CDnDWCCharCreateSelectDlg::Reset()
{
	m_nCharJobID = 0;
	m_nCharDBID  = 0;
	m_nSelectedTabID = 0;
	
	if(m_nSelectedTabID >= 0 && m_nSelectedTabID < (int)m_vRadioButtonList.size())
		m_vRadioButtonList[0]->SetChecked(true);
}

void CDnDWCCharCreateSelectDlg::SetCharNameComboBox()
{	
	// 내가 보유하고 있는 캐릭터 이름만 나온다..	
	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if(pLoginTask)
	{
		std::vector<TCharListData> vCharList = pLoginTask->GetMyCharListData();
		if(!vCharList.empty())
		{
			m_pComboBox->RemoveAllItems();

			for(int i = 0 ; i < (int)vCharList.size() ; ++i)
			{
				if(vCharList[i].cAccountLevel == AccountLevel_DWC)
					continue;

				if(vCharList[i].bDeleteFlag)
					continue;

				m_pComboBox->AddItem(vCharList[i].wszCharacterName, NULL, i);
			}

#if defined(PRE_MOD_SELECT_CHAR)			
			if(!m_vDWCCharCreateDataList.empty()) 
				m_nCharJobID = m_vDWCCharCreateDataList[0].nJobID1;
			m_nCharDBID = vCharList[0].biCharacterDBID;
			pParentDlg->ChangeCharImage(m_vDWCCharCreateDataList[0].strClassImageFileName, m_vDWCCharCreateDataList[0].nClassMidID);
#else
			m_nCharDBID = 0;
#endif
		}
	}
}
#endif // PRE_ADD_DWC