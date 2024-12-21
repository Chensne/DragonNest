#pragma once

namespace CDebugSet
{
	//	Note	:	파일로 화면에 뿌림
	//	Usage	:	파일명을 제일 앞에 쓰고, 디버그 정보를 printf ()를 사용하듯이 파라미터를 넘김
	void ToLogFile ( const char *szFormat, ... );
	void ToFile ( const char *szFileName, const char *szFormat, ... );
	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... );
	void ClearFile ( const char *szFileName );

	const char* GetPath ();
	std::string GetCurTime ();

	//	Note	:	디버그정보를 파일로 출력할 때, 절대경로를 설정해야 한다.
	//
	bool Initialize( const char *pszFullDir, bool bLogFileFinalOpen = false );
	bool Finalize();
};