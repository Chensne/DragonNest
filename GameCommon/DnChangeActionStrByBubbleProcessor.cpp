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
		// ����ID;������;�׼��̸�;����ID;������;�׼��̸�.... 3���� ������.
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

	// ���� �׼� ���� ���ǿ� �´°� �ִ°� Ȯ��.
	// ���� ū ������ ó���Ѵ�.
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

		// ���� �Ҹ�..  ������ ���� �������� �� ������ ���� ��Ȯ�ѵ�. 
		// �׼��� �ٲ�� ���� ��ų ����� Ŭ�󿡼� ���� ��ų�� ����ϰ� ������ ������ �����̱� ������,
		// Ŭ�󿡼� �׼��� �ٲ������ ���θ� ������ �������� �˾Ƴ��� �� Ÿ�̹��̾�� �ϹǷ� ���� Ŭ�󿡼��� �Ǵ��ϰ�
		// ���������� �Ǵ��Ͽ� ����/Ŭ��/�ٸ�Ŭ�� ���� ���� ������ ���δ�. 
		// �ٸ� Ŭ�� ��������. ��ų ��� ��Ŷ �ް� ���� ��ų ����� �� ���� ���� �Ǵ��Ͽ� �׼��� �ٲ۴�.
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