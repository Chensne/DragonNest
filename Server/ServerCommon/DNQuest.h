#pragma once

#include "DnTalk.h"

class CDNQuest : public CDNTalk
{
public:
	struct CREATE_INFO 
	{
		int					nQuestIndex;
		BYTE				cQuestType;
		std::wstring		szQuestName;
		std::wstring		szTalkFileName;
		std::wstring		szScriptFileName;
		BYTE				cRepeatable;
		BYTE				btLevelCapReward;
		bool				bCheckRemoteComplete;
	};

	CDNQuest();
	virtual ~CDNQuest();

	bool Create(CREATE_INFO& info);
	void Destroy();

#if defined(_VILLAGESERVER)
	virtual bool OnTalk(UINT nUserObjectID, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget);
	virtual void OnCompleteCounting(UINT nUserObjectID, TCount& Count);
	virtual void OnCounting(UINT nUserObjectID, TCount& Count);
	virtual void OnAllCompleteCounting(UINT nUserObjectID);
	virtual void OnItemCollecting(UINT nUserObjectID, int nItemID, short wCount);
#if defined(PRE_ADD_REMOTE_QUEST)
	virtual void OnRemoteStart(UINT nUserObjectID);
	virtual void OnRemoteComplete(UINT nUserObjectID);
	virtual bool CanRemoteCompleteStep(UINT nUserObjectID, UINT nQuestStep);
#endif // #if defined(PRE_ADD_REMOTE_QUEST)
#elif defined(_GAMESERVER)
	virtual bool OnTalk(class CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget);
	virtual void OnCompleteCounting(class CDNGameRoom *pRoom, UINT nUserObjectID, TCount& Count);
	virtual void OnCounting(class CDNGameRoom *pRoom, UINT nUserObjectID, TCount& Count);
	virtual void OnAllCompleteCounting(class CDNGameRoom *pRoom, UINT nUserObjectID);
	virtual void OnItemCollecting(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount);
#if defined(PRE_ADD_REMOTE_QUEST)
	virtual void OnRemoteStart(CDNGameRoom *pRoom, UINT nUserObjectID);
	virtual void OnRemoteComplete(CDNGameRoom *pRoom, UINT nUserObjectID);
	virtual bool CanRemoteCompleteStep(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nQuestStep);
#endif // #if defined(PRE_ADD_REMOTE_QUEST)
#endif	// _VILLAGESERVER

	CREATE_INFO& GetQuestInfo() { return m_CreateInfo; }
	bool IsLevelCapReward() const { return (m_CreateInfo.btLevelCapReward == 1) ? true : false; }

private:
	
	CREATE_INFO	m_CreateInfo;

	std::string m_strFuncNameOnTalk;
	std::string m_strFuncNameOnCompleteCounting;
	std::string m_strFuncNameOnCounting;
	std::string m_strFuncNameOnCompleteAllCounting;
	std::string m_strFuncNameOnItemCollecting;
#if defined(PRE_ADD_REMOTE_QUEST)
	std::string m_strFuncNameOnRemoteStart;
	std::string m_strFuncNameOnRemoteComplete;
	std::string m_strFuncNameCanRemoteCompleteStep;
#endif // #if defined(PRE_ADD_REMOTE_QUEST)
	std::string _MakeFuncName( const WCHAR* pwszName );
};

