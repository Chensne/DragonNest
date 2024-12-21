#pragma once
#define REBIRTH_MAX_DROP_ITEM 10

#include "DnCustomDlg.h"

class CDnRebirthSystem : public CSingleton<CDnRebirthSystem>
{
private:
	enum BonusType{
		STR
		//to be added
	};

	struct BonusItemDrop{
		int itemID[REBIRTH_MAX_DROP_ITEM];
		int itemCount[REBIRTH_MAX_DROP_ITEM];
		int itemLevel[REBIRTH_MAX_DROP_ITEM];
	};

	struct RebirthData{
		int id;
		int level;
		bool hasBonusDrop;
		BonusItemDrop BonusDrop;
	};

	std::vector<RebirthData> *m_pData;

	int GetMinRebirthLevel(int CurRebirth, int Level);

public:
	CDnRebirthSystem();
	~CDnRebirthSystem();
	void OpenRebirthWindow();
	void OnPressOK();
public:
	virtual void Initialize(bool bShow);
};

