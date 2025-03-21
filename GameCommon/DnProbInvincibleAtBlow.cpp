#include "StdAfx.h"
#include "DnProbInvincibleAtBlow.h"
#include "DnProjectile.h"

#ifdef _GAMESERVER
#include "DnMonsterActor.h"
#include "DnProjectile.h"
#else
#include "DnStateBlow.h"
#endif // #ifdef _GAMESERVER

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnProbInvincibleAtBlow::CDnProbInvincibleAtBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_226;
	SetValue( szValue );

#ifdef _GAMESERVER
	AddCallBackType( SB_ONCALCDAMAGE );
	AddInvincibleAt(szValue);
	m_pNowMapSelectedSkillInfos = NULL;
	m_bNowSelectedSkillUsedProjectile = false;
#else
	m_pEffectOutputInfo = CDnStateBlow::GetEffectOutputInfo( 100 );		// StateEffectTable 에 근원 아이템의 상태효과는 100 으로 지정됨.
#endif // #ifdef _GAMESERVER

	m_fValue = 0.0f;
}

CDnProbInvincibleAtBlow::~CDnProbInvincibleAtBlow(void)
{
}

void CDnProbInvincibleAtBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnProbInvincibleAtBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	CheckAllTargetSkillState();
	CheckProbFailedSkillState();
#endif // #ifdef _GAMESERVER
}


void CDnProbInvincibleAtBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "%s\n", __FUNCTION__);
}

#ifdef _GAMESERVER
void CDnProbInvincibleAtBlow::AddInvincibleAt(const char* szValue)
{	
	string str = szValue;
	vector<std::string> vlTokens;
	string delimiters = ";";

	// 회피대상 몬스터 ID;[회피 대상 스킬 ID1][회피 대상 스킬 확률1][회피 대상 스킬 ID2][회피 대상 스킬 확률2];...
	TokenizeA( str, vlTokens, delimiters );
	_ASSERT( vlTokens.size()%2 == 0 );
	for( int i = 0; i < (int)vlTokens.size(); i += 2 )
	{
		int iMonsterID = atoi( vlTokens.at(i).c_str() );
		S_INVINCIBLE_INFO& Info = m_mapInvincibleAt[ iMonsterID ];		// 몬스터 ID 를 중복으로 써도 되게 해달라는 요청으로..
		Info.iMonsterID = iMonsterID;
		vector<string> vlSkillInfos;
		TokenizeA( vlTokens.at(i+1), vlSkillInfos, string("["), string("]"), false );
		for( int k = 0; k < (int)vlSkillInfos.size(); k += 2 )
		{
			int iSkillID = atoi( vlSkillInfos.at( k ).c_str() );
			float fProb = (float)atof( vlSkillInfos.at( k+1 ).c_str() ) * 100.0f;
			Info.mapSkillInfos.insert( make_pair(iSkillID, fProb) );
		}
	}
}

void CDnProbInvincibleAtBlow::BeginInvincibleAt( DnSkillHandle hTargetSkill )
{
	if( !hTargetSkill )
		return;

	int iSkillID = hTargetSkill->GetClassID();
	if( false == IsBeginInvincible(iSkillID) )
	{
		int iSkillID = hTargetSkill->GetClassID();
		if( 0 < m_pNowMapSelectedSkillInfos->count( iSkillID ) )
		{
			// 확률 체크.
			int iProb = int((*m_pNowMapSelectedSkillInfos)[ iSkillID ] * 100.0f);
			if( _rand(m_hActor->GetRoom()) % 10000 <= iProb )
			{
				// 이 스킬 대상으로 무시 시작.
				//m_hTargetSkill = hTargetSkill;
				S_SKILL_INFO TargetSkillInfo;
				TargetSkillInfo.hTargetSkill = hTargetSkill;
				TargetSkillInfo.bTargetSkillUsedProjectile = m_bNowSelectedSkillUsedProjectile;
				m_mapTargetSkillUsedProjectile[ iSkillID ] = m_bNowSelectedSkillUsedProjectile;
				m_listTargetSkills.push_back( TargetSkillInfo );
			}
			else
			{
				// 확률 체크에 실패하면 해당 스킬이 종료되기 전까진 다시 확률 체크하지 않는다.
				m_mapTargetSkillUsedProjectile[ iSkillID ] = m_bNowSelectedSkillUsedProjectile;
				m_dqProbInvincibleFailedSkills.push_back( hTargetSkill );
			}
		}
	}
}

void CDnProbInvincibleAtBlow::EndInvincibleAt( DnSkillHandle hTargetSkill )
{
	if( !hTargetSkill )
		return;

	for( list<S_SKILL_INFO>::iterator iter = m_listTargetSkills.begin(); 
		m_listTargetSkills.end() != iter; )
	{
		S_SKILL_INFO& TargetSkillInfo = (*iter);	
		if( (!TargetSkillInfo.hTargetSkill) || 
			(TargetSkillInfo.hTargetSkill == hTargetSkill) )
		{
			m_mapTargetSkillUsedProjectile.erase( hTargetSkill->GetClassID() );
			iter = m_listTargetSkills.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

bool CDnProbInvincibleAtBlow::IsBeginInvincible( int iSkillID )
{
	bool bResult = false;

	bResult = (FindInvincibleTargetSkill(iSkillID) != NULL);

	return bResult;
}

CDnProbInvincibleAtBlow::S_SKILL_INFO* CDnProbInvincibleAtBlow::FindInvincibleTargetSkill( int iSkillID )
{
	S_SKILL_INFO* pResult = NULL;

	for( list<S_SKILL_INFO>::iterator iter = m_listTargetSkills.begin(); 
		m_listTargetSkills.end() != iter; )
	{
		S_SKILL_INFO& TargetSkillInfo = (*iter);
		if( TargetSkillInfo.hTargetSkill && 
			TargetSkillInfo.hTargetSkill->GetClassID() == iSkillID )
		{
			pResult = &TargetSkillInfo;
			break;
		}
		else
		if( !TargetSkillInfo.hTargetSkill )
		{
			m_mapTargetSkillUsedProjectile.erase( iSkillID );
			iter = m_listTargetSkills.erase( iter );
			continue;
		}

			++iter;
	}

	return pResult;
}

bool CDnProbInvincibleAtBlow::IsInvincibleAt( DnSkillHandle hTargetSkill )
{
	if( NULL == m_pNowMapSelectedSkillInfos )
		return false;

	if( !hTargetSkill )
		return false;

	bool bResult = false;
	int iSkillID = hTargetSkill->GetClassID();

	if( false == IsProbFailedSkillObject( hTargetSkill ) )
	{
		CheckTargetSkillStateAndDoEnd( hTargetSkill );

		if( false == IsBeginInvincible( iSkillID ) )
			BeginInvincibleAt( hTargetSkill );

		S_SKILL_INFO* pResult = FindInvincibleTargetSkill( iSkillID );
		if( pResult )
		{	
			if( pResult->hTargetSkill == hTargetSkill )
				bResult = true;
		}
	}

	return bResult;
}

bool CDnProbInvincibleAtBlow::IsProbFailedSkillObject( DnSkillHandle hSkill )
{
	bool bResult = false;

	// 한번 확률 체크해서 실패한 스킬은 해당 스킬이 종료될 때 까지 무적 여부 확률 체크를 다시 하지 않는다.
	deque<DnSkillHandle>::iterator iter = find( m_dqProbInvincibleFailedSkills.begin(), m_dqProbInvincibleFailedSkills.end(), hSkill );
	if( iter != m_dqProbInvincibleFailedSkills.end() )
	{
		if( IsSkillFinished( hSkill ) )
		{
			m_dqProbInvincibleFailedSkills.erase( iter );
		}
		else
		{
			bResult = true;
		}
	}

	return bResult;
}

void CDnProbInvincibleAtBlow::CheckAllTargetSkillState( void )
{
	vector<DnSkillHandle> vlhEndedTargetSkills;
	GatherEndTargetState( vlhEndedTargetSkills );

	for( int i = 0; i < (int)vlhEndedTargetSkills.size(); ++i )
	{
		EndInvincibleAt( vlhEndedTargetSkills.at( i ) );
	}
}

void CDnProbInvincibleAtBlow::GatherEndTargetState( /*IN OUT*/ vector<DnSkillHandle>& vlhEndedTargetSkill )
{
	for( list<S_SKILL_INFO>::iterator iter = m_listTargetSkills.begin(); m_listTargetSkills.end() != iter; ++iter )
	{
		if( IsSkillFinished(iter->hTargetSkill) )
		{
			vlhEndedTargetSkill.push_back( iter->hTargetSkill );
		}
	}
}

void CDnProbInvincibleAtBlow::CheckTargetSkillStateAndDoEnd( DnSkillHandle hTargetSkill )
{
	if( !hTargetSkill )
		return;

	if( IsSkillFinished( hTargetSkill ) )
	{
		EndInvincibleAt( hTargetSkill );
	}
}

void CDnProbInvincibleAtBlow::CheckProbFailedSkillState( void )
{
	deque<DnSkillHandle>::iterator iter = m_dqProbInvincibleFailedSkills.begin();
	for( iter; iter != m_dqProbInvincibleFailedSkills.end(); )
	{
		if( IsSkillFinished( *iter ) )
		{
			iter = m_dqProbInvincibleFailedSkills.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

bool CDnProbInvincibleAtBlow::IsSkillFinished( DnSkillHandle hSkill )
{
	bool bResult = false;

	if( hSkill &&
		hSkill->GetActor() &&
		hSkill->GetActor()->IsMonsterActor() )
	{
		bool bSkillObjectFinished = hSkill->IsFinished();
		bResult = bSkillObjectFinished;

		bool bSkillProjectileExist = false;

		bool bTargetSkillUsedProjectile = false;
		map<int, bool>::iterator iter = m_mapTargetSkillUsedProjectile.find( hSkill->GetClassID() );
		if( m_mapTargetSkillUsedProjectile.end() != iter )
		{
			bTargetSkillUsedProjectile = iter->second;
		}

		if( bTargetSkillUsedProjectile)
		{
			// 스킬 객체는 끝났더라도 스킬로 쐈던 발사체가 남아있는지 확인... 모두 확인하는 수 밖엔 없음.
			bResult = false;
			for( int i = 0; i < CDnWeapon::GetItemCount( m_hActor->GetRoom() ); ++i )
			{
				DnWeaponHandle hWeapon = CDnWeapon::GetItem( m_hActor->GetRoom(), i );
				if( hWeapon && CDnWeapon::Projectile == hWeapon->GetWeaponType() )
				{
					CDnProjectile* pProjectile = static_cast<CDnProjectile*>(hWeapon.GetPointer());
					DnSkillHandle hSkillFromProjectile = pProjectile->GetParentSkill();
					if( hSkillFromProjectile &&
						hSkillFromProjectile->GetClassID() == hSkill->GetClassID() )
					{
						if( hSkillFromProjectile->GetActor()->IsMonsterActor() )
						{
							CDnMonsterActor* pSkillUserMonsterActor = static_cast<CDnMonsterActor*>(hSkill->GetActor().GetPointer());
							CDnMonsterActor* pNowMonsterActor = static_cast<CDnMonsterActor*>(hSkillFromProjectile->GetActor().GetPointer());
							if( pNowMonsterActor->GetMonsterClassID() == pSkillUserMonsterActor->GetMonsterClassID() )
							{
								bSkillProjectileExist = true;
								break;
							}
						}
					}
				}
			}

			bResult = (bSkillObjectFinished && !bSkillProjectileExist);
		}
	}

	return bResult;
}

bool CDnProbInvincibleAtBlow::CanAddThisSkillsStateBlow( int iSkillID )
{
	bool bResult = true;

	//if( IsBeginInvincible() )
	S_SKILL_INFO* pResult = FindInvincibleTargetSkill( iSkillID );
	if( pResult )
	{
		if( iSkillID == pResult->hTargetSkill->GetClassID() )
			bResult = false;
	}

	return bResult;
}

float CDnProbInvincibleAtBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	float fResult = 0.0f;

	if( HitParam.hHitter || HitParam.hWeapon )
	{
		DnSkillHandle hHitterSkill;
		bool bSkillUseProjectile = false;
		if( HitParam.hWeapon &&  
			CDnWeapon::Projectile == HitParam.hWeapon->GetWeaponType() )
		{
			CDnProjectile* pProjectile = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() );
			hHitterSkill = pProjectile->GetParentSkill();
			bSkillUseProjectile = true;
		}
		else
		if( HitParam.hHitter )
		{
			hHitterSkill = HitParam.hHitter->GetProcessSkill();
		}

		if( hHitterSkill )
		{
			// 몬스터 ID 를 구분해달라고 해서 몬스터인 경우만 적용되도록 한다..
			DnActorHandle hSkillUser = hHitterSkill->GetActor();
			if( hSkillUser && hSkillUser->IsMonsterActor() )
			{
				int iMonsterID = static_cast<CDnMonsterActor*>(hSkillUser.GetPointer())->GetMonsterClassID();
				map<int, S_INVINCIBLE_INFO>::iterator iter = m_mapInvincibleAt.find( iMonsterID );
				if( m_mapInvincibleAt.end() != iter )
				{
					if( bSkillUseProjectile )
					{
						m_bNowSelectedSkillUsedProjectile = true;

						// #46112 처음엔 hit 시그널로 직접 때리다가 추후에 발사체를 깔아놓는 경우도 있기 때문에 현재 발사체로 히트 된 것인지 여부에 따라 업데이트 해준다.
						m_mapTargetSkillUsedProjectile[ hHitterSkill->GetClassID() ] = true;
					}

					m_pNowMapSelectedSkillInfos = &((iter->second).mapSkillInfos);

					bool bInvincible = IsInvincibleAt( hHitterSkill );
					if( bInvincible )
					{
						fResult = -fOriginalDamage;
						m_hActor->SendProbInvincibleSuccess();		// 클라로 데미지 무효화 성공했다고 패킷 보냄.
					}

					m_pNowMapSelectedSkillInfos = NULL;
					m_bNowSelectedSkillUsedProjectile = false;
				}
			}
		}
	}

	return fResult;
}
#else
void CDnProbInvincibleAtBlow::OnSuccess( void )
{
	if( m_hEtcObjectEffect )
	{
		m_hEtcObjectEffect->SetActionQueue( "Activate" );
	}
}
#endif // #ifdef _GAMESERVER


//#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
//void CDnProbInvincibleAtBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
//{
//	char szBuff[128] = {0, };
//
//	szNewValue = szOrigValue;
//	//szOrigValue 마지막에 ";"가 없으면 추가 해서 szAddValue를 추가 한다.
//	int nLength = (int)strlen(szOrigValue);
//
//	if (nLength == 0 || szOrigValue[nLength - 1] == ';')
//		szNewValue += szAddValue;
//	else
//	{
//		szNewValue += ";";
//		szNewValue += szAddValue;
//	}
//}
//
//void CDnProbInvincibleAtBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
//{
//	char szBuff[128] = {0, };
//
//	//"###;###;###;###;....;##;##;##"
//	//|<--이전 문자열----->|<-AddValue->|
//
//	//szOrigValue에서 szAddValue 문자열길이 만큼 뒤에서 자른다.
//	int nOrigLength = (int)strlen(szOrigValue);
//	int nAddLength = (int)strlen(szAddValue);
//	int nCount = nOrigLength - nAddLength;
//
//	sprintf_s(szBuff, "%s", szOrigValue);
//	szBuff[nCount] = 0;
//
//	szNewValue = szBuff;
//}
//#endif // PRE_ADD_PREFIX_SYSTE_RENEW
