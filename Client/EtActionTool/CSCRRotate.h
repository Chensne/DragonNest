#pragma once


#include "SignalCustomRender.h"

class CSCRRotate : public CSignalCustomRender
{
private:

	LOCAL_TIME m_LocalTime;

public:

	CSCRRotate( const char * strSignalName )
	  : CSignalCustomRender( strSignalName ){}
	virtual ~CSCRRotate(){}

	// Override - CSignalCustomRender
	virtual CSignalCustomRender *Clone();
	virtual void Initialize();
	virtual void Release();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver );

	virtual void OnStartSignal(); // �ñ׳� ���۽���.
	virtual void OnEndSignal();   // �ñ׳��������.
};