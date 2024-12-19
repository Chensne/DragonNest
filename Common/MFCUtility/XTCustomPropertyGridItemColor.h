#pragma once

// Color Picker ����� Property Class

class CXTCustomPropertyGridItemColor : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemColor( const CString& strCaption, COLORREF clr = 0, COLORREF* pBindColor = NULL );
	CXTCustomPropertyGridItemColor( UINT nID, COLORREF clr = 0, COLORREF* pBindColor = NULL );

	virtual ~CXTCustomPropertyGridItemColor();

public:
	virtual void SetColor(COLORREF clr);
	virtual COLORREF GetColor() const;

	static COLORREF AFX_CDECL StringToRGB(LPCTSTR str);
	static CString AFX_CDECL RGBToString(COLORREF clr);

	virtual void BindToColor(COLORREF* pBindColor);

	BOOL PASCAL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual CRect GetValueRect();

	virtual void SetValue(CString strValue);
	virtual void OnInplaceButtonDown();
	virtual void OnBeforeInsert();

protected:
	COLORREF m_clrValue;
	COLORREF* m_pBindColor;

private:

};

AFX_INLINE COLORREF CXTCustomPropertyGridItemColor::GetColor() const {
	return m_clrValue;
}
