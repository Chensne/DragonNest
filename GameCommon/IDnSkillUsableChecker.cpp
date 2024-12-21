#include "StdAfx.h"
#include "IDnSkillUsableChecker.h"
#include "DnActorStateChecker.h"
#include "DnHPChecker.h"
#include "DnProbabilityChecker.h"
#include "DnTargetStateChecker.h"
#include "DnMovableChecker.h"
#include "DnJumpableChecker.h"
#include "DnHitChecker.h"
#include "DnToggleChecker.h"
#include "DnTableDB.h"
#include "DnEnergyChargeChecker.h"
#include "DnGroundMovableChecker.h"
#include "DnRangeChecker.h"
#include "DnBubbleCountChecker.h"
#include "DNSummonChecker.h"
#include "DnTargetStateEffectChecker.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



IDnSkillUsableChecker::IDnSkillUsableChecker(void)
: m_iType( 0 )
{
}

IDnSkillUsableChecker::~IDnSkillUsableChecker(void)
{
}



// static
int IDnSkillUsableChecker::MapOperator( char* pOperator )
{
	int iOperator = -1;

	if( strcmp( pOperator, "<" ) == 0 )
	{
		iOperator = OP_GREATER;
	}
	else
	if( strcmp( pOperator, "==" ) == 0 )
	{
		iOperator = OP_EQUAL;
	}
	else
	if( strcmp( pOperator, "!=" ) == 0 )
	{
		iOperator = OP_NOT_EQUAL;
	}
	else
	if( strcmp( pOperator, ">" ) == 0 )
	{
		iOperator = OP_LESS;
	}
	else
	if( strcmp( pOperator, "<=" ) == 0 )
	{
		iOperator = OP_GREATER_EQUAL;
	}
	else
	if( strcmp( pOperator, ">=" ) == 0 )
	{
		iOperator = OP_LESS_EQUAL;
	}
	else
	if( strcmp( pOperator, "&" ) == 0 )
	{
		iOperator = OP_AND;
	}
	else
	if( strcmp( pOperator, "!&" ) == 0 )
	{
		iOperator = OP_NOT_AND;
	}

	return iOperator;
}



// static factory 함수
IDnSkillUsableChecker* IDnSkillUsableChecker::Create( DnActorHandle hActor, int iCheckerIndex, int iSkillLevelTableID, int* pIParamOffset )
{
	IDnSkillUsableChecker* pNewChecker = NULL;

	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	switch( iCheckerIndex )
	{
		case 0:
			// 걍 null 리턴
			break;

		// StateChecker, 2 param
		case 1:
			{
				// 적절한 파라메터를 뽑아주자.
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pRequiredState = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pRequiredState );
				int iRequiredState = (CDnActorState::ActorStateEnum)CDnActorState::String2ActorStateEnum( pRequiredState );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pOperator = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pOperator );
				int iOperator = IDnSkillUsableChecker::MapOperator( pOperator );
				//_ASSERT( -1 != iOperator );?
				++(*pIParamOffset);

				if( pRequiredState && -1 != iOperator )
					pNewChecker = new CDnActorStateChecker( hActor, (CDnActorState::ActorStateEnum)iRequiredState, iOperator );
			}
			break;

		// HPChecker, 2 param
		case 2:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pPercentStr = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pPercentStr );
				int iPercent = atoi( pPercentStr );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pOperator = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pOperator );
				int iOperator = IDnSkillUsableChecker::MapOperator( pOperator );
				//_ASSERT( -1 != iOperator );
				++(*pIParamOffset);

				if( pPercentStr && -1 != iOperator )
					pNewChecker = new CDnHPChecker( hActor, (float)iPercent, iOperator );
			}
			break;

		// ProbChecker, 1 param
		case 3:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pProb = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pProb );
				float fProb = (float)atof( pProb );
				++(*pIParamOffset);
				
				if( pProb )
					pNewChecker = new CDnProbabilityChecker( hActor, (float)fProb );
			}
			break;

		// TargetStateChecker, 2 param
		case 4:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pRequiredState = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pRequiredState );
				int iRequiredState = (CDnActorState::ActorStateEnum)CDnActorState::String2ActorStateEnum( pRequiredState );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pOperator = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pOperator );
				int iOperator = IDnSkillUsableChecker::MapOperator( pOperator );
				//_ASSERT( -1 != iOperator );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pRadius = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pRadius );
				int iRadius = atoi( pRadius );
				++(*pIParamOffset);

				if( pRequiredState && -1 != iOperator && pRadius )
					pNewChecker = new CDnTargetStateChecker( hActor, (CDnActorState::ActorStateEnum)iRequiredState, iOperator, (float)iRadius );
			}
			break;

		// MovableChecker, 인자는 없다. 
		case 5:
			{
				pNewChecker = new CDnMovableChecker( hActor );
			}
			break;

		// JumpableChecker, 인자 없음.
		case 6:
			{
				pNewChecker = new CDnJumpableChecker( hActor );
			}
			break;

		case 7:
			{
				pNewChecker = new CDnHitChecker( hActor );
			}
			break;

		// ToggleChecker, 인자 1개
		case 8:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pToggleSkillID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pToggleSkillID );
				int iToggleSkillID = atoi( pToggleSkillID );
				++(*pIParamOffset);

				if( pToggleSkillID )
					pNewChecker = new CDnToggleChecker( hActor, iToggleSkillID );
			}
			break;

		// EnergyChargeChecker, 인자 없음
		case 9:
			{
				pNewChecker = new CDnEnergyChargeChecker( hActor );
			}
			break;

		// GroundMovableChecker, 인자 없음
		case 10:
			{
				pNewChecker = new CDnGroundMovableChecker( hActor );
			}
			break;

		// RangeChecker, 인자 없음, 게임서버에서는 쓰이지 않는다.
		case 11:
			{
				pNewChecker = new CDnRangeChecker( hActor );
			}
			break;

		// BubbleCountChecker, 인자 2개. 게임 서버, 클라 모두 사용.
		case 12:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pBubbleTypeIDStr = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pBubbleTypeIDStr );
				int iBubbleTypeID = atoi( pBubbleTypeIDStr );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pNeedCountStr = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pNeedCountStr );
				int iNeedCount = atoi( pNeedCountStr );
				++(*pIParamOffset);

				if( pBubbleTypeIDStr && pNeedCountStr )
				{
					pNewChecker = new CDnBubbleCountChecker( hActor, iBubbleTypeID, iNeedCount );
				}
			}
			break;
		// SummonChecker , 인자 2개 < 액터 아이디(; 으로 구분. 복수개 가능. 하나라도 만족하면 ok) , 범위 >
		case 13:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pActorIDArgs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pActorIDArgs );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pRange = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pRange );
				int iRange = atoi( pRange );
				++(*pIParamOffset);

				if( pActorIDArgs && pRange )
					pNewChecker = new CDnSummonChecker( hActor , pActorIDArgs, iRange );
			}
			break;
		//#53454 꼭두각시 상태효과 관련 - 설정된 상태효과를 가지는 대상이 있어야 스킬 사용 가능
		case 14:
			{
				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pTargetStateEffectIndex = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pTargetStateEffectIndex );
				int nStateEffectIndex = pTargetStateEffectIndex ? atoi(pTargetStateEffectIndex) : 0;
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pRange = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pRange );
				float fRange = pRange ? (float)atof( pRange ) : 0.0f;
				++(*pIParamOffset);

				sprintf_s( caBuf, "_CanUseParam%d", *pIParamOffset+1 );
				char* pSkillUserCheck = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pSkillUserCheck );
				bool bCheckSkillUser = pSkillUserCheck ? atoi(pSkillUserCheck) == 1 : false;
				++(*pIParamOffset);

				if( pTargetStateEffectIndex && pRange && pSkillUserCheck )
					pNewChecker = new CDnTargetStateEffectChecker( hActor , (STATE_BLOW::emBLOW_INDEX)nStateEffectIndex, fRange, bCheckSkillUser );
			}
			break;
		default:
			_ASSERT( !"IDnSkillUsableChecker::Create() -> 알 수 없는 Checker 타입. 생성 실패!" );
			break;
			
	}

	return pNewChecker;
}
