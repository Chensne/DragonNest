#include "Stdafx.h"
#include "DnMasterTask.h"
#include "MasterSendPacket.h"
#include "DnInterface.h"
#include "DnAcceptRequestDlg.h"
#include "DnGaugeDlg.h"
#include "DnPartyTask.h"
#include "SyncTimer.h"
#include "DnLocalPlayerActor.h"
#include "DnChatRoomDlg.h"
#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_SECONDARY_SKILL
#include "DnInterfaceString.h"
#include "DnTableDB.h"

CDnMasterTask::CDnMasterTask()
: CTaskListener( false )
, m_bIntroductionRegist( false )
, m_bAcceptRequest( false )
, m_fAcceptTime( 0.0f )
, m_nAcceptType( 0 )
, m_iPenalyRespectPoint( 0 )
, m_iRespectPoint( 0 )
, m_cLevel( 0 )
, m_cJob( 0 )
{
	SecureZeroMemory( &m_MasterCharacterInfo, sizeof(m_MasterCharacterInfo) );
	SecureZeroMemory( &m_PartyCountInfo, sizeof(m_PartyCountInfo) );
	SecureZeroMemory( &m_SimpleInfo, sizeof(m_SimpleInfo) );
}
	
CDnMasterTask::~CDnMasterTask()
{
}

void CDnMasterTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd ) {
		case SC_MASTERSYSTEM : OnRecvMasterSystemMessage(nSubCmd, pData, nSize); break;
	}
}

void CDnMasterTask::OnRecvMasterSystemMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ){
		case eMasterSystem::SC_SIMPLEINFO :				OnRecvMasterSimpleInfo( (SCSimpleInfo *)pData );				break;
		case eMasterSystem::SC_MASTERLIST :				OnRecvMasterList( (SCMasterList *)pData );						break;
		case eMasterSystem::SC_MASTERCHARACTERINFO :	OnRecvMasterCharacterInfo( (SCMasterCharacterInfo *)pData );	break;
		case eMasterSystem::SC_PUPILLIST :				OnRecvMasterPupilList( (SCPupilList *)pData );					break;
		case eMasterSystem::SC_INTRODUCTION_ONOFF :		OnRecvMasterIntroduction( (SCIntroduction *)pData );			break;
		case eMasterSystem::SC_MASTER_APPLICATION :		OnRecvMasterApplication( (SCMasterApplication *)pData );		break;
		case eMasterSystem::SC_JOIN :					OnRecvMasterJoin( (SCJoin *)pData );							break;
		case eMasterSystem::SC_MASTERANDCLASSMATE :		OnRecvMasterAndClassmate( (SCMasterClassmate *)pData );			break;
		case eMasterSystem::SC_MYMASTERINFO :			OnRecvMyMasterInfo( (SCMyMasterInfo *)pData );					break;
		case eMasterSystem::SC_CLASSMATEINFO :			OnRecvClassmateInfo( (SCClassmateInfo *)pData );				break;
		case eMasterSystem::SC_LEAVE :					OnRecvLeave( (SCLeave *)pData );								break;
		case eMasterSystem::SC_INVITE_PUPIL :			OnRecvInvitePupil( (SCInvitePupil *)pData );					break;
		case eMasterSystem::SC_INVITE_PUPIL_CONFIRM :	OnRecvInvitePupilConfirm( (SCInvitePupilConfirm *)pData );		break;
		case eMasterSystem::SC_JOIN_DIRECT :			OnRecvJoinDirect( (SCJoinDirect *)pData );						break;
		case eMasterSystem::SC_JOIN_DIRECT_CONFIRM :	OnRecvJoinDirectConfirm( (SCJoinDirectConfirm *)pData );		break;
		case eMasterSystem::SC_GRADUATE :				OnRecvGraduate( (SCGraduate *)pData );							break;
		case eMasterSystem::SC_COUNTINFO :				OnRecvCountInfo( (SCCountInfo *)pData );						break;
		case eMasterSystem::SC_RESPECTPOINT :			OnRecvRespectPoint( (SCRespectPoint *)pData );					break;
		case eMasterSystem::SC_FAVORPOINT :				OnRecvFavorPoint( (SCFavorPoint *)pData );						break;
		case eMasterSystem::SC_RECALL_MASTER :			OnRecvRecallMaster( (SCRecallMaster *)pData );					break;
		case eMasterSystem::SC_CONNECT :				OnRecvConnect( (SCConnect *)pData );							break;
		case eMasterSystem::SC_JOIN_COMFIRM :			OnRecvJoinConfirm( (SCJoinComfirm *)pData );					break;
	}
}

void CDnMasterTask::OnRecvMasterSimpleInfo( SCSimpleInfo * pPacket )
{
	memset( &m_SimpleInfo, 0, sizeof(m_SimpleInfo) );
	m_SimpleInfo.iMasterCount			= pPacket->SimpleInfo.iMasterCount;
	m_SimpleInfo.iPupilCount			= pPacket->SimpleInfo.iPupilCount;
	m_SimpleInfo.iGraduateCount			= pPacket->SimpleInfo.iGraduateCount;
	m_SimpleInfo.BlockDate				= pPacket->SimpleInfo.BlockDate;
	m_SimpleInfo.cCharacterDBIDCount	= pPacket->SimpleInfo.cCharacterDBIDCount;

	int nCount = m_SimpleInfo.iMasterCount != 0 ? m_SimpleInfo.iMasterCount : m_SimpleInfo.iPupilCount;

	for( int itr = 0; itr < nCount; ++itr )
	{
		m_SimpleInfo.OppositeInfo[itr].CharacterDBID = pPacket->SimpleInfo.OppositeInfo[itr].CharacterDBID;
		_wcscpy( m_SimpleInfo.OppositeInfo[itr].wszCharName, _countof(m_SimpleInfo.OppositeInfo[itr].wszCharName), pPacket->SimpleInfo.OppositeInfo[itr].wszCharName, (int)wcslen(pPacket->SimpleInfo.OppositeInfo[itr].wszCharName) );
		m_SimpleInfo.OppositeInfo[itr].nFavorPoint = pPacket->SimpleInfo.OppositeInfo[itr].nFavorPoint;
	}
}

void CDnMasterTask::OnRecvMasterList( SCMasterList * pPacket )
{
	m_MasterList.cCount = pPacket->cCount;
	for( BYTE itr = 0; itr < m_MasterList.cCount; ++itr )
	{
		m_MasterList.MasterInfoList[itr].biCharacterDBID = pPacket->MasterInfoList[itr].biCharacterDBID;
		m_MasterList.MasterInfoList[itr].cLevel = pPacket->MasterInfoList[itr].cLevel;
		m_MasterList.MasterInfoList[itr].cJobCode = pPacket->MasterInfoList[itr].cJobCode;
		m_MasterList.MasterInfoList[itr].cGenderCode = pPacket->MasterInfoList[itr].cGenderCode;
		m_MasterList.MasterInfoList[itr].iRespectPoint = pPacket->MasterInfoList[itr].iRespectPoint;
		m_MasterList.MasterInfoList[itr].iGraduateCount = pPacket->MasterInfoList[itr].iGraduateCount;
		m_MasterList.MasterInfoList[itr].wszCharName = wstring( pPacket->MasterInfoList[itr].wszCharName );
		m_MasterList.MasterInfoList[itr].wszSelfIntroduction = wstring( pPacket->MasterInfoList[itr].wszSelfIntrodution );
	}
	
	GetInterface().RefreshMasterListDialog();
}

void CDnMasterTask::OnRecvMasterCharacterInfo( SCMasterCharacterInfo * pPacket )
{
	m_MasterCharacterInfo.bMasterListShowFlag = pPacket->MasterCharacterInfo.bMasterListShowFlag;
	m_MasterCharacterInfo.iGraduateCount = pPacket->MasterCharacterInfo.iGraduateCount;
	m_MasterCharacterInfo.iRespectPoint = pPacket->MasterCharacterInfo.iRespectPoint;
	_wcscpy( m_MasterCharacterInfo.wszSelfIntroduction, _countof(m_MasterCharacterInfo.wszSelfIntroduction), pPacket->MasterCharacterInfo.wszSelfIntroduction, (int)wcslen(pPacket->MasterCharacterInfo.wszSelfIntroduction));

	m_bIntroductionRegist = m_MasterCharacterInfo.bMasterListShowFlag;

	GetInterface().RefreshMasterCharacterInfo();
}

void CDnMasterTask::OnRecvMasterPupilList( SCPupilList * pPacket )
{
	memset( &m_PupilList, 0, sizeof(m_PupilList) );
	if( pPacket->iRet == ERROR_NONE )
	{
		m_PupilList.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;
		m_PupilList.cCount					= pPacket->cCount;
		for( UINT i=0 ; i < pPacket->cCount ; ++i )
		{
			m_PupilList.PupilInfoList[i] = pPacket->PupilInfoList[i];
			_wcscpy( m_PupilList.PupilInfoList[i].wszCharName, _countof(m_PupilList.PupilInfoList[i].wszCharName), pPacket->PupilInfoList[i].wszCharName, (int)wcslen(pPacket->PupilInfoList[i].wszCharName) );
		}
	}

	GetInterface().RefreshPupilList();
}

void CDnMasterTask::OnRecvMasterIntroduction( SCIntroduction * pPacket )
{
	if( pPacket->bRegister )
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7254 ), false );	//소개가 등록 하였습니다.
	else
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7255 ), false );	//소개가 해제 하였습니다.

	m_bIntroductionRegist = pPacket->bRegister;
	_wcscpy( m_MasterCharacterInfo.wszSelfIntroduction, _countof(m_MasterCharacterInfo.wszSelfIntroduction), pPacket->wszSelfIntroduction, (int)wcslen(pPacket->wszSelfIntroduction) );

	GetInterface().RefreshMasterCharacterInfo();
}

void CDnMasterTask::OnRecvMasterApplication( SCMasterApplication * pPacket )
{
	if( ERROR_MASTERSYSTEM_MASTERUSER_OFFLINE == pPacket->iRet )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7242 ), MB_OK );
	}
	else if( ERROR_MASTERSYSTEM_MASTERAPPLICATION_PENALTY == pPacket->iRet )
	{
		WCHAR wszStringDate[256];
		WCHAR wszStringMsg[256];

		__time64_t timeDelta = m_SimpleInfo.BlockDate - CSyncTimer::GetInstance().GetCurTime();
		INT64 biHourDiff = timeDelta / 3600;	//시간 단위로 변환
		INT64 biDayDiff = biHourDiff / 24;		//날짜 단위로 변환
		INT64 biMinDiff = timeDelta / 60;		//분   단위로 변환
		biHourDiff = biHourDiff % 24;			//시간으로 다시 변환
		biMinDiff = biMinDiff % 60;				//분으로 다시 변환

		int iDay  = (int)biDayDiff;
		int iHour = (int)biHourDiff;
		int iMin  = (int)biMinDiff;

		//디비상 시간의 오차가 생겨 3일이 조금 넘을 수있다.
		if( iDay < MasterSystem::Penalty::PupilLeavePenaltyDay )
			swprintf_s( wszStringDate, _countof(wszStringDate), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7251 ), iDay, iHour, iMin );	//[남은 시간 : %d 일 %d 시 %d 분]
		else
			swprintf_s( wszStringDate, _countof(wszStringDate), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7251 ), MasterSystem::Penalty::PupilLeavePenaltyDay, 0, 0 );	//[남은 시간 : %d 일 %d 시 %d 분]

		swprintf_s( wszStringMsg, L"%s\n%s"
					, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7250 )	//	당신께서는 스승 삭제를 하여, 현재 스승 등록 불가 상태입니다.
					, wszStringDate );

		GetInterface().MessageBox( wszStringMsg, MB_OK );
	}
}

void CDnMasterTask::OnRecvMasterJoin( SCJoin * pPacket )
{
	if( ERROR_NONE != pPacket->iRet )
	{
		if( ERROR_GENERIC_LEVELLIMIT == pPacket->iRet )
		{
			WCHAR wszMessage[512];
			int iWeight = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::LimitLevel_MasterSystem);
			swprintf_s( wszMessage, _countof(wszMessage), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7296 ), iWeight );	// 사제 기능은 %d 레벨 이후 사용 가능합니다.

			GetInterface().MessageBox( wszMessage );
		}
		if( ERROR_MASTERSYSTEM_JOIN_DENY == pPacket->iRet )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7271 ), false );	// 스승 신청을 거절하였습니다.
		else if( ERROR_MASTERSYSTEM_CANT_JOINSTATE == pPacket->iRet)
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7285 ), false );	// 현재 해당 유저는 스승 신청을 요청할 수 없는 상태입니다.
		DebugLog( "스승신청 실패" );
		return;
	}

	if( pPacket->bIsAddPupil )	//스승일 경우
	{
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7240 ), false );	//제자가 등록 되었습니다.
		RequestMasterCharacterInfo();
	}
	else	//제자일 경우 
	{
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7241 ), false );	//스승이 등록 되었습니다.
		RequestMasterAndClassmate();
	}
}

void CDnMasterTask::OnRecvMasterAndClassmate( SCMasterClassmate * pPacket )
{
	m_pMasterClassmate.cCount = pPacket->cCount;

	for( int itr = 0; itr < pPacket->cCount; ++itr )
	{
		m_pMasterClassmate.DataList[itr] = pPacket->DataList[itr];
		memcpy( m_pMasterClassmate.DataList[itr].wszCharName, pPacket->DataList[itr].wszCharName, sizeof(m_pMasterClassmate.DataList[itr].wszCharName) );
	}

	GetInterface().RefreshMasterClassmate();
}

void CDnMasterTask::OnRecvMyMasterInfo( SCMyMasterInfo * pPacket )
{
	m_MyMasterInfo = pPacket->MasterInfo;

	memcpy( m_MyMasterInfo.wszCharName, pPacket->MasterInfo.wszCharName, sizeof(m_MyMasterInfo.wszCharName) );

	GetInterface().RefreshMyMasterInfo();
}

void CDnMasterTask::OnRecvClassmateInfo( SCClassmateInfo *pPacket )
{
	m_ClassmateInfo = pPacket->ClassmateInfo;

	memcpy( m_ClassmateInfo.wszCharName, pPacket->ClassmateInfo.wszCharName, sizeof(m_ClassmateInfo.wszCharName) );

	for( int itr = 0; itr < m_ClassmateInfo.cMasterCount; ++itr )
		memcpy( m_ClassmateInfo.wszMasterCharName[itr], pPacket->ClassmateInfo.wszMasterCharName[itr], sizeof(m_ClassmateInfo.wszMasterCharName[itr]) );

	GetInterface().RefreshClassmateInfo();
}

void CDnMasterTask::OnRecvLeave( SCLeave * pPacket )
{
	if( pPacket->bIsDelPupil )	// 제자 추방
	{
		if( ERROR_MASTERSYSTEM_LEAVE_DESTUSER_OFFLINE == pPacket->iRet )	{
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7258 ), false );	//해당 제자님은 지금 추방할 수 없는 상태입니다.
			return;
		}
		else if( ERROR_MASTERSYSTEM_LEAVE_DESTUSER_CANTSTATE == pPacket->iRet )	{
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7259 ), false );	//제자님이 마을로 돌아오면 추방할 수 있습니다.
			return;
		}
		else if( ERROR_MASTERSYSTEM_LEAVE_FAILED == pPacket->iRet ) {
			RequestMasterCharacterInfo();
			return;
		}

		WCHAR wszString[256];
		swprintf_s(wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7247 ), pPacket->iPenaltyRespectPoint );

		if( 0 != pPacket->iPenaltyRespectPoint )
		{
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7246 ), false );	//제자가 삭제 되었습니다.

			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszString, false );													//존경수치 %d가 하락하였습니다.

			m_iPenalyRespectPoint = pPacket->iPenaltyRespectPoint;
		}
		else
		{
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7246 ), false );	//제자가 삭제 되었습니다.
			m_iPenalyRespectPoint = 0;
		}

		GetInterface().RefreshPupilLeave();
	}
	else	//스승삭제e
	{
		if( pPacket->iRet != ERROR_NONE )
		{
			if( ERROR_MASTERSYSTEM_LEAVE_DESTUSER_OFFLINE == pPacket->iRet )	{
				GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7258 ), false );	//해당제자님은 지금 추방 할수없는 상태입니다.
				return;
			}
			else if( ERROR_MASTERSYSTEM_LEAVE_DESTUSER_CANTSTATE == pPacket->iRet )	{
				GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7259 ), false );
				return;
			}
			else
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7274 ) );
				return;
			}
		}

		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7245 ), false );	//스승이 삭제되었습니다.
		RequestMasterAndClassmate();
	}
}

void CDnMasterTask::OnRecvInvitePupil( SCInvitePupil * pPacket )
{
	if( ERROR_NONE != pPacket->iRet )
	{
		if( ERROR_GENERIC_LEVELLIMIT == pPacket->iRet )
		{
			WCHAR wszMessage[512];
			int iWeight = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::LimitLevel_MasterSystem);
			swprintf_s( wszMessage, _countof(wszMessage), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7296 ), iWeight );	// 사제 기능은 %d 레벨 이후 사용 가능합니다.

			GetInterface().MessageBox( wszMessage );
		}

		if( ERROR_MASTERSYSTEM_JOIN_DENY == pPacket->iRet )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7271 ), false );	// 스승 신청을 거절하였습니다.
		else if( ERROR_MASTERSYSTEM_CANT_JOINSTATE == pPacket->iRet)
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7285 ), false );	// 현재 해당 유저는 스승 신청을 요청할 수 없는 상태입니다.
		DebugLog( "스승신청 실패" );

		return;
	}

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7269 ), false );	// 스승 신청을 하였습니다.
}

void CDnMasterTask::OnRecvInvitePupilConfirm( SCInvitePupilConfirm * pPacket )
{
	if( GetInterface().IsOpenBlind() )
	{
		SendReqInvitePupilConfirm( false, pPacket->wszMasterCharName );
		return;
	}

	//프로그레스바창
	m_bAcceptRequest = true;
	m_nAcceptType = PUPIL_CONFIRM;
	m_fAcceptTime = 10.0f;

	m_wszCharName = std::wstring( pPacket->wszMasterCharName );
}

void CDnMasterTask::OnRecvJoinDirect( SCJoinDirect * pPacket )
{
	if( ERROR_NONE != pPacket->iRet )
	{
		if( ERROR_GENERIC_LEVELLIMIT == pPacket->iRet )
		{
			WCHAR wszMessage[512];
			int iWeight = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::LimitLevel_MasterSystem);
			swprintf_s( wszMessage, _countof(wszMessage), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7296 ), iWeight );	// 사제 기능은 %d 레벨 이후 사용 가능합니다.

			GetInterface().MessageBox( wszMessage );
		}

		if( ERROR_MASTERSYSTEM_JOIN_DENY == pPacket->iRet )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7272 ), false );	// 제자 신청을 거절하였습니다.
		else if( ERROR_MASTERSYSTEM_CANT_JOINSTATE == pPacket->iRet)
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7286 ), false );	// 현재 해당 유저는 제자 신청을 요청할 수 없는 상태입니다.
		DebugLog( "제자신청 실패" );

		return;
	}

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7270 ), false );	// 제자 신청을 하였습니다.
}

void CDnMasterTask::OnRecvJoinDirectConfirm( SCJoinDirectConfirm * pPacket )
{
	if( GetInterface().IsOpenBlind() )
	{
		SendReqJoinDirectConfirm( false, pPacket->wszPupilCharName );
		return;
	}

	m_bAcceptRequest = true;
	m_nAcceptType = MASTER_CONFIRM;
	m_fAcceptTime = 10.0f;

	m_wszCharName = std::wstring( pPacket->wszPupilCharName );
}

void CDnMasterTask::OnRecvJoinConfirm( SCJoinComfirm * pPacket )
{
	if( GetInterface().IsOpenBlind() )
	{
		SendReqJoinConfirm( false, pPacket->wszPupilCharName );
		return;
	}

	m_bAcceptRequest = true;
	m_nAcceptType = JOIN_CONFIRM;
	m_fAcceptTime = 10.0f;

	m_wszCharName = std::wstring( pPacket->wszPupilCharName );
	m_cLevel = pPacket->cLevel;
	m_cJob = pPacket->cJob;
}

void CDnMasterTask::OnRecvGraduate( SCGraduate * pPacket )
{
	pPacket->wszCharName;

	WCHAR wszString[256];
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7253 ), pPacket->wszCharName, static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_MasterMinLevel)) );
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszString, false );

	if( m_SimpleInfo.iMasterCount > 0 )
		RequestMasterAndClassmate();
	else
		RequestMasterCharacterInfo();
}

void CDnMasterTask::OnRecvCountInfo( SCCountInfo * pPacket )
{
	m_PartyCountInfo = *pPacket;

	if( GetInterface().GetPlayerGauge() ) GetInterface().GetPlayerGauge()->UpdateKeepMasterInfo( pPacket->iMasterCount, pPacket->iPupilCount, pPacket->iClassmateCount );
}

void CDnMasterTask::OnRecvRespectPoint( SCRespectPoint * pPacket )
{
	WCHAR wszString[256];
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7252 ), pPacket->iRespectPoint );
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszString, false );

	m_iRespectPoint = pPacket->iRespectPoint;

	GetInterface().RefreshRespectPoint();
}

void CDnMasterTask::OnRecvFavorPoint( SCFavorPoint * pPacket )
{
	int nCount = GetPartyTask().GetPartyCount();
	WCHAR wszString[256];

	for( int itr = 0; itr < nCount; ++itr )
	{
		CDnPartyTask::PartyStruct * pParty = GetPartyTask().GetPartyData( itr );

		if( pPacket->biMasterCharacterDBID == IsMasterPlayer( pParty->hActor->GetName() ) )
		{
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7277 ), pParty->hActor->GetName() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );
		}
		else if( pPacket->biPupilCharacterDBID == IsMasterPlayer( pParty->hActor->GetName() ) )
		{
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7278 ), pParty->hActor->GetName() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );
		}
	}
}

void CDnMasterTask::OnRecvRecallMaster( SCRecallMaster * pPacket )
{
	if( ERROR_NONE != pPacket->iRet )
	{
		WCHAR wszString[256];
		if( ERROR_MASTERSYSTEM_RECALL_SAMECHANNEL == pPacket->iRet )
		{
			swprintf_s( wszString, L"%s\n%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7264 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7268 ) );
			GetInterface().MessageBox( wszString );
		}
		else if( ERROR_MASTERSYSTEM_RECALL_REJECT == pPacket->iRet )
		{
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7283 ), pPacket->wszCharName );
			GetInterface().MessageBox( wszString );
		}
		else
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7266 ) );

		return;
	}

#ifdef PRE_ADD_SECONDARY_SKILL
	GetInterface().CloseCookingDialog();
#endif // PRE_ADD_SECONDARY_SKILL

	WCHAR wszString[256];

	if( CDnLocalPlayerActor::IsLockInput() || GetInterface().IsOpenBlind()
		|| GetInterface().GetChatRoomDlg()->IsShow()
#ifdef PRE_ADD_SECONDARY_SKILL
		|| ( CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking() )
#endif // PRE_ADD_SECONDARY_SKILL
		)
	{
		SendReqBreakInto( ERROR_MASTERSYSTEM_RECALL_CANTSTATUS, pPacket->wszCharName );
		return;
	}

	if( pPacket->bIsConfirm )
	{
		m_bAcceptRequest = true;
		m_nAcceptType = RECALL_CONFIRM;
		m_fAcceptTime = 10.0f;

		m_wszCharName = std::wstring( pPacket->wszCharName );

		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7257 ), pPacket->wszCharName );
	}
	else
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7256 ), pPacket->wszCharName );

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );
}

void CDnMasterTask::OnRecvConnect( SCConnect * pPacket )
{
	WCHAR wszString[256];

	if( m_SimpleInfo.iMasterCount > 0 )
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pPacket->bIsConnect ? 7243 : 7275 ), pPacket->wszCharName );
	else if( m_SimpleInfo.iPupilCount > 0 )
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pPacket->bIsConnect ? 7244 : 7276 ), pPacket->wszCharName );
	else
		return;

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );
}

bool CDnMasterTask::Initialize()
{
	return true;
}

void CDnMasterTask::RequestMasterList( const UINT uiPage, const BYTE cJob, const BYTE cGender )
{
	SendReqMasterList(uiPage, cJob, cGender);
}

void CDnMasterTask::RequestMasterCharacterInfo()
{
	SendReqMasterCharacterInfo();
}

void CDnMasterTask::RequestMasterIntroduction( const bool bRegister, const WCHAR * wszSelfIntroduction )
{
	SendMasterIntroduction( bRegister, wszSelfIntroduction );
}

void CDnMasterTask::RequestMasterApplication( INT64 biCharacterDBID )
{
	SendMasterApplication( biCharacterDBID );
}

void CDnMasterTask::RequestMasterJoin( INT64 biMasterCharacterDBID, const WCHAR * wszMasterCharName )
{
	SendMasterJoin( biMasterCharacterDBID, wszMasterCharName );
}

void CDnMasterTask::RequestMasterAndClassmate()
{
	SendReqMasterAndClassmate();
}

void CDnMasterTask::RequestMyMasterInfo( INT64 biMasterCharacterDBID )
{
	SendReqMyMasterInfo(biMasterCharacterDBID);
}

void CDnMasterTask::RequestClassmateInfo( INT64 biClassmateCharacterDBID )
{
	SendReqClassmateInfo( biClassmateCharacterDBID );
}

void CDnMasterTask::RequestLeave( INT64 biDestCharacterDBID, bool bIsMaster )
{
	SendReqLeave( biDestCharacterDBID, bIsMaster );
}

void CDnMasterTask::RequestInvitePupil( const WCHAR * wszCharName )
{
	SendReqInvitePupil( wszCharName );
}

void CDnMasterTask::RequestInvitePupilConfirm( const bool bIsAccept, const WCHAR * wszMasterCharName )
{
	SendReqInvitePupilConfirm( bIsAccept, wszMasterCharName );
}

void CDnMasterTask::RequestJoinDirect( const WCHAR * wszMasterCharName )
{
	SendReqJoinDirect( wszMasterCharName );
}

void CDnMasterTask::RequestJoinDirectConfirm( const bool bIsAccept, const WCHAR * wszPupilCharName )
{
	SendReqJoinDirectConfirm( bIsAccept, wszPupilCharName );
}

void CDnMasterTask::RequestRecallMaster( const WCHAR * wszMasterCharName )
{
	SendReqRecallMaster( wszMasterCharName );
}

void CDnMasterTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bAcceptRequest )
	{
		if( m_fAcceptTime < 0.0f )
			m_bAcceptRequest = false;

		m_fAcceptTime -= fDelta;
	}
}

float CDnMasterTask::GetShortestAcceptTime()
{
	if( m_bAcceptRequest )
		return m_fAcceptTime;
	else
		return _INVALID_TIME_VALUE;
}

void CDnMasterTask::OpenAcceptRequestDialog()
{
	WCHAR wszString[256];
	if( PUPIL_CONFIRM == m_nAcceptType )
	{
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7239 ), m_wszCharName.c_str() );
		GetInterface().OpenAcceptRequestDialog(wszString, _REQUEST_ACCEPT_TOTALTIME, m_fAcceptTime, true, CDnAcceptRequestDlg::eType02, ACCEPT_REQUEST_DIALOG, this );
	}
	else if( MASTER_CONFIRM == m_nAcceptType )
	{
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7238 ), m_wszCharName.c_str() );
		GetInterface().OpenAcceptRequestDialog(wszString, _REQUEST_ACCEPT_TOTALTIME, m_fAcceptTime, true, CDnAcceptRequestDlg::eType02, ACCEPT_REQUEST_DIALOG, this );
	}
	else if( RECALL_CONFIRM == m_nAcceptType )
	{
		swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7263 ), m_wszCharName.c_str() );
		swprintf_s( wszString, _countof(wszString), L"%s\n%s", wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7262 ) );
		GetInterface().OpenAcceptRequestDialog(wszString, _REQUEST_ACCEPT_TOTALTIME, m_fAcceptTime, true, CDnAcceptRequestDlg::eType02, ACCEPT_REQUEST_DIALOG, this );
	}
	else if( JOIN_CONFIRM == m_nAcceptType )
	{
		std::wstring strMessage;
		strMessage = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7287), m_wszCharName.c_str(), DN_INTERFACE::STRING::GetClassString( m_cJob ), m_cLevel );
		GetInterface().OpenAcceptRequestDialog( strMessage.c_str(), _REQUEST_ACCEPT_TOTALTIME, m_fAcceptTime, true, CDnAcceptRequestDlg::eType02, ACCEPT_REQUEST_DIALOG, this );
	}
}

void CDnMasterTask::Finalize()
{
}

void CDnMasterTask::RejectRecall()
{
	if (m_nAcceptType == RECALL_CONFIRM)
	{
		SendReqBreakInto( ERROR_MASTERSYSTEM_RECALL_REJECT, m_wszCharName.c_str() );

		if (GetInterface().IsOpenAcceptDialog())
			GetInterface().CloseAcceptDialog();
		m_bAcceptRequest = false;
		m_fAcceptTime = _INVALID_TIME_VALUE;

		GetInterface().CloseAcceptRequestDialog();
	}
}

void CDnMasterTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 
	{
	case ACCEPT_REQUEST_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if (GetInterface().IsOpenAcceptDialog())
					GetInterface().CloseAcceptDialog();
				m_bAcceptRequest = false;
				m_fAcceptTime = _INVALID_TIME_VALUE;

				GetInterface().CloseAcceptRequestDialog();

				bool bIsAccept;
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
					bIsAccept = true;
				else
					bIsAccept = false;
				
				if( PUPIL_CONFIRM == m_nAcceptType )
				{
					SendReqInvitePupilConfirm( bIsAccept, m_wszCharName.c_str() );
				}
				else if( MASTER_CONFIRM == m_nAcceptType )
				{
					SendReqJoinDirectConfirm( bIsAccept, m_wszCharName.c_str() );
				}
				else if( RECALL_CONFIRM == m_nAcceptType )
				{
					m_nAcceptType = 0;
					if( !bIsAccept )
					{
						WCHAR wszString[256];
						swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7284 ), m_wszCharName.c_str() );
						GetInterface().MessageBox( wszString );
					}

					CDnLocalPlayerActor * localActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
					if(localActor&&localActor->IsVehicleMode() && bIsAccept)
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9213 ), textcolor::YELLOW, 4.0f );
						bIsAccept = false;
					}

					if (bIsAccept)
						CDnInterface::GetInstance().DisableCashShopMenuDlg(true);

					SendReqBreakInto( bIsAccept?ERROR_NONE:ERROR_MASTERSYSTEM_RECALL_REJECT, m_wszCharName.c_str() );
				}
				else if( JOIN_CONFIRM == m_nAcceptType )
				{
					SendReqJoinConfirm( bIsAccept, m_wszCharName.c_str() );
				}
			}
		}
	}
}

INT64 CDnMasterTask::IsMasterPlayer( const WCHAR * wszName )
{
	int nCount = m_SimpleInfo.iMasterCount != 0 ? m_SimpleInfo.iMasterCount : m_SimpleInfo.iPupilCount;

	for( int itr = 0 ; itr < nCount; ++itr )
	{
		if( __wcsicmp_l(wszName, m_SimpleInfo.OppositeInfo[itr].wszCharName) == 0 )
			return m_SimpleInfo.OppositeInfo[itr].CharacterDBID;
	}

	return 0;
}

bool CDnMasterTask::IsPlayWithPupil( DnActorHandle hActor )
{
	if( m_PartyCountInfo.iPupilCount > 0 )
	{
		for( int itr = 0; itr < m_SimpleInfo.iPupilCount; ++itr )
		{
			if( __wcsicmp_l( hActor->GetName(), m_SimpleInfo.OppositeInfo[itr].wszCharName ) == 0 )
				return false;
		}

		CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
		if( __wcsicmp_l( hActor->GetName(), localActor->GetName() ) == 0 )
			return true;
	}
	else if( m_PartyCountInfo.iMasterCount > 0 )
	{
		for( int itr = 0; itr < m_SimpleInfo.iMasterCount; ++itr )
		{
			if( __wcsicmp_l( hActor->GetName(), m_SimpleInfo.OppositeInfo[itr].wszCharName ) == 0 && hActor->GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_PupilMaxLevel)) )
				return true;
		}
	}

	return false;
}

void CDnMasterTask::CloseAcceptDlg()
{
	if( m_bAcceptRequest )
	{
		m_fAcceptTime = -1.f;

		if( PUPIL_CONFIRM == m_nAcceptType )
			SendReqInvitePupilConfirm( false, m_wszCharName.c_str() );
		else if( MASTER_CONFIRM == m_nAcceptType )
			SendReqJoinDirectConfirm( false, m_wszCharName.c_str() );
		else if( JOIN_CONFIRM == m_nAcceptType )
			SendReqJoinConfirm( false, m_wszCharName.c_str() );
	}
}
