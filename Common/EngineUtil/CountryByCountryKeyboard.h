#pragma once

class CCountryByCountryKeyboard : public CSingleton<CCountryByCountryKeyboard>
{
public :
	enum{
		eQWERTY_KEYBOARD,
		eAZERTY_KEYBOARD,
		eQWERTZ_KEYBOARD,
		eSPAIN_KEYBOARD,
	};

public:
	CCountryByCountryKeyboard();
	virtual ~CCountryByCountryKeyboard();

protected:
	int m_nKeyboardType;

public:
	void ChangeCountryByKeyboard( const int nKeyboardType, WCHAR ** wszKeyString );
	DWORD ChangeKey( const DWORD dwOfs );

protected:
	DWORD ChangeAzertyKey( const DWORD dwOfs );
	DWORD ChangeQwertzKey( const DWORD dwOfs );
	DWORD ChangeSpainKey( const DWORD dwOfs );

	void ChangeAzertyKeyString( WCHAR ** wszKeyString );
	void ChangeQwertzKeyString( WCHAR ** wszKeyString );
	void ChangeSpainKeyString( WCHAR ** wszKeyString );
	void ChangeOriginalKeyString( WCHAR ** wszKeyString );
};

#define GetCountryByCountryKeyboard()	CCountryByCountryKeyboard::GetInstance()