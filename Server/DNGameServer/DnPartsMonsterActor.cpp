
#include "StdAfx.h"
#include "DnPartsMonsterActor.h"
#include "MAAiCommand.h"
#include "MAAiScript.h"
#include "DnStateBlow.h"

const int g_nNaviUpdateDelta = 1000;

CDnPartsMonsterActor::CDnPartsMonsterActor( CMultiRoom *pRoom, int nClassID )
: TDnMonsterActor( pRoom, nClassID )
{
	m_nLastUpdateNaviTime = 0;
}

CDnPartsMonsterActor::~CDnPartsMonsterActor()
{
}

MAMovementBase* CDnPartsMonsterActor::CreateMovement()
{
	MAMovementBase* pMovement = new MAWalkMovementNav();
	return pMovement;
}

bool CDnPartsMonsterActor::Initialize()
{
	TDnMonsterActor< MASingleBody, MAMultiDamage >::Initialize();
	LoadMultiDamageInfo();
	
	return true;
}

#if defined( PRE_ADD_LOTUSGOLEM )
void CDnPartsMonsterActor::ProcessPartsAI( const std::vector<AIPartsProcessor>& vData )
{
	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		m_Parts[i].SetIgnore( false );
		m_Parts[i].SetNoDamage( false );
	}

	int iHP		= GetHPPercent();
	bool bPrev  = false;

	for( UINT i=0 ; i<vData.size() ; ++i )
	{
		// HP
		if( iHP < vData[i].iMinHP || iHP > vData[i].iMaxHP )
			continue;

		// CheckAction
		if( vData[i].strCheckActionName.empty() == false )
		{
			if( strcmp( GetCurrentAction(), vData[i].strCheckActionName.c_str() ) != 0 )
				continue;
		}

		// CheckSkill
		if( vData[i].iCheckSkillIndex > 0 )
		{
			if( !GetProcessSkill() )
				continue;

			if( m_hActor->GetProcessSkill()->GetClassID() != vData[i].iCheckSkillIndex )
				continue;
		}

		// CheckBlow
		if( vData[i].iCheckBlowIndex > 0 )
		{
			if( m_hActor->GetStateBlow() && m_hActor->GetStateBlow()->IsApplied( static_cast<STATE_BLOW::emBLOW_INDEX>(vData[i].iCheckBlowIndex) ) == false )
				continue;
		}

		// Ignore
		for( UINT j=0 ; j<vData[i].vIgnorePartsIndex.size() ; ++j )
		{
			MonsterParts* pParts = GetParts( vData[i].vIgnorePartsIndex[j] );
			if( pParts )
				pParts->SetIgnore( true );
		}

		// NoDamage
		for( UINT j=0 ; j<vData[i].vNoDamagePartsIndex.size() ; ++j )
		{
			MonsterParts* pParts = GetParts( vData[i].vNoDamagePartsIndex[j] );
			if( pParts )
				pParts->SetNoDamage( true );
		}

		// Active
		for( UINT j=0 ; j<vData[i].vActivePartsIndex.size() ; ++j )
		{
			MonsterParts* pParts = GetParts( vData[i].vActivePartsIndex[j] );
			if( pParts )
			{
				pParts->SetIgnore( false );
				pParts->SetNoDamage( false );
			}
		}

		bPrev = true;
	}

	// PartsProcessor 가 없다면 패킷 보낼필요 없음
	if( vData.empty() )
		return;

	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		int iPartsTableID	= m_Parts[i].GetPartsTableID();
		int iState			= m_Parts[i].GetPartsInfo().PartsState;;		
		int nIndex = (int)i;

		Stream.Write( &nIndex, sizeof(int) );	// Parts 인덱스 추가 [2011/05/11 semozz]
		Stream.Write( &iPartsTableID, sizeof(int) );
		Stream.Write( &iState, sizeof(int) );

		Send( eActor::SC_MONSTERPARTS_STATE, &Stream );	
	}
}
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

void CDnPartsMonsterActor::_ProcessParts( const float fDelta )
{
	if( m_listDestroyParts.empty() )
		return;

	_ASSERT( m_pAi );
	_ASSERT( m_pAi->IsScript() );

	// 세트파츠 부활
	MAAiScript*			pScript = static_cast<MAAiScript*>(m_pAi);
	const ScriptData&	data	= pScript->GetScriptData();

	for( std::list<std::pair<UINT,float>>::iterator itor=m_listDestroySetParts.begin() ; itor!=m_listDestroySetParts.end() ; )
	{
		(*itor).second -= fDelta;		
		if( (*itor).second > 0.f )
		{
			++itor;
			continue;
		}
		
		UINT uiSetID = (*itor).first;
		_ASSERT( uiSetID < data.m_vSetPartsRebirthTable.size() );

		for( UINT i=0 ; i<data.m_vSetPartsRebirthTable[uiSetID].vPartsTableID.size() ; ++i  )
		{
			for( std::list<MonsterParts*>::iterator itor2=m_listDestroyParts.begin() ; itor2!=m_listDestroyParts.end() ; )
			{
				if( (*itor2)->GetPartsOriginalInfo().uiMonsterPartsTableID == data.m_vSetPartsRebirthTable[uiSetID].vPartsTableID[i] )
				{
					_OnRefreshParts( *itor2 );
					itor2 = m_listDestroyParts.erase( itor2 );
					break;
				}

				++itor2;
			}
		}

		itor = m_listDestroySetParts.erase( itor );
	}

	// 개별파츠 부활
	for( std::list<MonsterParts*>::iterator itor=m_listDestroyParts.begin() ; itor!=m_listDestroyParts.end() ; )
	{
		if( (*itor)->Process( fDelta ) )
		{
			_OnRefreshParts( *itor );
			itor = m_listDestroyParts.erase( itor );
		}
		else
		{
			++itor;
		}
	}
}

void CDnPartsMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	_ProcessParts( fDelta );
	TDnMonsterActor< MASingleBody, MAMultiDamage >::Process( LocalTime, fDelta );
	if( IsNaviMode() && GetNaviType() == MAMovementBase::NaviType::eTarget )
	{
		m_nLastUpdateNaviTime -= static_cast<LOCAL_TIME>(fDelta*1000.0f);
		if( m_nLastUpdateNaviTime < 0 )
			UpdateNaviMode();
	}	
}

void CDnPartsMonsterActor::UpdateNaviMode()
{
	if( !IsNaviMode() )
		return;

	if ( !GetNaviTarget() || GetNaviTarget()->IsDie() )
	{
		ResetNaviMode();
		return;
	}

	CmdMoveNavi(GetNaviTarget(), GetNaviTargetMinDistance(), GetNaviTargetActionName(), -1);
}

void CDnPartsMonsterActor::CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	TDnMonsterActor< MASingleBody, MAMultiDamage >::CmdMoveNavi( hActor, fMinDistance, szActionName, nLoopCount, fBlendFrame );
	if( m_hActor->IsNaviMode() )
		m_nLastUpdateNaviTime = g_nNaviUpdateDelta;
}

void CDnPartsMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct* pStruct )
{
	TDnMonsterActor< MASingleBody, MAMultiDamage >::OnDamage(pHitter, HitParam, pStruct );
}

float CDnPartsMonsterActor::CalcDamage( CDnDamageBase* pHitter, SHitParam& HitParam )
{
	float fDamage		= PreCalcDamage( pHitter, HitParam );
	float fAddDamage	= 0.f;

	bool bBreak = false;

	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
#if defined( PRE_ADD_LOTUSGOLEM )
		// 파츠가 살아있고..ignore 상태가 아니라면..
		if( m_Parts[i].GetHP() > 0 && m_Parts[i].CheckPartsState( MonsterParts::eIgnore ) == false )
#else
		// 파츠가 살아있으면...
		if( m_Parts[i].GetHP() > 0 )
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
		{
			// 본체 데미지 비율 계산
			float fMainDamageRate = m_Parts[i].GetMainDamageRate();
			fAddDamage += (fDamage*(fMainDamageRate-1.f));
		}

		for( UINT j=0 ; j<HitParam.vBoneIndex.size() ; ++j )
		{
			std::string strBoneName = GetBoneName( HitParam.vBoneIndex[j] );

			if( m_Parts[i].HasBone( strBoneName.c_str() ) )
			{
				bool bHitCondition = true;
				for( UINT k=0 ; k<m_Parts[i].GetPartsInfo().vHitCondition.size() ; ++k )
				{
					for( UINT ii=0 ; ii<m_Parts.size() ; ++ii )
					{
						if( m_Parts[i].GetPartsInfo().vHitCondition[k].iRequiredPartsID == m_Parts[ii].GetPartsInfo().uiMonsterPartsTableID )
						{
							if( m_Parts[ii].GetHP() > m_Parts[i].GetPartsInfo().vHitCondition[k].iRequiredPartsUnderHP )
							{
								bHitCondition = false;
								break;
							}
						}
					}

					if( bHitCondition == false )
						break;
				}
				if( bHitCondition == false )
					continue;

#if defined( PRE_ADD_LOTUSGOLEM )

				if( m_Parts[i].CheckPartsState( MonsterParts::eNoDamage ) == true )
					return 0.f;

				if( m_Parts[i].CheckPartsState( MonsterParts::eIgnore ) == true )
				{
					bBreak = true;
					break;
				}

#endif // #if defined( PRE_ADD_LOTUSGOLEM )

				_OnPartsDamage( pHitter, HitParam, m_Parts[i] );
				_SendPartsHP( &m_Parts[i], pHitter->GetActorHandle() ? pHitter->GetActorHandle()->GetUniqueID() : 0 );

				bBreak = true;
				break;
			}
		}

		if( bBreak )
			break;
	}

#if defined( PRE_ADD_LOTUSGOLEM )
	if( static_cast<MAAiScript*>(GetAIBase())->GetScriptData().m_iOnlyPartsDamage > 0 )
	{
		if( bBreak == false )
		{
			// 2번 타입은 유효한 파츠가 아닌곳은 헛방치게 한다.
			if( static_cast<MAAiScript*>(GetAIBase())->GetScriptData().m_iOnlyPartsDamage == 2 )
				HitParam.bIgnoreShowDamage = true;
			return 0.f;
		}
	}
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

	fDamage += fAddDamage;
	if( fDamage <= 0.f )
		fDamage = 0.f;

	SetHP( static_cast<INT64>(GetHP()-fDamage) );

	return fDamage;
}

bool CDnPartsMonsterActor::IsLimitAction( const char* pszActionName )
{
	if( m_listDestroyParts.empty() )
		return false;

	for( std::list<MonsterParts*>::iterator itor=m_listDestroyParts.begin() ; itor!=m_listDestroyParts.end() ; ++itor )
	{
		if( (*itor)->IsLimitAction( pszActionName ) )
		{
			//g_Log.LogA( "CDnPartsMonsterActor::IsLimitAction() Action=%s 제한 액션!\r\n", pszActionName );
			return true;
		}
	}

	return false;
}

bool CDnPartsMonsterActor::IsLimitAction( DnSkillHandle hSkill )
{
	std::set<std::string> setLimitAction;

	for( std::list<MonsterParts*>::iterator itor=m_listDestroyParts.begin() ; itor!=m_listDestroyParts.end() ; ++itor )
	{
		for( size_t i=0 ; i<(*itor)->GetPartsInfo().vLimitAction.size() ; ++i )
			setLimitAction.insert( (*itor)->GetPartsInfo().vLimitAction[i] );
	}

	bool bRet = hSkill->IsUseActionNames( setLimitAction );
	//if( bRet )
	//	g_Log.LogA( "CDnPartsMonsterActor::IsLimitAction() SkillID=%d 제한 액션!\r\n", hSkill->GetClassID() );

	return bRet;
}

bool CDnPartsMonsterActor::bIsDestroyParts( const UINT uiTableID )
{
	for( std::list<MonsterParts*>::iterator itor=m_listDestroyParts.begin() ; itor!=m_listDestroyParts.end() ; ++itor )
	{
		if( (*itor)->GetPartsInfo().uiMonsterPartsTableID == uiTableID )
			return true;
	}

	return false;
}

bool CDnPartsMonsterActor::bIsDestroySetParts( const UINT uiSetID )
{
	for( std::list<std::pair<UINT,float>>::iterator itor=m_listDestroySetParts.begin() ; itor!=m_listDestroySetParts.end() ; ++itor )
	{
		if( (*itor).first == uiSetID )
			return true;
	}

	return false;
}

void CDnPartsMonsterActor::_OnPartsDamage( CDnDamageBase* pHitter, SHitParam& HitParam, MonsterParts& Parts )
{
	int nCurHP = Parts.GetHP();
	if( nCurHP <= 0 )
		return;
	
	int nPartsDamage = static_cast<int>(PreCalcDamage( pHitter, HitParam, Parts.GetDefenseRate() ));

	// 파츠 파괴
	if( nCurHP <= nPartsDamage )
	{
		_OnDestroyParts( &Parts );
	}
	else
	{
		Parts.SetHP( nCurHP-nPartsDamage );
	}
}

void CDnPartsMonsterActor::_OnRefreshParts( MonsterParts* pParts )
{
	pParts->SetRefresh( GetActorHandle() );
	_SendPartsHP( pParts );

	// _PartsHPLink
	for( UINT i=0 ; i<pParts->GetPartsInfo().vRebirthPartsTableID.size() ; ++i )
	{
		MonsterParts* pRebirthParts = GetParts( pParts->GetPartsInfo().vRebirthPartsTableID[i] );
		if( pRebirthParts )
		{
			if( pRebirthParts->GetHP() <= 0 )
				pRebirthParts->ResetDestoryDelta();
			else
			{
				pRebirthParts->SetHP( pRebirthParts->GetPartsOriginalInfo().nHP );
				_SendPartsHP( pRebirthParts );
			}
		}
	}
}

void CDnPartsMonsterActor::_OnDestroyParts( MonsterParts* pParts )
{
	pParts->SetDestroy( GetActorHandle() );
	m_listDestroyParts.push_back( pParts );

	// 세트파츠 파괴 검사
	MAAiScript*			pScript = static_cast<MAAiScript*>(m_pAi);
	const ScriptData&	data	= pScript->GetScriptData();
	for( UINT i=0 ; i<data.m_vSetPartsRebirthTable.size() ; ++i )
	{
		if( bIsDestroySetParts(i) )
			continue;

		UINT uiDestroyCount = 0;

		for( UINT j=0 ; j<data.m_vSetPartsRebirthTable[i].vPartsTableID.size() ; ++j )
		{
			UINT uiPartsID = data.m_vSetPartsRebirthTable[i].vPartsTableID[j];
			if( !bIsDestroyParts( uiPartsID ) )
				break;
			++uiDestroyCount;
		}

		if( uiDestroyCount == data.m_vSetPartsRebirthTable[i].vPartsTableID.size() )
		{
			m_listDestroySetParts.push_back( std::make_pair(i,static_cast<float>(data.m_vSetPartsRebirthTable[i].uiRebirthTick/1000.f) ));
		}
	}

	// 현재 액션이 LimitAction인경우
	DnActorHandle hActor = GetActorHandle();
	if( hActor )
	{
		const char* pszAction = hActor->GetCurrentAction();
		if( pszAction && strlen( pszAction ) )
		{
			if( pParts->IsLimitAction( pszAction ) )
			{
				//g_Log.LogA( "제한 액션이어서 멈춤 %s\r\n", pszAction );
				hActor->CmdStop( "Stand" );
			}
			//g_Log.LogA( "파츠파괴 액션 %s\r\n", pszAction );
		}
	}

	if( m_pAi )
	{
		// 파츠 파괴 되었을 때 스킬 상태효과 없앰
		bool bDeadAction = false;
		if( !pParts->GetPartsInfo().szDeadActName.empty() )
		{
			bDeadAction = true;
			SetActionQueue( pParts->GetPartsInfo().szDeadActName.c_str() );
		}

		DnSkillHandle hSkill = pParts->GetSkillHandle();
		if( hSkill )
		{
			AICommand::SRemoveBlowBySkillIndex sCommand;
			sCommand.iSkillIndex = hSkill->GetClassID();

			m_pAi->AICmdMsgProc( AICommand::CMD_REMOVEBLOW_BYSKILLINDEX, &sCommand );
		}

		// 파츠 파괴 되었을 때 사용할 스킬
		int iSkillIndex = pParts->GetDestroySkillIndex();
		if( iSkillIndex > 0 && bDeadAction == false )
		{
			AICommand::SUseSkill sCommand;
			sCommand.iSkillIndex = iSkillIndex;

			m_pAi->AICmdMsgProc( AICommand::CMD_USESKILL, &sCommand );
		}
	}
}

void CDnPartsMonsterActor::_SendPartsHP( MonsterParts* pParts, UINT uiSessionID/*=0*/ )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	
	UINT	uiPartsTableID	= pParts->GetPartsTableID();
	int		iHP				= pParts->GetHP();

	Stream.Write( &uiPartsTableID, sizeof(UINT) );
	Stream.Write( &iHP, sizeof(int) );
	Stream.Write( &uiSessionID, sizeof(UINT) );

	Send( eActor::SC_PARTSDAMAGE, &Stream );
}


#if defined(PRE_FIX_59347)
void CDnPartsMonsterActor::ApplyPartsDamage(float& fDamage, DnActorHandle hHitterActor)
{
	float fAddDamage	= 0.f;

	std::list<MonsterParts*> activateParts;
	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		MonsterParts& monsterParts = m_Parts[i];

		bool bHitCondition = true;
		for( UINT k=0 ; k<monsterParts.GetPartsInfo().vHitCondition.size() ; ++k )
		{
			for( UINT n=0 ; n<m_Parts.size() ; ++n )
			{
				if( monsterParts.GetPartsInfo().vHitCondition[k].iRequiredPartsID == m_Parts[n].GetPartsInfo().uiMonsterPartsTableID )
				{
					if( m_Parts[n].GetHP() > monsterParts.GetPartsInfo().vHitCondition[k].iRequiredPartsUnderHP )
					{
						bHitCondition = false;
						break;
					}
				}
			}

			if( bHitCondition == false )
				break;
		}
		if( bHitCondition == false )
			continue;


#if defined( PRE_ADD_LOTUSGOLEM )
		if( monsterParts.CheckPartsState( MonsterParts::eNoDamage ) == true )
			continue;
		if( monsterParts.CheckPartsState( MonsterParts::eIgnore ) == true )
			continue;
#endif
		activateParts.push_back(&monsterParts);
	}


	if( activateParts.empty() == false )
	{

#ifdef PRE_FIX_67656

		float m_fMainDamageRatio = 0.f;

		std::list<MonsterParts*>::iterator iter = activateParts.begin();
		for ( iter = activateParts.begin() ; iter != activateParts.end(); ++iter)
		{
			MonsterParts* pMonsterParts = (*iter);
			if( pMonsterParts )
			{
				int nCurHP = pMonsterParts->GetHP();
				if( nCurHP <= 0 )
					continue;

				// 파츠 파괴
				if( nCurHP <= fDamage )
				{
					_OnDestroyParts( pMonsterParts );
				}
				else
				{
					pMonsterParts->SetHP( nCurHP - (int)fDamage );
				}

				_SendPartsHP( pMonsterParts, hHitterActor ? hHitterActor->GetUniqueID() : 0 );

				if( m_fMainDamageRatio < pMonsterParts->GetMainDamageRate() )
					m_fMainDamageRatio = pMonsterParts->GetMainDamageRate();
			}
		}
#endif

		//실제 몬스터 HP는 감소 되면 안되므로 fDamage값은 0.0으로 설정..

#ifdef PRE_FIX_67656
		fDamage = fDamage * m_fMainDamageRatio;
#else
		fDamage = 0.f;
#endif

	}
}
#endif // PRE_FIX_59347

#if defined(PRE_FIX_66687)
void CDnPartsMonsterActor::ApplyPartsHP(float fRate)
{
	for( UINT i=0 ; i<m_Parts.size() ; ++i )
	{
		int iHP = m_Parts[i].GetHP();
		int iMaxHP = m_Parts[i].GetMaxHP();

		int iDelta = int((float)iMaxHP * fRate);
		int iResult = iHP + iDelta;

		if( iMaxHP < iResult )
		{
			iResult = iMaxHP;
			iDelta = iMaxHP - iHP;
		}
		else if( iResult <= 0 )
		{
			iResult = 0;
			iDelta = 0 - iHP;

			// # 68809 - [데저트 드래곤] HP_RATIO로 몬스터 파츠 파괴시 destroyparts(Lua 명령어) 가 작동하지 않습니다.
			_OnDestroyParts( &(m_Parts[i]) );
		}

		m_Parts[i].SetHP( iResult );
		_SendPartsHP( &m_Parts[i] );
	}
}
#endif // PRE_FIX_66687