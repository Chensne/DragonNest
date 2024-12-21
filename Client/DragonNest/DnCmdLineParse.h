#pragma once

extern enum CmdLineIndexEnum;

class CDnCmdLineParse {
public:
	CDnCmdLineParse( LPTSTR szString );
	~CDnCmdLineParse();

	struct CmdLineDefineStruct {
		CmdLineIndexEnum Index;
		TCHAR *szString;
	};

	void DetectedCommand( CmdLineIndexEnum Index );

protected:
	TCHAR m_szParam[128];
	TCHAR m_szCmdLine[2048];

	void ProcessParse( TCHAR *szString );
	bool FindString( TCHAR *szString );
	void ChangeLowerCase( TCHAR *szString );
};
