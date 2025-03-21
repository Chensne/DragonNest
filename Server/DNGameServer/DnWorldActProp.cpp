#include "StdAfx.h"
#include "DnWorldActProp.h"
#include "EtMatrixEx.h"
#include "DnWorldSector.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "PerfCheck.h"

STATIC_DECL_INIT( CDnWorldActProp, DWORD, s_dwCreatePropCount ) = { 0, };

const LOCAL_TIME DONT_HAVE_LIFETIME = -1;

CDnWorldActProp::CDnWorldActProp( CMultiRoom *pRoom )
: CDnWorldProp( pRoom )
, m_pFSM( new TDnFiniteStateMachine<DnPropHandle>(GetMySmartPtr()) )
, m_LifeTime( DONT_HAVE_LIFETIME )
, m_PrevLocalTime( 0 )
//, m_bProcessAllowed( false )
{
	EtMatrixIdentity( &m_matWorld );
	CDnActionBase::Initialize( this );

	m_bDestroy = false;
//	m_bInstantUpdate = false;

	++STATIC_INSTANCE_(s_dwCreatePropCount);
//	m_bProcessAllowed = ((STATIC_INSTANCE_(s_dwCreatePropCount) % 2) == 0) ? true : false;
}

CDnWorldActProp::~CDnWorldActProp()
{
	SAFE_DELETE( m_pFSM );
	SAFE_RELEASE_SPTR( m_Handle );
}

bool CDnWorldActProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	//m_pFSM->Initialize();

	bool bResult = CDnWorldProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult == false ) return false;

	CDnWorldSector *pSector = static_cast<CDnWorldSector *>(m_pParentSector);
	if( pSector ) pSector->InsertProcessProp( this );

	return true;
}

bool CDnWorldActProp::InitializeTable( int nTableID )
{
	if( CDnWorldProp::InitializeTable( nTableID ) == false ) return false;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );

	m_szAniName = pSox->GetFieldFromLablePtr( nTableID, "_AniName" )->GetString();
	m_szActName = pSox->GetFieldFromLablePtr( nTableID, "_ActName" )->GetString();
	char *szStr = pSox->GetFieldFromLablePtr( nTableID, "_DefaultAction" )->GetString();

	int nActionCount = 1 + _GetNumSubStr( szStr, ';' );
	if( nActionCount > 0 ) {
		m_szDefaultActionName = _GetSubStrByCountSafe( _rand(GetRoom())%nActionCount, szStr, ';' );
	}

	return true;
}

bool CDnWorldActProp::CreateObject()
{
	m_Handle = EternityEngine::CreateAniObject( GetRoom(), GetPropName(), ( m_szAniName.empty() ) ? NULL : m_szAniName.c_str() );

	if( !m_szActName.empty() ) {
		if( LoadAction( CEtResourceMng::GetInstance().GetFullName( m_szActName ).c_str() ) == false ) return false;
	}

	if( m_Handle ) {
		m_Handle->SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
	}

	if( !m_Handle ) return false;

	if( GetElementIndex( m_szDefaultActionName.c_str() ) != -1 )
		SetAction( m_szDefaultActionName.c_str(), 0.f, 0.f );

	return true;
}

void CDnWorldActProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	/*
	m_bProcessAllowed = !m_bProcessAllowed;

	if( false == m_bProcessAllowed )
		return;
	*/

	CDnActionBase::ProcessAction( LocalTime, fDelta );
	Process( &m_matWorld, LocalTime, fDelta );

	// 라이프타임 있는 경우만.
	if( m_LifeTime != DONT_HAVE_LIFETIME )
	{
		// 로컬 타임 기준으로 해야 함.
		// delta 값이 프랍 쪽에 3번에 한번씩 process 돌기 때문에 정확한 값이 오는 게 아님.
		LOCAL_TIME Delta = 0;
		if( 0 == m_PrevLocalTime )
			Delta = LOCAL_TIME(fDelta * 1000.0f);
		else
			Delta = LocalTime - m_PrevLocalTime;

		m_PrevLocalTime = LocalTime;

		if( m_LifeTime < Delta )
		{
			_OnLifeTimeEnd( LocalTime, fDelta );
			m_LifeTime = DONT_HAVE_LIFETIME;
		}
		else
			m_LifeTime -= Delta;
	}

	m_pFSM->Process( LocalTime, fDelta );
}

void CDnWorldActProp::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_Handle ) return;
	if( !m_Handle->IsCalcAni() ) return;
	if( m_nActionIndex == -1 ) return;
	if( m_nActionIndex >= (int)m_nVecAniIndexList.size() ) return;
	if( m_nVecAniIndexList[m_nActionIndex] == -1 ) return;

	m_Handle->SetAniFrame( m_nVecAniIndexList[m_nActionIndex], m_fFrame );
//	if( m_bInstantUpdate )
//	{
	if( !m_bIsStaticCollision ) m_Handle->Update( pmatWorld );
//	}
}

int CDnWorldActProp::GetAniIndex( const char *szAniName )
{
	if( !m_Handle ) return -1;
	for( int i=0; i<m_Handle->GetAniCount(); i++ ) {
		if( strcmp( m_Handle->GetAniName(i), szAniName ) == NULL ) return i;
	}
	return -1;
}

void CDnWorldActProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Destroy:
			{
				m_bDestroy = true;
			}
			break;
		case STE_EnableCollision:
			{
				EnableCollisionStruct *pStruct = (EnableCollisionStruct *)pPtr;
				if( !m_Handle ) break;

				EnableCollision( pStruct->bEnable ? true : false );
			}
			break;
	}
}

// pSession == NULL 인 경우 모두에게 패킷을 보내고 pSession != NULL 인 경우에는 pSession 에게만 패킷을 보낸다.
void CDnWorldActProp::CmdAction( const char *szActionName, CDNUserSession* pSession/*=NULL*/, int nLoopCount/*=0*/, float fBlendFrame/*=3.f*/ )
{
	SetActionQueue( szActionName, nLoopCount, fBlendFrame );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nActionIndex = GetElementIndex( szActionName );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

	if( pSession )
		Send( eProp::SC_CMDACTION, &Stream, pSession );
	else
		Send( eProp::SC_CMDACTION, &Stream );
}

void CDnWorldActProp::CmdAction( DnActorHandle hActor, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	SetActionQueue( szActionName, nLoopCount, fBlendFrame );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nActionIndex = GetElementIndex( szActionName );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

	Send( eProp::SC_CMDACTION, hActor, &Stream );
}


void CDnWorldActProp::ActionSync( const char *szActionName, CDNUserSession* pSession/*=NULL*/, int nLoopCount/*=0*/, float fBlendFrame/*=3.f*/ )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nActionIndex = GetElementIndex( szActionName );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

	if( pSession )
		Send( eProp::SC_CMDACTION, &Stream, pSession );
	else
		Send( eProp::SC_CMDACTION, &Stream );
}


void CDnWorldActProp::OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ )
{
	if( !pBreakIntoGameSession )
		return;

	CmdAction( GetCurrentAction(), pBreakIntoGameSession );
}

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
void CDnWorldActProp::OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol )
	{
		case eProp::CS_QUERY_RANDOM_CREATED:
			{
				//BYTE pBuffer[32];
				//CPacketCompressStream Stream( pBuffer, 32 );
				//Stream.Write( &m_bRandomResultIsVisible, sizeof(bool) );
				//Send( eProp::SC_CMDENABLEOPERATOR, pSession->GetActorHandle(), &Stream );

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
				CmdShow(m_VisibleOptions[PropDef::Option::eRandomCreate] != PropDef::Option::False);
#else
				CmdShow( m_bRandomResultIsVisible );
#endif

				BYTE pBuffer[ 32 ] = { 0 };
				CPacketCompressStream Stream( pBuffer, 32 );
#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
				Stream.Write( &m_VisibleOptions[PropDef::Option::eRandomCreate], sizeof(PropDef::Option::eValue) );
#else
				Stream.Write( &m_bRandomResultIsVisible, sizeof(bool) );
#endif
				Send( eProp::SC_QUERY_RANDOM_CREATED, &Stream );
			}
			break;
	};

	__super::OnDispatchMessage( pSession, dwProtocol, pPacket );
}
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE