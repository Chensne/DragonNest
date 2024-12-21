#include "Stdafx.h"
#if 0 
#include "BasiliskState.h"
#include "MAAiBasilisk.h"

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

	return BasiliskState::Ground_Stand;
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
	return BasiliskState::Pillar_ThornAttack;
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

	float fCurHp = (float)m_pBasilisk->GetCurrentInfo().nCurHP;
	float fMaxHp = (float)m_pBasilisk->GetCurrentInfo().nMaxHP;

	fMaxHp*=0.50f;
	// 밸런스를 잃었을때 체력이 30프로 이하면 떨어진다.
	if ( fMaxHp >= fCurHp )
	{
		return BasiliskState::Pillar_Fall;
	}

	// 아니면 다시 기둥에 붙는다.
	return BasiliskState::Pillar_KeepBalance;

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
	return 0;
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
#endif