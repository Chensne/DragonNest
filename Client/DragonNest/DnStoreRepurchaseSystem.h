#pragma once

class CDnStoreTabDlg;
class CDnItem;
class CDnStoreRepurchaseSystem
{
public:
#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST(CDnStoreRepurchaseSystem, SYSTEM_TEST);
#endif // #if !defined( _FINAL_BUILD )
	enum
	{
		eINVALID_PRICE = -1,
		eINVALID_REPURCHASE_ID = -1,
	};

	enum eEnableRepurchaseResult
	{
		eOK,
		eNO_NOT_ENOUGH_MONEY,
		eNO_NOT_ENOUGH_INVENSLOT,
	};

	struct SRepurchaseInfo
	{
		__time64_t tSellDate;
		int nSellPrice;
		CDnItem* pItem;
	};

	CDnStoreRepurchaseSystem();
	virtual ~CDnStoreRepurchaseSystem(void);
	bool Initialize();

	bool SetRepurchaseInfo(const int& nInfoCount, const TRepurchaseItemInfo* pInfoArray);
	void SetStoreDialog(CDnStoreTabDlg *pDialog) { m_pStoreDialog = pDialog; }

	int GetRepurchasePrice(const CDnItem& item) const;
	eEnableRepurchaseResult CheckEnableRepurchase(const CDnItem& item) const;
	bool RequestRepurchase(const CDnItem& item) const;
	void OnRepurchaseSuccess(int nRepurchaseID);

private:
	void DeleteStoreData();
	void DeleteStoreData(int nRepurchaseID);
	const CDnStoreRepurchaseSystem::SRepurchaseInfo* GetRepurchaseInfo(const CDnItem& item) const;
	const CDnStoreRepurchaseSystem::SRepurchaseInfo* GetRepurchaseInfo(int nRepurchaseID) const;
	int GetRepurchaseID(const CDnItem& item) const;

	bool IsSameItem(const CDnItem& source, const CDnItem& target) const;

	std::map<int, SRepurchaseInfo> m_mapRepurchaseInfo;
	CDnStoreTabDlg*	m_pStoreDialog;
};
