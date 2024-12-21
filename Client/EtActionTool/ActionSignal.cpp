#include "StdAfx.h"
#include "ActionSignal.h"
#include "SignalManager.h"
#include "SignalItem.h"
#include "GlobalValue.h"
#include "SignalCustomRender.h"
#include "ActionElement.h"

CActionSignal::CActionSignal()
{
	SetType( SIGNAL );
	m_nSignalIndex = -1;
	m_nStartFrame = -1;
	m_nEndFrame = -1;
	m_nYOrder = -1;

	m_pCustomRender = NULL;
}

CActionSignal::~CActionSignal()
{
	CGlobalValue::GetInstance().SetSignalCustomRenderObject( NULL );
	SAFE_DELETE( m_pCustomRender );
}


void CActionSignal::Process( LOCAL_TIME LocalTime )
{
	if( m_pCustomRender ) {
		if( IsFocus() ) {
			if( !m_pCustomRender->IsActivate() ) m_pCustomRender->Initialize();
			m_pCustomRender->OnSelect();
			CGlobalValue::GetInstance().SetSignalCustomRenderObject( m_pCustomRender );
		}

		CActionElement *pElement = (CActionElement *)GetParent();
		if( pElement != NULL && pElement->GetType() == CActionBase::ELEMENT ) {
			if( !pElement->IsPause() && !pElement->IsStop() ) {

				if( m_pCustomRender->CheckSignal( pElement->GetPrevFrame(), pElement->GetCurFrame() ) ) {
					if( !m_pCustomRender->IsActivate() ) m_pCustomRender->Initialize();
					m_pCustomRender->OnProcess( LocalTime, 0.f );
				}



				// #56216.
				m_pCustomRender->CheckStartEndSignal( pElement->GetCurFrame() );


			}
		}
	}
}

void CActionSignal::InitPropertyInfo()
{
	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );
	if( pItem == NULL ) return;

	if( !m_pVecPropertyList.empty() ) return;
	for( DWORD i=0; i<pItem->GetParameterCount(); i++ ) {
		CUnionValueProperty *pSource = pItem->GetParameter(i);
		CUnionValueProperty *pVariable = new CUnionValueProperty( pSource->GetType() );
		*pVariable = *pSource;
		pVariable->SetCategory( pItem->GetName() );
		m_pVecPropertyList.push_back( pVariable );
	}
	if( !m_pCustomRender ) {
		CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );
		m_pCustomRender = CSignalCustomRender::AllocCustomRender( pItem->GetName().GetBuffer() );
		if( m_pCustomRender ) m_pCustomRender->SetSignal( this );
	}
}

void CActionSignal::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	if( !m_pCustomRender ) {
		CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );
		m_pCustomRender = CSignalCustomRender::AllocCustomRender( pItem->GetName().GetBuffer() );
		if( m_pCustomRender ) {
			m_pCustomRender->SetSignal( this );
			if( !m_pCustomRender->IsActivate() ) m_pCustomRender->Initialize();
		}
	}
}

void CActionSignal::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	if( pVariable->GetType() == CUnionValueProperty::String_FileOpen ) {
		char szTemp[_MAX_PATH] = { 0, };
		_GetFullFileName( szTemp, _countof(szTemp), pVariable->GetVariableString() );
		pVariable->SetVariable( szTemp );
	}
	if( GetCustomRender() )
		GetCustomRender()->OnModify();
}

void CActionSignal::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	if( NULL == pVariable || m_pVecPropertyList.size() <= dwIndex )
		return;

	CUnionValueProperty * pProperty = GetProperty( dwIndex );
	CUnionValueProperty sProperty( pVariable->GetType() );
	sProperty = *pVariable;

	if( pProperty )
		*pProperty = sProperty;
}

bool CActionSignal::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_nSignalIndex, sizeof(int), 1, fp );
	fwrite( &m_nStartFrame, sizeof(int), 1, fp );
	fwrite( &m_nEndFrame, sizeof(int), 1, fp );
	fwrite( &m_nYOrder, sizeof(int), 1, fp );

	CalcArray();

	int nBufferSize = 256;
	int nBufferSectorByte = 4;

	int nArraySize = nBufferSize / nBufferSectorByte;

	union {
		int nBuffer[64];
		float fBuffer[64];
	};
	memset( nBuffer, 0, sizeof(nBuffer));

	int nVector2Index = 0;
	int nVector3Index = 0;
	int nVector4Index = 0;
	int nStringIndex = 0;


	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );

	int nOffset;
	for( DWORD i=0; i<m_pVecPropertyList.size(); i++ ) {
		CUnionValueProperty *pSourceVar = pItem->GetParameter(i);
		nOffset = pItem->GetParameterUniqueIndex(i);

		switch( m_pVecPropertyList[i]->GetType() ) {
			case CUnionValueProperty::Integer:
			case CUnionValueProperty::Integer_Combo:
			case CUnionValueProperty::Color:
				nBuffer[nOffset] = m_pVecPropertyList[i]->GetVariableInt();
				break;
			case CUnionValueProperty::Boolean:
				nBuffer[nOffset] = (BOOL)m_pVecPropertyList[i]->GetVariableBool();
				break;
			case CUnionValueProperty::Float:
				fBuffer[nOffset] = m_pVecPropertyList[i]->GetVariableFloat();
				break;
			case CUnionValueProperty::Vector2:
				nBuffer[nOffset] = nVector2Index;
				nVector2Index++;
				break;
			case CUnionValueProperty::Vector3:
				nBuffer[nOffset] = nVector3Index;
				nVector3Index++;
				break;
			case CUnionValueProperty::Vector4:
			case CUnionValueProperty::Vector4Color:
				nBuffer[nOffset] = nVector4Index;
				nVector4Index++;
				break;
			case CUnionValueProperty::String:
			case CUnionValueProperty::String_FileOpen:
				nBuffer[nOffset] = nStringIndex;
				nStringIndex++;
				break;
			case CUnionValueProperty::Pointer:
				nBuffer[nOffset] = 0;
				break;
		}
	}
	fwrite( nBuffer, sizeof(nBuffer), 1, fp );

	// Array 저장
	fwrite( &nVector2Index, sizeof(int), 1, fp );
	if( m_VecVector2List.size() ) {
		fwrite( &m_nVecVector2TableIndexList[0], sizeof(int), m_nVecVector2TableIndexList.size(), fp );
		fwrite( &m_VecVector2List[0], sizeof(EtVector2), m_VecVector2List.size(), fp );
	}

	fwrite( &nVector3Index, sizeof(int), 1, fp );
	if( m_VecVector3List.size() ) {
		fwrite( &m_nVecVector3TableIndexList[0], sizeof(int), m_nVecVector3TableIndexList.size(), fp );
		fwrite( &m_VecVector3List[0], sizeof(EtVector3), m_VecVector3List.size(), fp );
	}

	fwrite( &nVector4Index, sizeof(int), 1, fp );
	if( m_VecVector4List.size() ) {
		fwrite( &m_nVecVector4TableIndexList[0], sizeof(int), m_nVecVector4TableIndexList.size(), fp );
		fwrite( &m_VecVector4List[0], sizeof(EtVector4), m_VecVector4List.size(), fp );
	}

	fwrite( &nStringIndex, sizeof(int), 1, fp );
	if( m_VecStringList.size() )
		fwrite( &m_nVecStringTableIndexList[0], sizeof(int), m_nVecStringTableIndexList.size(), fp );
	for( DWORD i=0; i<m_VecStringList.size(); i++ ) {
		WriteCString( &m_VecStringList[i], fp );
	}
	return true;
}

bool CActionSignal::ImportObject( FILE *fp )
{
	fread( &m_nSignalIndex, sizeof(int), 1, fp );
	fread( &m_nStartFrame, sizeof(int), 1, fp );
	fread( &m_nEndFrame, sizeof(int), 1, fp );
	fread( &m_nYOrder, sizeof(int), 1, fp );

	// 시그널이 지워졌으면 읽지를 말자.
	bool bValidSignal = true;
	if( CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex ) == NULL ) bValidSignal = false;
	else Activate();

	union {
		int nBuffer[64];
		float fBuffer[64];
	};
	memset( nBuffer, 0, sizeof(nBuffer));
	fread( nBuffer, sizeof(nBuffer), 1, fp );

	int nVector2Index = 0;
	int nVector3Index = 0;
	int nVector4Index = 0;
	int nStringIndex = 0;
	m_VecVector2List.clear();
	m_VecVector3List.clear();
	m_VecVector4List.clear();
	m_VecStringList.clear();
	m_nVecVector2TableIndexList.clear();
	m_nVecVector3TableIndexList.clear();
	m_nVecVector4TableIndexList.clear();
	m_nVecStringTableIndexList.clear();

	int nCount = 0;
	EtVector2 Vector2;
	EtVector3 Vector3;
	EtVector4 Vector4;
	CString szStr;


	fread( &nCount, sizeof(int), 1, fp );
	if( nCount ) {
		fseek( fp, sizeof(int) * nCount, SEEK_CUR );
		m_VecVector2List.resize( nCount );
		fread( &m_VecVector2List[0], sizeof(EtVector2), nCount, fp );
	}
	nCount = 0;
	fread( &nCount, sizeof(int), 1, fp );
	if( nCount ) {
		fseek( fp, sizeof(int) * nCount, SEEK_CUR );
		m_VecVector3List.resize( nCount );
		fread( &m_VecVector3List[0], sizeof(EtVector3), nCount, fp );
	}
	nCount = 0;
	fread( &nCount, sizeof(int), 1, fp );
	if( nCount ) {
		fseek( fp, sizeof(int) * nCount, SEEK_CUR );
		m_VecVector4List.resize( nCount );
		fread( &m_VecVector4List[0], sizeof(EtVector4), nCount, fp );
	}
	nCount = 0;
	fread( &nCount, sizeof(int), 1, fp );
	fseek( fp, sizeof(int) * nCount, SEEK_CUR );
	for( int i=0; i<nCount; i++ ) {
		ReadCString( &szStr, fp );
		m_VecStringList.push_back( szStr );
	}
	if( bValidSignal == false ) return false;

	int nOffset;
	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );
	if( pItem == NULL ) return false;
	for( DWORD i=0; i<pItem->GetParameterCount(); i++ ) {
		CUnionValueProperty *pSource = pItem->GetParameter(i);
		CUnionValueProperty *pVariable = m_pVecPropertyList[i];
		nOffset = pItem->GetParameterUniqueIndex(i);

		switch( pSource->GetType() ) {
			case CUnionValueProperty::Integer:
			case CUnionValueProperty::Integer_Combo:
			case CUnionValueProperty::Color:
				pVariable->SetVariable( (int)nBuffer[nOffset] );
				break;
			case CUnionValueProperty::Boolean:
				pVariable->SetVariable( (bool)(nBuffer[nOffset]==TRUE) );
				break;
			case CUnionValueProperty::Float:
				pVariable->SetVariable( fBuffer[nOffset] );
				break;
			case CUnionValueProperty::Vector2:
				if( nVector2Index < (int)m_VecVector2List.size() ) {
					pVariable->SetVariable( (D3DXVECTOR2)m_VecVector2List[nVector2Index] );
					nVector2Index++;
				}
				break;
			case CUnionValueProperty::Vector3:
				if( nVector3Index < (int)m_VecVector3List.size() ) {
					pVariable->SetVariable( (D3DXVECTOR3)m_VecVector3List[nVector3Index] );
					nVector3Index++;
				}
				break;
			case CUnionValueProperty::Vector4:
			case CUnionValueProperty::Vector4Color:
				if( nVector4Index < (int)m_VecVector4List.size() ) {
					pVariable->SetVariable( (D3DXVECTOR4)m_VecVector4List[nVector4Index] );
					nVector4Index++;
				}
				break;
			case CUnionValueProperty::String:
			case CUnionValueProperty::String_FileOpen:
				if( nStringIndex < (int)m_VecStringList.size() ) {
					pVariable->SetVariable( (char*)m_VecStringList[nStringIndex].GetBuffer() );
					nStringIndex++;
				}
				break;
			case CUnionValueProperty::Pointer:
				pVariable->SetVariable( 0 );
				break;
		}
	}

	return true;
}


void CActionSignal::CalcArray()
{
	m_VecVector2List.clear();
	m_VecVector3List.clear();
	m_VecVector4List.clear();
	m_VecStringList.clear();
	m_nVecVector2TableIndexList.clear();
	m_nVecVector3TableIndexList.clear();
	m_nVecVector4TableIndexList.clear();
	m_nVecStringTableIndexList.clear();

	int nOffset;
	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_nSignalIndex );
	for( DWORD i=0; i<m_pVecPropertyList.size(); i++ ) {
		CUnionValueProperty *pVariable = m_pVecPropertyList[i];
		nOffset = pItem->GetParameterUniqueIndex(i);
		switch( pVariable->GetType() ) {
			case CUnionValueProperty::Vector2:
				m_VecVector2List.push_back( pVariable->GetVariableVector2() );
				m_nVecVector2TableIndexList.push_back(nOffset);
				break;
			case CUnionValueProperty::Vector3:
				m_VecVector3List.push_back( pVariable->GetVariableVector3() );
				m_nVecVector3TableIndexList.push_back(nOffset);
				break;
			case CUnionValueProperty::Vector4:
			case CUnionValueProperty::Vector4Color:
				m_VecVector4List.push_back( pVariable->GetVariableVector4() );
				m_nVecVector4TableIndexList.push_back(nOffset);
				break;
			case CUnionValueProperty::String:
			case CUnionValueProperty::String_FileOpen:
				m_VecStringList.push_back( CString( pVariable->GetVariableString() ) );
				m_nVecStringTableIndexList.push_back(nOffset);
				break;
		}
	}
}

CActionSignal &CActionSignal::operator = ( CActionSignal &e )
{
	m_nSignalIndex = e.m_nSignalIndex;
	m_nStartFrame = e.m_nStartFrame;
	m_nEndFrame = e.m_nEndFrame;
	m_nYOrder = e.m_nYOrder;

	*(CActionBase*)this = *(CActionBase*)&e;

	return *this;
}

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
void CActionSignal::ExportSignal( FILE *fp, int nCount )
{
	char	sTemp[_MAX_PATH] = {0, };	
	bool	bEnable = TRUE;	
	int i;
	const char *szString = NULL;

	if( fp == NULL )
		return;

	fprintf_s( fp, "\n");

	// 시그널 번호 
	ZeroMemory( sTemp, _MAX_PATH );
	itoa( nCount , sTemp, 10);
	fprintf_s( fp, "%s,", sTemp );

	// 시그널 속성 데이터 
	for( i = 0; i < this->GetPropertyList()->size(); i++ ){	

		ZeroMemory( sTemp, _MAX_PATH );		

		switch( this->GetProperty( i )->GetType() )
		{
		case CUnionValueProperty::Boolean: 
			if( this->GetProperty( i )->GetVariableBool() )
				sprintf_s( sTemp, "TRUE");
			else 
				sprintf_s( sTemp, "FALSE");	
			break;
		case CUnionValueProperty::Char:
			sprintf_s( sTemp, "%c", sTemp );
			break;
		case CUnionValueProperty::Integer:
			sprintf_s( sTemp, "%d", this->GetProperty( i )->GetVariableInt() ); 
			break;
		case CUnionValueProperty::Integer_Combo:
			szString = _GetSubStrByCount( 1 + this->GetProperty( i )->GetVariableInt(), (char*)this->GetProperty( i )->GetSubDescription() );
			sprintf_s( sTemp, "%s",szString ); 
			break;
		case CUnionValueProperty::Float:		
			sprintf_s(sTemp, "%f", this->GetProperty( i )->GetVariableFloat() );
			break;
		case CUnionValueProperty::String:
		case CUnionValueProperty::String_FileOpen:
			sprintf_s( sTemp, "%s", this->GetProperty( i )->GetVariableString() );
			break;
		case CUnionValueProperty::Vector2:
			sprintf_s( sTemp, "x : %f y : %f", 
				this->GetProperty( i )->GetVariableVector2().x, 												
				this->GetProperty( i )->GetVariableVector2().y ); 
			break;
		case CUnionValueProperty::Vector3:
			sprintf_s( sTemp, "x : %f y : %f z : %f", 
				this->GetProperty( i )->GetVariableVector3().x, 
				this->GetProperty( i )->GetVariableVector3().y,
				this->GetProperty( i )->GetVariableVector3().z );
			break;
		case CUnionValueProperty::Vector4:
			sprintf_s(  sTemp, "x : %f y : %f z : %f w : %f", 
				this->GetProperty( i )->GetVariableVector4().x, 
				this->GetProperty( i )->GetVariableVector4().y, 
				this->GetProperty( i )->GetVariableVector4().z, 
				this->GetProperty( i )->GetVariableVector4().w ); 
			break;
		case CUnionValueProperty::Integer_Range:
			sprintf_s( sTemp, "%d", *this->GetProperty( i )->GetVariableIntRange() ); 
			break;
		case CUnionValueProperty::Float_Range:
			sprintf_s( sTemp, "%f", *this->GetProperty( i )->GetVariableFloatRange() ); 
			break;
		default:
			bEnable = FALSE;
			break;
		}

		// 사용할려고 만들어 놓은 요소이나, 속성창에서 보여지지 않는 요소는 인식불가로 판정 
		if( bEnable )
			fprintf_s( fp, "%s,", sTemp );
		else  {
			fprintf_s( fp, "인식불가,");
			bEnable = TRUE;
		}
	}

}
#endif

