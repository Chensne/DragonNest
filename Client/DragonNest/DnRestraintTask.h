
#pragma once

#include "Task.h"
#include "MessageListener.h"

#ifdef PRE_ADD_ANTI_CHAT_SPAM
// 미국 채팅 스팸 체크용.
class CDnChatSpamChecker
{
private:
	struct S_CHAT_INFO
	{
		wstring strSentence;
		set<wstring> setWords;		// 영단어만 갖고 있음.
		DWORD dwTime;
		int iSpamCount;

		S_CHAT_INFO( void ) : dwTime( 0 ), iSpamCount( 0 )
		{

		}

		bool operator < ( const S_CHAT_INFO& rhs )
		{
			return dwTime > rhs.dwTime;
		}
	};

	// 60초동안의 채팅 이력을 갖고 있는다.
	list<S_CHAT_INFO> m_listChatHistory;

	const DWORD SPAM_CHECK_TIME;
	const float SAME_WORDS_TOLERANCE;
	const int SPAM_JUDGEMENT_COUNT;

public:
	CDnChatSpamChecker( void ) : SPAM_CHECK_TIME( 60000 ), SAME_WORDS_TOLERANCE( 0.7f ), SPAM_JUDGEMENT_COUNT( 9 ) {};
	bool Check( const wstring& strChat );
	bool IsPunct( wchar_t Char );
	void TokenizeExceptPunct( wstring& strSource, set<wstring>& setResult );
};
#endif // #ifdef PRE_ADD_ANTI_CHAT_SPAM

class CDnRestraintTask : public CTask, public CTaskListener, public CSingleton<CDnRestraintTask>
{
public:
	CDnRestraintTask();
	virtual ~CDnRestraintTask();

	void OnDisconnectTcp(bool bValidDisconnect) {}
	void OnDisconnectUdp(bool bValidDisconnect) {}
	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

	bool Initialize() { return true; }

	bool IsRestrainting( eRestraintType Type, TRestraint &Data );
	bool CheckRestraint( eRestraintType Type, bool bMessageBox = false, bool bChatMsg = false );

#ifdef PRE_ADD_ANTI_CHAT_SPAM
	bool CheckUSAChatSpam( const wstring& strChat );
#endif // #ifdef PRE_ADD_ANTI_CHAT_SPAM

protected:
	void OnRecvRestraintMessage(int nSubCmd, char * pData, int nSize);

public:
	void OnRecvRestraintAdd(SCRestraintAdd * pPacket);
	void OnRecvRestraintDel(SCRestraintDel * pPacket);
	void OnRecvRestraintRetCode(SCRestraintRet * pPacket);
	void OnRecvRestraintList(SCRestraintList * pPakcet);

private:
	std::vector <TRestraint> m_RestraintList;

#ifdef PRE_ADD_ANTI_CHAT_SPAM
	CDnChatSpamChecker m_ChatSpamChecker;
#endif // #ifdef PRE_ADD_ANTI_CHAT_SPAM

	bool IsExistsRestraint(UINT nKey);	// 클라는 이거 안쓴다.
};

#define GetRestraintTask()	CDnRestraintTask::GetInstance()