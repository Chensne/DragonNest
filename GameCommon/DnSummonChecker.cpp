#include "StdAfx.h"
#include "DnSummonChecker.h"
#include "DnPlayerActor.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CDnSummonChecker::CDnSummonChecker( DnActorHandle hActor , const char* pArg, int nRange ) : IDnSkillUsableChecker( hActor )
{
	m_iType = SUMMON_CHECKER;
	m_iRange = nRange;

	if( pArg )
	{
		m_strValue = pArg;

		vector<string> vlTokens;
		TokenizeA( pArg, vlTokens, ";" );
		for( int i = 0; i < (int)vlTokens.size(); ++i )
		{
			m_vlCheckActorIDs.push_back( atoi(vlTokens.at(i).c_str()) );
		}
	}
}

CDnSummonChecker::~CDnSummonChecker(void)
{

}


bool CDnSummonChecker::CanUse( void )
{
	bool bResult = false;

#ifdef _GAMESERVER
	if( m_hHasActor && m_hHasActor->IsPlayerActor() )
	{
		const list<DnMonsterActorHandle>& listSummonMonster = m_hHasActor->GetSummonedMonsterList();

		if( false == listSummonMonster.empty() )
		{
			list<DnMonsterActorHandle>::const_iterator iter = listSummonMonster.begin();
			for( iter; iter != listSummonMonster.end(); ++iter )
			{
				DnMonsterActorHandle hMonster = (*iter);
				if( hMonster )
				{
					// 복수개의 액터 id 가 셋팅된 경우 하나라도 있으면 인정.
					DNVector(int)::iterator iter = find( m_vlCheckActorIDs.begin(), m_vlCheckActorIDs.end(), hMonster->GetClassID() );
					if( m_vlCheckActorIDs.end() != iter )
					{
						EtVector2 v1,v2;
						v1.x = m_hHasActor->GetPosition()->x;
						v1.y = m_hHasActor->GetPosition()->z;
						v2.x = hMonster->GetPosition()->x;
						v2.y =  hMonster->GetPosition()->z;
						EtVector2 v = v1 - v2;
						float fDist = EtVec2Length(&v);

						if(fDist < m_iRange)
						{
							bResult = true;
							break;
						}
					}
				}
			}
		}	

		const map<int, list<DnMonsterActorHandle> >& mapSummonMonsterByGroup = m_hHasActor->GetGroupingSummonedMonsterList();
		if( false == mapSummonMonsterByGroup.empty() )
		{
			map<int, list<DnMonsterActorHandle> >::const_iterator iterMap = mapSummonMonsterByGroup.begin();
			for( iterMap; iterMap != mapSummonMonsterByGroup.end(); ++iterMap )
			{
				const list<DnMonsterActorHandle>& listSummonMonster = iterMap->second;
				list<DnMonsterActorHandle>::const_iterator iter = listSummonMonster.begin();
				for( iter; iter != listSummonMonster.end(); ++iter )
				{
					DnMonsterActorHandle hMonster = (*iter);
					if( hMonster )
					{
						// 복수개의 액터 id 가 셋팅된 경우 하나라도 있으면 인정.
						DNVector(int)::iterator iter = find( m_vlCheckActorIDs.begin(), m_vlCheckActorIDs.end(), hMonster->GetClassID() );
						if( m_vlCheckActorIDs.end() != iter )
						{
							EtVector2 v1,v2;
							v1.x = m_hHasActor->GetPosition()->x;
							v1.y = m_hHasActor->GetPosition()->z;
							v2.x = hMonster->GetPosition()->x;
							v2.y =  hMonster->GetPosition()->z;
							EtVector2 v = v1 - v2;
							float fDist = EtVec2Length(&v);

							if(fDist < m_iRange)
							{
								bResult = true;
								break;
							}
						}
					}
				}
			}
		}

	}
#else

	if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() ) // 마을이면 그냥 사용 못함. 클라에서만 사용
		return false;

	DNVector(DnActorHandle) vlActors;
	CDnActor::ScanActor( (*m_hHasActor->GetPosition()), (float)m_iRange, vlActors );

	int iNumActors = (int)vlActors.size();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		DnActorHandle hTargetActor = vlActors.at( iActor );

		if( hTargetActor 
			&&  hTargetActor->IsMonsterActor() 
			&& (hTargetActor->GetTeam() == m_hHasActor->GetTeam()) 
			&& (false == hTargetActor->IsDie()) )
		{
			// 복수개의 액터 id 가 셋팅된 경우 하나라도 있으면 인정.
			DNVector(int)::iterator iter = find( m_vlCheckActorIDs.begin(), m_vlCheckActorIDs.end(), hTargetActor->GetClassID() );
			if( m_vlCheckActorIDs.end() != iter )
			{
				if( hTargetActor->GetSummonerUniqueID() == m_hHasActor->GetUniqueID() )
				{
					bResult = true;
					break;
				}
			}
		}
	}
#endif

	return bResult;
}

IDnSkillUsableChecker* CDnSummonChecker::Clone()
{
	CDnSummonChecker* pNewChecker = new CDnSummonChecker(m_hHasActor, m_strValue.c_str(), m_iRange);

	return pNewChecker;
}
