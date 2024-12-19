#pragma once

// MiniSlider 를 Property 에서 사용하기 위함.
// 크라이텍 센드박스 스타일.ㅋㅋ

#define UM_MINISLIDER_UPDATE_POS WM_USER + 4000
#define UM_MINISLIDER_SET_POS WM_USER + 4001
#define UM_MINISLIDER_UPDATE_END_POS WM_USER + 4002
#define UM_MINISLIDER_SET_END_POS WM_USER + 4003

class CXTCustomPropertyGridItemNumberSlider;
class CXTCustomPropertyGridItemDoubleSlider;
class CXTCustomPropertyGridItemFloatSlider;
class CXTCustomPropertyGridItemVector2DRange;
class CXTCustomPropertyGridItemFloat;

class CMiniSliderPropertyGrid : public CWnd
{
	DECLARE_DYNAMIC(CMiniSliderPropertyGrid)

public:
	CMiniSliderPropertyGrid();
	virtual ~CMiniSliderPropertyGrid();

	void SetPos( float fValue );
	float GetPos() { return m_fPos; }

	void SetRangeMode( bool bValue ) { m_bRange = bValue; }
	void SetEndPos( float fValue );
	float GetEndPos() { return m_fEndPos; }

	void SetClass( CXTCustomPropertyGridItemNumberSlider *pClass ) {
		m_pNumClass = pClass;
		m_pDoubleClass = NULL;
		m_pVectorClass = NULL;
		m_pFloatClass = NULL;
	};
	void SetClass( CXTCustomPropertyGridItemDoubleSlider *pClass ) {
		m_pDoubleClass = pClass;
		m_pNumClass = NULL;
		m_pVectorClass = NULL;
		m_pFloatClass = NULL;
	}
	void SetClass( CXTCustomPropertyGridItemVector2DRange *pClass ) {
		m_pVectorClass = pClass;
		m_pDoubleClass = NULL;
		m_pNumClass = NULL;
		m_pFloatClass = NULL;
	}
	void SetClass( CXTCustomPropertyGridItemFloatSlider *pClass ) {
		m_pFloatClass = pClass;
		m_pNumClass = NULL;
		m_pVectorClass = NULL;
		m_pDoubleClass = NULL;
	}

protected:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPos( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetEndPos( WPARAM wParam, LPARAM lParam );

	bool m_bRange;
	float m_fPos;
	float m_fEndPos;
	bool m_bLButtonDown;

	CXTCustomPropertyGridItemNumberSlider *m_pNumClass;
	CXTCustomPropertyGridItemDoubleSlider *m_pDoubleClass;
	CXTCustomPropertyGridItemVector2DRange * m_pVectorClass;
	CXTCustomPropertyGridItemFloatSlider *m_pFloatClass;

};



class CXTCustomPropertyGridItemNumberSlider : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemNumberSlider(const CString& strCaption, long nValue = 0, long* pBindNumber = NULL, CSize Range = CSize( -1, -1 ), long nUnit = 1 );
	CXTCustomPropertyGridItemNumberSlider(UINT nID, long nValue = 0, long* pBindNumber = NULL, CSize Range = CSize( -1, -1 ), long nUnit = 1 );  // <COMBINE CXTCustomPropertyGridItemNumberSlider::CXTCustomPropertyGridItemNumberSlider@const CString&@long@long*>

	virtual ~CXTCustomPropertyGridItemNumberSlider();

public:
	virtual void SetNumber(long nValue);
	virtual long GetNumber() const;
	virtual void BindToNumber(long* pBindNumber);
	virtual void OnChangeSlider();

	void SetRangeMin( int nValue ) { m_Range.cx = nValue; }
	void SetRangeMax( int nValue ) { m_Range.cy = nValue; }
	void SetRange( int nMin, int nMax ) { m_Range = CSize( nMin, nMax ); }

protected:
	virtual void SetValue(CString strValue);
	virtual void OnBeforeInsert();
	virtual void OnSelect();
	virtual void OnDeselect();
	virtual CRect GetValueRect();

protected:
	CMiniSliderPropertyGrid m_Slider;

	CSize m_Range;
	long m_nUnit;

	long m_nValue;
	long* m_pBindNumber;
private:

	DECLARE_DYNAMIC(CXTCustomPropertyGridItemNumberSlider)
};


class CXTCustomPropertyGridItemDoubleSlider : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemDoubleSlider(const CString& strCaption, double fValue = 0, LPCTSTR strFormat = NULL, double* pBindNumber = NULL, double fRangeMin = -10000.f, double fRangeMax = 10000.f, double fUnit = 0.0001f );
	CXTCustomPropertyGridItemDoubleSlider(UINT nID, double fValue = 0, LPCTSTR strFormat = NULL, double* pBindNumber = NULL, double fRangeMin = -10000.f, double fRangeMax = 10000.f, double fUnit = 0.0001f ); 

	virtual ~CXTCustomPropertyGridItemDoubleSlider();

public:
	virtual void SetDouble(double fValue);
	virtual double GetDouble() const;
	virtual void BindToDouble(double* pBindDouble);
	virtual void OnChangeSlider();
	void UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol);

	void SetRangeMin( double fValue ) { m_fRangeMin = fValue; }
	void SetRangeMax( double fValue ) { m_fRangeMax = fValue; }
	void SetRange( double fMin, double fMax ) { m_fRangeMin = fMin; m_fRangeMax = fMax; }


protected:
	virtual void SetValue(CString strValue);
	virtual void OnBeforeInsert();
	virtual void OnSelect();
	virtual void OnDeselect();
	virtual CRect GetValueRect();


protected:

	double StringToDouble(LPCTSTR strValue);

	CString DoubleToString(double dValue);

protected:
	CMiniSliderPropertyGrid m_Slider;

	double m_fRangeMin, m_fRangeMax;
	double m_fUnit;

	double m_fValue;
	double* m_pBindDouble;
	BOOL m_bUseSystemDecimalSymbol;

private:
	DECLARE_DYNAMIC(CXTCustomPropertyGridItemDoubleSlider)

};

class CXTCustomPropertyGridItemFloatSlider : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemFloatSlider(const CString& strCaption, float fValue = 0, LPCTSTR strFormat = NULL, float* pBindNumber = NULL, float fRangeMin = -10000.f, float fRangeMax = 10000.f, float fUnit = 0.0001f );
	CXTCustomPropertyGridItemFloatSlider(UINT nID, float fValue = 0, LPCTSTR strFormat = NULL, float* pBindNumber = NULL, float fRangeMin = -10000.f, float fRangeMax = 10000.f, float fUnit = 0.0001f ); 

	virtual ~CXTCustomPropertyGridItemFloatSlider();

public:
	virtual void SetFloat(float fValue);
	virtual float GetFloat() const;
	virtual void BindToFloat(float* pBindFloat);
	virtual void OnChangeSlider();
	void UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol);

	void SetRangeMin( float fValue ) { m_fRangeMin = fValue; }
	void SetRangeMax( float fValue ) { m_fRangeMax = fValue; }
	void SetRange( float fMin, float fMax ) { m_fRangeMin = fMin; m_fRangeMax = fMax; }


protected:
	virtual void SetValue(CString strValue);
	virtual void OnBeforeInsert();
	virtual void OnSelect();
	virtual void OnDeselect();
	virtual CRect GetValueRect();


protected:

	float StringToFloat(LPCTSTR strValue);

	CString FloatToString(float dValue);

protected:
	CMiniSliderPropertyGrid m_Slider;

	float m_fRangeMin, m_fRangeMax;
	float m_fUnit;

	float m_fValue;
	float* m_pBindFloat;
	BOOL m_bUseSystemDecimalSymbol;

private:
	DECLARE_DYNAMIC(CXTCustomPropertyGridItemFloatSlider)

};

class CXTCustomPropertyGridItemVector2DRange : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemVector2DRange(CString strCaption, D3DXVECTOR2 Value, D3DXVECTOR2* pBindVector = NULL, float fRangeMin = -1000.f, float fRangeMax = 1000.f, float fUnit = 0.01f );
	CXTCustomPropertyGridItemVector2DRange(UINT nID, D3DXVECTOR2 Value, D3DXVECTOR2* pBindVector = NULL, float fRangeMin = -1000.f, float fRangeMax = 1000.f, float fUnit = 0.01f );

	virtual ~CXTCustomPropertyGridItemVector2DRange(void);

protected:

	BOOL m_bAutoNormalize;
	D3DXVECTOR2 m_Value;
	D3DXVECTOR2 *m_pBindVector;
	CMiniSliderPropertyGrid m_Slider;

	float m_fRangeMin, m_fRangeMax;
	float m_fUnit;
	bool m_bInteger;

public:

	void SetVector(D3DXVECTOR2 Value);

	D3DXVECTOR2 GetVector() { return m_Value; }

	void BindToVector(D3DXVECTOR2 *pBindVector);

	void SetAutoNormalize( BOOL bFlag ) { m_bAutoNormalize = bFlag; }


	virtual void OnSelect();
	virtual void OnDeselect();
	virtual CRect GetValueRect();
	virtual void OnChangeSlider();
private:
	class CXTCustomPropertyGridItemVectorCase;

	CXTCustomPropertyGridItemVectorCase *m_itemX;
	CXTCustomPropertyGridItemVectorCase *m_itemY;

	CString VectorToString(D3DXVECTOR2 Value);
	D3DXVECTOR2 StringToVector(CString strValue);

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	void UpdateChilds();

	void SetX( CString szStr );
	void SetY( CString szStr );

	friend class CXTCustomPropertyGridItemVectorCase;
};

AFX_INLINE long CXTCustomPropertyGridItemNumberSlider::GetNumber() const {
	return m_nValue;
}

AFX_INLINE double CXTCustomPropertyGridItemDoubleSlider::GetDouble() const {
	return m_fValue;
}
AFX_INLINE void CXTCustomPropertyGridItemDoubleSlider::UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol) {
	m_bUseSystemDecimalSymbol = bUseSystemDecimalSymbol;
	m_strValue = DoubleToString(m_fValue);
}

AFX_INLINE float CXTCustomPropertyGridItemFloatSlider::GetFloat() const {
	return m_fValue;
}
AFX_INLINE void CXTCustomPropertyGridItemFloatSlider::UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol) {
	m_bUseSystemDecimalSymbol = bUseSystemDecimalSymbol;
	m_strValue = FloatToString(m_fValue);
}

// XTPPropertyGridItemNumber 가지고 Float로 타입만 추가합니다. 위에 MiniSlider 를 위해 어쩔수없음.
class CXTPPropertyGridItemFloat : public CXTPPropertyGridItem
{
public:
	CXTPPropertyGridItemFloat(const CString& strCaption, float fValue = 0, LPCTSTR strFormat = NULL, float* pBindNumber = NULL);
	CXTPPropertyGridItemFloat(UINT nID, float fValue = 0, LPCTSTR strFormat = NULL, float* pBindNumber = NULL);

	virtual ~CXTPPropertyGridItemFloat();

public:

	virtual void SetFloat(float fValue);

	virtual float GetFloat() const;
	virtual void BindToFloat(float* pBindFloat);

	void UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol);

protected:

	virtual void SetValue(CString strValue);

	virtual void OnBeforeInsert();


protected:

	float StringToFloat(LPCTSTR strValue);

	CString FloatToString(float dValue);
	virtual void OnInplaceButtonDown();

protected:

	float m_fValue;
	float* m_pBindFloat;
	BOOL m_bUseSystemDecimalSymbol;

private:
	DECLARE_DYNAMIC(CXTPPropertyGridItemFloat)

};

AFX_INLINE float CXTPPropertyGridItemFloat::GetFloat() const {
	return m_fValue;
}
AFX_INLINE void CXTPPropertyGridItemFloat::UseSystemDecimalSymbol(BOOL bUseSystemDecimalSymbol) {
	m_bUseSystemDecimalSymbol = bUseSystemDecimalSymbol;
	m_strValue = FloatToString(m_fValue);
}
