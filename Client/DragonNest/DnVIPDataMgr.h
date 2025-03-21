#pragma once

#ifdef PRE_ADD_VIP

class CDnVIPDataMgr
{
public:
	enum eBonusType
	{
		eEXP,
		eFATIGUE,
		eREBIRTHCOIN,
		eMONTHITEM,
		eCHRNAME_COLOR,
		eEMBLEM,
		eDOUBLE_TREASURE,
		eVIPFARM,
		eMAX
	};

	enum eConstant
	{
		eERROR			= -1,
		eEXPIRE_LIMIT	= 7,
	};

	CDnVIPDataMgr();
	virtual ~CDnVIPDataMgr() {}

	void	MakeServiceString(std::wstring& result) const;
	void	SetData(const __time64_t expireDate, bool bAutopay, int pts, bool bVIP);
	void	SetVIPMode(bool bSet)	{ m_bVIPMode = bSet; }

	bool	IsVIPMode() const		{ return m_bVIPMode; }
	bool	IsVIP() const;
	bool	IsVIPAutoPay() const;
	bool	IsVIPNearExpireDate() const;

	void					GetBonusString(std::wstring& result, eBonusType type) const;
	const __time64_t*		GetVIPExpireDate() const;
	int						GetCurrentPts() const { return m_nVIPPts; }

private:
	bool		m_bVIPMode;
	__time64_t	m_tVIPExpireDate;
	int			m_nVIPPts;
	bool		m_bVIPAutoPay;
	bool		m_bVIP;
};

#endif // PRE_ADD_VIP