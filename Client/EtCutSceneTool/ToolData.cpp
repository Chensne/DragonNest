#include "stdafx.h"
#include <wx/wx.h>
#include "ToolData.h"
#include "CmdProcessor.h"
#include "CommandSet.h"
#include "SundriesFunc.h"
#include "DnCommonUtil.h"
#include <wx/strconv.h>
#include <wx/FileName.h>
#include "cwxPropertyPanel.h"
#include "RTCutSceneMakerFrame.h"
#include "LuaDelegate.h"
#include "EtUIXML.h"
#include <wx/msgdlg.h>




CToolData::CToolData(void) :  m_pCoreData( new CDnCutSceneData ),
							  m_pCmdProcessor( new CCmdProcessor ),
							  m_pActorTable( NULL ),
							  m_pMapTable( NULL ),
							  m_pPropertyPanel( NULL ),
							  m_pMonsterTable( NULL ),
							  m_pMainFrame( NULL ),
							  m_bModified( false ),
							  m_iNowSelectedRegRes( wxNOT_FOUND ),
							  m_pRenderer( NULL ),
							  m_iSelectedObjectID( wxNOT_FOUND ),
							  m_iSelectedObjectUseType( wxNOT_FOUND ),
							  m_iEditMode( EM_COUNT ),
							  m_pActionTimeLineCtrl( NULL ),
							  m_pEventTLPanel( NULL ),
							  m_dwLocalTime( 0 ),
							  m_bEdited( false ),
							  m_bWritable( true ),
							  m_pActionInfoClipboard( NULL ),
							  m_pKeyInfoClipboard( NULL ),
							  m_pEventInfoClipboard( NULL )
{
	if( !GetEtUIXML().LoadXML( "uistring.xml", CEtUIXML::idCategory1 ) ) 
		assert( !"UIString.xml 로딩 에러");

	ZeroMemory( m_caBuf, sizeof(m_caBuf) );

	const char* pResourcePath = LUA_DELEGATE.GetString( "ext_path" );
	string strEXTPath( pResourcePath );

	m_pActorTable = new DNTableFileFormat;
	m_pActorTable->Load( string(strEXTPath+"/"+"ActorTable.dnt").c_str() );

	m_pMapTable = new DNTableFileFormat;
	m_pMapTable->Load( string(strEXTPath+"/"+"MapTable.dnt").c_str() );

	m_pWeatherTable = new DNTableFileFormat;
	m_pWeatherTable->Load( string(strEXTPath+"/"+"WeatherTable.dnt").c_str() );

	m_pFaceAniTable = new DNTableFileFormat;
	m_pFaceAniTable->Load( string(strEXTPath+"/"+"FaceTable.dnt").c_str() );

	m_pWeaponTable = new DNTableFileFormat;
	m_pWeaponTable->Load( string(strEXTPath+"/"+"WeaponTable.dnt").c_str() );

	m_pFileTable = new DNTableFileFormat;
	m_pFileTable->Load( string(strEXTPath+"/"+"FileTable.dnt").c_str() );

	m_pMonsterTable = new DNTableFileFormat;
	m_pMonsterTable->SetGenerationInverseLabel( "_ActorTableID" );
	m_pMonsterTable->Load( string(strEXTPath+"/"+"MonsterTable.dnt").c_str() );

	m_pNPCTable = new DNTableFileFormat;
	m_pNPCTable->SetGenerationInverseLabel( "_ActorIndex" );
	m_pNPCTable->Load( string(strEXTPath+"/"+"NPCTable.dnt" ).c_str() );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];

	char caActionFileName[ 256 ];
	char caAniFileName[ 256 ];
	char caSkinFileName[ 256 ];
	int iNumItem = m_pActorTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem; ++iItem )
	{
		int iItemID = m_pActorTable->GetItemID( iItem );
		const char* pActorName = m_pActorTable->GetFieldFromLablePtr( iItemID, "_StaticName" )->GetString();

		ZeroMemory( caActionFileName, sizeof(caActionFileName) );
		//const char* pActionFilePath = m_pActorTable->GetFieldFromLablePtr( iItemID, "_ActName" )->GetString();
		const char* pActionFilePath	= CommonUtil::GetFileNameFromFileEXT( m_pActorTable, iItemID, "_ActName", m_pFileTable );
		if( pActionFilePath ) 
			_splitpath( pActionFilePath, NULL, NULL, caActionFileName, NULL );

		ZeroMemory( caAniFileName, sizeof(caAniFileName) );
		//const char* pAniFilePath = m_pActorTable->GetFieldFromLablePtr( iItemID, "_AniName" )->GetString();
		const char* pAniFilePath = CommonUtil::GetFileNameFromFileEXT( m_pActorTable, iItemID, "_AniName", m_pFileTable );
		if( pAniFilePath )
			_splitpath( pAniFilePath, NULL, NULL, caAniFileName, NULL );
		
		ZeroMemory( caSkinFileName, sizeof(caSkinFileName) );
		//const char* pSkinFilePath = m_pActorTable->GetFieldFromLablePtr( iItemID, "_SkinName" )->GetString();
		const char* pSkinFilePath = CommonUtil::GetFileNameFromFileEXT( m_pActorTable, iItemID, "_SkinName", m_pFileTable );

		// 주인공들이라면 주인공들의 이름을 그대로 사용
		if( pSkinFilePath )
		{
			if( strcmp( pActorName, "Warrior" ) == 0 ||
				strcmp( pActorName, "Archer" ) == 0 ||
				strcmp( pActorName, "Soceress" ) == 0 ||
				strcmp( pActorName, "Cleric" ) == 0 ||
				strcmp( pActorName, "Academic" ) == 0 ||
				strcmp( pActorName, "Kali" ) == 0 )
			{
				strcpy_s( caSkinFileName, pActorName );
			}
			else
				_splitpath( pSkinFilePath, NULL, NULL, caSkinFileName, NULL );
		}

		if( strcmp(pActorName, "") != 0 )
		{
			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, pActorName, 256 );

			S_ACTOR_RES_INFO* pActorResInfo = new S_ACTOR_RES_INFO;
			pActorResInfo->strActorName.assign( caWBuf );
			pActorResInfo->strActorName.MakeUpper();

			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, caActionFileName, 256 );
			pActorResInfo->strActionFileName.assign( caWBuf );
			pActorResInfo->strActionFileName.append( wxT(".") );
			pActorResInfo->strActionFileName.append( wxT("act") );
			pActorResInfo->strActionFileName.MakeLower();

			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, caAniFileName, 256 );
			pActorResInfo->strAniFileName.assign( caWBuf );
			pActorResInfo->strAniFileName.append( wxT(".") );
			pActorResInfo->strAniFileName.append( wxT("ani") );
			pActorResInfo->strAniFileName.MakeLower();

			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, caSkinFileName, 256 );
			pActorResInfo->strSkinFileName.assign( caWBuf );
			pActorResInfo->strSkinFileName.append( wxT(".") );
			pActorResInfo->strSkinFileName.append( wxT("skn") );
			pActorResInfo->strSkinFileName.MakeLower();

			pActorResInfo->iTableID = iItemID;
			
			m_vlpActorResInfo.push_back( pActorResInfo );

			m_mapActorResInfo.insert( make_pair( wxString(pActorResInfo->strActorName), pActorResInfo ) );
			//m_vlpActorResInfo.back().MakeUpper();
		}
	}

	iNumItem = m_pFaceAniTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem;++iItem )
	{
	
	}

	iNumItem = m_pMonsterTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem; ++iItem )
	{
		int iItemID = m_pMonsterTable->GetItemID( iItem );

		// 보스 뿐만 아니라 작은 몬스터도 있어서 일반 몹까지 전부 포함.
		//if( Boss == m_pMonsterTable->GetFieldFromLablePtr( iItemID, "_Grade" )->GetInteger() )
		//{
			S_MONSTER_INFO* pNewBossMonsterInfo = new S_MONSTER_INFO;

			pNewBossMonsterInfo->iMonsterTableID = iItemID;
			pNewBossMonsterInfo->iActorTableID = m_pMonsterTable->GetFieldFromLablePtr( iItemID, "_ActorTableID" )->GetInteger();
			pNewBossMonsterInfo->m_fScale = m_pMonsterTable->GetFieldFromLablePtr( iItemID, "_SizeMax" )->GetFloat();

			wstring wszMonsterName;
			int iNameID = m_pMonsterTable->GetFieldFromLablePtr( iItemID, "_NameID" )->GetInteger();
			char *szParam = m_pMonsterTable->GetFieldFromLablePtr( iItemID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszMonsterName, iNameID, szParam );
			pNewBossMonsterInfo->strName = wszMonsterName.c_str();

			m_vlpBossMonsterInfo.push_back( pNewBossMonsterInfo );
		//}
	}
}



CToolData::~CToolData(void)
{
	SaveAsToFile( wxT("R:/GameRes/Resource/Dmv/LastWorkFile.dmv") );

	ResetActionClipboard();
	ResetKeyClipboard();
	ResetEventClipboard();

	for_each( m_vlpActorResInfo.begin(), m_vlpActorResInfo.end(), 
			  DeleteData<S_ACTOR_RES_INFO*>() );

	for_each( m_vlpBossMonsterInfo.begin(), m_vlpBossMonsterInfo.end(), 
			  DeleteData<S_MONSTER_INFO*>() );

	delete m_pActorTable;
	delete m_pMapTable;
	delete m_pWeatherTable;
	delete m_pFaceAniTable;
	delete m_pMonsterTable;
	delete m_pWeaponTable;
	delete m_pNPCTable;

	delete m_pCmdProcessor;
	delete m_pCoreData;
}


void CToolData::Initialize( void )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];

	int iNumItem = m_pMapTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem; ++iItem )
	{
		int iMapID = m_pMapTable->GetItemID( iItem );
		const char* pMapName = m_pMapTable->GetFieldFromLablePtr(  iMapID, "_ToolName1" )->GetString();

		int iWeatherID = m_pMapTable->GetFieldFromLablePtr( iMapID, "_WeatherID" )->GetInteger();
		//const char* pMapEnvFileName = m_pWeatherTable->GetFieldFromLablePtr( iWeatherID, "_Envi1_1" )->GetString();
		const char* pMapEnvFileName = CommonUtil::GetFileNameFromFileEXT( m_pWeatherTable, iWeatherID, "_Envi1_1", m_pFileTable );

		if( strcmp(pMapName, "") != 0 )
		{
			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, pMapName, 256 );

			if( pMapEnvFileName && 0 == strlen(pMapEnvFileName) )
			{
				wxString strMsg;
				strMsg.Printf( wxT("[%d of %s map Weather ID is not exist!!"), iWeatherID, caWBuf );
				wxMessageBox( strMsg, wxT("Map & Weather Table Error!!") );
			}

			m_vlMapResNames.push_back( wxString(caWBuf) );
			m_vlMapResNames.back().MakeUpper();

			MBConv.MB2WC( caWBuf, pMapEnvFileName, 256 );
			m_mapMapNameToEnvFile.insert( make_pair(m_vlMapResNames.back(), wxString(caWBuf).MakeUpper()) );

			//string strEnvFileName(pMapEnvFileName);
			//transform( strEnvFileName.begin(), strEnvFileName.end(), strEnvFileName.begin(), toupper );
			//m_mapMapNameToEnvFile.insert( make_pair(m_vlMapResNames.back(), strEnvFileName ) );
		}
	}
}


void CToolData::MakeUIStringUseVariableParam( wstring &wszStr, int nMessageId, char *szParam )
{
	std::wstring wszBaseMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMessageId );

	// 여기에 영어, 한글, 소수가 들어갈 일은 절대 없으니, {int형숫자} 혹은 숫자만 생각하고 처리하면 된다.
	// 파라미터를 스트링형태로 저장.
	std::vector<std::string> vecParam;
	if( szParam != NULL && (int)strlen(szParam) > 0 )
	{
		for( int i=0;; i++ )
		{
			const char *pStr = _GetSubStrByCount( i, szParam, ',' );
			if( pStr == NULL || (int)strlen(pStr) == 0 ) break;
			vecParam.push_back(pStr);
		}
	}

	int nOffset = 0;
	// BaseMessage에서 치환 스트링을 찾아 바꾼다.
	while(1)
	{
		// nOffset은 치환 스트링이 대체되지 않았을 경우(파라미터가 없다거나 해서) 그 뒤부터 검색하기 위해 사용한다.
		// 제대로 치환된다면 계속 맨 앞부터 찾는다.
		std::wstring::size_type nPos = wszBaseMessage.find_first_of( L"{", nOffset );
		std::wstring::size_type nPos2 = wszBaseMessage.find_first_of( L"}", nOffset );
		if( (nPos != std::wstring::npos) && (nPos2 != std::wstring::npos) )
		{
			// 예외처리. }게 {보다 먼저 나올 경우, 그 지점부터 다시 찾는다.
			if( nPos2 < nPos )
			{
				nOffset = (int)nPos2+1;
				continue;
			}

			std::wstring wszParamIndex = wszBaseMessage.substr(nPos+1, nPos2-nPos-1);

#ifndef _FINAL_BUILD
			// 예외처리. {}안에 아무것도 없을 경우,
			if( wszParamIndex.size() == 0 )
				_ASSERT(0&&"UIString조합 중 치환 스트링 중괄호 안에 숫자가 안적혀있습니다.");

			// 예외처리. {}안에 숫자 말고 다른게 들어있을 경우,
			for( int i = 0; i < (int)wszParamIndex.size(); ++i )
			{
				if( L'0' <= wszParamIndex[i] && wszParamIndex[i] <= L'9' )
				{
				}
				else
				{
					_ASSERT(0&&"UIString조합 중 치환 스트링 중괄호 안에 숫자 말고 다른 스트링이 들어있습니다.");
				}
			}
#endif

			int nIndex = _wtoi(wszParamIndex.c_str());

			// 인덱스가 유효한지 보고, 유효하지 않다면 치환 내용을 그대로 둔다.
			if( nIndex < 0 || nIndex+1 > (int)vecParam.size() )
			{
				nOffset = (int)nPos2+1;
				continue;
			}

			// 파라미터 인덱스에 맞는 파라미터를 구해온다.
			//std::string szParam = vecParam[nIndex];			// 지역 선언이 외부 범위에 있는 같은 이름의 선언을 숨깁니다. code analysis
			std::string wszParam = vecParam[nIndex];		
			std::wstring wszParamString;

			// 파라미터가 그냥 숫자인지, {숫자}인지 판단해 스트링으로 변환한다.(데이터에 공백 없을거라한다.)
			if( wszParam[0] == '{' && wszParam[wszParam.size()-1] == '}' )
			{
				std::string szSubStr = wszParam.substr(1, wszParam.size()-2);

#ifndef _FINAL_BUILD
				// 예외처리. {}안에 아무것도 없을 경우,
				if( szSubStr.size() == 0 )
					_ASSERT(0&&"UIString조합 중 파라미터 중괄호 안에 숫자가 안적혀있습니다.");

				// 예외처리. {}안에 숫자 말고 다른게 들어있을 경우,
				for( int i = 0; i < (int)szSubStr.size(); ++i )
				{
					if( L'0' <= szSubStr[i] && szSubStr[i] <= L'9' )
					{
					}
					else
					{
						_ASSERT(0&&"UIString조합 중 파라미터 중괄호 안에 숫자 말고 다른 스트링이 들어있습니다.");
					}
				}
#endif

				wszParamString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, atoi(szSubStr.c_str()) );

#ifndef _FINAL_BUILD
				// 예외처리. 파라미터에 있는 {}를 UIString으로 변환했는데, 그 안에 중괄호 {}가 있을 경우
				for( int i = 0; i < (int)wszParamString.size(); ++i )
				{
					if( wszParamString[i] == L'{' || wszParamString[i] == L'}' )
					{
						_ASSERT(0&&"UIString조합 중 파라미터 중괄호의 값안에 중괄호{} 가 들어있습니다.");
					}
				}
#endif
			}
			else
			{
				ToWideString( wszParam, wszParamString );
			}

			// {숫자}위치에 파라미터 스트링값을 넣는다.
			wszBaseMessage.replace( nPos, nPos2-nPos+1, wszParamString );
		}
		else
		{
			// 결과값 리턴 후 종료
			wszStr = wszBaseMessage;
			break;
		}
	}
}


const wxChar* CToolData::GetEnvFileName( const wxString& strMapName )
{
	const wxChar* pEnvFileName = NULL;

	map<wxString, wxString>::iterator iter = m_mapMapNameToEnvFile.find( strMapName );
	if( m_mapMapNameToEnvFile.end() != iter )
	{
		pEnvFileName = iter->second.c_str();
	}

	return pEnvFileName;
}



const S_ACTOR_RES_INFO* CToolData::GetActorResInfoByIndex( int iIndex )
{
	const S_ACTOR_RES_INFO* pResult = NULL;
	
	if( iIndex < (int)m_vlpActorResInfo.size() )
	{
		pResult = m_vlpActorResInfo.at(iIndex);
	}

	return pResult;
}



const S_ACTOR_RES_INFO* CToolData::GetActorResInfoByName( const wxString& strActorName )
{
	const S_ACTOR_RES_INFO* pResult = NULL;

	map<wxString, S_ACTOR_RES_INFO*>::iterator iter = m_mapActorResInfo.find( strActorName );
	if( m_mapActorResInfo.end() != iter )
	{
		pResult = iter->second;
	}

	return pResult;
}



bool CToolData::SetMonsterTableID( const wxChar* pActorName, int iMonsterTableID )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->SetMonsterTableID( caBuf, iMonsterTableID );
}


int CToolData::GetMonsterTableID( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetMonsterTableID( caBuf );
}



const wxChar* CToolData::GetMapResNameByIndex( int iIndex )
{
	const wxChar* pResult = NULL;

	if( iIndex < (int)m_vlMapResNames.size() )
		pResult = m_vlMapResNames.at( iIndex ).c_str();

	return pResult;
}



void CToolData::SetMainFrame( wxWindow* pMainFrame )
{
	m_pMainFrame = pMainFrame;
}


void CToolData::WideCharToMultiByte( const wxChar* pSource, char* pResult, int iSize )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	MBConv.WC2MB( pResult, pSource, iSize );
}


void CToolData::SetResourceType( const wxChar* pResFileName, int iResType )
{
	m_mapResKindDB.insert( make_pair(wxString(pResFileName), iResType) );
}


int CToolData::GetResourceKind( const wxChar* pResFileName )
{
	int iResult = -1;

	wxString strKey( pResFileName );
	map<wxString, int>::iterator iter = m_mapResKindDB.find( strKey );
	if( m_mapResKindDB.end() != iter )
	{
		iResult = iter->second;
	}

	return iResult;
}


void CToolData::SetFullPath( const wxChar* pResName, const wxChar* pFullPath )
{
	m_mapResFullPath.insert( make_pair(wxString(pResName), wxString(pFullPath)) );
}



const wxChar* CToolData::GetFullPath( const wxChar* pResName )
{
	const wxChar* pResult = NULL;

	map<wxString, wxString>::iterator iter = m_mapResFullPath.find( wxString(pResName) );
	if( m_mapResFullPath.end() != iter )
		pResult = iter->second;

	return pResult;
}




void CToolData::GetFullPathA( const wxChar* pResName, string& strFullPath )
{
	const char* pResult = NULL;
	const wxChar* pFullPath = GetFullPath( pResName );

	if( pFullPath )
	{
		ZeroMemory( m_caBuf, sizeof(m_caBuf) );
		WideCharToMultiByte( pFullPath, m_caBuf, 256 );
		strFullPath.assign( m_caBuf );
	}
//#if defined(_DEBUG)
//	else
//		assert( !"해당 파일의 풀 패스를 찾을 수 없습니다." );
//#endif
}



void CToolData::GetFullPath( const char* pFileName, string& strFullPath )
{
	if( NULL != pFileName )
	{
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar caWBuf[ 256 ];
		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pFileName, 256 );

		// 죄다 소문자임
		wxString strFileName( caWBuf );
		strFileName.MakeLower();

		GetFullPathA( strFileName.c_str(), strFullPath );
	}
}



bool CToolData::RegisterResource(  const wxChar* pResName, int iResourceKind  )
{
	// 사용되는 리소스로 등록
	char caMBBuf[ 256 ];
	ZeroMemory( caMBBuf, sizeof(caMBBuf) );
	WideCharToMultiByte( pResName, caMBBuf, 256 );

	//int iTableID = 0;

	//switch( iResourceKind )
	//{
	//	case CDnCutSceneData::RT_RES_MAP:
	//		iTableID = -1;
	//		break;

	//		// 아직까지는 액터에서만 쓰임.. 맵에서도 쓰긴 써야할 거 같으이..
	//	case CDnCutSceneData::RT_RES_ACTOR:
	//		{
	//			const S_ACTOR_RES_INFO* pActorResInfo = GetActorResInfoByName( wxString(pResName) );
	//			iTableID = pActorResInfo->iTableID;
	//		}
	//		break;

	//	case CDnCutSceneData::RT_RES_SOUND:
	//		iTableID = -1;
	//		break;
	//}

	return m_pCoreData->RegisterResource( /*iTableID,*/ caMBBuf, iResourceKind );
}


bool CToolData::UnRegisterResource( const wxChar* pResName, int iResourceKind )
{
	// 사용되는 리소스 리스트에서 제거.
	char caMBBuf[ 256 ];
	ZeroMemory( caMBBuf, sizeof(caMBBuf) );
	WideCharToMultiByte( pResName, caMBBuf, 256 );

	return m_pCoreData->UnRegisterResource( caMBBuf, iResourceKind );
}



bool CToolData::IsRegResource( const wxChar* pResName )
{
	char caMBBuf[ 256 ];
	ZeroMemory( caMBBuf, sizeof(caMBBuf) );
	WideCharToMultiByte( pResName, caMBBuf, 256 );

	return m_pCoreData->IsRegResource( caMBBuf );
}



int CToolData::GetNumRegRes( void )
{
	return m_pCoreData->GetNumRegResource();
}


void CToolData::GetRegResNameByIndex( int iIndex, /*IN OUT*/ wxString& wxResName )
{
	const char* pResName = m_pCoreData->GetRegResNameByIndex( iIndex );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];
	ZeroMemory( caWBuf, sizeof(caWBuf) );
	MBConv.MB2WC( caWBuf, pResName, 256 );

	wxResName.assign( caWBuf );
}




int CToolData::GetRegResKindByIndex( int iIndex )
{
	return m_pCoreData->GetRegResKindByIndex( iIndex );
}



int CToolData::GenerateID( void )
{
	return m_pCoreData->GenerateID();
}


bool CToolData::InsertAction( ActionInfo* pActionInfo )
{
	bool bResult = m_pCoreData->InsertAction( pActionInfo );
	//if( bResult )
	//{
	//	TDataHelper<ActionInfo>::InsertData( pActionInfo );
	//}

	return bResult;
}


bool CToolData::RemoveAction( int iID )
{
	bool bResult = m_pCoreData->RemoveAction( iID );
	//if( bResult )
	//{
	//	TDataHelper<ActionInfo>::RemoveData( iID );
	//}

	return bResult;
}



bool CToolData::InsertKey( KeyInfo* pKeyInfo )
{
	bool bResult = m_pCoreData->InsertKey( pKeyInfo );
	//if( bResult )
	//{
	//	TDataHelper<KeyInfo>::InsertData( pKeyInfo );
	//}

	return bResult;
}


bool CToolData::RemoveKey( int iID )
{
	bool bResult = m_pCoreData->RemoveKey( iID );
	//if( bResult )
	//{
	//	TDataHelper<KeyInfo>::RemoveData( iID );
	//}

	return bResult;
}


bool CToolData::ModifyKeyInfo( KeyInfo& Key )
{
	return m_pCoreData->ModifyKeyInfo( Key );
}





bool CToolData::InsertEvent( EventInfo* pEventInfo )
{
	bool bResult = m_pCoreData->InsertEvent( pEventInfo );
	//if( bResult )
	//{
	//	TDataHelper<EventInfo>::InsertData( pEventInfo );
	//}

	return bResult;
}



bool CToolData::RemoveEvent( int iID )
{
	bool bResult = m_pCoreData->RemoveEvent( iID );
	//if( bResult )
	//{
	//	TDataHelper<EventInfo>::RemoveData( iID );
	//}

	return bResult;
}



bool CToolData::ModifyEventInfo( EventInfo* pEventInfo )
{
	return m_pCoreData->ModifyEventInfo( pEventInfo );
}




int CToolData::GetNumActors( void )
{
	return m_pCoreData->GetNumActors();
}


void CToolData::GetActorNameByIndex( int iActorIndex, /*IN OUT*/ wxString& strActorName )
{
	const char* pActorName = m_pCoreData->GetActorByIndex( iActorIndex );
	
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];
	ZeroMemory( caWBuf, sizeof(caWBuf) );
	MBConv.MB2WC( caWBuf, pActorName, 256 );

	strActorName.assign( caWBuf );
}


const ActionInfo* CToolData::GetActionInfoByID( int iID )
{
	return m_pCoreData->GetActionInfoByID( iID );
}



bool CToolData::ModifyActionInfo( ActionInfo& Action )
{
	return m_pCoreData->ModifyActionInfo( Action );
}



int CToolData::GetThisActorsActionNum( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsActionNum( caBuf );
}



const ActionInfo* CToolData::GetThisActorsActionInfoByIndex( const wxChar* pActorName, int iActionIndex )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsActionInfoByIndex( caBuf, iActionIndex );
}



const KeyInfo* CToolData::GetKeyInfoByID( int iID )
{
	return m_pCoreData->GetKeyInfoByID( iID );
}


int CToolData::GetThisActorsKeyNum( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsKeyNum( caBuf );
}


const KeyInfo* CToolData::GetThisActorsKeyInfoByIndex( const wxChar* pActorName, int iKeyIndex )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsKeyInfoByIndex( caBuf, iKeyIndex );
}




int CToolData::GetThisActorsAnimationNum( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsAnimationNum( caBuf );
}


const ActionEleInfo* CToolData::GetThisActorsAnimation( const wxChar* pActorName, int iAnimationIndex )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsAnimation( caBuf, iAnimationIndex );
}




bool CToolData::GetThisActorsFitYPosToMap( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsFitYPosToMap( caBuf );
}



bool CToolData::SetActorsFitYPosToMap( const wxChar* pActorName, bool bFitYPosToMap )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->SetActorsFitYPosToMap( caBuf, bFitYPosToMap );
}


bool CToolData::GetThisActorsInfluenceLightmap( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsInfluenceLightmap( caBuf );
}


bool CToolData::SetActorsInfluenceLightmap( const wxChar* pActorName, bool bInfluenceLightmap )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->SetActorsInfluenceLightmap( caBuf, bInfluenceLightmap );
}


bool CToolData::GetThisActorsScaleLock( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->GetThisActorsScaleLock( caBuf );
}


bool CToolData::SetActorsScaleLock( const wxChar* pActorName, bool bScaleLock )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->SetActorsScaleLock( caBuf, bScaleLock );
}



const EventInfo* CToolData::GetEventInfoByID( int iID )
{
	return m_pCoreData->GetEventInfoByID( iID );
}



int CToolData::GetThisTypesEventNum( int iEventType )
{
	return m_pCoreData->GetThisTypesEventNum( iEventType );
}



const EventInfo* CToolData::GetEventInfoByIndex( int iEventType, int iIndex )
{
	return m_pCoreData->GetEventInfoByIndex( iEventType, iIndex );
}





const EtVector3& CToolData::GetRegResPos( const wxChar* pResName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pResName, caBuf, 256 );
	
	return m_pCoreData->GetRegResPos( caBuf );
}


float CToolData::GetRegResRot( const wxChar* pResName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pResName, caBuf, 256 );

	return m_pCoreData->GetRegResRot( caBuf );
}


bool CToolData::ChangeRegResName( const wxChar* pOriResName, const wxChar* pNewResName )
{
	char caOriResName[ 256 ];
	char caNewResName[ 256 ];
	ZeroMemory( caOriResName, sizeof(caOriResName) );
	ZeroMemory( caNewResName, sizeof(caNewResName) );
	WideCharToMultiByte( pOriResName, caOriResName, 256 );
	WideCharToMultiByte( pNewResName, caNewResName, 256 );

	return m_pCoreData->ChangeRegResName( caOriResName, caNewResName );
}


bool CToolData::AddActorsAnimation( const wxChar* pActorName, ActionEleInfo* pAnimationElement )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->AddActorsAnimation( caBuf, pAnimationElement );
}


bool CToolData::ClearActorsAnimations( const wxChar* pActorName )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pActorName, caBuf, 256 );

	return m_pCoreData->ClearActorsAnimations( caBuf );
}



bool CToolData::SetRegResPos( const wxChar* pResName, EtVector3& vPos )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pResName, caBuf, 256 );

	return m_pCoreData->SetRegResPos( caBuf, vPos );
}


bool CToolData::SetRegResRot( const wxChar* pResName, float fRot )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pResName, caBuf, 256 );

	return m_pCoreData->SetRegResRot( caBuf, fRot );
}



void CToolData::RunCommand( ICommand* pCommand )
{
	// 임시 파일 저장.. ㅋㅋ
	//SaveAsToFile( wxT("temporary_work_file.dmv") );

	m_pCmdProcessor->SetCommand( pCommand );
	m_pCmdProcessor->DoCommand();

	if( m_bModified ) 
	{
		NotifyToAllListeners( pCommand );
		m_pCmdProcessor->OnUpdatedAllListeners();

		m_bModified = false;
	}
}


void CToolData::UndoCommand( void )
{
	if( m_pCmdProcessor->CanUndo() )
	{
		m_pCmdProcessor->UndoCommand();
		NotifyToAllListeners( m_pCmdProcessor->GetLastDidCommand() );
	}
}


void CToolData::RedoCommand( void )
{
	if( m_pCmdProcessor->CanRedo() )
	{
		m_pCmdProcessor->RedoCommand();
		NotifyToAllListeners( m_pCmdProcessor->GetLastDidCommand() );
	}
}


void CToolData::UpdateToThisHistory( int iHistory )
{
	m_pCmdProcessor->UpdateToThisHistory( m_pCmdProcessor->GetNumCmdHistory() - iHistory - 1 );
	CUpdateViewCmd UpdateView( this );
	NotifyToAllListeners( &UpdateView/*m_pCmdProcessor->GetLastDidCommand()*/ );
}


ICommand* CToolData::GetLastDidCommand( void )
{
	return m_pCmdProcessor->GetLastDidCommand();
}



//EventInfo* CToolData::CreateEventInfo( int iResType )
//{
//	EventInfo* pNewEvent = NULL;
//
//	switch( iResType )
//	{
//		case CDnCutSceneData::RT_RES_MAX_CAMERA:
//			pNewEvent = new CamEventInfo;
//			break;
//	}
//
//	return pNewEvent;
//}


bool CToolData::SaveToFile( const wxChar* pFilePath )
{
	m_strOpenFilePath.assign( pFilePath );

	wxString strFileName;
	wxString strFileExt;
	wxFileName::SplitPath( m_strOpenFilePath, NULL, &strFileName, &strFileExt );
	m_strOpenFileName = strFileName + wxT(".") + strFileExt;

	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pFilePath, caBuf, 256 );

	m_bEdited = false;

	return m_pCoreData->SaveToFile( caBuf );
}


bool CToolData::SaveAsToFile( const wxChar* pFilePath )
{
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pFilePath, caBuf, 256 );

	return m_pCoreData->SaveToFile( caBuf );
}




bool CToolData::LoadFromFile( const wxChar* pFilePath )
{
	m_strOpenFilePath.assign( pFilePath );

	// 읽기 전용이면 읽기 전용 상태라고 알려준다.
	m_bWritable = wxFileName::IsFileWritable( m_strOpenFilePath );
	if( false == m_bWritable )
		wxMessageBox( wxT("This File is Read Only!!\nMaybe file is locked by another member. Check SVN!"), wxT("Error!!") );
	
	wxString strFileName;
	wxString strFileExt;
	wxFileName::SplitPath( m_strOpenFilePath, NULL, &strFileName, &strFileExt );
	m_strOpenFileName = strFileName + wxT(".") + strFileExt;

	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	WideCharToMultiByte( pFilePath, caBuf, 256 );

	m_bEdited = false;

	return m_pCoreData->LoadFromFile( caBuf );
}


// 툴에서 사용하는 임시 데이터 리셋
void CToolData::ClearToolTempData( void )
{
	m_pCmdProcessor->Clear();

	m_pPropertyPanel->ClearTempData();

	m_bModified = false;
	m_iNowSelectedRegRes = wxNOT_FOUND;
	m_iSelectedObjectID = wxNOT_FOUND;
	m_iSelectedObjectUseType = wxNOT_FOUND;
	m_iEditMode = EM_EDIT;
	m_dwLocalTime = 0;
	m_bEdited = false;

	ZeroMemory( m_caBuf, sizeof(m_caBuf) );
}


bool CToolData::IsFileOpen( void )
{
	return !m_strOpenFilePath.IsEmpty();
}


const wxChar* CToolData::GetOpenedFilePath( void )
{
	return m_strOpenFilePath.c_str();
}


const wxChar* CToolData::GetOpenedFileName( void )
{
	return m_strOpenFileName.c_str();
}


void CToolData::SetModified( bool bModified )
{
	m_bModified = bModified;
	m_bEdited = true;

	static_cast<CRTCutSceneMakerFrame*>(m_pMainFrame)->OnEdited();
}


bool CToolData::IsFileEdited( void )
{
	return m_bEdited;
}


void CToolData::SyncWithDependencyResources( void )
{
	SyncActionIndexWithActFile();
	SyncCameraEventActorList();
	SyncMonsterTableID();
}


void CToolData::SyncActionIndexWithActFile( void )
{
	int iNumActor = m_pCoreData->GetNumActors();
	wxString strMsg;

	vector<int> vlActionToRemoveID;

	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		const char* pActorName =  m_pCoreData->GetActorByIndex( iActor );
	
		int iNumAction = m_pCoreData->GetThisActorsActionNum( pActorName );
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			const ActionInfo* pActionInfo = m_pCoreData->GetThisActorsActionInfoByIndex( pActorName, iAction );
			const ActionEleInfo* pActionEleInfo = m_pCoreData->GetThisActorsAnimation( pActorName, pActionInfo->iAnimationIndex );

			// 컷신툴에서 사용한 액션툴에서 정의한 액션 정보가 없음. 
			// 새로 바뀐 액션셋의 전체 범위를 벗어난 경우일 수 있기 때문에 액션 인덱스에 0 을 셋팅하고 
			// 자연스럽에 인덱스 복구 루틴으로 넘겨준다.
			if( NULL == pActionEleInfo )
			{
				pActionEleInfo = m_pCoreData->GetThisActorsAnimation( pActorName, 0 );
				//wxCSConv MBConv( wxFONTENCODING_CP949 );

				//wxChar caWActorName[ 256 ];
				//ZeroMemory( caWActorName, sizeof(caWActorName) );
				//MBConv.MB2WC( caWActorName, pActionInfo->strActorName.c_str(), 256 );

				//wxChar caWActionName[ 256 ];
				//ZeroMemory( caWActionName, sizeof(caWActionName) );
				//MBConv.MB2WC( caWActionName, pActionInfo->strAnimationName.c_str(), 256 );

				//wxString str;
				//str.Printf( wxT("Can't find [%s]'s [%s] Action!!"), caWActorName, caWActionName );
				//wxMessageBox( str, wxT("Error") );
			}

			if( pActionInfo->strAnimationName != pActionEleInfo->szName )
			{
				ActionInfo* pActionInfoToEdit = const_cast<ActionInfo*>(pActionInfo);

				int iNumAniEleInfo = m_pCoreData->GetThisActorsAnimationNum( pActorName );
				int iFinded = -1;
				for( int iInfo = 0; iInfo < iNumAniEleInfo; ++iInfo )
				{
					const ActionEleInfo* pInfo = m_pCoreData->GetThisActorsAnimation( pActorName, iInfo );
					if( pActionInfo->strAnimationName == pInfo->szName )
					{
						iFinded = iInfo;
						break;
					}
				}

				if( -1 != iFinded )
				{
					// 찾은 경우. 이 경우엔 액션의 인덱스가 변경된 경우이다.
					pActionInfoToEdit->iAnimationIndex = iFinded;

					wxString strBuf;
					wxCSConv MBConv( wxFONTENCODING_CP949 );
					
					wxChar caWActorName[ 256 ];
					ZeroMemory( caWActorName, sizeof(caWActorName) );
					MBConv.MB2WC( caWActorName, pActorName, 256 );

					wxChar caWAnimationName[ 256 ];
					ZeroMemory( caWAnimationName, sizeof(caWAnimationName) );
					MBConv.MB2WC( caWAnimationName, pActionInfo->strAnimationName.c_str(), 256 );

					strBuf.Printf( wxT("%s - %s animation index changed.\n"), caWActorName, caWAnimationName );
					strMsg.append( strBuf );
				}
				else
				{
					// 못 찾은 경우. 이 경우엔 애니메이션이 아예 사라진 경우.
					// 액션을 아예 삭제 시킨다.
					wxString strBuf;
					wxCSConv MBConv( wxFONTENCODING_CP949 );

					wxChar caWActorName[ 256 ];
					ZeroMemory( caWActorName, sizeof(caWActorName) );
					MBConv.MB2WC( caWActorName, pActorName, 256 );

					wxChar caWAnimationName[ 256 ];
					ZeroMemory( caWAnimationName, sizeof(caWAnimationName) );
					MBConv.MB2WC( caWAnimationName, pActionInfo->strAnimationName.c_str(), 256 );

					strBuf.Printf( wxT("%s - %s animation removed.\n"), caWActorName, caWAnimationName );
					strMsg.append( strBuf );

					vlActionToRemoveID.push_back( pActionInfo->iID );
					//m_pCoreData->RemoveAction( pActionInfo->iID );
				}
			}
		}
	}

	if( false == strMsg.IsEmpty() )
	{
		int iNumToRemove = (int)vlActionToRemoveID.size();
		for( int i = 0; i < iNumToRemove; ++i )
			m_pCoreData->RemoveAction( vlActionToRemoveID.at(i) );

		wxMessageDialog* pMsgDialog = new wxMessageDialog( NULL, strMsg, wxT("Action data mismatch that cutscene with action file.") );
		pMsgDialog->ShowModal();
		delete pMsgDialog;

		CUpdateViewCmd UpdateView( this );
		RunCommand( &UpdateView );
	}
}



void CToolData::SyncCameraEventActorList( void )
{
	int iNumCamEvent = m_pCoreData->GetThisTypesEventNum( EventInfo::CAMERA );
	for( int i = 0; i < iNumCamEvent; ++i )
	{
		EventInfo* pEventInfo = const_cast<EventInfo*>(m_pCoreData->GetEventInfoByIndex( EventInfo::CAMERA, i ));
		CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);

		// 없어진 액터 체크
		map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.begin();
		for( iter; pCamEventInfo->mapActorsShowHide.end() != iter; ++iter )
		{
			bool bRegActor = m_pCoreData->IsRegResource( iter->first.c_str() );
			if( false == bRegActor )
				iter = pCamEventInfo->mapActorsShowHide.erase( iter );
		}

		// 추가된 액터 체크
		int iNumActor = m_pCoreData->GetNumActors();
		for( int iActor = 0; iActor < iNumActor; ++iActor )
		{
			const char* pActorName = m_pCoreData->GetActorByIndex( iActor );
			map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.find( pActorName );
			if( pCamEventInfo->mapActorsShowHide.end() == iter )
				pCamEventInfo->mapActorsShowHide.insert( make_pair(pActorName, true) );
		}
	}
}


void CToolData::SyncMonsterTableID( void )
{
	int iNumActors = m_pCoreData->GetNumActors();
	wxString strMsg;
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		const char* pActorName =  m_pCoreData->GetActorByIndex( iActor );
		int iMonsterTableID = m_pCoreData->GetMonsterTableID( pActorName );
		if( 0 != iMonsterTableID )
		{
			wxCSConv MBConv( wxFONTENCODING_CP949 );
			wxChar caWActorName[ 256 ];
			ZeroMemory( caWActorName, sizeof(caWActorName) );
			MBConv.MB2WC( caWActorName, pActorName, 256 );

			// 보스 뿐만 아니라 일반 몹도 스케일 작은 경우가 있으므로 선택 가능하도록 처리.
			//if( Boss == m_pMonsterTable->GetFieldFromLablePtr( iMonsterTableID, "_Grade" )->GetInteger() )
			//{
				int iNowActorTableID = m_pMonsterTable->GetFieldFromLablePtr( iMonsterTableID, "_ActorTableID" )->GetInteger();
				
				wxString strActorToolDefineName( caWActorName );
				wxString strActorResName = strActorToolDefineName.substr( 0, strActorToolDefineName.find_last_of(wxT("_")) );

				const S_ACTOR_RES_INFO* pActorResInfo = GetActorResInfoByName( strActorResName.c_str() );
				if( iNowActorTableID != pActorResInfo->iTableID )
				{
					wxString strBuf;
					strBuf.Printf( wxT("[%s] Boss Monster's Information Has Changed!!\n"), caWActorName );
					strMsg.Append( strBuf );

					// 보스 몬스터로 셋팅했던 몬스터 테이블 인덱스를 찾을 수 없거나,
					m_pCoreData->SetMonsterTableID( pActorName, 0 );
				}
			//}
			//else
			//{
				//wxString strBuf;
				//strBuf.Printf( wxT("Can't find [%s] Boss Monster's Information from MonsterTable!!\n"), caWActorName );
				//strMsg.Append( strBuf );

				//// 보스 몬스터로 셋팅했던 몬스터 테이블 인덱스의 원래 액터 인덱스가 변경되었으므로 다시 셋팅해야한다. 
				//// 인덱스 리셋시킴.
				//m_pCoreData->SetMonsterTableIDAsBoss( pActorName, 0 );
			//}
		}
	}

	if( !strMsg.empty() )
	{
		strMsg.Append( wxT("Please resetting these boss actor table.") );
		wxMessageDialog* pMsgDialog = new wxMessageDialog( NULL, strMsg, wxT("Changed MonsterTableID!!") );
		pMsgDialog->ShowModal();
		delete pMsgDialog;
	}
}



void CToolData::CopyToClipboard( const ActionInfo* pActionInfo )
{
	ResetActionClipboard();
	m_pActionInfoClipboard = new ActionInfo( *pActionInfo );
}


void CToolData::CopyToClipboard( const KeyInfo* pKeyInfo )
{
	ResetKeyClipboard();
	m_pKeyInfoClipboard = new KeyInfo( *pKeyInfo );
}


void CToolData::CopyToClipboard( const EventInfo* pEventInfo )
{
	ResetEventClipboard(); 
	m_pEventInfoClipboard = pEventInfo->clone();
}

void CToolData::GatherBossInfoOfThisActor( const wxString& strActorName, /*IN OUT*/ vector<const S_MONSTER_INFO*>& vlResult )
{
	const S_ACTOR_RES_INFO* pActorInfo = GetActorResInfoByName( strActorName );
	if( pActorInfo )
		GatherBossInfoOfThisActor( pActorInfo->iTableID, vlResult );
}

void CToolData::GatherBossInfoOfThisActor( int iActorTableIDToFind, /*IN OUT*/ vector<const S_MONSTER_INFO*>& vlResult )
{
	int iNumBossMonster = (int)m_vlpBossMonsterInfo.size();
	for( int i = 0; i < iNumBossMonster; ++i )
	{
		const S_MONSTER_INFO* pBossMonsterInfo = m_vlpBossMonsterInfo.at( i );
		if( pBossMonsterInfo->iActorTableID == iActorTableIDToFind )
			vlResult.push_back( pBossMonsterInfo );
	}
}