#pragma once

class IEtEffectDataValue
{
public:
	IEtEffectDataValue() { m_nType = -1; m_fKey = 0.0f; }
	virtual ~IEtEffectDataValue() {}

protected:
	int m_nType;
	float m_fKey;

public:
	void SetType( int nType ) { m_nType = nType; }
	int GetType() { return m_nType; }
	void SetKey( float fKey ) { m_fKey = fKey; }
	float GetKey() { return m_fKey; }

	virtual void SetValue( void *pValue ) = 0;
	virtual void *GetValue() = 0;
	virtual int GetSize() = 0;
};

template < class T >
class TEtEffectDataValue : public IEtEffectDataValue
{
protected:
	T m_Value;

public:
	virtual void SetValue( void *pValue ) { memcpy( &m_Value, pValue, sizeof(T) ); }
	virtual void *GetValue() { return &m_Value; }
	virtual int GetSize() { return sizeof(T); }
};

class CEtEffectColorDataValue : public TEtEffectDataValue<EtVector3>
{
public:
	CEtEffectColorDataValue();
	CEtEffectColorDataValue( float fKey, EtVector3 vColor );
	virtual ~CEtEffectColorDataValue() {}

	void SetColor( float fR, float fG, float fB ) { m_Value = EtVector3( fR, fG, fB ); }
	void SetColor( EtVector3 &vColor ) { m_Value = vColor; }
	float GetRed() { return m_Value.x; }
	float GetGreen() { return m_Value.y; }
	float GetBlue() { return m_Value.z; }
};

class CEtEffectAlphaDataValue : public TEtEffectDataValue<float>
{
public:
	CEtEffectAlphaDataValue();
	CEtEffectAlphaDataValue( float fKey, float fAlpha );
	virtual ~CEtEffectAlphaDataValue() {}

	void SetAlpha( float fAlpha ) { m_Value = fAlpha; }
	float GetAlpha() { return m_Value; }
};

class CEtEffectScaleDataValue : public TEtEffectDataValue<float>
{
public:
	CEtEffectScaleDataValue();
	CEtEffectScaleDataValue( float fKey, float fScale );
	virtual ~CEtEffectScaleDataValue() {}

	void SetScale( float fScale ) { m_Value = fScale; }
	float GetScale() { return m_Value; }
};

class CEtEffectPositionDataValue : public TEtEffectDataValue<EtVector3>
{
public:
	CEtEffectPositionDataValue();
	CEtEffectPositionDataValue( float fKey, EtVector3 vPos );
	virtual ~CEtEffectPositionDataValue() {}

	void SetPosition( EtVector3 &vPos ) { m_Value = vPos; }
	EtVector3 *GetPosition() { return &m_Value; }
};

class CEtEffectRotationDataValue : public TEtEffectDataValue<EtVector3>
{
public:
	CEtEffectRotationDataValue();
	CEtEffectRotationDataValue( float fKey, EtVector3 vAxisAngle );
	virtual ~CEtEffectRotationDataValue() {}

	void SetRotation( EtVector3 vAxisAngle ) { m_Value = vAxisAngle; }
	EtVector3 *GetRotation() { return &m_Value; }
};

class CStream;
class CEtEffectDataContainer
{
public:
	CEtEffectDataContainer();
	virtual ~CEtEffectDataContainer();

	void ResetData();
	void AddData( IEtEffectDataValue *pValue, bool bSort = false );
	void RemoveData( DWORD dwIndex );
	IEtEffectDataValue *GetValueFromIndex( DWORD dwIndex );
	DWORD GetDataCount() { return (DWORD)m_pVecDataValue.size(); }
	float GetMaximumKeyValue() { return m_fMaximumKeyValue; }

	void Load( CStream *pStream );
	void Save( CStream *pStream );

	float *GetInterpolationFloat( float fInterpolationKey, bool *bSuccess = NULL, bool bCurve = false );
	EtVector3 *GetInterpolationVector3( float fInterpolationKey, bool *bSuccess = NULL, bool bCurve = false );
	EtQuat *GetInterpolationQuat( float fInterpolationKey, bool *bSuccess = NULL, bool bCurve = false );

	// Operator
	void operator = ( CEtEffectDataContainer &e );

protected:
	std::vector<IEtEffectDataValue *> m_pVecDataValue;
	float m_fMinimunKeyValue;
	float m_fMaximumKeyValue;

	void SortDataValue();
	void CalcMinMax();
	void *GetInterpolationValue( float fInterpolationKey, bool *bSuccess = NULL, bool bCurve = false );
};