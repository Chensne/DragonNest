#pragma once

#include "DnSkill.h"


class CDnBuffUIMng
{
public:
	enum eBuffUIType
	{
		ePlayer,
		ePartyMember
	};

	struct SBuffUnit
	{
		int							skillID;
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
		int                         skillLevelID;
#endif
		int							blowIdx;
		CDnSkill::DurationTypeEnum	buffType;
		int							texturePageIdx;
		SUICoord					textureUV;
		float						duration;
		bool						bUpdated;
		bool						bOwner;		//스킬 실 사용자인지..??

		SBuffUnit()				{ Clear(); }
		bool IsEmpty() const	{ return (skillID == -1); }
		void Clear()			
		{ 
			skillID = -1; 
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
			skillLevelID = -1;
#endif
			blowIdx = -1;
			buffType = CDnSkill::Instantly;
			texturePageIdx = 0; 
			duration = 0.f; 
			bUpdated = false; 
			bOwner = false; 
		}
	};

	typedef std::list<SBuffUnit> BUFFLIST;

	struct SBubbleUnit
	{
		int			bubbleTypeID;
		int			texturePageIdx;
		SUICoord	textureUV;
		float		duration;
		float		remainTime;
		int			bubbleCount;
		bool		bUpdated;

		SBubbleUnit()			{ Clear(); }
		bool IsEmpty() const	{ return (bubbleTypeID == -1); }
		void Clear()			{ bubbleTypeID = -1; texturePageIdx = 0; duration = 0.f; bubbleCount = 0; remainTime = 0.f; bUpdated = false; }
	};

	typedef std::list<SBubbleUnit> BUBBLELIST;

	CDnBuffUIMng(int maxSlotCount, int maxBubbleSlotCount, eBuffUIType type);
	virtual ~CDnBuffUIMng() {}

	void			Process(DnActorHandle hActor);
	BUFFLIST&		GetBuffSlotList()				{ return m_BuffSlotList; }
	BUFFLIST&		GetBuffWaitList()				{ return m_WaitingBuffList; }
	BUFFLIST&       GetLowPriorityList()            { return m_LowPriorityList; }
	void			ClearLists();
	void			GetTextureUV(SUICoord& uvCoord, int iconImageIdx);
	BUBBLELIST&		GetBubbleSlotList()				{ return m_BubbleSlotList; }

private:
	bool		ProcessExistingBuffs(DnBlowHandle hBlow, int skillId, DnActorHandle hActor);
	void		ProcessBuffsTime(float fDelta);
	SBuffUnit*	GetBuffInWaitingList(int skillId);
	SBuffUnit*	GetBuffSlot(int skillId);
	void		GetDuration(float& resDuration, int& resBlowID, DnActorHandle hActor, DnBlowHandle hCurBlow);
	int			CheckEmptyBuffSlot() const;
	int			CheckEmptyBubbleSlot() const;
	SBubbleUnit* GetBubbleSlot(int bubbleTypeID);
	void		ClearSlot();

	int			m_BuffSlotCount;
	BUFFLIST	m_BuffSlotList;
	BUFFLIST	m_WaitingBuffList;
	BUFFLIST    m_LowPriorityList;
	int			m_BubbleSlotCount;
	BUBBLELIST	m_BubbleSlotList;
	eBuffUIType m_Type;
};
