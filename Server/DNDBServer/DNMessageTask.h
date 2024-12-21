#pragma once

class CDNConnection;
class CDNMessageTask
{
public:
	CDNMessageTask(CDNConnection* pConnection);
	virtual ~CDNMessageTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) {}

protected:
	CDNConnection* m_pConnection;
};

class CDNSPErrorCheckManager
{
public:

	void Add( UINT nAccountDBID )
	{
		if( nAccountDBID > 0)
		{
			ScopeLock<CSyncLock> Lock(m_Lock);
			m_sErrorAcountID.insert(nAccountDBID);
		}		
	}

	bool bIsError( UINT nAccountDBID )
	{
		ScopeLock<CSyncLock> Lock(m_Lock);
		if( m_sErrorAcountID.empty())
			return false;
		return m_sErrorAcountID.find( nAccountDBID ) != m_sErrorAcountID.end();
	}

	void Del( UINT nAccountDBID )
	{
		ScopeLock<CSyncLock> Lock(m_Lock);
		if( m_sErrorAcountID.empty() )
			return;
		m_sErrorAcountID.erase(nAccountDBID);		
	}

private:

	std::set<UINT> m_sErrorAcountID;
	CSyncLock m_Lock;
};

extern CDNSPErrorCheckManager* g_pSPErrorCheckManager;
