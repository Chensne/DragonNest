#pragma once

class MAActorProp
{
public:
#ifndef _GAMESERVER
//	static DWORD s_dwActorPropUniqueID;
//	static vector<DnActorHandle> s_vlhActorProp;
#else
//	STATIC_DECL( DWORD s_dwActorPropUniqueID );
#endif

protected:
	DnActorHandle m_hMonster;

public:
	MAActorProp( void );
	virtual ~MAActorProp( void );

#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
	bool InitializeMonsterActorProp( int nMonsterTableID , BOOL bUseMonsterWeightTable = FALSE );
#else
	bool InitializeMonsterActorProp( int nMonsterTableID );
#endif
	bool InitializeNpcActorProp( int nNpcTableId );
	bool CopyActorStateFromThis( DnActorHandle hActor );

	DnActorHandle GetActorHandle() { return m_hMonster; }
};

