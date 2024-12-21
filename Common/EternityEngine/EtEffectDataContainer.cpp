#include "StdAfx.h"
#include "EtEffectDataContainer.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtEffectColorDataValue::CEtEffectColorDataValue()
{
	m_Value = EtVector3( 0.f, 0.f, 0.f );
	m_nType = 0;
}

CEtEffectColorDataValue::CEtEffectColorDataValue( float fKey, EtVector3 vColor )
{
	m_fKey = fKey;
	m_Value = vColor;
	m_nType = 0;
}

CEtEffectAlphaDataValue::CEtEffectAlphaDataValue()
{
	m_Value = 1.0f;
	m_nType = 1;
}

CEtEffectAlphaDataValue::CEtEffectAlphaDataValue( float fKey, float fAlpha )
{
	m_fKey = fKey;
	m_Value = fAlpha;
	m_nType = 1;
}

CEtEffectScaleDataValue::CEtEffectScaleDataValue()
{
	m_Value = 1.0f;
	m_nType = 2;
}

CEtEffectScaleDataValue::CEtEffectScaleDataValue( float fKey, float fScale )
{
	m_fKey = fKey;
	m_Value = fScale;
	m_nType = 2;
}

CEtEffectPositionDataValue::CEtEffectPositionDataValue()
{
	m_Value = EtVector3( 0, 0, 0 );
	m_nType = 3;
}

CEtEffectPositionDataValue::CEtEffectPositionDataValue( float fKey, EtVector3 vPos )
{
	m_fKey = fKey;
	m_Value = vPos;
	m_nType = 3;
}

CEtEffectRotationDataValue::CEtEffectRotationDataValue()
{
	m_Value = EtVector3( 0, 0, 0 );
	m_nType = 4;
}

CEtEffectRotationDataValue::CEtEffectRotationDataValue( float fKey, EtVector3 vAxisAngle )
{
	m_fKey = fKey;
	m_Value = vAxisAngle;
	m_nType = 4;
}

CEtEffectDataContainer::CEtEffectDataContainer()
{
	ResetData();
}

CEtEffectDataContainer::~CEtEffectDataContainer()
{
	ResetData();
}

void CEtEffectDataContainer::ResetData()
{
	m_fMinimunKeyValue = 0.f;
	m_fMaximumKeyValue = 0.f;
	SAFE_DELETE_PVEC( m_pVecDataValue );
}

void CEtEffectDataContainer::AddData( IEtEffectDataValue *pValue, bool bSort )
{
	m_pVecDataValue.push_back( pValue );
	CalcMinMax();
	if( bSort ) SortDataValue();
}

void CEtEffectDataContainer::RemoveData( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= m_pVecDataValue.size() ) return;

	delete m_pVecDataValue[dwIndex];
	m_pVecDataValue.erase( m_pVecDataValue.begin() + dwIndex );

	CalcMinMax();
}

IEtEffectDataValue *CEtEffectDataContainer::GetValueFromIndex( DWORD dwIndex )
{
	if( dwIndex <0 || dwIndex >= m_pVecDataValue.size() ) return NULL;
	return m_pVecDataValue[dwIndex];
}

void CEtEffectDataContainer::Load( CStream *pStream )
{
	ResetData();
	DWORD dwCount;
	DWORD dwValueType;
	float fKey;
	pStream->Read( &dwCount, sizeof(DWORD) );

	for( DWORD i=0; i<dwCount; i++ ) {
		pStream->Read( &dwValueType, sizeof(DWORD) );

		IEtEffectDataValue *pValue = NULL;
		switch( dwValueType )
		{
		case 0: pValue = new CEtEffectColorDataValue; break;
		case 1: pValue = new CEtEffectAlphaDataValue; break;
		case 2:	pValue = new CEtEffectScaleDataValue; break;
		case 3:	pValue = new CEtEffectPositionDataValue; break;
		case 4:	pValue = new CEtEffectRotationDataValue; break;
		}
		if( pValue ) {
			pStream->Read( &fKey, sizeof(float) );
			pStream->Read( pValue->GetValue(), pValue->GetSize() );

			pValue->SetKey( fKey );
			m_pVecDataValue.push_back( pValue );
		}
	}
	CalcMinMax();
}

void CEtEffectDataContainer::Save( CStream *pStream )
{
	DWORD dwCount = GetDataCount();
	pStream->Write( &dwCount, sizeof(DWORD) );

	for( DWORD i=0; i<dwCount; i++ ) {
		IEtEffectDataValue *pValue = GetValueFromIndex(i);
		int nType = pValue->GetType();
		float fKey = pValue->GetKey();

		pStream->Write( &nType, sizeof(int) );
		pStream->Write( &fKey, sizeof(float) );
		pStream->Write( pValue->GetValue(), pValue->GetSize() );
	}
}

void CEtEffectDataContainer::CalcMinMax()
{
	m_fMinimunKeyValue = 0.f;
	m_fMaximumKeyValue = 0.f;
	for( DWORD i=0; i<m_pVecDataValue.size(); i++ )
	{
		float fKey = m_pVecDataValue[i]->GetKey();
		if( m_fMinimunKeyValue > fKey ) m_fMinimunKeyValue = fKey;
		else if( m_fMaximumKeyValue < fKey ) m_fMaximumKeyValue = fKey;
	}
}

bool CompareEffectValue( IEtEffectDataValue *pObj1, IEtEffectDataValue *pObj2 )
{
	if( pObj1->GetKey() < pObj2->GetKey() ) return true;
	return false;
}

void CEtEffectDataContainer::SortDataValue()
{
	std::sort( m_pVecDataValue.begin(), m_pVecDataValue.end(), CompareEffectValue );
}

float *CEtEffectDataContainer::GetInterpolationFloat( float fInterpolationKey, bool *bSuccess, bool bCurve )
{
	float *pAlpha = (float *)GetInterpolationValue( fInterpolationKey, bSuccess, bCurve );
	return pAlpha;
}

EtVector3 *CEtEffectDataContainer::GetInterpolationVector3( float fInterpolationKey, bool *bSuccess, bool bCurve )
{
	EtVector3 *pVector = (EtVector3 *)GetInterpolationValue( fInterpolationKey, bSuccess, bCurve );
	return pVector;
}

EtQuat *CEtEffectDataContainer::GetInterpolationQuat( float fInterpolationKey, bool *bSuccess, bool bCurve )
{
	EtQuat *pQuat = (EtQuat *)GetInterpolationValue( fInterpolationKey, bSuccess, bCurve );
	return pQuat;
}

void *CEtEffectDataContainer::GetInterpolationValue( float fInterpolationKey, bool *bSuccess, bool bCurve )
{
	if( bSuccess )
		*bSuccess = false;

	DWORD dwCount = (DWORD)m_pVecDataValue.size();
	if( dwCount == 0 ) return NULL;
	if( dwCount == 1 ) return m_pVecDataValue[0]->GetValue();	
	if( fInterpolationKey < m_fMinimunKeyValue ) return m_pVecDataValue[0]->GetValue();	
	if( fInterpolationKey > m_fMaximumKeyValue ) return m_pVecDataValue[dwCount-1]->GetValue();

	static EtVector3 s_vColor;
	static float s_fAlphaScale;
	static EtQuat s_qRotation;

	for( DWORD i=0; i<dwCount-1; i++ )
	{
		float fKey = m_pVecDataValue[i]->GetKey();
		float fKeyNext = m_pVecDataValue[i+1]->GetKey();

		if( fInterpolationKey >= fKey && fInterpolationKey <= fKeyNext )
		{
			float fWeight = ( fInterpolationKey - fKey ) / ( fKeyNext - fKey );
			switch( m_pVecDataValue[i]->GetType() )
			{
			case 0:	// Color
				{
					EtVector3 *pValue = (EtVector3 *)m_pVecDataValue[i]->GetValue();
					EtVector3 *pValueNext = (EtVector3 *)m_pVecDataValue[i+1]->GetValue();
					if( bSuccess ) *bSuccess = true;
					return EtVec3Lerp( &s_vColor, pValue, pValueNext, fWeight );
				}
				break;
			case 1:	// Alpha
			case 2:	// Scale
				{
					float *pValue = (float *)m_pVecDataValue[i]->GetValue();
					float *pValueNext = (float *)m_pVecDataValue[i+1]->GetValue();
					s_fAlphaScale = *pValue + ( ( *pValueNext - *pValue ) * fWeight );
					if( bSuccess ) *bSuccess = true;
					return &s_fAlphaScale;
				}
				break;
			case 3:	// Pos
				{
					if( !bCurve )
					{
						EtVector3 *pValue = (EtVector3 *)m_pVecDataValue[i]->GetValue();
						EtVector3 *pValueNext = (EtVector3 *)m_pVecDataValue[i+1]->GetValue();
						if( bSuccess ) *bSuccess = true;
						return EtVec3Lerp( &s_vColor, pValue, pValueNext, fWeight );
					}
					else
					{
						EtVector3 *pValuePrev = ( i > 0 ) ? (EtVector3 *)m_pVecDataValue[i-1]->GetValue() : (EtVector3 *)m_pVecDataValue[i]->GetValue();
						EtVector3 *pValue = (EtVector3 *)m_pVecDataValue[i]->GetValue();
						EtVector3 *pValueNext = (EtVector3 *)m_pVecDataValue[i+1]->GetValue();
						EtVector3 *pValueNext2 = ( i < dwCount-2 ) ? (EtVector3 *)m_pVecDataValue[i+2]->GetValue() : (EtVector3 *)m_pVecDataValue[i+1]->GetValue();
						if( bSuccess ) *bSuccess = true;
						return EtVec3CatmullRom( &s_vColor, pValuePrev, pValue, pValueNext, pValueNext2, fWeight );
					}
				}
				break;
			case 4:	// Rot
				{
					EtVector3 vTemp;
					EtVector3 *pValue = (EtVector3 *)m_pVecDataValue[i]->GetValue();
					EtVector3 *pValueNext = (EtVector3 *)m_pVecDataValue[i+1]->GetValue();
					EtVec3Lerp( &vTemp, pValue, pValueNext, fWeight );
					vTemp.x = vTemp.x / 180.f * D3DX_PI;
					vTemp.y = vTemp.y / 180.f * D3DX_PI;
					vTemp.z = vTemp.z / 180.f * D3DX_PI;
					if( bSuccess ) *bSuccess = true;
					return EtQuaternionRotationYawPitchRoll( &s_qRotation, vTemp.x, vTemp.y, vTemp.z );
				}
				break;
			}
		}
	}
	return NULL;
}

void CEtEffectDataContainer::operator = ( CEtEffectDataContainer &e )
{
	ResetData();
	for( DWORD i=0; i<e.GetDataCount(); i++ )
	{
		IEtEffectDataValue *pSorValue = e.GetValueFromIndex(i);
		ASSERT( NULL != pSorValue );
		IEtEffectDataValue *pValue = NULL;
		switch( pSorValue->GetType() )
		{
		case 0: pValue = new CEtEffectColorDataValue; break;
		case 1: pValue = new CEtEffectAlphaDataValue; break;
		case 2:	pValue = new CEtEffectScaleDataValue; break;
		case 3:	pValue = new CEtEffectPositionDataValue; break;
		case 4:	pValue = new CEtEffectRotationDataValue; break;
		}
		ASSERT( NULL != pValue );
		pValue->SetKey( pSorValue->GetKey() );
		pValue->SetValue( pSorValue->GetValue() );
		m_pVecDataValue.push_back( pValue );
	}
	CalcMinMax();
}