#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDWCTask.h"
#include "DnDwcDlg.h"
#include "DnDwcMemberListDlg.h"
#include "DnDwcAddMemberDlg.h"
#include "DnInterface.h"
#include "VillageSendPacket.h"
#include "TaskManager.h"
#include "DnCommonTask.h"
#include "DnCommunityDlg.h"

static bool CompareLeader( TDWCTeamMember s1, TDWCTeamMember s2 ) // ���Ŀ�
{
	bool bResult = false;
	if(s1.bTeamLeader) 
		bResult = true;

	return bResult;
}

CDnDwcDlg::CDnDwcDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEX(NULL)
, m_pInviteButton(NULL)
, m_pDIsBandButton(NULL)
, m_pStaticTeamName(NULL)
, m_pStaticPvPRank(NULL)
, m_pStaticResult(NULL)
, m_pStaticScore(NULL)
, m_pInviteTeamDlg(NULL)
, m_pHelpButton(NULL)
{
}

CDnDwcDlg::~CDnDwcDlg()
{
	SAFE_DELETE(m_pInviteTeamDlg);
}

void CDnDwcDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PVPDlg.ui" ).c_str(), bShow );

	m_pInviteTeamDlg = new CDnDwcAddMemberDlg(UI_TYPE_CHILD, this);
	m_pInviteTeamDlg->Initialize(false);
}

void CDnDwcDlg::InitialUpdate()
{
	m_pHelpButton	= GetControl<CEtUIButton>("ID_BT_SMALLHELP");
	m_pHelpButton->Enable(false);
	m_pListBoxEX	= GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
	m_pInviteButton = GetControl<CEtUIButton>("ID_BT_INVITE");
	m_pDIsBandButton = GetControl<CEtUIButton>("ID_BT_QUIT");

	CEtUIStatic* pTempStatic = NULL;
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT0");
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126095));	// mid: PVP����
	
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT1");
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2331));	// mid: ���

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT2");
	if(pTempStatic) pTempStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126020));	// mid: ����

	m_pStaticTeamName = GetControl<CEtUIStatic>("ID_TEXT_TEAMNAME");
	m_pStaticPvPRank  = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pStaticResult	  = GetControl<CEtUIStatic>("ID_TEXT_RESULT");
	m_pStaticScore    = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
}

void CDnDwcDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{	
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_BT_SMALLHELP"))
		{
		}
		else if( IsCmdControl("ID_BT_INVITE") )
		{
			ShowChildDialog(m_pInviteTeamDlg, !m_pInviteTeamDlg->IsShow());
		}
		else if( IsCmdControl("ID_BT_QUIT") )
		{
			{
				// �����̳� �����̳Ŀ� ���� ���ϴ� ��Ʈ�� ����
				int		nMidNumber = -1;
				bool	bIsLeader  = GetDWCTask().IsDWCTeamLeader();
				(bIsLeader)? nMidNumber = 120228 : nMidNumber = 120264;
				// MID: 120228 - ���� ���� ��ü �Ͻðڽ��ϱ�? ��� ������ ������ �Ҹ�ǰ� ���� ��ü�˴ϴ�.
				// MID: 120264 - ������ Ż���Ͻðڽ��ϱ�? Ż��� �ش����� ��� ������ ������ ������ϴ�.
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,nMidNumber), MB_OKCANCEL, E_CALLBACK_LEAVE_TEAM, this);
			}
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDwcDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_YES") || IsCmdControl("ID_OK"))
		{
			switch(nID)
			{
			case E_CALLBACK_LEAVE_TEAM:
				{
					SendDWCTeamLeave();
				}
				break;
			}
		}
	}
}

void CDnDwcDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{		
		RefreshUI();
		RefreshTeamInfo();
		RefreshTeamMemberInfo();			
	}

	CEtUIDialog::Show(bShow);
}

void CDnDwcDlg::Reset()
{
	// �����̳� �����̳Ŀ� ���� ���ϴ� ��Ʈ�� ����
	bool bIsLeader = GetDWCTask().IsDWCTeamLeader();
	m_pInviteButton->Enable(bIsLeader);

	int nStringNum = -1;
	(bIsLeader)? nStringNum = 120227 : nStringNum = 120262; // �� ��ü / �� Ż��
	m_pDIsBandButton->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nStringNum) );
}

void CDnDwcDlg::RefreshUI()
{
	// �����̳� �����̳Ŀ� ���� ���ϴ� ��Ʈ�� ����
	bool bIsLeader		= GetDWCTask().IsDWCTeamLeader();	
	bool bIsFullMember  = false;
	const std::vector<TDWCTeamMember> vMemberList = GetDWCTask().GetDwcTeamMemberList();
	
	if( vMemberList.empty() == false )
		bIsFullMember = ( DWC::eDWCMember::DWC_MAX_MEMBERISZE <= (int)vMemberList.size() );	

	// �ʴ��ư :: Ȱ�� / ��Ȱ��
	bool bIsEnable = (bIsLeader && !bIsFullMember);
	m_pInviteButton->Enable(bIsEnable);

	// Ż���ư :: Text ����
	int nStringNum = -1;
	(bIsLeader)? nStringNum = 120227 : nStringNum = 120262; // �� ��ü / �� Ż��
	m_pDIsBandButton->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nStringNum) );
}

void CDnDwcDlg::RefreshTeamInfo()
{
	m_vTeamInfomationList.clear();
	m_vTeamInfomationList = GetDWCTask().GetDwcTeamInfoList();
	if(!m_vTeamInfomationList.empty())
	{
		TDWCTeam sTeamData = m_vTeamInfomationList[0];
		
		// �� �̸�
		m_pStaticTeamName->SetText(sTeamData.wszTeamName);
		
		// �� ��ŷ
		WCHAR wszTemp[256] = { 0 , };
		_itow(sTeamData.nDWCRank, wszTemp, 10);
		m_pStaticPvPRank->SetText(wszTemp);

		// �� ����
		_itow(sTeamData.nDWCPoint, wszTemp, 10);
		m_pStaticScore->SetText(wszTemp);

		// �� �·�
		std::wstring wTempStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126039), sTeamData.nTotalWin, sTeamData.nTotalLose, sTeamData.nTotalDraw);
		int TotalCount = (sTeamData.nTotalWin + sTeamData.nTotalLose + sTeamData.nTotalDraw)? (sTeamData.nTotalWin + sTeamData.nTotalLose + sTeamData.nTotalDraw) : 1;
		wsprintf(wszTemp, L" %d%%", (int)((float)sTeamData.nTotalWin/(float)TotalCount*100));
		wTempStr.append(wszTemp);
		m_pStaticResult->SetText(wTempStr.c_str());

		m_pDIsBandButton->Enable(true);
	}
	else
	{
		CDnCommunityDlg*  pCommunityDlg = (CDnCommunityDlg*)this->GetParentDialog();
		if(pCommunityDlg) pCommunityDlg->SetCommunityTab(false);

		m_pStaticTeamName->SetText(L"-");
		m_pStaticPvPRank->SetText(L"-");
		m_pStaticScore->SetText(L"-");
		m_pStaticResult->SetText(L"-");
		m_pListBoxEX->RemoveAllItems();
		m_pInviteButton->Enable(false);
		m_pDIsBandButton->Enable(false);
	}
}

void CDnDwcDlg::RefreshTeamMemberInfo()
{
	m_vMemberInfomationList.clear();
	m_vMemberInfomationList = GetDWCTask().GetDwcTeamMemberList();
	if(!m_vMemberInfomationList.empty())
	{
		m_pListBoxEX->RemoveAllItems();

		std::sort(m_vMemberInfomationList.begin(), m_vMemberInfomationList.end(), CompareLeader); // ������ ���� ����

		for(int i = 0 ; i < (int)m_vMemberInfomationList.size() ; ++i)
		{
			TDWCTeamMember		 tempData = m_vMemberInfomationList[i];
			CDnDwcMemberListDlg* pInfo	  = m_pListBoxEX->AddItem<CDnDwcMemberListDlg>();
			if(pInfo) pInfo->SetData(tempData);
		}
	}
}

void CDnDwcDlg::RefreshAllData(eRefreshType eType /*= E_REFRESH_ALL_DATA*/)
{	
	switch(eType)
	{
	case eRefreshType::E_REFRESH_TEAM_DATA:
		{
			RefreshTeamInfo();
		}
		break;

	case eRefreshType::E_REFRESH_MEMBER_DATA:
		{
			RefreshTeamMemberInfo();
		}
		break;

	case eRefreshType::E_REFRESH_ALL_DATA:
		{
			RefreshTeamInfo();
			RefreshTeamMemberInfo();
		}
		break;
	}

	// UI Refresh
	RefreshUI();
}

#endif // PRE_ADD_DWC