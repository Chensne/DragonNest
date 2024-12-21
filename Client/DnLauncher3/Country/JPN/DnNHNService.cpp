#include "stdafx.h"
#include "DnNHNService.h"
//#include "StringUtil.h"

#ifdef _JPN

#include "../../Extern/NHNPurple/HanAuthForClient/HanAuthForClient.h"

//������ �ʱ�ȭ ������ ������ ���� ���Ŀ��� �����°��� wchar���̴�
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

	//�Ѱ��� ��� �ʱ�ȭ
	iRet = HanAuthInitGameString( m_szGameStringOld );

	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

	// ���޵� ���ӽ�Ʈ���� ���ο� �ִ� ���Ʈ���� ��⿡ ���� ����
	iRet = HanAuthForClientGameString( m_szGameStringOld );
	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );

	char szOutMemberID[30] = {0,};
	iRet = HanAuthGetId( m_szGameStringOld, m_szMemberID, sizeof(m_szMemberID) );

	if( iRet != HAN_AUTHCLI_OK )
		return ErrorMsg( iRet );
	
	// ���������� Patch �� Ư���ð��� ���������� ������Ʈ���� ���÷��� ���Ŀ��� �ѹ��� ����
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
	// GetGameString �Ҵ�� �޵�� UpdateString�� ���ľ� �Ѵ� .. 
	// ���ο� ��Ʈ������ Ŭ�������� ������ 
	// ���� ���İ� ��ġ �������� m_szGameStringOld �� �ٽ� ������.

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
	// ���������� Patch �� Ư���ð��� ���������� ������Ʈ���� ���÷��� ���Ŀ��� �ѹ��� ����
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
		case HAN_AUTHCLI_OK:					str =	_T( "����" ); return TRUE;
		case HAN_AUTHCLI_ARGUMENT_INVALID:		str =	_T( "���޵� ���ڰ� �ùٸ��� �ʽ��ϴ�. " ) ;break; 
		case HAN_AUTHCLI_INITED_NOT :			str =	_T( "Ŭ���̾�Ʈ ����� �ʱ�ȭ ���� �ʾҽ��ϴ�. " ); break;
		case HAN_AUTHCLI_INITED_FAIL :			str =	_T( "��� �ʱ�ȭ�� �����߽��ϴ� " ); break;
		case HAN_AUTHCLI_AUTHHTTP_INITFAIL:		str =	_T( "HTTP �ʱ�ȭ�� �����߽��ϴ�. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_CONNFAIL :	str =	_T( "HTTP ���ῡ �����߽��ϴ�." ); break;
		case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:	str =	_T( "HTTP�ʱ�ȭ(refresh)�� �����߽��ϴ�. " ); break;
		case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:	str =	_T( " HTTP����(refresh)�� �����߽��ϴ�." ); break;
		case HAN_AUTHCLI_NOT_IMPLEMENTED:		str =	_T( "���� �̱����� ����Դϴ�. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:	str =	_T( "HTTP ������Ʈ ���¿� �����߽��ϴ�. " ); break;
		case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL : str =	_T( "������Ʈ send�� �����߽��ϴ�." ); break;
		case HAN_AUTHCLI_COOKIE_SETFAIL	:		str =	_T( "��Ű ���ÿ� �����߽��ϴ�." ); break;
		case HAN_AUTHCLI_GAMESTRING_IDINVALID:	str =	_T( "���޵� ���ӽ�Ʈ���� �ùٸ��� �ʽ��ϴ�. " ); break;
		case HAN_AUTHCLI_GAMESTRING_USNINVALID :str =	_T( "USN�� �ùٸ��� �ʽ��ϴ�. " ); break;
		case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID: str =_T(" GAMEID�� �ùٸ��� �ʽ��ϴ�." ); break;
		case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID: str = _T("servicecode invalid!!"); break; // str = _T( " servicecode�� �ùٸ��� �ʽ��ϴ�." ); break;
		case HAN_AUTHCLI_DLL_UNEXPECTED : str =			_T( " 	����ġ���� ������ �߻��߽��ϴ�." ); break;
		case HAN_AUTHCLI_PARAM_INVALID 	: str =			_T( "�������� ���޵� ���ڰ� �������մϴ�. " ); break;
		case HAN_AUTHCLI_IPADDR_INVALID	: str =			_T( "�������ּҰ� Ʋ���ϴ�. " ); break;
	}	

	MessageBox( NULL, str.c_str(), _T("NHN Error"), MB_OK );

	return FALSE;
}

#endif // _JPN 