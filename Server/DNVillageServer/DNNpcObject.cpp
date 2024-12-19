#include "StdAfx.h"
#include "DNNpcObject.h"
#include "DNQuest.h"
#include "DNQuestManager.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "VarArg.h"

CDNNpcObject::CDNNpcObject(void)
{
	m_NpcType = ActorNpc;
}

CDNNpcObject::~CDNNpcObject(void)
{
}

bool CDNNpcObject::EnterWorld()
{
	return CDNBaseObject::EnterWorld();
}

bool CDNNpcObject::LeaveWorld()
{
	return CDNBaseObject::LeaveWorld();
}

bool CDNNpcObject::InitObject(WCHAR *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos)
{
	return CDNBaseObject::InitObject(pName, nUID, nChannelID, nMapIndex, pCurPos);
}

bool CDNNpcObject::FinalObject()
{
	return CDNBaseObject::FinalObject();
}

int CDNNpcObject::FieldProcess(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData)
{
	return CDNBaseObject::FieldProcess(pSender, wMsg, pSenderData, pParamData);
}

void CDNNpcObject::DoUpdate(DWORD CurTick)
{

}

bool CDNNpcObject::CreateNpc(TNpcData *pNpcData, char *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos, float fRotate)
{
	memset(&m_NpcData, 0, sizeof(TNpcData));
	memcpy(&m_NpcData, pNpcData, sizeof(TNpcData));

	InitObject(NULL, nUID, nChannelID, nMapIndex, pCurPos);

	MultiByteToWideChar(CP_ACP, 0, pName, -1, m_BaseData.wszName, NAMELENMAX);

	m_BaseData.fRotate = fRotate;

	CDnNpc::Create(pNpcData);

	EnterWorld();

	return true;
}

bool CDNNpcObject::Talk(CDNUserBase* pUserBase, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget)
{
	CDNUserSession* pUserSession = static_cast<CDNUserSession*>(pUserBase);

	std::transform(wszTarget.begin(), wszTarget.end(), wszTarget.begin(), towlower); 
	TALK_MAP_IT iter = m_TalkMap.find(wszTarget);

	// npc 토크가 아니라면 
	if ( iter == m_TalkMap.end() )
	{
		// 퀘스트에서 한번 뒤져 본다.
		CDNQuest* pQuest = g_pQuestManager->GetQuest(wszTarget);

		if ( !pQuest )
		{
			g_Log.Log(LogType::_ERROR, pUserSession, L"[CS_NPCTALK] (5-2) UID:%d, PAK:%d/%s/%s\n", pUserSession->GetObjectID(), nNpcUniqueID, wszIndex.c_str(), wszIndex.c_str());
			return false;
		}

		pUserSession->SetCalledNpcResponse(false, false);

		// 대사 파일 대상이 퀘스트라면 퀘스트 ontalk 실행
		return pQuest->OnTalk(pUserSession->GetObjectID(), m_BaseData.nObjectID,  wszIndex, wszTarget);
	}

	pUserSession->SetCalledNpcResponse(false, false);

	TALK_MAP_IT aIt = m_TalkMap.find(wszTarget);
	if (m_TalkMap.end() == aIt) {
		USES_CONVERSION;
		DN_ASSERT(0,	CVarArgA<128>("Invalid! -> m_TalkMap.end() == aIt : NpcUniqueID(%d), wszIndex(%s), wszTarget(%s)", nNpcUniqueID, CW2CT(wszIndex.c_str()), CW2CT(wszTarget.c_str())));
		return false;
	}
	CDNTalk* pTalk = aIt->second;
	DN_ASSERT(NULL != pTalk,	CVarArgA<128>("Invalid! -> NULL == pTalk"));
	
	return pTalk->OnTalk(pUserSession->GetObjectID(), m_BaseData.nObjectID, wszIndex, wszTarget);
}
