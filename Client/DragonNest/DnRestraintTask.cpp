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

	// Add�ɶ��� �ý��� �޼��� ����Ѵ�.
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

			// ���� �ð��� ���ؼ�, ���̰��̶��,
			if( CSyncTimer::IsActive() ) {
				if( (*iter)._tBegineTime < CSyncTimer::GetInstance().GetCurTime() && (*iter)._tEndTime > CSyncTimer::GetInstance().GetCurTime() ) {
					Data = (*iter);
					return true;
				}
			}
			else {
#ifndef _FINAL_BUILD
				//_ASSERT(0&&"DnRestraintTask. ����ȭ Ÿ�̸� �� �Ⱦ���.")
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
	// ���� �˻縦 �ؼ� �ɸ����� Ȯ��
	WCHAR wszTemp[256];
	std::wstring wszBeginTime, wszEndTime;
	TRestraint Data;
	memset( &Data, 0, sizeof(TRestraint) );
	bool bCheck = IsRestrainting( Type, Data );
	if( bCheck ) {
		DN_INTERFACE::STRING::GetTimeText( wszBeginTime, Data._tBegineTime );
		DN_INTERFACE::STRING::GetTimeText( wszEndTime, Data._tEndTime );

#ifdef PRE_ADD_DWC
		if( GetDWCTask().IsDWCChar() == false ) // DWCĳ���ʹ� �ŷ����� �޼����� ����� ����.
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
	// Ư�� ���ڴ� ��� �������� ��ȯ.
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

	// �� ù ���ڰ� �������� ��� ����
	std::wstring::size_type lastPos = strSource.find_first_not_of( CharSpace, 0 );
	// �����ڰ� �ƴ� ù ���ڸ� ã�´�
	std::wstring::size_type pos = strSource.find_first_of( CharSpace, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token�� ã������ vector�� �߰��Ѵ�
		setResult.insert( strSource.substr( lastPos, pos - lastPos ) );
		// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
		lastPos = strSource.find_first_not_of( CharSpace, pos );
		// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
		pos = strSource.find_first_of( CharSpace, lastPos );
	}
}

// �������� �ɸ��� true ����.
bool CDnChatSpamChecker::Check( const wstring& strChat )
{
	bool bIsSpammer = false;
	DWORD dwNowTime = timeGetTime();

	// ���� ����Ʈ�� ���������� �Էµ� �ð����κ��� ���� �ð� ���� 
	// �̻��� ���ڿ��� ������ ����.
	list<S_CHAT_INFO>::iterator iter = m_listChatHistory.begin();
	for( iter; iter != m_listChatHistory.end(); )
	{
		if( SPAM_CHECK_TIME < dwNowTime - iter->dwTime )
		{
			// ���� ī��Ʈ�� ������ �� �̻� �Ѿ ���¶�� �ð� �� �ŵ� 
			// ���� ī��Ʈ��ŭ�� ���ְ� ��� ����.
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
			// �켱 ���� ���ڿ��� �ִ��� Ȯ��.
			S_CHAT_INFO& Chat = *iter;
			if( Chat.strSentence == strChat )
			{
				Chat.iSpamCount++;
				Chat.dwTime = dwNowTime;
				bIsSpamChat = true;
			}
			else
			{
				// �ƴϸ� ���� �ܾ�� ���� ���� �̻� ���� ���ڿ��� �ִ��� Ȯ��.
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

			// ���� Ƚ�� �̻� ����� ä�� ����� �Ǵܵ�..
			if( bIsSpamChat && (SPAM_JUDGEMENT_COUNT <= Chat.iSpamCount) )
			{
				bIsSpammer = true;
			}
		}
	}

	// ������Ʈ �ð� �������� ���� �ֱٿ� �Էµ� ä�� ���ڿ��� ������ ������ ����.
	m_listChatHistory.sort();

	// �������� �з����� �ʾҴٸ� ����Ʈ �տ� ���� �޾Ƶд�.
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