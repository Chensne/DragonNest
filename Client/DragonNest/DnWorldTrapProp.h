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
	// Ʈ�� �ߵ� ���� ����.
	enum
	{
		LOOP,					// ������ �׼��� ��� ����
		BOUNDING_BOX_CHECK,		// �ٿ�� �ڽ� üũ�ؼ� Activate ��
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
	// 2010.4.22 �������� �Ź� �׼� ����� ������ CmdAction ��Ŷ�� �����ֹǷ� ���ŵ�.
	//virtual void OnFinishAction( const char* szPrevAction, const char *szNextAction, LOCAL_TIME time );
};