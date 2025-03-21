#pragma once

class CDNUserBase;
/**	퀘스트 조건체크 인터페이스 스트럭쳐
*/
class ConditionBase 
{
public:
	bool bUse;
	virtual void Init(std::wstring& data) { if( data.empty() ) bUse = false; }
	virtual bool Check(CDNUserBase * pUserBase) {	return false; }
	ConditionBase()
	{
		bUse = false;
	}
	virtual ~ConditionBase(){ }
};

/** 퀘스트 조건체크하는 객체 이다. 퀘스트 수락시 현재 레벨보다 높거나 이러한 경우들을 만족 시키지 못하면
퀘스트 수락을 하지 못한다. 이런 조건들을 체크하는 객체이다.
*/ 
class QuestCondition 
{
public:
	UINT nQuestIndex;
	std::vector<ConditionBase*>	ConditionList;
	bool Check( CDNUserBase * pUserBase );

	QuestCondition();
	virtual ~QuestCondition();
};

//////////////////////////////////////////////////////////////////////////
// __HaveNormalItem
//////////////////////////////////////////////////////////////////////////
class __HaveNormalItem : public ConditionBase
{
public:
	~__HaveNormalItem()	{}

	int nItemTableNumber;
	int nItemNumber;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

//////////////////////////////////////////////////////////////////////////
// __UserLevel
//////////////////////////////////////////////////////////////////////////
class __UserLevel : public ConditionBase 
{
public:
	~__UserLevel()	{}

	int nMinUserLevel;
	int nMaxUserLevel;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);

};

//////////////////////////////////////////////////////////////////////////
// __UserClass
//////////////////////////////////////////////////////////////////////////
class __UserClass : public ConditionBase 
{
public:
	~__UserClass()	{}

	std::vector<int> ClassIDList;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

//////////////////////////////////////////////////////////////////////////
// __PrevQuest
//////////////////////////////////////////////////////////////////////////
class __PrevQuest : public ConditionBase 
{
public:
	~__PrevQuest()	{}

	std::vector<UINT> PrevQuestList;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

//////////////////////////////////////////////////////////////////////////
// __CompleteMission
//////////////////////////////////////////////////////////////////////////
class __CompleteMission : public ConditionBase 
{
public:
	~__CompleteMission()	{}

	std::vector<int> MissionIDList;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

//////////////////////////////////////////////////////////////////////////
// __Reputation_Favor
//////////////////////////////////////////////////////////////////////////
class __Reputation_Favor : public ConditionBase // 좋아함 수치
{
public:
	~__Reputation_Favor()	{}

	int nNpcID;
	int nReputationValue_Min;
	int nReputationValue_Max;

	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

//////////////////////////////////////////////////////////////////////////
// __Reputation_Hatred
//////////////////////////////////////////////////////////////////////////
struct __Reputation_Hatred : public ConditionBase // 싫어함 수치
{
public:
	~__Reputation_Hatred()	{}

	int nNpcID;
	int nReputationValue_Min;
	int nReputationValue_Max;

	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};

#if defined(PRE_ADD_QUEST_CHECKCAHEITEM)
//////////////////////////////////////////////////////////////////////////
// __HaveCashlItem
//////////////////////////////////////////////////////////////////////////
class __HaveCashItem : public ConditionBase
{
public:
	~__HaveCashItem()	{}

	int nItemTableNumber;
	int nItemNumber;
	virtual void Init(std::wstring& data);
	virtual bool Check(CDNUserBase * pUserBase);
};
#endif