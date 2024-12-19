#include "StdAfx.h"
#include "DnPropActor.h"
#include "DnSkill.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef _GAMESERVER
CDnPropActor::CDnPropActor( CMultiRoom* pRoom, int nClassID ) : CDnActor( pRoom, nClassID )
#else
CDnPropActor::CDnPropActor( int nClassID, bool bProcess ) : CDnActor( nClassID, bProcess )
#endif
{
	
}

CDnPropActor::~CDnPropActor(void)
{
	//SAFE_RELEASE_SPTRVEC( m_vlhSkillList );
}


bool CDnPropActor::Initialize( void )
{
	CDnActorState::Initialize( m_nClassID );
	CDnActor::Initialize();

	return true;
}

//
//bool CDnPropActor::AddPropSkill( int iSkillID, int iSkillLevelID )
//{
//	bool bResult = false;
//
//	DnSkillHandle hSkill = CDnSkill::CreateSkill( GetMySmartPtr(), iSkillID, iSkillLevelID );
//	if( hSkill )
//	{
//		m_vlhSkillList.push_back( hSkill );
//		bResult = true;
//	}
//
//	return bResult;
//}
//
//
//DnSkillHandle CDnPropActor::UsePropSkill( int iSkillID, LOCAL_TIME LocalTime, float fDelta )
//{
//	DnSkillHandle hUseSkill;
//
//	int iNumSkills = (int)m_vlhSkillList.size();
//	for( int iSkill = 0; iSkill < iNumSkills; ++iSkill )
//	{
//		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );
//		if( hSkill->GetClassID() == iSkillID )
//		{
//			hSkill->OnBegin( LocalTime, fDelta );
//
//			// NOTE: MASkillUser의 m_hProcessSkill 을 현재 프랍의 스킬로 바꿔준다.
//			// 알아서 CDnActor 쪽에서 Processing 된다.
//			MASkillUser::m_hProcessSkill = hSkill;
//			hUseSkill = hSkill;
//			break;
//		}
//	}
//
//	return hUseSkill;
//}


// 추후에 스킬을 쓰는 경우 여기서 패킷 받아서 처리 해줌.
//void CDnPropActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
//{
//	
//}