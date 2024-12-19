#pragma once

#include "DnWorldOperationProp.h"
#include "EtUIDialog.h"

class CDnLocalPlayerActor;

class CDnWorldKeepOperationProp : public CDnWorldOperationProp, public CEtUICallback
{
public :
	CDnWorldKeepOperationProp();
	~CDnWorldKeepOperationProp();

	void OnSetKeepOperationProp( const int nActorHandle, const int nKeepClick, const int nStringID, const int nActionIndex );
	void OnCancelKeepOperationProp();

	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl * pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

protected:
	enum{
		KEEP_REQUEST_DIALOG = 0
	};

	bool m_bProgress;
	bool m_bChangeAction;

	void SendKeepCancle();
};