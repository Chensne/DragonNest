#include "stdafx.h"
#include "DnRestraintTask.h"
#include "DnInterface.h"
#include "DnMessageBox.h"
#include "SyncTimer.h"
#include "DnInterfaceString.h"
#include <mmsystem.h>
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnRestraintTask::CDnRestraintTask() : CTaskListener(true)
{
}

CDnRestraintTask::~CDnRestraintTask()
{
}

void CDnRestraintTask::OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
	case SC_RESTRAINT: OnRecvRestraintMessage(nSubCmd, pData, nSize); break;
	}
}

void CDnRestraintTask::OnRecvRestraintMessage(int nSubCmd, char * pData, int nSize)
{

	switch (nSubCmd)
	{
	case eRestraint::SC_RESTRAINTADD: OnRecvRestraintAdd((SCRestraintAdd*)pData); break;
	case eRestraint::SC_RESTRAINTDEL: OnRecvRestraintDel((SCRestraintDel*)pData); break;
	case eRestraint::SC_RESTRAINTRETCODE: OnRecvRestraintRetCode((SCRestraintRet*)pData); break;
	case eRestraint::SC_RESTRAINTLIST: OnRecvRestraintList((SCRestraintList*)pData); break;
	}
}

void CDnRestraintTask::OnRecvRestraintAdd(SCRestraintAdd * pPacket)
{
	if (IsExistsRestraint(pPacket->Restraint.nRestraintKey))
	{
		return;
	}
	m_RestraintList.push_back(pPacket->Restraint);

	// Add될때도 시스템 메세지 사용한다.
	CheckRestraint( (eRestraintType)pPacket->Restraint.nRestraintType, false, true );
}

void CDnRestraintTask::OnRecvRestraintDel(SCRestraintDel * pPacket)
{
	std::vector <TRestraint>::iterator ii; 
	for (ii = m_RestraintList.begin(); ii != m_RestraintList.end(); ii++)
	{
		if ((*ii).nRestraintKey == pPacket->nRestraintKey)
		{
			m_RestraintList.erase(ii);
			return;
		}
	}
}

void CDnRestraintTask::OnRecvRestraintRetCode(SCRestraintRet * pPacket)
{
	GetInterface().ServerMessageBox(pPacket->nRestraintRet);
}

void CDnRestraintTask::OnRecvRestraintList(SCRestraintList * pPacket)
{
	//std::vector <TRestraint> m_RestraintList;
	m_RestraintList.clear();

	for (int i = 0; i < pPacket->cCount; i++)
	{
		m_RestraintList.push_back(pPacket->restraint[i]);
		CheckRestraint( (eRestraintType)pPacket->restraint[i].nRestraintType, false, true );
	}
}

bool CDnRestraintTask::IsExistsRestraint(UINT nKey)
{
	std::vector <TRestraint>::iterator ii; 
	for (ii = m_RestraintList.begin(); ii != m_RestraintList.end(); ii++)
		if ((*ii).nRestraintKey == nKey)
			return true;
	return false;
}

bool CDnRestraintTask::IsRestrainting( eRestraintType Type, TRestraint &Data )
{
	std::vector<TRestraint>::iterator iter;
	for( iter = m_RestraintList.begin(); iter != m_RestraintList.end(); ++iter ) {
		if( (*iter).nRestraintType == Type ) {

			// 현재 시간을 구해서, 사이값이라면,
			if( CSyncTimer::IsActive() ) {
				if( (*iter)._tBegineTime < CSyncTimer::GetInstance().GetCurTime() && (*iter)._tEndTime > CSyncTimer::GetInstance().GetCurTime() ) {
					Data = (*iter);
					return true;
				}
			}
			else {
#ifndef _FINAL_BUILD
				//_ASSERT(0&&"DnRestraintTask. 동기화 타이머 왜 안쓰나.")
#endif
				time_t Time;
				time(&Time);
				if( (*iter)._tBegineTime <= Time && (*iter)._tEndTime >= Time ) {
					Data = (*iter);
					return true;
				}
			}
		}
	}
	return false;
}

bool CDnRestraintTask::CheckRestraint( eRestraintType Type, bool bMessageBox, bool bChatMsg )
{
	// 먼저 검사를 해서 걸리는지 확인
	WCHAR wszTemp[256];
	std::wstring wszBeginTime, wszEndTime;
	TRestraint Data;
	memset( &Data, 0, sizeof(TRestraint) );
	bool bCheck = IsRestrainting( Type, Data );
	if( bCheck ) {
		DN_INTERFACE::STRING::GetTimeText( wszBeginTime, Data._tBegineTime );
		DN_INTERFACE::STRING::GetTimeText( wszEndTime, Data._tEndTime );

#ifdef PRE_ADD_DWC
		if( GetDWCTask().IsDWCChar() == false ) // DWC캐릭터는 거래제제 메세지를 띄우지 않음.
#endif
		{
			switch( Type )
			{
			case _RESTRAINTTYPE_BLOCK:
				break;
			case _RESTRAINTTYPE_CHAT:
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4404 ), Data.wszRestraintReason, wszBeginTime.c_str(), wszEndTime.c_str() );
				if( bMessageBox ) GetInterface().MessageBox( wszTemp );
				if( bChatMsg ) GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, true );
				break;
			case _RESTRAINTTYPE_TRADE:
				if( bMessageBox ) {
					swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4407 ) );
					GetInterface().MessageBox( wszTemp );
				}
				if( bChatMsg ) {
					swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4406 ), Data.wszRestraintReason, wszBeginTime.c_str(), wszEndTime.c_str() );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, true );
				}
				break;
			}
		}
	}
	return bCheck;
}

#ifdef PRE_ADD_ANTI_CHAT_SPAM
bool CDnRestraintTask::CheckUSAChatSpam( const wstring& strChat )
{
	return m_ChatSpamChecker.Check( strChat );
}

bool CDnChatSpamChecker::IsPunct( wchar_t Char )
{
	switch( Char )
	{
		case L'^':
		case L'-':
		case L'!':
		case L'\"':
		case L'#':
		case L'$':
		case L'%':
		case L'&':
		case L'\'':
		case L'(':
		case L')':
		case L'*':
		case L'+':
		case L',':
		case L'.':
		case L'/':
		case L':':
		case L';':
		case L'<':
		case L'=':
		case L'>':
		case L'?':
		case L'@':
		case L'[':
		case L'\\':
		case L']':
		case L'_':
		case L'`':
		case L'{':
		case L'|':
		case L'}':
		case L'~':
			return true;
			break;
	}

	return false;
}

void CDnChatSpamChecker::TokenizeExceptPunct( wstring& strSource, set<wstring>& setResult )
{
	// 특수 문자는 모두 공백으로 변환.
	wchar_t CharSpace = L' ';
	wstring::size_type NowPos = 0;
	wstring::size_type LastPos = 0;
	for( int i = 0; i < (int)strSource.length(); ++i )
	{
		wchar_t Char = strSource.at( i );
		if( IsPunct(Char) )
			strSource.at( NowPos ) = CharSpace;

		++NowPos;
	}

	// 맨 첫 글자가 구분자인 경우 무시
	std::wstring::size_type lastPos = strSource.find_first_not_of( CharSpace, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::wstring::size_type pos = strSource.find_first_of( CharSpace, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		setResult.insert( strSource.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = strSource.find_first_not_of( CharSpace, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = strSource.find_first_of( CharSpace, lastPos );
	}
}

// 스팸으로 걸리면 true 리턴.
bool CDnChatSpamChecker::Check( const wstring& strChat )
{
	bool bIsSpammer = false;
	DWORD dwNowTime = timeGetTime();

	// 먼저 리스트에 마지막으로 입력된 시간으로부터 일정 시간 간격 
	// 이상의 문자열이 있으면 빼줌.
	list<S_CHAT_INFO>::iterator iter = m_listChatHistory.begin();
	for( iter; iter != m_listChatHistory.end(); )
	{
		if( SPAM_CHECK_TIME < dwNowTime - iter->dwTime )
		{
			// 스팸 카운트가 정해진 것 이상 넘어간 상태라면 시간 다 돼도 
			// 스팸 카운트만큼만 빼주고 계속 유지.
			if( SPAM_JUDGEMENT_COUNT <= iter->iSpamCount )
			{
				iter->iSpamCount -= SPAM_JUDGEMENT_COUNT;
				iter->dwTime = dwNowTime;
			}
			else
			{
				iter = m_listChatHistory.erase( iter );
			}
		}
		else
		{
			++iter;
		}
	}

	wstring strChatToCheck = strChat;
	ToLowerW( strChatToCheck );
	
	vector<wstring> vlTokens;
	set<wstring> setWords;
	TokenizeExceptPunct( strChatToCheck, setWords );

	bool bIsSpamChat = false;
	if( false == m_listChatHistory.empty() )
	{
		iter = m_listChatHistory.begin();
		for( iter; iter != m_listChatHistory.end(); ++iter )
		{
			// 우선 같은 문자열이 있는지 확인.
			S_CHAT_INFO& Chat = *iter;
			if( Chat.strSentence == strChat )
			{
				Chat.iSpamCount++;
				Chat.dwTime = dwNowTime;
				bIsSpamChat = true;
			}
			else
			{
				// 아니면 같은 단어별로 일정 비율 이상 같은 문자열이 있는지 확인.
				vector<wstring> vlSameWords;
				set_intersection( Chat.setWords.begin(), Chat.setWords.end(),
								  setWords.begin(), setWords.end(), 
								  back_inserter(vlSameWords) );

				if( false == vlSameWords.empty() )
				{
					int iWholeWordsCount = (int)Chat.setWords.size();
					int iSameWordsCount = (int)vlSameWords.size();
					float fSameRatio = (float)iSameWordsCount / (float)iWholeWordsCount;
					if( SAME_WORDS_TOLERANCE <= fSameRatio )
					{
						Chat.iSpamCount++;
						Chat.dwTime = dwNowTime;
						bIsSpamChat = true;
					}
				}
			}

			// 일정 횟수 이상 비슷한 채팅 도배로 판단됨..
			if( bIsSpamChat && (SPAM_JUDGEMENT_COUNT <= Chat.iSpamCount) )
			{
				bIsSpammer = true;
			}
		}
	}

	// 업데이트 시간 기준으로 가장 최근에 입력된 채팅 문자열이 앞으로 오도록 정렬.
	m_listChatHistory.sort();

	// 스팸으로 분류되지 않았다면 리스트 앞에 새로 달아둔다.
	if( false == bIsSpamChat )
	{
		S_CHAT_INFO Chat;
		Chat.dwTime = dwNowTime;
		Chat.setWords = setWords;
		Chat.strSentence = strChat;
		m_listChatHistory.push_front( Chat );
	}

	return bIsSpammer;
}
#endif // #ifdef PRE_ADD_ANTI_CHAT_SPAM