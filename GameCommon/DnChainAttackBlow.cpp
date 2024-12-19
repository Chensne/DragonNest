#include "StdAfx.h"
#include "DnChainAttackBlow.h"
#include "DnProjectile.h"
#include "EtActionSignal.h"
#include <float.h>
#if _GAMESERVER
#include "DnGameServerManager.h"
#endif

#ifdef _CLIENT
#include "DnHideMonsterActor.h"
#include "DnPetActor.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnChainAttackBlow::CDnChainAttackBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
	m_iMaxCount( 0 ),
	m_fRange( 0.0f ),
	m_fHitApplyPercent( -1.0f ),
	m_iRootAttackerTeam( 0 )
{
#ifndef _GAMESERVER
	ZeroMemory( &m_ProjectileSignalInfo, sizeof(m_ProjectileSignalInfo) );
#endif

	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_060;
	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	string strValue( szValue );
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bValidArgument = (string::npos != iSemiColonIndex);
	_ASSERT( bValidArgument && "ü�ΰ��� ����ȿ�� ȿ�� ��ġ�� �߸� �Ǿ����ϴ�. �����ݷ��� ã�� ���� ����" );
	if( bValidArgument )
	{
		string strRange = strValue.substr( 0, iSemiColonIndex );
		
		int iOffset = iSemiColonIndex+1;
		iSemiColonIndex = (int)strValue.find_first_of( ';', iOffset );
		string strMaxCount = strValue.substr( iOffset, iSemiColonIndex-iOffset );

		iOffset = iSemiColonIndex+1;
		iSemiColonIndex = (int)strValue.find_first_of( ';', iOffset );
		string strHitApplyPercent = strValue.substr( iOffset, iSemiColonIndex-iOffset );

		// ���ڷ� ���� �� �� ����. ����;�ִ���ƮȽ��;��Ʈ�Ķ����hitpercent
		m_fRange = (float)atoi( strRange.c_str() );
		m_iMaxCount = (int)atoi( strMaxCount.c_str() );
		m_fHitApplyPercent = (float)atof( strHitApplyPercent.c_str() );
	}
}

CDnChainAttackBlow::~CDnChainAttackBlow(void)
{

}


void CDnChainAttackBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "[%x] CDnChainAttackBlow::OnBegin, Value: %2.2f\n", this, m_fValue );
}


void CDnChainAttackBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	//// ����׸� ���� Ÿ�� ���� �д�.
	//m_fElapsedTime += fDelta;
	//if( 1.0f > m_fElapsedTime )
	//	return;

	// Note: ���� ���� ������Ÿ�� ������ �״�� �Ʊ����� ����. ������ Ŭ�� ���������� �����Ѵ�.
	// ���� ����� ���� Ŭ��� �´� ��ġ�� �� �ٸ� ���� �ִ�.
	// ������ ���� ���� �����ߴ� ������ ���ݷ��� �����ؼ� ������ ��.

	// ������ �Ÿ� �ȿ� �Ʊ��� ���� ������Ÿ�� ����. ������ ����ȿ�� ����.
	DNVector(DnActorHandle) vlActorsInRange;
#if _GAMESERVER

	// ó�� ������ ü�� �����̶�� ��ü ī��Ʈ�� ����� �д�.
	if( -1 == m_ParentSkillInfo.iLeaveCount )
		m_ParentSkillInfo.iLeaveCount = m_iMaxCount;

	// ī��Ʈ �� �Ǹ� �� �̻� ���ĵ��� �ʰ� ���⼭ ��.
	if( m_ParentSkillInfo.iLeaveCount <= 0 )
	{
		SetState( STATE_BLOW::STATE_END );
		return;
	}

	CDnActor::ScanActor( m_hActor->GetRoom(), *m_hActor->GetPosition(), m_fRange, vlActorsInRange );

	float fShortestDistanceSQ = FLT_MAX;
	DnActorHandle hActorToAttack;
	DWORD dwNumActors = (DWORD)vlActorsInRange.size();
	for( DWORD dwActor = 0; dwActor < dwNumActors; ++dwActor )
	{
		DnActorHandle hActor = vlActorsInRange.at( dwActor );
		if (!hActor)
			continue;

		if( false == (hActor->IsShow() && hActor->IsProcess()) )
			continue;

		if (hActor->IsDie() || hActor->IsNpcActor() )
			continue;

		// ������ ������ ����ȿ�� �Ѱ���� �������״� �ٽ� ���� �ʴ´�.
		if( m_ParentSkillInfo.hPrevAttacker != hActor )
		{
			// #30643 �� �̿ܿ� �� ���̴�~~~
			if( m_iRootAttackerTeam != hActor->GetTeam() &&
				hActor != m_hActor )
			{
				EtVector3 vDistance = (*m_hActor->GetPosition()) - (*hActor->GetPosition());

				float fLengthSQ = EtVec3LengthSq( &vDistance );
				if( fLengthSQ < fShortestDistanceSQ )
				{
					fShortestDistanceSQ = fLengthSQ;
					hActorToAttack = hActor;
				}
			}
		}
	}

	// 01/23 m_pProjectileSignalInfo �� NULL �� ��찡 �־ �������� �켱 NULL üũ�� �صд�.
	// 2010.3.19 ���� ���� ���� �ǽ� ���� �κ� ����.
	if( hActorToAttack && m_hRootAttacker )
	{
		ProjectileStruct* pProjectileSignalInfo = &m_ProjectileSignalInfo;
	
		// ������Ÿ�� �߻� ���� ��Ÿ ������.
		DnSkillHandle hSkill = m_hRootAttacker->FindSkill( m_ParentSkillInfo.iSkillID );

		// ȣ������ Ÿ���� ������� �����ش�.
		pProjectileSignalInfo->nOrbitType = CDnProjectile::Homing;
		pProjectileSignalInfo->nTargetType = CDnProjectile::Target;
		pProjectileSignalInfo->VelocityType = CDnProjectile::Accell;
		pProjectileSignalInfo->fSpeed = 1000.0f;
		pProjectileSignalInfo->nValidTime = 5000;
		//CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( m_hActor->GetRoom(), hActorToAttack,
		//															  *m_hActor->GetMatEx(), pProjectileSignalInfo.get() );

		MatrixEx Cross = *m_hActor->GetMatEx();
		Cross.m_vPosition.y += m_hActor->GetHeight() / 2.0f;

		CDnProjectile *pProjectile = new CDnProjectile( GetRoom(), m_hRootAttacker );
		
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
		pProjectile->SetWeaponLength( nLength );

		pProjectile->SetWeaponType( (CDnWeapon::WeaponTypeEnum)( pProjectile->GetWeaponType() | CDnWeapon::Projectile ) );

		pProjectile->SetSpeed( pProjectileSignalInfo->fSpeed );

		pProjectile->SetTargetPosition( *hActorToAttack->GetPosition() );
		pProjectile->SetTargetActor( hActorToAttack );

		pProjectile->Initialize( Cross, static_cast<CDnProjectile::OrbitTypeEnum>(pProjectileSignalInfo->nOrbitType), 
										static_cast<CDnProjectile::DestroyOrbitTypeEnum>(pProjectileSignalInfo->nDestroyOrbitType), 
										static_cast<CDnProjectile::TargetTypeEnum>(pProjectileSignalInfo->nTargetType) );
		pProjectile->SetValidTime( pProjectileSignalInfo->nValidTime );
		pProjectile->SetVelocityType( static_cast<CDnProjectile::VelocityTypeEnum>(pProjectileSignalInfo->VelocityType) );
		
		pProjectile->SetParentSkill( hSkill );
		
		m_ParentSkillInfo.hPrevAttacker = m_hActor;
		m_ParentSkillInfo.iLeaveCount -= 1; // �����Ǿ��ִ� ī��Ʈ �ϳ��� ���δ�.
		pProjectile->SetParentSkillInfo( m_ParentSkillInfo );
		pProjectile->FromSkill( true );
		
		pProjectile->SetHitApplyPercent( m_fHitApplyPercent );

		int iNumSE = hSkill->GetStateEffectCount();
		for( int i = 0; i < iNumSE; ++i )
		{
			const CDnSkill::StateEffectStruct* pStateEffect = hSkill->GetStateEffectFromIndex( i );
			if( pStateEffect->ApplyType == CDnSkill::ApplySelf ) 
				continue;

			CDnSkill::StateEffectStruct SE = *pStateEffect;

			// ü�� ���� ����ȿ���� ���� �ð� ������ ���� ������Ÿ�Ͽ� ����ȿ�� �Ǿ ����.
			if( STATE_BLOW::BLOW_060 == pStateEffect->nID )
				SE.nDurationTime = int(GetDurationTime() * 1000);

			pProjectile->AddStateEffect( SE );
		}

		// ������Ÿ�Ͽ� ���� �� �༮�� ���ݷ� ������ ������ ��Ƽ� ����.
		boost::shared_ptr<CDnState> pActorStateSnapshotToPass = boost::shared_ptr<CDnState>(new CDnState);
		*pActorStateSnapshotToPass = m_RootAttackerState;
		pProjectile->SetShooterStateSnapshot( pActorStateSnapshotToPass );
		pProjectile->PostInitialize();
	

#if defined(PRE_FIX_65287)
		float fFinalDamageRate = 0.0f;
		if (m_hRootAttacker && m_hRootAttacker->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
		{
			DNVector(DnBlowHandle) vlhBlows;
			m_hRootAttacker->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				fFinalDamageRate += vlhBlows[i]->GetFloatValue();
			}
		}

		pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287
	
		// ����� �Ʊ����� ������Ÿ���� ���� �� ����ȿ�� ����.
		SetState( STATE_BLOW::STATE_END );

#else
	CDnActor::ScanActor( *m_hActor->GetPosition(), m_fRange, vlActorsInRange );

	float fShortestDistanceSQ = FLT_MAX;
	DnActorHandle hActorToAttack;
	DWORD dwNumActors = (DWORD)vlActorsInRange.size();
	for( DWORD dwActor = 0; dwActor < dwNumActors; ++dwActor )
	{
		DnActorHandle hActor = vlActorsInRange.at( dwActor );
		if (!hActor )
			continue;

		if (hActor->IsDie() || hActor->IsNpcActor() )
			continue;

		// #32115 Ư�� �������� �Ⱥ��̴� ���ͷ� ������ ���ʹ� �н�.
		if( hActor->IsMonsterActor() )
		{
			if( NULL != dynamic_cast<CDnHideMonsterActor*>(hActor.GetPointer()) )
				continue;
		}

		//���������� �� ���� �ȵ�..
		if ( NULL != dynamic_cast<CDnPetActor*>(hActor.GetPointer()))
			continue;

		// ������ ������ ����ȿ�� �Ѱ���� �������״� �ٽ� ���� �ʴ´�.
		if( m_hPrevAttacker != hActor )
		{
			//��ų �����ڿ� ���� �ٸ��� ���� �ǵ����Ѵ�.
			if( m_ParentSkillInfo.hSkillUser && hActor->GetTeam() != m_ParentSkillInfo.hSkillUser->GetTeam() &&
				hActor != m_hActor )
			{
				EtVector3 vDistance = (*m_hActor->GetPosition()) - (*hActor->GetPosition());

				float fLengthSQ = EtVec3LengthSq( &vDistance );
				if( fLengthSQ < fShortestDistanceSQ )
				{
					fShortestDistanceSQ = fLengthSQ;
					hActorToAttack = hActor;
				}
			}
		}
	}

	if( hActorToAttack && m_hRootAttacker )
	{
		// ������Ÿ�� �߻� ���� ��Ÿ ������.
		DnSkillHandle hSkill = m_hRootAttacker->FindSkill( m_ParentSkillInfo.iSkillID );

		// ȣ������ Ÿ���� ������� �����ش�.
		m_ProjectileSignalInfo.nOrbitType = CDnProjectile::Homing;
		m_ProjectileSignalInfo.nTargetType = CDnProjectile::Target;
		m_ProjectileSignalInfo.VelocityType = CDnProjectile::Accell;
		m_ProjectileSignalInfo.fSpeed = 20000.0f;
		m_ProjectileSignalInfo.nValidTime = 5000;
		CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( m_hRootAttacker,
									*m_hActor->GetMatEx(), &m_ProjectileSignalInfo, 
									EtVector3( 0.0f, 0.0f, 0.0f), hActorToAttack, NULL, false );
		if( pProjectile )
		{
			pProjectile->SetTargetActor( hActorToAttack );
			pProjectile->SetChainShooter( m_hActor );
		}

		// ����� �Ʊ����� ������Ÿ���� ���� �� ����ȿ�� ����.
		SetState( STATE_BLOW::STATE_END );
#endif
	}
}


void CDnChainAttackBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#if _GAMESERVER
	OutputDebug( "[%x] CDnChainAttackBlow::OnEnd, LeaveCount: %d\n", this, m_ParentSkillInfo.iLeaveCount );
#else
	OutputDebug( "[%x] CDnChainAttackBlow::OnEnd", this );
#endif
}

// ü�� ����ȿ���� �ߺ� ������.
void CDnChainAttackBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{

}

#if !defined(SW_ADD_CHAINATTACK_STATEEFFECT_20091029_jhk8211 )

#if _GAMESERVER
void CDnChainAttackBlow::WriteAdditionalPacket( void )
{
	//if( m_bWroteAdditionalPacket )
	//{
		_ASSERT( m_ParentSkillInfo.hSkillUser );
		m_hRootAttacker = m_ParentSkillInfo.hSkillUser;
		m_iRootAttackerTeam = m_hRootAttacker->GetTeam();

		// ���� ó�� ������ ������ ������ unique id
		DWORD dwRootAttackerActorUniqueID = m_hRootAttacker->GetUniqueID();
		m_pPacketStream->Write( &dwRootAttackerActorUniqueID, sizeof(DWORD) );

		DWORD dwPrevAttackerActorUniqueID = m_ParentSkillInfo.hPrevAttacker ? m_ParentSkillInfo.hPrevAttacker->GetUniqueID() : UINT_MAX;
		m_pPacketStream->Write( &dwPrevAttackerActorUniqueID, sizeof(DWORD) );

		// ������Ÿ�� �ñ׳��� Ŭ���̾�Ʈ���� ã�µ� �ʿ��� ���� ������.
		//m_pPacketStream->Write( &m_iShootActionIndex, sizeof(int) );
		//m_pPacketStream->Write( &m_iProjectileSignalArrayIndex, sizeof(int) );
		m_pPacketStream->Write( &m_ParentSkillInfo.iProjectileShootActionIndex, sizeof(int) );
		m_pPacketStream->Write( &m_ParentSkillInfo.iProjectileSignalArrayIndex, sizeof(int) );

	//	m_bWroteAdditionalPacket = true;
	//}
}
#else
void CDnChainAttackBlow::ReadyForChainAttack( DWORD dwRootAttackerActorID, DWORD dwPrevAttackerActorID, int iActionIndex, int iProjectileSignalArrayIndex )
{
	// ���͸� ã�� �׼��� ã�� ������Ÿ�� �ε����� ã�Ƽ� ������Ÿ�� �ñ׳� ���� ����.
	m_hRootAttacker = CDnActor::FindActorFromUniqueID( dwRootAttackerActorID );
	m_hPrevAttacker = CDnActor::FindActorFromUniqueID( dwPrevAttackerActorID );
	if( m_hRootAttacker )
	{
		bool bFound = false;

		/*
		CEtActionBase::ActionElementStruct* pActionElement = m_hRootAttacker->GetElement( iActionIndex );
		DWORD dwNumSignalList = (DWORD)pActionElement->pVecSignalList.size();
		for( DWORD dwSignal = 0; dwSignal < dwNumSignalList; ++dwSignal )
		{
			CEtActionSignal* pSignal = pActionElement->pVecSignalList.at( dwSignal );
			if( pSignal->GetSignalListArrayIndex() == iProjectileSignalArrayIndex )
			{
				m_ProjectileSignalInfo = *(static_cast<ProjectileStruct*>(pSignal->GetData()));
				bFound = true;
				break;
			}
		}
		*/
		CEtActionSignal *pSignal = m_hRootAttacker->GetSignal( iActionIndex, iProjectileSignalArrayIndex );
		if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) {
#ifdef PRE_FIX_MEMOPT_SIGNALH
			CopyShallow_ProjectileStruct(m_ProjectileSignalInfo, static_cast<ProjectileStruct*>(pSignal->GetData()));
#else
			m_ProjectileSignalInfo = *(static_cast<ProjectileStruct*>(pSignal->GetData()));
#endif
			bFound = true;
		}

		_ASSERT( bFound && "CDnChainAttackBlow::SetProjectileSignal() -> ������Ÿ�� �ñ׳��� ã�� �� ����. ���� Ŭ�� �׼������� �ٸ� �� �ֽ��ϴ�!" );
		if( false == bFound )
		{
			SetState( STATE_BLOW::STATE_END );
			OutputDebug( "CDnChainAttackBlow::SetProjectileSignal Failed!! - Can't found Projectile Signal\n" );
		}
	}
	else
	{
		// ó�� ������ ���͸� ã�� �������Ƿ� ����ȿ�� ����.
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnChainAttackBlow::SetProjectileSignal Failed!! - Can't found Root Attacker Actor Handle\n" );
	}
}

void CDnChainAttackBlow::OnReceiveAddPacket( CPacketCompressStream& PacketStream )
{
	// ü�� ���� ����ȿ����� �߰������� ���ƿ� ��Ŷ�� ó���Ѵ�.
	DWORD dwRootAttackerActorUniqueID = UINT_MAX;
	DWORD dwPrevAttackerActorUniqueID = UINT_MAX;
	int iShootActionIndex = -1;
	int iProjectileSignalArrayIndex = -1;

	PacketStream.Read( &dwRootAttackerActorUniqueID, sizeof(DWORD) );
	PacketStream.Read( &dwPrevAttackerActorUniqueID, sizeof(DWORD) );
	PacketStream.Read( &iShootActionIndex, sizeof(int) );
	PacketStream.Read( &iProjectileSignalArrayIndex, sizeof(int) );

	ReadyForChainAttack( dwRootAttackerActorUniqueID, dwPrevAttackerActorUniqueID, 
						 iShootActionIndex, iProjectileSignalArrayIndex );
}

#endif

#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChainAttackBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� ����
	float fRange[2] = {0.0f, };
	int iMaxCount[2] = {0, };
	float fHitPercent[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) {
		fRange[0] = (float)atof( vlTokens[0][0].c_str() );
		iMaxCount[0] = atoi( vlTokens[0][1].c_str() );
		fHitPercent[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�.
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) {
		fRange[1] = (float)atof( vlTokens[1][0].c_str() );
		iMaxCount[1] = atoi( vlTokens[1][1].c_str() );
		fHitPercent[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultRange = max(fRange[0], fRange[1]);
	int iResultMaxCount = iMaxCount[0] + iMaxCount[1];
	float fResultHitPercent = fHitPercent[0] + fHitPercent[1];

	sprintf_s(szBuff, "%f;%d;%f", fResultRange, iResultMaxCount, fResultHitPercent);
	
	szNewValue = szBuff;
}

void CDnChainAttackBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� ����
	float fRange[2] = {0.0f, };
	int iMaxCount[2] = {0, };
	float fHitPercent[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) {
		fRange[0] = (float)atof( vlTokens[0][0].c_str() );
		iMaxCount[0] = atoi( vlTokens[0][1].c_str() );
		fHitPercent[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�.
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) {
		fRange[1] = (float)atof( vlTokens[1][0].c_str() );
		iMaxCount[1] = atoi( vlTokens[1][1].c_str() );
		fHitPercent[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultRange = min(fRange[0], fRange[1]);
	int iResultMaxCount = iMaxCount[0] - iMaxCount[1];
	float fResultHitPercent = fHitPercent[0] - fHitPercent[1];

	sprintf_s(szBuff, "%f;%d;%f", fResultRange, iResultMaxCount, fResultHitPercent);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
