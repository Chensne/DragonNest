#pragma once


template<class EntityHandle>
class TDnFSMTransitCondition
{
protected:
	EntityHandle m_hEntity;

public:
	TDnFSMTransitCondition( EntityHandle hEntity ) : m_hEntity( hEntity ) {};
	virtual ~TDnFSMTransitCondition( void ) {};

	virtual bool IsSatisfy( void ) = 0;
};
