#pragma once

#ifdef PRE_ADD_SHUTDOWN_CHILD

class CDnShutDownSystem
{
public:
#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST( ShutDownSystemClient_unittest, SYSTEM_TEST );
#endif // #if !defined( _FINAL_BUILD )

	enum eMsgType
	{
		eMsgType_Min = 1,
		eChatWindow = eMsgType_Min,
		eCaptionCenter,
		eSlideUnder,
		eMsgType_Max
	};

	enum eNoticeReturnType
	{
		eError,
		eNoticed,
		eUnNoticed,
	};

	struct SNotice
	{
		int hour;
		int minute;
		eMsgType msgType;
		int msgUIStringNum;
		bool bNoticed;
		
#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
		bool bSelective; // 선택적셧다운제(true), 셧다운제(false).
#endif

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
		SNotice() : hour(-1), minute(-1), msgType(eChatWindow), msgUIStringNum(0), bNoticed(false), bSelective(false) {}
#else
		SNotice() : hour(-1), minute(-1), msgType(eChatWindow), msgUIStringNum(0), bNoticed(false) {}
#endif	
		bool IsEmpty() const { return (hour < 0 || minute < 0); }
		bool IsNoticed() const { return (bNoticed); }
	};

	CDnShutDownSystem();
	virtual ~CDnShutDownSystem(void);
	bool Initialize();
	void Activate(USHORT age);

	void Process(float fElapsedTime);
	bool IsShutDownUser() const;
	eNoticeReturnType CheckShutDownMsg(eMsgType& resultType, std::wstring& resultMsg, const __time64_t& currentTime);

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	eNoticeReturnType CheckSelectiveShutDownMsg(eMsgType& resultType, std::wstring& resultMsg, const __time64_t& currentTime); // #59492 한국 선택적 셧다운제.
#endif

	bool NoticeShutDownMsg(const CDnShutDownSystem::eMsgType& msgType, const std::wstring msg);
#ifdef _WORK
	void TestShutDownMsg(int testStartHour, int testEndHour);
#endif

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	// 선택적셧다운제.
#if defined(_KRAZ)
	void SetSelectiveShutDown( int nHour );	
#else	// #if defined(_KRAZ)
	void SetSelectiveShutDown( int nShutDownTime );	
#endif	// #if defined(_KRAZ)
#endif

private:
	USHORT m_Age;
	float m_CheckTimer;

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	// 선택적셧다운제.
	bool m_bSelective; // 선택적셧다운제 대상인가?
	int m_tSelectiveHour;
	int m_tSelectiveMin;	
#endif



#ifdef _WORK
	int m_testStartHour;
	int m_testEndHour;
#endif

	std::vector<SNotice> m_NoticeInfo;

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	std::vector<SNotice> m_NoticeInfoSelective; // 선택적셧다운제.
#endif
};

#endif // PRE_ADD_SHUTDOWN_CHILD