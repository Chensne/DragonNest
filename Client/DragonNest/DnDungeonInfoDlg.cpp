#include "StdAfx.h"
#include "DnDungeonInfoDlg.h"
#include "DnTableDB.h"
#include "DnFarmGameTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnQuestTask.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarTask.h"
#include "DnLatencyDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)	
#endif

const int NPC_UNION_ICON_TEXTURE_SIZE = 32;
#ifdef PRE_ADD_ACTIVEMISSION
const int NPC_UNION_MARK_TEXTURE_WIDTH = 256;
#endif PRE_ADD_ACTIVEMISSION

CDnDungeonInfoDlg::CDnDungeonInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pLatency = NULL;
	m_pStaticMapName = NULL;
	m_pStaticDiff = NULL;
	m_pStaticMovePosition = NULL;
	m_bMoveOffset = false;
	int i = 0;
	for (; i < _MAX_MARK_COUNT; ++i)
	{
		m_pMarks0[i] = NULL;
		m_pMarks1[i] = NULL;
	}

	SecureZeroMemory( m_pMarkCoord0, sizeof( m_pMarkCoord0 ) );
	SecureZeroMemory( m_pMarkCoord1, sizeof( m_pMarkCoord1 ) );
}

CDnDungeonInfoDlg::~CDnDungeonInfoDlg(void)
{
	SAFE_RELEASE_SPTR(m_hMarkTexture);
	SAFE_RELEASE(m_pLatency);
}

void CDnDungeonInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonInfoDlg.ui" ).c_str(), bShow );
}

void CDnDungeonInfoDlg::InitialUpdate()
{
	//Latency bar rlkt
	m_pLatency = new CDnLatencyDlg(UI_TYPE_CHILD, this);
	m_pLatency->Initialize(true);

	m_pStaticMapName = GetControl<CEtUIStatic>("ID_STATIC_MAPNAME");
	m_pStaticDiff = GetControl<CEtUIStatic>("ID_STATIC_DIFF");

	m_uiDefaultCoord[0] = m_pStaticMapName->GetUICoord();
	m_uiDefaultCoord[1] = GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord();
	m_uiDefaultCoord[2] = GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord();

	m_pStaticMovePosition = GetControl<CEtUIStatic>("ID_MOVE_POSITION");
	m_pStaticMovePosition->Show( false );
	m_uiMovePositionOffset.fX = m_pStaticMovePosition->GetUICoord().fX - GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fX;
	m_uiMovePositionOffset.fY = m_pStaticMovePosition->GetUICoord().fY - GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fY;

	int i = 0;
	std::string ctrlName;
	for (; i < _MAX_MARK_COUNT; ++i)
	{
		ctrlName = FormatA("ID_TEXTUREL_MARK0_%d", i);
		m_pMarks0[i] = GetControl<CEtUITextureControl>(ctrlName.c_str());
		m_pMarks0[i]->Show(false);
		m_pMarks0[i]->GetUICoord( m_pMarkCoord0[i] );
	}

	for (i = 0; i < _MAX_MARK_COUNT; ++i)
	{
		ctrlName = FormatA("ID_TEXTUREL_MARK1_%d", i);
		m_pMarks1[i] = GetControl<CEtUITextureControl>(ctrlName.c_str());
		m_pMarks1[i]->Show(false);
		m_pMarks1[i]->GetUICoord( m_pMarkCoord1[i] );
	}

	SAFE_RELEASE_SPTR(m_hMarkTexture);
	m_hMarkTexture = LoadResource("Repute_SmallMark.dds", RT_TEXTURE, true);
}

void CDnDungeonInfoDlg::RefreshInfo()
{
	if(m_pLatency) m_pLatency->SetDelay(1);


	// 새로 위치 정할 경우 혹시 이미 오프셋처리가 되어있다면 해제한다.
	if( m_bMoveOffset ) MoveOffset( false );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox ) return;

	int nID = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	int nStringTableID = pSox->GetFieldFromLablePtr( nID, "_MapNameID" )->GetInteger();

	// 적용 전 초기화
	m_pStaticMapName->SetUICoord( m_uiDefaultCoord[0] );
	GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->SetUICoord( m_uiDefaultCoord[1] );
	GetControl<CEtUIStatic>("ID_STATIC1")->SetUICoord( m_uiDefaultCoord[2] );

	// 길이가 길어질 경우 해당 컨트롤의 가로크기를 키우고, 필요한 나머지 컨트롤들을 왼쪽으로 이동시킨다.
	// 사이즈 변경시 다른 컨트롤들도 같이 이동시켜야하기 때문에, 이 기능을 엔진단에 넣기엔 애매하다.
	SUICoord uiCoord, textCoord;
	m_pStaticMapName->GetUICoord(uiCoord);
	SUIElement *pElement = m_pStaticMapName->GetElement(0);
	CalcTextRect( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ), pElement, textCoord );
	if(wcslen(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID )) > 10)
	{
		const wchar_t *oldName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
		wchar_t newName[19]; //rlkt_ok
		for(int i=0;i<15;i++)
		{
			newName[i] = oldName[i];
		}
		wcscat(newName,L"...\0");
		m_pStaticMapName->SetText( newName );
		m_pStaticMapName->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ) );
	}else{
		m_pStaticMapName->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ) );
	}
	/*
	if( uiCoord.fWidth < textCoord.fWidth+24.0f/DEFAULT_UI_SCREEN_WIDTH )
	{
		float fDiff = textCoord.fWidth+24.0f/DEFAULT_UI_SCREEN_WIDTH - uiCoord.fWidth;
		GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->SetPosition( GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord().fX - fDiff, GetControl<CEtUIStatic>("ID_STATIC_DUNGEON")->GetUICoord().fY );
		
		//GetControl<CEtUIStatic>("ID_STATIC1")->SetPosition( GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fX - fDiff, GetControl<CEtUIStatic>("ID_STATIC1")->GetUICoord().fY );
		
		m_pStaticMapName->SetPosition( uiCoord.fX - fDiff, uiCoord.fY );
		m_pStaticMapName->SetSize( uiCoord.fWidth + fDiff, uiCoord.fHeight );

		int i = 0;
		float posX = 0.f, posY = 0.f;
		SUICoord coord;
		for (; i < _MAX_MARK_COUNT; ++i)
		{
			m_pMarks0[i]->GetUICoord(coord);
			posX = m_pMarkCoord0[i].fX - fDiff;
			posY = coord.fY;
			m_pMarks0[i]->SetPosition(posX, posY);
		}

		for (i = 0; i < _MAX_MARK_COUNT; ++i)
		{
			m_pMarks1[i]->GetUICoord(coord);
			posX = m_pMarkCoord1[i].fX - fDiff;
			posY = coord.fY;
			m_pMarks1[i]->SetPosition(posX, posY);
		}
	}
	*/
	
	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask( "GameTask" ));

	if( pGameTask->GetGameTaskType() == GameTaskType::Farm )
	{
		WCHAR wszString[256];
		swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7422 ), dynamic_cast<CDnFarmGameTask *>(pGameTask)->GetFarmZoneNumber() );
		m_pStaticDiff->SetText( wszString );
	}
	else
	{
		switch( pGameTask->GetStageConstructionLevel() ) 
		{
			case 0:	m_pStaticDiff->SetTextColor( difficultycolor::EASY );	break;
			case 1:	m_pStaticDiff->SetTextColor( difficultycolor::NORMAL );	break;
			case 2:	m_pStaticDiff->SetTextColor( difficultycolor::HARD );	break;
			case 3:	m_pStaticDiff->SetTextColor( difficultycolor::MASTER );	break;
			case 4:	m_pStaticDiff->SetTextColor( difficultycolor::ABYSS );	break;
		}
		int nMapSubType = pSox->GetFieldFromLablePtr( nID, "_MapSubType" )->GetInteger();
		if( CDnWorld::MapSubTypeNest == nMapSubType && pGameTask->GetStageConstructionLevel() == 3 )	// 하드코어모드 무조건 어비스로 표시
		{
			m_pStaticDiff->SetTextColor( difficultycolor::ABYSS );
			m_pStaticDiff->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2374 ) );
		}
		else
		{
#ifdef PRE_ADD_NEST_DIFFICULTY_TRANSFORM_FORCING		// LV 60 네스트의 경우 테이블에서 추가된 난이도 탭을 읽어와서 변환 

			DNTableFileFormat* pDungeonEnterSox = GetDNTable( CDnTableDB::TMAP );
			if( pDungeonEnterSox->IsExistItem( nID ) 
				&& CDnWorld::MapSubTypeNest == nMapSubType 
				&& pDungeonEnterSox->GetFieldFromLablePtr( nID, "_LevelUIString")->GetInteger() > 0 ) 
			{
				int nColorType = pDungeonEnterSox->GetFieldFromLablePtr( nID, "_StringColor")->GetInteger();
				switch( nColorType )
				{
				case 0:	m_pStaticDiff->SetTextColor( difficultycolor::EASY );	break;
				case 1:	m_pStaticDiff->SetTextColor( difficultycolor::NORMAL );	break;
				case 2:	m_pStaticDiff->SetTextColor( difficultycolor::HARD );	break;
				case 3:	m_pStaticDiff->SetTextColor( difficultycolor::MASTER );	break;
				case 4:	m_pStaticDiff->SetTextColor( difficultycolor::ABYSS );	break;
				default: m_pStaticDiff->SetTextColor( descritioncolor::VIOLET ); break; // 색상정보가 없을때 강제로 표현 
				}
				m_pStaticDiff->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pDungeonEnterSox->GetFieldFromLablePtr( nID, "_LevelUIString")->GetInteger()  ) );
			}
			else
				m_pStaticDiff->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2370+pGameTask->GetStageConstructionLevel() ) );
#else
			m_pStaticDiff->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2370+pGameTask->GetStageConstructionLevel() ) );
#endif 
		}
	}

	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if (pReputationRepos == NULL)
		return;

	const std::vector<CDnInterface::SUnionMembershipData>& membershipData = pReputationRepos->GetUnionMembershipData();
	CDnInterface::GetInstance().RefreshRepUnionMembershipMark(membershipData);

	if (CDnGuildWarTask::IsActive())
	{
		eGuildWarStepType curStep = CDnGuildWarTask::GetInstance().GetCurrentGuildWarEventTimeType();
		bool bShow = (curStep != GUILDWAR_STEP_NONE && curStep != GUILDWAR_STEP_END);
		GetInterface().RefreshGuildWarMark(bShow, false);
	}


	m_NoticeMarkHandler.SetUpdateMarksFlag(true);
}

void CDnDungeonInfoDlg::MoveOffset( bool bMove )
{
	if( m_bMoveOffset == bMove )
		return;

	m_bMoveOffset = bMove;

	int nControlCount = GetControlCount();
	for( int i = 0; i < nControlCount; ++i )
	{
		CEtUIControl *pControl = GetControl(i);

		if( m_bMoveOffset )
			pControl->SetPosition( pControl->GetUICoord().fX + m_uiMovePositionOffset.fX, pControl->GetUICoord().fY + m_uiMovePositionOffset.fY );
		else
			pControl->SetPosition( pControl->GetUICoord().fX - m_uiMovePositionOffset.fX, pControl->GetUICoord().fY - m_uiMovePositionOffset.fY );
	}
}


void CDnDungeonInfoDlg::UpdateMarks(float fElapsedTime)
{
	if (m_NoticeMarkHandler.IsUpdateMarks())
	{
		int i = 0;
		for (; i < _MAX_MARK_COUNT; ++i)
		{
			m_pMarks0[i]->Show(false);
			m_pMarks1[i]->Show(false);
		}

		const std::list<CDnMarkInformer*>& markInfoList = m_NoticeMarkHandler.GetMarkInformerList();
		std::list<CDnMarkInformer*>::const_iterator iter = markInfoList.begin();
		for (i = 0; i < _MAX_MARK_COUNT && iter != markInfoList.end(); ++i, ++iter)
		{
			if (i >= (int)markInfoList.size())
				break;

			const CDnMarkInformer* pInformer = (*iter);
			if (pInformer)
			{
				const int& iconIdx = pInformer->GetIconIdex();
				CEtUITextureControl* pCtrl = (m_pStaticMovePosition->IsShow()) ? m_pMarks1[i] : m_pMarks0[i];
				if (pCtrl)
				{
#ifdef PRE_ADD_ACTIVEMISSION
					// Repute_SmallMark.dds 의 가로가 꽉찾음. 세로로 내리도록 수정.
					int x = (iconIdx * NPC_UNION_ICON_TEXTURE_SIZE) % NPC_UNION_MARK_TEXTURE_WIDTH;
					int y = (iconIdx * NPC_UNION_ICON_TEXTURE_SIZE) / NPC_UNION_MARK_TEXTURE_WIDTH;
					pCtrl->SetTexture(m_hMarkTexture, x, y * NPC_UNION_ICON_TEXTURE_SIZE,
						NPC_UNION_ICON_TEXTURE_SIZE, NPC_UNION_ICON_TEXTURE_SIZE);
#else
					pCtrl->SetTexture(m_hMarkTexture, (iconIdx % NPC_UNION_ICON_TEXTURE_SIZE) * NPC_UNION_ICON_TEXTURE_SIZE, (iconIdx / NPC_UNION_ICON_TEXTURE_SIZE) * NPC_UNION_ICON_TEXTURE_SIZE,
						NPC_UNION_ICON_TEXTURE_SIZE, NPC_UNION_ICON_TEXTURE_SIZE);
#endif PRE_ADD_ACTIVEMISSION		
					
					pCtrl->Show(true);
				}
			}
		}

		m_NoticeMarkHandler.SetUpdateMarksFlag(false);
	}

	m_NoticeMarkHandler.UpdateMarks(fElapsedTime, ((m_pStaticMovePosition->IsShow()) ? m_pMarks1 : m_pMarks0));
}

void CDnDungeonInfoDlg::Process(  float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	UpdateMarks(fElapsedTime);
}

bool CDnDungeonInfoDlg::OnMouseOver(CEtUITextureControl** ppCtrl, float fMouseX, float fMouseY)
{
	if (ppCtrl == NULL)
		return true;

	int i = 0;
	bool bOnMouse = false;
	for (; i < _MAX_MARK_COUNT; ++i)
	{
		if (ppCtrl[i] && ppCtrl[i]->IsShow() && ppCtrl[i]->IsInside(fMouseX, fMouseY))
		{
			float fX, fY;
			GetPosition(fX, fY);

			m_NoticeMarkHandler.OnMouseOver(i, fX + fMouseX, fY + fMouseY);
			return true;
		}
	}

	return false;
}

bool CDnDungeonInfoDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX, fMouseY;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			CDnInterface::GetInstance().CloseRepUnionMarkTooltip();
#if defined(PRE_ADD_VIP_FARM)
			CDnInterface::GetInstance().CloseLifeUnionMarkTooltip();
#endif
			CDnInterface::GetInstance().CloseGuildWarMarkTooltip();
			CDnInterface::GetInstance().CloseGuildRewardMarkTooltip();
#ifdef PRE_ADD_WEEKLYEVENT
			CDnInterface::GetInstance().CloseWeeklyEventMarkTooltip();
#endif

#ifdef PRE_ADD_ACTIVEMISSION
			CDnInterface::GetInstance().CloseActiveMissionEventMarkTooltip();
#endif // PRE_ADD_ACTIVEMISSION

			bool bRetMarks0 = OnMouseOver(m_pMarks0, fMouseX, fMouseY);
			if (bRetMarks0 == false)
				OnMouseOver(m_pMarks1, fMouseX, fMouseY);
		}
		break;
	}

	return bRet;
}

void CDnDungeonInfoDlg::SetUnionMarks(std::vector<int>& unionMarkTableIds)
{
	if (IsShow())
		m_NoticeMarkHandler.SetUnionMarks(unionMarkTableIds);
}

void CDnDungeonInfoDlg::SetGuildWarMark(bool bShow, bool bNew)
{
	if (IsShow())
		m_NoticeMarkHandler.SetGuildWarMark(bShow, bNew);
}

void CDnDungeonInfoDlg::SetGuildRewardMark(bool bShow, bool bNew)
{
	if (IsShow())
		m_NoticeMarkHandler.SetGuildRewardMark(bShow, bNew);
}

#if defined(PRE_ADD_WEEKLYEVENT)
void CDnDungeonInfoDlg::SetWeeklyEventMark(bool bShow, bool bNew)
{
	if (IsShow())
		m_NoticeMarkHandler.SetWeeklyEventMark(bShow, bNew);
}
#endif


#if defined(PRE_ADD_ACTIVEMISSION)
void CDnDungeonInfoDlg::SetActiveMissionEventMark( int acMissionID, std::wstring & str, bool bShow, bool bNew)
{
	m_NoticeMarkHandler.SetActiveMissionEventMark( acMissionID, str, bShow, bNew);
}

void CDnDungeonInfoDlg::GetActiveMarkPos( float & fx, float & fy )
{
	const std::list<CDnMarkInformer*>& markInfoList = m_NoticeMarkHandler.GetMarkInformerList();
	std::list<CDnMarkInformer*>::const_iterator iter = markInfoList.begin();
	for( int i = 0; i<_MAX_MARK_COUNT && iter != markInfoList.end(); ++i, ++iter )
	{
		if( i >= (int)markInfoList.size() )
			break;

		const CDnMarkInformer* pInformer = (*iter);
		if( pInformer && pInformer->GetGroupType() == eMarkGroupType::MARK_ACTIVEMISSION_ALERT )
		{		
			CEtUITextureControl* pCtrl = (m_pStaticMovePosition->IsShow()) ? m_pMarks1[i] : m_pMarks0[i];
			if( pCtrl )
			{
				float _fX, _fY;
				GetPosition( _fX, _fY );

				fx = pCtrl->GetProperty()->UICoord.fX + _fX;
				fy = pCtrl->GetProperty()->UICoord.fY + _fY;
			}
		}
	}
}

#endif // PRE_ADD_ACTIVEMISSION

void CDnDungeonInfoDlg::SetLatency(int Latency)
{
	if(m_pLatency)
		m_pLatency->SetDelay(Latency);
}