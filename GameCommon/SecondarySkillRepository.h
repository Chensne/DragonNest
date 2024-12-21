
#pragma once

#if defined( PRE_ADD_SECONDARY_SKILL )

class CSecondarySkill;
class CManufactureSkill;
class ISecondarySkillEventHandler;
class CSecondarySkillRecipe;

class CSecondarySkillRepository
{
public:
	CSecondarySkillRepository();
	virtual ~CSecondarySkillRepository();

	int					GetCount( SecondarySkill::Type::eType Type );
	CSecondarySkill*	Get( const int iSecondarySkillID );
	CSecondarySkill*	Get( SecondarySkill::SubType::eType Type );
	CManufactureSkill*	GetManufactureSkill( const int iSecondarySkillID );
	bool				Create( const int iSecondarySkillID );											// ������ų ���� �� �߰�
	bool				Delete( const int iSecondarySkillID );											// ��ų ����
	bool				Add( const int iSecondarySkillID, const int iExp );								// ���� ������ų �߰�
	bool				AddExp( const int iSecondarySkillID, const int iExp );							// ����ġ ����
	bool				AddExp( SecondarySkill::SubType::eType Type, const int iExp );							// ����ġ ����
	bool				AddRecipeExp( const int iSecondarySkillID, const int iItemID, const int iExp );	// ������ ����ġ ����
	std::map<int,CSecondarySkill*>& GetSecondarySkillMap() { return m_mSecondarySkill; }

	virtual void		SendList(){}
	virtual int			CanCreate( CSecondarySkill* pCreateSkill ){ return ERROR_NONE; }
	virtual int			CanAddRecipe( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe ){ return ERROR_NONE; }
	virtual int			CanDeleteRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe ){ return ERROR_NONE; }
	virtual int			CanExtractRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe ){ return ERROR_NONE; }
	virtual int			CanManufacture( CSecondarySkill* pSkill, bool bIsStart, int iItemID, CSecondarySkillRecipe*& pRecipe ){ return ERROR_NONE; }
	virtual int			CanCancelManufacture( CManufactureSkill*& pManufacturingSkill ){ return ERROR_NONE; }

protected:
	ISecondarySkillEventHandler*	m_pEventHandler;
	std::map<int,CSecondarySkill*>	m_mSecondarySkill;
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
