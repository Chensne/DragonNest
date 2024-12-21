#include "StdAfx.h"
#include "Observable.h"

#include "ICommandListener.h"


CObservable::CObservable(void)
{
}

CObservable::~CObservable(void)
{
}


void CObservable::AddCommandListener( ICommandListener* pEventListener )
{
	m_vlpListeners.push_back( pEventListener );
}


bool CObservable::DelCommandListener( ICommandListener* pEventListener )
{
	bool bResult = false;

	vector<ICommandListener*>::iterator iter = find( m_vlpListeners.begin(), m_vlpListeners.end(), pEventListener );
	
	if( m_vlpListeners.end() != iter )
	{
		m_vlpListeners.erase( iter );
		bResult = true;
	}

	return bResult;
}


bool CObservable::NotifyToAllListeners( ICommand* pCommand )
{
	bool bResult = false;

	if( pCommand )
	{
		vector<ICommandListener*>::iterator iter = m_vlpListeners.begin();
		for( iter; iter != m_vlpListeners.end(); ++iter )
		{
			(*iter)->CommandPerformed( pCommand );
		}

		bResult = true;
	}

	return bResult;
}