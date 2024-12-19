#include "StdAfx.h"
#include "DnMainFrame.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnActionBase.h"
#include "DnWorld.h"
#include "DebugSet.h"
#include "EtCollisionMng.h"
#include "EtActionCoreMng.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DNGameDataManager.h"
#endif

// ETC
#include <mmsystem.h>

extern TGameConfig g_Config;
CDnMainFrame::CDnMainFrame()
{
	m_pTableDB	= NULL;
	m_pUIXML	= NULL;
	m_pResMng	= CEtResourceMng::IsActive() ? CEtResourceMng::GetInstancePtr() :new CEtResourceMng( false, !g_Config.bPreLoad );
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	m_pExceptionalUIXML = NULL;
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING
	InitializeLocalize();
}

CDnMainFrame::~CDnMainFrame()
{
	Finalize();
	FinalizeDevice();
	if( CEtResourceMng::IsActive())
		delete CEtResourceMng::GetInstancePtr();
}

void CDnMainFrame::InitializeLocalize()
{
}

bool CDnMainFrame::PreInitialize()
{
	// Path 설정
	std::string szResource = g_Config.szResourcePath + "\\Resource";
	std::string szMapData = g_Config.szResourcePath + "\\MapData";

	// 국가별 셋팅
	std::string szNationStr;
	if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;
	if( !szNationStr.empty() ) {
		szResource += szNationStr;
		szMapData += szNationStr;

		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
		CEtResourceMng::GetInstance().AddResourcePath( szMapData, true );

		szResource = g_Config.szResourcePath + "\\Resource";
		szMapData = g_Config.szResourcePath + "\\MapData";
	}

	CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
	CEtResourceMng::GetInstance().AddResourcePath( szMapData, true );

	// DB Load
	if( m_pTableDB ) SAFE_DELETE( m_pTableDB );
	m_pTableDB = new CDnTableDB;
	m_pTableDB->Initialize();

	CDebugSet::Initialize( CEtResourceMng::GetInstance().GetCurrentDirectory().c_str(), true );

	// UI XML String File Load
	m_pUIXML = new CEtUIXML;
#if defined(PRE_ADD_MULTILANGUAGE)
	std::string strNationFileName;
#ifdef PRE_ADD_UISTRING_DIVIDE
	std::string strNationItemFileName;
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
	for (int i = 0; i < MultiLanguage::SupportLanguage::NationMax; i++)
	{
		strNationFileName.clear();

		strNationFileName = "uistring";
		if (i != 0)		//0번은 디폴트
			strNationFileName.append(MultiLanguage::NationString[i]);
		strNationFileName.append(".xml");

		CStream *pStream = CEtResourceMng::GetInstance().GetStream( strNationFileName.c_str() );
		m_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i );
		SAFE_DELETE( pStream );

		strNationFileName.clear();

#ifdef PRE_ADD_UISTRING_DIVIDE
		strNationItemFileName = "uistring_item";
		if (i != 0)		//0번은 디폴트
			strNationItemFileName.append(MultiLanguage::NationString[i]);
		strNationItemFileName.append(".xml");

		pStream = CEtResourceMng::GetInstance().GetStream( strNationItemFileName.c_str() );
		m_pUIXML->Initialize( pStream, CEtUIXML::idCategory1, i, true );
		SAFE_DELETE( pStream );
#endif
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CStream *pStream = CEtResourceMng::GetInstance().GetStream( "uistring.xml" );
	m_pUIXML->Initialize( pStream, CEtUIXML::idCategory1 );
	SAFE_DELETE( pStream );

#ifdef PRE_ADD_UISTRING_DIVIDE
	pStream = CEtResourceMng::GetInstance().GetStream( "uistring_item.xml" );
	m_pUIXML->Initialize( pStream, CEtUIXML::idCategory1 );
	SAFE_DELETE( pStream );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE

#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	m_pExceptionalUIXML = new CEtExceptionalUIXML;
	if( m_pExceptionalUIXML )
		m_pExceptionalUIXML->LoadXML( "uistring_exception.xml" );
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING
	return true;
}

bool CDnMainFrame::InitializeDevice()
{
	CEtResource::Initialize( !g_Config.bPreLoad );
	CEtActionCoreMng::GetInstance().Initialize( !g_Config.bPreLoad );
	return true;
}

bool CDnMainFrame::FinalizeDevice()
{
	return true;
}

bool CDnMainFrame::Initialize()
{
	if( g_Config.bPreLoad ) {
		g_Log.Log(LogType::_FILELOG, L"Begin PreLoading...\n");
		PreloadResource();
		g_Log.Log(LogType::_FILELOG, L"Complete PreLoad.\n");
	}


	return true;
}

bool CDnMainFrame::Finalize()
{
	SAFE_DELETE( m_pTableDB );
	SAFE_DELETE( m_pUIXML );
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	SAFE_DELETE( m_pExceptionalUIXML );
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING
	g_ActionCoreMng.Finalize();
	FinalizePreloadFile();
	return true;
}

void CDnMainFrame::InitializeMultiCommon(int iCnt, std::vector <unsigned int> * vList)
{
	//InitializeMultiThread();
}

void CDnMainFrame::SetMultiThreadID(unsigned int iID)
{
}

bool CDnMainFrame::PreloadResource()
{
	// Actor Table
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Actor Table\n");
	PreloadSkinFile( CDnTableDB::TACTOR, "_SkinName" );
	PreloadAniFile( CDnTableDB::TACTOR, "_AniName" );
	PreloadActFile( CDnTableDB::TACTOR, "_ActName" );

	// Parts Table
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Parts Table\n");
	char szLable[64];
	for( int i=0; i<10; i++ ) {
		sprintf_s( szLable, "_Player%d_SkinName", i + 1 );
		PreloadSkinFile( CDnTableDB::TPARTS, szLable );
		sprintf_s( szLable, "_Player%d_AniName", i + 1 );
		PreloadAniFile( CDnTableDB::TPARTS, szLable );
		sprintf_s( szLable, "_Player%d_ActName", i + 1 );
		PreloadActFile( CDnTableDB::TPARTS, szLable );
	}

	// Weapon Table
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Weapon Table\n");
	PreloadSkinFile( CDnTableDB::TWEAPON, "_SkinName" );
	PreloadAniFile( CDnTableDB::TWEAPON, "_AniName" );
	PreloadActFile( CDnTableDB::TWEAPON, "_ActName" );
	
	// Item Table
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Item Table\n");
	PreloadSkinFile( CDnTableDB::TITEM, "_DropSkinName" );
	PreloadAniFile( CDnTableDB::TITEM, "_DropAniName" );
	PreloadActFile( CDnTableDB::TITEM, "_DropActName" );

	// World
	char szWorldFolder[_MAX_PATH];
	char szPath[_MAX_PATH];

	sprintf_s( szWorldFolder, CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str() );

	sprintf_s( szPath, "%s\\Resource\\Tile", szWorldFolder );
	CEtResourceMng::GetInstance().AddResourcePath( szPath, true );

	sprintf_s( szPath, "%s\\Resource\\Prop", szWorldFolder );
	CEtResourceMng::GetInstance().AddResourcePath( szPath, true );

	sprintf_s( szPath, "%s\\Resource\\Envi", szWorldFolder );
	CEtResourceMng::GetInstance().AddResourcePath( szPath, true );

	sprintf_s( szPath, "%s\\Resource\\Trigger", szWorldFolder );
	CEtResourceMng::GetInstance().AddResourcePath( szPath, true );

	// Grid
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Grid\n");
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	int nItemID;
	char szStr[64];
	char szGridPath[_MAX_PATH];
	char szFullName[_MAX_PATH];
	char *szInfoFileList[] = {
		"gridinfo.ini",
		"0_0\\default.ini",
		"0_0\\eventareainfo.ini",
		"0_0\\height.ini",
		"0_0\\heightattribute.ini",
		"0_0\\navigation.ini",
		"0_0\\propinfo.ini",
		"0_0\\textable.ini",
		"0_0\\trigger.ini",
		"0_0\\triggerdefine.ini",
		"0_0\\sectorsize.ini",
		"0_0\\soundinfo.ini",
		"0_0\\grasstable.ini",
		"0_0\\alphatable.ini",
		"0_0\\colbuild.ini",
		NULL
	};
	CEtResourceMng::GetInstance().AddCacheMemoryStream( "Infomation" );
	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		nItemID = pSox->GetItemID(i);
		for( int j=0; j<10; j++ ) {
			std::string szGridName;
			sprintf_s( szStr, "_ToolName%d", j + 1 );
			szGridName = pSox->GetFieldFromLablePtr( nItemID, szStr )->GetString();
			if( szGridName.empty() ) continue;
			sprintf_s( szGridPath, "%s\\Grid\\%s", szWorldFolder, szGridName.c_str() );

			for( int k=0; ; k++ ) {
				if( szInfoFileList[k] == NULL ) break;
				sprintf_s( szFullName, "%s\\%s", szGridPath, szInfoFileList[k] );

				CEtResourceMng::GetInstance().AddCacheMemoryStream( szFullName );

				if( strstr( szFullName, "colbuild.ini" ) ) {
					CStream *pStream = CEtResourceMng::GetInstance().GetCacheMemoryStream( szFullName );
					if( pStream ) CEtCollisionMng::PreLoad( pStream );
				}
			}
		}
	}

	// World Prop
	g_Log.Log(LogType::_FILELOG, L"PreLoad - Prop\n");
	std::vector<std::string> szVecFileList;
	sprintf_s( szFullName, "%s\\Resource\\Prop", szWorldFolder );
	FindFileListInDirectory( szFullName, "*.*", szVecFileList, false, true );
	for( DWORD i=0; i<szVecFileList.size(); i++ ) {
		_GetExt( szStr, _countof(szStr), szVecFileList[i].c_str() );
		_strlwr_s( szStr );
		if( strcmp( szStr, "skn" ) == NULL ) {
			PreloadSkinFile( szVecFileList[i].c_str() );
		}
		else if( strcmp( szStr, "ani" ) == NULL ) {
			PreloadAniFile( szVecFileList[i].c_str() );
		}
		else if( strcmp( szStr, "act" ) == NULL ) {
			PreloadActFile( szVecFileList[i].c_str() );
		}
	}
	g_Log.Log(LogType::_NORMAL, L"PreLoad - Trigger\n");
	sprintf_s( szFullName, "%s\\Resource\\Trigger", szWorldFolder );
	FindFileListInDirectory( szFullName, "*.*", szVecFileList, false, true );
	for( DWORD i=0; i<szVecFileList.size(); i++ ) {
		_GetExt( szStr, _countof(szStr), szVecFileList[i].c_str() );
		_strlwr_s( szStr );
		if( strcmp( szStr, "lua" ) == NULL ) {
			CEtResourceMng::GetInstance().AddCacheMemoryStream( szVecFileList[i].c_str() );
		}
	}

	return true;
}

void CDnMainFrame::PreloadSkinFile( int nTableID, char *szLable )
{
	DNTableFileFormat* pSox = GetDNTable( (CDnTableDB::TableEnum)nTableID );
	std::string szFileName;
//	EtResourceHandle hResource;
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable(CDnTableDB::TFILE);
	if (pSox == NULL || pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif

	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);
#ifdef PRE_FIX_MEMOPT_EXT
		g_pDataManager->GetFileNameFromFileEXT(szFileName, pSox, nItemID, szLable, pFileNameSox);
#else
		szFileName = pSox->GetFieldFromLablePtr( nItemID, szLable )->GetString();
#endif
		if( !szFileName.empty() && szFileName != "0" ) {
			/*
			hResource = EternityEngine::LoadSkin( NULL, CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
			if( hResource ) m_hVecEngineResList.push_back( hResource );
			*/
			PreloadSkinFile( szFileName.c_str() );
		}
	}
}

void CDnMainFrame::PreloadAniFile( int nTableID, char *szLable )
{
	DNTableFileFormat* pSox = GetDNTable( (CDnTableDB::TableEnum)nTableID );
	std::string szFileName;
//	EtResourceHandle hResource;
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable(CDnTableDB::TFILE);
	if (pSox == NULL || pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif

	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);
#ifdef PRE_FIX_MEMOPT_EXT
		g_pDataManager->GetFileNameFromFileEXT(szFileName, pSox, nItemID, szLable, pFileNameSox);
#else
		szFileName = pSox->GetFieldFromLablePtr( nItemID, szLable )->GetString();
#endif
		if( !szFileName.empty() && szFileName != "0" ) {
			/*
			hResource = EternityEngine::LoadAni( NULL, CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
			if( hResource ) m_hVecEngineResList.push_back( hResource );
			*/
			PreloadAniFile( szFileName.c_str() );
		}
	}
}

void CDnMainFrame::PreloadActFile( int nTableID, char *szLable )
{
	DNTableFileFormat* pSox = GetDNTable( (CDnTableDB::TableEnum)nTableID );
	std::string szFileName;
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable(CDnTableDB::TFILE);
	if (pSox == NULL || pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif

//	CDnActionBase *pBase = NULL;
	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);
#ifdef PRE_FIX_MEMOPT_EXT
		g_pDataManager->GetFileNameFromFileEXT(szFileName, pSox, nItemID, szLable, pFileNameSox);
#else
		szFileName = pSox->GetFieldFromLablePtr( nItemID, szLable )->GetString();
#endif
		if( !szFileName.empty() && szFileName != "0" ) {
			/*
			pBase = new CDnActionBase;
			if( pBase->LoadAction( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() ) ) {
				m_pVecActionList.push_back( pBase );
			}
			else {
				SAFE_DELETE( pBase );
			}
			*/
			PreloadActFile( szFileName.c_str() );
		}
	}
}

void CDnMainFrame::PreloadSkinFile( const char *szFileName )
{
	EtResourceHandle hResource = EternityEngine::LoadSkin( NULL, CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	if( hResource ) m_hVecEngineResList.push_back( hResource );
}

void CDnMainFrame::PreloadAniFile( const char *szFileName )
{
	EtResourceHandle hResource = EternityEngine::LoadAni( NULL, CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	if( hResource ) m_hVecEngineResList.push_back( hResource );
}

void CDnMainFrame::PreloadActFile( const char *szFileName )
{
	CDnActionBase *pBase = new CDnActionBase;
	if( pBase->LoadAction( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() ) ) {
		m_pVecActionList.push_back( pBase );
	}
	else {
		SAFE_DELETE( pBase );
	}
}

void CDnMainFrame::FinalizePreloadFile()
{
	SAFE_RELEASE_SPTRVEC( m_hVecEngineResList );
	SAFE_DELETE_PVEC( m_pVecActionList );
	SAFE_DELETE_PVEC( m_pVecWorldList );
}