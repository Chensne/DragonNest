#include "StdAfx.h"
#include "DnBuffProp.h"
#include "DnPropStateScanActor.h"
#include "DnPropActionCondition.h"
#include "SMScanActor.h"
#include "boost/shared_ptr.hpp"


CDnBuffProp::CDnBuffProp( CMultiRoom* pRoom ) : CDnWorldActProp( pRoom ),
												CDnDamageBase( DamageObjectTypeEnum::Prop )
{
}

CDnBuffProp::~CDnBuffProp(void)
{
	ReleasePostCustomParam();
}


bool CDnBuffProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) return false;
	if( m_hMonster ) *m_hMonster->GetMatEx() = *GetMatEx();
	return true;
}

bool CDnBuffProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	bool bResult = false;

	// TODO: BuffProp 타입 맵툴에 추가해야함.
	if( GetData() )
	{
		BuffStruct* pStruct = (BuffStruct*)(GetData());
		int nSkillTableID = pStruct->nSkillTableID;
		int nSkillLevel = pStruct->nSkillLevel;
		float fCheckRange = pStruct->fCheckRange;			// 액터들 체크 범위
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );

		// 잘못된 스킬 정보
		_ASSERT( nSkillTableID != 0 && nSkillLevel != 0 );
		if( nSkillTableID == 0 || nSkillLevel == 0 )
			return false;

		set<int>* pSetTargetActorIDs = NULL;
		set<int> setTargetActorIDs;
		if( bResult )
		{
			// 대상 액터가 지정되어있다면 값 셋팅.
			if( 3 == pStruct->nTeam )
			{
				// 대상 액터가 지정되어있을 때만 포인터 셋팅해준다.
				pSetTargetActorIDs = &setTargetActorIDs;

				string strTargetActorIDs = pStruct->TargetActorID;
				if( strTargetActorIDs.at( strTargetActorIDs.length()-1 ) != ';' )
					strTargetActorIDs.push_back( ';' );

				int iFoundPos = -1;
				while( true )
				{
					int iStartPos = iFoundPos + 1;
					iFoundPos = static_cast<int>(strTargetActorIDs.find_first_of( ';', iStartPos ));
					if( string::npos != iFoundPos )
					{
						string strTargetID = strTargetActorIDs.substr( iStartPos, iFoundPos -iStartPos );
						int iTargetID = atoi( strTargetID.c_str() );
						setTargetActorIDs.insert( iTargetID );
					}
					else
						break;
				}
			}

			m_BuffPropComponent.Initialize( GetRoom(), pStruct->nTeam, nSkillTableID, nSkillLevel, pSetTargetActorIDs );

			// state 들 초기화.
			// Idle 액션에서는 아무것도 하지 않고, Activate 액션이 실행되면 주변을 검색해서 걸리는 놈들에게 스킬에 붙어있는 상태효과 부여.
			CDnPropState* pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
			CDnPropState* pScanState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::SCAN_ACTOR );

			CDnPropCondition* pNormalToScan = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );
			CDnPropCondition* pScanToNormal = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );

			static_cast<CDnPropStateScanActor*>(pScanState)->SetRange( fCheckRange );

			static_cast<CDnPropActionCondition*>(pNormalToScan)->SetActionName( "Activate" );
			static_cast<CDnPropActionCondition*>(pScanToNormal)->SetActionName( "Idle" );

			m_pFSM->AddCondition( pNormalToScan );
			m_pFSM->AddCondition( pScanToNormal );

			m_pFSM->AddState( pNormalState );
			m_pFSM->AddState( pScanState );

			pNormalState->AddTransitState( pScanState, pNormalToScan );
			pScanState->AddTransitState( pNormalState, pScanToNormal );

			m_pFSM->SetEntryState( pNormalState );
		}
	}

	return bResult;
}


void CDnBuffProp::OnMessage( const boost::shared_ptr<IStateMessage>& pMessage )
{
	m_BuffPropComponent.OnMessage( pMessage );
}

void CDnBuffProp::OnChangeAction( const char *szPrevAction )
{
	if( (0 == strcmp( szPrevAction, "Activate" ) && 0 == strcmp( GetCurrentAction(), "Idle" )) ||
		(0 == strcmp( szPrevAction, "Off" ) && 0 == strcmp(GetCurrentAction(), "Idle")) )	// off 액션 후에 idle 로 넘어가는 프랍도 있음.
	{
		// 적용되던 효과 삭제.
		m_BuffPropComponent.RemoveAffectedStateEffects();
	}
}