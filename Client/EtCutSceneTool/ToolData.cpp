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
		assert( !"UIString.xml �ε� ����");

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

		// ���ΰ����̶�� ���ΰ����� �̸��� �״�� ���
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

		// ���� �Ӹ� �ƴ϶� ���� ���͵� �־ �Ϲ� ������ ���� ����.
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

	// ���⿡ ����, �ѱ�, �Ҽ��� �� ���� ���� ������, {int������} Ȥ�� ���ڸ� �����ϰ� ó���ϸ� �ȴ�.
	// �Ķ���͸� ��Ʈ�����·� ����.
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
	// BaseMessage���� ġȯ ��Ʈ���� ã�� �ٲ۴�.
	while(1)
	{
		// nOffset�� ġȯ ��Ʈ���� ��ü���� �ʾ��� ���(�Ķ���Ͱ� ���ٰų� �ؼ�) �� �ں��� �˻��ϱ� ���� ����Ѵ�.
		// ����� ġȯ�ȴٸ� ��� �� �պ��� ã�´�.
		std::wstring::size_type nPos = wszBaseMessage.find_first_of( L"{", nOffset );
		std::wstring::size_type nPos2 = wszBaseMessage.find_first_of( L"}", nOffset );
		if( (nPos != std::wstring::npos) && (nPos2 != std::wstring::npos) )
		{
			// ����ó��. }�� {���� ���� ���� ���, �� �������� �ٽ� ã�´�.
			if( nPos2 < nPos )
			{
				nOffset = (int)nPos2+1;
				continue;
			}

			std::wstring wszParamIndex = wszBaseMessage.substr(nPos+1, nPos2-nPos-1);

#ifndef _FINAL_BUILD
			// ����ó��. {}�ȿ� �ƹ��͵� ���� ���,
			if( wszParamIndex.size() == 0 )
				_ASSERT(0&&"UIString���� �� ġȯ ��Ʈ�� �߰�ȣ �ȿ� ���ڰ� �������ֽ��ϴ�.");

			// ����ó��. {}�ȿ� ���� ���� �ٸ��� ������� ���,
			for( int i = 0; i < (int)wszParamIndex.size(); ++i )
			{
				if( L'0' <= wszParamIndex[i] && wszParamIndex[i] <= L'9' )
				{
				}
				else
				{
					_ASSERT(0&&"UIString���� �� ġȯ ��Ʈ�� �߰�ȣ �ȿ� ���� ���� �ٸ� ��Ʈ���� ����ֽ��ϴ�.");
				}
			}
#endif

			int nIndex = _wtoi(wszParamIndex.c_str());

			// �ε����� ��ȿ���� ����, ��ȿ���� �ʴٸ� ġȯ ������ �״�� �д�.
			if( nIndex < 0 || nIndex+1 > (int)vecParam.size() )
			{
				nOffset = (int)nPos2+1;
				continue;
			}

			// �Ķ���� �ε����� �´� �Ķ���͸� ���ؿ´�.
			//std::string szParam = vecParam[nIndex];			// ���� ������ �ܺ� ������ �ִ� ���� �̸��� ������ ����ϴ�. code analysis
			std::string wszParam = vecParam[nIndex];		
			std::wstring wszParamString;

			// �Ķ���Ͱ� �׳� ��������, {����}���� �Ǵ��� ��Ʈ������ ��ȯ�Ѵ�.(�����Ϳ� ���� �����Ŷ��Ѵ�.)
			if( wszParam[0] == '{' && wszParam[wszParam.size()-1] == '}' )
			{
				std::string szSubStr = wszParam.substr(1, wszParam.size()-2);

#ifndef _FINAL_BUILD
				// ����ó��. {}�ȿ� �ƹ��͵� ���� ���,
				if( szSubStr.size() == 0 )
					_ASSERT(0&&"UIString���� �� �Ķ���� �߰�ȣ �ȿ� ���ڰ� �������ֽ��ϴ�.");

				// ����ó��. {}�ȿ� ���� ���� �ٸ��� ������� ���,
				for( int i = 0; i < (int)szSubStr.size(); ++i )
				{
					if( L'0' <= szSubStr[i] && szSubStr[i] <= L'9' )
					{
					}
					else
					{
						_ASSERT(0&&"UIString���� �� �Ķ���� �߰�ȣ �ȿ� ���� ���� �ٸ� ��Ʈ���� ����ֽ��ϴ�.");
					}
				}
#endif

				wszParamString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, atoi(szSubStr.c_str()) );

#ifndef _FINAL_BUILD
				// ����ó��. �Ķ���Ϳ� �ִ� {}�� UIString���� ��ȯ�ߴµ�, �� �ȿ� �߰�ȣ {}�� ���� ���
				for( int i = 0; i < (int)wszParamString.size(); ++i )
				{
					if( wszParamString[i] == L'{' || wszParamString[i] == L'}' )
					{
						_ASSERT(0&&"UIString���� �� �Ķ���� �߰�ȣ�� ���ȿ� �߰�ȣ{} �� ����ֽ��ϴ�.");
					}
				}
#endif
			}
			else
			{
				ToWideString( wszParam, wszParamString );
			}

			// {����}��ġ�� �Ķ���� ��Ʈ������ �ִ´�.
			wszBaseMessage.replace( nPos, nPos2-nPos+1, wszParamString );
		}
		else
		{
			// ����� ���� �� ����
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
//		assert( !"�ش� ������ Ǯ �н��� ã�� �� �����ϴ�." );
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

		// �˴� �ҹ�����
		wxString strFileName( caWBuf );
		strFileName.MakeLower();

		GetFullPathA( strFileName.c_str(), strFullPath );
	}
}



bool CToolData::RegisterResource(  const wxChar* pResName, int iResourceKind  )
{
	// ���Ǵ� ���ҽ��� ���
	char caMBBuf[ 256 ];
	ZeroMemory( caMBBuf, sizeof(caMBBuf) );
	WideCharToMultiByte( pResName, caMBBuf, 256 );

	//int iTableID = 0;

	//switch( iResourceKind )
	//{
	//	case CDnCutSceneData::RT_RES_MAP:
	//		iTableID = -1;
	//		break;

	//		// ���������� ���Ϳ����� ����.. �ʿ����� ���� ����� �� ������..
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
	// ���Ǵ� ���ҽ� ����Ʈ���� ����.
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
	// �ӽ� ���� ����.. ����
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

	// �б� �����̸� �б� ���� ���¶�� �˷��ش�.
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


// ������ ����ϴ� �ӽ� ������ ����
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

			// �ƽ������� ����� �׼������� ������ �׼� ������ ����. 
			// ���� �ٲ� �׼Ǽ��� ��ü ������ ��� ����� �� �ֱ� ������ �׼� �ε����� 0 �� �����ϰ� 
			// �ڿ������� �ε��� ���� ��ƾ���� �Ѱ��ش�.
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
					// ã�� ���. �� ��쿣 �׼��� �ε����� ����� ����̴�.
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
					// �� ã�� ���. �� ��쿣 �ִϸ��̼��� �ƿ� ����� ���.
					// �׼��� �ƿ� ���� ��Ų��.
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

		// ������ ���� üũ
		map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.begin();
		for( iter; pCamEventInfo->mapActorsShowHide.end() != iter; ++iter )
		{
			bool bRegActor = m_pCoreData->IsRegResource( iter->first.c_str() );
			if( false == bRegActor )
				iter = pCamEventInfo->mapActorsShowHide.erase( iter );
		}

		// �߰��� ���� üũ
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

			// ���� �Ӹ� �ƴ϶� �Ϲ� ���� ������ ���� ��찡 �����Ƿ� ���� �����ϵ��� ó��.
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

					// ���� ���ͷ� �����ߴ� ���� ���̺� �ε����� ã�� �� ���ų�,
					m_pCoreData->SetMonsterTableID( pActorName, 0 );
				}
			//}
			//else
			//{
				//wxString strBuf;
				//strBuf.Printf( wxT("Can't find [%s] Boss Monster's Information from MonsterTable!!\n"), caWActorName );
				//strMsg.Append( strBuf );

				//// ���� ���ͷ� �����ߴ� ���� ���̺� �ε����� ���� ���� �ε����� ����Ǿ����Ƿ� �ٽ� �����ؾ��Ѵ�. 
				//// �ε��� ���½�Ŵ.
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