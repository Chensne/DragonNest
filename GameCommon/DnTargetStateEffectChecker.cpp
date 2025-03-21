#include "StdAfx.h"
#include "DnTargetStateEffectChecker.h"
#include "DnActor.h"
#include "DnBlow.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CDnTargetStateEffectChecker::CDnTargetStateEffectChecker( DnActorHandle hActor, STATE_BLOW::emBLOW_INDEX targetStateIndex, float fRadiusDistToSearch, bool bCheckSkillUser ) : IDnSkillUsableChecker( hActor ), 
m_targetStateIndex( targetStateIndex ),
m_fRadiusDistToSearch( fRadiusDistToSearch ),
m_bCheckSkillUser(bCheckSkillUser)
{
	m_iType = TARGET_STATEEFECT_CHECKER;
}

CDnTargetStateEffectChecker::~CDnTargetStateEffectChecker(void)
{

}



bool CDnTargetStateEffectChecker::CanUse( void )
{
	int nCheckCount = 0;

	if( m_hHasActor && m_targetStateIndex != STATE_BLOW::BLOW_NONE)
	{
		DNVector(DnActorHandle) vlhTargetActors;
		CDnActor::ScanActorByActorSize( ARG_STATIC_FUNC(m_hHasActor) *m_hHasActor->GetPosition(), m_hHasActor->GetUnitSize() + m_fRadiusDistToSearch, vlhTargetActors, false, true );
		EtVector3 vDistance;
		EtVector3 vDir;

		int iNumTargetActors = (int)vlhTargetActors.size();
		for( int i = 0; i < iNumTargetActors; ++i )
		{
			DnActorHandle hTargetActor = vlhTargetActors.at( i );

			if( hTargetActor == m_hHasActor->GetMySmartPtr() )
				continue;

			if( hTargetActor->IsDie() )
				continue;

			//타겟 액터가 설정된 상태효과가 적용 되어 있는지 확인
			if (hTargetActor->IsAppliedThisStateBlow(m_targetStateIndex) == true)
			{
				//스킬유저 체크 인경우
				if (m_bCheckSkillUser == true)
				{
					DNVector( DnBlowHandle ) vlhBlows;
					hTargetActor->GatherAppliedStateBlowByBlowIndex(m_targetStateIndex, vlhBlows);

					int nBlowCount = (int)vlhBlows.size();
					for (int i = 0; i < nBlowCount; ++i)
					{
						DnBlowHandle hBlow = vlhBlows[i];
						if (!hBlow || hBlow->IsEnd())
							continue;

						CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
						if (pSkillInfo && pSkillInfo->hSkillUser == m_hHasActor)
							nCheckCount++;
					}
				}
				//스킬유저 체크 아닌경우
				else
					nCheckCount++;
			}
		}
	}

	return (nCheckCount > 0);
}

IDnSkillUsableChecker* CDnTargetStateEffectChecker::Clone()
{
	CDnTargetStateEffectChecker* pNewChecker = new CDnTargetStateEffectChecker(m_hHasActor, m_targetStateIndex, m_fRadiusDistToSearch, m_bCheckSkillUser);

	return pNewChecker;
}

