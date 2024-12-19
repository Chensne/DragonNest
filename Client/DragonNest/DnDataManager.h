#pragma once
#include "DNPacket.h"



/** 대화시 질문에 필요한 구조체 질문메세지만 가지고 있다.
*/
struct TALK_QUESTION
{
	std::wstring szQuestion;
};

/** 대화시 응답에 필요한 구조체 이 응답을 선택 할 경우 어떤 대화로 이어지는 정보도 포함되어 있다.
*/
struct TALK_ANSWER 
{
	/// 응답시 링크될 타겟(다른 대사파일 혹은 다른 퀘스트 파일)
	std::wstring szLinkTarget;
	/// 응답시 링크될 인덱스
	std::wstring szLinkIndex;
	/// 응답 메세지
	std::wstring szAnswer;
	/// 다른 토크파일로 링크 하는 경우 true
	bool		 bOtherTargetLink;	

#if defined ( PRE_ADD_MAINQUEST_UI )
	BYTE		 cSelectType; // 0:선택지, 2:계속, 3:수락, 4:완료, 5:대화종료, 6:돌아가기
#endif // #if defined ( PRE_ADD_MAINQUEST_UI )
};

/** npc 와의 한번의 대화시 대화창에 표시되는 단위 이다 
*/
struct TALK_PARAGRAPH
{
	/// 이 대사문단의 인덱스
	std::wstring				szIndex;
	/// 이 대사문단이 세팅될때의 배경 이미지 이름
	std::string					szImageName;
	/// 이 대사문단이 세팅될때의 아이템 인덱스
	int								nItemIndex;
	/// 질문 구조체
	TALK_QUESTION				Question;
	/// 질문에 따른 해답 구조체 목록
	std::vector<TALK_ANSWER>	Answers;

#if defined ( PRE_ADD_MAINQUEST_UI )
	std::string					szLeftPortraitName; // 왼쪽에 나타나는 Npc portrait
	std::string					szRightPortraitName; // 오른쪽에 나타나는 Npc portrait
#endif // #if defined ( PRE_ADD_MAINQUEST_UI )
};

/// key - 문단스트링인덱스, second - 문단
typedef std::map<std::wstring, TALK_PARAGRAPH>	TALK_PARAGRAPH_MAP;
typedef TALK_PARAGRAPH_MAP::iterator			TALK_PARAGRAPH_MAP_IT;

/// key - 대사xml 파일 이름, second - 대사전체
typedef std::map<std::wstring, TALK_PARAGRAPH_MAP> TALK_MAP;
typedef TALK_MAP::iterator						   TALK_MAP_IT;

/// 저널 아이템
struct  JournalItem
{
	enum Type
	{
		Item = 1,
		Monster = 2,
		Gold = 3
	};

	/// 타입은 위 JournalItem::Type 값이다.
	char cType;
	/// item, monster 일땐 인덱스, Glod 일땐 금액
	UINT nValue;
	/// 갯수
	UINT nCount;
};

/// 퀘스트 알림이 방향지시자의 타입
struct ArrowType
{
	enum 
	{
		Hide = 0,
		NormalArrow,
		ArrivalMark,
		BattleMark,
		FindMark,
		MaxCount
	};
};

/** 퀘스트저널페이지에 필요한 값들을 세팅하고 있는 구조체
@remarks
특정 퀘스트를 완료하는 동안에 퀘스트 저널 단계가 10개 일경우 저널페이지는 10개가 된다.
*/
struct JournalPage 
{
	/*
	none - 기본
	hide - 전부 끄는거
	arrival - 전부 도착
	stage_hide - 아무 존맵(던전)에 들어가면 끄는거
	stage_arrival - 아무 존맵(던전)에 들어가면 도착아이콘
	select_hide:10,11,12 - 선택된(10,11,12) 맵에 있으면 hide
	select_arrival:10,11,12 - 선택된(10,11,12) 맵에 있으면 도착아이콘
	stage_hide_except:10,11,12 - 10,11,12 맵을 제외한 스테이지맵에서 끈다.
	custom:10@1,10@2 - 기본적으로 던젼맵은 HIDE 표시 월드맵은 NORMAL 표시 하고 값이 있는 것들은 해당 아이콘 표시
	*/


	enum DestinationMode
	{
		none = 0,
		hide,
		arrival,
		stage_hide,
		stage_arrival,
		select_hide,
		select_arrival,
		stage_hide_except,
		custom,
	};

	struct DestinationMark
	{
		int nMapIdx;
		int nMarkIdx;
	};
		
	bool bOnlyMinimap;
	int nStateMarkNpcID;
	int nStateMarkType;
	int	nJournalPageIndex;
	int nChapterIndex;
	int	nDestnationMapIndex;
	int nDestinationNpc;
	int	nDestnationMode; 

	std::wstring szTitle; 
	std::wstring szTitleImage;
	std::wstring szContents; 
	std::wstring szContentsImage; 
	std::wstring szTodoMsg;
	std::wstring szDestination; 

	std::vector<int> DestnationModeValue;
	std::vector<JournalItem> NeedItemList; 
	std::vector<DestinationMark>	CustomDestnationMark; 
		
	D3DXVECTOR2  vDestinationPos; 

	JournalPage()
	{
		bOnlyMinimap = false;
		nStateMarkNpcID = 0;
		nStateMarkType = 0;
		nJournalPageIndex = 0;
		nChapterIndex = 0;
		nDestnationMapIndex = 0;
		nDestnationMode = 0;
		nDestinationNpc = -1;

		vDestinationPos = D3DXVECTOR2(0.f,0.f);
	}

};

/** 저널 구조체
@remarks
퀘스트의 진행에 필요한 저널(일지)정보 이다. 저널 정보에는 기본적인 퀘스트 정보 표시에 필요한 정보들과
전체 저널페이지를 가지고 있다. 클라이언트에서는 현재의 저널스텝에 따라 그 페이지 까지만 보여준다.
*/ 
struct Journal 
{
	enum {
		emQuestType_Main = 1,
		emQuestType_Sub,
		emQuestType_Day,
		emQuestType_Rep,
	};

	/// 퀘스트 인덱스
	UINT						nQuestIndex;
	/// 퀘스트 제목
	std::wstring				wszQuestTitle;
	/// 퀘스트 수행 레벨 제목
	std::wstring				wszQuestLevelTitle;
	/// 퀘스트 수행 레벨
	int							nQuestLevel;
	/// 퀘스트 마크 타입 ( 1이면 느낌표 한개 3이면 3개)
	int							nQuestMarkType;
	
	typedef std::vector<JournalPage>	JOURNALPAGE_VEC;
	/// 저널 페이지들..
	JOURNALPAGE_VEC				JounalPages;

	/// 챕터인덱스
	int							nChapterIndex;

	/// 보상 테이블 아이디 
	int							nRecompenseTableID;
	int							nRecompenseTableIDForClass[ CLASSKINDMAX ];

	// 퀘스트 타입
	int							nQuestType;

	/// 생성자
	Journal()
		: nQuestIndex(0), nQuestLevel(0), nChapterIndex(0), nQuestMarkType(0), nRecompenseTableID(0), nQuestType(0)
	{
		for( int i = 0; i < CLASSKINDMAX; i++) {
			nRecompenseTableIDForClass[ i ] = 0;
		}
	}

	/** 현재 퀘스트저널에서 원하는 저널페이지를 얻는다.
	@param
	nJounalPageIndex 저널페이지 인덱스, 1부터 시작한다.
	*/
	JournalPage* FindJournalPage(int nJournalPageIndex)
	{
		for ( size_t i = 0 ; i < JounalPages.size() ; i++ )
		{
			if ( JounalPages[i].nJournalPageIndex == nJournalPageIndex )
				return &JounalPages[i];
		}
		return NULL;
	}

	bool IsMainQuest() { return (nChapterIndex > 0 );}

	int GetMyRecompenseID( int nClass )
	{
		int nClassIndex = nClass - 1;

		if( 0 > nClassIndex || CLASSKINDMAX <= nClassIndex )
			return nRecompenseTableID;

		int nClassRecompenseID = nRecompenseTableIDForClass[ nClassIndex ];
		
		if( nClassRecompenseID != 0 )
			return nClassRecompenseID;

		return nRecompenseTableID;
	}
};

/**	퀘스트 조건체크 인터페이스 스트럭쳐
*/
struct ConditionBase 
{
	bool bUse;
	virtual void Init(std::wstring& data) { if ( data.empty() ) bUse = false; }
	virtual bool Check() {	return false; }
	
	virtual bool ShowCheck() { return true;} 
	// ShowCheck() :: 퀘스트 StateMark를 안보이게 설정하기위해 씁니다 < 수락은 가능 > // 필요한 컨디션안에서 가상으로 추가 설정해주세요 
	// 현재 __UserLevel 에서 레벨이 10이상 차이가 나면 마크를 보이지 않기위해 사용합니다.  컨디션마다 이러한 제약조건이 추가되면 설정해주세요

	ConditionBase()
	{
		bUse = false;
	}
	virtual ~ConditionBase(){ }
};

/** 퀘스트 조건체크하는 객체 이다. 퀘스트 수락시 현재 레벨보다 높거나 이러한 경우들을 만족 시키지 못하면
	퀘스트 수락을 하지 못한다. 이런 조건들을 체크하는 객체이다.
*/ 
struct QuestCondition 
{
	UINT nQuestIndex;
	std::vector<ConditionBase*>	ConditionList;
	bool Check();
	bool ShowCheck();
	
	QuestCondition();
	virtual ~QuestCondition();
};

/** 퀘스트 테이블에서 로드한 퀘스트 정보를 담는 스트럭쳐
*/
struct QuestInfo 
{
	UINT				nQuestIndex;
	std::wstring		szQuestName;
	std::wstring		szQuestTalk;
	std::wstring		szQuestScript;
	int					nSortIndex;
	char				cQuestType;
	int					nMapIndex;
	bool				bLevelCapReward;
#if defined( PRE_ADD_MAINQUEST_UI )
	BYTE				cUIStyle;
#endif // #if defined( PRE_ADD_MAINQUEST_UI )
};

struct PeriodQuestInfo
{
	int nType;
	std::vector<int> vecQuestID;
};

struct TActorData
{
	int nActorID;
	BYTE cClass;						// 유닛 타입 종류
};

const int QUEST_MAX_CNT = 50;
struct TNpcData
{
	int nNpcID;
	int nParam[2];
	WCHAR wszName[EXTDATANAMELENMAX];
	char szTalkFile[256];					// 대사파일이름
	char szScriptFile[256];					// script 파일이름
	int QuestIndexArr[QUEST_MAX_CNT];		// 퀘스트인덱스
	int ReputeQuestIndexArr[QUEST_MAX_CNT];	// 호감도 퀘스트 인덱스
	TActorData ActorData;
};

typedef map<int, TActorData*> TMapActorData;
typedef map<int, TNpcData*> TMapNpcData;

typedef std::map<UINT, QuestCondition*>		QUEST_CONDITION_MAP;
typedef QUEST_CONDITION_MAP::iterator		QUEST_CONDITION_MAP_IT;

typedef std::map<UINT, Journal*>			JOURNAL_DATA_MAP;
typedef JOURNAL_DATA_MAP::iterator			JOURNAL_DATA_MAP_IT;

typedef std::map<std::wstring, QuestInfo>	QUEST_INFO_MAP;
typedef QUEST_INFO_MAP::iterator			QUEST_INFO_MAP_IT;

typedef std::vector< PeriodQuestInfo >			PERIOD_QUEST_INFO_VEC;
typedef PERIOD_QUEST_INFO_VEC::const_iterator	PERIOD_QUEST_INFO_VEC_IT;

typedef std::map<int, std::wstring>			TALK_PARAM_MAP;
typedef TALK_PARAM_MAP::iterator			TALK_PARAM_MAP_IT;

typedef map<DWORD, std::wstring> TXMLFileMap;
typedef map<DWORD, std::wstring> TXMLIndexMap;

/// 퀘스트 보상 아이템 테이블 정보 돈과 아이템 6종류 까지 세팅할 수 있다. nItemID 과 nItemCount는 세트이다.
const int RECOMPENSE_ITEM_MAX = 6;

/**	퀘스트 보상 테이블 스트럭쳐
*/
struct TQuestRecompense 
{
	struct _ITEMSET
	{
		int		nItemID;
		int		nItemCount;
	};

	/// 퀘스트 인덱스
	int			nQuestIndex;
	/// 보상테이블 인덱스
	UINT		nIndex;			
	/// 보상테이블 스트링
	UINT		nStringIndex;	
	/// 보상타입 1이면 전체보상 / 2면 선택보상
	char		cType;			
	/// 선택보상일경우 몇개까지 선택할것인지.
	char		cSelectMax;		
	/// 보상금액
	UINT		nCoin;			
	/// 보상 경험치
	int			nExp;			
	_ITEMSET	ItemArray[RECOMPENSE_ITEM_MAX];
	/// 보상 캐시템 캐시코모디티 테이블의 SN
	int			CashItemSNArray[RECOMPENSE_ITEM_MAX];
	int			nLevelCapStringIndex;
	int			nRewardFatigue;
	int			nOperator;
	int			nDifficult;
};

struct TQuestLevelCapRecompense
{
	struct _ITEMSET
	{
		int nItemID;
		int nItemCount;
	};

	char cType;
	char cClassID;
	_ITEMSET RecompenseItem[MAX_QUEST_LEVEL_CAP_REWARD];
};

typedef map<UINT, TQuestRecompense>		 QUEST_RECOMPENSE_MAP;
typedef QUEST_RECOMPENSE_MAP::iterator	 QUEST_RECOMPENSE_MAP_IT;

// 퀘스트 보상은 리워드 테이블 기준이며 , 스크립트에서 명시된 보상테이블 아이디로 설정되어있으므로 
// 퀘스트 아이디로 리워드의 역추적이 불가능, 그래서 리워드 테이블 아이디와 퀘스트 인덱스를 묶어둔 구조체 하나생성 , 서버도 가지고있음
typedef map<UINT,UINT>                        QUEST_INDEX_RECOMPENSE_MAP; 
typedef QUEST_INDEX_RECOMPENSE_MAP::iterator  QUEST_INDEX_RECOMPENSE_IT;

typedef map<tr1::tuple<char, char>, TQuestLevelCapRecompense> QUEST_LEVEL_CAP_RECOMPENSE_MAP;
typedef QUEST_LEVEL_CAP_RECOMPENSE_MAP::iterator QUEST_LEVEL_CAP_RECOMPENSE_IT;

// 대화의 문자 치환시 이벤트 콜백이 필요해서 ( 액션 플레이 ) 
// {action:cry_npc} 라고 대사에 포함 되어있다면 cry_npc 라는 액션을 npc 가 플레이 한다.
struct StaticParamCallback
{
	virtual void OnAction(std::string& szActionName) { }
	virtual void OnSound(std::string& szActionName) { }
};

/**
@brief
	퀘스트 관련 정보를 로드해서 가지고 있는 정보 컨테이너 클래스
@remarks
	퀘스트/npc 대사정보, 퀘스트저널 정보, 퀘스트보상 정보, npc 정보, 대사 치환 정보 
	등을 Load 및 Get 할수 있는 클래스 이다.
	g_DataManager 로 전역 외부 객체로 선언 되어있으며 정보가 필요한 곳에서 접근해서 사용하면 된다. 

@note
_ 로 시작하는 private 함수들은 대부분 많은 데이터들을 파싱할때 함수가 길어지는것을 분류하기 위해 여러 함수가 추가 되었다.
외부에서 직접적으로 호출할일은 없다.

*/
class CDnDataManager
{
public:

	enum eQuestTalkPriorityType
	{
		Main,
		Complete,
		Playing,
		Available,
		LowPriority,
		Text,
		Max,
	};
	/// 기본 생성자. 생성시 LoadData 를 호출하여 모든 정보를 로드 한다.
	CDnDataManager();
	/// 소멸자. 로드 햇던 데이터들을 해제한다.
	virtual ~CDnDataManager();

	/// 소멸자에서 호출해주는 소멸처리를 하고 있는 함수.
	void ClearAll();
	/// 전체 데이터를 로드 한다. 추가할 데이터가 있다면 여기서 로드 하도록 한다.
	bool LoadData();

	/**	대사문단을 얻는다.
	@param	wszIndex 대사문단의 인덱스, 맨처음이면 start 이다.
	@param	wszTarget 대사파일이름 ex)npc_001_halod.xml
	@param	talkParam 문자치환할 목록 이게 있다면 해당 문자열은 치환된다.
	@param	talk 참조값인 상태이므로 해당 대사문단을 얻는다면 저기에 세팅 된다.
	@param	pCallback 기본 대사 치환시 {action} {sound} 같이 문자치환이 아닌 다른 행위를 해야할 경우에 사용되는 콜백클래스이다.	StaticParamCallback 스트럭쳐를 참고.
	*/
	bool GetTalkParagraph(IN std::wstring& wszIndex, IN std::wstring& wszTarget, IN std::vector<TalkParam>& talkParam, OUT TALK_PARAGRAPH& talk, IN StaticParamCallback* pCallback = NULL );

	/**	대사파일의 인덱스나 문단파일이름을 hashcode 로 만들어서 서버와 통신한다.
	@remarks
	예를 들어 start.xml 라는 인덱스를 보낼때 start.xml 를 문자열로 보내는 것이 아니라 "start.xml" 의 hashcode 를 주고받음으로서
	어떠한 문단이나 인덱스를 선택했는지 구별한다.
	*/ 
	bool GetTalkFileName(DWORD nHashCode, OUT std::wstring& wszIndex);
	bool GetTalkIndexName(DWORD nHashCode, OUT std::wstring& wszIndex);

	/// 해당 문자열로 hashcode 를 생성한다.
	DWORD GetStringHashCode(const WCHAR* pString);
	
	/// 퀘스트 보상테이블을 로드한다.
	bool LoadQuestRecompense();
	/// 퀘스트 보상테이블을 얻는다. 
	bool GetQuestRecompense(UINT nRecompenseIndex, OUT TQuestRecompense& recom);
	bool GetQuestRecompenseByQuestIndex(UINT nQuestID,OUT TQuestRecompense& recom);

	bool LoadQuestLevelCapRecompense();
	bool GetQuestLevelCapRecompense( const char cQuestType, const char cClassID, OUT TQuestLevelCapRecompense & questLevelCapRecompense );

	/// 해당 퀘스트 의 저널 객체를 얻는다.
	Journal* GetJournalData(UINT nQuestIndex);
	/// 해당 퀘스트의 조건 객체를 얻는다.
	QuestCondition* GetQuestCondition(UINT nQuestIndex);

	/// Actor 정보 로드 
	bool LoadActorData();
	TActorData* GetActorData(int nActorIndex);
	
	/// NPC 데이터 로드
	bool LoadNpcData();
	bool IsExistStartQuestFromNpc( int nNpcIndex, int nQuestIndex );
	TNpcData* GetNpcData( int nNpcIndex );

	/** 기본적으로 정해져있는 대사치환을 할 경우에 사용되는 함수 
	@remarks
	기본 대사 치환시 {action} {sound} 같이 문자치환이 아닌 다른 행위를 해야할 경우에 사용되는 콜백클래스 이다.
	StaticParamCallback 스트럭쳐를 참고.
	*/ 
	void ReplaceStaticTalkParam(std::wstring& str, IN StaticParamCallback* pCallback = NULL );

	/// utility func
	std::vector<std::wstring> GetTalkFileList();

	/// 유틸리티 함수들
	std::wstring GetChapterString(int nChapterIdx);
	std::string GetChapterImageFileName(int nChapterIdx);
	std::string GetQuestImageFileName(int nQuestIndex);
	JOURNAL_DATA_MAP &GetJournalList() { return m_JournalMap; }
	char GetQuestType(int nQuestIndex) const;
	bool GetQuestHideType(int nQuestIndex);

#ifdef PRE_ADD_LOWLEVEL_QUEST_HIDE
	bool IsEventQuest( int nQuestindex );
	// 저레벨 퀘스트 필터링
	bool IsLowLevelQuest( Journal* pJournal);
#endif 
	/// 퀘스트대사파일(.xml) 로 퀘스트 정보 객체를 얻는다.
	QuestInfo* FindQuestInfo(std::wstring& szTalkFilename);
	/// 퀘스트인덱스 로 퀘스트 정보 객체를 얻는다.
	QuestInfo* FindQuestInfo(UINT nQuestID);
	/// 기간제 퀘스트 여부 확인
	bool IsPeriodQuest( const int nQuestID ) const;

	/// 스테이지 시작 할 때에 퀘스트 및 대화 정보 초기화
	void InitNpcTalkData( int nMapTableID );
	/// 스테이지 끝날 때에 퀘스트 및 대화 정보 삭제
	void ClearTalkData();
	/// 대화 정보 개별 로드
	bool LoadNpcTalkData( int nNpcIndex );
	/// 퀘스트 정보 개별 로드
	bool LoadNpcQuestData( int nNpcIndex );
	/// 퀘스트 정보 개별 로드
	bool LoadQuestData( int nQuestID );

private:
	/// 퀘스트 대사파일(.xml)을 로드한다. 
	bool _AddTalk(const WCHAR* _wszTalkFileName);
	/// 해당 대사파일을 얻는다.
	bool _GetTalk(const WCHAR* wszTalkFileName, OUT TALK_PARAGRAPH_MAP& TalkParagraphMap );
	/// 퀘스트 대사파일을 제거한다. 
	bool _RemoveTalk(const WCHAR* wszTalkFileName);
	/// 모든 저널데이터 제거
	void _RemoveAllJournalData();
	/// 모든 퀘스트조건 객체 제거
	void _RemoveAllQuestCondition();

	/// 저널데이터 파싱함수
	void _LoadJournalData(TiXmlElement* pRootElement, UINT nQuestID);
	/// 저널데이터 파싱함수
	void _ParseJournalItemlist(std::wstring& wszString, std::vector<JournalItem>& j_item_list);
	/// 저널데이터 파싱함수
	void _ParseDestMode(std::wstring& wszString, JournalPage&  page);
	
#if defined( PRE_ADD_MAINQUEST_UI )
	/// UI스타일 파싱 함수
	void _LoadQuestUIStyle(TiXmlElement* pRootElement, QuestInfo * pQuestInfo);
#endif // #if defined( PRE_ADD_MAINQUEST_UI )
	/// 조건데이터 파싱함수
	void _LoadQuestCondition(TiXmlElement* pRootElement, UINT nQuestID);
	/// 퀘스트 목록 로드
	bool _LoadQuestList();
	/// 기간제 퀘스트 정보 로드
	bool _LoadPeriodQuestList();
	/// 대사치환 정보 로드	
	bool _LoadTalkParamTable();
	/// 대사치환시 내부적으로 사용되는 함수
	std::wstring _GetString(int nType, std::vector<std::wstring>& tokens, IN StaticParamCallback* pCallback = NULL );
	/// 대사 치환시 내부적으로 사용되는 함수 해당 타입에 맞는 치환자를 검색.
	std::wstring _GetTalkParam(int nIndex);
	/// 대화 정보 로드
	bool _LoadTalkData();

	/// 저널데이터 map 
	JOURNAL_DATA_MAP		m_JournalMap;
	/// 퀘스트조건
	QUEST_CONDITION_MAP		m_QuestConditionMap;
	QUEST_INFO_MAP			m_QuestInfoMap;
	TALK_MAP				m_TalkMap;
	TALK_PARAM_MAP			m_TalkParamMap;
	TXMLFileMap				m_XMLFileMap; 
	TXMLIndexMap			m_XMLIndexMap; 
	QUEST_RECOMPENSE_MAP	m_QuestRecompenseMap;
	TMapActorData			m_pActorData;
	TMapNpcData				m_pNpcData;
	QUEST_INDEX_RECOMPENSE_MAP m_QuestIndexRecompenseMap;
	QUEST_LEVEL_CAP_RECOMPENSE_MAP m_QuestLevelCapRecompenseMap;
	PERIOD_QUEST_INFO_VEC	m_vPeriodQuestInfo;

	struct  __StaticTalkParamInfo
	{
		enum 
		{
			user_nick = 1,
			user_class,
			monster,
			npc,
			item,
			skill,
			map,
			action,
			sound,
			uistring,
			repair_cost,
			repair_cost_all,
			money,
			quest_level,//rlkt_new
		};
		int nType;
		std::wstring	szKey;
		__StaticTalkParamInfo(int type, const WCHAR* wszKey)
		{
			nType = type;
			szKey = wszKey;
		}
	};

	std::vector<__StaticTalkParamInfo>	m_StaticTalkParamList;


private:

#ifdef _DEBUG
	FILE * m_fp;
#endif


};

extern CDnDataManager g_DataManager;
