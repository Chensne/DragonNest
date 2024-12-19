#include "StdAfx.h"
#include "DnActionSpecificInfo.h"

CDnActionSpecificInfo g_ActionSpecifigInfo;

CDnActionSpecificInfo::CDnActionSpecificInfo(void) : m_pCurrentLoadSignalInfo( NULL )
{
}

CDnActionSpecificInfo::~CDnActionSpecificInfo(void)
{
	map<std::string, S_PROJECTILE_COUNT_INFO*>::iterator iter = m_mapProjectileSignalInfoByActionFile.begin();
	for( iter; iter != m_mapProjectileSignalInfoByActionFile.end(); ++iter )
		delete iter->second;

	map<std::string, S_SKILLCHAIN_INPUT_INFO*>::iterator iterSkillChain = m_mapSkillChainActionSetByActionFile.begin();
	for( iterSkillChain; iterSkillChain != m_mapSkillChainActionSetByActionFile.end(); ++iterSkillChain )
		delete iterSkillChain->second;

	map<std::string, S_INPUTHASPASSIVESKILL_INFO*>::iterator iterSkillPassive = m_mapPassiveSkillInfoByActionFile.begin();
	for( iterSkillPassive; iterSkillPassive != m_mapPassiveSkillInfoByActionFile.end(); ++iterSkillPassive )
		delete iterSkillPassive->second;

	map<std::string, S_BASIC_ATTACK_INFO*>::iterator iterBasic = m_mapBasicAttackInfoByActionFile.begin();
	for( iterBasic; iterBasic != m_mapBasicAttackInfoByActionFile.end(); ++iterBasic )
		delete iterBasic->second;
}

void 
CDnActionSpecificInfo::AddProjectileSignalInfo( const std::string& strFullPath, const S_PROJECTILE_COUNT_INFO& ProjectileCountInfo )
{
	S_PROJECTILE_COUNT_INFO* pNewInfo = new S_PROJECTILE_COUNT_INFO;
	*pNewInfo = ProjectileCountInfo;
	m_mapProjectileSignalInfoByActionFile.insert( make_pair( strFullPath, pNewInfo ) );
}

const CDnActionSpecificInfo::S_PROJECTILE_COUNT_INFO*
CDnActionSpecificInfo::FindProjectileSignalInfo( const std::string& strFullPath )
{
	map<std::string, S_PROJECTILE_COUNT_INFO*>::iterator iter = m_mapProjectileSignalInfoByActionFile.find( strFullPath );
	if( m_mapProjectileSignalInfoByActionFile.end() != iter )
	{
		return iter->second;
	}

	return NULL;
}

void 
CDnActionSpecificInfo::AddSkillChainActionSet( const std::string& strFullPath, const S_SKILLCHAIN_INPUT_INFO& SkillChainInputInfo )
{
	S_SKILLCHAIN_INPUT_INFO* pNewInfo = new S_SKILLCHAIN_INPUT_INFO;
	*pNewInfo = SkillChainInputInfo;
	m_mapSkillChainActionSetByActionFile.insert( make_pair(strFullPath, pNewInfo) );
}

const CDnActionSpecificInfo::S_SKILLCHAIN_INPUT_INFO*
CDnActionSpecificInfo::FindSkillChainActionSet( const std::string& strFullPath )
{
	map<std::string, S_SKILLCHAIN_INPUT_INFO*>::iterator iter = m_mapSkillChainActionSetByActionFile.find( strFullPath );
	if( m_mapSkillChainActionSetByActionFile.end() != iter )
	{
		return iter->second;
	}

	return NULL;
}

void
CDnActionSpecificInfo::AddPassiveSkillInfo( const std::string& strFullPath, const S_INPUTHASPASSIVESKILL_INFO& PassiveSkillInfo )
{
	S_INPUTHASPASSIVESKILL_INFO* pNewInfo = new S_INPUTHASPASSIVESKILL_INFO;
	*pNewInfo = PassiveSkillInfo;
	m_mapPassiveSkillInfoByActionFile.insert( make_pair(strFullPath, pNewInfo) );
}

const CDnActionSpecificInfo::S_INPUTHASPASSIVESKILL_INFO*
CDnActionSpecificInfo::FindPassiveSkillInfo( const std::string& strFullPath )
{
	map<std::string, S_INPUTHASPASSIVESKILL_INFO*>::iterator iter = m_mapPassiveSkillInfoByActionFile.find( strFullPath );
	if( m_mapPassiveSkillInfoByActionFile.end() != iter )
	{
		return iter->second;
	}

	return NULL;
}

void
CDnActionSpecificInfo::AddBasicAttackActionInfo( const std::string& strFullPath, const S_BASIC_ATTACK_INFO& BasicAttackInfo )
{
	S_BASIC_ATTACK_INFO* pNewInfo = new S_BASIC_ATTACK_INFO;
	*pNewInfo = BasicAttackInfo;
	m_mapBasicAttackInfoByActionFile.insert( make_pair(strFullPath, pNewInfo) );
}

const CDnActionSpecificInfo::S_BASIC_ATTACK_INFO*
CDnActionSpecificInfo::FindBasicAttackInfo( const std::string& strFullPath )
{
	map<std::string, S_BASIC_ATTACK_INFO*>::iterator iter = m_mapBasicAttackInfoByActionFile.find( strFullPath );
	if( m_mapBasicAttackInfoByActionFile.end() != iter )
	{
		return iter->second;
	}

	return NULL;
}