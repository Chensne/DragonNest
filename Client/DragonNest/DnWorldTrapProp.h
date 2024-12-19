#pragma once

#include "DnWorldActProp.h"
#include "DnActor.h"
#include "DnDropItem.h"
#include "DnDamageBase.h"
#include "MAActorProp.h"


class CDnWorldTrapProp : public CDnWorldActProp, 
						 public CDnDamageBase,
						 public MAActorProp
{
private:
	// 트랩 발동 조건 관련.
	enum
	{
		LOOP,					// 정해진 액션을 계속 실행
		BOUNDING_BOX_CHECK,		// 바운딩 박스 체크해서 Activate 됨
	};

	int			m_iActivateType;
	bool		m_bIdle;

public:
	CDnWorldTrapProp();
	virtual ~CDnWorldTrapProp();

protected:
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject( void );

public:
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	DnActorHandle GetMonsterHandle() { return m_hMonster; }

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };

	// from CDnActionBase
	virtual void SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );
	// 2010.4.22 서버에서 매번 액션 변경될 때마다 CmdAction 패킷을 보내주므로 제거됨.
	//virtual void OnFinishAction( const char* szPrevAction, const char *szNextAction, LOCAL_TIME time );
};