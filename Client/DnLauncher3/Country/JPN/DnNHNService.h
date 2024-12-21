#pragma once

#include "Singleton.h"

#ifdef _JPN

class DnNHNService : public CSingleton<DnNHNService>
{
public:
	DnNHNService();
	virtual ~DnNHNService();

	//런쳐에서 사용하는것.. 
	bool UpdateInfo( CString cmdline );
	bool ErrorMsg( int _Error );		//Error 메세지 출력 
	bool UpdateString();				//패치가 끝나면 이함수를 호출
	void SetGameString( char *pString );
	bool Refresh();
	bool CheckChannelingMS();

	//클라로 송출부분 wchar 로 변경시킨다 
	WCHAR *GetGameString();
	WCHAR *GetMemberID();
	WCHAR *GetUserID();

private:
	char m_szGameStringOld[4096];
	char m_szGameStringNew[2048];
	char m_szMemberID[30];
	char m_szUserID[30];
};

#endif // _JPN