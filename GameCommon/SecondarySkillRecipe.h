
#pragma once

#if defined( PRE_ADD_SECONDARY_SKILL )

class ISecondarySkillEventHandler;

class CSecondarySkillRecipe:public TBoostMemoryPool<CSecondarySkillRecipe>
{
public:

	CSecondarySkillRecipe( int iSkillID, int iItemID, INT64 biItemSerial, USHORT nExp );

	int GetSkillID(){ return m_iSkillID; }
	int	GetItemID(){ return m_iItemID; }
	INT64 GetItemSerial(){ return m_biItemSerial; }
	short GetExp(){ return m_nExp; }
	short GetMaxExp(){ return m_nMaxExp; }

	SecondarySkill::Grade::eType GetRequiredGrade(){ return m_RequiredGrade; }
	int	GetRequiredSkillLevel(){ return m_iRequiredSkillLevel; }

	void AddExp( int iExp, ISecondarySkillEventHandler* pEventHandler );
	bool IsValid();
#if defined( _CLIENT )
	void SetExp( int nExp );
#endif // _CLIENT

protected:

	int	m_iSkillID;
	int	m_iItemID;
	INT64 m_biItemSerial;
	short m_nExp;
	short m_nMaxExp;

	SecondarySkill::Grade::eType m_RequiredGrade;
	int	m_iRequiredSkillLevel;
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
