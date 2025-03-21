#pragma once

class WinRegistry
{
	
public:
	
    HKEY    hKey;     
	char    RegPath[400];

	WinRegistry()
	{
		hKey = NULL;
	}

	// Set path
	void SetRegistryPath( char* NewPath)
	{
		strcpy( RegPath, NewPath );
	}

	void SetKeyString( char* KeyName, void* ValString )
	{
		DWORD ValSize = strlen( (char*)ValString ) + 1;

		DWORD Res;
		LONG KeyError;
		KeyError = ::RegCreateKeyEx( HKEY_CURRENT_USER, (char*)&RegPath, 0L, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &Res );
		if( KeyError == ERROR_SUCCESS ) 
		{
			::RegSetValueEx( hKey, KeyName, 0L, REG_SZ, (CONST BYTE*)ValString, ValSize ); //#DEBUG size
			::RegCloseKey( hKey );
			hKey = 0;
		}
		else
		{
			//PopupBox("SetKeyError: %i", KeyError);
		}
	}

	void GetKeyString( char* KeyName, void* ValString )
	{
		*(char*)ValString = '\0';

		LONG  KeyError;		    
		KeyError = ::RegOpenKeyEx( HKEY_CURRENT_USER, (const char*)&RegPath, 0L, KEY_READ, &hKey );
		if( KeyError == ERROR_SUCCESS ) 
		{
			DWORD Type;
			DWORD DataLen = 300; //strlen( (char*)ValString )+1;
			::RegQueryValueEx( hKey, 
				               KeyName, 
				               0, 
				               &Type, 
							   (byte*) ValString, //reinterpret_cast<unsigned char*>( ValString ),
							   &DataLen );
			::RegCloseKey( hKey );
			hKey = 0;

			//PopupBox("string length: %i datalen: %i string: [%s]", strlen((char*)ValString), DataLen, ValString);
		}
		else
		{
			//PopupBox("GetKeyError: %i",KeyError);
		}
	}

	void SetKeyValue( char* KeyName, DWORD Value ) // set any 32-bit value...
	{
		DWORD Res;
		LONG KeyError;
		DWORD Val = Value;
		KeyError = ::RegCreateKeyEx( HKEY_CURRENT_USER, (char*)&RegPath, 0L, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &Res );
		if( KeyError == ERROR_SUCCESS ) 
		{
			::RegSetValueEx( hKey, KeyName, 0L, REG_DWORD, (CONST BYTE*)&Val, sizeof(DWORD) ); //#DEBUG size
			::RegCloseKey( hKey );
			hKey = 0;
		}
		else
		{
			//PopupBox("SetKeyError: %i",KeyError);
		}
	}

	void SetKeyValue( char* KeyName, FLOAT FloatValue ) // FLOAT overload
	{
		SetKeyValue( KeyName, *((DWORD*)&FloatValue) );
	}

	void SetKeyValue( char* KeyName, BOOL UBOOLValue ) // UBOOL overload
	{
		SetKeyValue( KeyName, *((DWORD*)&UBOOLValue) );
	}

	void GetKeyValue( char* KeyName, INT& Value )    // set any 32-bit value...
	{

		LONG  KeyError;		    
		KeyError = ::RegOpenKeyEx( HKEY_CURRENT_USER, (const char*)&RegPath, 0L, KEY_READ, &hKey );
		if( KeyError == ERROR_SUCCESS ) 
		{
			DWORD Type;
			DWORD DataLen = sizeof(DWORD);
			::RegQueryValueEx( hKey, 
				               KeyName, 
				               0, 
				               &Type, 
							   (byte*) &Value,
							   &DataLen );
			::RegCloseKey( hKey );
			hKey = 0;
		}
		else
		{			
			//PopupBox("GetKeyError: %i",KeyError);
		}
	}

	void GetKeyValue( char* KeyName, FLOAT& FloatValue ) // FLOAT overload
	{
		GetKeyValue( KeyName, *((INT*)&FloatValue) );
	}

	//void GetKeyValue( char* KeyName, UBOOL& UBOOLValue ) // UBOOL overload
	//{
	//		GetKeyValue( KeyName, *((INT*)&UBOOLValue) );
	//	}


	// Remove settings 
	void DeleteRegistryKey( const char* KeyName )
	{
		::RegDeleteKey( hKey, KeyName );
		::RegCloseKey( hKey );
	}
	
};


int _GetCheckBox( HWND hWnd, int CheckID );
int _SetCheckBox( HWND hWnd,int CheckID, int Switch );
int GetTextureChannelCount( Mtl *pMtl );
