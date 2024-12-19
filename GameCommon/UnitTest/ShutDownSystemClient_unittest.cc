#include "stdafx.h"
#include "DnShutDownSystem.h"
#include "mmsystem.h"
#include "DnTableDB.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
#ifdef PRE_ADD_SHUTDOWN_CHILD

class ShutDownSystemClient_unittest:public testing::Test
{
protected:
	static void SetUpTestCase()
	{
		m_pResMng = new CEtResourceMng( false, true );
		EXPECT_TRUE( m_pResMng->AddResourcePath( "r:\\gameres\\resource\\ext" ) );
		EXPECT_TRUE( m_pResMng->AddResourcePath( "r:\\gameres\\resource\\uistring" ) );

		m_pTableDB = new CDnTableDB();
		EXPECT_TRUE( m_pTableDB->Initialize() );

		m_pUIXML = new CEtUIXML;
		EtInterface::xml::SetXML( m_pUIXML );
		EXPECT_TRUE( EtInterface::xml::IsValid() );

		if (m_pUIXML)
		{
			CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
			m_pUIXML->Initialize( pStream, CEtUIXML::idCategory1 );
			SAFE_DELETE( pStream );
		}
	}

	static void TearDownTestCase()
	{
		SAFE_DELETE(m_pResMng);
		SAFE_DELETE(m_pTableDB);
		SAFE_DELETE(m_pUIXML);
	}

	virtual void SetUp()
	{
		m_ShutDownSystem.Initialize();
		_srand(timeGetTime());
	}

	virtual void TearDown() {}

	static CEtResourceMng* m_pResMng;
	static CDnTableDB* m_pTableDB;
	static CEtUIXML *m_pUIXML;
	CDnShutDownSystem m_ShutDownSystem;
};

CEtResourceMng* ShutDownSystemClient_unittest::m_pResMng = NULL;
CDnTableDB* ShutDownSystemClient_unittest::m_pTableDB = NULL;
CEtUIXML* ShutDownSystemClient_unittest::m_pUIXML = NULL;

TEST_F( ShutDownSystemClient_unittest, SYSTEM_TEST )
{
	ASSERT_FALSE(m_ShutDownSystem.m_NoticeInfo.size() <= 0);

	USHORT age = (USHORT)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ShutDownSystemLimitAge);
	EXPECT_FALSE(age <= 0);

	m_ShutDownSystem.Activate(age - 1);

	__time64_t curTime;
	_time64(&curTime);
	tm date;
	DnLocalTime_s(&date, &curTime);

	const std::vector<CDnShutDownSystem::SNotice>& noticeInfoVector = m_ShutDownSystem.m_NoticeInfo;

	int i = 0;
	for (; i < 10; ++i)
	{
		struct tm ShutDownStartTime;
		ShutDownStartTime = date;
		ShutDownStartTime.tm_hour = _rand() % 24;
		ShutDownStartTime.tm_min = _rand() % 60;
		std::vector<CDnShutDownSystem::SNotice>::const_iterator iter = noticeInfoVector.begin();
		bool bCheck = true;
		for (; iter != noticeInfoVector.end(); ++iter)
		{
			const CDnShutDownSystem::SNotice& notice = (*iter);
			if (notice.hour == ShutDownStartTime.tm_hour && notice.minute == ShutDownStartTime.tm_min)
			{
				bCheck = false;
				break;
			}
		}

		if (bCheck)
		{
			__time64_t startTimeT = _mktime64(&ShutDownStartTime);

			CDnShutDownSystem::eMsgType type;
			std::wstring result;
			CDnShutDownSystem::eNoticeReturnType ret = m_ShutDownSystem.CheckShutDownMsg(type, result, startTimeT);
			EXPECT_TRUE(ret == CDnShutDownSystem::eUnNoticed);
		}
	}

	std::vector<CDnShutDownSystem::SNotice>::const_iterator iter = noticeInfoVector.begin();
	for (; iter != noticeInfoVector.end(); ++iter)
	{
		const CDnShutDownSystem::SNotice& notice = (*iter);
		struct tm ShutDownStartTime;
		ShutDownStartTime = date;
		ShutDownStartTime.tm_hour = notice.hour;
		ShutDownStartTime.tm_min = notice.minute;
		__time64_t startTimeT = _mktime64(&ShutDownStartTime);

		CDnShutDownSystem::eMsgType type;
		std::wstring result;
		CDnShutDownSystem::eNoticeReturnType ret = m_ShutDownSystem.CheckShutDownMsg(type, result, startTimeT);
		EXPECT_TRUE(ret == CDnShutDownSystem::eNoticed);
		EXPECT_TRUE(type >= CDnShutDownSystem::eMsgType_Min && type < CDnShutDownSystem::eMsgType_Max);
		EXPECT_FALSE(result.empty());
	}
}

#endif // #ifdef PRE_ADD_SHUTDOWN_CHILD
#endif // #if !defined( _FINAL_BUILD )
