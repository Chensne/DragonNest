#include "stdafx.h"
#include "CountryByCountryKeyboard.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CCountryByCountryKeyboard::CCountryByCountryKeyboard()
: m_nKeyboardType( CCountryByCountryKeyboard::eQWERTY_KEYBOARD )
{

}

CCountryByCountryKeyboard::~CCountryByCountryKeyboard()
{

}

void CCountryByCountryKeyboard::ChangeCountryByKeyboard( const int nKeyboardType, WCHAR ** wszKeyString )
{
	m_nKeyboardType = nKeyboardType;

	if( CCountryByCountryKeyboard::eAZERTY_KEYBOARD == m_nKeyboardType )
		ChangeAzertyKeyString( wszKeyString );
	else if( CCountryByCountryKeyboard::eQWERTZ_KEYBOARD == m_nKeyboardType )
		ChangeQwertzKeyString( wszKeyString );
	else if( CCountryByCountryKeyboard::eSPAIN_KEYBOARD == m_nKeyboardType )
		ChangeSpainKeyString( wszKeyString );
	else
		ChangeOriginalKeyString( wszKeyString );
}

DWORD CCountryByCountryKeyboard::ChangeKey( const DWORD dwOfs )
{
	DWORD retValue = dwOfs;

	if( CCountryByCountryKeyboard::eAZERTY_KEYBOARD == m_nKeyboardType )
		retValue = ChangeAzertyKey( dwOfs );
	else if( CCountryByCountryKeyboard::eQWERTZ_KEYBOARD == m_nKeyboardType )
		retValue = ChangeQwertzKey( dwOfs );
	else if( CCountryByCountryKeyboard::eSPAIN_KEYBOARD == m_nKeyboardType )
		retValue = ChangeSpainKey( dwOfs );

	return retValue;
}

DWORD CCountryByCountryKeyboard::ChangeAzertyKey( const DWORD dwOfs )
{
	DWORD retValue = dwOfs;

	switch( dwOfs )
	{
	case DIK_Q:
		retValue = DIK_A;
		break;

	case DIK_A:
		retValue = DIK_Q;
		break;

	case DIK_W:
		retValue = DIK_Z;
		break;

	case DIK_Z:
		retValue = DIK_W;
		break;

	case DIK_GRAVE:
		retValue = DIK_APOSTROPHE;
		break;

	case DIK_APOSTROPHE:
		retValue = DIK_GRAVE;
		break;

	case DIK_COMMA:
		retValue = DIK_PERIOD;
		break;

	case DIK_PERIOD:
		retValue = DIK_SLASH;
		break;

	case DIK_SEMICOLON:
		retValue = DIK_M;
		break;

	case DIK_M:
		retValue = DIK_COMMA;
		break;
	}

	return retValue;
}

DWORD CCountryByCountryKeyboard::ChangeQwertzKey( const DWORD dwOfs )
{
	DWORD retValue = dwOfs;

	switch( dwOfs )
	{
	case DIK_Y:
		retValue = DIK_Z;
		break;

	case DIK_Z:
		retValue = DIK_Y;
		break;

	case DIK_SEMICOLON:
		retValue = DIK_GRAVE;
		break;

	case DIK_LBRACKET:
		retValue = DIK_SEMICOLON;
		break;

	case DIK_BACKSLASH:
		retValue = DIK_SLASH;
		break;

	case DIK_GRAVE:
		retValue = DIK_BACKSLASH;
		break;
	}

	return retValue;
}

DWORD CCountryByCountryKeyboard::ChangeSpainKey( const DWORD dwOfs )
{
	DWORD retValue = dwOfs;

	switch( dwOfs )
	{
	case DIK_LBRACKET:
		retValue = DIK_SEMICOLON;
		break;

	case DIK_SEMICOLON:
		retValue = DIK_GRAVE;
		break;

	case DIK_BACKSLASH:
		retValue = DIK_SLASH;
		break;

	case DIK_GRAVE:
		retValue = DIK_BACKSLASH;
		break;
	}

	return retValue;
}

void CCountryByCountryKeyboard::ChangeAzertyKeyString( WCHAR ** wszKeyString )
{
	wszKeyString[39] = L"$";
	wszKeyString[40] = L"²";
	wszKeyString[41] = L"ù";
	wszKeyString[43] = L"*";
	wszKeyString[52] = L";";
	wszKeyString[53] = L":";
}

void CCountryByCountryKeyboard::ChangeQwertzKeyString( WCHAR ** wszKeyString )
{
	wszKeyString[39] = L"ü";
	wszKeyString[40] = L"ä";
	wszKeyString[41] = L"ö";
	wszKeyString[43] = L"^^";
	wszKeyString[53] = L"#";
}

void CCountryByCountryKeyboard::ChangeSpainKeyString( WCHAR ** wszKeyString )
{
	wszKeyString[39] = L"`";
	wszKeyString[40] = L"´";
	wszKeyString[41] = L"ñ";
	wszKeyString[43] = L"º";
	wszKeyString[53] = L"ç";
}

void CCountryByCountryKeyboard::ChangeOriginalKeyString( WCHAR ** wszKeyString )
{
	wszKeyString[39] = L";";
	wszKeyString[40] = L"";
	wszKeyString[41] = L"~";
	wszKeyString[43] = L"\\";
	wszKeyString[52] = L".";
	wszKeyString[53] = L"/";
}