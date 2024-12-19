#pragma once

#include <d3d.h>
typedef D3DXVECTOR2 EtVector2;
typedef D3DXVECTOR3 EtVector3;
typedef D3DXVECTOR4 EtVector4;

// XTPPropertyGridItemColor 기본형 Vector2,3,4 3가지

class CXTCustomPropertyGridItemVector2 : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemVector2(CString strCaption, EtVector2 Value, EtVector2* pBindVector = NULL);
	CXTCustomPropertyGridItemVector2(UINT nID, EtVector2 Value, EtVector2* pBindVector = NULL);

	virtual ~CXTCustomPropertyGridItemVector2();

public:
	void SetVector(EtVector2 vVec);
	EtVector2 GetVector() { return m_vVector; }

	void BindToVector(EtVector2 *pBindVector);

	void SetNormalized( bool bValue ) { m_bNormalized = bValue; }

protected:
	EtVector2 m_vVector;
	EtVector2 *m_pBindVector;
	bool m_bNormalized;

protected:
	class CXTCustomPropertyGridItemVector2Case;

	CXTCustomPropertyGridItemVector2Case *m_ItemVector[2];

	CString VectorToString(EtVector2 vVec);
	EtVector2 StringToVector(CString szValue);

	void SetX( CString szStr );
	void SetY( CString szStr );
	void SetVector( int nIndex, CString szStr );

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	void UpdateChilds();

	friend class CXTCustomPropertyGridItemVector2Case;
};


class CXTCustomPropertyGridItemVector3 : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemVector3(CString strCaption, EtVector3 Value, EtVector3* pBindVector = NULL);
	CXTCustomPropertyGridItemVector3(UINT nID, EtVector3 Value, EtVector3* pBindVector = NULL);

	virtual ~CXTCustomPropertyGridItemVector3();

public:
	void SetVector(EtVector3 vVec);
	EtVector3 GetVector() { return m_vVector; }

	void BindToVector(EtVector3 *pBindVector);

	void SetNormalized( bool bValue ) { m_bNormalized = bValue; }

protected:
	EtVector3 m_vVector;
	EtVector3 *m_pBindVector;
	bool m_bNormalized;

protected:
	class CXTCustomPropertyGridItemVector3Case;

	CXTCustomPropertyGridItemVector3Case *m_ItemVector[3];

	CString VectorToString(EtVector3 vVec);
	EtVector3 StringToVector(CString szValue);

	void SetX( CString szStr );
	void SetY( CString szStr );
	void SetZ( CString szStr );
	void SetVector( int nIndex, CString szStr );

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	void UpdateChilds();

	friend class CXTCustomPropertyGridItemVector3Case;
};


class CXTCustomPropertyGridItemVector4 : public CXTPPropertyGridItem
{
public:

	CXTCustomPropertyGridItemVector4(CString strCaption, EtVector4 Value, EtVector4* pBindVector = NULL);
	CXTCustomPropertyGridItemVector4(UINT nID, EtVector4 Value, EtVector4* pBindVector = NULL);

	virtual ~CXTCustomPropertyGridItemVector4();

public:
	void SetVector(EtVector4 vVec);
	EtVector4 GetVector() { return m_vVector; }

	void BindToVector(EtVector4 *pBindVector);

	void SetNormalized( bool bValue ) { m_bNormalized = bValue; }

protected:
	EtVector4 m_vVector;
	EtVector4 *m_pBindVector;
	bool m_bNormalized;

protected:
	class CXTCustomPropertyGridItemVector4Case;

	CXTCustomPropertyGridItemVector4Case *m_ItemVector[4];

	CString VectorToString(EtVector4 vVec);
	EtVector4 StringToVector(CString szValue);

	void SetX( CString szStr );
	void SetY( CString szStr );
	void SetZ( CString szStr );
	void SetW( CString szStr );
	void SetVector( int nIndex, CString szStr );

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	void UpdateChilds();

	friend class CXTCustomPropertyGridItemVector4Case;
};
