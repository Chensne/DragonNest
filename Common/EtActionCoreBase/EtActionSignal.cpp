#include "StdAfx.h"
#include "EtActionSignal.h"
#include "EtActionCoreMng.h"
#include "../GameCommon/SignalHeader.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtActionSignal::CEtActionSignal()
{
	m_nSignalIndex = -1;
	m_nSignalListArrayIndex	= -1;
	m_nStartFrame = -1;
	m_nEndFrame = -1;
	m_cOrder = 0;
	m_pData = NULL;
}

CEtActionSignal::~CEtActionSignal()
{
	if( m_pData ) {

#ifdef WIN64
		const int usingCount = GetSignalDataUsingCount64(m_nSignalIndex);
#else
		const int usingCount = GetSignalDataUsingCount(m_nSignalIndex);
#endif

		for( DWORD i=0; i<m_nVec2Index.size(); i++ ) {
			if( m_nVec2Index[i] < usingCount )
			{
				EtVector2 *pVec;
				memcpy( &pVec, (int*)(m_pData) + m_nVec2Index[i], sizeof(int*) );
				SAFE_DELETE( pVec );
			}
		}

		for( DWORD i=0; i<m_nVec3Index.size(); i++ ) {
			if( m_nVec3Index[i] < usingCount )
			{
				EtVector3 *pVec;
				memcpy( &pVec, (int*)(m_pData) + m_nVec3Index[i], sizeof(int*) );
				SAFE_DELETE( pVec );
			}
		}
		for( DWORD i=0; i<m_nVec4Index.size(); i++ ) {
			if( m_nVec4Index[i] < usingCount )
			{
				EtVector4 *pVec;
				memcpy( &pVec, (int*)(m_pData) + m_nVec4Index[i], sizeof(int*) );
				SAFE_DELETE( pVec );
			}
		}
		for( DWORD i=0; i<m_nVecStrIndex.size(); i++ ) {
			if( m_nVecStrIndex[i] < usingCount )
			{
				char *pPtr;
				memcpy( &pPtr, (int*)(m_pData) + m_nVecStrIndex[i], sizeof(int*) );
				SAFE_DELETEA( pPtr );
			}
		}
	}

	SAFE_DELETEA( m_pData );
}

// note by kalliste : 시그널 스트럭쳐를 내용 복사해서 게임내에 사용하는 경우가 있어 일단 막습니다.(#40311)
//#define _TEST_DEFAULT_SIGNAL_ALLOC

bool CEtActionSignal::LoadSignal( CStream *pStream )
{
	int nTemp;
	pStream->Read( &nTemp, sizeof(int) );
	m_nSignalIndex = nTemp;
	pStream->Read( &nTemp, sizeof(int) );
	m_nStartFrame = nTemp;
	pStream->Read( &nTemp, sizeof(int) );
	m_nEndFrame = nTemp;
	pStream->Read( &nTemp, sizeof(int) );
	m_cOrder = nTemp;
//	pStream->Seek( sizeof(int), SEEK_CUR );

#ifdef _TEST_DEFAULT_SIGNAL_ALLOC
	m_pData = new int[64];
	pStream->Read( m_pData, 256 );
#else
#ifdef WIN64
	const int usingCount = GetSignalDataUsingCount64(m_nSignalIndex);
#else
	const int usingCount = GetSignalDataUsingCount(m_nSignalIndex);
#endif
	DWORD dataSize = usingCount * sizeof(int);
	if (dataSize > 0 && dataSize <= 256)
	{
		m_pData = new int[usingCount];
		pStream->Read( m_pData, dataSize );
		pStream->Seek((256 - dataSize), SEEK_CUR);
	}
	else
	{
		pStream->Seek(256, SEEK_CUR);
	}
#endif

	std::vector<EtVector2> vVec2List;
	std::vector<EtVector3> vVec3List;
	std::vector<EtVector4> vVec4List;
	std::vector<std::string> szVecList;

	m_nVec2Index.clear();
	m_nVec3Index.clear();
	m_nVec4Index.clear();
	m_nVecStrIndex.clear();

	// Table Load
	int nTableCount;
	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		vVec2List.resize( nTableCount );
		m_nVec2Index.resize( nTableCount );
		for( int i=0; i<nTableCount; i++ ) {
			pStream->Read( &nTemp, sizeof(int) );
			m_nVec2Index[i] = nTemp;
		}
		pStream->Read( &vVec2List[0], sizeof(EtVector2) * nTableCount );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		vVec3List.resize( nTableCount );
		m_nVec3Index.resize( nTableCount );
		for( int i=0; i<nTableCount; i++ ) {
			pStream->Read( &nTemp, sizeof(int) );
			m_nVec3Index[i] = nTemp;
		}
		pStream->Read( &vVec3List[0], sizeof(EtVector3) * nTableCount );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		vVec4List.resize( nTableCount );
		m_nVec4Index.resize( nTableCount );
		for( int i=0; i<nTableCount; i++ ) {
			pStream->Read( &nTemp, sizeof(int) );
			m_nVec4Index[i] = nTemp;
		}
		pStream->Read( &vVec4List[0], sizeof(EtVector4) * nTableCount );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		m_nVecStrIndex.resize( nTableCount );
		for( int i=0; i<nTableCount; i++ ) {
			pStream->Read( &nTemp, sizeof(int) );
			m_nVecStrIndex[i] = nTemp;
		}
	}
	for( int i=0; i<nTableCount; i++ ) {
		std::string szStr;
		ReadStdString( szStr, pStream );
		szVecList.push_back( szStr );
	}
#ifdef WIN64
	// 64비트일 경우 포인터 때문에 버퍼를 제정렬 해줘야 합니다.
	m_nVec2Index64.assign( m_nVec2Index.size(), 0 );
	m_nVec3Index64.assign( m_nVec3Index.size(), 0 );
	m_nVec4Index64.assign( m_nVec4Index.size(), 0 );
	m_nVecStrIndex64.assign( m_nVecStrIndex.size(), 0 );

#ifdef _TEST_DEFAULT_SIGNAL_ALLOC
	int nAddSize = 64 + (int)( m_nVec2Index.size() + m_nVec3Index.size() + m_nVec4Index.size() + m_nVecStrIndex.size() );
	int *pTempBuf = new int[nAddSize];
	memset( pTempBuf, 0, nAddSize * 4 );
	int nTargetOffset = 0;
	for( int i=0; i<64; i++ ) {
		char cFlag = IsPointerTable(i);
		if( cFlag == -1 ) {
			pTempBuf[nTargetOffset] = ((int*)m_pData)[i];
			nTargetOffset++;
		}
		else {
			IncreasePointerTableIndex(i);
			nTargetOffset += 2;
		}
	}
	for( DWORD i=0; i<m_nVec2Index.size(); i++ ) m_nVec2Index[i] += m_nVec2Index64[i];
	for( DWORD i=0; i<m_nVec3Index.size(); i++ ) m_nVec3Index[i] += m_nVec3Index64[i];
	for( DWORD i=0; i<m_nVec4Index.size(); i++ ) m_nVec4Index[i] += m_nVec4Index64[i];
	for( DWORD i=0; i<m_nVecStrIndex.size(); i++ ) m_nVecStrIndex[i] += m_nVecStrIndex64[i];
	SAFE_DELETEA( m_pData );
	m_pData = pTempBuf;
#else
	int *pTempBuf = NULL;
	int nCorrectionSize = (int)( m_nVec2Index.size() + m_nVec3Index.size() + m_nVec4Index.size() + m_nVecStrIndex.size() );
	if (m_pData == NULL && nCorrectionSize != 0)
	{
		_ASSERT(0);
		return false;
	}

	if (m_pData != NULL)
	{
		int nAddSize = usingCount + (int)( m_nVec2Index.size() + m_nVec3Index.size() + m_nVec4Index.size() + m_nVecStrIndex.size() );
		pTempBuf = new int[nAddSize];
		memset( pTempBuf, 0, nAddSize * 4 );
		int nTargetOffset = 0;
		for( int i=0; i<usingCount; i++ ) {
			char cFlag = IsPointerTable(i);
			if( cFlag == -1 ) {
				pTempBuf[nTargetOffset] = ((int*)m_pData)[i];
				nTargetOffset++;
			}
			else {
				IncreasePointerTableIndex(i);
				nTargetOffset += 2;
			}
		}

		for( DWORD i=0; i<m_nVec2Index.size(); i++ ) m_nVec2Index[i] += m_nVec2Index64[i];
		for( DWORD i=0; i<m_nVec3Index.size(); i++ ) m_nVec3Index[i] += m_nVec3Index64[i];
		for( DWORD i=0; i<m_nVec4Index.size(); i++ ) m_nVec4Index[i] += m_nVec4Index64[i];
		for( DWORD i=0; i<m_nVecStrIndex.size(); i++ ) m_nVecStrIndex[i] += m_nVecStrIndex64[i];

		if (m_pData != NULL)
		{
			SAFE_DELETEA( m_pData );
			m_pData = pTempBuf;
		}
	}
#endif // _TEST_DEFAULT_SIGNAL_ALLOC
#endif

	////////////////////
	// note by kalliste : m_pData가 타입에 따라 생성되지 않을 가능성도 있지만 그럴 경우 어차피 table 값이 셋팅되면 안되기 때문에
	//					  데이터 이상 반증이므로 널체크 하지 않는다.
	nTableCount = (int)m_nVec2Index.size();
	for( int i=0; i<nTableCount; i++ ) {
		if( m_nVec2Index[i] < usingCount )
		{
			EtVector2 *pVec = new EtVector2;
			*pVec = vVec2List[i];
			memcpy( (int*)m_pData + ( m_nVec2Index[i] ), &pVec, sizeof(int*) );
		}
	}
	nTableCount = (int)m_nVec3Index.size();
	for( int i=0; i<nTableCount; i++ ) {		
		if( m_nVec3Index[i] < usingCount )
		{
			EtVector3 *pVec = new EtVector3;
			*pVec = vVec3List[i];
			memcpy( (int*)m_pData + ( m_nVec3Index[i] ), &pVec, sizeof(int*) );
		}
	}
	nTableCount = (int)m_nVec4Index.size();
	for( int i=0; i<nTableCount; i++ ) {
		if( m_nVec4Index[i] < usingCount )
		{
			EtVector4 *pVec = new EtVector4;
			*pVec = vVec4List[i];
			memcpy( (int*)m_pData + ( m_nVec4Index[i] ), &pVec, sizeof(int*) );
		}
	}
	
	nTableCount = (int)m_nVecStrIndex.size();
	for( int i=0; i<nTableCount; i++ ) {
		if( m_nVecStrIndex[i] < usingCount )
		{
			char *pStr = new char[szVecList[i].size()+1];
			memset( pStr, 0, szVecList[i].size()+1 );
			strcpy_s( pStr, szVecList[i].size()+1, szVecList[i].c_str() );
			memcpy( (int*)m_pData + ( m_nVecStrIndex[i] ), &pStr, sizeof(int*) );
		}
	}

	SAFE_DELETE_VEC( vVec2List );
	SAFE_DELETE_VEC( vVec2List );
	SAFE_DELETE_VEC( vVec3List );
	SAFE_DELETE_VEC( szVecList );

	return true;
}

bool CEtActionSignal::SkipSignal( CStream *pStream )
{
	pStream->Seek( 256 + sizeof(int) * 4, SEEK_CUR );

	int nTableCount;
	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		pStream->Seek( (sizeof(EtVector2)+sizeof(int)) * nTableCount, SEEK_CUR );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		pStream->Seek( (sizeof(EtVector3)+sizeof(int)) * nTableCount, SEEK_CUR );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		pStream->Seek( (sizeof(EtVector4)+sizeof(int)) * nTableCount, SEEK_CUR );
	}

	pStream->Read( &nTableCount, sizeof(int) );
	if( nTableCount > 0 ) {
		pStream->Seek( sizeof(int) * nTableCount, SEEK_CUR );
	}

	for( int i=0; i<nTableCount; i++ ) {
		char szBuf[4096] = {0,};
		int nSize;		
		pStream->Read( &nSize, sizeof(int) );
		if( nSize ) {
			if( nSize >= 4096 ) nSize = 4095;
			pStream->Seek( nSize, SEEK_CUR );
		}
	}
	return true;
}

void CEtActionSignal::InsertVec2Table( EtVector2 *pPtr, EtVector2 &vVec )
{
	for( int i=0; i<64; i++ ) {
		if( (int*)(m_pData) + i == (int*)(pPtr) ) {
			if( *((int*)(m_pData) + i) != 0 ) break;
			EtVector2 *pVec = new EtVector2;
			*pVec = vVec;
			m_nVec2Index.push_back(i);
			memcpy( (int*)m_pData + i, &pVec, sizeof(int*) );
			break;
		}
	}
}

void CEtActionSignal::InsertVec3Table( EtVector3 *pPtr, EtVector3 &vVec )
{
	for( int i=0; i<64; i++ ) {
		if( (int*)(m_pData) + i == (int*)(pPtr) ) {
			if( *((int*)(m_pData) + i) != 0 ) break;
			EtVector3 *pVec = new EtVector3;
			*pVec = vVec;
			m_nVec3Index.push_back(i);
			memcpy( (int*)m_pData + i, &pVec, sizeof(int*) );
			break;
		}
	}
}

void CEtActionSignal::InsertVec4Table( EtVector4 *pPtr, EtVector4 &vVec )
{
	for( int i=0; i<64; i++ ) {
		if( (int*)(m_pData) + i == (int*)(pPtr) ) {
			if( *((int*)(m_pData) + i) != 0 ) break;
			EtVector4 *pVec = new EtVector4;
			*pVec = vVec;
			m_nVec4Index.push_back(i);
			memcpy( (int*)m_pData + i, &pVec, sizeof(int*) );
			break;
		}
	}
}

void CEtActionSignal::InsertStrTable( char *pPtr, std::string &szStr )
{
//	if( pPtr != NULL ) return;
	for( int i=0; i<64; i++ ) {
		if( (int*)(m_pData) + i == (int*)(pPtr) ) {
			if( *((int*)(m_pData) + i) != 0 ) break;
			char *pStr = new char[szStr.size() + 1];
			memset( pStr, 0, szStr.size()+1 );
			strcpy_s( pStr, szStr.size()+1, szStr.c_str() );

			m_nVecStrIndex.push_back(i);
			memcpy( (int*)m_pData + i, &pStr, sizeof(int*) );
			break;
		}
	}
}

#ifdef WIN64
char CEtActionSignal::IsPointerTable( int nIndex )
{
	if( std::find( m_nVec2Index.begin(), m_nVec2Index.end(), nIndex ) != m_nVec2Index.end() ) return 0;
	if( std::find( m_nVec3Index.begin(), m_nVec3Index.end(), nIndex ) != m_nVec3Index.end() ) return 1;
	if( std::find( m_nVec4Index.begin(), m_nVec4Index.end(), nIndex ) != m_nVec4Index.end() ) return 2;
	if( std::find( m_nVecStrIndex.begin(), m_nVecStrIndex.end(), nIndex ) != m_nVecStrIndex.end() ) return 3;

	return -1;
}

void CEtActionSignal::IncreasePointerTableIndex( int nValue )
{
	for( DWORD i=0; i<m_nVec2Index.size(); i++ ) {
		if( m_nVec2Index[i] > nValue ) m_nVec2Index64[i]++;
	}
	for( DWORD i=0; i<m_nVec3Index.size(); i++ ) {
		if( m_nVec3Index[i] > nValue ) m_nVec3Index64[i]++;
	}
	for( DWORD i=0; i<m_nVec4Index.size(); i++ ) {
		if( m_nVec4Index[i] > nValue ) m_nVec4Index64[i]++;
	}
	for( DWORD i=0; i<m_nVecStrIndex.size(); i++ ) {
		if( m_nVecStrIndex[i] > nValue ) m_nVecStrIndex64[i]++;
	}
}

#endif