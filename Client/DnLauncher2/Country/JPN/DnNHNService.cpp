#include "stdafx.h"
#include "DnNHNService.h"
//#include "StringUtil.h"

#ifdef _JPN

#include "../../Extern/NHNPurple/HanAuthForClient/HanAuthForClient.h"

//퍼플의 초기화 문제로 사용안함 현재 런쳐에서 보내는값은 wchar값이다
//#include "../Extern/NHNPurple/Purpleforclient/PurpleForClient.h"

DnNHNService::DnNHNService(void)
{
	ZeroMemory( m_szGameStringOld , sizeof(m_szGameStringOld ));
	ZeroMemory( m_szGameStringNew, sizeof(m_szGameStringNew));
	ZeroMemory( m_szMemberID , sizeof(m_szMemberID)) ;
	ZeroMemory( m_szUserID , sizeof(m_szUserID)) ;
}
  
DnNHNService::~DnNHNService(void)
{
}

bool DnNHNService::UpdateInfo( CString Cmdline )
{
	int iRet = HAN_AUTHCLI_OK; 

	WideCharToMultiByte( CP_ACP, 0, Cmdline.GetBuffer(), -1, m_szGameStringOld, sizeof(m_szGameStringOld), NULL, NULL );

	//한게임 모듈 초기화
	iRet = HanAuthInitGameString( m_szGameStringOld );

	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

	// 전달된 게임스트링의 내부에 있는 인즈스트링을 모듈에 내에 저장
	iRet = HanAuthForClientGameString( m_szGameStringOld );
	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

	char szOutMemberID[30] = {0,};
	iRet = HanAuthGetId( m_szGameStringOld, m_szMemberID, sizeof(m_szMemberID) );

	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );
	
	// 내부적으로 Patch 중 특정시간이 지날때마다 인증스트링을 리플래쉬 런쳐에서 한번만 실행
	iRet = HanAuthRefresh( szOutMemberID );

	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

//	UpdateString();

	return TRUE;
}

bool DnNHNService::UpdateString()
{
	int iRet =0;

	iRet = UpdateGameString( m_szGameStringOld, m_szGameStringNew, sizeof( m_szGameStringNew ) );
	
	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

	return TRUE;
}
 
WCHAR* DnNHNService::GetGameString()
{
	// GetGameString 할대는 받드시 UpdateString을 거쳐야 한다 .. 
	// 새로운 스트링값을 클라쪽으로 보내고 
	// 만약 런쳐가 패치 됐을때는 m_szGameStringOld 를 다시 보낸다.

 	static WCHAR szGameStringNew[2048] = {0,};
	MultiByteToWideChar( CP_ACP, 0, m_szGameStringNew, -1, szGameStringNew, sizeof( szGameStringNew ) );

	return szGameStringNew;
}

WCHAR* DnNHNService::GetUserID()
{
	static WCHAR szUserID[30] = {0,};

	MultiByteToWideChar( CP_ACP, 0, m_szUserID, -1, szUserID, sizeof( szUserID ) );

	return szUserID;
}

WCHAR* DnNHNService::GetMemberID()
{
	static WCHAR szMemberID[30] = {0,};
	
	MultiByteToWideChar( CP_ACP, 0, m_szMemberID, -1, szMemberID, sizeof( szMemberID ) );

	return szMemberID;
}

bool DnNHNService::Refresh()
{
	// 내부적으로 Patch 중 특정시간이 지날때마다 인증스트링을 리플래쉬 런쳐에서 한번만 실행
	int iRet = HAN_AUTHCLI_OK ; 

	iRet = HanAuthRefresh( m_szMemberID );

	if( iRet != HAN_AUTHCLI_OK )
	{
		return ErrorMsg( iRet );
	}

	return TRUE; 
}

bool DnNHNService::CheckChannelingMS()
{
	char szValue[256] = {0,};
	HanAuthGetValue( m_szGameStringOld, "site", szValue, sizeof( szValue ) );

	bool bCheckMS = false;
	if( strcmp( szValue, "MS" ) == 0 )
		bCheckMS = true;

	return bCheckMS;
}

void DnNHNService::SetGameString( char *pString )
{
	strcpy( m_szGameStringOld, pString );
}

bool DnNHNService::ErrorMsg( int _Error )
{
	std::wstring str; 

	switch( _Error )
	{
		case HAN_AUTHCLI_OK:					str =	_T( "성공" ); return TRUE;
		case HAN_AUTHCLI_ARGUMENT_INVALID:		str =	_T( "전달된 인자가 올바르지 않습니다. " ) ;break; 
		case HAN_AUTHCLI_INITED_NOT :			str =	_T( "클라이언트 모듈이 초기화 되지 않았습니다. " ); break;
		case HAN_AUTHCLI_INITED_FAIL :			str =	_T( "모듈 초기화에 실패했습니다 " ); break;
		case HAN_AUTHCLI_AUTHHTTP_INITFAIL:		str =	_T( "HTTP 초기화에 실패했습니다. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_CONNFAIL :	str =	_T( "HTTP 연결에 실패했습니다." ); break;
		case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:	str =	_T( "HTTP초기화(refresh)에 실패했습니다. " ); break;
		case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:	str =	_T( " HTTP연결(refresh)에 실패했습니다." ); break;
		case HAN_AUTHCLI_NOT_IMPLEMENTED:		str =	_T( "아직 미구현된 기능입니다. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:	str =	_T( "HTTP 리퀘스트 오픈에 실패했습니다. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL : str =	_T( "리퀘스트 send를 실패했습니다." ); break;
		case HAN_AUTHCLI_COOKIE_SETFAIL	:		str =	_T( "쿠키 세팅에 실패했습니다." ); break;
		case HAN_AUTHCLI_GAMESTRING_IDINVALID:	str =	_T( "전달된 게임스트링이 올바르지 않습니다. " ); break;
		case HAN_AUTHCLI_GAMESTRING_USNINVALID :str =	_T( "USN이 올바르지 않습니다. " ); break;
		case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID: str =_T(" GAMEID가 올바르지 않습니다." ); break;
		case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID: str = _T("servicecode invalid!!"); break; // str = _T( " servicecode가 올바르지 않습니다." ); break;
		case HAN_AUTHCLI_DLL_UNEXPECTED : str =			_T( " 	예상치못한 에러가 발생했습니다." ); break;
		case HAN_AUTHCLI_PARAM_INVALID 	: str =			_T( "웹서버로 전달된 인자가 부적절합니다. " ); break;
		case HAN_AUTHCLI_IPADDR_INVALID	: str =			_T( "아이피주소가 틀립니다. " ); break;
	}	

	MessageBox( NULL, str.c_str(), _T("NHN Error"), MB_OK );

	return FALSE;
}

#endif // _JPN 