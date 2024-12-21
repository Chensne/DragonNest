
#include "stdafx.h"
#include "SecondarySkillRepository.h"
#include "SecondarySkill.h"
#include "ManufactureSkill.h"
#include "SecondarySkillFactory.h"
#include "SecondarySkillEventHandler.h"
#include "SecondarySkillRecipe.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

CSecondarySkillRepository::CSecondarySkillRepository() : m_pEventHandler( NULL )
{
}

CSecondarySkillRepository::~CSecondarySkillRepository()
{
	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
		delete (*itor).second;

	SAFE_DELETE( m_pEventHandler );
}

int	CSecondarySkillRepository::GetCount( SecondarySkill::Type::eType Type )
{
	int iCount = 0;

	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
	{
		if( (*itor).second->GetType() == Type )
			++iCount;
	}

	return iCount;
}

CSecondarySkill* CSecondarySkillRepository::Get( const int iSecondarySkillID )
{
	std::map<int,CSecondarySkill*>::iterator itor = m_mSecondarySkill.find( iSecondarySkillID );
	if( itor != m_mSecondarySkill.end() )
		return (*itor).second;

	return NULL;
}

CSecondarySkill* CSecondarySkillRepository::Get( SecondarySkill::SubType::eType Type )
{
	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
	{
		if( (*itor).second->GetSubType() == Type )
			return (*itor).second;
	}

	return NULL;
}

CManufactureSkill* CSecondarySkillRepository::GetManufactureSkill( const int iSecondarySkillID )
{
	CSecondarySkill* pSkill = Get( iSecondarySkillID );
	if( pSkill == NULL )
		return NULL;

	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return NULL;

	return static_cast<CManufactureSkill*>(pSkill);
}

// 보조스킬 생성 후 추가
bool CSecondarySkillRepository::Create( const int iSecondarySkillID )
{
	// 스킬생성
	CSecondarySkill* pCreateSkill = CSecondarySkillFactory::CreateSkill( iSecondarySkillID );
	if( pCreateSkill == NULL )
		return false;

	if( CanCreate( pCreateSkill ) != ERROR_NONE )
	{
		delete pCreateSkill;
		return false;
	}

	m_mSecondarySkill.insert( std::make_pair(iSecondarySkillID,pCreateSkill) );
#if defined( _SERVER )
	bool bRet = m_pEventHandler->OnCreate( pCreateSkill );
	_ASSERT( bRet == true );
#endif // _SERVER

	return true;
}

bool CSecondarySkillRepository::Delete( const int iSecondarySkillID )
{
	CSecondarySkill* pSkill = Get( iSecondarySkillID );
	if( pSkill == NULL )
		return false;

	// 제작 스킬 조건 검사
	if( pSkill->GetType() == SecondarySkill::Type::ManufactureSkill )
	{
		CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

		// 보유 레시피 검사
		if( pManufactureSkill->GetRecipeCount() > 0 )
			return false;
	}

	m_mSecondarySkill.erase( pSkill->GetSkillID() );
#if defined( _SERVER )
	bool bRet = m_pEventHandler->OnDelete( pSkill );
	_ASSERT( bRet == true );
#endif // _SERVER
	SAFE_DELETE( pSkill );

	return true;
}

// 기존 보조스킬 추가
bool CSecondarySkillRepository::Add( const int iSecondarySkillID, const int iExp )
{
	// 스킬생성
	CSecondarySkill* pAddSkill = CSecondarySkillFactory::CreateSkill( iSecondarySkillID );
	if( pAddSkill == NULL )
		return false;

	// 중복검사
	if( Get( iSecondarySkillID ) )
	{
		delete pAddSkill;
		return false;
	}

	// Exp 설정
	pAddSkill->SetExp( iExp, m_pEventHandler );

	m_mSecondarySkill.insert( std::make_pair(iSecondarySkillID,pAddSkill) );
	return true;
}

// 보조스킬 경험치 증가
bool CSecondarySkillRepository::AddExp( const int iSecondarySkillID, const int iExp )
{
	CSecondarySkill* pSkill = Get( iSecondarySkillID );
	if( pSkill == NULL )
		return false;

	pSkill->AddExp( iExp, m_pEventHandler );
	return true;
}

bool CSecondarySkillRepository::AddExp( SecondarySkill::SubType::eType Type, const int iExp )
{
	CSecondarySkill* pSkill = Get( Type );
	if( pSkill == NULL )
		return false;
	pSkill->AddExp( iExp, m_pEventHandler );
	return true;
}

// 레시피 경험치 증가
bool CSecondarySkillRepository::AddRecipeExp( const int iSecondarySkillID, const int iItemID, const int iExp )
{
	CSecondarySkill* pSkill = Get( iSecondarySkillID );
	if( pSkill == NULL )
		return false;

	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return false;

	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

	CSecondarySkillRecipe* pRecipe = pManufactureSkill->GetRecipe( iItemID );
	if( pRecipe == NULL )
		return false;

	pRecipe->AddExp( iExp, m_pEventHandler );
	return true;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
