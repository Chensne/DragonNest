#include "StdAfx.h"
#include "DnChangeActionStrByBubbleProcessor.h"
#include "DnPlayerActor.h"
#include "DnBubbleSystem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChangeActionStrByBubbleProcessor::CDnChangeActionStrByBubbleProcessor( DnActorHandle hActor, const char* pArg ) : IDnSkillProcessor( hActor ) 
{
	m_iType = CHANGE_ACTIONSTR_BY_BUBBLE;
	
	if( pArg )
	{
		// 버블ID;버블갯수;액션이름;버블ID;버블갯수;액션이름.... 3개씩 쌍으로.
		vector<string> strTokens;
		TokenizeA( pArg, strTokens, ";" );

		if (strTokens.size() == 0 || strcmp(pArg,"0") == 0) return;
		
		for( int i = 0; i < (int)strTokens.size(); i += 3 )
		{
			S_ACTION_BY_BUBBLE ActionByBubble;

			ActionByBubble.iBubbleID = atoi(strTokens.at( i ).c_str());
			ActionByBubble.iBubbleCount = atoi(strTokens.at( i+1 ).c_str());
			ActionByBubble.strActionName = strTokens.at( i+2 ).c_str();

			m_vlActionByBubble.push_back( ActionByBubble );
		}
	}
}


CDnChangeActionStrByBubbleProcessor::~CDnChangeActionStrByBubbleProcessor(void)
{
}


void CDnChangeActionStrByBubbleProcessor::SetHasActor( DnActorHandle hActor )
{
	_ASSERT( hActor && "void CDnChangeActionStrByBubbleProcessor::SetHasActor( DnActorHandle hActor ), Actor is NULL!!" );
	IDnSkillProcessor::SetHasActor( hActor );
}



void CDnChangeActionStrByBubbleProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{

}


void CDnChangeActionStrByBubbleProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{

}


void CDnChangeActionStrByBubbleProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}


bool CDnChangeActionStrByBubbleProcessor::IsFinished( void )
{
	return true;
}

const char *CDnChangeActionStrByBubbleProcessor::GetChangeActionNameAndRemoveNeedBubble( bool* pBChanged/* = NULL*/ )
{
	if( false == m_hHasActor->IsPlayerActor() )
		return NULL;

	const char* pResult = NULL;

	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hHasActor.GetPointer());
	BubbleSystem::CDnBubbleSystem* pBubbleSystem = pPlayerActor->GetBubbleSystem();

	// 현재 액션 변경 조건에 맞는게 있는가 확인.
	// 가장 큰 갯수로 처리한다.
	int iGreatestCount = 0;
	const S_ACTION_BY_BUBBLE* pSelectedActionByBubble = NULL;
	for( int i = 0; i < (int)m_vlActionByBubble.size(); ++i )
	{
		const S_ACTION_BY_BUBBLE& ActionByBubble = m_vlActionByBubble.at( i );
		int iNowBubbleCount = pBubbleSystem->GetBubbleCountByTypeID( ActionByBubble.iBubbleID );
		if( ActionByBubble.iBubbleCount <= iNowBubbleCount )
		{
			if( iGreatestCount < ActionByBubble.iBubbleCount )
			{
				iGreatestCount = ActionByBubble.iBubbleCount;
				pSelectedActionByBubble = &ActionByBubble;
			}
		}
	}

	if( pSelectedActionByBubble )
	{
		if( pBChanged )
			*pBChanged = true;

		// 버블 소모..  모양새는 별로 안좋지만 이 시점이 가장 명확한듯. 
		// 액션이 바뀌는 것은 스킬 사용한 클라에서 먼저 스킬을 사용하고 서버로 보내는 형식이기 때문에,
		// 클라에서 액션이 바뀌었는지 여부를 서버의 응답으로 알아내기 전 타이밍이어야 하므로 직접 클라에서도 판단하고
		// 서버에서도 판단하여 서버/클라/다른클라 각자 버블 갯수를 줄인다. 
		// 다른 클라도 마찬가지. 스킬 사용 패킷 받고 나서 스킬 사용할 때 버블 갯수 판단하여 액션을 바꾼다.
		pBubbleSystem->RemoveBubbleByTypeID( pSelectedActionByBubble->iBubbleID, pSelectedActionByBubble->iBubbleCount );

		pResult = pSelectedActionByBubble->strActionName.c_str();
	}

	return pResult;
}


void CDnChangeActionStrByBubbleProcessor::CopyFrom( IDnSkillProcessor* pProcessor )
{
	if( pProcessor )
	{
		m_vlActionByBubble.clear();

		CDnChangeActionStrByBubbleProcessor *pCopyProcessor = static_cast<CDnChangeActionStrByBubbleProcessor*>(pProcessor);
		if( pCopyProcessor->GetBubbleActionList()->empty() == false )
		{
			m_vlActionByBubble.resize( pCopyProcessor->GetBubbleActionList()->size() );
			std::copy( pCopyProcessor->GetBubbleActionList()->begin(), pCopyProcessor->GetBubbleActionList()->end(), m_vlActionByBubble.begin() );
		}
	}
}