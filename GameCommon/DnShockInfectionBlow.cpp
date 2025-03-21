#include "StdAfx.h"
#include "DnShockInfectionBlow.h"
#include "EtActionSignal.h"
#include "DnProjectile.h"

#if _GAMESERVER
#include "DnGameServerManager.h"

#else
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnMainDlg.h"

#endif

#define SHOCK_INFECTION_ACTION_NAME "Skill_ShockInfection"


#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnShockInfectionBlow::CDnShockInfectionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#ifdef _GAMESERVER
																							, m_dwCoolTime(0)
																							, m_LastActivateTime( 0 )
#else
																							, m_pLocalPlayerActor( NULL )
#endif // #ifdef _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_168;
	SetValue( szValue );

	m_fHitApplyPercent = 0.0f;
	m_fLimitRadius = 0.0f;
	m_fShockProb = 0.0f;
	m_nShockDurationTime = 0;

	SecureZeroMemory( &m_ProjectileSignalInfo, sizeof(m_ProjectileSignalInfo) );

	// 인자가 총 4개임.
	// 감전확률;감전지속시간;범위;발사체 히트 시그널에 적용할 데미지%
	string strValue( szValue );

	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens;
	TokenizeA( strValue, vlTokens, ";" );
	bool bValidArgument = (4 == (int)vlTokens.size());
	_ASSERT( bValidArgument && "감전 전이 상태효과 인자 셋팅이 잘못 되었습니다." );

	if( bValidArgument )
	{
		string& strShockProb = vlTokens.at( 0 );
		string& strShockDuration = vlTokens.at( 1 );
		string& strRadius = vlTokens.at( 2 );
		string& strProjectileHitSignalPercent = vlTokens.at( 3 );

		m_fShockProb = (float)atof( strShockProb.c_str() );
		m_nShockDurationTime = (int)atoi( strShockDuration.c_str() );
		m_fLimitRadius = (float)atof( strRadius.c_str() );
		m_fHitApplyPercent = (float)atof( strProjectileHitSignalPercent.c_str() );
		
		//// 우선 테스트용.
		//m_fShockProb = 1.0f;
		//m_nShockDurationTime = 100000;
		//m_fLimitRadius = 1000.0f;
		//m_fHitApplyPercent = 1.0f;
		////////////////////////////////////////////////////////////////////////////
	}

	// 감전 전이에서 사용할 발사체 시그널 정보를 미리 약속해둔 액션에서 발사체 정보를 꺼내온다.
	// 이 상태효과는 self 로 추가되는 것이기 때문에 m_hActor 에서 얻어오면 된다.
	CDnActionBase::ActionElementStruct* pActionElement = m_hActor->GetElement( SHOCK_INFECTION_ACTION_NAME );
	bool bValid = false;
	if( pActionElement )
	{
		if( false == pActionElement->pVecSignalList.empty() )
		{
			CEtActionSignal* pSignal = pActionElement->pVecSignalList.front();
			if( STE_Projectile == pSignal->GetSignalIndex() )
			{
#ifdef PRE_FIX_MEMOPT_SIGNALH
				CopyShallow_ProjectileStruct(m_ProjectileSignalInfo, static_cast<ProjectileStruct*>(pSignal->GetData()));
#else
				m_ProjectileSignalInfo = *static_cast<ProjectileStruct*>(pSignal->GetData());
#endif

#ifdef _GAMESERVER
				// 감전 상태 효과
				m_AddtionalSE.nID = STATE_BLOW::BLOW_043;
				m_AddtionalSE.ApplyType = CDnSkill::ApplyTarget;
				m_AddtionalSE.nDurationTime = m_nShockDurationTime;
				m_AddtionalSE.szValue = "1.0";
#else
				// 상태효과 이펙트 붙이고 말고를 이 클래스에서직접 컨트롤 한다.
				UseTableDefinedGraphicEffect( false );
#endif // #ifdef _GAMESERVER

				bValid = true;
			}
		}
	}

	// 리소스 셋팅이 잘못되어 상태효과 바로 종료됨.
	if( false == bValid )
	{
		SetState( STATE_BLOW::STATE_END );
	}

	//m_fRate = 1.0f;
	//m_fLimitRadius = 1000.0f;
	//m_nDurationTime = 5000;
	//m_fHitApplyPercent = 1.0f;

	////////////////////////////////////////////////////////////////////////////
	//m_OffSet = EtVector3(0.0f, 0.0f, 0.0f);
	//m_Direction = EtVector3(0.0f, 0.0f, 1.0f);
	//m_DestPos = EtVector3(0.0f, 0.0f, 0.0f);

	//m_nOrbitType = CDnProjectile::Homing;
	//m_nTargetType = CDnProjectile::Target;
	//m_VelocityType = CDnProjectile::Accell;
	//m_fProjectileSpeed = 1000.0f;
	//m_nProjectileValidTime = 5000;
	////////////////////////////////////////////////////////////////////////////

	//memset(&m_ProjectileSignalInfo, 0, sizeof(m_ProjectileSignalInfo));

	//m_ProjectileSignalInfo.nWeaponTableID = 2000;
	//m_ProjectileSignalInfo.nProjectileIndex = -1;
	//m_ProjectileSignalInfo.nOrbitType = CDnProjectile::Homing;
	//m_ProjectileSignalInfo.nTargetType = CDnProjectile::Target;
	//m_ProjectileSignalInfo.nDestroyOrbitType = CDnProjectile::Instantly;
	//m_ProjectileSignalInfo.bTraceHitTarget = FALSE;

	//m_ProjectileSignalInfo.vOffset = &m_OffSet;
	//m_ProjectileSignalInfo.vDirection = &m_Direction;
	//m_ProjectileSignalInfo.vDestPosition = &m_DestPos;
	
	//// 감전 상태 효과
	//m_AddtionalSE.nID = STATE_BLOW::BLOW_043;
	//m_AddtionalSE.ApplyType = CDnSkill::ApplyTarget;
	//m_AddtionalSE.nDurationTime = m_nDurationTime;
	//m_AddtionalSE.szValue = "1.0;1.0";

	//m_bCreateProjectile = false;

#ifdef _GAMESERVER
	AddCallBackType( SB_ONTARGETHIT );
#endif
}

CDnShockInfectionBlow::~CDnShockInfectionBlow(void)
{

}

void CDnShockInfectionBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// 쿨타임을 부모 스킬의 정보를 그대로 활용한다.
	if( m_ParentSkillInfo.hSkillUser )
	{
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		m_dwCoolTime = DWORD(hParentSkill->GetDelayTime() * 1000.0f);
	}
#else
	m_pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>( m_hActor.GetPointer() );
	if( !m_hParentSkillForCoolTime && m_pLocalPlayerActor )
	{
		m_hParentSkillForCoolTime = CDnSkill::CreateSkill( m_hActor, m_ParentSkillInfo.iSkillID, 
#if defined(PRE_FIX_NEXTSKILLINFO)
															m_ParentSkillInfo.nSkillLevel);
#else
														   m_ParentSkillInfo.iSkillLevelID - m_ParentSkillInfo.iSkillLevelIDOffset + 1 );
#endif // PRE_FIX_NEXTSKILLINFO
	}
#endif

	__super::OnBegin(LocalTime, fDelta);
}

void CDnShockInfectionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
#else
	if( m_pLocalPlayerActor )
	{
		// 쿨타임 진행.
		m_hParentSkillForCoolTime->Process( LocalTime, fDelta ); 
	}
#endif // _GAMESERVER

	//if (m_bCreateProjectile)
	//{
	//	CreateProjectile();
	//	m_bCreateProjectile = false;
	//}
	//OutputDebug( "CDnFireBurnBlow::Process, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}

//void CDnShockInfectionBlow::DoShockInfection( DnActorHandle hTargetActor )
//{
//#ifdef _GAMESERVER
//	m_LastActivateTime = m_hActor->GetLocalTime();
//	CreateProjectileOnServer( hTargetActor );
////#else
////	CreateProjectileOnClient();
//#endif
//}

#if defined(_GAMESERVER)
void CDnShockInfectionBlow::DoShockInfection( DnActorHandle hHitTargetActor )
{
	if( !hHitTargetActor )
		return;

	m_LastActivateTime = m_hActor->GetLocalTime();

	DNVector(DnActorHandle) vlActorsInRange;
	if (!hHitTargetActor->ScanActor( hHitTargetActor->GetRoom(), *hHitTargetActor->GetPosition(), m_fLimitRadius, vlActorsInRange ))
		return;
	
	// 우선 제일 가까운 액터 선택.. 추후에 다른 규칙이 들어간다면 그렇게 처리한다.
	float fShortestDistanceSQ = FLT_MAX;
	DnActorHandle hActorToInfection;
	DWORD dwNumActors = (DWORD)vlActorsInRange.size();
	for( DWORD dwActor = 0; dwActor < dwNumActors; ++dwActor )
	{
		DnActorHandle hActor = vlActorsInRange.at( dwActor );
		if( false == (hActor->IsShow() && hActor->IsProcess()) )
			continue;

		// 같은 액터 말고
		if( hActor == hHitTargetActor )
			continue;

		// npc 액터는 제외
		if( hActor->IsNpcActor() )
			continue;

		// 다른 팀인 경우만.. 
		if( hActor->GetTeam() != m_hActor->GetTeam())
		{
			EtVector3 vDistance = (*m_hActor->GetPosition()) - (*hActor->GetPosition());

			float fLengthSQ = EtVec3LengthSq( &vDistance );
			if( fLengthSQ < fShortestDistanceSQ )
			{
				fShortestDistanceSQ = fLengthSQ;
				hActorToInfection = hActor;
			}
		}
	}

	
	if( hActorToInfection )
	{
		ProjectileStruct* pProjectileSignalInfo = &m_ProjectileSignalInfo;

		// 프로젝타일 발사 관련 기타 설정들.
		DnSkillHandle hSkill = m_hActor->FindSkill( m_ParentSkillInfo.iSkillID );
		if( !hSkill )
			return;

		MatrixEx Cross = *m_hActor->GetMatEx();
		Cross.m_vPosition.y += m_hActor->GetHeight() / 2.0f;

		CDnProjectile *pProjectile = new CDnProjectile( GetRoom(), m_hActor );

		pProjectile->SetPierce( pProjectileSignalInfo->bPierce == TRUE ? true : false );
		pProjectile->SetMaxHitCount( pProjectileSignalInfo->nMaxHitCount );

		if( pProjectileSignalInfo->nWeaponTableID > 0 ) 
		{
			if( pProjectileSignalInfo->nProjectileIndex != -1 )
			{
				DnWeaponHandle hWeapon = CDnWeapon::GetSmartPtr( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), pProjectileSignalInfo->nProjectileIndex );
				if( hWeapon ) *(CDnWeapon*)pProjectile = *hWeapon.GetPointer();
			}
		}

		int nLength = pProjectile->GetWeaponLength();
		if( pProjectileSignalInfo->bIncludeMainWeaponLength ) 
		{
			DnWeaponHandle hWeapon = m_hActor->GetWeapon(0);
			if ( hWeapon )
				nLength += hWeapon->GetWeaponLength();
		}

		// 감전 전이가 잘 되도록 최소한 범위값 만큼의 사거리를 준다.
		if( nLength < (int)m_fLimitRadius )
			nLength = (int)m_fLimitRadius;

		pProjectile->SetWeaponLength( nLength );

		pProjectile->SetWeaponType( (CDnWeapon::WeaponTypeEnum)( pProjectile->GetWeaponType() | CDnWeapon::Projectile ) );

		pProjectile->SetSpeed( pProjectileSignalInfo->fSpeed );

		pProjectile->SetTargetPosition( *hActorToInfection->GetPosition() );
		pProjectile->SetTargetActor( hActorToInfection );

		pProjectile->Initialize( Cross, static_cast<CDnProjectile::OrbitTypeEnum>(pProjectileSignalInfo->nOrbitType), 
										static_cast<CDnProjectile::DestroyOrbitTypeEnum>(pProjectileSignalInfo->nDestroyOrbitType), 
										static_cast<CDnProjectile::TargetTypeEnum>(pProjectileSignalInfo->nTargetType) );
		pProjectile->SetValidTime( pProjectileSignalInfo->nValidTime );
		pProjectile->SetVelocityType( static_cast<CDnProjectile::VelocityTypeEnum>(pProjectileSignalInfo->VelocityType) );

		pProjectile->SetParentSkill( hSkill );

		//m_ParentSkillInfo.hPrevAttacker = m_hActor;
		CDnSkill::SkillInfo ParentSkillInfo = m_ParentSkillInfo;
		
		char szBuff[128] = {0, };
		_snprintf_s(szBuff, _countof(szBuff), _TRUNCATE, "%d", 402 );
		ParentSkillInfo.szEffectOutputIDToClient = szBuff;

		// 발사체의 shooter 는 이 상태효과를 가진 액터이고 감전 대상은 적군이므로 이렇게 보내줘야 클라이언트에서 이펙트가 제대로 붙는다.
		// 이 상태효과를 사용하는 주체는 self 로 이 상태효과를 갖고 있다.
		ParentSkillInfo.eTargetType = CDnSkill::TargetTypeEnum::Enemy;
		pProjectile->SetParentSkillInfo( ParentSkillInfo );
		pProjectile->FromSkill( true );

		pProjectile->SetHitApplyPercent( m_fHitApplyPercent );

		// 감전 상태 효과 추가..
		pProjectile->AddStateEffect( m_AddtionalSE );

		boost::shared_ptr<CDnState> pActorStateSnapshotToPass = boost::shared_ptr<CDnState>(new CDnState);
		*pActorStateSnapshotToPass = *static_cast<CDnState*>( m_ParentSkillInfo.hSkillUser.GetPointer() );
		pActorStateSnapshotToPass->SetAttackMMax( m_hActor->GetAttackMMaxWithoutSkill() );
		pActorStateSnapshotToPass->SetAttackMMin( m_hActor->GetAttackMMinWithoutSkill() );
		pProjectile->SetShooterStateSnapshot( pActorStateSnapshotToPass );
		pProjectile->PostInitialize();

#if defined(PRE_FIX_65287)

		float fFinalDamageRate = 0.0f;
		if (m_hActor && m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
		{
			DNVector(DnBlowHandle) vlhBlows;
			m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				fFinalDamageRate += vlhBlows[i]->GetFloatValue();
			}
		}

		pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287


		// 근처에 가까운 대상에게 발사체를 보내도록 클라에 패킷을 보내준다.
		// 대상은 서버에서 선택된 액터로.
		BYTE pBuffer[ 128 ] = { 0 };
		CPacketCompressStream Stream( pBuffer, 128 );

		DWORD dwTargetActorUniqueID = hHitTargetActor->GetUniqueID();
		DWORD dwActorToInfection = hActorToInfection->GetUniqueID();
		int nBlowID = GetBlowID();

		Stream.Write( &dwTargetActorUniqueID, sizeof(dwTargetActorUniqueID) );
		Stream.Write( &dwActorToInfection, sizeof(dwActorToInfection) );
		Stream.Write( &nBlowID, sizeof(nBlowID) );
		//Stream.Write( &m_bCreateProjectile, sizeof(m_bCreateProjectile));

		m_hActor->Send( eActor::SC_SHOCK_INFECTION, &Stream );
	}
}

#else

void CDnShockInfectionBlow::DoShockInfection( DnActorHandle hHitTargetActor, DnActorHandle hActorToInfection )
{
	if( hHitTargetActor && hActorToInfection )
	{
		// 서버에서 쿨타임 패링이 성공했을 경우 따로 패킷으로 보내와서 호출된다.
		// 로컬 클라이언트일때만 쿨타임 보여주면 된다.
		if( m_pLocalPlayerActor )
		{
			// 쿨타임만 진행시켜줌. 화면 표시용
			m_hParentSkillForCoolTime->OnBeginCoolTime();
			if( m_hParentSkillForCoolTime->HasCoolTime() )
			{
				CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
				if( pMainDlg ) 
				{
					pMainDlg->AddPassiveSkill( m_hParentSkillForCoolTime );
				}
			}
		}

		// 프로젝타일 발사 관련 기타 설정들.
		CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( m_hActor,
																	  *hHitTargetActor->GetMatEx(), &m_ProjectileSignalInfo, 
																	  EtVector3( 0.0f, 0.0f, 0.0f), hActorToInfection );
	}
}
#endif // _GAMESERVER


void CDnShockInfectionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(_GAMESERVER)
void CDnShockInfectionBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	if (!m_hActor)
		return;

	if (!hTargetActor || hTargetActor->IsDie())
		return;

	if (hTargetActor->IsImmuned(m_StateBlow.emBlowIndex))
		return;

	// 현재 적용되고 있는 내 액션의 히트 시그널 정보를 얻어온다.
	CDnDamageBase::SHitParam* pHitParameter = m_hActor->GetHitParam();

	// 적용되는 구간의 기준은 히트시그널의 구간과 일치한다.
	// 한번 쿨타임에 하나의 히트시그널에 적용된다.
	// first hit 플래그가 켜져 있다면 이 히트 시그널이 처음으로 hit 된 것이므로
	// 쿨타임을 돌려주기 시작한다. 다음 FirstHit 플래그가 올 때는 쿨타임 간격을
	// 체크해서 해당 간격만큼 시간이 지나지 않았다면 감전 전이를 시키지 않도록 한다.
	bool bNowActivate = true;
	if( pHitParameter->bFirstHit )
	{
		LOCAL_TIME NowLocalTime = m_hActor->GetLocalTime();
		if( NowLocalTime - m_LastActivateTime < m_dwCoolTime )
		{
			bNowActivate = false;
		}
	}

	if( bNowActivate )
	{
		//감전 상태가 아니면 스킵..
		if (!hTargetActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_043))
			return;

		//빛 속성 공격이 아니면 스킵
		CDnDamageBase::SHitParam* pHitParam = hTargetActor->GetHitParam();
		if( NULL == pHitParam || 
			CDnState::Light != pHitParam->HasElement )
			return;
		
		//확률 계산..
		bool bExecuteable = rand() % 10000 <= (m_fShockProb * 10000.0f);
		if (!bExecuteable)
			return;

		DoShockInfection( hTargetActor );
	}

	//이 상태효과의 m_bCreateProjectile를 바꾸는 패킷을 보낸다..
	//m_bCreateProjectile = true;
}
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnShockInfectionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 변수
	float fShockProb[2] = {0.0f, };
	int nShockDuration[2] = { 0, };
	float fLimitRadius[2] = { 0.0f, };
	float fHitApplyPercent[2] = { 0.0f, };

	// 인자가 총 4개임.
	// 감전확률;감전지속시간;범위;발사체 히트 시그널에 적용할 데미지%
	std::string strValue( szOrigValue );

	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens[2];
	TokenizeA( strValue, vlTokens[0], ";" );
	
	if( (int)vlTokens[0].size() == 4 )
	{
		fShockProb[0] = (float)atof( vlTokens[0][0].c_str() );
		nShockDuration[0] = (int)atoi( vlTokens[0][1].c_str() );
		fLimitRadius[0] = (float)atof( vlTokens[0][2].c_str() );
		fHitApplyPercent[0] = (float)atof( vlTokens[0][3].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	strValue = szAddValue;
	TokenizeA( strValue, vlTokens[1], ";" );

	if( (int)vlTokens[1].size() == 4 )
	{
		fShockProb[1] = (float)atof( vlTokens[1][0].c_str() );
		nShockDuration[1] = (int)atoi( vlTokens[1][1].c_str() );
		fLimitRadius[1] = (float)atof( vlTokens[1][2].c_str() );
		fHitApplyPercent[1] = (float)atof( vlTokens[1][3].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultShockProb = fShockProb[0] + fShockProb[1];
	int nResultShockDuration = max(nShockDuration[0], nShockDuration[1]);
	float fResultLimitRadius = max(fLimitRadius[0], fLimitRadius[1]);
	float fResultHitApplyPrecent = fHitApplyPercent[0] + fHitApplyPercent[1];

	sprintf_s(szBuff, "%f;%d;%f;%f", fResultShockProb, nResultShockDuration, fResultLimitRadius, fResultHitApplyPrecent);

	szNewValue = szBuff;
}

void CDnShockInfectionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 변수
	float fShockProb[2] = {0.0f, };
	int nShockDuration[2] = { 0, };
	float fLimitRadius[2] = { 0.0f, };
	float fHitApplyPercent[2] = { 0.0f, };

	// 인자가 총 4개임.
	// 감전확률;감전지속시간;범위;발사체 히트 시그널에 적용할 데미지%
	std::string strValue( szOrigValue );

	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens[2];
	TokenizeA( strValue, vlTokens[0], ";" );

	if( (int)vlTokens[0].size() == 4 )
	{
		fShockProb[0] = (float)atof( vlTokens[0][0].c_str() );
		nShockDuration[0] = (int)atoi( vlTokens[0][1].c_str() );
		fLimitRadius[0] = (float)atof( vlTokens[0][2].c_str() );
		fHitApplyPercent[0] = (float)atof( vlTokens[0][3].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	strValue = szAddValue;
	TokenizeA( strValue, vlTokens[1], ";" );

	if( (int)vlTokens[1].size() == 4 )
	{
		fShockProb[1] = (float)atof( vlTokens[1][0].c_str() );
		nShockDuration[1] = (int)atoi( vlTokens[1][1].c_str() );
		fLimitRadius[1] = (float)atof( vlTokens[1][2].c_str() );
		fHitApplyPercent[1] = (float)atof( vlTokens[1][3].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultShockProb = fShockProb[0] - fShockProb[1];
	int nResultShockDuration = min(nShockDuration[0], nShockDuration[1]);
	float fResultLimitRadius = min(fLimitRadius[0], fLimitRadius[1]);
	float fResultHitApplyPrecent = fHitApplyPercent[0] - fHitApplyPercent[1];

	sprintf_s(szBuff, "%f;%d;%f;%f", fResultShockProb, nResultShockDuration, fResultLimitRadius, fResultHitApplyPrecent);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW