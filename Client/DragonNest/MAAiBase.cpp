#include "StdAfx.h"
#include "MAAiBase.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MATransAction.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


MAAiBase::MAAiBase( DnActorHandle hActor, MAAiReceiver *pReceiver )
{
	m_hActor = hActor;
	m_pReceiver = pReceiver;

	m_PrevLocalTime = 0;
	m_nThreatRange = 0;
	m_nThreatAggro = 0;
	m_CognizanceGentleRange = 0;
	m_CognizanceThreatRange = 0;

	ResetAILook();
}

MAAiBase::~MAAiBase()
{

}

void MAAiBase::ResetAILook()
{
	m_bIsAILook			= false;
	m_vAILook			= EtVector2( 0.f, 0.f );
	m_vProjectileTarget	= EtVector3( 0.f, 0.f, 0.f );
}

bool MAAiBase::bIsProjectileTargetSignal()
{
	if( m_bIsAILook )
		return ( EtVec3LengthSq( &m_vProjectileTarget ) > 0.f ) ? true : false;

	return false;
}

bool MAAiBase::Initialize()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
	if( !pSox ) return false;
	int nItemID = 0;
	CDnMonsterActor *pActor = dynamic_cast<CDnMonsterActor *>(m_hActor.GetPointer());
	if( pActor ) nItemID = pActor->GetMonsterClassID();
	if( nItemID < 1 ) return false;

	m_nThreatRange = pSox->GetFieldFromLablePtr( nItemID, "_ThreatRange" )->GetInteger();
	m_nThreatAggro = pSox->GetFieldFromLablePtr( nItemID, "_ThreatAggro" )->GetInteger();
	m_CognizanceGentleRange = pSox->GetFieldFromLablePtr( nItemID, "_CognizanceGentleRange" )->GetInteger();
	m_CognizanceThreatRange = pSox->GetFieldFromLablePtr( nItemID, "_CognizanceThreatRange" )->GetInteger();

	return true;
}

void MAAiBase::ProcessDelay( LOCAL_TIME LocalTime )
{
	if( m_PrevLocalTime == 0 ) m_PrevLocalTime = LocalTime;
	for( DWORD i=0; i<m_nVecDelay.size(); i++ ) {
		if( m_nVecDelay[i] > 0 ) {
			m_nVecDelay[i] -= ( LocalTime - m_PrevLocalTime );
			if( m_nVecDelay[i] < 0 ) m_nVecDelay[i] = 0;
		}
	}
	m_PrevLocalTime = LocalTime;
}

void MAAiBase::SetDelay( DWORD dwSlot, int nDelay )
{
	if( dwSlot >= m_nVecDelay.size() ) {
		DWORD dwCount = ( dwSlot - m_nVecDelay.size() ) + 1;
		for( DWORD i=0; i<dwCount; i++ )
			m_nVecDelay.push_back( (int)0 );
	}
	m_nVecDelay[dwSlot] = nDelay;
}

bool MAAiBase::IsDelay( DWORD dwSlot )
{
	if( dwSlot >= m_nVecDelay.size() ) return false;

	if( m_nVecDelay[dwSlot] > 0 ) return true;

	return false;
}

void MAAiBase::Process( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessDelay( LocalTime );
	ProcessAggro();
}

void MAAiBase::OnChangeAction( const char* szPrevAction )
{
	if( bIsAILook() && !m_strAILookAction.empty() )
	{
		if( strcmp( m_strAILookAction.c_str(), szPrevAction ) == 0 )
		{
			ResetAILook();
			m_strAILookAction.clear();
		}
		else
		{
			return;
		}
	}

	if( bIsAILook() && !m_hActor->IsProcessSkill() )
		ResetAILook();
}

void MAAiBase::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
{
	if( bIsAILook() && !m_strAILookAction.empty() )
	{
		if( strcmp( m_strAILookAction.c_str(), szPrevAction ) == 0 )
		{
			ResetAILook();
			m_strAILookAction.clear();
		}
	}
}

void MAAiBase::ProcessAggro()
{
	std::vector<DnActorHandle> hEnemyList;
	GetEnemyList( hEnemyList );

	// 거리에 따라 어그로 리스트에 추가할지 뺄지 체크한다.
	for( DWORD i=0; i<hEnemyList.size(); i++ ) {
		if( hEnemyList[i]->IsDie() ) continue;
		float fDist = EtVec3Length( &( *m_hActor->GetPosition() - *hEnemyList[i]->GetPosition() ) );
		if( !GetAggroStruct( hEnemyList[i] ) ) {
			if( fDist < GetThreatRange() )
				AddAggro( hEnemyList[i], m_nThreatAggro );
		}
		else {
			if( fDist >= GetThreatRange() )
				RemoveAggro( hEnemyList[i] );
		}
	}
}

void MAAiBase::GetEnemyList( std::vector<DnActorHandle> &hVecList )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pTask ) return;

	hVecList.push_back( pTask->GetLocalPlayerActor() );
	for( DWORD i=0; i<pTask->GetConnectUserCount(); i++ ) {
		hVecList.push_back( pTask->GetConnectUserData(i)->hActor );
	}
}

DnActorHandle MAAiBase::GetAggroTarget()
{
	int nMaxAggro = 0;
	int nIndex = -1;
	for( DWORD i=0; i<m_VecAggroList.size(); i++ )  {
		if( m_VecAggroList[i].nAggro > nMaxAggro ) {
			nIndex = i;
			nMaxAggro = m_VecAggroList[i].nAggro;
		}
	}
	if( nIndex == -1 ) return CDnActor::Identity();
	return m_VecAggroList[nIndex].hActor;
}

void MAAiBase::OnDamageAggro( DnActorHandle hActor, CDnActor::SHitParam &HitParam, int nDamage )
{
	int nAggroValue = nDamage * 2;
	if( nAggroValue < 1 ) nAggroValue = 1;
	AddAggro( hActor, nAggroValue );

	RefreshAggroTarget();
}

void MAAiBase::RefreshAggroTarget()
{
	DnActorHandle hTarget = GetAggroTarget();
	if( hTarget != m_hTarget ) m_hTarget.Identity();
}

void MAAiBase::AddAggro( DnActorHandle hActor, int nAggro )
{
	AggroStruct *pStruct = GetAggroStruct( hActor );
	if( pStruct ) pStruct->nAggro += nAggro;
	else {
		m_VecAggroList.push_back( AggroStruct( hActor, nAggro ) );
	}
}

void MAAiBase::RemoveAggro( DnActorHandle hActor )
{
	for( DWORD i=0; i<m_VecAggroList.size(); i++ ) {
		if( m_VecAggroList[i].hActor == hActor ) {
			m_VecAggroList.erase( m_VecAggroList.begin() + i ) ;
			return;
		}
	}
}

MAAiBase::AggroStruct *MAAiBase::GetAggroStruct( DnActorHandle hActor )
{
	for( DWORD i=0; i<m_VecAggroList.size(); i++ ) {
		if( m_VecAggroList[i].hActor == hActor ) return &m_VecAggroList[i];
	}
	return NULL;
}

void MAAiBase::SetTarget( DnActorHandle hActor )
{
	if( m_hTarget == hActor ) return;

	m_hTarget = hActor;
}

void MAAiBase::SetProjectileTarget()
{
	if( m_hTarget )
	{
		m_bIsAILook			= true;

#ifdef PRE_FIX_PARTSMONSTER_AI_TARGETTING
		m_vAILook = EtVec3toVec2( m_hTarget->FindAutoTargetPos() - *m_hActor->GetPosition() );
		m_vProjectileTarget	= m_hTarget->FindAutoTargetPos();
#else
		m_vAILook = EtVec3toVec2( *m_hTarget->GetPosition() - *m_hActor->GetPosition() );
		m_vProjectileTarget	= *m_hTarget->GetPosition();
#endif
		EtVec2Normalize( &m_vAILook, &m_vAILook );
	}
}
