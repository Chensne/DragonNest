#pragma once

#include <d3d.h>
typedef D3DXVECTOR4 EtVector4;

class CXTCustomPropertyGridItemVector4Color : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemVector4Color(CString strCaption, EtVector4 Value, EtVector4* pBindVector = NULL);
	CXTCustomPropertyGridItemVector4Color(UINT nID, EtVector4 Value, EtVector4* pBindVector = NULL);

	virtual ~CXTCustomPropertyGridItemVector4Color(void);

public:
	void SetVector(EtVector4 Value);
	EtVector4 GetVector() { return m_vVector; }
	void SetNormalized( bool bFlag ) { m_bNormalized = bFlag; }

	void BindToVector(EtVector4 *pBindVector);

	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual CRect GetValueRect();
	virtual void OnInplaceButtonDown();
	virtual void OnBeforeInsert();

protected:
	EtVector4 m_vVector;
	EtVector4 *m_pBindVector;
	bool m_bNormalized;

private:
	class CXTCustomPropertyGridItemVectorCase;

	CXTCustomPropertyGridItemVectorCase *m_ItemVector[4];

	CString VectorToString(EtVector4 Value);
	EtVector4 StringToVector(CString strValue);

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	void UpdateChilds();

	void SetX( CString szStr );
	void SetY( CString szStr );
	void SetZ( CString szStr );
	void SetW( CString szStr );
	void SetVector( int nIndex, CString szStr );

	friend class CXTCustomPropertyGridItemVectorCase;
};
