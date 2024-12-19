#include "StdAfx.h"
#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnWeapon.h"
#include "DnProjectile.h"
#include "DNGameServerManager.h"
#include "PerfCheck.h"
#include "DnActionSpecificInfo.h"

CDnActionBase::CDnActionBase()
{
	m_pRender = NULL;
	ResetActionBase();
	
	m_CustomActionTime = 0;
	m_fCustomPrevFrame = 0.f;
	// 이 포인터가 가리키는 구조체의 주소는 프로그램이 종료되기 전까진 변하지 않는다.
	m_pProjectileCountInfo = NULL;

	m_pCheckPreSignalFunc = CDnActionBase::CheckPreSignal;
	m_pCheckPostSignalFunc = CDnActionBase::CheckPostSignal;
}

void CDnActionBase::ResetActionBase()
{
	m_LocalTime = 0;
	m_ActionTime = 0;
	m_nActionIndex = -1;
	m_nPrevActionIndex = -1;
	m_fPrevFrame = 0.f;
	m_fFrame = 0.f;
	m_fQueueBlendFrame = m_fQueueStartFrame = 0.f;
	m_nCustomActionIndex = -1;	

	m_nLoopCount = 0;
	m_fFPS = 60.f;
	m_bCustomProcessSignal = false;
}

CDnActionBase::~CDnActionBase()
{
	FreeAction();
}

bool CDnActionBase::Initialize( CDnRenderBase *pRender )
{
	m_pRender = pRender;
	return true;
}

void CDnActionBase::ProcessAction( LOCAL_TIME LocalTime, float fDelta )
{
	m_LocalTime = LocalTime;
	if( !m_szActionQueue.empty() )
	{
		if( m_fQueueBlendFrame == -1 ) 
		{
			ActionElementStruct *pStruct = GetElement( m_nActionIndex );
			if( pStruct ) {
				m_fQueueBlendFrame = (float)pStruct->dwBlendFrame;
			}
		}
		SetAction( m_szActionQueue.c_str(), m_fQueueStartFrame, m_fQueueBlendFrame );
		m_szActionQueue.clear();

	}

	if( m_nActionIndex == -1 )
		return;

	ActionElementStruct *pStruct = GetElement( m_nActionIndex );

	if( 0.0f < m_fFPS )
		m_fFrame = ( ( m_LocalTime - m_ActionTime ) / 1000.f ) * m_fFPS;
	else
		return;

	if( !pStruct ) return;
	if( m_fFrame < 0.f ) m_fFrame = (float)pStruct->dwLength + 1.f;

	if( m_fFrame > (float)pStruct->dwLength )
	{
		ProcessSignal( pStruct, m_fFrame, m_fPrevFrame );

		if( m_pRender && m_nVecAniIndexList[m_nActionIndex] != -1 ) {
			EtVector3 vDist;
			m_pRender->CalcAniDistance( m_nVecAniIndexList[m_nActionIndex], (float)pStruct->dwLength, m_fPrevFrame, vDist );
			m_pRender->AddAniDistance( vDist );
		}

		if( m_nLoopCount > 0 || m_nLoopCount == -1 ) 
		{
			if( m_nLoopCount > 0 )
				m_nLoopCount--;

			OnLoopAction( m_fFrame, m_fPrevFrame );

			float fTemp = m_fFrame - (float)pStruct->dwLength;
			if( pStruct->szNextActionName == pStruct->szName ) {
				fTemp += (float)pStruct->dwNextActionFrame;
				if( fTemp > (float)pStruct->dwLength ) fTemp = (float)pStruct->dwLength;
			}

			SetAction( m_szAction.c_str(), fTemp, 0.f, true );
			OnNextAction( m_szAction.c_str() , pStruct->szNextActionName.c_str() );
		}
		else
		{
			OnFinishAction(m_szAction.c_str(), LocalTime);

			if( pStruct->szNextActionName.empty() ) 
			{
				m_nPrevActionIndex = m_nActionIndex;
				m_nActionIndex = -1;
				m_fFrame = -1.f;
				m_szAction.clear();
				return;
			}
			else 
			{
				SetAction( pStruct->szNextActionName.c_str(), ( pStruct->dwNextActionFrame == 0 ) ? 0.f : (float)pStruct->dwNextActionFrame - 0.001f, (float)pStruct->dwBlendFrame );
				if( m_fFrame > 0.f )
					m_fPrevFrame -= 1.f;
				OnNextAction( m_szAction.c_str() , pStruct->szNextActionName.c_str() );
			}
			
		}

		if( m_nActionIndex == -1 )
			return;

		pStruct = GetElement( m_nActionIndex );
		if( !pStruct )
			return;
	}

	if( m_nCustomActionIndex != -1 ) 
	{
		ActionElementStruct *pCustomStruct = GetElement( m_nCustomActionIndex );

		if( pCustomStruct ) {
			float fFrame = ( ( m_LocalTime - m_CustomActionTime ) / 1000.f ) * m_fFPS;
			if( fFrame > (float)pCustomStruct->dwLength ) 
			{
				ResetCustomAction();
				ProcessSignal( pCustomStruct, fFrame, m_fCustomPrevFrame );
			}
			else 
			{
				m_bCustomProcessSignal = true;
				float fTemp = m_fCustomPrevFrame;
				ProcessSignal( pCustomStruct, fFrame, m_fCustomPrevFrame );
				m_bCustomProcessSignal = false;

				if( m_fCustomPrevFrame == fTemp )
				{
					if( fFrame == m_fCustomPrevFrame )
						m_fCustomPrevFrame = fFrame + 0.001f;
					else
						m_fCustomPrevFrame = fFrame;
				}
			}
		}
	}
	// CanMove 가 True 이고 CustomAction 이 실행되는 경우에
	// Input Signal 들은 이동에 있는 시그널들을 처리해주면 편하기 땜시 일단 풀어놓는다..
	// 이케 하면 CanMove 가 중간에 True 루 바뀌는 경우에 그 아래 Input Signal 들을 넣어줄 필요가 없다. 
	// 단 움직이지 않았을 경우엔 Move 쪽에 박혀있는 Signal들이 적용되지 않으므로 필요시 추가로 넣어주길 바람
//	ProcessSignal( pStruct, m_fFrame, m_fPrevFrame );
	else
	{
		ProcessSignal( pStruct, m_fFrame, m_fPrevFrame );
	}
	if( m_fFrame == m_fPrevFrame )
		m_fPrevFrame = m_fFrame + 0.001f;
	else 
		m_fPrevFrame = m_fFrame;
}

void CDnActionBase::ProcessSignal( ActionElementStruct *pStruct, float fFrame, float fPrevFrame )
{
	CEtActionSignal *pSignal;
	for( DWORD i=0; i<pStruct->pVecSignalList.size(); i++ ) {
		pSignal = pStruct->pVecSignalList[i];

		if( pSignal->CheckSignal( fPrevFrame, fFrame ) == true ) 
		{
			LOCAL_TIME StartTime = m_LocalTime - (LOCAL_TIME)( 1000.f / m_fFPS * ( fFrame - pSignal->GetStartFrame() ) );
			LOCAL_TIME EndTime = m_LocalTime + (LOCAL_TIME)( 1000.f / m_fFPS * ( pSignal->GetEndFrame() - fFrame ) );

			OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), m_LocalTime, StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
		}

		if( !m_szActionQueue.empty() ) break;
	}
}

void CDnActionBase::SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop )
{
	if (szActionName == NULL) return;
	int nIndex = GetElementIndex( szActionName );
	if( nIndex == -1 ) {
#ifdef _DEBUG
		OutputDebug( "Can't find Action : %s, %s\n", m_szFileName.c_str(), szActionName );
#endif
		return;
	}

	std::string szPrevAction = m_szAction;

	m_nPrevActionIndex = m_nActionIndex;
	m_nActionIndex = nIndex;
	m_szAction = szActionName;

	m_fFrame = m_fPrevFrame = (float)fFrame;
	m_ActionTime = m_LocalTime - (LOCAL_TIME)( fFrame / m_fFPS * 1000.f );
	if( m_ActionTime < 0 ) m_ActionTime = 0;

	if( m_pRender && m_nVecAniIndexList[nIndex] != -1 ) 
	{
		m_pRender->ChangeAnimation( m_nVecAniIndexList[nIndex], fFrame, fBlendFrame );
		if( bLoop ) m_pRender->SetPrevFrame( 0.f );
	}

	OnChangeAction( szPrevAction.c_str() );
}

void CDnActionBase::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame )
{
	m_szActionQueue = szActionName;
	m_fQueueBlendFrame = fBlendFrame;
	m_fQueueStartFrame = fStartFrame;

	m_nLoopCount = nLoopCount;
	OnChangeActionQueue( m_szAction.c_str() );

}

const char *CDnActionBase::GetCurrentAction()
{
	if( !m_szActionQueue.empty() ) return m_szActionQueue.c_str();
	return m_szAction.c_str();
}

void CDnActionBase::SetCustomAction( const char *szActionName, float fFrame )
{
	m_szCustomAction = szActionName;
	m_CustomActionTime = m_LocalTime - (LOCAL_TIME)( fFrame / m_fFPS * 1000.f );
	m_nCustomActionIndex = GetElementIndex( szActionName );
	m_fCustomPrevFrame = fFrame - 1.f;
}

bool CDnActionBase::IsCustomAction() 
{ 
	return ( m_nCustomActionIndex == -1 ) ? false : true;
}

void CDnActionBase::ResetCustomAction()
{
	m_szCustomAction.clear();
	m_nCustomActionIndex = -1;	
}

#include "DNConfig.h"
extern TGameConfig g_Config;

bool CDnActionBase::LoadAction( const char *szFullPathName )
{
	m_ProjectileCountInfoForInit.Clear();
	m_SkillChainInfoForInit.Clear();

	m_PassiveSkillInfoForInit.Clear();

	m_BasicAttackInfoForInit.Clear();

	bool bResult = false;
	bResult = CEtActionBase::LoadAction( szFullPathName );
	if( bResult ) CacheAniIndex();

	if( bResult )
	{
		if( false == m_ProjectileCountInfoForInit.mapMaxProjectileCountInAction.empty() ||
			false == m_ProjectileCountInfoForInit.mapSendActionWeapon.empty() )
		{
			if (g_Config.bPreLoad && g_Config.bAllLoaded)
			{
				//프리로드가 켜져있으면 서버기동시 이외에 여기에 들어오면 안됨다! 일단 로그밖음
				g_Log.Log(LogType::_PRELOADED_DYNAMICLOAD, L"DynamicLoad [%S]\n", szFullPathName);
			}

			CDnActionSpecificInfo::GetInstance().AddProjectileSignalInfo( szFullPathName, m_ProjectileCountInfoForInit );
		}

		m_pProjectileCountInfo = CDnActionSpecificInfo::GetInstance().FindProjectileSignalInfo( szFullPathName );

		if( false == m_SkillChainInfoForInit.setSkillChainAction.empty() )
		{
			if (g_Config.bPreLoad && g_Config.bAllLoaded)
			{
				//프리로드가 켜져있으면 서버기동시 이외에 여기에 들어오면 안됨다! 일단 로그밖음
				g_Log.Log(LogType::_PRELOADED_DYNAMICLOAD, L"DynamicLoad [%S]\n", szFullPathName);
			}

			CDnActionSpecificInfo::GetInstance().AddSkillChainActionSet( szFullPathName, m_SkillChainInfoForInit );
		}

		m_pSetSkillChainAction = CDnActionSpecificInfo::GetInstance().FindSkillChainActionSet( szFullPathName );

		if( false == m_PassiveSkillInfoForInit.mapPassiveSkillInfo.empty() )
		{
			if (g_Config.bPreLoad && g_Config.bAllLoaded)
			{
				//프리로드가 켜져있으면 서버기동시 이외에 여기에 들어오면 안됨다! 일단 로그밖음
				g_Log.Log(LogType::_PRELOADED_DYNAMICLOAD, L"DynamicLoad [%S]\n", szFullPathName);
			}

			CDnActionSpecificInfo::GetInstance().AddPassiveSkillInfo( szFullPathName, m_PassiveSkillInfoForInit );
		}

		m_pPassiveSkillInfo = CDnActionSpecificInfo::GetInstance().FindPassiveSkillInfo( szFullPathName );

		if( false == m_BasicAttackInfoForInit.mapBasicAttackInfo.empty() )
		{
			if (g_Config.bPreLoad && g_Config.bAllLoaded)
			{
				//프리로드가 켜져있으면 서버기동시 이외에 여기에 들어오면 안됨다! 일단 로그밖음
				g_Log.Log(LogType::_PRELOADED_DYNAMICLOAD, L"DynamicLoad [%S]\n", szFullPathName);
			}

			CDnActionSpecificInfo::GetInstance().AddBasicAttackActionInfo( szFullPathName, m_BasicAttackInfoForInit );
		}

		m_pBasicAttackInfo = CDnActionSpecificInfo::GetInstance().FindBasicAttackInfo( szFullPathName );
	}

	return bResult;
}

void CDnActionBase::FreeAction()
{
	CEtActionBase::FreeAction();
}

void CDnActionBase::CacheAniIndex()
{
	m_nVecAniIndexList.clear();
	
	// 서버 덤프 , 컨테이너에 포인터를 물려놓는 경우는 크래쉬를 유발할수 있다 일단 empty() 체크를 추가하고 empty() 
	// 자체를 콜하다가 크래쉬가 난다면 자료구조를 바꿔야할듯 싶습니다.
	if (m_pVecActionElementList == NULL || m_pVecActionElementList->empty() )
		return;

	for( DWORD i=0; i<m_pVecActionElementList->size(); i++ ) {
		int nIndex = -1;
		if( m_pRender && (*m_pVecActionElementList)[i] && !(*m_pVecActionElementList)[i]->szLinkAniName.empty() ) {
			nIndex = m_pRender->GetAniIndex( (*m_pVecActionElementList)[i]->szLinkAniName.c_str() );
		}
		m_nVecAniIndexList.push_back( nIndex );
	}
}

void CDnActionBase::CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase )
{
	/*
	주의.. 미리 생성시 Room 포인터를 넘겨야 하는 경우가 있는데요..
	This를 얻어서 넣어주게 되면 안됩니다.
	반드시 (CMultiRoom*)g_pGameServerManager->GetRootRoom() 룸을 얻어서 셋팅하시고
	얻어오실때도 해당 객체의 룸을 얻으시면 안되고 GetRootRoom() 을 사용해서 얻으세요~
	*/
	switch( pSignal->GetSignalIndex() ) {
		case STE_Input:
			{
				if( pSignal->GetStartFrame() == 0 && pSignal->GetEndFrame() == pElement->dwLength ) {
					pSignal->SetStartFrame( -1 );
				}

				// 인풋 시그널에 스킬 체인 플래그가 켜져 있는 경우에 스킬 체인되는 액션으로 미리 분류해두었다가 핵을 막는다.
				if( pActionBase )
				{
					CDnActionBase* pDnActionBase = static_cast<CDnActionBase*>( pActionBase );

					pDnActionBase->InsertBasicAttackInfo( nElementIndex, pSignal );
					pDnActionBase->InsertSkillChainInfo( pElement, nElementIndex, pSignal );
					pDnActionBase->InsertBasicShootActionCoolTime( pElement, nElementIndex, pSignal );
				}
			}
			break;

		case STE_InputHasPassiveSkill:
			{
				InputHasPassiveSkillStruct *pStruct = (InputHasPassiveSkillStruct *)pSignal->GetData();

				if( pSignal->GetStartFrame() == 0 && pSignal->GetEndFrame() == pElement->dwLength ) {
					pSignal->SetStartFrame( -1 );
				}

				if( !pStruct->szEXSkillChangeAction ) pSignal->InsertStrTable( (char*)&pStruct->szEXSkillChangeAction, std::string("") );
				if( pActionBase ) {
					((CDnActionBase*)pActionBase)->InsertPassiveSkillInfo( nElementIndex, pSignal );
				}
			}
			break;
		case STE_Projectile:
			{
				ProjectileStruct *pStruct = (ProjectileStruct *)pSignal->GetData();
				pStruct->nProjectileIndex = -1;

				if( !g_pGameServerManager )	break;

				// 무기 인덱스가 0인 경우는 클라이언트에서 보조 무기의 인덱스로 대체해서 나가도록 클라에서 되어있기 때문에
				// 무기 인덱스가 0 이라고 break 되는 부분 위에 카운트가 되어야 한다.
				// 이 액션에서 나갈 수 있는 발사체의 최대 갯수를 저장해둔다. 
				// 클라로부터 발사체 패킷이 오면 액션이 바뀌기 전에 최대 갯수를 넘기지 않도록 쏜다.
				if( pActionBase )
					((CDnActionBase*)pActionBase)->InsertProjectileCountInfo( nElementIndex, pSignal );

				if( pStruct->nWeaponTableID == 0 ) break;

				CDnProjectile *pProjectile = new CDnProjectile( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), CDnActor::Identity(), false , false );
				pProjectile->CDnWeapon::Initialize( pStruct->nWeaponTableID, -1 );
				pProjectile->CDnWeapon::CreateObject();
				pStruct->nProjectileIndex = pProjectile->GetMyIndex();
			}
			break;

		case STE_SendAction_Weapon:
			{
				if( pActionBase )
					((CDnActionBase*)pActionBase)->InsertSendActionProjectileCountInfo( nElementIndex, pSignal );
			}
			break;
		case STE_Gravity:
			{
				GravityStruct *pStruct = (GravityStruct *)pSignal->GetData();
				if( !pStruct->vOffset ) pSignal->InsertVec3Table( (EtVector3*)&pStruct->vOffset, EtVector3( 0.f, 0.f, 0.f ) );
			}
			break;

		default:
			{
				if( pActionBase )
				{
					CDnActionBase* pDnActionBase = static_cast<CDnActionBase*>( pActionBase );
					pDnActionBase->InsertStandChangeSEShootSkillCoolTime( pElement, nElementIndex, pSignal );
				}
			}
			break;
	}
}

void CDnActionBase::CheckPostSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase )
{
	if( !g_pGameServerManager ) return;
	if( !g_pGameServerManager->GetRootRoom() ) return;
	switch( pSignal->GetSignalIndex() ) {
		case STE_Projectile:
			{
				ProjectileStruct *pStruct = (ProjectileStruct *)pSignal->GetData();
				if( pStruct->nWeaponTableID == 0 ) break;
				if( pStruct->nProjectileIndex == -1 ) break;

				DnWeaponHandle hWeapon = CDnWeapon::GetSmartPtr( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), pStruct->nProjectileIndex );
				SAFE_RELEASE_SPTR( hWeapon );
			}
			break;
	}
}

void CDnActionBase::InsertBasicAttackInfo( int nElementIndex, CEtActionSignal *pSignal )
{
	InputStruct* pInputStruct = (InputStruct*)(pSignal->GetData());

	CDnActionSpecificInfo::S_BASIC_ATTACK_INPUT_SIGNAL_INFO BasicActionSignalInfo;
	BasicActionSignalInfo.strChangeActionName = pInputStruct->szChangeAction;
	int iStartFrame = pSignal->GetStartFrame() < 0 ? 0 : pSignal->GetStartFrame();
	BasicActionSignalInfo.dwStartFrame = iStartFrame;
	BasicActionSignalInfo.dwEndFrame = pSignal->GetEndFrame();
	m_BasicAttackInfoForInit.mapBasicAttackInfo[ nElementIndex ].push_back( BasicActionSignalInfo );
}

void CDnActionBase::InsertSkillChainInfo( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal )
{
	InputStruct* pInputStruct = (InputStruct*)(pSignal->GetData());

	if( TRUE == pInputStruct->bSkillChain )
	{
		m_SkillChainInfoForInit.setSkillChainAction.insert( nElementIndex );
		m_SkillChainInfoForInit.mapCanChainToThisAction[ nElementIndex ].push_back( pInputStruct->szChangeAction );
	}
}

void CDnActionBase::InsertBasicShootActionCoolTime( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal )
{
	InputStruct* pInputStruct = (InputStruct*)(pSignal->GetData());

	// 기본 슛 액션이고 변환 액션이 pick, jump 액션이 아닌 것.
	const char* pActionName = pElement->szName.c_str();
	if( strstr(pActionName, "Shoot_") )
	{
		if( stricmp("Pick", pInputStruct->szChangeAction) != 0 &&
			stricmp("Jump", pInputStruct->szChangeAction) != 0 )
		{
			// 실제 다른 액션으로 바뀌는 인풋이 여러개 있더라도 그냥 덮어 씌운다.
			int iStartFrame = pSignal->GetStartFrame() < 0 ? 0 : pSignal->GetStartFrame();
			m_ProjectileCountInfoForInit.mapBasicShootActionCoolTime[ nElementIndex ] = DWORD(((float)iStartFrame / m_fFPS) * 1000);
		}
	}
}

void CDnActionBase::InsertStandChangeSEShootSkillCoolTime( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal )
{
	if( m_ProjectileCountInfoForInit.mapBasicShootActionCoolTime.end() == m_ProjectileCountInfoForInit.mapBasicShootActionCoolTime.find( nElementIndex ) )
	{
		const char* pActionName = pElement->szName.c_str();

		// 시즈 스탠스의 공격 액션도 쿨타임이 필요하다. 입력을 받아 액션이 바뀌면서 발사체를 쏘기 때문에..
		// 약간 모양새가 좋진 않지만 여기서 시즈스탠스어택 액션의 전체 길이를 쿨타임으로 설정한다. 
		// 시즈스탠스어택 액션에서는 이 input 시그널로 점프 입력 받는 것 밖엔 없다.
		// 데몰리션도 마찬가지로 시즈 스탠스와 구조가 같으므로 같은 방식으로 처리한다.
		if( strcmp(pActionName, "Skill_SiegeStance_Attack") == 0 ||
			strcmp(pActionName, "Skill_Demolition_Attack1") == 0 ||
			strcmp(pActionName, "Skill_Demolition_Attack2") == 0 )
		{
			m_ProjectileCountInfoForInit.mapBasicShootActionCoolTime[ nElementIndex ] = DWORD(((float)pElement->dwLength  / m_fFPS) * 1000);
		}
		else
		if( strcmp(pActionName, "Skill_FlashStance_AttackL") == 0 ||
			strcmp(pActionName, "Skill_FlashStance_AttackR") == 0 )
		{
			// 플래쉬 스탠스는 플래쉬 스탠스 공격액션에 input 으로 공격하는 시그널이 있으므로 input 시그널 구간까지 시간을 계산해 둔다.
			// 공격 액션에 현재는 같은 시작 프레임에 2개의 Input 시그널이 있지만 추후에 시작 프레임이 달라질 소지가 있다.
			// 그런 경우엔 가장 먼저 시작되는 Input 시그널의 프레임을 기준으로 쿨타임을 잡아둔다.
			int iSmallestStartFrame = INT_MAX;
			int iNumSignals = (int)pElement->pVecSignalList.size();
			for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
			{
				CEtActionSignal* pSignal = pElement->pVecSignalList.at( iSignal );
				if( STE_Input == pSignal->GetSignalIndex() )
				{
					InputStruct* pInputStruct = (InputStruct*)(pSignal->GetData());
					if( stricmp("Pick", pInputStruct->szChangeAction) != 0 &&
						stricmp("Jump", pInputStruct->szChangeAction) != 0 )
					{
						int iStartFrame = pSignal->GetStartFrame() < 0 ? 0 : pSignal->GetStartFrame();
						if( iStartFrame < iSmallestStartFrame )
						{
							iSmallestStartFrame = iStartFrame;
						}
					}
				}
			}

			if( INT_MAX != iSmallestStartFrame )
			{
				m_ProjectileCountInfoForInit.mapBasicShootActionCoolTime[ nElementIndex ] = DWORD(((float)iSmallestStartFrame / m_fFPS) * 1000);
			}
		}
	}
}

void CDnActionBase::InsertPassiveSkillInfo( int nElementIndex, CEtActionSignal *pSignal )
{
	InputHasPassiveSkillStruct* pStruct = (InputHasPassiveSkillStruct*)(pSignal->GetData());

	CDnActionSpecificInfo::S_PASSIVESKILL_SIGNAL_INFO PassiveSignalInfo;
	PassiveSignalInfo.iSkillID = pStruct->nSkillIndex;
	PassiveSignalInfo.strChangeActionName = pStruct->szChangeAction;
	PassiveSignalInfo.strEXSkillChangeActionName = pStruct->szEXSkillChangeAction;
	m_PassiveSkillInfoForInit.mapPassiveSkillInfo[ nElementIndex ].push_back( PassiveSignalInfo );
}

void CDnActionBase::InsertProjectileCountInfo( int nElementIndex, CEtActionSignal *pSignal )
{
	ProjectileStruct *pStruct = (ProjectileStruct *)pSignal->GetData();

	map<int, int>::iterator iter = m_ProjectileCountInfoForInit.mapMaxProjectileCountInAction.find( nElementIndex );
	if( iter != m_ProjectileCountInfoForInit.mapMaxProjectileCountInAction.end() )
		m_ProjectileCountInfoForInit.mapMaxProjectileCountInAction[ nElementIndex ]++;
	else
		m_ProjectileCountInfoForInit.mapMaxProjectileCountInAction.insert( make_pair(nElementIndex, 1) );

	// 무기 테이블 ID도 받아둔다. 일반 공격액션 패킷으로 스킬 체크 루틴을 피하고, 
	// 발사체 갯수 체크 루틴으로 충전된 한발로 무기 번호 바꿔서 보내는 놈들이 있음.... 아 짱깨...
	m_ProjectileCountInfoForInit.mapUsingProjectileWeaponTableIDs[ nElementIndex ].insert( pStruct->nWeaponTableID );

	// 스킬 발사체 바꿔보내는 것과 별도로, 아쳐, 소서리스의 기본 공격 액션과 발사체 패킷을 난사해서 마구 쏘는 것을
	// 막기 위해 발사체 패킷을 받았을 때 시그널의 간격도 체크한다.
	m_ProjectileCountInfoForInit.mapProjectileSignalFrameOffset[ nElementIndex ].push_back( pSignal->GetStartFrame() );
}

void CDnActionBase::InsertSendActionProjectileCountInfo( int nElementIndex, CEtActionSignal *pSignal )
{
	SendAction_WeaponStruct *pStruct = (SendAction_WeaponStruct *)pSignal->GetData();

	if( 2 <= pStruct->nWeaponIndex )
		return;

	CDnActionSpecificInfo::S_WEAPONACTION_INFO WeaponActionInfo;
	WeaponActionInfo.iWeaponIndex = pStruct->nWeaponIndex;
	WeaponActionInfo.iFrame = pSignal->GetStartFrame();
	if( 0 < strlen(pStruct->szActionName) )
		WeaponActionInfo.strActionName = pStruct->szActionName;

	map<int, vector<CDnActionSpecificInfo::S_WEAPONACTION_INFO> >::iterator iter = m_ProjectileCountInfoForInit.mapSendActionWeapon.find( nElementIndex );
	if( m_ProjectileCountInfoForInit.mapSendActionWeapon.end() == iter )
	{
		// 최초로 발견되었음.
		vector<CDnActionSpecificInfo::S_WEAPONACTION_INFO> vlWeaponActionInfo;
		vlWeaponActionInfo.push_back( WeaponActionInfo );
		m_ProjectileCountInfoForInit.mapSendActionWeapon.insert( make_pair(nElementIndex, vlWeaponActionInfo) );
	}
	else
	{
		// 두번째 부터는 벡터로 추가. (ex)크로스 보우)
		iter->second.push_back( WeaponActionInfo );
	}
}

void CDnActionBase::SetFPS( float fValue )
{
	if( 0.0f < fValue )
	{
		m_ActionTime = m_LocalTime - (LOCAL_TIME)( m_fFrame / fValue * 1000.f );
		if( IsCustomAction() ) {
			float fFrame = ( ( m_LocalTime - m_CustomActionTime ) / 1000.f ) * m_fFPS;
			m_CustomActionTime = m_LocalTime - (LOCAL_TIME)( fFrame / fValue * 1000.f );
		}
	}
	else
	{
		// fValue 가 0.0f 로 호출된 경우.
		m_ActionTime = m_LocalTime;
		if( IsCustomAction() ) {
			m_CustomActionTime = m_LocalTime;
		}
	}

	m_fFPS = fValue;

	if( m_pRender ) 
		m_pRender->SetFPS( fValue );
}

float CDnActionBase::GetFPS()
{
	return m_fFPS;
}

bool CDnActionBase::IsIgnoreSignal( int nSignalIndex )
{
	switch( nSignalIndex ) {
		case STE_DnNullSignal:
		case STE_Sound:
		case STE_Particle:
		case STE_EnvironmentEffect:
		case STE_ShowWeapon:
		case STE_AlphaBlending:
		case STE_AttachTrail:
		case STE_CameraEffect_Shake:
		case STE_FX:
		case STE_CanRotate:
		case STE_ActionObject:
		case STE_ObjectVisible:
		case STE_AttachSwordTrail:
		case STE_ShowSwordTrail:
		case STE_CameraEffect_RadialBlur:
		case STE_Decal:
		case STE_HeadLook:
		case STE_ShaderCustomParameter:
		case STE_ChangeWeaponLink:
		case STE_FreezeCamera:
		case STE_SocialAction:
		case STE_HideExposureInfo:
		case STE_PhysicsSkip:
		case STE_Particle_LoopEnd:
		case STE_FX_LoopEnd:
		case STE_OutlineFilter:
		case STE_Billboard:
		case STE_EyeLightTrail:
		case STE_PointLight:
		case STE_OtherSelfEffect:
		case STE_ImmediatelyAttach:
		case STE_Dialogue:
		case STE_AttachLine:
		case STE_SyncChangeAction:
		case STE_CannonTargeting:
		case STE_SkillChecker:
		case STE_CameraEffect_Swing:
			return true;
	}
	return false;
}