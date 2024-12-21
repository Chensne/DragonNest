#pragma once

#if 0

class CDnMessageList
{
	friend class CCompareMessage;

protected:
	struct SMessageInfo
	{
		int m_nMainCmd;
		int m_nSubCmd;

		SMessageInfo( int nMainCmd, int nSubCmd )
			: m_nMainCmd(nMainCmd)
			, m_nSubCmd(nSubCmd)
		{
		}
	};

public:
	CDnMessageList(void);
	virtual ~CDnMessageList(void);

protected:
	std::list<SMessageInfo> m_listMessage;

public:
	bool FindMessage( int nMainCmd, int nSubCmd );
	bool AddMessage( int nMainCmd, int nSubCmd );
	bool RemoveMessage( int nMainCmd, int nSubCmd );
	bool IsEmpty() { return m_listMessage.empty(); }
};

class CCompareMessage
{
private:
	CDnMessageList::SMessageInfo *m_pMessageInfo;

public:
	CCompareMessage( CDnMessageList::SMessageInfo *pMessageInfo )
		: m_pMessageInfo(pMessageInfo)
	{
	}

	bool operator() ( const CDnMessageList::SMessageInfo &messageInfo ) const
	{
		if( (m_pMessageInfo->m_nMainCmd == messageInfo.m_nMainCmd) && (m_pMessageInfo->m_nSubCmd == messageInfo.m_nSubCmd) )
		{
			return true;
		}

		return false;
	}
};

#endif