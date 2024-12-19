#include "StdAfx.h"
#include "DnCelberosMonsterActor.h"
#include "DnActorClassDefine.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MAAiBase.h"
#include "DnBlow.h"

CDnCelberosMonsterActor::CDnCelberosMonsterActor( CMultiRoom *pRoom, int nClassID )
:  CDnPartsMonsterActor( pRoom, nClassID )
{
	m_fShakeDelta	= 0.f;
	m_HitCheckType	= HitCheckTypeEnum::Collision;

	int i = 0;
	for (; i < MaxBufCount; ++i)
		m_bBufDieArray[i] = false;
}

CDnCelberosMonsterActor::~CDnCelberosMonsterActor()
{
}


bool CDnCelberosMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();

	if( m_hObject )
	{
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
		m_hObject->SetTargetCollisionGroup( 0 );
	}

	return true;
}

void CDnCelberosMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fValue = 1.f;
	if( m_fShakeDelta > 0.f ) 
	{
		m_fShakeDelta -= fDelta;
		if( m_fShakeDelta <= 0.f ) 
			m_fShakeDelta = 0.f;
		fValue = 1.f - ( ( _rand(m_hActor->GetRoom())%100 ) * 0.0005f );
	}
	MAActorRenderBase::SetScale( EtVector3( GetScale() * fValue, GetScale(), GetScale() * fValue ) );

	CDnPartsMonsterActor::Process( LocalTime, fDelta );

	/*
	if( !IsDie() ) 
	{
		std::vector<DnActorHandle> hVecList;
		EtVector3 vAniDist = *GetPosition();
		vAniDist.x -= GetAniDistance()->x;
		vAniDist.z -= GetAniDistance()->z;

		int nCount =  ScanActor( GetRoom(), vAniDist, (float)GetUnitSize(), hVecList );
		if( nCount > 0 ) 
		{
			EtVector2 vDir;
			int nSumSize;
			for( int i=0; i<nCount; i++ ) 
			{
				if( hVecList[i] == GetMySmartPtr() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				vDir = EtVec3toVec2( *hVecList[i]->GetPosition() - *GetPosition() );
				nSumSize = GetUnitSize() + hVecList[i]->GetUnitSize();
				float fLength = D3DXVec2Length( &vDir );
				if( fLength > (float)nSumSize ) continue;

				D3DXVec2Normalize( &vDir, &vDir );
				MAMovementBase *pMovement = dynamic_cast<MAMovementBase *>(hVecList[i].GetPointer());
				if( pMovement ) pMovement->MoveToWorld( vDir * (float)( nSumSize - fLength ) );
			}
		}
	}
	*/
}


void CDnCelberosMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	m_fShakeDelta = 0.3f;
	CDnPartsMonsterActor::OnDamage( pHitter, HitParam, pHitStruct );
}

bool CDnCelberosMonsterActor::CheckDamageAction( DnActorHandle hActor )
{
	return true;
}

void CDnCelberosMonsterActor::OnDrop( float fCurVelocity )
{
	
}

bool CDnCelberosMonsterActor::OnPreAiProcess(const char* szPrevAction, LOCAL_TIME time)
{
	if( strstr( szPrevAction, "Dash_End" ) )
		return true;

	return false;
}

void CDnCelberosMonsterActor::OnFinishAction(const char* szPrevAction, LOCAL_TIME time)
{
	if( strstr( szPrevAction, "Dash_End" ) )
	{		
		EtVector2 vView = EtVec3toVec2( *GetLookDir() );
		vView *= -1.f;
		Look( vView, true );
	}

	CDnMonsterActor::OnFinishAction( szPrevAction, time );
}

bool CDnCelberosMonsterActor::OnbIsCalcDamage( const int iSkillIndex )
{
	if( iSkillIndex == 0 )
		return true;

	for( int i=0 ; i<m_hActor->GetNumAppliedStateBlow() ; ++i )
	{
		DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( i );
		if( hBlow )
		{
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if( !pSkillInfo )
				continue;

			if( pSkillInfo->iSkillID == iSkillIndex )
				return true;
		}
	}

	return false;
}

void CDnCelberosMonsterActor::OnChangeActionQueue( const char *szPrevAction )
{
	//CmdMixPartsDeadAction();
	return;
	/*
	Capsule01 => Bone13       cerberos_R Stun_Ice	IceBuf
	Capsule02 => Bip01 Head   cerberos_C Stun_Dark	DarkBuf
	Capsule03 => Bone09       cerberos_L Stun_Fire	FireBuf
	*/
	// 각각의 머리가 죽었을 경우에는 고개를 숙이게 애니를 섞어준다.
	
	/*
	if ( m_StateBuf[IceBuf].bDie )
	{
		CmdMixedAction("Bone13", "Bip01 Spine", "Stun_Ice", 0,3.0f);

	}

	if ( m_StateBuf[FireBuf].bDie )
	{
		CmdMixedAction("Bone09", "Bip01 Spine", "Stun_Fire", 0,3.0f);
	}

	if ( m_StateBuf[DarkBuf].bDie )
	{
		CmdMixedAction("Bip01 Head", "Bip01 Spine", "Stun_Dark", 0,3.0f);
	}
	*/
}

void CDnCelberosMonsterActor::CmdMixPartsDeadAction()
{
	int i = 0;
	for (; i < MaxBufCount; ++i)
	{
		if (m_bBufDieArray[i])
		{
			CmdMixPartsDeadAction((StateBuf)i);
			//m_bBufDieArray[i] = false;
		}
	}
}

void CDnCelberosMonsterActor::CmdMixPartsDeadAction(StateBuf state)
{
	std::map<StateBuf, SDeadActorActionInfo>::const_iterator iter = m_DeadActionInfoList.find(state);
	if (iter != m_DeadActionInfoList.end())
	{
		const SDeadActorActionInfo& info = (*iter).second;
		SetActionQueue(info.names[ActionName].c_str());
	}
}

CDnCelberosMonsterActor::StateBuf CDnCelberosMonsterActor::GetBufStateByPartsID(int partNum) const
{
	switch(partNum)
	{
	case 4:	return IceBuf;		// 캘베로스 머리R
	case 5:	return DarkBuf;		// 캘베로스 머리C
	case 6: return FireBuf;		// 캘베로스 머리L
	}

	return MaxBufCount;
}

void CDnCelberosMonsterActor::_OnAddMonsterParts(const MonsterParts::_Info& partsInfo)
{
	CDnPartsMonsterActor::_OnAddMonsterParts(partsInfo);

	SDeadActorActionInfo deadPartsActionInfo;
	StateBuf key = GetBufStateByPartsID(partsInfo.uiMonsterPartsTableID);
	deadPartsActionInfo.Set(partsInfo.szDeadActorActName, partsInfo.szDeadActorActBoneName, partsInfo.szDeadActorFixBoneName);

	if (key != MaxBufCount)
		m_DeadActionInfoList.insert(std::make_pair(key, deadPartsActionInfo));
}
void CDnCelberosMonsterActor::_OnDestroyParts(MonsterParts* pParts)
{
	CDnPartsMonsterActor::_OnDestroyParts(pParts);

	StateBuf state = GetBufStateByPartsID(pParts->GetPartsTableID());
	if (state >= MaxBufCount)
		return;

	m_bBufDieArray[state] = true;
	CmdMixPartsDeadAction();
}

void CDnCelberosMonsterActor::_OnRefreshParts( MonsterParts* pParts )
{
	CDnPartsMonsterActor::_OnRefreshParts(pParts);

	StateBuf state = GetBufStateByPartsID(pParts->GetPartsTableID());
	if (state >= MaxBufCount)
		return;

	m_bBufDieArray[state] = false;
	CmdMixPartsDeadAction();
}