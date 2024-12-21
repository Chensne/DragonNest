
#include "stdafx.h"
#include "DnGameOptionTask.h"
#include "DnInterface.h"
#include "GameOptionSendPacket.h"
#include "DnQuestTask.h"
#include "InputWrapper.h"
#include "DnGameProfileOptDlg.h"
#include "DnInterfaceString.h"
#include "DnGameControlTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGameOptionTask::CDnGameOptionTask() : CTaskListener(true)
{
	m_pProfileDialog = NULL;
}

CDnGameOptionTask::~CDnGameOptionTask()
{
}

void CDnGameOptionTask::OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
	case SC_GAMEOPTION: OnRecvGameOptionMessage(nSubCmd, pData, nSize); break;
	}
}

void CDnGameOptionTask::ReqSendSetGameOption( TGameOptions &pOption )
{
	SendSetGameOption( pOption );
}

void CDnGameOptionTask::ReqSendUserComm( UINT nSessionID )
{
	SendCharCommOption( nSessionID );
}

void CDnGameOptionTask::ReqSendKeySetting()
{
	bool bDefault = true;
	TKeySetting KeySetting;
	memset( &KeySetting, 0, sizeof(KeySetting) );

	// DB에 할당된 공간이 늘어나야 패킷도 같이 늘리는데,
	// 만약 키설정작업만 되어있고, 공간이 모자르다면 아직 적용할 수 없는 상태다.
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	if( sizeof(KeySetting.arrKey2) < sizeof(g_UIWrappingKeyData) )
		return;
#endif

	bDefault = GetGameControlTask().IsDefaultValue( CDnGameControlTask::eControlType_Keyboard );

	if( !bDefault ) {
		KeySetting.cVersion = _VERSION_KEYSETTING;
		memcpy_s( &KeySetting.arrKey1, sizeof(g_WrappingKeyData), &g_WrappingKeyData, sizeof(g_WrappingKeyData) );
		memcpy_s( &KeySetting.arrKey2, sizeof(g_UIWrappingKeyData), &g_UIWrappingKeyData, sizeof(g_UIWrappingKeyData) );
	}
	SendKeySetting( bDefault, &KeySetting );
}

void CDnGameOptionTask::ReqSendJoypadSetting()
{
	bool bDefault = true;
	TPadSetting PadSetting;
	memset( &PadSetting, 0, sizeof(PadSetting) );

	bDefault = GetGameControlTask().IsDefaultValue( CDnGameControlTask::eControlType_CustomPad );

	if( !bDefault ) {
		PadSetting.cVersion = _VERSION_PADSETTING;
		memcpy_s( &PadSetting.arrKey1, sizeof(g_WrappingCustomData), &g_WrappingCustomData, sizeof(g_WrappingCustomData) );
		memcpy_s( &PadSetting.arrKey2, sizeof(g_WrappingCustomAssistData), &g_WrappingCustomAssistData, sizeof(g_WrappingCustomAssistData) );
		memcpy_s( &PadSetting.arrKey3, sizeof(g_UIWrappingCustomData), &g_UIWrappingCustomData, sizeof(g_UIWrappingCustomData) );
		memcpy_s( &PadSetting.arrKey4, sizeof(g_UIWrappingCustomAssistData), &g_UIWrappingCustomAssistData, sizeof(g_UIWrappingCustomAssistData) );
	}
	SendPadSetting( bDefault, &PadSetting );
}

void CDnGameOptionTask::OnRecvGameOptionMessage(int nSubCmd, char * pData, int nSize)
{
	switch (nSubCmd)
	{
	case eGameOption::SC_GAMEOPTION_USERCOMMOPTION:	OnRecvSystemCharCommOption((SCGameOptionCommOption*)pData);	break;
	case eGameOption::SC_GAMEOPTION_REFRESH_NOTIFIER:	OnRecvRefreshNotifier((SCGameOptionRefreshNotifier*)pData);	break;
	case eGameOption::SC_GAMEOPTION_SELECT_KEYSETTING:	OnRecvSelectKeySetting((SCGameOptionSelectKeySetting*)pData); break;
	case eGameOption::SC_GAMEOPTION_SELECT_PADSETTING:	OnRecvSelectPadSetting((SCGameOptionSelectPadSetting*)pData); break;
	case eGameOption::SC_GAMEOPTION_GET_PROFILE:		OnRecvGetProfile((SCGameOptionGetProfile*)pData); break;
	case eGameOption::SC_GAMEOPTION_DISPLAY_PROFILE:	OnRecvDisplayProfile((SCGameOptionDisplayProfile*)pData); break;
	}
}

void CDnGameOptionTask::OnRecvSystemCharCommOption(SCGameOptionCommOption * pPacket)
{
	if (pPacket)
		GetInterface().SetDirectDialogPermission(pPacket->nSessionID, pPacket->cCommunityOption, pPacket->PartyID);
	GetInterface().HandleMsgListenDlg(eGameOption::SC_GAMEOPTION_USERCOMMOPTION, (char*)pPacket);
}

void CDnGameOptionTask::OnRecvRefreshNotifier( SCGameOptionRefreshNotifier *pPacket )
{
	if( CDnQuestTask::IsActive() ) {
		GetQuestTask().RefreshNotifier( pPacket->data );
	}
}

void CDnGameOptionTask::OnRecvSelectKeySetting( SCGameOptionSelectKeySetting *pPacket )
{
	if( pPacket->bIsDefault )
	{
		GetGameControlTask().DefaultValue( CDnGameControlTask::eControlType_Keyboard );
		GetGameControlTask().ChangeValue( CDnGameControlTask::eControlType_Keyboard );
		return;
	}

	if( pPacket->sKeySetting.cVersion != _VERSION_KEYSETTING )
	{
		// 대략적인 유효 범위 안에 들어왔다면, 버전이 업데이트 되었음을 알린다.
		if( 0 < (_VERSION_KEYSETTING - pPacket->sKeySetting.cVersion) && (_VERSION_KEYSETTING - pPacket->sKeySetting.cVersion) < 5 )
		{
			if( CDnInterface::IsActive() )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3189 ), MB_OK, -1, NULL, false );

			// 최신버전의 디폴트값으로 새로 보내 다음번 접속때 더이상 이 메세지가 안보이게 한다.
			SendKeySetting( true, NULL );
		}
		return;
	}

	memcpy_s( &g_WrappingKeyData, sizeof(g_WrappingKeyData), pPacket->sKeySetting.arrKey1, sizeof(g_WrappingKeyData) );
	memcpy_s( &g_UIWrappingKeyData, sizeof(g_UIWrappingKeyData), pPacket->sKeySetting.arrKey2, sizeof(g_UIWrappingKeyData) );

	if( CDnInterface::IsActive() )
		GetInterface().ApplyUIHotKey();
}

void CDnGameOptionTask::OnRecvSelectPadSetting(SCGameOptionSelectPadSetting *pPacket)
{
	if( pPacket->bIsDefault )
	{
		GetGameControlTask().DefaultValue( CDnGameControlTask::eControlType_CustomPad );
		GetGameControlTask().ChangeValue( CDnGameControlTask::eControlType_CustomPad );
		return;
	}

	if( pPacket->sPadSetting.cVersion != _VERSION_PADSETTING )
	{
		// 대략적인 유효 범위 안에 들어왔다면, 버전이 업데이트 되었음을 알린다.
		if( 0 < (_VERSION_PADSETTING - pPacket->sPadSetting.cVersion) && (_VERSION_PADSETTING - pPacket->sPadSetting.cVersion) < 5 )
		{
			if( CDnInterface::IsActive() )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3189 ), MB_OK, -1, NULL, false );

			// 최신버전의 디폴트값으로 새로 보내 다음번 접속때 더이상 이 메세지가 안보이게 한다.
			SendPadSetting( true, NULL );
		}
		return;
	}

	memcpy_s( &g_WrappingCustomData, sizeof(g_WrappingCustomData), pPacket->sPadSetting.arrKey1, sizeof(g_WrappingCustomData) );
	memcpy_s( &g_WrappingCustomAssistData, sizeof(g_WrappingCustomAssistData), pPacket->sPadSetting.arrKey2, sizeof(g_WrappingCustomAssistData) );
	memcpy_s( &g_UIWrappingCustomData, sizeof(g_UIWrappingCustomData), pPacket->sPadSetting.arrKey3, sizeof(g_UIWrappingCustomData) );
	memcpy_s( &g_UIWrappingCustomAssistData, sizeof(g_UIWrappingCustomAssistData), pPacket->sPadSetting.arrKey4, sizeof(g_UIWrappingCustomAssistData) );
}

void CDnGameOptionTask::OnRecvGetProfile( SCGameOptionGetProfile *pPacket )
{
	if( !m_pProfileDialog ) return;
	m_pProfileDialog->SetProfile( &pPacket->sProfile );
}

void CDnGameOptionTask::OnRecvDisplayProfile( SCGameOptionDisplayProfile *pPacket )
{
	// 멀리 있는 파티원이 가입할 경우도 있으니 검색하지 않는다.
	//DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	//if( !hActor ) return;

	// OpenPublic은 서버에서 이미 검사했다.
	std::wstring wszProfile;
	DN_INTERFACE::STRING::GetProfileText( wszProfile, pPacket->sProfile );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszProfile.c_str(), false );

	// 현재 채팅방의 경우엔, 주변 캐릭터가 채팅방 들어왔을때 로딩이 끝나고 나야 "채팅방에 입장했습니다."가 뜨는 구조다.
	// 그런데 위 패킷은 로딩되기 전에 먼저 올 수 있기때문에,
	// 채팅방이 열려있을땐 채팅쪽 태스크가 캐싱하는 구조로 가는게 정석인데.. 우선은 우선순위 낮으니 미루도록 한다.
}
