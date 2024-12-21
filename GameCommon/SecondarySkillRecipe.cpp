
#include "Stdafx.h"
#include "SecondarySkillRecipe.h"
#include "SecondarySkillEventHandler.h"
#if defined( _SERVER )
#include "DNGameDataManager.h"
#else // _SERVER
#include "DnTableDB.h"
#include "DNTableFile.h"
#endif // _SERVER

#if defined( PRE_ADD_SECONDARY_SKILL )

CSecondarySkillRecipe::CSecondarySkillRecipe( int iSkillID, int iItemID, INT64 biItemSerial, USHORT nExp )
:m_iSkillID(iSkillID),m_iItemID(iItemID),m_biItemSerial(biItemSerial),m_nExp(nExp),m_nMaxExp(0),m_RequiredGrade(SecondarySkill::Grade::Max),m_iRequiredSkillLevel(INT_MAX)
{
#if defined( _SERVER )
	TSecondarySkillRecipeTableData* pTable = g_pDataManager->GetSecondarySkillRecipeTableData( m_iItemID );
	_ASSERT( pTable );
	if( pTable )
	{
		if( m_nExp == 0 )
			m_nExp = pTable->iStartExp;
		m_nMaxExp				= pTable->iMasterExp;
		m_RequiredGrade			= pTable->RequireGrade;
		m_iRequiredSkillLevel	= pTable->iRequireSkillLevel;
	}
#else
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
		return;
	}
	if( m_nExp == 0 )
		m_nExp = pSox->GetFieldFromLablePtr( iItemID, "_StartExp" )->GetInteger();
	m_nMaxExp = pSox->GetFieldFromLablePtr( iItemID, "_MasterExp" )->GetInteger();
	m_RequiredGrade = (SecondarySkill::Grade::eType)pSox->GetFieldFromLablePtr( iItemID, "_SecondarySkillClass" )->GetInteger();
	m_iRequiredSkillLevel = pSox->GetFieldFromLablePtr( iItemID, "_SecondarySkillLevel" )->GetInteger();
#endif // #if defined( _SERVER )
	if( m_nExp > m_nMaxExp )
		m_nExp = m_nMaxExp;
}

void CSecondarySkillRecipe::AddExp( int iExp, ISecondarySkillEventHandler* pEventHandler )
{
	if( GetExp() >= GetMaxExp()  )
		return;
	
	if( GetExp()+iExp > GetMaxExp() )
		iExp = GetMaxExp()-GetExp();

	m_nExp += iExp;

	if( pEventHandler )
		pEventHandler->OnChangeRecipeExp( this, iExp );
}

#if defined( _CLIENT )
void CSecondarySkillRecipe::SetExp( int nExp )
{
	if( GetExp() >= GetMaxExp()  )
		return;

	if( nExp > GetMaxExp() )
		nExp = GetMaxExp();

	m_nExp = nExp;
}
#endif // _CLIENT


bool CSecondarySkillRecipe::IsValid()
{
	if( GetMaxExp() <= 0 )
		return false;

	return true;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
