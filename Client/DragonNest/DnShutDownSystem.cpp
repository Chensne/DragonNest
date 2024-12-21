#include "StdAfx.h"
#include "DnShutDownSystem.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "SyncTimer.h"
#include "TimeSet.h"

#ifdef PRE_ADD_SHUTDOWN_CHILD

#define CHECK_TIMER 30
#define DEFAULT_AGE	256

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnShutDownSystem::CDnShutDownSystem() : m_Age(DEFAULT_AGE), m_CheckTimer(0.f)
{
#if defined(_WORK)
	m_testEndHour = 0;
	m_testStartHour = 0;
#endif

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	m_bSelective = false; // �������˴ٿ��� ����ΰ�?
	m_tSelectiveHour = 0;
	m_tSelectiveMin = 0;
#endif

}

CDnShutDownSystem::~CDnShutDownSystem(void)
{
	m_NoticeInfo.clear();

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	m_NoticeInfoSelective.clear();
#endif
}

bool CDnShutDownSystem::Initialize()
{
	DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TSHUTDOWNMSG);
	if (!pSox)
		return false;

	int i = 0;
	for (; i <= pSox->GetItemCount(); ++i)
	{
		SNotice curNotice;
		int itemID = pSox->GetItemID( i );
		DNTableCell* pTimeField = pSox->GetFieldFromLablePtr(itemID, "_Time");
		if (pTimeField != NULL)
		{
			const char* pTimeString = pTimeField->GetString();
			if (pTimeString)
			{
				std::string tempTimeStr(pTimeString), cur;
				std::string::size_type stringPos;
				stringPos = tempTimeStr.find_first_of(":");
				if (stringPos != std::string::npos)
				{
					cur = tempTimeStr.substr(0, stringPos);
					curNotice.hour = atoi(cur.c_str());

					cur = tempTimeStr.substr(stringPos + 1, std::string::npos);
					curNotice.minute = atoi(cur.c_str());
				}
			}
		}
		else
		{
			_ASSERT(0);
			return false;
		}

		DNTableCell* pMsgTypeField = pSox->GetFieldFromLablePtr(itemID, "_MsgClass");
		if (pMsgTypeField != NULL)
		{
			int tempMsgType = pMsgTypeField->GetInteger();
			if (tempMsgType >= eMsgType_Min && tempMsgType < eMsgType_Max)
			{
				curNotice.msgType = (eMsgType)tempMsgType;
			}
			else
			{
				_ASSERT(0);
				return false;
			}
		}
		else
		{
			_ASSERT(0);
			return false;
		}

		DNTableCell* pMsgIDField = pSox->GetFieldFromLablePtr(itemID, "_MsgID");
		if (pMsgIDField)
		{
			curNotice.msgUIStringNum = pMsgIDField->GetInteger();
		}
		else
		{
			_ASSERT(0);
			return false;
		}

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
		// �������˴ٿ���.
		DNTableCell* pShutDownTypeField = pSox->GetFieldFromLablePtr(itemID, "_ShutdownType");
		if (pShutDownTypeField)
		{
			curNotice.bSelective = pShutDownTypeField->GetInteger() == 0 ? false : true;
		}
		else
		{
			_ASSERT(0);
			return false;
		}
#endif

		if (curNotice.IsEmpty() == false)
		{
#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
			
			if( curNotice.bSelective ) // �������˴ٿ���.
				m_NoticeInfoSelective.push_back(curNotice);
			else // �˴ٿ���.
				m_NoticeInfo.push_back(curNotice);

#else
			m_NoticeInfo.push_back(curNotice);
#endif

		}
	}

#ifdef _WORK
	m_testStartHour = -1;
	m_testEndHour = -1;
#endif

	return true;
}

void CDnShutDownSystem::Activate(USHORT age)
{
	m_Age = age;

}

void CDnShutDownSystem::Process(float fElapsedTime)
{
	if (IsShutDownUser())
	{
		if (CSyncTimer::GetInstance().IsStarted() && m_CheckTimer >= CHECK_TIMER)
		{
			const __time64_t curTime = CSyncTimer::GetInstance().GetCurTime();
			eMsgType type;
			std::wstring result;
			if (CheckShutDownMsg(type, result, curTime) == eNoticed)
				NoticeShutDownMsg(type, result);

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
			if (m_bSelective && CheckSelectiveShutDownMsg(type, result, curTime) == eNoticed)
				NoticeShutDownMsg(type, result);
#endif

			m_CheckTimer = 0.f;
		}

		m_CheckTimer += fElapsedTime;
	}
}

bool CDnShutDownSystem::IsShutDownUser() const
{
#if defined (_KR)
	return (m_Age < CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemLimitAge));
#else
	return false;
#endif
}


#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
// �������˴ٿ���.

#if defined(_KRAZ)
void CDnShutDownSystem::SetSelectiveShutDown( int nHour )
{
	// �������˴ٿ��� �ð�.
	if( nHour > 6 ) 
	{
		m_bSelective = true;
		m_tSelectiveHour = nHour;
		m_tSelectiveMin = 0;
	}
	// 0~6 �ô� �����˴ٿ� �ð�.
	else
	{
		m_bSelective = false;
	}
}

#else	// #if defined(_KRAZ)
void CDnShutDownSystem::SetSelectiveShutDown( int nShutDownTime )
{
	char buf[128]={0,};
	std::string strTime( itoa( nShutDownTime, buf, 10 ) );

	int size = (int)strTime.size();
	if( size > 2 )
	{
		strTime = strTime.substr( size-2, size );

		int hour = atoi( strTime.c_str() );

		// �������˴ٿ��� �ð�.
		if( hour > 6 ) 
		{
			m_bSelective = true;
			m_tSelectiveHour = hour;
			m_tSelectiveMin = 0;
		}

		// 0~6 �ô� �����˴ٿ� �ð�.
		else
		{
			m_bSelective = false;
		}
	}
	else
		m_bSelective = false;
}
#endif	// #if defined(_KRAZ)

CDnShutDownSystem::eNoticeReturnType CDnShutDownSystem::CheckSelectiveShutDownMsg(eMsgType& resultType, std::wstring& resultMsg, const __time64_t& currentTime)
{
	tm date;
	::_localtime64_s(&date, &currentTime);

	// �����ð����κ��� �����ð����.	
	__time64_t tCurTime = CTimeSet::ConvertTmToTimeT64_LC( &date ); // ����ð�.

	struct tm tmShutDown;
	tmShutDown = date;

	//// ����ð��� �˴ٿ���۽ð����� ũ�� ������.
	if( date.tm_hour > m_tSelectiveHour )
		tmShutDown.tm_mday++;
	// ����ð��� �˴ٿ���۽ð����� ũ�� ����.
	//if( date.tm_hour > m_tSelectiveHour )
	//	return eNoticeReturnType::eError;

	tmShutDown.tm_hour = m_tSelectiveHour;
	tmShutDown.tm_min = m_tSelectiveMin;
	//tmShutDown.tm_sec = 0;
	__time64_t startTimeT = CTimeSet::ConvertTmToTimeT64_LC( &tmShutDown );

	double diff = _difftime64( startTimeT, tCurTime );
	double diffHour = diff / ( 60.0f * 60.0f );
	diffHour = diffHour < 1.0f ? 0.0f : diffHour;
	double diffMin = diff / 60;

	std::vector<SNotice>::iterator iter = m_NoticeInfoSelective.begin();
	for (; iter != m_NoticeInfoSelective.end(); ++iter)
	{
		SNotice& curNotice = (*iter);
		if (curNotice.IsEmpty() == false && curNotice.IsNoticed() == false && (curNotice.hour == diffHour && curNotice.minute == diffMin))
		{
			std::wstring str;
			str = FormatW( L"%d�� %d��", (int)diffHour, (int)diffMin );
			//str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum );

			// ������ �˴ٿ����� ���� %d�� �� ������ ����˴ϴ�. ������������ ������ ������ �ּ���.
			str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), (int)diffMin + (int)diffHour * 60 );

			if (curNotice.msgType < eMsgType_Min || curNotice.msgType >= eMsgType_Max)
			{
				_ASSERT(0);
				return eError;
			}

			resultType = curNotice.msgType;
			resultMsg = str;

			curNotice.bNoticed = true;

			return eNoticed;
		}
	}

	return eUnNoticed;
}
#endif


CDnShutDownSystem::eNoticeReturnType CDnShutDownSystem::CheckShutDownMsg(eMsgType& resultType, std::wstring& resultMsg, const __time64_t& currentTime)
{
	tm date;
	::_localtime64_s(&date, &currentTime);

	std::vector<SNotice>::iterator iter = m_NoticeInfo.begin();
	for (; iter != m_NoticeInfo.end(); ++iter)
	{
		SNotice& curNotice = (*iter);
		if (curNotice.IsEmpty() == false && curNotice.IsNoticed() == false && (curNotice.hour == date.tm_hour && curNotice.minute == date.tm_min))
		{
			std::wstring str;
#ifdef _WORK
			int startHour = (m_testStartHour >= 0) ? m_testStartHour : (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemStartTime);
			int endHour = (m_testEndHour >= 0) ? m_testEndHour : (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemEndTime);
			int limitAge = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemLimitAge);
#else
			int startHour = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemStartTime);
			int endHour = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemEndTime);
			int limitAge = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemLimitAge);
#endif

			struct tm ShutDownStartTime;
			ShutDownStartTime = date;

			ShutDownStartTime.tm_hour = curNotice.hour;
			ShutDownStartTime.tm_min = curNotice.minute;
			__time64_t curNoticeTimeT = CTimeSet::ConvertTmToTimeT64_LC(&ShutDownStartTime);

			if (date.tm_hour > startHour)
				ShutDownStartTime.tm_mday++;
			ShutDownStartTime.tm_hour = startHour;
			ShutDownStartTime.tm_min = 0;

			__time64_t startTimeT = CTimeSet::ConvertTmToTimeT64_LC(&ShutDownStartTime);

			double diff = _difftime64(startTimeT, curNoticeTimeT);
			double diffMin = diff / 60;

			if (curNotice.msgUIStringNum == 3330) // UISTRING : û�ҳ� ��ȣ���� ���� %d�ú��� %d�ñ����� �� %d�� ���� ������ ������ ����� �� �����ϴ�.
			{
				str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), startHour, endHour, limitAge);
			}
			else if (curNotice.msgUIStringNum == 3331) // UISTRING : û�ҳ� ��ȣ���� ���� %d�ú��� %d�ñ����� �� %d�� ���� ������ ������ ����� �� �����ϴ�. %d�ÿ� ������ �ڵ� ���� �ǿ��� �� �� ���� �ٶ��ϴ�.
			{
				str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), startHour, endHour, limitAge, startHour);
			}
			else if (curNotice.msgUIStringNum == 3332) // UISTRING : ���� ������� ������ %d�� ���ҽ��ϴ�.
			{
				str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), (int)diffMin);
			}
			else if (curNotice.msgUIStringNum == 3333) // UISTRING : %d�� �� ������ �ڵ� ����˴ϴ�. ������������ ������ ������ �ֽñ� �ٶ��ϴ�.
			{
				str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), (int)diffMin);
			}
			else
			{
				_ASSERT(0);
				return eError;
			}

			if (curNotice.msgType < eMsgType_Min || curNotice.msgType >= eMsgType_Max)
			{
				_ASSERT(0);
				return eError;
			}

			resultType = curNotice.msgType;
			resultMsg = str;

			curNotice.bNoticed = true;

			return eNoticed;
		}
	}

	return eUnNoticed;
}

bool CDnShutDownSystem::NoticeShutDownMsg(const CDnShutDownSystem::eMsgType& msgType, const std::wstring msg)
{
	if (msgType== eChatWindow)
	{
		CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), false);
	}
	else if (msgType == eCaptionCenter)
	{
		CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), true);
	}
	else if (msgType == eSlideUnder)
	{
		CDnInterface::GetInstance().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), false);
		CDnInterface::GetInstance().AddNoticeMessage(msg.c_str(), 10);
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	return true;
}

#ifdef _WORK
void CDnShutDownSystem::TestShutDownMsg(int testStartHour, int testEndHour)
{
	if (testStartHour > 0 && testEndHour > 0)
	{
		m_testStartHour = testStartHour;
		m_testEndHour = testEndHour;

		int testHour = (testStartHour == 0) ? 23 : testStartHour - 1;
		std::vector<SNotice>::iterator iter = m_NoticeInfo.begin();
		for (; iter != m_NoticeInfo.end(); ++iter)
		{
			SNotice& curNotice = (*iter);
			if (curNotice.IsEmpty() == false)
			{
				curNotice.hour = testHour;
				curNotice.bNoticed = false;
			}
		}
	}

	if (testStartHour < 0 && testEndHour < 0)
	{
		int startHour = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemStartTime);
		int endHour = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemEndTime);
		int limitAge = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemLimitAge);

		std::wstring str;
		const SNotice& curNotice = m_NoticeInfo[0];
		if (curNotice.msgUIStringNum == 3330) // UISTRING : û�ҳ� ��ȣ���� ���� %d�ú��� %d�ñ����� �� %d�� ���� ������ ������ ����� �� �����ϴ�.
		{
			str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), startHour, endHour, limitAge);
		}
		else if (curNotice.msgUIStringNum == 3331) // UISTRING : û�ҳ� ��ȣ���� ���� %d�ú��� %d�ñ����� �� %d�� ���� ������ ������ ����� �� �����ϴ�. %d�ÿ� ������ �ڵ� ���� �ǿ��� �� �� ���� �ٶ��ϴ�.
		{
			str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum ), startHour, endHour, limitAge, startHour);
		}
		else
		{
			str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, curNotice.msgUIStringNum );
		}

		NoticeShutDownMsg(curNotice.msgType, str);
	}
}
#endif

#endif // PRE_ADD_SHUTDOWN_CHILD