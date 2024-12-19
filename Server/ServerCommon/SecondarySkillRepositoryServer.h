
#pragma once

#include "SecondarySkillRepository.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

class CDNUserSession;

class CSecondarySkillRepositoryServer:public CSecondarySkillRepository,public TBoostMemoryPool<CSecondarySkillRepositoryServer>
{
public:

	CSecondarySkillRepositoryServer( CDNUserSession* pSession );

	int					AddRecipe( SecondarySkill::CSAddRecipe* pPacket );			// 클라이언트에 받은 정보로 레시피 추가
	bool				AddRecipe( TSecondarySkillRecipe* pRecipeData );			// DB에서 받은 정보로 레시피 추가
	bool				DeleteRecipe( SecondarySkill::CSDeleteRecipe* pPacket );	// 클라이언트에서 받은 정보로 레시피 제거
	bool				ExtractRecipe( SecondarySkill::CSExtractRecipe* pPacket );	// 클라이언트에서 받은 정보로 레시피 추출
	int					DoManufacture( SecondarySkill::CSManufacture* pPacket );	// 클라이언트에서 받은 정보로 제작
	bool				CancelManufacture( bool bSend=true);						// 제작 취소
	CSecondarySkill*	GetManufacturingSkill();									// 현재 제작중인 스킬 포인터 얻기
	
	void				SetManufacturingFlag( bool bFlag ){ m_bManufacturing = bFlag; }
	bool				bIsManufacturing(){ return m_bManufacturing; }

	virtual void		SendList();
	
	virtual int			CanCreate( CSecondarySkill* pCreateSkill );
	virtual int			CanAddRecipe( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe );
	virtual int			CanDeleteRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe );
	virtual int			CanExtractRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe );
	virtual int			CanManufacture( CSecondarySkill* pSkill, bool bIsStart, int iItemID, CSecondarySkillRecipe*& pRecipe );
	virtual int			CanCancelManufacture( CManufactureSkill*& pManufacturingSkill );

protected:

	void _SendRecipeList();

	CDNUserSession* m_pSession;
	bool			m_bManufacturing;
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
