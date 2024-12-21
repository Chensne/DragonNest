#include "StdAfx.h"
#include "MAPartsBody.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnParts.h"
#include "DnWeapon.h"

MAPartsBody::MAPartsBody()
{
	m_pActor = NULL;
	m_bIsValidActor = true;

	memset( m_bSelfDelete, 0, sizeof(m_bSelfDelete) );
	memset( m_bCashSelfDelete, 0, sizeof(m_bCashSelfDelete) );

	// 무기를 제외한 전환버튼
	memset( m_bPartsViewOrder, 1, sizeof(m_bPartsViewOrder) );
}

MAPartsBody::~MAPartsBody()
{
	for( DWORD i=0; i<CDnParts::PartsTypeEnum_Amount; i++ ) {
		DetachParts( (CDnParts::PartsTypeEnum)i );
	}
	for( DWORD i=0; i<CDnParts::CashPartsTypeEnum_Amount; i++ ) {
		DetachCashParts( (CDnParts::PartsTypeEnum)i );
	}
}

bool MAPartsBody::IsValidActor()
{
	if( m_pActor ) return true;
	else {
		if( !m_bIsValidActor ) return false;
		m_pActor = dynamic_cast<CDnActor*>(this);
		if( !m_pActor ) m_bIsValidActor = false;
	}
	return true;
}

void MAPartsBody::LoadSkin( const char *szDummySkinName, const char *szAniName )
{
	if( m_hObject ) return;

	m_szAniFileName = szAniName;

	m_hObject = EternityEngine::CreateAniObject( STATICFUNC() szDummySkinName, szAniName );
	m_hObject->SetCalcPositionFlag( CALC_POSITION_Y );
}

bool MAPartsBody::AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete )
{
	if( !hParts ) return false;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PartsTable.ext failed\r\n");
		return false;
	}

	if( !IsValidActor() ) return false;

	CDnParts::PartsTypeEnum SlotIndex = ( Index == (CDnParts::PartsTypeEnum)-1 ) ?  hParts->GetPartsType() : Index;
	if( m_hPartsObject[SlotIndex] ) {
		DetachParts( SlotIndex );
	}

	m_hPartsObject[SlotIndex] = hParts;
	m_bSelfDelete[SlotIndex] = bDelete;
	hParts->CreateObject( m_pActor->GetMySmartPtr(), m_szAniFileName.c_str() );

	return true;
}

bool MAPartsBody::DetachParts( CDnParts::PartsTypeEnum Index )
{
	if( !m_hPartsObject[Index] ) return true;

	m_hPartsObject[Index]->FreeObject();
	if( m_bSelfDelete[Index] ) {
		SAFE_RELEASE_SPTR( m_hPartsObject[Index] );
		m_bSelfDelete[Index ] = false;
	}
	m_hPartsObject[Index].Identity();
	return true;
}

bool MAPartsBody::AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete )
{
	if( !hParts ) return false;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PartsTable.ext failed\r\n");
		return false;
	}

	if( !IsValidActor() ) return false;

	CDnParts::PartsTypeEnum SlotIndex = ( Index == (CDnParts::PartsTypeEnum)-1 ) ?  hParts->GetPartsType() : Index;
	if( m_hCashPartsObject[SlotIndex] ) {
		DetachCashParts( SlotIndex );
	}

	m_hCashPartsObject[SlotIndex] = hParts;
	m_bCashSelfDelete[SlotIndex] = bDelete;
	hParts->CreateObject( m_pActor->GetMySmartPtr(), m_szAniFileName.c_str() );

	return true;
}

bool MAPartsBody::DetachCashParts( CDnParts::PartsTypeEnum Index )
{
	if( !m_hCashPartsObject[Index] ) return true;

	m_hCashPartsObject[Index]->FreeObject();
	if( m_bCashSelfDelete[Index] ) {
		SAFE_RELEASE_SPTR( m_hCashPartsObject[Index] );
		m_bCashSelfDelete[Index ] = false;
	}
	m_hCashPartsObject[Index].Identity();
	return true;
}

void MAPartsBody::RefreshPartsViewOrder( int nEquipIndex )
{
	if( nEquipIndex < 0 || nEquipIndex >= CDnParts::SwapParts_Amount ) return;

	// 클라이언트와 달리 쇼-하이드 처리할필요 없으므로 아무일 하지 않는다.
}