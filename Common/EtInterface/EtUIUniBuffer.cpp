#include "StdAfx.h"
#include "EtUIUniBuffer.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

HINSTANCE CEtUIUniBuffer::s_hDll = NULL;
HRESULT (WINAPI *CEtUIUniBuffer::_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) = Dummy_ScriptApplyDigitSubstitution;
HRESULT (WINAPI *CEtUIUniBuffer::_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringAnalyse;
HRESULT (WINAPI *CEtUIUniBuffer::_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) = Dummy_ScriptStringCPtoX;
HRESULT (WINAPI *CEtUIUniBuffer::_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* ) = Dummy_ScriptStringXtoCP;
HRESULT (WINAPI *CEtUIUniBuffer::_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringFree;
const SCRIPT_LOGATTR* (WINAPI *CEtUIUniBuffer::_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pLogAttr;
const int* (WINAPI *CEtUIUniBuffer::_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS ) = Dummy_ScriptString_pcOutChars;

CEtUIUniBuffer::CEtUIUniBuffer( int nInitialSize )
{
	CEtUIUniBuffer::Initialize();

	m_nBufferSize = 0;
	m_pwszBuffer = NULL;
	m_bAnalyseRequired = true;
	m_Analysis = NULL;
	//m_pFont = NULL;
	m_nMaxEditBoxLength = MAX_COMPSTRING_SIZE;
	
	if( nInitialSize > 0 )
	{
		SetBufferSize( nInitialSize );
	}
}

CEtUIUniBuffer::~CEtUIUniBuffer(void)
{
	SAFE_DELETE_ARRAY( m_pwszBuffer );

	if( m_Analysis )
	{
		_ScriptStringFree( &m_Analysis );
	}
}

void CEtUIUniBuffer::Initialize()
{
	if( s_hDll )
	{
		return;
	}

	char szPath[ MAX_PATH + 1 ];

	if( !::GetSystemDirectory( szPath, MAX_PATH + 1 ) )
	{
		return;
	}

	strcat( szPath, UNISCRIBE_DLLNAME );
	s_hDll = LoadLibrary( szPath );
	if( s_hDll )
	{
		*( FARPROC * )&_ScriptApplyDigitSubstitution = GetProcAddress( s_hDll, "ScriptApplyDigitSubstitution" );
		*( FARPROC * )&_ScriptStringAnalyse = GetProcAddress( s_hDll, "ScriptStringAnalyse" );
		*( FARPROC * )&_ScriptStringCPtoX = GetProcAddress( s_hDll, "ScriptStringCPtoX" );
		*( FARPROC * )&_ScriptStringXtoCP = GetProcAddress( s_hDll, "ScriptStringXtoCP" );
		*( FARPROC * )&_ScriptStringFree = GetProcAddress( s_hDll, "ScriptStringFree" );
		*( FARPROC * )&_ScriptString_pLogAttr = GetProcAddress( s_hDll, "ScriptString_pLogAttr" );
		*( FARPROC * )&_ScriptString_pcOutChars = GetProcAddress( s_hDll, "ScriptString_pcOutChars" );
	}
}


//--------------------------------------------------------------------------------------
void CEtUIUniBuffer::Uninitialize()
{
	if( s_hDll )
	{
		_ScriptApplyDigitSubstitution = Dummy_ScriptApplyDigitSubstitution;
		_ScriptStringAnalyse = Dummy_ScriptStringAnalyse;
		_ScriptStringCPtoX = Dummy_ScriptStringCPtoX;
		_ScriptStringXtoCP = Dummy_ScriptStringXtoCP;
		_ScriptStringFree = Dummy_ScriptStringFree;
		_ScriptString_pLogAttr = Dummy_ScriptString_pLogAttr;
		_ScriptString_pcOutChars = Dummy_ScriptString_pcOutChars;

		FreeLibrary( s_hDll );
		s_hDll = NULL;
	}
}

bool CEtUIUniBuffer::SetBufferSize( int nNewSize )
{
	if( GetBufferSize() >= GetMaxEditBoxLength() )
	{
		return false;
	}

	int nAllocateSize(0);
	WCHAR *pTempBuffer(NULL);
	int nBufferSize = GetBufferSize();

	if( ( nNewSize == -1 ) || ( nNewSize < nBufferSize * 2 ) )
	{
		if( nBufferSize )
		{
			nAllocateSize = nBufferSize * 2;
		}
		else
		{
			nAllocateSize = MAX_COMPSTRING_SIZE;
		}
	}
	else
	{
		nAllocateSize = nNewSize * 2;
	}

	if( nAllocateSize > GetMaxEditBoxLength() )
	{
		nAllocateSize = GetMaxEditBoxLength();
	}

	pTempBuffer = new WCHAR[ nAllocateSize ];
	if( m_pwszBuffer )
	{
		memcpy( pTempBuffer, m_pwszBuffer, sizeof( WCHAR ) * nBufferSize );
		SAFE_DELETE_ARRAY( m_pwszBuffer );
	}
	else
	{
		memset( pTempBuffer, 0, sizeof( WCHAR ) * nAllocateSize );
	}

	m_pwszBuffer = pTempBuffer;
	m_nBufferSize = nAllocateSize;

	return true;
}

void CEtUIUniBuffer::SetBuffer( WCHAR* pBuffer, int nBufferSize )
{
	m_pwszBuffer = pBuffer;
	m_nBufferSize = nBufferSize;
}

WCHAR &CEtUIUniBuffer::operator[]( int nIndex )
{
	m_bAnalyseRequired = true;
	return m_pwszBuffer[ nIndex ];
}

void CEtUIUniBuffer::Clear()
{
	*m_pwszBuffer = L'\0';
	m_bAnalyseRequired = true;
}

void CEtUIUniBuffer::ClearMemory()
{
	SecureZeroMemory( m_pwszBuffer, sizeof( WCHAR ) * lstrlenW( m_pwszBuffer ) );
	m_bAnalyseRequired = true;
}

bool CEtUIUniBuffer::InsertChar( int nIndex, WCHAR wChar, bool bIgnoreValidityCheck )
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return InsertCharThai( nIndex, wChar );

	if( bIgnoreValidityCheck == false )
	{
		if (IsEnableInsertChar( nIndex, wChar ) == false)
			return false;
	}

	if( lstrlenW( m_pwszBuffer ) + 1 >= GetBufferSize() )
	{
		SetBufferSize( -1 );
	}

	WCHAR *pwszDest, *pwszStop, *pwszSour;

	pwszDest = m_pwszBuffer + lstrlenW( m_pwszBuffer ) + 1;
	pwszStop = m_pwszBuffer + nIndex;
	pwszSour = pwszDest - 1;

	while( pwszDest > pwszStop )
	{
		*pwszDest-- = *pwszSour--;
	}
	m_pwszBuffer[ nIndex ] = wChar;
	m_bAnalyseRequired = true;

	return true;
}

bool CEtUIUniBuffer::RemoveChar( int nIndex )
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return RemoveCharThai( nIndex );

#ifdef _TEST_CODE_KAL
	int len = lstrlenW( m_pwszBuffer );
#endif

	if( ( !lstrlenW( m_pwszBuffer ) ) || ( nIndex < 0 ) || ( nIndex >= lstrlenW( m_pwszBuffer ) ) )
	{
		return false;
	}
	memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof( WCHAR ) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );
	m_bAnalyseRequired = true;

	return true;
}

bool CEtUIUniBuffer::IsEnableInsertChar( int nIndex, WCHAR wChar ) const
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return IsEnableInsertCharThai( nIndex, wChar );

	if( ( nIndex < 0 ) || ( nIndex > lstrlenW( m_pwszBuffer ) ) )
	{
		return false;
	}

	if( IsMaxEditBoxLength() )
	{
		return false;
	}

	return true;
}

bool CEtUIUniBuffer::IsEnableInsertString( int nIndex, const WCHAR *pStr, int nCount ) const
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return IsEnableInsertStringThai( nIndex, pStr );

	if( nIndex > lstrlenW( m_pwszBuffer ) )
	{
		return false;
	}

	if( nCount == -1 )
	{
		nCount = lstrlenW( pStr );
	}

	if( GetTextSize() + nCount >= GetMaxEditBoxLength() )
	{
		return false;
	}

	return true;
}

bool CEtUIUniBuffer::InsertString( int nIndex, const WCHAR *pStr, int nCount, bool bIgnoreValidityCheck )
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return InsertStringThai( nIndex, pStr );

	if( nCount == -1 )
	{
		nCount = lstrlenW( pStr );
	}

	if (bIgnoreValidityCheck == false)
	{
		if (IsEnableInsertString(nIndex, pStr, nCount) == false)
			return false;
	}

	if( lstrlenW( m_pwszBuffer ) + nCount >= GetBufferSize() )
	{
		SetBufferSize( lstrlenW( m_pwszBuffer ) + nCount + 1 );
	}

	memmove( m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof( WCHAR ) * ( lstrlenW( m_pwszBuffer ) - nIndex + 1 ) );
	memcpy( m_pwszBuffer + nIndex, pStr, nCount * sizeof( WCHAR ) );
	m_bAnalyseRequired = true;

	return true;
}

bool CEtUIUniBuffer::SetText( LPCWSTR wszText )
{
	if( CEtUIEditBox::s_bCheckThaiLanguageVowelRule )
		return SetTextThai( wszText );

	int nRequired;
	nRequired = ( int )wcslen( wszText ) + 1;

	// >= 로 비교하면 MaxChars를 2로 설정 후 "11"을 SetText로 설정할 수 없게된다.
	// 그래서 if( nRequired > GetMaxEditBoxLength() )
	// 이 코드를 사용해야할거 같은데.. 내부코드라 우선 안건드리기로 하겠다.
	// MaxChars의 의미가 조금 다른건지도..
	if( nRequired > GetMaxEditBoxLength() )
	{
		return false;
	}

	while( GetBufferSize() < nRequired )
	{
		if( !SetBufferSize( -1 ) )
		{
			break;
		}
	}
	if( GetBufferSize() >= nRequired )
	{
		wcscpy( m_pwszBuffer, wszText );
		m_bAnalyseRequired = true;
		return true;
	}

	return false;
}

HRESULT CEtUIUniBuffer::CPtoX( int nCP, BOOL bTrail, int *pX )
{
	*pX = 0;

	HRESULT hr = S_OK;

	if( m_bAnalyseRequired )
	{
		hr = Analyse();
	}
	if( SUCCEEDED( hr ) )
	{
		hr = _ScriptStringCPtoX( m_Analysis, nCP, bTrail, pX );
	}

	return hr;
}

HRESULT CEtUIUniBuffer::XtoCP( int nX, int *pCP, int *pnTrail )
{
	*pCP = 0; 
	*pnTrail = FALSE;

	HRESULT hr = S_OK;

	if( m_bAnalyseRequired )
	{
		hr = Analyse();
	}
	if( SUCCEEDED( hr ) )
	{
		hr = _ScriptStringXtoCP( m_Analysis, nX, pCP, pnTrail );
	}

	if( ( *pCP == -1 ) && ( *pnTrail == TRUE ) )
	{
		*pCP = 0; 
		*pnTrail = FALSE;
	} 
	else
	{
		if( ( *pCP > lstrlenW( m_pwszBuffer ) ) && ( *pnTrail == FALSE ) )
		{
			*pCP = lstrlenW( m_pwszBuffer ); 
			*pnTrail = TRUE;
		}
	}

	return hr;
}

void CEtUIUniBuffer::GetPriorItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;
	if( m_bAnalyseRequired )
	{
		Analyse();
	}

	int i, nInitial;
	const SCRIPT_LOGATTR *pLogAttr;

	pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
	{
		return;
	}

	if( !_ScriptString_pcOutChars( m_Analysis ) )
	{
		return;
	}
	nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( ( nCP - 1 ) < ( nInitial ) )
	{
		nInitial = nCP - 1;
	}
	for( i = nInitial; i > 0; i-- )
	{
		if( ( pLogAttr[ i ].fWordStop ) || ( ( !pLogAttr[ i ].fWhiteSpace ) && ( pLogAttr[ i - 1 ].fWhiteSpace ) ) )
		{
			*pPrior = i;
			return;
		}
	}
	*pPrior = 0;
}

void CEtUIUniBuffer::GetNextItemPos( int nCP, int *pPrior )
{
	*pPrior = nCP;

	if( m_bAnalyseRequired )
	{
		Analyse();
	}

	int i, nInitial;
	const SCRIPT_LOGATTR *pLogAttr;

	pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
	{
		return;
	}
	if( !_ScriptString_pcOutChars( m_Analysis ) )
	{
		return;
	}

	nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( ( nCP + 1 ) < ( nInitial ) )
	{
		nInitial = nCP + 1;
	}
	for( i = nInitial; i < *_ScriptString_pcOutChars( m_Analysis ) - 1; i++ )
	{
		if( pLogAttr[i].fWordStop )
		{
			*pPrior = i;
			return;
		}
		else
		{
			if( ( pLogAttr[ i ].fWhiteSpace ) && ( !pLogAttr[ i + 1 ].fWhiteSpace ) )
			{
				*pPrior = i + 1;
				return;
			}
		}
	}
	*pPrior = *_ScriptString_pcOutChars( m_Analysis ) - 1;
}

HRESULT CEtUIUniBuffer::Analyse()
{
	if( m_Analysis )
	{
		_ScriptStringFree( &m_Analysis );
	}

	HRESULT hr;
	SCRIPT_CONTROL ScriptControl;
	SCRIPT_STATE   ScriptState;

	memset( &ScriptControl, 0, sizeof ( SCRIPT_CONTROL ) );
	memset( &ScriptState, 0, sizeof( SCRIPT_STATE ) );

	_ScriptApplyDigitSubstitution( NULL, &ScriptControl, &ScriptState );
	HDC hDC = ::GetDC( GetEtDevice()->GetHWnd() );
	hr = _ScriptStringAnalyse( hDC/*m_pFont->GetDC()*/, m_pwszBuffer, lstrlenW( m_pwszBuffer ) + 1,
		lstrlenW( m_pwszBuffer ) * 3 / 2 + 16, -1, SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0, &ScriptControl, &ScriptState, NULL, NULL, NULL, &m_Analysis );
	if( SUCCEEDED( hr ) )
	{
		m_bAnalyseRequired = false;
	}
	::ReleaseDC( GetEtDevice()->GetHWnd(), hDC );

	return hr;
}

bool CEtUIUniBuffer::RemoveCharThai( int nIndex )
{
	int nBufferSize = GetTextSize();
	if( nIndex < 0 || nIndex >= nBufferSize )
		return false;

	const WCHAR* pBuffer = GetBuffer();
	if( pBuffer == NULL ) return false;

	if( pBuffer[nIndex] >= CEtUIEditBox::GetThaiStartChar() )
	{
		int nVowels = 0;
		for( int i=nIndex+1; i<nBufferSize; i++ )
		{
			int nTextIndex = CEtUIEditBox::CheckThaiLanguageVowel( pBuffer[i] );
			if( nTextIndex == -1 || nTextIndex < CEtUIEditBox::GetThaiBelowVowelStartIndex() )
				break;
			else
				nVowels++;
		}

		if( ( !lstrlenW( m_pwszBuffer ) ) || ( nIndex < 0 ) || ( nIndex >= lstrlenW( m_pwszBuffer ) ) )
		{
			return false;
		}
		memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof( WCHAR ) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );

		for( int i=0; i<nVowels; i++ )
		{
			if( ( !lstrlenW( m_pwszBuffer ) ) || ( nIndex < 0 ) || ( nIndex >= lstrlenW( m_pwszBuffer ) ) )
			{
				return false;
			}
			memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof( WCHAR ) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );
		}
	}
	else
	{
		if( ( !lstrlenW( m_pwszBuffer ) ) || ( nIndex < 0 ) || ( nIndex >= lstrlenW( m_pwszBuffer ) ) )
		{
			return false;
		}
		memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof( WCHAR ) * ( lstrlenW( m_pwszBuffer ) - nIndex ) );
	}

	return true;
}

bool CEtUIUniBuffer::SetTextThai( LPCWSTR wszText )
{
	int nTextSize = (int)wcslen( wszText );
	int nBufferThaiLength = CEtUIEditBox::GetThaiLength( wszText );
	int nMaxEditBoxLength = GetMaxEditBoxLength();
	if( nBufferThaiLength + 1 > nMaxEditBoxLength )
	{
		return false;
	}

	int nRequiredBuffer;
	nRequiredBuffer = nTextSize + 2;
	int nBufferSize = GetBufferSize();

	if( nBufferSize < nRequiredBuffer )
	{
		WCHAR* pTempBuffer( NULL );
		pTempBuffer = new WCHAR[ nRequiredBuffer ];
		WCHAR* pBuffer = GetModifyBuffer();
		if( pBuffer )
		{
			memcpy( pTempBuffer, pBuffer, sizeof( WCHAR ) * nBufferSize );
			SAFE_DELETE_ARRAY( pBuffer );
		}
		else
		{
			memset( pTempBuffer, 0, sizeof( WCHAR ) * nRequiredBuffer );
		}

		SetBuffer( pTempBuffer, nRequiredBuffer );
	}
	if( GetBufferSize() >= nRequiredBuffer )
	{
		wcscpy_s( GetModifyBuffer(), GetBufferSize(), wszText );
		SetAnalyseRequired( true );
		return true;
	}

	return false;
}

bool CEtUIUniBuffer::IsEnableInsertCharThai( int nCaret, WCHAR wChar ) const
{
	int nBufferSize = GetTextSize();
	if( ( nCaret < 0 ) || ( nCaret > nBufferSize ) )
	{
		return false;
	}

	int nCharIndex = CEtUIEditBox::CheckThaiLanguageVowel( wChar );
	if( nCharIndex != -1 && nCharIndex >= CEtUIEditBox::GetThaiBelowVowelStartIndex() )	// 위/아래모음 인 경우
		return true;

	int nBufferThaiLength = CEtUIEditBox::GetThaiLength( GetBuffer() );
	int nMaxEditBoxLength = GetMaxEditBoxLength();
	if( nBufferThaiLength + 1 >= nMaxEditBoxLength )
	{
		return false;
	}

	return true;
}

bool CEtUIUniBuffer::IsEnableInsertStringThai( int nIndex, const WCHAR *pStr ) const
{
	int nBufferSize = GetTextSize();
	if( nIndex > nBufferSize )
	{
		return false;
	}

	int nBufferThaiLength = CEtUIEditBox::GetThaiLength( GetBuffer() );
	int nAddTextThaiLength = CEtUIEditBox::GetThaiLength( pStr );

	int nMaxEditBoxLength = GetMaxEditBoxLength();
	if(nBufferThaiLength + nAddTextThaiLength >= nMaxEditBoxLength )
	{
		return false;
	}

	return true;
}

bool CEtUIUniBuffer::InsertCharThai( int nIndex, WCHAR wChar )
{
	int nTextSize = GetTextSize();
	int nBufferSize = GetBufferSize();
	int nRequiredBuffer = nTextSize + 2;

	if( nBufferSize < nRequiredBuffer )
	{
		WCHAR* pTempBuffer( NULL );
		pTempBuffer = new WCHAR[ nRequiredBuffer ];
		WCHAR* pBuffer = GetModifyBuffer();
		memset( pTempBuffer, 0, sizeof( WCHAR ) * nRequiredBuffer );

		if( pBuffer )
		{
			memcpy( pTempBuffer, pBuffer, sizeof( WCHAR ) * nTextSize );
			SAFE_DELETE_ARRAY( pBuffer );
		}

		SetBuffer( pTempBuffer, nRequiredBuffer );
	}

	WCHAR* pBuffer = GetModifyBuffer();
	WCHAR *pwszDest, *pwszStop, *pwszSour;
	pwszDest = pBuffer + lstrlenW( pBuffer ) + 1;
	pwszStop = pBuffer + nIndex;
	pwszSour = pwszDest - 1;

	while( pwszDest > pwszStop )
	{
		*pwszDest-- = *pwszSour--;
	}
	pBuffer[ nIndex ] = wChar;
	SetAnalyseRequired( true );

	return true;
}

bool CEtUIUniBuffer::InsertStringThai( int nIndex, const WCHAR *pStr )
{
	if( pStr == NULL )
		return false;

	int nCount = lstrlenW( pStr );
	int nTextSize = GetTextSize();
	int nBufferSize = GetBufferSize();
	int nRequiredBuffer = nTextSize + nCount + 1;

	if( nBufferSize < nRequiredBuffer )
	{
		WCHAR* pTempBuffer( NULL );
		pTempBuffer = new WCHAR[ nRequiredBuffer ];
		WCHAR* pBuffer = GetModifyBuffer();
		memset( pTempBuffer, 0, sizeof( WCHAR ) * nRequiredBuffer );

		if( pBuffer )
		{
			memcpy( pTempBuffer, pBuffer, sizeof( WCHAR ) * nTextSize );
			SAFE_DELETE_ARRAY( pBuffer );
		}

		SetBuffer( pTempBuffer, nRequiredBuffer );
	}

	memmove( GetModifyBuffer() + nIndex + nCount, GetModifyBuffer() + nIndex, sizeof( WCHAR ) * ( lstrlenW( GetModifyBuffer() ) - nIndex + 1 ) );
	memcpy( GetModifyBuffer() + nIndex, pStr, nCount * sizeof( WCHAR ) );
	SetAnalyseRequired( true );

	return true;
}