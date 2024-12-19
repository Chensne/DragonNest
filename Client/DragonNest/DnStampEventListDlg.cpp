#include "StdAfx.h"

#ifdef PRE_ADD_STAMPSYSTEM

#include "DnItem.h"
#include "DnQuestTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#include "DnMainDlg.h"

#include "DnStampEventDlg.h"
#include "DnStampEventListDlg.h"


CDnStampEventListItem::CDnStampEventListItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{	
	memset( m_pStaticCheck, 0, sizeof(m_pStaticCheck) );
	memset( m_pStaticDayCheck, 0, sizeof(m_pStaticDayCheck) );
	memset( m_pItemSlotReward, 0, sizeof(m_pItemSlotReward) );
	m_pCheckBox = NULL;
	m_pStampData = NULL;
}

CDnStampEventListItem::~CDnStampEventListItem()
{
	SAFE_DELETE( m_pStampData );

	int size = COUNT_OF( m_pItemSlotReward );
	for( int i=0; i<size; ++i ) {		
		if( m_pItemSlotReward[i] )
		{
			CDnItem * pItem = (CDnItem * )m_pItemSlotReward[i]->GetItem();
			SAFE_DELETE( pItem );
			m_pItemSlotReward[i]->ResetSlot();
		}
	}

}

void CDnStampEventListItem::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_DayCheckListDlg.ui" ).c_str(), bShow );
}

void CDnStampEventListItem::InitialUpdate()
{
	char buf[64];

	int size = COUNT_OF( m_pItemSlotReward );
	for( int i=0; i<size; i++ )
	{
		sprintf_s( buf, "ID_BT_ITEM%d", i );
		m_pItemSlotReward[i] = GetControl<CDnItemSlotButton>(buf);
	}

	// 서버에서는 일요일부터 시작이다.
	size = COUNT_OF( m_pStaticCheck ) - 1;
	for( int i=0; i<size; i++ )
	{
		sprintf_s( buf, "ID_STATIC_MARK%d", i+1 );
		m_pStaticCheck[i] = GetControl<CEtUIStatic>(buf);
		m_pStaticCheck[i]->Show( false );
	}
	m_pStaticCheck[size] = GetControl<CEtUIStatic>("ID_STATIC_MARK0");
	m_pStaticCheck[size]->Show( false );

	
	//size = COUNT_OF( m_pStaticDayCheck );
	//for( int i=0; i<size; i++ )
	//{
	//	sprintf_s( buf, "ID_STATIC_DAYCHECK%d", i );
	//	m_pStaticDayCheck[i] = GetControl<CEtUIStatic>(buf);
	//	m_pStaticDayCheck[i]->Show( false );
	//}

	size = COUNT_OF( m_pStaticDayCheck ) - 1;
	for( int i=0; i<size; i++ )
	{
		sprintf_s( buf, "ID_STATIC_DAYCHECK%d", i+1 );
		m_pStaticDayCheck[i] = GetControl<CEtUIStatic>(buf);
		m_pStaticDayCheck[i]->Show( false );
	}
	m_pStaticDayCheck[size] = GetControl<CEtUIStatic>("ID_STATIC_DAYCHECK0");
	m_pStaticDayCheck[size]->Show( false );


}

void CDnStampEventListItem::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{		
		if( IsCmdControl("ID_BT_TITLE") )
		{	
			InputContent();
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnStampEventListItem::SetListItem( STableStampData * pData )
{	
	m_pStampData = pData;

	//CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_TEXT_CONTENT");
	CEtUIButton * pStatic = GetControl<CEtUIButton>("ID_BT_TITLE");
	if( pStatic )
	{
		pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->strID ) );
		pStatic->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->strDescriptID ) );
	}

	for( int i=0; i<2; ++i )
	{
		if( m_pItemSlotReward[ i ] && pData->items[i] )
			m_pItemSlotReward[i]->SetItem( pData->items[i], pData->items[i]->GetOverlapCount() );
	}
}


void CDnStampEventListItem::SetStamp( int idx, bool b )
{
	if( idx < AWEEK )
		m_pStaticCheck[idx]->Show( b );

}


void CDnStampEventListItem::InputContent()
{
	if( !m_pStampData )
		return;

	CDnMainDlg * pMainDlg = GetInterface().GetMainBarDialog();
	if( pMainDlg )
		pMainDlg->FindComplete();

	// 일일퀘스트.
	if( m_pStampData->type == 1 )
	{
		GetInterface().GetMainMenuDialog()->ShowIntegrateEventDialog( false );

		// 일일퀘 수령여부검사.
		bool bTake = true;
		int size = (int)m_pStampData->arrIndex.size();
		for( int i=0; i<size; ++i )
		{
			int questIndex = m_pStampData->arrIndex[i];
			if( GetQuestTask().FindQuest( questIndex ) == -1 )
			{
				
				// 완료여부검사.
				bool bCompleteQuest = false; 
				std::vector<Journal*> vecCompleteQuest;
				GetQuestTask().GetCompleteQuest( EnumQuestType::QuestType_PeriodQuest, vecCompleteQuest );
				int qSize = (int)vecCompleteQuest.size();
				for( int x=0; x<qSize; ++x )
				{
					Journal * pJ = vecCompleteQuest[ x ];
					if( pJ && pJ->nQuestIndex == questIndex )
					{
						bCompleteQuest = true;
						break;
					}
				}

				if( bCompleteQuest == false )
				{
					bTake = false;
					break;
				}
			}
		}

		// 주간일일퀘스트 완료여부검사
		bool bComplete = true;
		for( int i=0; i<AWEEK; ++i )
		{
			if( m_pStaticCheck[i]->IsShow() == false )
			{
				bComplete = false;
				break;
			}
		}

		// 모두받았음.
		if( bTake )
		{
			// 모두완료.
			if( bComplete )
				GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption2, 8381, textcolor::YELLOW ); // "오늘의 과제 퀘스트를 모두 완료하였습니다"

			// 비완료.
			else
				GetInterface().ShowDailyQuestTab(); // 퀘스트창 일일퀘스트탭 열기.
		}

		// 받지않은 퀘가있다.
		else
		{
			// 모두완료.
			if( bComplete )
				GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption2, 8381, textcolor::YELLOW ); // "오늘의 과제 퀘스트를 모두 완료하였습니다"

			// 비완료.
			else
				GetInterface().NPCSearch( m_pStampData->mapID, m_pStampData->npcID );
		}



		
	}

	// 일일미션.
	else if( m_pStampData->type == 2 )
	{
		// Stamp 테이블에 있는 GroupID 로  MissionSchedule 테이블에서 해당 직업의 레벨에 해당하는 미션목록을 가져온다.
		// 가져온 미션목록들과 내가 가지고 있는 일일미션 목록들을 비교해서 동일한 미션을 찾으면( 동일한 미션은 하나이다. )
		// 그 찾아낸 미션을 미션알리미에 등록한다.
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TMISSIONSCHEDULE );

		if( CDnActor::s_hLocalActor && pTable && pTable->IsExistItem( m_pStampData->nMissionGroupID ) )
		{
			CDnActor * pActor = static_cast< CDnActor * >(CDnActor::s_hLocalActor);		
			int missionScheduleID = (pActor->GetClassID() - 1) * 100 + pActor->GetLevel();

			char str[32] = {0,};
			sprintf_s( str, 32, "_DailyGroup%d", m_pStampData->nMissionGroupID );
			DNTableCell * pCell = pTable->GetFieldFromLablePtr( missionScheduleID, str );
			if( pCell )
			{
				std::vector<std::string> tokens;
				TokenizeA( pCell->GetString(), tokens, ";" );

				CDnMissionTask & rMissionTask = CDnMissionTask::GetInstance();
				CDnMissionTask::MissionInfoStruct * pStruct = NULL;

				bool bBreak = false;
				int missionCnt = rMissionTask.GetDailyMissionCount( CDnMissionTask::DailyMissionTypeEnum::Daily );
				for( int i=0; i<missionCnt; ++i )
				{					
					pStruct = rMissionTask.GetDailyMissionInfo( CDnMissionTask::DailyMissionTypeEnum::Daily, i );
					if( !pStruct )
						continue;
					
					int groupCnt = (int)tokens.size();
					for( int k=0; k<groupCnt; ++k )
					{
						if( pStruct->nNotifierID == atoi( tokens[k].c_str() ) )
						{								
							bBreak = true;
							break;
						}
					}

					if( bBreak )
						break;
				}
				
				// Mission.
				if( pStruct && bBreak )
				{
					DNNotifier::Type::eType _type = rMissionTask.GetNotifierType( pStruct );

					bool bReg = false;

					// 해당미션이 등록되어있으면 지우고 다시 등록.
					if( rMissionTask.IsRegisterNotifier( _type, pStruct->nNotifierID ) )
					{
						if( rMissionTask.RemoveNotifier( _type, pStruct->nNotifierID ) )
						{
							if( rMissionTask.RegisterNotifier( _type, pStruct->nNotifierID ) )
								bReg = true;
						}
					}				

					// 미션알리미가 꽉차있으면 하나 지우고 등록.
					if( bReg == false )
					{
						if( rMissionTask.PushBack( _type, pStruct->nNotifierID ) )
							bReg = true;
					}

					// 미션등록완료.
					if( bReg )
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7875 ), textcolor::YELLOW, 4.0f );
				}
				else
				{
					CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, L"TEST : 내 미션목록에 없는 미션ID 입니다 - 확인필요", textcolor::YELLOW, 4.0f );
				}
				
			}

		}

	}


}


void CDnStampEventListItem::ChangeWeekDay( int wDay )
{
	if( wDay < 1 || wDay > AWEEK )
		return;

	for( int i=0; i<AWEEK; i++ )
		m_pStaticDayCheck[i]->Show( false );
	m_pStaticDayCheck[ wDay-1 ]->Show( true );

}

#endif // PRE_ADD_STAMPSYSTEM