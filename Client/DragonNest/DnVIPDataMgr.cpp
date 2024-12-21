#include "StdAfx.h"

#ifdef PRE_ADD_VIP

#include "DnVIPDataMgr.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnVIPDataMgr::CDnVIPDataMgr()
{
	m_bVIPAutoPay	= false;
	m_nVIPPts		= 0;
	m_tVIPExpireDate = 0;
#if defined _CH || defined _TW
	m_bVIPMode		= true;
#else
	m_bVIPMode		= false;
#endif
	m_bVIP			= false;
}

void CDnVIPDataMgr::MakeServiceString(std::wstring& result) const
{
	std::wstring temp;
	int i = 0;
	for (; i < eMAX; ++i)
	{
		GetBonusString(temp, (eBonusType)i);
		if (temp.empty() == false)
		{
			result += temp;
			result += L"\n";
		}
	}
}

void CDnVIPDataMgr::GetBonusString(std::wstring& result, eBonusType type) const
{
	//	note by kalliste :	현재는 vip 서비스 하나만 존재하는 것으로 결정되어 간단히 처리되어 있지만, vip 서비스가 여러개로 늘어날 경우 
	//						읽어두고 서비스(table) id 로 판별하여 셋팅하도록 수정 필요.
	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TVIP );
	DNTableFileFormat*  pCoinSox = GetDNTable(CDnTableDB::TREBIRTHCOIN);

	result.clear();

	if (pSox && pCoinSox)
	{
		//	make bonus_exp string
		if (type == eEXP)
		{
			int exp = CPlayerLevelTable::GetInstance().GetValue( 1, 1, CPlayerLevelTable::VIPExp );
			if (exp > 0)
				result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020008), exp);
			return;
		}

		//	make bonus_fatigue string
		if (type == eFATIGUE)
		{
			int fatigue = CPlayerLevelTable::GetInstance().GetValue( 1, 1, CPlayerLevelTable::VIPFatigue );
			if (fatigue > 0)
				result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020009), fatigue);
			return;
		}

		if (type == eREBIRTHCOIN)
		{
			int coin = pCoinSox->GetFieldFromLablePtr( 1, "_VIPCoin")->GetInteger();
			if (coin > 0)
				result = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020010), coin);
			return;
		}

		int tableID = 1;
		if (type == eMONTHITEM)
		{
			result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(tableID, "_MonthItemID")->GetInteger());
			return;
		}

		if (type == eCHRNAME_COLOR)
		{
			result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(tableID, "_NamecolorID")->GetInteger());
			return;
		}

		if (type == eEMBLEM)
		{
			result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(tableID, "_EmblemID")->GetInteger());
			return;
		}

		if (type == eDOUBLE_TREASURE)
		{
			result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(tableID, "_TreasureChestString")->GetInteger());
			return;
		}
		if (type == eVIPFARM)
		{
			result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(tableID, "_VIPFARM")->GetInteger());
			return;
		}
	}
}

bool CDnVIPDataMgr::IsVIP() const
{
	if (IsVIPMode() == false)
		return false;

	/*
	__time64_t cur;
	_time64(&cur);

	return (m_tVIPExpireDate >= cur);
	*/
	return m_bVIP;
}

bool CDnVIPDataMgr::IsVIPAutoPay() const
{
	return m_bVIPAutoPay;
}

const __time64_t* CDnVIPDataMgr::GetVIPExpireDate() const
{
	if (IsVIP() == false)
		return NULL;

	return &m_tVIPExpireDate;
}

bool CDnVIPDataMgr::IsVIPNearExpireDate() const
{
	if (IsVIPAutoPay())
		return false;

	const __time64_t* pExpireDate = GetVIPExpireDate();
	if (pExpireDate)
	{
		__time64_t cur, gap;
		_time64(&cur);
		gap = *pExpireDate - cur;
		return (gap > 0 && gap <= eEXPIRE_LIMIT * 86400);
	}

	return false;
}

void CDnVIPDataMgr::SetData(const __time64_t expireDate, bool bAutopay, int pts, bool bVIP)
{
	m_bVIP				= bVIP;
	m_tVIPExpireDate	= expireDate;
	m_bVIPAutoPay		= bAutopay;
	m_nVIPPts			= pts;
}

#endif // PRE_ADD_VIP