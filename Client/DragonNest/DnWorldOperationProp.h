#pragma once

#include "DnWorldActProp.h"
#include "DnActor.h"
#include "DnDropItem.h"

class CDnWorldOperationProp : public CDnWorldActProp
{
public:
	CDnWorldOperationProp();
	virtual ~CDnWorldOperationProp();

protected:
	bool m_bEnableOperator;
	int m_nCrosshairType;

public:
	bool IsEnableOperator() { return m_bEnableOperator; }
	virtual void CmdOperation();

	virtual bool InitializeTable( int nTableID );

	// MPTransAction
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	virtual int GetCrosshairType();

	void SetCrosshairType( int nCrosshairType );
};