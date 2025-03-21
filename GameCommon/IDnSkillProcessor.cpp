#include "StdAfx.h"
#include "IDnSkillProcessor.h"
#include "DnPlayAniProcess.h"
#include "DnPartialPlayProcessor.h"
#include "DnChangeProjectileProcessor.h"
#include "DnDivideSEArgumentByTargets.h"
#include "DnImpactBlowProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnRangeSEProcessor.h"
#include "DnPetPlayAniProcess.h"
#include "DnCheckKeyProcessor.h"
#include "DnTableDB.h"

#ifdef _GAMESERVER
#include "DnAuraProcessor.h"
#include "DnApplySEWhenActionSetBlowEnabledProcessor.h"
#include "DnApplySEWhenTargetNormalHitProcessor.h"
#include "DnCallMySummonedMonsterProcessor.h"
#include "DnStateEffectApplyOnOffByBubbleProcessor.h"
#endif

#include "DnChangeActionStrByBubbleProcessor.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



IDnSkillProcessor::IDnSkillProcessor( DnActorHandle hActor ) : m_hHasActor( hActor ), 
															   m_fTimeLength( 0.0f ),
															   m_iType( COUNT )
						
{

}

IDnSkillProcessor::~IDnSkillProcessor(void)
{

}



IDnSkillProcessor* IDnSkillProcessor::Create( DnActorHandle hActor, int iProcessorIndex, int iSkillLevelTableID, int* pIParamOffset, set<string>& vlUseActions )
{
	IDnSkillProcessor* pNewProcessor = NULL;

	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );

	// 인자를 순차적으로 읽어들이므로 _GAMSERVER 디파인으로 구분한 것들과 아닌 발현타입이 섞여 있을 경우 문제가 될 수 있음.
	switch( iProcessorIndex-1 )
	{
		//// none 
		//case 0:
		//	
		//	break;

		// PlayAni
		case PLAY_ANI:
			{
				// 적절한 파라메터를 뽑아주자.
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pActionName );
				vlUseActions.insert( pActionName );
				++(*pIParamOffset);

				if( pActionName )
					pNewProcessor = new CDnPlayAniProcess( hActor, pActionName );
			}
			break;

		// Partial Play
		case PARTIAL_PLAY_ANI:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pStartActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				vlUseActions.insert( pStartActionName );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pLoopActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				vlUseActions.insert( pLoopActionName );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pEndActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				vlUseActions.insert( pEndActionName );
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pTimeLength = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pMoveSpeed = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				_ASSERT( pStartActionName && pLoopActionName && pMoveSpeed && pTimeLength && pMoveSpeed );
				if( pStartActionName && pLoopActionName && pMoveSpeed && pTimeLength && pMoveSpeed )
				{
					int iTimeLength = atoi( pTimeLength );
					int iMoveSpeed = atoi( pMoveSpeed );

					pNewProcessor = new CDnPartialPlayProcessor( hActor, pStartActionName, pLoopActionName, pEndActionName, 
																							 iTimeLength, (float)iMoveSpeed );
				}
			}
			break;

		// ChangePrj
		case CHANGE_PROJECTILE:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pSourceWeaponID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pChangeWeaponID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				_ASSERT( pSourceWeaponID && pChangeWeaponID );
				if( pSourceWeaponID && pChangeWeaponID )
				{
					int iSourceWeaponID = atoi( pSourceWeaponID );
					int iChangeWeaponID = atoi( pChangeWeaponID );

					pNewProcessor = new CDnChangeProjectileProcessor( hActor, iSourceWeaponID, iChangeWeaponID );
				}
			}
			break;

		// Divide State Effect Argument
		case DIVIDE_STATE_EFFECT_ARG:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pSEArgument = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				_ASSERT( pSEArgument );
				if( pSEArgument )
				{
					int iSEArgument = atoi( pSEArgument );

					pNewProcessor = new CDnDivideSEArgumentByTargets( hActor, iSEArgument );
				}
			}
			break;

		// Impact Blow, 인자 없음
		case IMPACT_BLOW:
			{
				pNewProcessor = new CDnImpactBlowProcessor( hActor );
			}
			break;

		// Aura, 인자는 "범위"로 1개 받는다.
		// 게임서버에서만 생성해서 사용됨
		case AURA:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pRangeArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

#ifdef _GAMESERVER
				_ASSERT( pRangeArg );
				if( pRangeArg )
				{
					float fRange = (float)atof( pRangeArg );

					pNewProcessor = new CDnAuraProcessor( hActor, fRange );
				}
				else
				{
					g_Log.Log( LogType::_ERROR, L"IDnSkillProcessor* IDnSkillProcessor::Create() - AURA Processor Create Fail!!\n" );
				}
#endif
			}
			break;
		// Change Action String
		case CHANGE_ACTIONSTR:
			{
				//char *szAddStr = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ProcessParam1" )->GetString();
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char *szAddStr = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				DNVector(std::string) szVecList;
				char *szActionName;
				for( int i=1; i<10; i++ ) {
					sprintf_s( caBuf, "_ProcessParam%d", (*pIParamOffset) + 1 );
					szActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
					if( szActionName == NULL || strlen( szActionName ) == 0 || strcmp( szActionName, "0" ) == 0 ) continue;

					++(*pIParamOffset);
					szVecList.push_back( szActionName );
				}
				pNewProcessor = new CDnChangeActionStrProcessor( hActor, szAddStr, szVecList );
			}
			break;

		// RangeStateEffect
		// 겜 서버에서만 사용함. 인자는 범위, 확률, 대상에게 적용할 상태효과 이펙트 테이블 인덱스, processor 지속시간
		case RANGE_STATE_EFFECT:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pRangeArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pProbArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pTargetEffectOutputID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

#ifdef _GAMESERVER
				_ASSERT( pRangeArg && pProbArg && pTargetEffectOutputID && pDurationTime );
				if( pRangeArg && pProbArg && pTargetEffectOutputID && pDurationTime )
				{
					float fRange = (float)atof( pRangeArg );
					float fProb = (float)atof( pProbArg );
					int iTargetOutputID = atoi( pTargetEffectOutputID );
					int iDurationTime = atoi( pDurationTime );

					pNewProcessor = new CDnRangeSEProcessor( hActor, fRange, fProb, iTargetOutputID, iDurationTime );
				}
				else
					g_Log.Log( LogType::_ERROR, L"IDnSkillProcessor* IDnSkillProcessor::Create() - RANGE_STATE_EFFECT Processor Create Fail!!\n" );
#endif // #ifdef _GAMESERVER
			}
			break;

		// ApplySEWhenActionSetEnabled
		// 겜 서버에서만 사용함. 인자 없음.
		case APPLY_SE_WHEN_ACTIONSET_ENABLED:
#ifdef _GAMESERVER
			pNewProcessor = new CDnApplySEWhenActionSetBlowEnabledProcessor( hActor );
#endif // #ifdef _GAMESERVER
			break;

		// ApplySEWhenTargetNormalHit
		// 겜 서버에서만 사용. 인자는 확률 하나.
		case APPLY_SE_WHEN_TARGET_NORMAL_HIT:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pProbArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

#ifdef _GAMESERVER
				float fProb = (float)atof( pProbArg );
				pNewProcessor = new CDnApplySEWhenTargetNormalHitProcessor( hActor, fProb );
#endif // #ifdef _GAMESERVER
			}
			break;

		// CallMySummonedMonster
		// 겜 서버에서만 사용. 인자는 내 옆으로 가져다 놓을 몬스터 IDs
		case CALL_MY_SUMMONED_MONSTER:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pProbArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);

#ifdef _GAMESERVER
				int iSummonedMonsterIDToCall = (int)atoi( pProbArg );
				pNewProcessor = new CDnCallMySummonedMonsterProcessor( hActor, iSummonedMonsterIDToCall );
#endif // #ifdef _GAMESERVER
			}
			break;

			// ChangeActionStrByBubble
		case CHANGE_ACTIONSTR_BY_BUBBLE:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);				

				// 버블ID;버블갯수;버블ID;액션이름;버블갯수;액션이름.... 3개씩 쌍으로.
				pNewProcessor = new CDnChangeActionStrByBubbleProcessor( hActor, pArg );
			}
			break;

		case STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);				

				// 버블ID;버블갯수;[적용할상태효과의 인덱스1][적용할상태효과의 인덱스2];[적용할상태효과의 인덱스1][적용할상태효과의 인덱스2].... 3개씩 쌍으로.
#ifdef _GAMESERVER
				pNewProcessor = new CDnStateEffectApplyOnOffByBubbleProcessor( hActor, pArg );
#endif // #ifdef _GAMESERVER
			}
			break;
		
		case PLAY_PET_ANI:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pActionName = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				_ASSERT( pActionName );
				vlUseActions.insert( pActionName );
				++(*pIParamOffset);

				if( pActionName )
					pNewProcessor = new CDnPetPlayAniProcess( hActor, pActionName );
			}
			break;

		case CHECK_KEY:
		{
			sprintf_s(caBuf, "_ProcessParam%d", *pIParamOffset + 1);
			char* pActionName = pSkillLevelTable->GetFieldFromLablePtr(iSkillLevelTableID, caBuf)->GetString();
			_ASSERT(pActionName);
			vlUseActions.insert(pActionName);
			++(*pIParamOffset);

			if (pActionName)
				pNewProcessor = new CDnCheckKeyProcess(hActor, pActionName);
		}
		break;
	}

	return pNewProcessor;
}