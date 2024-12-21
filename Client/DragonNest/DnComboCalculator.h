#pragma once


// 연타 입력 감지하는 기능을 하는 객체
class CDnComboCalculator
{
public:
	enum
	{
		LINEAR_CHECK,
		CIRCULAR_CHECK,
	};

private:
	int m_iCheckMethod;
	vector<BYTE> m_vlKeysToCheck;
	vector<BYTE> m_vlPadsToCheck;
	
	int m_iLastKeyIndex;
	int m_iLastPadIndex;
	float m_fPadRatio;
	float m_fPadAccum;

private:
	CDnComboCalculator( void ) {};

public:
	CDnComboCalculator( int iCheckMethod );
	~CDnComboCalculator(void);

	void SetPadsToCheck( std::vector<BYTE>& vlPadsToCheck, float DurationTime );
	void SetKeysToCheck( std::vector<BYTE>& vlKeysToCheck )		{ m_vlKeysToCheck = vlKeysToCheck; }

	// 설정해둔 조건이 맞으면 true를 리턴. 콤보된 카운트도 리턴해준다.
	bool Process( LOCAL_TIME LocalTime, float fDelta, /*OUT*/ int* piComboCount );
};
