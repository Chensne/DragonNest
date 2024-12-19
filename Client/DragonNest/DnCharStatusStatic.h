#pragma once
#include "EtUIStatic.h"

class CDnCharStatusStatic : public CEtUIStatic
{
public:
	enum emVALUE_TYPE
	{
		TYPE_01,	// %d(+%d)
		TYPE_02,	// %d/%d
		TYPE_03,	// %.2f%%
		TYPE_04,	// %d~%d(+%d~%d)
		TPYE_05,    // %d(+%d)(%d%%)
	};

public:
	CDnCharStatusStatic( CEtUIDialog *pParent );
	virtual ~CDnCharStatusStatic(void);

protected:

	bool m_bInitialized;

	int m_nValue1;
	int m_nValue2;
	int m_nValue3;
	int m_nValue4;
	int m_nValue5;
	int m_nValue6;

	float m_fValue1;
	float m_fValue2;
	float m_fValue3;

	float m_fBuffValue1;
	float m_fBuffValue2;

	SUICoord m_ValueCoord1;
	SUICoord m_ValueCoord2;

	std::wstring m_strValueString1;
	std::wstring m_strValueString2;
	std::wstring m_strDetailString;
	std::wstring m_strFinalString;

	bool m_bShowTooltip;

protected:
	void UpdateValueCoord();

public:
	// 두번째 인자에 툴팁 true로 되어있으면 괄호 안의 증가수치(m_wszValueString2)를 툴팁으로 표시한다.

	void SetValue( emVALUE_TYPE valueType, bool bShowTooltip, int nValue1, int nValue2 = 0, int nValue3 = 0, int nValue4 = 0, int nValue5 = 0, int nValue6 = 0 );
	void SetValue_Float( emVALUE_TYPE valueType, bool bShowTooltip, float fValue1, float fValue2 = 0.f, float fValue3 = 0.f );
	
	void AddDetailString( int nUIString, int nValue, bool bPercentage = true );
	void AddDetailString( int nUIString, float fValue, bool bPercentage = true );

	void AddBuffValue( float fBuffValue1 = 0.f , float fBuffValue2 = 0.f );
	void AddBuffValue( int nBuffValue1 = 0 , int nBuffValue2 = 0 );
	void ClearDetailString();
	void ClearValue();
	void ClearBuffValue();

public:
	virtual void Render( float fElapsedTime );
	virtual void OnChangeResolution();
};
