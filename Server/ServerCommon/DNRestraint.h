	
#pragma once

class CDNUserBase;

class CDNRestraint:public TBoostMemoryPool<CDNRestraint>
{
public:
	CDNRestraint(CDNUserBase* pSession);
	~CDNRestraint();

	void Initialize();

#if defined( PRE_ADD_DWC )
	bool LoadRestraint(TRestraintData * pData, char cAccountLevel);
#else // #if defined( PRE_ADD_DWC )
	bool LoadRestraint(TRestraintData * pData);
#endif // #if defined( PRE_ADD_DWC )
	bool DelRestraint(UINT nRestraintKey);	
	
	bool CheckRestraint(int nType, bool bSend = true);
	void SendRestraintList();

protected:
	TRestraintData m_RestraintData;

	TRestraint * m_pRestraintChat;
	TRestraint * m_pRestraintTrade;
#if defined( PRE_ADD_DWC )
	char m_cAccountLevel;
#endif // #if defined( PRE_ADD_DWC )

private:
	CDNUserBase* m_pSession;
};