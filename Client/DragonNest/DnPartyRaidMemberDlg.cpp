#include "StdAfx.h"

#ifdef PRE_ADD_NEWCOMEBACK
#include "DnPartyTask.h"
#include "DnPlayerActor.h"
#include "DnTableDB.h"
#endif // PRE_ADD_NEWCOMEBACK

#include "DnPartyRaidMemberDlg.h"
#include "DnPartyTask.h"
#include "DnInterface.h"
#include "DnSimpleTooltipDlg.h"
#include "DNIsolate.h"
#include "DnCommonUtil.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartyRaidMemberDlg::CDnPartyRaidMemberDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnPartyMemberDlg(dialogType, pParentDialog, nID, pCallback)
{
}

CDnPartyRaidMemberDlg::~CDnPartyRaidMemberDlg(void)
{
}

void CDnPartyRaidMemberDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("PartyInfoRaidListDlg.ui").c_str(), bShow);
}

void CDnPartyRaidMemberDlg::InitialUpdate()
{
	CDnPartyMemberDlg::InitialUpdate();

	char szUserLevel[32]={0};
	char szMaster[32]={0};
	char szName[32]={0};
	char szBase[32]={0};
	char szFreeCoinIcon[32] = {0};
	char szFreeCoinCount[32] = {0};
	char szCashCoinIcon[32] = {0};
	char szCashCoinCount[32] = {0};
	char szPartyInvite[32] = {0};
	char szPartyInviteText[32] = {0};
	char szPartyJobIcon[32] = {0};

	int i = 0;
	for (; i < RAIDPARTYCOUNTMAX; i++)
	{
		SPartyMemberRaidUIData data;
		sprintf_s( szUserLevel, 32, "ID_LEVEL%d", i );
		sprintf_s( szMaster, 32, "ID_MASTER%d", i );
		sprintf_s( szName, 32, "ID_NAME%d", i );

		sprintf_s( szBase, 32, "ID_BASE%d", i );

		sprintf_s( szFreeCoinIcon, 32, "ID_HEART_BLUE%d", i );
		sprintf_s( szFreeCoinCount, 32, "ID_COIN_FREE%d", i );
		sprintf_s( szCashCoinIcon, 32, "ID_HEART_RED%d", i );
		sprintf_s( szCashCoinCount, 32, "ID_COIN_CASH%d", i );

		data.pUserLevel		= GetControl<CEtUIStatic>(szUserLevel);
		data.pMaster		= GetControl<CEtUIStatic>(szMaster);
		data.pName			= GetControl<CEtUIStatic>(szName);
		data.pBase			= GetControl<CEtUIStatic>(szBase);
		data.pFreeCoinIcon	= GetControl<CEtUIStatic>(szFreeCoinIcon);
		data.pFreeCoinCount = GetControl<CEtUIStatic>(szFreeCoinCount);
		data.pCashCoinIcon	= GetControl<CEtUIStatic>(szCashCoinIcon);
		data.pCashCoinCount = GetControl<CEtUIStatic>(szCashCoinCount);

		sprintf_s( szPartyJobIcon, 32, "ID_CLASS%d", i);
		data.pJobIcon		= GetControl<CDnJobIconStatic>(szPartyJobIcon);

		sprintf_s(szPartyInvite, 32, "ID_STATIC_WAIT%d", i);
		sprintf_s(szPartyInviteText, 32, "ID_TEXT_WAIT%d", i);
		data.pStaticInvitation		= GetControl<CEtUIStatic>(szPartyInvite);
		data.pTextInviting			= GetControl<CEtUIStatic>(szPartyInviteText);

// #78053 귀환자 아이콘이 보여짐.
		sprintf_s( szUserLevel, 32, "ID_STATIC_RETURNBONUSEXP%d", i );
		CEtUIStatic * pStatic = GetControl<CEtUIStatic>(szUserLevel);
#ifdef PRE_ADD_NEWCOMEBACK
		sprintf_s( szUserLevel, 32, "ID_STATIC_RETURNBONUSEXP%d", i );
		data.pComebackUser = pStatic;
#else
		if( pStatic )
			pStatic->Show( false );
#endif // PRE_ADD_NEWCOMEBACK 

		m_PartyMemberDataList.push_back(data);
	}
}

void CDnPartyRaidMemberDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	CDnPartyMemberDlg::Show(bShow);

	if( bShow )
	{
#ifdef PRE_ADD_NEWCOMEBACK
		int size = CDnPartyTask::GetInstance().GetPartyCount();
		for( int i=0; i<size; ++i )
		{
			CDnPartyTask::PartyStruct * pStruct = CDnPartyTask::GetInstance().GetPartyData( i );
			if( pStruct->hActor )
			{
				CDnActor * pActor = static_cast< CDnActor * >( pStruct->hActor.GetPointer() );				
				CDnPlayerActor * pPlayer = dynamic_cast< CDnPlayerActor * >( pActor );				
				if( pPlayer )
				{
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
					int nItemID = pSox->GetItemID( pPlayer->GetAppellationIndex() );				
					if( pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger() == AppellationType::Type::eCode::ComeBack )
					{
						m_PartyMemberDataList[ i ].SetComeback( true );
					}
				}
			}
		}
#endif // PRE_ADD_NEWCOMEBACK
	}
}

bool CDnPartyRaidMemberDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));
			PointToFloat(MousePoint, fMouseX, fMouseY);

			int nFindSelect = FindInsideItem(fMouseX, fMouseY);
			if (nFindSelect > -1)
			{
				const SPartyMemberRaidUIData& member = m_PartyMemberDataList[nFindSelect];
				std::wstring tooltipStr;
				CDnSimpleTooltipDlg* tooltipDlg = GetInterface().GetSimpleTooltipDialog();

				if (tooltipDlg)
				{
					if (CDnPartyTask::IsActive() && member.pJobIcon->IsInside(fMouseX, fMouseY))
					{
						const CDnPartyTask::PartyStruct* pCurData = CDnPartyTask::GetInstance().GetPartyDataFromSessionID(member.GetSessionID());
						if (pCurData)
						{
							int jobId = CommonUtil::GetJobID(pCurData->cClassID, pCurData->cJobArray);
							tooltipStr = DN_INTERFACE::STRING::GetJobString(jobId);
						}
					}
					else
					{
						//	todo : 동시에 표시하기
						if (member.GetFatigueValue() <= 0)
						{
							//CDnPartyTask::PartyStruct* foundMemberData = GetPartyTask().GetPartyData(nFindSelect); // RefreshList()를 네트워크 업뎃할 때마다 하면 상관없지만, 그렇지 않다면 task에서 바로 가져다 쓰는 것 고려 by kalliste
							tooltipStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3540);	// UISTRING : 해당 멤버는 피로도를 모두 소모해서 스테이지에 들어갈 수 없습니다.
						}
						else if (GetIsolateTask().IsBlackList(member.GetName().c_str()))
						{
							tooltipStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3541);	// UISTRING : 차단된 캐릭터입니다.
						}
					}

					if (tooltipStr.empty() == false)
					{
						tooltipDlg->ShowTooltipDlg(member.GetControl(), true, tooltipStr, 0xffffffff, true);
					}
				}
			}
			else
			{
				CDnSimpleTooltipDlg* tooltipDlg = GetInterface().GetSimpleTooltipDialog();
				if (tooltipDlg && tooltipDlg->IsShow())
					tooltipDlg->Show(false);
			}
		}
		break;

	case WM_RBUTTONUP:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat(MousePoint, fMouseX, fMouseY);

			int nFindSelect = FindInsideItem(fMouseX, fMouseY);
			if( nFindSelect > -1 && nFindSelect < (int)m_PartyMemberDataList.size())
			{
				SetCurSelectIndex(nFindSelect);
				UpdateSelectBar();

				float dlgPosX, dlgPosY;
				GetPosition(dlgPosX, dlgPosY);
				dlgPosX += fMouseX;
				dlgPosY += fMouseY;
				ShowPartyContextMenu(true, dlgPosX, dlgPosY, m_PartyMemberDataList[nFindSelect]);
			}
		}
	}

	return CDnPartyMemberDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnPartyRaidMemberDlg::Process(float fElapsedTime)
{
	CDnPartyMemberDlg::Process(fElapsedTime);
}

int	 CDnPartyRaidMemberDlg::FindInsideItem(float fX, float fY)
{
	int i = 0;
	std::vector<SPartyMemberRaidUIData>::iterator iter = m_PartyMemberDataList.begin();
	for (; iter != m_PartyMemberDataList.end(); ++iter, ++i)
	{
		SPartyMemberRaidUIData& data = *iter;
		if (data.IsInsideItem(fX, fY))
		{
			if (data.GetUserLevel() > 0)
				return i;
			else
				return -2;
		}
	}

	return -1;
}

void CDnPartyRaidMemberDlg::SetPartyInfoLeader(int nLeaderIndex)
{
	std::vector<SPartyMemberRaidUIData>::iterator iter = m_PartyMemberDataList.begin();
	for (; iter != m_PartyMemberDataList.end(); ++iter)
	{
		SPartyMemberRaidUIData& data = *iter;
		data.SetMaster(false);
	}

	if (CDnPartyTask::IsActive() == false)
		return;

	if (CDnPartyTask::GetInstance().GetPartyRole() == CDnPartyTask::SINGLE)
		return;

	CDnPartyTask::PartyStruct* pStruct(NULL);
	DWORD dwPartyCount = CDnPartyTask::GetInstance().GetPartyCount();

	for (DWORD i = 0; i < dwPartyCount; i++)
	{
		if (m_PartyMemberDataList.size() <= i)
			break;

		pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
		if (!pStruct)
			continue;

		if (pStruct->cMemberIndex < 0 || pStruct->cMemberIndex >= m_PartyMemberDataList.size())
			continue;
		m_PartyMemberDataList[pStruct->cMemberIndex].SetMaster(pStruct->bPartyMaster);
	}
}

void CDnPartyRaidMemberDlg::RefreshList()
{
	std::vector<SPartyMemberRaidUIData>::iterator iter = m_PartyMemberDataList.begin();
	for (; iter != m_PartyMemberDataList.end(); ++iter)
	{
		SPartyMemberRaidUIData& data = *iter;
		data.InitInfoList();
	}

	if (CDnPartyTask::IsActive() == false)
		return;

	CDnPartyTask::PartyStruct *pStruct(NULL);
	int partyCount = (int)CDnPartyTask::GetInstance().GetPartyCount();

	int i = 0;
	for (iter = m_PartyMemberDataList.begin(); iter != m_PartyMemberDataList.end(); ++iter, ++i)
	{
		if (partyCount <= i)
			break;

		pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
		if( !pStruct ) continue;
		if( pStruct->bGMTrace ) continue;

		if ((pStruct->cMemberIndex >= m_PartyMemberDataList.size()) || (pStruct->cMemberIndex < 0))
			continue;

		SPartyMemberRaidUIData& data = m_PartyMemberDataList[pStruct->cMemberIndex];

		data.SetUserLevel( pStruct->cLevel );
		data.SetMaster( pStruct->bPartyMaster );
		int nJob = pStruct->cClassID;
		if( !pStruct->nVecJobHistoryList.empty() ) nJob = pStruct->nVecJobHistoryList[ pStruct->nVecJobHistoryList.size() - 1 ];
		data.SetJobClass( nJob );
		data.SetName( pStruct->wszCharacterName, GetIsolateTask().IsBlackList(pStruct->wszCharacterName));
		data.SetSessionID( pStruct->nSessionID );
		data.SetFatigueValue( pStruct->wFatigue );
		data.Enable((pStruct->wFatigue > 0));
		data.SetCoin((int)pStruct->wStdRebirthCoin, (int)pStruct->wCashRebirthCoin);
#ifdef PRE_ADD_NEWCOMEBACK		
		data.SetComeback( pStruct->bCheckComebackAppellation );
#endif // PRE_ADD_NEWCOMEBACK 
	}

	ResetCurSelectIndex();
}

bool CDnPartyRaidMemberDlg::ShowInvitationCtrl(bool bShow)
{
	if (bShow)
	{
		std::vector<SPartyMemberRaidUIData>::iterator iter = m_PartyMemberDataList.begin();
		for (; iter < m_PartyMemberDataList.end(); ++iter)
		{
			SPartyMemberRaidUIData& memberUnit = *iter;
			if (memberUnit.GetUserLevel() <= 0)
			{
				memberUnit.ShowInvitationCtrl(true);
				return true;
			}
		}
	}
	else
	{
		std::vector<SPartyMemberRaidUIData>::iterator iter = m_PartyMemberDataList.begin();
		for (; iter < m_PartyMemberDataList.end(); ++iter)
		{
			SPartyMemberRaidUIData& memberUnit = *iter;
			if (memberUnit.IsShowInvitationCtrl())
				memberUnit.ShowInvitationCtrl(false);
		}
		return true;
	}

	return false;
}

CEtUIControl* CDnPartyRaidMemberDlg::GetCurSelectCtrl()
{
	return m_PartyMemberDataList[GetCurSelectIndex()].GetControl();
}

void CDnPartyRaidMemberDlg::UpdateSelectBar()
{
	SUICoord uiCoordNumber, uiCoordSelectBar;

	if (GetCurSelectIndex() < 0 || GetCurSelectIndex() >= RAIDPARTYCOUNTMAX)
		return;

	CEtUIControl* pCtrl = GetCurSelectCtrl();
	CEtUIStatic* pBar = GetSelectBar();
	if (pCtrl == NULL || pBar == NULL)
		return;

	pCtrl->GetUICoord(uiCoordNumber);
	pBar->GetUICoord(uiCoordSelectBar);
	uiCoordSelectBar.fX = uiCoordNumber.fX;
	uiCoordSelectBar.fY = uiCoordNumber.fY;
	pBar->SetUICoord(uiCoordSelectBar);
	pBar->Show(true);
}

int CDnPartyRaidMemberDlg::GetCurSelectUserSessionID() const
{
	if (GetCurSelectIndex() < 0 || GetCurSelectIndex() >= (int)m_PartyMemberDataList.size())
		return 0;

	return m_PartyMemberDataList[GetCurSelectIndex()].nSessionID;
}


#ifdef PRE_ADD_NEWCOMEBACK
void CDnPartyRaidMemberDlg::SetComebackAppellation( UINT sessionID, bool bComeback )
{
	int size = (int)m_PartyMemberDataList.size();
	for( int i=0; i<size; ++i )
	{
		if( m_PartyMemberDataList[ i ].nSessionID == sessionID )
		{
			m_PartyMemberDataList[ i ].SetComeback( bComeback );
			break;
		}
	}
}
#endif // PRE_ADD_NEWCOMEBACK