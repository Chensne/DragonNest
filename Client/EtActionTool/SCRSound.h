#pragma once

#include "SignalCustomRender.h"
#include "EtSoundChannel.h"
class CSCRSound : public CSignalCustomRender 
{
public:
	CSCRSound( const char *szSignalName );
	virtual ~CSCRSound();

protected:
	std::vector<EtVector3> m_vVecList[2];
	EtSoundChannelHandle m_hChannel;
//	CEtSoundChannel *m_pChannel;
	int m_nSoundIndex;
	CString m_szPrevFileName;

public:
	virtual CSignalCustomRender *Clone() { return new CSCRSound( m_szSignalName.c_str() ); }

	virtual void Initialize();
	virtual void OnSelect( bool bFirst = false );
	virtual void OnUnSelect();
	virtual void OnModify();
	virtual void OnProcess( LOCAL_TIME LocalTime, float fDelta );
};

