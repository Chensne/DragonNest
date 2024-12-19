#include "stdafx.h"
#include "DnRevengeTask.h"
#include "TaskManager.h"
#include "DnBridgeTask.h"
#include "DnPartyTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ADD_REVENGE )

CDnRevengeTask::CDnRevengeTask()
: CTaskListener( false )
, m_uiMyRevengeID( 0 )
{

}

CDnRevengeTask::~CDnRevengeTask()
{

}

void CDnRevengeTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;

	switch( nMainCmd )
	{
	case SC_PVP:	_OnRecvPVPSkillMessage( nSubCmd, pData, nSize ); bProcessDispatch = true; break;
	}

	if( bProcessDispatch ) 
	{
		WriteLog( 1, ", Info, CDnRevengeTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnRevengeTask::_OnRecvPVPSkillMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
	case ePvP::SC_PVP_SET_REVENGE_TARGET:	OnRecvSetRevengeTarget( (SCPvPSetRevengeTarget*)pData ); break;
	case ePvP::SC_PVP_SUCCESS_REVENGE:		OnRecvSuccessRevenge( (SCPvPSuccessRevenge*)pData ); break;
	}
}

void CDnRevengeTask::OnRecvSetRevengeTarget( SCPvPSetRevengeTarget * pData )
{
	if( NULL == pData )
		return;

	SetRevengeUser( *pData );
}

void CDnRevengeTask::OnRecvSuccessRevenge( SCPvPSuccessRevenge * pData )
{
	RevengeMessage( pData->uiSessionID, pData->uiRevengeTargetSessionID, Revenge::TargetReason::SuccessRevenge );
}

bool CDnRevengeTask::Initialize()
{
	return true;
}

void CDnRevengeTask::ClearData()
{
	m_uiMyRevengeID = 0;

	m_mapRevenge_User.clear();
	m_mapRevenge_Target.clear();
}

void CDnRevengeTask::ClearMyRevengeUser()
{
	m_uiMyRevengeID = 0;
}

UINT CDnRevengeTask::GetMyRevengeUser()
{
	return m_uiMyRevengeID;
}

void CDnRevengeTask::SetMyRevengeUser( UINT uiRevengeUserID )
{
	m_uiMyRevengeID = uiRevengeUserID;
}

void CDnRevengeTask::SetRevengeUser( const SCPvPSetRevengeTarget & sData )
{
	RevengeMessage( sData.uiSessionID, sData.uiRevengeTargetSessionID, sData.eReason);

	if( Revenge::TargetReason::SetNewRevengeTarget == sData.eReason )
	{
		m_mapRevenge_User.insert(std::make_pair(sData.uiSessionID, sData.uiRevengeTargetSessionID ) );
		m_mapRevenge_Target.insert(std::make_pair(sData.uiRevengeTargetSessionID, sData.uiSessionID ) );
	}
	else if( Revenge::TargetReason::SuccessRevenge == sData.eReason 
			|| Revenge::TargetReason::TargetLeaveRoom == sData.eReason 
			|| Revenge::TargetReason::TargetLeaveGame == sData.eReason 
			|| Revenge::TargetReason::NotPartedInPlay == sData.eReason )
	{
		std::map<UINT, UINT>::iterator Itor = m_mapRevenge_User.find( sData.uiSessionID );
		if( m_mapRevenge_User.end() != Itor )
			m_mapRevenge_User.erase( Itor );

		for( Itor = m_mapRevenge_Target.begin(); Itor != m_mapRevenge_Target.end(); ++Itor )
		{
			if( sData.uiSessionID == Itor->second )
			{
				Itor = m_mapRevenge_Target.erase( Itor );
				if( m_mapRevenge_Target.end() == Itor )
					break;
			}
		}
	}
	else if( Revenge::TargetReason::ResetRevengeTarget == sData.eReason )
		ClearData();

	if( CDnBridgeTask::GetInstance().GetSessionID() == sData.uiSessionID )
		m_uiMyRevengeID = sData.uiRevengeTargetSessionID;
}

void CDnRevengeTask::GetRevengeUserID( const UINT uiSessionID, UINT & eRevenge )
{
	eRevenge = Revenge::RevengeTarget::eRevengeTarget_None;

	std::map<UINT, UINT>::iterator Itor = m_mapRevenge_Target.find( uiSessionID );
	if( m_mapRevenge_Target.end() != Itor )
		eRevenge = GetRevengeTargetType( Itor->first );
}

UINT CDnRevengeTask::GetRevengeTargetType( const UINT uiRevengeUserID )
{
	if( 0 == uiRevengeUserID )
		return Revenge::RevengeTarget::eCode::eRevengeTarget_None;

	const UINT uiMySessionID = CDnBridgeTask::GetInstance().GetSessionID();

	if( uiRevengeUserID == m_uiMyRevengeID )
		return Revenge::RevengeTarget::eCode::eRevengeTarget_Target;
	//else if( uiRevengeUserID == uiMySessionID )
	//	return Revenge::RevengeTarget::eCode::eRevengeTarget_Me;

	return Revenge::RevengeTarget::eCode::eRevengeTarget_None;
}

void CDnRevengeTask::RevengeMessage( const UINT uiSessionID, const UINT uiRevengeTargetSessionID, const Revenge::TargetReason::eCode eReason )
{
	CDnPartyTask * pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( NULL == pPartyTask )
		return;

	WCHAR wszString[512] = {0,};
	const UINT uiMySessionID = CDnBridgeTask::GetInstance().GetSessionID();

	if( Revenge::TargetReason::SetNewRevengeTarget == eReason )
	{
		if( uiMySessionID == uiSessionID )
		{
			CDnPartyTask::PartyStruct * pRevenge_User = pPartyTask->GetPartyDataFromSessionID( uiRevengeTargetSessionID );

			if( NULL == pRevenge_User )
				return;

			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7840 ), pRevenge_User->wszCharacterName, pRevenge_User->wszCharacterName );	// UISTRING : %s 님에게 3회 죽었습니다. %s 님 처치에 도전하세요!
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, true );
		}
		/*else if( uiMySessionID == uiRevengeTargetSessionID )
		{
			CDnPartyTask::PartyStruct * pDie_User = pPartyTask->GetPartyDataFromSessionID( uiSessionID );

			if( NULL == pDie_User )
				return;

			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7841 ), pDie_User->wszCharacterName, pDie_User->wszCharacterName );	// UISTRING : %s 님을 3회 처치했습니다. %s 님의 눈초리가 심상치 않습니다.
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, true );
		}*/
	}
	else if( 0 != uiRevengeTargetSessionID && Revenge::TargetReason::SuccessRevenge == eReason )
	{
		CDnPartyTask::PartyStruct * pHit_User = pPartyTask->GetPartyDataFromSessionID( uiSessionID );
		CDnPartyTask::PartyStruct * pDie_User = pPartyTask->GetPartyDataFromSessionID( uiRevengeTargetSessionID );

		if( NULL == pHit_User || NULL == pDie_User )
			return;

		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7849 ), pHit_User->wszCharacterName, pDie_User->wszCharacterName );	// UISTRING : %s 님이 %s 님에게 복수를 가했습니다!!
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, true );
	}
}
#endif	// #if defined( PRE_ADD_REVENGE )