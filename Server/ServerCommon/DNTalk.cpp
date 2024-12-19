#include "stdafx.h"
#include "DnTalk.h"

#include "EtResourceMng.h"
#include <algorithm>
#include "DNUserSession.h"
#include "DnScriptManager.h"

#if defined(_VILLAGESERVER)
#include "DNUserSessionManager.h"
#include "DNScriptAPI.h"

#elif defined(_GAMESERVER)
#include "DNGameServerManager.h"
#include "DNRUDPGameServer.h"
#include "DNGameServerScriptAPI.h"
#endif

DnScriptManager* CDNTalk::m_pScriptManager = NULL;

CDNTalk::CDNTalk()
{

}

CDNTalk::~CDNTalk()
{

}

bool CDNTalk::Create(const WCHAR* wszTalkFileName, const WCHAR* wszScriptFileName)
{
	static std::wstring __wszExt(L".lua");

	m_wszTalkFileName = wszTalkFileName;
	m_wszScriptFilename = wszScriptFileName;

	RemoveStringW(m_wszScriptFilename, __wszExt );
	return true;
}

void CDNTalk::SetScriptManager(DnScriptManager* pScriptManager)
{
	m_pScriptManager = pScriptManager;
}

void CDNTalk::Destroy()
{

}

bool CDNTalk::CheckTalk(UINT nUserObjectID, IN std::wstring& wszIndex, IN std::wstring& wszTarget)
{
	//@ 여기서 내가 보낼수 있는 인덱스와 타겟인지 확인 한번 하고 

	std::wstring wszCheckIndex;
	wszCheckIndex = wszIndex;
	ToLowerW(wszCheckIndex);

	// 대화 시작이 아닐 경우에만 체크 한다.
	if ( wszCheckIndex != L"start" )
	{
#if defined(_VILLAGESERVER)
		CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByObjectID(nUserObjectID);
#elif defined(_GAMESERVER)
		CDNUserSession * pUser = NULL;
		CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByUID(nUserObjectID);
		if (pServer == NULL)	return false;

		pUser = pServer->GetSession(nUserObjectID);
		if (pUser == NULL) return false;
#endif

		TALK_PARAGRAPH& para = pUser->GetLastTalkParagraph();

		bool bCheckSuccess = false;

		// 비어있을 경우엔 체크를 스킵한다. 
		if ( pUser->GetSkipParagraphCheck() )
		{
			bCheckSuccess = true;
			bool& bSkip = pUser->GetSkipParagraphCheck();
			bSkip = false;
		}

		for ( int  i = 0 ; i < (int)para.Answers.size() ; i++ )
		{
			if ( para.Answers[i].szLinkIndex == wszIndex && para.Answers[i].szLinkTarget == wszTarget )
			{
				bCheckSuccess = true;
				break;
			}
		}

		if ( bCheckSuccess == false )
		{
#ifndef _FINAL_BUILD
			// 뭔가 잘못된 유저다.. 짜르자.

			std::wstring wszString;
			wszString = L"[TalkCheck Error:";
			wszString += wszIndex;
			wszString += L"  , ";
			wszString += wszTarget;

			pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());

#endif // _FINAL_BUILD
			return false;
		}
	}
	return true;
}

bool CDNTalk::OnTalk(UINT nUserObjectID, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget)
{
	if ( m_pScriptManager == NULL )
		return false;

	lua_State* pLua = m_pScriptManager->OpenState();
	if ( !pLua )
		return false;

	char szIndex[512] = {0,};
	ZeroMemory(szIndex, sizeof(char)*512);
	WideCharToMultiByte( CP_ACP, 0, wszIndex.c_str(), -1, szIndex, 512, NULL, NULL );

	char szTarget[512] = {0,};
	ZeroMemory(szTarget, sizeof(char)*512);
	WideCharToMultiByte( CP_ACP, 0, wszTarget.c_str(), -1, szTarget, 512, NULL, NULL );
	
	std::wstring wszFuncName;
	wszFuncName = m_wszScriptFilename;
	wszFuncName += L"_OnTalk";

	char szFunctionName[512] = {0,};
	ZeroMemory(szFunctionName, sizeof(char)*512);
	WideCharToMultiByte( CP_ACP, 0, wszFuncName.c_str(), -1, szFunctionName, 512, NULL, NULL );

#if defined(_VILLAGESERVER)
	CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByObjectID(nUserObjectID);
	if (!pUser) return false;
#elif defined(_GAMESERVER)
	CDNUserSession * pUser = NULL;
	CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByUID(nUserObjectID);
	if (pServer == NULL)	return false;

	pUser = pServer->GetSession(nUserObjectID);
	if (!pUser) return false;
#endif	// _VILLAGESERVER

	//g_Log.Log(LogType::_QUESTHACK, pUser, L"CDNTalk::OnTalk func(%S) user(%d) npc(%d) index(%S) , target(%S)\n", szFunctionName, nUserObjectID, nNpcUniqueID, szIndex, szTarget);

#if defined(_VILLAGESERVER)
	lua_tinker::call<void>(pLua, szFunctionName, nUserObjectID, nNpcUniqueID, szIndex, szTarget );
#elif defined(_GAMESERVER)
	//lua_tinker::call<void>(pLua, szFunctionName, pUser->GetGameRoom(), nUserObjectID, nNpcUniqueID, szIndex, szTarget );
	m_pScriptManager->call<void>(pLua, szFunctionName, pUser->GetGameRoom(), nUserObjectID, nNpcUniqueID, szIndex, szTarget );
#endif	// _VILLAGESERVER

#ifndef _FINAL_BUILD
	std::string szMsg;
	szMsg = FormatA("OnTalk func(%s) user(%d) npc(%d) index(%s) , target(%s)\n", szFunctionName, nUserObjectID, nNpcUniqueID, szIndex, szTarget );	
#if defined(_VILLAGESERVER)
	api_log_UserLog(nUserObjectID, szMsg.c_str() );
#elif defined(_GAMESERVER)
	api_log_UserLog(pUser->GetGameRoom(), nUserObjectID, szMsg.c_str() );
#endif	// _VILLAGESERVER
#endif // _FINAL_BUILD

	// npc 응답을 못해줬다면
	if ( pUser->GetCalledNpcResponse() == false )
	{
		pUser->SendNextTalkError();

		std::string szLocalMsg;
		szLocalMsg = FormatA("GetCalledNpcResponse is FALSE func(%s) user(%d) npc(%d) index(%s) , target(%s)\n", szFunctionName, nUserObjectID, nNpcUniqueID, szIndex, szTarget );
#if defined(_VILLAGESERVER)
		api_log_UserLog(nUserObjectID, szLocalMsg.c_str() );
#elif defined(_GAMESERVER)
		api_log_UserLog(pUser->GetGameRoom(), nUserObjectID, szLocalMsg.c_str() );
#endif	// _VILLAGESERVER
	}

	return true;
}
