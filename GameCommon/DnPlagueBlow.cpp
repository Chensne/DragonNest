#include "StdAfx.h"
#include "DnPlagueBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const LOCAL_TIME PLAGUE_DAMAGE_INTERVAL = 2000;


CDnPlagueBlow::CDnPlagueBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																			m_IntervalChecker( hActor, GetMySmartPtr() ),
																			m_fDamageProb( 0.f ),
																			m_nPlageProbability( 0 ),
																			m_fDamageAbsolute( 0.f ),
																			m_fPlagueRange( 0.f ),
																			m_fEffectLength( 0.f ),
																			m_bEffecting( false )
#ifdef _GAMESERVER
																			,m_fIntervalDamage( 0.0f )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_136;
	SetValue( szValue );

	std::vector<std::string> tokens;
	TokenizeA( szValue, tokens , ";" );

	if( tokens.size() == 2 )
	{
		m_nPlageProbability = 100;
		m_fDamageProb = (float)atof( tokens[0].c_str() );
		m_fPlagueRange = (float)atof( tokens[1].c_str() );
	}
	else if( tokens.size() == 4 )
	{
		m_nPlageProbability = (float)atof( tokens[0].c_str() ) * 100.f;
		m_fDamageProb = (float)atof( tokens[1].c_str() );
		m_fDamageAbsolute = (float)atof( tokens[2].c_str() );
		m_fPlagueRange = (float)atof( tokens[3].c_str() );
	}

#ifdef _GAMESERVER
	// 전염되어온 경우 데미지 비율이 1.0 을 넘을 것이다. 이 경우엔 그대로 데미지 적용해주면 된다.
	if( m_fDamageProb < 1.0f )
		m_fIntervalDamage = (m_hActor->GetLastDamage() * m_fDamageProb) + m_fDamageAbsolute;
	else
		m_fIntervalDamage = m_fDamageProb;
#endif

#ifndef _GAMESERVER
	UseTableDefinedGraphicEffect( false );
#endif

}

CDnPlagueBlow::~CDnPlagueBlow(void)
{

}


#ifdef _GAMESERVER
bool CDnPlagueBlow::CanBegin( void )
{
	return true;
}
#endif



bool CDnPlagueBlow::OnCustomIntervalProcess( void )
{
#ifdef _GAMESERVER
	if( 0.0f < m_fIntervalDamage )
	{
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);

		if (hActor && 
			hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);
		}
#else
		if (m_hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);
		}
#endif // PRE_FIX_61382
	}

	// 주변에 옮길만한 대상이 있는지 아군중에서 찾는다.
	// 옮기게 되면 같은 데미지로 처리되도록 한다.
	DNVector( DnActorHandle ) vlActorsInRange;
	CDnActor::ScanActor( m_hActor->GetRoom(), *m_hActor->GetPosition(), m_fPlagueRange, vlActorsInRange );

	// 가장 가까운 대상에게 옮김. 이미 전염병 상태효과가 걸린 대상에게는 옮기지 않는다.
	float fShortestLengthSQ = FLT_MAX;
	int iNumActorsInRange = (int)vlActorsInRange.size();
	DnActorHandle hResultTarget;
	for( int i = 0; i < iNumActorsInRange; ++i )
	{
		DnActorHandle hActor = vlActorsInRange.at( i );
		// 아군에게만.
		if( hActor->GetTeam() == m_hActor->GetTeam() )
		{
			// 이미 전염된 상태인 애들 빼고.
			if( false == hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_136 ) )
			{
				int iRandVal = _rand(GetRoom()) % 100;
				if( iRandVal < m_nPlageProbability )
				{
					EtVector3 vLength = *hActor->GetPosition() - *m_hActor->GetPosition();
					float fLengthSQ = EtVec3LengthSq( &vLength );
					if( fLengthSQ < fShortestLengthSQ )
					{
						fShortestLengthSQ = fLengthSQ;
						hResultTarget = hActor;
						break;
					}
				}
			}
		}
	}

	if( hResultTarget )
	{
		// 결과로 나온 액터가 있다면 전염병 상태효과 추가.
		// 공격 당해서 걸리는 게 아니라 전염되는 것이므로 결정된 데미지를 그대로 전달한다.
		char acBuf[ STATE_BLOW_ARGUMENT_MAX_SIZE ] = { 0 };
		sprintf_s( acBuf, sizeof(acBuf), "%2.2f;%2.2f", m_fIntervalDamage, m_fPlagueRange );
		hResultTarget->CmdAddStateEffect( &m_ParentSkillInfo, m_StateBlow.emBlowIndex, 
			int(m_fOriginalDurationTime*1000.0f), acBuf );
	}
#else
	_AttachGraphicEffect();
	if( m_hEtcObjectEffect )
	{	
		CEtActionBase::ActionElementStruct* pStruct = m_hEtcObjectEffect->GetElement( "Idle" );
		if( pStruct )
		{
			m_fEffectLength = (float)pStruct->dwLength / 1000.0f;
		}

		_SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
		m_bEffecting = true;
	}
#endif

	return true;
}


void CDnPlagueBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	if( m_fIntervalDamage < 1.0f )
	{
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnPlagueBlow::OnBegin - Damage is too small. ( < 1) Plague Fail\n" );
	}
#endif

	m_IntervalChecker.OnBegin( LocalTime, PLAGUE_DAMAGE_INTERVAL );

	m_fEffectLength = 0.0f;
	m_bEffecting = false;

	m_fOriginalDurationTime  = m_StateBlow.fDurationTime;

}


void CDnPlagueBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifndef _GAMESERVER
	if( m_bEffecting )
	{
		m_fEffectLength -= fDelta;
		if( m_fEffectLength < 0.0f )
		{
			m_fEffectLength = 0.0f;
			_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );

			m_bEffecting = false;
		}
	}
#endif

	// 내부에서 Die 처리가 되기 때문에 본인이 Die 되는 경우도 있으므로 본인의 이 상태효과가 무효화 될 수 있다.
	// 따라서 맨 마지막에 두도록한다.
	m_IntervalChecker.Process( LocalTime, fDelta );

}


void CDnPlagueBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// LocalTime 이 0으로 호출되면 삭제, 중첩처리 등을 위한 강제 종료임.
	if( LocalTime != 0 )
		m_IntervalChecker.OnEnd( LocalTime, fDelta );

#ifndef _GAMESERVER
	_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	_DetachGraphicEffect();
#endif

}


#if defined(_GAMESERVER)
void CDnPlagueBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// TODO: 중첩되었을 때 어떻게 할까?
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPlagueBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	std::string strArgument[2];
	strArgument[0] = szOrigValue;
	strArgument[1] = szAddValue;

	std::vector<string> vlOriginTokens, vlNewTokens;
	TokenizeA( strArgument[0], vlOriginTokens, ";" );
	TokenizeA( strArgument[1], vlNewTokens, ";" );

	if( vlOriginTokens.size() == vlNewTokens.size() ) 
	{
		std::string strResult;

		for( DWORD i=0; i<vlOriginTokens.size(); i++ )
		{
			strResult += FormatA( "%f" , ( (float)atof( vlOriginTokens[i].c_str() ) + (float)atof( vlNewTokens[i].c_str() ) ) );
			if( i < vlOriginTokens.size()-1 )
			{
				strResult += ';';
			}
		}

		szNewValue = strResult.c_str();
	}
}

void CDnPlagueBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	std::string strArgument[2];
	strArgument[0] = szOrigValue;
	strArgument[1] = szAddValue;

	std::vector<string> vlOriginTokens, vlNewTokens;
	TokenizeA( strArgument[0], vlOriginTokens, ";" );
	TokenizeA( strArgument[1], vlNewTokens, ";" );

	if( vlOriginTokens.size() == vlNewTokens.size() ) 
	{
		std::string strResult;

		for( DWORD i=0; i<vlOriginTokens.size(); i++ )
		{
			strResult += FormatA( "%f" , ( (float)atof( vlOriginTokens[i].c_str() ) - (float)atof( vlNewTokens[i].c_str() ) ) );
			if( i < vlOriginTokens.size()-1 )
			{
				strResult += ';';
			}
		}

		szNewValue = strResult.c_str();
	}
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW