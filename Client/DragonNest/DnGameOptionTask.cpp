
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

	// DB�� �Ҵ�� ������ �þ�� ��Ŷ�� ���� �ø��µ�,
	// ���� Ű�����۾��� �Ǿ��ְ�, ������ ���ڸ��ٸ� ���� ������ �� ���� ���´�.
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
		// �뷫���� ��ȿ ���� �ȿ� ���Դٸ�, ������ ������Ʈ �Ǿ����� �˸���.
		if( 0 < (_VERSION_KEYSETTING - pPacket->sKeySetting.cVersion) && (_VERSION_KEYSETTING - pPacket->sKeySetting.cVersion) < 5 )
		{
			if( CDnInterface::IsActive() )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3189 ), MB_OK, -1, NULL, false );

			// �ֽŹ����� ����Ʈ������ ���� ���� ������ ���Ӷ� ���̻� �� �޼����� �Ⱥ��̰� �Ѵ�.
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
		// �뷫���� ��ȿ ���� �ȿ� ���Դٸ�, ������ ������Ʈ �Ǿ����� �˸���.
		if( 0 < (_VERSION_PADSETTING - pPacket->sPadSetting.cVersion) && (_VERSION_PADSETTING - pPacket->sPadSetting.cVersion) < 5 )
		{
			if( CDnInterface::IsActive() )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3189 ), MB_OK, -1, NULL, false );

			// �ֽŹ����� ����Ʈ������ ���� ���� ������ ���Ӷ� ���̻� �� �޼����� �Ⱥ��̰� �Ѵ�.
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
	// �ָ� �ִ� ��Ƽ���� ������ ��쵵 ������ �˻����� �ʴ´�.
	//DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	//if( !hActor ) return;

	// OpenPublic�� �������� �̹� �˻��ߴ�.
	std::wstring wszProfile;
	DN_INTERFACE::STRING::GetProfileText( wszProfile, pPacket->sProfile );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszProfile.c_str(), false );

	// ���� ä�ù��� ��쿣, �ֺ� ĳ���Ͱ� ä�ù� �������� �ε��� ������ ���� "ä�ù濡 �����߽��ϴ�."�� �ߴ� ������.
	// �׷��� �� ��Ŷ�� �ε��Ǳ� ���� ���� �� �� �ֱ⶧����,
	// ä�ù��� ���������� ä���� �½�ũ�� ĳ���ϴ� ������ ���°� �����ε�.. �켱�� �켱���� ������ �̷絵�� �Ѵ�.
}
