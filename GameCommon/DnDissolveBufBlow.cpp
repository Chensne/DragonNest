#include "StdAfx.h"
#include "DnDissolveBufBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDissolveBufBlow::CDnDissolveBufBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_083;
	SetValue( szValue );
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);
}

CDnDissolveBufBlow::~CDnDissolveBufBlow(void)
{
}


void CDnDissolveBufBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 해제 단위는 상태효과 갯수가 아니라 디버프 스킬 묶음 단위.
	multimap<int, DnBlowHandle> mmapBuffBlowsBySkill;

	int iDissolveCount = 0;
	int iNumAppliedBlow = m_hActor->GetNumAppliedStateBlow();
	vector<int> vlBuffSkillID;
	for( int iBlow = 0; iBlow < iNumAppliedBlow; ++iBlow )
	{
		DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( iBlow );
		if( !hBlow )
			continue;

		const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();

		// 임의로 넣어주는 무한 상태효과는 해제 대상이 아님.
		if( hBlow->IsPermanent() )
			continue;

		if( pSkillInfo && pSkillInfo->iSkillID )
		{
			// 좋은 상태효과. 버프로 지정된 스킬들을 해제한다.
			// 단, 패시브 버프로 되어있는 것들은 제외.
			if( (0 == pSkillInfo->iDissolvable) && 
				(CDnSkill::Passive != pSkillInfo->eSkillType ) &&
				(CDnSkill::Buff == pSkillInfo->eDurationType) )
			{
				// 근원 아이템에서 사용하는 특정 스킬 회피 상태효과는 버프 제거 상태효과로 지워지지 않는다.
				// 추후에 일반적으로 쓰일 경우 따로 처리해주어야 함.
				if( hBlow->IsFromSourceItem() )
					continue;

				if( hBlow->IsEternity() )
					continue;

				mmapBuffBlowsBySkill.insert( make_pair(pSkillInfo->iSkillID, hBlow) );

				// 하나의 스킬에 여러개의 상태효과가 있을 수 있다. 유니크한 스킬 아이디만 넣어준다.
				if( vlBuffSkillID.end() == find( vlBuffSkillID.begin(), vlBuffSkillID.end(), pSkillInfo->iSkillID ) )
					vlBuffSkillID.push_back( pSkillInfo->iSkillID );
			}
		}
	}

	DNVector(int) vlAppliedBuffBlowIDs;
	for( int i = 0; i < (int)vlBuffSkillID.size(); ++i )
	{
		if( iDissolveCount >= (int)m_fValue )
			break;

		pair<multimap<int, DnBlowHandle>::iterator, multimap<int, DnBlowHandle>::iterator> iter_pair = 
			mmapBuffBlowsBySkill.equal_range( vlBuffSkillID.at(i) );

		// 같은 스킬에서 비롯된 디버프 상태효과들을 모아둔다.
		multimap<int, DnBlowHandle>::iterator iter = iter_pair.first;
		for( iter; iter != iter_pair.second; ++iter )
			vlAppliedBuffBlowIDs.push_back( iter->second->GetBlowID() );

		++iDissolveCount;
	}


	// 해제 가능한 패시브 스킬이 아닌 버프 상태효과만 해제
	int iNumBlow = (int)vlAppliedBuffBlowIDs.size();
	for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
	{
		int iBlowID = vlAppliedBuffBlowIDs.at( iBlow );
		m_hActor->RemoveStateBlowFromID( iBlowID );

		//// Note: 정의된 상태효과 인덱스로 제거하므로 같은 타입이 1개 이상일 수 있다.
		//// 따라서 hBlow 가 이전 루프에서 이미 삭제되었을 수도 있으므로 유효성 검증을 한다.
		//if( hBlow )
		//{
		//	m_hActor->RemoveStateBlowByBlowDefineIndex( hBlow->GetBlowIndex() );
		//}
	}

	OutputDebug( "CDnDissolveBufBlow::OnBegin, Value:%d \n", (int)m_fValue );
}


void CDnDissolveBufBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnDissolveBufBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDissolveBufBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnDissolveBufBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
