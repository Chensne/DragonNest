
#pragma once

//
class NoFilter
{
public:
	bool operator()( CEtWorldProp* pWorldProp ) const
	{
		return true;
	}
};

// 
class FilterActorHitSignal
{
public:
	FilterActorHitSignal()
	{
		m_List.push_back( PTE_Broken );
		m_List.push_back( PTE_BrokenDamage );
		m_List.push_back( PTE_HitMoveDamageBroken );
		m_List.push_back( PTE_BuffBroken );
	}
	bool operator()( CEtWorldProp* pWorldProp ) const
	{
		CDnWorldProp* pProp = static_cast<CDnWorldProp*>(pWorldProp);
		return std::find( m_List.begin(), m_List.end(), pProp->GetPropType() ) != m_List.end();
	}

private:
	std::list<PropTypeEnum> m_List;
};

// 
class FilterProjectileHitSignal
{
public:
	bool operator()( CEtWorldProp* pWorldProp ) const
	{
		return true;
	}
};
