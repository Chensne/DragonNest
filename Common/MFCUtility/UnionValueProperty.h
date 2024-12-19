#pragma once

#include <d3d.h>
typedef D3DXVECTOR2 EtVector2;
typedef D3DXVECTOR3 EtVector3;
typedef D3DXVECTOR4 EtVector4;

// XTream ToolKit Property 값을 저장하기 위한 Unoin class 객체
class CUnionValueProperty
{
public:
	enum Type 
	{
		Unknown = -1,
		// 기본형
		Boolean = 7,
		Char = 2,
		Integer = 0,
		Float = 1,
		String = 3,
		Pointer = 6,
		Vector2 = 11,
		Vector3 = 4,
		Vector4 = 12,
		Color = 5,
		// 기본확장
		Integer_Range = 8,
		Float_Range = 14,
		Integer_Combo = 9,
		String_FileOpen = 10,
		Vector4Color = 13,
	};
	CUnionValueProperty( Type Type );
	virtual ~CUnionValueProperty();

	union 
	{
		bool m_bValue;
		char m_cVlaue;
		int	m_nValue;
		float m_fValue;
		char *m_szValue;
		void *m_pPtr;

		EtVector2* m_pVec2Value;
		EtVector3* m_pVec3Value;
		EtVector4* m_pVec4Value;
		int	m_nRangeValue[3];
		float m_fRangeValue[3];
	};

protected:
	Type m_Type;
	bool m_bDefaultEnable;
	char *m_szCategory;
	char *m_szDescription;
	char *m_pSubDescription;
	bool m_bAllocSubDesc;
	CString	m_szBindStr;
	CXTPPropertyGridItem *m_pXTPropItem;
	bool m_bCustomDialog;

	void Create( Type Type );
	void Destroy();
public:
	void ChangeType( Type Type );
	Type GetType() { return m_Type; }

	void SetDescription( const char *szDescription );
	char *GetDescription() { return m_szDescription; }

	void SetSubDescription( char *szDescription, bool bAlloc = false );
	char *GetSubDescription() { return m_pSubDescription; }

	void SetCategory( const char *szStr );
	const char *GetCategory() const { return m_szCategory; }

	bool IsDefaultEnable() { return m_bDefaultEnable; }
	void SetDefaultEnable( bool bEnable ) { m_bDefaultEnable = bEnable;	}

	void SetXTPropItem( CXTPPropertyGridItem *pItem ) { m_pXTPropItem = pItem; }
	CXTPPropertyGridItem *GetXTPropItem() { return m_pXTPropItem; }

	void SetCustomDialog( bool bEnable ) { m_bCustomDialog = bEnable; }
	bool IsCustomDialog() { return m_bCustomDialog; }

	// Type 객체들 get/set
	void SetVariable( bool bValue ) { m_bValue = bValue; }
	void SetVariable( char cValue ) { m_cVlaue = cValue; }
	void SetVariable( int nValue ) { m_nValue = nValue; }
	void SetVariable( float fValue ) { m_fValue = fValue; }
	void SetVariable( char *szValue );
	void SetVariable( void *pPtr ) { m_pPtr = pPtr; }
	void SetVariable( EtVector2 vVec ) { *m_pVec2Value = vVec; }
	void SetVariable( EtVector3 vVec ) { *m_pVec3Value = vVec; }
	void SetVariable( EtVector4 vVec ) { *m_pVec4Value = vVec; }
	void SetVariable( int nValue, int nMin, int nMax );
	void SetVariable( float fValue, float fMin, float fMax );

	bool GetVariableBool() { return m_bValue; }
	char GetVariableChar() { return m_cVlaue; }
	int	GetVariableInt() const { return m_nValue; }
	float GetVariableFloat() { return m_fValue; }
	char *GetVariableString() { return m_szValue; }
	void *GetVariablePtr() { return m_pPtr; }
	EtVector2 GetVariableVector2() { return *m_pVec2Value; }
	EtVector2 *GetVariableVector2Ptr()	{ return m_pVec2Value; }
	EtVector3 GetVariableVector3() { return *m_pVec3Value; }
	EtVector3 *GetVariableVector3Ptr()	{ return m_pVec3Value; }
	EtVector4 GetVariableVector4() { return *m_pVec4Value; }
	EtVector4 *GetVariableVector4Ptr()	{ return m_pVec4Value; }
	int *GetVariableIntRange() { return m_nRangeValue; }
	float *GetVariableFloatRange() { return m_fRangeValue; }

	CString *GetBindStr();
	const CString& GetBindStr() const;

	CUnionValueProperty&operator = ( CUnionValueProperty&e );

};
