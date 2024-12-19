#include "StdAFx.h"
#include "MAAiBasilisk.h"
#include "DnBasiliskMonsterActor.h"


const float BLEND_FRAME = 0.0f;

void
BasiliskStateBase::Init(LOCAL_TIME time)
{
	m_nLastTime = time;

}

void
BasiliskStateBase::OnStartState(LOCAL_TIME time)
{
	m_nStartTime = time;

}

void	
BasiliskStateBase::OnEndState(LOCAL_TIME time)
{

}

void 
BasiliskStateBase::ChangeState(DWORD nState)
{
	m_pBasilisk->ChangeState(nState); 
}

void 
BasiliskStateBase::CmdAction(const char *szActionName, int nLoopCount /* = 0 */ , float fBlendFrame /* = 3.f */)
{
	if ( !m_pBasilisk )
		return;

	m_pBasilisk->CmdAction(szActionName, nLoopCount, fBlendFrame); 
}

DWORD
BasiliskStateBase::OnState(LOCAL_TIME time)
{
	m_nLastTime = time;
	return 0;
}


bool
BasiliskStateBase::IsPassedCoolTime(LOCAL_TIME time)
{
	if ( m_nLastTime == 0 )
		return true;

	if ( m_nLastTime + m_nCoolTime >= time )
		return false;

	return true;
}


bool
BasiliskStateBase::IsPassedKeepupTime(LOCAL_TIME time)
{


	if ( m_nLastTime + m_nKeepupTime >= time )
		return false;

	return true;
}

bool
BasiliskStateBase::Verify(CurrentInfo& property)
{
	if ( m_nPositionState != property.nPosState )
		return false;
	return true;

}

//--------------------------------------------------------------------------------------------------------

DWORD
Ground_Stand::OnState(LOCAL_TIME time)
{

	CmdAction("Ground_Stand",0 ,BLEND_FRAME);
	return 0;

//	return BasiliskState::Ground_Stand;
}

bool
Ground_Stand::Verify(CurrentInfo& property)
{
	return false;
}



DWORD 
Ground_TailAttack::OnState(LOCAL_TIME time)
{

	CmdAction("TailAttack",0 , BLEND_FRAME);
	return 0;

}

bool
Ground_TailAttack::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Behind )
	{
		return true;
	}
	return false;
}


DWORD
Ground_BodyAttack::OnState(LOCAL_TIME time)
{

	CmdAction("BodyAttack",0 , BLEND_FRAME);
	return 0;
}

bool
Ground_BodyAttack::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Front )
	{
		return true;
	}
	return false;
}


DWORD
Ground_RunAttack::OnState(LOCAL_TIME time)
{

	CmdAction("RunAttack",0 , BLEND_FRAME);
	return 0;
}

bool
Ground_RunAttack::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Front )
	{
		return true;
	}
	return false;
}


DWORD 
Ground_StandUp::OnState(LOCAL_TIME time)
{

	CmdAction("StandUp",0 , BLEND_FRAME);
	return 0;
}

bool
Ground_StandUp::Verify(CurrentInfo& property)
{
	return false;
}

DWORD
Ground_StandDead::OnState(LOCAL_TIME time)
{
	return 0;
}

bool
Ground_StandDead::Verify(CurrentInfo& property)
{
	return false;
}



DWORD 
Turn_Left::OnState(LOCAL_TIME time)
{

	CmdAction("Turn_Left",0 , BLEND_FRAME);
	return 0;
}

bool
Turn_Left::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Left )
	{
		return true;
	}
	return false;
}



DWORD 
Turn_Right::OnState(LOCAL_TIME time)
{

	CmdAction("Turn_Right",0 ,BLEND_FRAME );
	return 0;
}

bool
Turn_Right::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Right )
	{
		return true;
	}
	return false;
}



DWORD 
FootAttack_Left::OnState(LOCAL_TIME time)
{

	CmdAction("FootAttack_Left",0 , BLEND_FRAME);
	return 0;
}

bool
FootAttack_Left::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Left )
	{
		return true;
	}
	return false;
}


DWORD 
FootAttack_Right::OnState(LOCAL_TIME time)
{

	CmdAction("FootAttack_Right",0 , BLEND_FRAME);
	return 0;
}

bool
FootAttack_Right::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	int nDir = m_pBasilisk->GetEnermyDir();
	if ( nDir == EnermyDir::Right )
	{
		return true;
	}
	return false;
}


DWORD 
OverTurned::OnState(LOCAL_TIME time)
{
	CmdAction("OverTurned",0 , BLEND_FRAME);
	return 0;
}

bool
OverTurned::Verify(CurrentInfo& property)
{
	return false;
}


DWORD
OverTurned_Dead::OnState(LOCAL_TIME time)
{
	return 0;

}

bool
OverTurned_Dead::Verify(CurrentInfo& property)
{
	return false;
}

DWORD 
OverTurned_Hit::OnState(LOCAL_TIME time)
{
	return 0;


}

bool
OverTurned_Hit::Verify(CurrentInfo& property)
{
	return false;
}


DWORD 
Pillar_Climb::OnState(LOCAL_TIME time)
{
	CmdAction("Pillar_Climb",0 ,BLEND_FRAME);
	//	m_pBasilisk->InitRegionDamage();
	return BasiliskState::Pillar_Stand;
}

bool
Pillar_Climb::Verify(CurrentInfo& property)
{

	if ( property.nPosState == BasiliskPosition::Pillar )
	{
		return false;
	}

	// 체력이 85% 이하가 되면 기둥 탈수 있다.
	float fCurHp = (float)property.nCurHP;
	float fMaxHp = (float)property.nMaxHP;

	fMaxHp*=0.85f;
	if ( fMaxHp >= fCurHp )
	{
		return true;
	}
	return false;
}

DWORD 
Pillar_Stand::OnState(LOCAL_TIME time)
{
	CmdAction("Pillar_Stand",0 ,BLEND_FRAME);
	return 0;

}

bool
Pillar_Stand::Verify(CurrentInfo& property)
{
	return false;
}


DWORD
Pillar_ThornAttack::OnState(LOCAL_TIME time)
{
	CmdAction("Pillar_ThornAttack",0 , BLEND_FRAME);
	//return BasiliskState::Pillar_ThornAttack;
	return 0;

}

bool
Pillar_ThornAttack::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	return true;
}


DWORD 
Pillar_LoseBalance::OnState(LOCAL_TIME time)
{

	CmdAction("Pillar_LoseBalance",0 , BLEND_FRAME);

	CDnBasiliskMonsterActor* pBasil = dynamic_cast<CDnBasiliskMonsterActor*>(m_pBasilisk->GetActor().GetPointer());
	if ( pBasil )
	{
		if ( pBasil->IsFallDown() )
		{
			return BasiliskState::Pillar_Fall;
		}
		else
		{
			// 아니면 다시 기둥에 붙는다.
			return BasiliskState::Pillar_KeepBalance;
		}
	}
	return 0;
}

bool
Pillar_LoseBalance::Verify(CurrentInfo& property)
{
	return false;
}

DWORD 
Pillar_KeepBalance::OnState(LOCAL_TIME time)	
{
	CmdAction("Pillar_KeepBalance",0 , BLEND_FRAME);
	//	m_pBasilisk->InitRegionDamage();
	return BasiliskState::Pillar_Stand;
}

bool
Pillar_KeepBalance::Verify(CurrentInfo& property)
{
	return false;
}

DWORD 
Pillar_Fall::OnState(LOCAL_TIME time)
{
	CmdAction("Pillar_Fall",0 , BLEND_FRAME);

	CDnBasiliskMonsterActor* pBasil = dynamic_cast<CDnBasiliskMonsterActor*>(m_pBasilisk->GetActor().GetPointer());
	if ( pBasil )
	{
		pBasil->OnFallToTheGround();
	}
	return BasiliskState::OverTurned;
}

bool
Pillar_Fall::Verify(CurrentInfo& property)
{
	return false;
}


DWORD 
Vaulting_Climb::OnState(LOCAL_TIME time)
{
	CmdAction("Vaulting_Climb",0 ,BLEND_FRAME);
	return BasiliskState::Vaulting_Stand;
}

bool
Vaulting_Climb::Verify(CurrentInfo& property)
{
	return false;
}

DWORD 
Vaulting_Stand::OnState(LOCAL_TIME time)
{
	CmdAction("Vaulting_Stand",0 ,BLEND_FRAME);
	return 0;

}

bool
Vaulting_Stand::Verify(CurrentInfo& property)
{
	return false;
}


DWORD
Vaulting_ThornAttack::OnState(LOCAL_TIME time)
{
	CmdAction("Vaulting_ThornAttack",0 ,BLEND_FRAME);
	return 0;
}

bool
Vaulting_ThornAttack::Verify(CurrentInfo& property)
{
	if ( IsPassedCoolTime(property.nCurTime) == false )
		return false;

	return true;
}


DWORD
Vaulting_LoseBalance::OnState(LOCAL_TIME time)
{

	CmdAction("Vaulting_LoseBalance",0 ,BLEND_FRAME);

	float fCurHp = (float)m_pBasilisk->GetCurrentInfo().nCurHP;
	float fMaxHp = (float)m_pBasilisk->GetCurrentInfo().nMaxHP;

	fMaxHp*=0.50f;
	// 밸런스를 잃었을때 체력이 30프로 이하면 떨어진다.
	if ( fMaxHp >= fCurHp )
	{
		return BasiliskState::Vaulting_Fall;
	}

	// 아니면 다시 기둥에 붙는다.
	return BasiliskState::Vaulting_KeepBalance;


	return 0;
}

bool
Vaulting_LoseBalance::Verify(CurrentInfo& property)
{
	return false;
}

DWORD
Vaulting_KeepBalance::OnState(LOCAL_TIME time)
{
	CmdAction("Vaulting_LoseBalance",0 ,BLEND_FRAME);
	return 0;
}

bool
Vaulting_KeepBalance::Verify(CurrentInfo& property)
{
	return false;
}


DWORD
Vaulting_Fall::OnState(LOCAL_TIME time)
{
	CmdAction("Vaulting_Fall",0 ,BLEND_FRAME);
	CDnBasiliskMonsterActor* pBasil = dynamic_cast<CDnBasiliskMonsterActor*>(m_pBasilisk->GetActor().GetPointer());
	if ( pBasil )
	{
		pBasil->OnFallToTheGround();
	}
	return BasiliskState::OverTurned;
}

bool
Vaulting_Fall::Verify(CurrentInfo& property)
{
	return false;
}

DWORD 
Pillar_ComeDown::OnState(LOCAL_TIME time)
{
	CmdAction("Pillar_ComeDown",0 ,BLEND_FRAME);
	return BasiliskState::Ground_Stand;

}
bool
Pillar_ComeDown::Verify(CurrentInfo& property)
{
	return false;
}


//----------------------------------------------------------------------------------------------



static PLAY_ANI_ARRAY __gs_Table_DyingGround;
static PLAY_ANI_ARRAY __gs_Table_DyingPillar;
static PLAY_ANI_ARRAY __gs_Table_DyingPillar_LoseBalance;
static PLAY_ANI_ARRAY __gs_Table_DyingVaulting;
static PLAY_ANI_ARRAY __gs_Table_DyingOverTurned;


class __PlauAniArrayInit
{
public:
	__PlauAniArrayInit()
	{
		// 바닥에서 죽었을 경우 플레이할 애니메이션 테이블
		__gs_Table_DyingGround.push_back("Stand_Dead");

		// 기둥에서 죽었을 경우 플레이할 애니메이션 테이블 기둥에서 균형 잃고, 떨어진후, 뒤집어진 상태에서 죽는다.
		__gs_Table_DyingPillar.push_back("Pillar_LoseBalance");
		__gs_Table_DyingPillar.push_back("Pillar_Fall");

		__gs_Table_DyingPillar_LoseBalance.push_back("Pillar_Fall");

		// 천정에서 죽었을 경우 플레이할 애니메이션 테이블 천정에서 균형 잃고, 떨어진후, 뒤집어진 상태에서 죽는다.
		__gs_Table_DyingVaulting.push_back("Vaulting_LoseBalance");
		__gs_Table_DyingVaulting.push_back("Vaulting_Fall");

		// 뒤집혀진 상태에서 죽기
		__gs_Table_DyingOverTurned.push_back("OverTurned_Dead");
	}
}_____PlauAniArrayInit;

//--------------------------------------------------------------------------------------------------------------------

MAAiBasilisk* g_pBasil = NULL;

//--------------------------------------------------------------------------------------------------------------------
MAAiBasilisk::MAAiBasilisk( DnActorHandle hActor, MAAiReceiver *pReceiver )
: MAAiBase( hActor, pReceiver )
{
	
	m_LocalTime = 0;
	m_nCurPosStateTime = 0;
	m_nNextState = 0;
	m_nPrevState= 0;
	m_nCurPosition = 0;
	m_nCurrentAnimationIndex = 0;

	g_pBasil = this;
	m_pLua = NULL;

	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		m_StateArray[i] = NULL;
	}
	_InitState();
	
}


MAAiBasilisk::~MAAiBasilisk()
{
	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		SAFE_DELETE(m_StateArray[i]);
	}
	if ( m_pLua )
	{
		lua_close(m_pLua);
	}
}

void 
MAAiBasilisk::_InitState()
{
	m_nPrevState = BasiliskState::NoneState;
	m_nState = BasiliskState::Ground_Stand;

	//_InitRegionDamage();
	LoadState("BasiliskAI.lua");

}

int
MAAiBasilisk::_FindCurPosition(const char* szActionName)
{
	std::string szAction;
	szAction = szActionName;

	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		if ( m_PositionStateTable[i].szActionName.empty() == false && 
			m_PositionStateTable[i].szActionName == szAction )
		{
			return m_PositionStateTable[i].nPosition;
		}
	}

	return -1;
}

void
MAAiBasilisk::LoadState(const char* szFile)
{

	_POS_STATE_TABLE _PosStateTable[] = 
	{
		// 상태 -------------------------------------------바실리스크위치----------
		{BasiliskState::NoneState,						BasiliskPosition::None,			std::string("")}, 
		{BasiliskState::Ground_Stand,					BasiliskPosition::Ground,		std::string("Ground_Stand") }, 
		{BasiliskState::Ground_TailAttack,				BasiliskPosition::Ground,		std::string("Ground_TailAttack") }, 
		{BasiliskState::Ground_BodyAttack,				BasiliskPosition::Ground,		std::string("Ground_BodyAttack") }, 
		{BasiliskState::Ground_RunAttack,				BasiliskPosition::Ground,		std::string("Ground_RunAttack") }, 
		{BasiliskState::Ground_StandUp,					BasiliskPosition::Ground,		std::string("Ground_StandUp") }, 
		{BasiliskState::Ground_StandDead,				BasiliskPosition::Ground,		std::string("Ground_StandDead") }, 

		{BasiliskState::Turn_Left,						BasiliskPosition::Ground,		std::string("Turn_Left") }, 
		{BasiliskState::Turn_Right,						BasiliskPosition::Ground,		std::string("Turn_Right") }, 
		{BasiliskState::FootAttack_Left,				BasiliskPosition::Ground,		std::string("FootAttack_Left") }, 
		{BasiliskState::FootAttack_Right,				BasiliskPosition::Ground,		std::string("FootAttack_Right") }, 

		{BasiliskState::OverTurned,						BasiliskPosition::OverTurned,		std::string("OverTurned") }, 
		{BasiliskState::OverTurned_Dead,				BasiliskPosition::OverTurned,		std::string("OverTurned_Dead") }, 
		{BasiliskState::OverTurned_Hit,					BasiliskPosition::OverTurned,		std::string("OverTurned_Hit") }, 

		{BasiliskState::Pillar_Climb,					BasiliskPosition::Pillar_Climb,		std::string("Pillar_Climb") }, 
		{BasiliskState::Pillar_Stand,					BasiliskPosition::Pillar,			std::string("Pillar_Stand") }, 
		{BasiliskState::Pillar_ThornAttack,				BasiliskPosition::Pillar,			std::string("Pillar_ThornAttack") }, 
		{BasiliskState::Pillar_LoseBalance,				BasiliskPosition::Pillar_LoseBalance,		std::string("Pillar_LoseBalance") }, 
		{BasiliskState::Pillar_KeepBalance,				BasiliskPosition::Pillar_KeepBalance,		std::string("Pillar_KeepBalance") }, 
		{BasiliskState::Pillar_Fall,					BasiliskPosition::Pillar,					std::string("Pillar_Fall") }, 
		{BasiliskState::Pillar_ComeDown,				BasiliskPosition::Pillar_ComeDown,			std::string("Pillar_ComeDown") }, 

		{BasiliskState::Vaulting_Climb,					BasiliskPosition::Vaulting_Climb,		std::string("Vaulting_Climb") }, 
		{BasiliskState::Vaulting_Stand,					BasiliskPosition::Vaulting,				std::string("Vaulting_Stand") }, 
		{BasiliskState::Vaulting_ThornAttack,			BasiliskPosition::Vaulting,				std::string("Vaulting_ThornAttack") }, 
		{BasiliskState::Vaulting_LoseBalance,			BasiliskPosition::Vaulting,				std::string("Vaulting_LoseBalance") }, 
		{BasiliskState::Vaulting_KeepBalance,			BasiliskPosition::Pillar,				std::string("Vaulting_KeepBalance") }, 
		{BasiliskState::Vaulting_Fall,					BasiliskPosition::Pillar,				std::string("Vaulting_Fall") }, 

		{BasiliskState::PlayAnimation,					BasiliskPosition::None,					std::string("") }, 
		{BasiliskState::StateMax,						BasiliskPosition::None,					std::string("") }, 
	};

	std::string szFullName;
	szFullName = g_PathMng.GetFullPathName( szFile ).c_str();

	m_pLua =  lua_open();
	luaL_openlibs(m_pLua); 
	if ( lua_tinker::dofile(m_pLua, szFullName.c_str()) != 0 )
	{
		lua_close(m_pLua);
		return;
	}

	lua_tinker::table keepuptable = lua_tinker::get<lua_tinker::table>(m_pLua, "g_lua_KeepupTimeTable");

	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		m_PositionStateTable[i] = _PosStateTable[i];
		switch(m_PositionStateTable[i].nPosition)
		{
		case BasiliskPosition::Ground:
			m_PositionStateTable[i].nKeepupTime = keepuptable.get<int>( "Ground" );
			break;
		case BasiliskPosition::Pillar:
			m_PositionStateTable[i].nKeepupTime = keepuptable.get<int>( "Pillar" );
			break;
		case BasiliskPosition::Vaulting:
			m_PositionStateTable[i].nKeepupTime = keepuptable.get<int>( "Vaulting" );
			break;
		case BasiliskPosition::OverTurned:
			m_PositionStateTable[i].nKeepupTime = keepuptable.get<int>( "OverTurned" );
			break;
		}
	}

	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		SAFE_DELETE(m_StateArray[i]);
	}

	m_StateArray[BasiliskState::NoneState]						= NULL;
	m_StateArray[BasiliskState::Ground_Stand]					= new Ground_Stand();
	m_StateArray[BasiliskState::Ground_TailAttack]				= new Ground_TailAttack();
	m_StateArray[BasiliskState::Ground_BodyAttack]				= new Ground_BodyAttack();
	m_StateArray[BasiliskState::Ground_RunAttack]				= new Ground_RunAttack();
	m_StateArray[BasiliskState::Ground_StandUp]					= new Ground_StandUp();
	m_StateArray[BasiliskState::Ground_StandDead]				= new Ground_StandDead();

	m_StateArray[BasiliskState::Turn_Left]						= new Turn_Left();
	m_StateArray[BasiliskState::Turn_Right]						= new Turn_Right();
	m_StateArray[BasiliskState::FootAttack_Left]				= new FootAttack_Left();
	m_StateArray[BasiliskState::FootAttack_Right]				= new FootAttack_Right();

	m_StateArray[BasiliskState::OverTurned]						= new OverTurned();
	m_StateArray[BasiliskState::OverTurned_Dead]				= new OverTurned_Dead();
	m_StateArray[BasiliskState::OverTurned_Hit]					= new OverTurned_Hit();

	m_StateArray[BasiliskState::Pillar_Climb]					= new Pillar_Climb();
	m_StateArray[BasiliskState::Pillar_Stand]					= new Pillar_Stand();
	m_StateArray[BasiliskState::Pillar_ThornAttack]				= new Pillar_ThornAttack();
	m_StateArray[BasiliskState::Pillar_LoseBalance]				= new Pillar_LoseBalance();
	m_StateArray[BasiliskState::Pillar_KeepBalance]				= new Pillar_KeepBalance();
	m_StateArray[BasiliskState::Pillar_Fall]					= new Pillar_Fall();
	m_StateArray[BasiliskState::Pillar_ComeDown]				= new Pillar_ComeDown();

	m_StateArray[BasiliskState::Vaulting_Climb]					= new Vaulting_Climb();
	m_StateArray[BasiliskState::Vaulting_Stand]					= new Vaulting_Stand();
	m_StateArray[BasiliskState::Vaulting_ThornAttack]			= new Vaulting_ThornAttack();
	m_StateArray[BasiliskState::Vaulting_LoseBalance]			= new Vaulting_LoseBalance();

	m_StateArray[BasiliskState::Vaulting_KeepBalance]			= new Vaulting_KeepBalance();
	m_StateArray[BasiliskState::Vaulting_Fall]					= new Vaulting_Fall();

	m_StateArray[BasiliskState::PlayAnimation]					= NULL;
	m_StateArray[BasiliskState::StateMax]						= NULL;

	for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		if ( m_StateArray[i] )
		{
			m_StateArray[i]->SetParent(this);
			m_StateArray[i]->LoadSetting();
			m_StateArray[i]->SetPositionState(m_PositionStateTable[i].nPosition);
		}
	}

}

/*
int 
MAAiBasilisk::_RegionDamage(const char* szRegion)
{

	// 데미지를 받은 부위가 체크해야할 부위라면. 
	REGION_DAMAGE_MAP_IT it = m_RegionDamageMap.find(std::string(szRegion));

	// 데미지 카운트를 계산해준다.
	if ( it != m_RegionDamageMap.end() )
	{
		REGION_INFO& region_info = (it->second);
		
		if ( region_info.nDamageCnt > 0 )
		{
			region_info.nDamageCnt--;
		}
	}

	// 데미지를 다 받은 부위의 갯수
	int nTotal = 0;
	it = m_Regi(Map.begin();
	for (  ; it != m_RegionDamageMap.end() ; ++it )
	{
		REGION_INFO& region_info = (it->second);

		if ( region_info.nDamageCnt < 1 )
			nTotal++;
	}

	CDnBasiliskMonsterActor* pActor = dynamic_cast<CDnBasiliskMonsterActor*>(m_hActor.GetPointer());
	if ( pActor )
	{
		it = m_RegionDamageMap.begin();
		for (  ; it != m_RegionDamageMap.end() ; ++it )
		{
			REGION_INFO& region_info = (it->second);
			if ( region_info.nDamageCnt < DAMAGE_CNT ) 
			{
				int nCnt = DAMAGE_CNT - region_info.nDamageCnt; 
				float f = 1.0f / DAMAGE_CNT;

				EtColor Color( float(nCnt)*f, 0.0f, 0.0f, 1.0f );
				pActor->SetMeshDiffuseColor(region_info.szSubMeshName.c_str(), Color);
			}
		}
	}

	return nTotal;
}
*/

void	
MAAiBasilisk::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MAAiBase::Process( LocalTime, fDelta );
	// 초기화 안되어있을 경우에... 
	if ( m_LocalTime == 0 )
	{
		// 현재 시간을 세팅해준다.
		for ( int i = 0 ; i < BasiliskState::StateMax ; i++ )
		{
			if ( m_StateArray[i] )
			{
				m_StateArray[i]->Init(LocalTime);
			}
		}
	}

	m_LocalTime = LocalTime;
	_OnAI("", LocalTime);
}

void
MAAiBasilisk::OnFinishAction(const char* szPrevAction, LOCAL_TIME time)
{
	_OnAI(szPrevAction, time);
}


void	
MAAiBasilisk::OnDamage(DnActorHandle hActor, CDnActor::SHitParam &HitParam, LOCAL_TIME time) 
{
	// 현재 데미지들 갱신
	CDnBasiliskMonsterActor* pBoss = dynamic_cast<CDnBasiliskMonsterActor*>(m_hActor.GetPointer());
	if ( pBoss )
	{
		m_CurrentInfo.nCurHP = pBoss->GetHP();
		m_CurrentInfo.nMaxHP = pBoss->GetMaxHP();
	}

	if ( pBoss->IsLoseBalance() )
	{
		switch( m_nCurPosition )
		{
		case BasiliskPosition::Ground:
			break;
		case BasiliskPosition::Pillar:
			{
				std::string szAction = m_hActor->GetCurrentPlayAction();
				if ( szAction == "Pillar_Stand" ||
					szAction == "Pillar_ThornAttack" )
				{
					m_nState = BasiliskState::Pillar_LoseBalance;

					// 다리를 많이 맞앗으면 밸런스를 잃어버린다.
					if ( m_StateArray[m_nState] )
					{
						m_StateArray[m_nState]->BasiliskStateBase::OnState(time);
						DWORD nState = m_StateArray[m_nState]->OnState(time);
						ChangeState(nState);
					}

				}
			}
			break;
		case BasiliskPosition::Vaulting:
			{
				std::string szAction = m_hActor->GetCurrentPlayAction();
				if ( szAction == "Vaulting_Stand" ||
					szAction == "Vaulting_ThornAttack" )
				{
					m_nState = BasiliskState::Vaulting_LoseBalance;

					// 다리를 많이 맞앗으면 밸런스를 잃어버린다.
					if ( m_StateArray[m_nState] )
					{
						m_StateArray[m_nState]->BasiliskStateBase::OnState(time);
						DWORD nState = m_StateArray[m_nState]->OnState(time);
						ChangeState(nState);
					}

				}
			}
			break;
		}
	}
	
	

	

}


void	
MAAiBasilisk::OnDie(LOCAL_TIME LocalTime)
{
	m_CurrentAnimationArray.clear();
	m_nCurrentAnimationIndex = 0;

	// 죽었을때.. 현재 위치별로 재생할 애니메이션 리스트를 세팅한다.
	int nPos = _FindCurPosition( m_hActor->GetCurrentPlayAction().c_str() );
	
	switch( nPos )
	{
	case BasiliskPosition::Ground: 
		{
			m_CurrentAnimationArray = __gs_Table_DyingGround;
		}
		break;
	case BasiliskPosition::Pillar: 
	case BasiliskPosition::Pillar_Climb:
		{
			m_CurrentAnimationArray = __gs_Table_DyingPillar;
		}
		break;
	case BasiliskPosition::Vaulting: 
		{
			m_CurrentAnimationArray = __gs_Table_DyingVaulting;
		}
		break;
	case BasiliskPosition::OverTurned:
		{
			m_CurrentAnimationArray = __gs_Table_DyingOverTurned;
		}
		break;
	case BasiliskPosition::Pillar_LoseBalance:
		{
			m_CurrentAnimationArray = __gs_Table_DyingPillar_LoseBalance;

		}
		break;
	default:
		{
			m_CurrentAnimationArray = __gs_Table_DyingGround;
		}
		break;
	}

	// 죽었으면 애니메이션 상태로 변경후 바로 진행한다.
	ChangeState(BasiliskState::PlayAnimation);
	_OnPlayAnimation();
}


void 
MAAiBasilisk::ChangeState(DWORD nState)
{
	m_nPrevState = m_nState;
	m_nState = nState;

	if ( m_nCurPosition != m_PositionStateTable[m_nState].nPosition )
	{
		m_nCurPosStateTime = m_LocalTime;
	}
	m_nCurPosition = m_PositionStateTable[m_nState].nPosition;
	
}

bool	
MAAiBasilisk::IsPassedKeepupTime() 
{
	LOCAL_TIME nElapsedTime = GetPosStateElapsedTime();
	if ( m_PositionStateTable[m_nState].nKeepupTime <= nElapsedTime )
		return true;
	return false;
}


void 
MAAiBasilisk::CmdAction(const char *szActionName, int nLoopCount /* = 0 */ , float fBlendFrame /* = 3.f */ )
{

	m_hActor->CmdAction(szActionName, nLoopCount, fBlendFrame);
	return;

}

void 
MAAiBasilisk::_OnAI(const char* szPrevAction, LOCAL_TIME time)
{
	// 바실리스크 정보들 채워 놓는다.
	m_CurrentInfo.hActor = m_hActor;
	m_CurrentInfo.hTargetActor = m_hTarget;
	m_CurrentInfo.nCurTime = time;
	m_CurrentInfo.nPosState = m_PositionStateTable[m_nState].nPosition;
	m_CurrentInfo.nState = m_nState;


	CDnBasiliskMonsterActor* pBoss = dynamic_cast<CDnBasiliskMonsterActor*>(m_hActor.GetPointer());
	if ( pBoss )
	{
		m_CurrentInfo.nCurHP = pBoss->GetHP();
		m_CurrentInfo.nMaxHP = pBoss->GetMaxHP();
	}


	if ( __NULL_STR != m_hActor->GetCurrentAction() )		return;		

	switch(m_nState)
	{
	case BasiliskState::PlayAnimation:
		_OnPlayAnimation();
		return;
	}

	if ( m_StateArray[m_nState] )
	{
		m_StateArray[m_nState]->BasiliskStateBase::OnState(time);
		DWORD nState = m_StateArray[m_nState]->OnState(time);

		// 각스테이트에서 임의로 정해준게 잇다면 그걸 적용한다.
		if ( nState > 0 )
		{
			ChangeState(nState);
			return;
		}

		// 포지션 유지 시간이 지났다면 기어오르거나 기어 내려간다.
		if ( IsPassedKeepupTime() )
		{
			switch(m_PositionStateTable[m_nState].nPosition)
			{
			case BasiliskPosition::Ground:			ChangeState(BasiliskState::Pillar_Climb);			return;
			case BasiliskPosition::Pillar:			ChangeState(BasiliskState::Vaulting_Climb);			return;
			case BasiliskPosition::Vaulting:		ChangeState(BasiliskState::Pillar_ComeDown);		return;
			case BasiliskPosition::OverTurned:		ChangeState(BasiliskState::Ground_StandUp);			return;
			}
		}

		

	}

	//-----------------------------------------------------------------
	// 사용 가능한 기술들을 체크 해서 그중에서 랜덤으로 하나 고른다.
	std::vector<DWORD>	vecStateList;
	
	// 각 스테이트들이 사용 가능한지 검증해서 vector 에 담는ㄷ.
	for ( DWORD i = 0 ; i < BasiliskState::StateMax ; i++ )
	{
		if( m_StateArray[i] )
		{
			if ( m_StateArray[i]->BasiliskStateBase::Verify(m_CurrentInfo) )
			{
				if ( m_StateArray[i]->Verify(m_CurrentInfo)  )
				{
					vecStateList.push_back(i);
				}	
			}
		}
	}

	// 사용 가능한 스테이트들중 랜덤으로 고른다.
	int nSize = (int)vecStateList.size();
	if ( nSize > 0 )
	{
		DWORD newState = vecStateList[(rand()%nSize)];
		m_CurrentInfo.nState = m_nState;
		ChangeState(newState);
	}

	// 사용가능한게 없으면 그냥 있는다.
	else
	{
		switch(m_nCurPosition)
		{
		case BasiliskPosition::Ground:		ChangeState(BasiliskState::Ground_Stand);	break;
		case BasiliskPosition::Pillar:		ChangeState(BasiliskState::Pillar_Stand);	break;
		case BasiliskPosition::Vaulting:	ChangeState(BasiliskState::Vaulting_Stand);	break;
		case BasiliskPosition::OverTurned:	ChangeState(BasiliskState::OverTurned);		break;
		default:
			int a = 0;
			break;
		}
	}

	if ( m_nPrevState != m_nState )
	{
		if ( m_StateArray[m_nPrevState] )
			m_StateArray[m_nPrevState]->OnEndState(time);
	}

	if ( m_nPrevState != m_nState )
	{
		if ( m_StateArray[m_nState] )
			m_StateArray[m_nState]->OnStartState(time);
	}
}



void 
MAAiBasilisk::_OnPlayAnimation()
{
	// 애니메이션 재생 다했다면 이제 숨지는거임. 완전히..
	if ( m_nCurrentAnimationIndex >= m_CurrentAnimationArray.size() )
	{
		ChangeState(BasiliskState::NoneState);
		m_hActor->SetDieDelta(9.0f);
	}

	else
	{
		std::string szAction = m_CurrentAnimationArray[m_nCurrentAnimationIndex];
		if ( m_nCurrentAnimationIndex < 1 )
		{

			CmdAction(szAction.c_str(), 0, 0.0f);
		}
		else
		{
			CmdAction(szAction.c_str(), 0, 0.0f);
		}

		m_nCurrentAnimationIndex++;
	}

}


float
MAAiBasilisk::GetDegree(DnActorHandle hActor, IN const EtVector3& AxisVector )
{
	EtVector3 v = *(hActor->GetPosition()) - *(m_hActor->GetPosition());
	v.y = 0.0f;

	EtVec3Normalize(&v, &v);
	float fDot = EtVec3Dot(&v, &AxisVector );

	return EtToDegree(acosf(fDot));
}


bool 
MAAiBasilisk::ScanEnermy(float fDist, OUT std::vector<DnActorHandle>& EnermyList )
{
	std::vector<DnActorHandle> hVecList;
	EtVector3 *pvPos = m_hActor->GetPosition();
	int nCount = CDnActor::ScanActor( *pvPos, fDist, hVecList );

	if( nCount > 1 )
	{
		std::vector<DnActorHandle> hVecTargetResult;
		for( DWORD i=0; i<hVecList.size(); i++ ) 
		{

			if( hVecList[i]->GetTeam() == m_hActor->GetTeam() )
				continue;
			if( hVecList[i]->IsDie() ) 
				continue;
			EnermyList.push_back( hVecList[i] );
		}

		if ( EnermyList.size() )
			return true;

	}
	return false;
}

// 적이 어느방향에 있는지 결정한다.
int 
MAAiBasilisk::GetEnermyDir()
{
	std::vector<DnActorHandle> EnermyList;
	bool bResult = ScanEnermy(3000.0f, EnermyList);

	// 적이 근처에 없으면
	if ( bResult == false  )
		return EnermyDir::None;

	// 각 방향별 인원수
	int nFrontCnt = 0;
	int nBehindCnt = 0;
	int nLeftCnt = 0;
	int nRightCnt = 0;

	std::vector<DnActorHandle>::iterator it;
	it = EnermyList.begin();
	for ( ; it != EnermyList.end() ; ++it )
	{
		DnActorHandle hActor = *it;
		int nDir = WhereisActor(hActor);
		switch( nDir )
		{
		case EnermyDir::Front:  nFrontCnt++; 	break;
		case EnermyDir::Left:   nLeftCnt++;		break;
		case EnermyDir::Right:  nRightCnt++; 	break;
		case EnermyDir::Behind: nBehindCnt++;	break;
		}
	}

	struct __DirCnt 
	{
		int nDir;
		int nCnt;
	};

	std::map<int, __DirCnt> __Cntmap;
	__DirCnt n;
	n.nDir = EnermyDir::Front;
	n.nCnt = nFrontCnt;
	__Cntmap.insert(std::pair<int,__DirCnt>(nFrontCnt, n) );

	n.nDir = EnermyDir::Left;
	n.nCnt = nLeftCnt;
	__Cntmap.insert(std::pair<int,__DirCnt>(nLeftCnt, n) );

	n.nDir = EnermyDir::Right;
	n.nCnt = nRightCnt;
	__Cntmap.insert(std::pair<int,__DirCnt>(nRightCnt, n) );

	n.nDir = EnermyDir::Behind;
	n.nCnt = nBehindCnt;
	__Cntmap.insert(std::pair<int,__DirCnt>(nBehindCnt, n) );

	std::map<int, __DirCnt>::reverse_iterator r_it;
	r_it = __Cntmap.rbegin();
	return (r_it->second).nDir;
}

int 
MAAiBasilisk::WhereisActor(DnActorHandle hActor)
{
	// 정면 검사
	float fDegree = GetDegree(hActor, -(m_hActor->GetCross()->m_ZVector));
	// 정면 60도 내에 있다면 
	if ( fDegree <= 30.0f )
	{
		return EnermyDir::Front;
	}
	// 후방 60도 내에 있다면.
	else if ( fDegree >= 150.0f && fDegree <= 180.0f )
	{
		return EnermyDir::Behind;
	}

	// 측면 검사
	fDegree = GetDegree(hActor, -(m_hActor->GetCross()->m_XVector));

	// 우측면 120도 내에 있다면 
	if ( fDegree <= 60.0f )
	{
		return EnermyDir::Right;
	}
	// 좌측면 120도 내에 있다면 
	else if ( fDegree >= 120.0f && fDegree <= 180.0f )
	{
		return EnermyDir::Left;
	}

	return EnermyDir::None;
}