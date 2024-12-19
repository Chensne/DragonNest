#pragma once

#include "Connection.h"

class CDNSQLWorld;
class CDNSQLMembership;
class CDNMessageTask;
class CDNConnection: public CConnection
{
private:
	std::map<unsigned char, CDNMessageTask*> m_MessageTasks;

public:
	CDNConnection(void);
	virtual ~CDNConnection(void);

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);
	void DBMessageProcess(char *pData, int nThreadID);

	void QueryResultError(UINT nAccountDBID, int nRetCode, char cMainCmd, char cSubCmd);

	// 안에서 CashItemDBQuery 처리.. 한번 더 감쌌음
	int CashItemDBProcess(CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB, int nWorldSetID, int nAccountDBID, INT64 biCharacterDBID, int nMapID, int nChannelID, bool bPetalPurchase, TCashItemBase &BuyItem, int nPrice, 
		INT64 biPurchaseOrderID, int nAddMaterializedItemCode, const WCHAR* pwszIP, INT64 biSenderCharacterDBID = 0, bool bGift = false, char cPayMethodCode = DBDNWorldDef::PayMethodCode::Cash);

	long m_lDBQueueRemainSize;

	CDNMessageTask* GetMessageTask(unsigned char cMainCmd);

private:
	void RegisterMessageTask(eServerMainCmd cMainCmd, CDNMessageTask* pTask);
};
