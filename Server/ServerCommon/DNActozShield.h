#pragma once

#if defined(_KRAZ)

class CDNActozShield
{
private:
	std::string m_strReserved;
	DWORD m_dwReconnectTick;

public:
	CDNActozShield(void);
	~CDNActozShield(void);

	bool Init();
	void CheckerResult();

	void RepeatCheckerResult(DWORD dwCurTick);
};

extern CDNActozShield *g_pActozShield;

#endif	// #if defined(_KRAZ)