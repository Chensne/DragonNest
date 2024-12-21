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

	// ���ڸ� ���������� �о���̹Ƿ� _GAMSERVER ���������� ������ �͵�� �ƴ� ����Ÿ���� ���� ���� ��� ������ �� �� ����.
	switch( iProcessorIndex-1 )
	{
		//// none 
		//case 0:
		//	
		//	break;

		// PlayAni
		case PLAY_ANI:
			{
				// ������ �Ķ���͸� �̾�����.
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

		// Impact Blow, ���� ����
		case IMPACT_BLOW:
			{
				pNewProcessor = new CDnImpactBlowProcessor( hActor );
			}
			break;

		// Aura, ���ڴ� "����"�� 1�� �޴´�.
		// ���Ӽ��������� �����ؼ� ����
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
		// �� ���������� �����. ���ڴ� ����, Ȯ��, ��󿡰� ������ ����ȿ�� ����Ʈ ���̺� �ε���, processor ���ӽð�
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
		// �� ���������� �����. ���� ����.
		case APPLY_SE_WHEN_ACTIONSET_ENABLED:
#ifdef _GAMESERVER
			pNewProcessor = new CDnApplySEWhenActionSetBlowEnabledProcessor( hActor );
#endif // #ifdef _GAMESERVER
			break;

		// ApplySEWhenTargetNormalHit
		// �� ���������� ���. ���ڴ� Ȯ�� �ϳ�.
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
		// �� ���������� ���. ���ڴ� �� ������ ������ ���� ���� IDs
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

				// ����ID;������;����ID;�׼��̸�;������;�׼��̸�.... 3���� ������.
				pNewProcessor = new CDnChangeActionStrByBubbleProcessor( hActor, pArg );
			}
			break;

		case STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE:
			{
				sprintf_s( caBuf, "_ProcessParam%d", *pIParamOffset+1 );
				char* pArg = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caBuf )->GetString();
				++(*pIParamOffset);				

				// ����ID;������;[�����һ���ȿ���� �ε���1][�����һ���ȿ���� �ε���2];[�����һ���ȿ���� �ε���1][�����һ���ȿ���� �ε���2].... 3���� ������.
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