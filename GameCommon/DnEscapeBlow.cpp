#include "StdAfx.h"
#include "DnEscapeBlow.h"

#include "DnCantMoveBlow.h"
#include "DnCantActionBlow.h"
#include "DnTableDB.h"
#include "DnFrameStopBlow.h"
#include "DnMonsterActor.h"

#ifdef _GAMESERVER
#include "DnGameRoom.h"
#include "DnGameDataManager.h"
#include "DnUserSession.h"
#else
#include "DnEtcObject.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "navigationmesh.h"
#include "InputWrapper.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

const int ICE_DAMAGE_ADD_PROBABILITY = 30;
const LOCAL_TIME SHAKE_TIME = 3000;


CDnEscapeBlow::CDnEscapeBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
m_pCantMoveBlow( new CDnCantMoveBlow(hActor, NULL ) ),
m_pCantActionBlow( new CDnCantActionBlow(hActor, NULL) ),
m_pFrameStopBlow( new CDnFrameStopBlow(hActor, NULL) ),
//m_iDurabilityCount( 1 ),
m_dwOriSpeedFrame( 0 ),
m_bNestMap( false )
#ifndef _GAMESERVER
,m_ComboCalc( CDnComboCalculator::CIRCULAR_CHECK )
,m_bShake( false )
,m_ShakeStartTime( 0 )
#endif
,m_bPlayerCharacter( false )
,m_bAllowShakeControl(false)
,m_IntervalChecker( hActor, GetMySmartPtr() )
,m_isAbleIntervalChecker(false)
{
	static int sValueType = 1;

	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_218;
	SetValue( szValue );

	std::string str = szValue;//"확률;내구도;(0/1)흔들림여부;피격액션";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	bool bValidValues = false;

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if (tokens.size() >= 3)
	{
		m_fValue = (float)atof(tokens[0].c_str());
//		m_iDurabilityCount = atoi(tokens[1].c_str());

		m_bAllowShakeControl = atoi(tokens[1].c_str()) == 0; //0 : 흔들림, 1 : 흔들리지 않음

		//마지막 피격 액션은 옵션 사항
		if (tokens.size() == 3)
		{
			m_strHitedAction = tokens[2];
		}

		bValidValues = true;
	}
	else
	{
		m_fValue = 0.0f;
//		m_iDurabilityCount = 2;

		bValidValues = false;
	}

	if (bValidValues)
	{
#ifndef _GAMESERVER
		std::vector<BYTE> vlKeysToCheck;
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );

		m_ComboCalc.SetKeysToCheck( vlKeysToCheck );

		vlKeysToCheck.clear();
		vlKeysToCheck.push_back( IW_MOVELEFT );		vlKeysToCheck.push_back( IW_MOVELEFT );		vlKeysToCheck.push_back( IW_MOVELEFT );
		vlKeysToCheck.push_back( IW_MOVERIGHT );	vlKeysToCheck.push_back( IW_MOVERIGHT );	vlKeysToCheck.push_back( IW_MOVERIGHT );
		vlKeysToCheck.push_back( IW_MOVEBACK );		vlKeysToCheck.push_back( IW_MOVEBACK );		vlKeysToCheck.push_back( IW_MOVEBACK );
		vlKeysToCheck.push_back( IW_MOVEFRONT );	vlKeysToCheck.push_back( IW_MOVEFRONT );	vlKeysToCheck.push_back( IW_MOVEFRONT );

		m_ComboCalc.SetPadsToCheck( vlKeysToCheck, 3.0f );
		
		UseTableDefinedGraphicEffect( false );
		if (m_pCantMoveBlow)
			m_pCantMoveBlow->UseTableDefinedGraphicEffect( false );

		m_bPlayerCharacter = dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) ? true : false;
#endif

		//AddCallBackType( SB_ONCALCDAMAGE );

		//피격 액션이 없을때는 기존 Freezing과 동일하게..
		if (m_pCantActionBlow)
		{
			//노멀 상태(isMoveable)에서도 피격 액션 설정을 위해(m_strHitedAction.empty() ? false : true)
			m_pCantActionBlow->SetAvailableNormalState(!m_strHitedAction.empty());

			m_pCantActionBlow->SetActionWhenCancelAttack( m_strHitedAction.empty() ? "Freezing" : m_strHitedAction.c_str() );
		}

	}

#ifdef _GAMESERVER
	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(m_hActor->GetRoom());
	if( false == pGameRoom->bIsPvPRoom() )
	{
		UINT uiSessionID = 0;
		pGameRoom->GetLeaderSessionID( uiSessionID );
		CDNUserSession *pUserSession = pGameRoom ? pGameRoom->GetUserSession(uiSessionID) : NULL;
		if( pUserSession )
		{
			const TMapInfo* pMapData = g_pDataManager->GetMapInfo( pUserSession->GetMapIndex() );
			if( pMapData )
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				m_bNestMap = CDnBlow::CheckEffectIgnoreMapType(pMapData->MapType, pMapData->MapSubType);
#else	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				m_bNestMap = (GlobalEnum::MAP_DUNGEON == pMapData->MapType) && 
							 ((GlobalEnum::MAPSUB_NEST == pMapData->MapSubType) ||(GlobalEnum::MAPSUB_NESTNORMAL ==  pMapData->MapSubType));
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
		}
	}
#else
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
	m_bNestMap = CDnBlow::CheckEffectIgnoreMapType();
#else	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
	CDnWorld* pWorld = CDnWorld::GetInstancePtr();
	m_bNestMap = (CDnWorld::MapTypeEnum::MapTypeDungeon == pWorld->GetMapType()) && 
				 ((CDnWorld::MapSubTypeEnum::MapSubTypeNest == pWorld->GetMapSubType() || CDnWorld::MapSubTypeEnum::MapSubTypeNestNormal == pWorld->GetMapSubType()));
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
#endif

	CheckBossMonster();

	m_fOrigWight = 0.0f;
	m_iOrigPressLevel = 0;

	// 상태효과가 상태를 가지고 있는 경우 계속 유지를 위해서
	if (m_pCantMoveBlow)
		m_pCantMoveBlow->SetPermanent(true);
	if (m_pCantActionBlow)
		m_pCantActionBlow->SetPermanent(true);
	if (m_pFrameStopBlow)
		m_pFrameStopBlow->SetPermanent(true);

}

CDnEscapeBlow::~CDnEscapeBlow(void)
{
	SAFE_DELETE( m_pCantMoveBlow );
	SAFE_DELETE( m_pCantActionBlow );
	SAFE_DELETE( m_pFrameStopBlow );
}


#ifdef _GAMESERVER
bool CDnEscapeBlow::CanBegin( void )
{
	bool bResult = true;

	int iProb = int(m_fValue * 10000.0f);
	if( _rand(GetRoom())%10000 > iProb )
	{
		// 호출한 쪽에서 CanBegin 호출하고 실패시 즉시 상태효과 삭제토록 변경.
		//SetState( STATE_BLOW::STATE_END );
		OutputDebug( "%s - Freezing Fail\n", __FUNCTION__ );
		bResult = false;
	}
	
	return bResult;
}
#endif


// 처음에 결빙이 걸릴 확률이 Value 로 셋팅되므로 확률에 맞지 않는다면 그대로 끝내버림
void CDnEscapeBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 결빙지속시간 = <효과지속시간> * (1 – 물속성내성*상태보정weight값)
	float fGlobalStateEffectWeight = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StateRevision );
	float fIceResistance = m_hActor->GetElementDefense(CDnState::Ice);
	
	// 지속시간을 내성에 따라 줄여준다.
	m_StateBlow.fDurationTime = m_StateBlow.fDurationTime * (1.0f - fIceResistance*fGlobalStateEffectWeight);


	// 행동불가, 이동불가
	if( m_pCantMoveBlow ) m_pCantMoveBlow->OnBegin( LocalTime, fDelta );
	if( m_pCantActionBlow ) m_pCantActionBlow->OnBegin( LocalTime, fDelta );

	// TODO: 이펙트 출력.. 클라에서만 해당됨
#ifndef _GAMESERVER
	m_vlBreakActions.clear();
	_AttachGraphicEffect();

	m_vlBreakActions.push_back( "Break" );

	// 플레이어가 걸렸을 경우에 버튼 연타 UI 보여줌
	if( m_bPlayerCharacter )
	{
		GetInterface().ShowStickAniDialog( true );
	}
#else
	// 몬스터 액터라면 이펙트 테이블에 정의되어있는 액션이 클라로부터 패킷이 오지 않기 때문에 (CmdAction 은 클라에서 패킷 안보냄)
	// 여기서 따로 처리해준다.
	if( m_hActor->IsMonsterActor() )
	{
		if (false == m_bIgnoreEffectAction)
		{
			if( false == m_bIgnoreEffectAction )
			{
				DNTableFileFormat* pStateEffectTable = CDnTableDB::GetInstancePtr()->GetTable( CDnTableDB::TSTATEEFFECT );

				std::vector<std::string> infoTokens;
				std::string delimiters = ";";

				TokenizeA(m_ParentSkillInfo.szEffectOutputIDs, infoTokens, delimiters);
				int nTokenSize = (int)infoTokens.size();
				for (int i = 0; i < nTokenSize; ++i)
				{
					int nEffectOutputID = atoi(infoTokens[i].c_str());
					if (IsMatchStateEffectIndex(nEffectOutputID))
					{
						string strActionName = pStateEffectTable->GetFieldFromLablePtr( nEffectOutputID, "_ActorActionName" )->GetString();
						m_hActor->SetActionQueue( strActionName.c_str() );
					}
				}
			}
		}
	}

	// 클라이언트에게도 이 상태효과의 지속시간을 방금 업데이트 된 것으로 바꿔준다. 
	// PVP 같은 보정테이블 값을 참조받는 경우 , 서버와 클라이언트의 동기가 틀어지는경우가 생긴다.
	m_hActor->CmdModifyStateEffect( GetBlowID(), m_StateBlow );

#endif

	//시작될때 Weight/PressLevel을 저장해 놓는다.
	m_fOrigWight = m_hActor->GetWeight();
	m_iOrigPressLevel = m_hActor->GetPressLevel();

	////캐릭터 밀림을 방지 하기 위해 설정.
	//m_hActor->SetWeight(0.0f);
	//m_hActor->SetPressLevel(-1);

	//피격 액션이 있으면 피격 액션이 끝나는 시점에서 멈추도록..
	if (!m_strHitedAction.empty())
	{
		//SAFE_DELETE(m_pFrameStopBlow);
		CEtActionBase::ActionElementStruct* pStruct = m_hActor->GetElement( m_strHitedAction.c_str() );
		if( pStruct )
		{
			m_isAbleIntervalChecker = true;
			//60frame기준으로
			float fFrameTime = 0.01666666666666666666666666666667f * 1000.0f;
			m_IntervalChecker.OnBegin( LocalTime, (DWORD)(pStruct->dwLength * fFrameTime));
		}
	}
	else
	if( m_pFrameStopBlow ) m_pFrameStopBlow->OnBegin( LocalTime, fDelta );

	OutputDebug( "%s - BlowID:%d, Ice Resistance: %2.2f, Result DurationTime: %2.2f\n", __FUNCTION__, GetBlowID(), fIceResistance, GetDurationTime() );
}



#ifndef _GAMESERVER
void CDnEscapeBlow::_ProcessShake( LOCAL_TIME LocalTime, float fDelta )
{
	LOCAL_TIME TimeGap = LocalTime - m_ShakeStartTime;
	if( TimeGap > SHAKE_TIME )
	{
		m_bShake = false;
		return;
	}

	float fShakeDelta = 0.0f;
	float fStartRatio = 0.2f;
	float fEndRatio = 0.2f;
	float fShakeValue = 3.0f;

	if( LocalTime < m_ShakeStartTime + ( TimeGap * fStartRatio ) ) 
	{
		fShakeDelta = 1.f / ( SHAKE_TIME * fStartRatio ) * ( TimeGap );
		fShakeDelta *= fShakeValue;
	}
	else if( LocalTime < m_ShakeStartTime + ( SHAKE_TIME * fEndRatio ) ) 
	{
		fShakeDelta = fShakeValue;
	}
	else
	{
		fShakeDelta = 1.f / ( SHAKE_TIME * fEndRatio ) * ( ( m_ShakeStartTime + SHAKE_TIME ) - LocalTime );
		fShakeDelta *= fShakeValue;
	}

	if( fShakeDelta > 0.1f ) 
	{
		//MatrixEx *pCross = m_hActor->GetMatEx();
		MatrixEx CrossMove = *m_hActor->GetMatEx();

		EtVector3 vPrevPos = CrossMove.m_vPosition;

		CrossMove.m_vPosition.x += -fShakeDelta + ( ( _rand()%(int)(fShakeDelta*20) ) / 10.f );
		CrossMove.m_vPosition.y += -fShakeDelta + ( ( _rand()%(int)(fShakeDelta*20) ) / 10.f );
		CrossMove.m_vPosition.z += -fShakeDelta + ( ( _rand()%(int)(fShakeDelta*20) ) / 10.f );

		NavigationMesh *pNavMesh = INSTANCE(CDnWorld).GetNavMesh( CrossMove.m_vPosition );
		if( !pNavMesh )
		{
			if( CDnWorld::GetInstance().GetAttribute( CrossMove.m_vPosition ) != 0 )
			{
				CrossMove.m_vPosition = vPrevPos;
			}
			CrossMove.m_vPosition.y = CDnWorld::GetInstance().GetHeight( CrossMove.m_vPosition );
		}
		else 
		{
			NavigationCell* pCurCell = NULL;
			if( pCurCell == NULL ) 
			{
				pCurCell = pNavMesh->FindClosestCell( CrossMove.m_vPosition );
				if( ( pCurCell == NULL ) || ( !pCurCell->IsPointInCellCollumn( CrossMove.m_vPosition ) ) ) 
				{
					pCurCell = NULL;

					//EtVector3 vPrevPos = Cross.m_vPosition;
					if( CDnWorld::GetInstance().GetAttribute( CrossMove.m_vPosition ) != 0 )
					{
						CrossMove.m_vPosition = vPrevPos;
					}
					CrossMove.m_vPosition.y = CDnWorld::GetInstance().GetHeight( CrossMove.m_vPosition );
				}
			}
			if( pCurCell )
			{
				//EtVector3 vPrevPos = Cross.m_vPosition;
				//Cross.MoveLocalZAxis( fValue );
				int nSide = -1;
				NavigationCell *pLastCell = NULL;
				pCurCell->FindLastCollision( vPrevPos, CrossMove.m_vPosition, &pLastCell, nSide );
				if( nSide != -1 )
				{
					if( pLastCell->Link( nSide ) == NULL )
					{
						EtVector2 vMoveDir2D( CrossMove.m_vPosition.x - vPrevPos.x, CrossMove.m_vPosition.z - vPrevPos.z );
						float fMoveLength = EtVec2Length( &vMoveDir2D );
						vMoveDir2D /= fMoveLength;
						EtVector2 vWallDir2D = pLastCell->Side( nSide )->EndPointB() - pLastCell->Side( nSide )->EndPointA();
						EtVec2Normalize( &vWallDir2D, &vWallDir2D );
						fMoveLength *= EtVec2Dot( &vWallDir2D, &vMoveDir2D );
						CrossMove.m_vPosition.x = vPrevPos.x + fMoveLength * vWallDir2D.x;
						CrossMove.m_vPosition.z = vPrevPos.z + fMoveLength * vWallDir2D.y;

						int nNewSide = -1;
						pCurCell->FindLastCollision( vPrevPos, CrossMove.m_vPosition, &pLastCell, nNewSide );
						if( nNewSide != -1 )
						{
							if( pLastCell->Link( nNewSide ) )
							{
								NavigationCell *pNewCell;
								pNewCell = pLastCell->Link( nNewSide );
								if( pNewCell->IsPointInCellCollumn( CrossMove.m_vPosition ) )
								{
									pCurCell = pNewCell;
								}
								else
								{
									CrossMove.m_vPosition.x = vPrevPos.x;
									CrossMove.m_vPosition.z = vPrevPos.z;
								}
							}
							else if( !pCurCell->IsPointInCellCollumn( CrossMove.m_vPosition ) )
							{
								CrossMove.m_vPosition.x = vPrevPos.x;
								CrossMove.m_vPosition.z = vPrevPos.z;
							}
						}
					}
					else
					{
						pCurCell = pLastCell->Link( nSide );
					}
				}

				if( pCurCell->GetType() == NavigationCell::CT_PROP )
					CrossMove.m_vPosition.y = pCurCell->GetPlane()->SolveForY( CrossMove.m_vPosition.x, CrossMove.m_vPosition.z );
				else
					CrossMove.m_vPosition.y = CDnWorld::GetInstance().GetHeight( CrossMove.m_vPosition );
			}
		}

		m_hActor->ProcessCollision( EtVector3(CrossMove.m_vPosition - vPrevPos) );
	}
}
#endif



// 결빙은 버튼 연타로 풀어낼 수 있다.
void CDnEscapeBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	//피격 동작이 설정 되어 있으면 피격 동작 끝날때 FrameStop상태효과를 활성화를 위해 time체크..
	if (m_isAbleIntervalChecker)
		m_IntervalChecker.Process( LocalTime, fDelta );

#ifndef _GAMESERVER
	if( CDnLocalPlayerActor::s_hLocalActor == m_hActor )
	{
		// TODO: 좌우 입력 받을 때 마다 시간 줄여줌. 결과적으로 서버로도 보내줘야 함...
		int iNumComboCount = 0;
		bool bComboSatisfy = m_ComboCalc.Process( LocalTime, fDelta, &iNumComboCount );

		for( int i = 0; i < iNumComboCount; ++i )
			m_StateBlow.fDurationTime -= 1.0f;

		if( bComboSatisfy )
		{
			m_bShake = true;
			m_ShakeStartTime = LocalTime;

			OutputDebug( "[위기탈출] %d 회 콤보 성공, 시간 %2.2f 로 줄어듬 \n", iNumComboCount, m_StateBlow.fDurationTime );
		}
	}

	if( m_bAllowShakeControl && m_bShake )
		_ProcessShake( LocalTime, fDelta );

	// 자기 자신의 플레이어 캐릭터만 위기 탈출로 시간을 줄일 수 있으므로 체크해서 서버로 상태효과 끝낸다고 보냄.
	if( CDnLocalPlayerActor::s_hLocalActor == m_hActor )
	{
		if( m_StateBlow.fDurationTime < 0.2f )
		{
			CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer());
			if (pLocalPlayerActor)
				pLocalPlayerActor->CmdRemoveStateEffectByServerBlowID( GetServerBlowID() );
		}
	}
#endif
	
	if( m_pCantMoveBlow ) m_pCantMoveBlow->Process( LocalTime, fDelta );
	if( m_pCantActionBlow ) m_pCantActionBlow->Process( LocalTime, fDelta );
}

bool CDnEscapeBlow::OnCustomIntervalProcess( void )
{
	m_IntervalChecker.OnEnd(0, 0.0f);

	if (m_pFrameStopBlow)
		m_pFrameStopBlow->OnBegin(m_LastLocalTime, 0.0f);

	m_isAbleIntervalChecker = false;

	return true;
}

void CDnEscapeBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pCantMoveBlow ) m_pCantMoveBlow->OnEnd( LocalTime, fDelta );
	if( m_pCantActionBlow ) m_pCantActionBlow->OnEnd( LocalTime, fDelta );
	if( m_pFrameStopBlow ) m_pFrameStopBlow->OnEnd( LocalTime, fDelta );

	//////////////////////////////////////////////////////////////////////////
	// #36294
	// 아이스 스피어 스킬 사용시 상태효과 중복 처리때문에 OnBegin되기 전에
	// OnEnd로 들어 오는 경우가 있음. 이때 OnBegin되기 전에 m_bIgnoreEffectAction가 false
	// 상태여서 중복처리 될때마다 OnEnd로 들어와서 SetActionQueue("Stand")호출됨.
	// CheckBossMonster()로 함수 빼내서 생성자에서 호출되도록 수정해서 
	// m_bIgnoreEffectAction 값 정상적으로 설정 되도록 수정함.
	//////////////////////////////////////////////////////////////////////////
	
	// 다운 상태가 아니라면 풀어준다.
	if( false == m_bIgnoreEffectAction )
	{
		// 이펙트에 지정된 액션을 계속 실행하고 있을 경우에 Stand 로 풀어주도록 한다.
#ifdef _GAMESERVER
		if( false == m_hActor->IsDown() )
#else
		// #37065 이펙트 정보에 액션 이름이 없거나 (퍼니셔 브라스의 묶기)
		// 이펙트 정보에 정의된 액션을 취하고 있는 경우 결빙 종료시에 Stand 액션 실행.
		if( NULL == m_pEffectOutputInfo ||
			m_pEffectOutputInfo->strActorActionName.empty() ||
			m_pEffectOutputInfo->strActorActionName == m_hActor->GetCurrentAction() )
#endif
		{
#if defined(PRE_FIX_50007)
			//이미 죽은 경우는 동작 바뀌지 않도록..
			const char *szCurrentAction = m_hActor->GetCurrentAction();

			//Die동작인 경우는 Stand로 전환 안되도록 한다.
			if (strstr(szCurrentAction, "Die") == NULL &&
				strstr(szCurrentAction, "Relieve") == NULL) //#53900
#endif // PRE_FIX_50007
				m_hActor->SetActionQueue( "Stand" );
		}
	}
	
#ifndef _GAMESERVER	
	// 플레이어가 걸렸을 경우에 버튼 연타 UI 보여줌
	if( dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) )
	{
		GetInterface().ShowStickAniDialog( false );
	}

	if( m_pEffectOutputInfo )
	{
		// #30527 결빙 이펙트도 각 본에 붙게 되었으므로 본에 붙어있는 이펙트들을 모두 찾아서 break 액션을 시켜준다.
		if( EffectOutputInfo::ATTACH == m_pEffectOutputInfo->iOutputType )
		{
			if( !m_vlBreakActions.empty() && m_hEtcObjectEffect )
				m_hEtcObjectEffect->SetActionQueue( m_vlBreakActions.at( _rand() % (int)m_vlBreakActions.size() ).c_str() );

			// 깨지는 액션 실행될 때는 링크 끊어줌
			TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *pStruct = 
				(TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *)m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetStruct( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, GetBlowID() );

			if( pStruct )
				pStruct->bLinkObject = false;
		}
		else
		if( EffectOutputInfo::DUMMY_BONE_ATTACH == m_pEffectOutputInfo->iOutputType )
		{
			int nNumBone = (int)m_pEffectOutputInfo->vlDummyBoneIndices.size();
			for( int nBone = 0; nBone < nNumBone; ++nBone )
			{
				if( m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, (m_nBlowID*100)+nBone ) ) 
				{
					// 깨지는 액션 실행될 때는 링크 끊어줌
					TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *pStruct = 
						(TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *)m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetStruct( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, (m_nBlowID*100)+nBone );
					if( pStruct )
						pStruct->bLinkObject = false;

					DnEtcHandle hEffectHandle = m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetSignalHandle( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, (m_nBlowID*100)+nBone );

					if( hEffectHandle )
					{
						if( hEffectHandle->IsExistAction( "Break" ) )
							hEffectHandle->SetActionQueue( "Break" );	// Destroy 시그널 반드시 존재해야 객체 소멸됨 
						else
							m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, (m_nBlowID*100)+nBone );
					}
					else 
						m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, (m_nBlowID*100)+nBone );
				}
			}
		}
	}

	// 효과 삭제... 실제로 이펙트를 없애는 건 아니고, 이펙트 자체는 destroy 시그널로 알아서 죽는다.
	// 하지만 상태효과는 이 시점에서 끝이므로 이펙트를 추가할 때 판단 근거가 되는 스킬 정보를 여기서 지워줘야 한다.
	// 그렇지 않으면 스킬 정보 계속 남아있어서 이펙트가 추가가 안됨.
	m_hActor->DetachSEEffectSkillInfo( m_ParentSkillInfo );
	_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	//m_hActor->DetachSEEffect( m_ParentSkillInfo, m_StateBlow.emBlowIndex, GetBlowID(), m_pEffectOutputInfo );
#else
#endif

	////상태가 끝날때 기존에 저장해놓은 값으로 복원
	//m_hActor->SetWeight(m_fOrigWight);
	//m_hActor->SetPressLevel(m_iOrigPressLevel);

	OutputDebug( "%s BlowID: %d\n", __FUNCTION__, GetBlowID() );
}



// #ifdef _GAMESERVER
// float CDnEscapeBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
// {
// 	float fResult = 0.0f;
// 
// 	--m_iDurabilityCount;
// 
// 	// 결빙상태에서는 데미지를 1.5배로 먹는다.
// 	fResult = fOriginalDamage / 2.0f;
// 
// 	// 결빙상태에서는 Freezing 이던 Stand 액션을 하고 있던 간에 맞았다고 hit 류 액션으로 바꾸지 않는다.
// 	// hit 류의 CanHit 가 false 이면 계속 맞지 않는 경우가 생긴다.  #15675
// 	HitParam.szActionName.clear();
// 
// 	// 막타라면 상태효과 종료.
// 	if( m_iDurabilityCount <= 0 )
// 	{
// 		SetState( STATE_BLOW::STATE_END );
// 		OutputDebug( "%s - Freezing Broken by Attack!\n", __FUNCTION__ );
// 	}
// 	
// 	return fResult;
// }
// #else
// float CDnEscapeBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
// {
// 	--m_iDurabilityCount;
// 
// 	// 마지막 타격일 경우엔 데미지를 두 배로 먹는다... 클라에서는 아이스 볼트 맞았을 때도 
// 	// 이쪽으로 들어오게 됨..
// 	// 클라에서는 처음에 상태효과 걸렸을 때도 데미지를 먹기 때문에 이쪽으로 호출됨.. 
// 	// 따라서 2회 맞으면 부서진다고 했을 때 총 3회로 해주어야 결빙 걸리고 두 번 맞고 깨지게 된다.
// 	if( m_iDurabilityCount <= 0 )
// 	{
// 		SetState( STATE_BLOW::STATE_END );
// 		OutputDebug( "%s - Freezing Broken by Attack!\n", __FUNCTION__ );
// 	}
// 
// 	return 0.0f;
// }
// #endif


void CDnEscapeBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 결빙 상태효과에서는 중복처리할 때 아무것도 하지 않는다.
	// 추후 정확한 요구사항이 오면 반영토록 한다.
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnEscapeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fValue[2] = { 0.0f, };
	int nCountValue[2] = { 0, };
	int nAllowShakeControl[2] = {0, };
	std::string strHitedAction[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() >= 3 ) 
	{
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		nCountValue[0] = atoi( vlTokens[0][1].c_str() );
		nAllowShakeControl[0] = atoi(vlTokens[0][2].c_str());

		if (vlTokens[0].size() == 4)
			strHitedAction[0] = vlTokens[0][3];
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		nCountValue[1] = atoi( vlTokens[1][1].c_str() );
		nAllowShakeControl[1] = atoi(vlTokens[1][2].c_str());

		if (vlTokens[1].size() == 4)
			strHitedAction[1] = vlTokens[1][3];
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];
	int nResultCount = (nCountValue[0] + nCountValue[1]);
	int nResultAllowShakeControl = (nAllowShakeControl[0] == 0 || nAllowShakeControl[1] == 0) ? 0 : 1;
	std::string strResultAction = strHitedAction[0].empty() ? (strHitedAction[1].empty() ? "" : strHitedAction[1]) : strHitedAction[0];

	sprintf_s(szBuff, "%f;%d;%d;%s", fResultValue, nResultCount, nResultAllowShakeControl, strResultAction.c_str());
	
	szNewValue = szBuff;
}

void CDnEscapeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fValue[2] = { 0.0f, };
	int nCountValue[2] = { 0, };
	int nAllowShakeControl[2] = {0, };
	std::string strHitedAction[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() >= 3 ) 
	{
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		nCountValue[0] = atoi( vlTokens[0][1].c_str() );
		nAllowShakeControl[0] = atoi(vlTokens[0][2].c_str());

		if (vlTokens[0].size() == 4)
			strHitedAction[0] = vlTokens[0][3];
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		nCountValue[1] = atoi( vlTokens[1][1].c_str() );
		nAllowShakeControl[1] = atoi(vlTokens[1][2].c_str());

		if (vlTokens[1].size() == 4)
			strHitedAction[1] = vlTokens[1][3];
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] - fValue[1];
	int nResultCount = (nCountValue[0] - nCountValue[1]);
	int nResultAllowShakeControl = (nAllowShakeControl[0] == 0 || nAllowShakeControl[1] == 0) ? 0 : 1;
	std::string strResultAction = strHitedAction[0].empty() ? (strHitedAction[1].empty() ? "" : strHitedAction[1]) : strHitedAction[0];

	sprintf_s(szBuff, "%f;%d;%d;%s", fResultValue, nResultCount, nResultAllowShakeControl, strResultAction.c_str());

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


void CDnEscapeBlow::CheckBossMonster()
{
	// #27679 네임드, 보스, 8인 네스트보스에게는 효과는 적용되나 실제로 escape 의 부가효과가 적용되진 않는다.
	// #28385 네스트에서만 적용됨.
	if( m_hActor->IsMonsterActor() )
	{
		if( m_bNestMap )
		{
			CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
			if( (CDnMonsterState::Boss == pMonsterActor->GetGrade() ||
				CDnMonsterState::BossHP4 == pMonsterActor->GetGrade() ||
				CDnMonsterState::NestBoss == pMonsterActor->GetGrade() ||
				CDnMonsterState::NestBoss8 == pMonsterActor->GetGrade()) )
			{
				SAFE_DELETE( m_pCantMoveBlow );
				SAFE_DELETE( m_pCantActionBlow );
				SAFE_DELETE( m_pFrameStopBlow );

				// 이래야 보스급에선 freezing 액션 실행 안됨.
				m_bIgnoreEffectAction = true;
			}
		}

		// #51048 상태효과가 있는 경우 네스트 네임드급 몬스터들과 동일한 취급을 받는다.
		// 독립적으로 상태효과가 박히는 것이므로 맵과 관계 없다.
		if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_235) )
		{
			m_bIgnoreEffectAction = true;
		}
	}
}

#if defined(PRE_FIX_51048)
void CDnEscapeBlow::RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta)
{
	if( m_pCantMoveBlow ) m_pCantMoveBlow->OnEnd( LocalTime, fDelta );
	if( m_pCantActionBlow ) m_pCantActionBlow->OnEnd( LocalTime, fDelta );
	if( m_pFrameStopBlow ) m_pFrameStopBlow->OnEnd( LocalTime, fDelta );

	SAFE_DELETE( m_pCantMoveBlow );
	SAFE_DELETE( m_pCantActionBlow );
	SAFE_DELETE( m_pFrameStopBlow );

	m_bIgnoreEffectAction = true;
}
#endif // PRE_FIX_51048