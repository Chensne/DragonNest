// DnTableVerifier.cpp : Defines the entry point for the console application.
//


// R:\GameRes KOR SkillTable --gtest_filter=skill_data_test.* --gtest_output="xml:result.xml"


#include "stdafx.h"
#include "EtActionCoreMng.h"

wstring g_strResourcePath;
string g_strResourcePathA;
map<wstring, string> g_mapExtFolderPath;
wstring g_strNation;
string g_strNationExtFolder;
string g_strTableName;

extern CEtActionCoreMng g_ActionCoreMng;

// 해당 국가 폴더 안에서만 가져오도록 구현사항 변경됨.
void GatherConcernedExt( /*IN*/ const char* pTableFileFullName, /*IN OUT*/ vector<string>& vlFilePathsToVerify, /*IN OUT*/ string& strTableFullPath )
{
	// 우선 메인 리소스에 있는 테이블 파일 리스트와 지정된 국가 폴더에 있는 리스트를 불러들여서 
	// 특정 국가 폴더에 있는 파일이 우선이고 거기 없으면 메인 리소스에 있는 파일을 가져다 사용하면 된다.
	// _ 붙은 나눠진 파일만 id 중복 검증 대상이다.
	string strNationExtFolder = g_strNationExtFolder;

	strTableFullPath.clear();

	// 지정된 국가의 ext 폴더만 사용한다.
	// 리소스 매니져 내부적으로 먼저 등록된 폴더 기준으로 검색하도록 구현되어 있음.
	//map<wstring, string>::iterator iter = g_mapExtFolderPath.begin();
	//for( iter; iter != g_mapExtFolderPath.end(); ++iter )
	//	CEtResourceMng::GetInstance().AddResourcePath( iter->second.c_str() );

	//vector<CFileNameString> vlBaseFileList;
	//CEtResourceMng::GetInstance().FindFileListInFolder( g_mapExtFolderPath[ L"KOR" ].c_str(), "*.ext", vlBaseFileList );

	vector<CFileNameString> vlTargetNationFileList;
	CEtResourceMng::GetInstance().FindFileListInFolder( strNationExtFolder.c_str(), "*.dnt", vlTargetNationFileList );

	char acTableFileName[ MAX_PATH ] = { 0 };
	_GetFileName( acTableFileName, _countof(acTableFileName), pTableFileFullName );

	char acFilePrefixToVerify[ MAX_PATH ] = { 0 };
	_GetFileName( acFilePrefixToVerify, _countof(acFilePrefixToVerify), pTableFileFullName );
	_strlwr_s( acFilePrefixToVerify );
	int nLength = (int)strlen(acFilePrefixToVerify);
	acFilePrefixToVerify[nLength] = '_';
	acFilePrefixToVerify[nLength+1] = 0;

	vlFilePathsToVerify.clear();
	vector<string> vlFileNamesToVerify;

	// 우선 타겟 국가의 폴더에서 나눠진 파일들의 풀 패스를 전부 모은다.
	for( DWORD i = 0; i<vlTargetNationFileList.size(); i++ ) 
	{
		char acTargetNationFileName[ MAX_PATH ] = { 0 };
		_GetFileName( acTargetNationFileName, _countof(acTargetNationFileName), vlTargetNationFileList[ i ].c_str() );
		_strlwr_s( acTargetNationFileName );

		string strTargetNationFileName( acTargetNationFileName );
		string::size_type iPrefixPos = strTargetNationFileName.find_first_of( '_' );
		string strTargetNationFilePrefix;
		if( string::npos != iPrefixPos )
			strTargetNationFilePrefix = strTargetNationFileName.substr( 0, iPrefixPos+1 );

		if( strTargetNationFilePrefix == acFilePrefixToVerify )
		//if( strstr( acTargetNationFileName, acFilePrefixToVerify ) )
		{
			CFileNameString szFullPath;
			szFullPath = CEtResourceMng::GetInstance().GetFullName( vlTargetNationFileList[i].c_str() );
			vlFilePathsToVerify.push_back( szFullPath.c_str() );
			vlFileNamesToVerify.push_back( acTargetNationFileName );
		}

		if( strTableFullPath.empty() && (0 == _stricmp( acTargetNationFileName, acTableFileName )) )
		{
			CFileNameString szFullPath;
			szFullPath = CEtResourceMng::GetInstance().GetFullName( pTableFileFullName );
			strTableFullPath.assign( szFullPath.c_str() );
		}
	}

	//// 베이스 리소스 폴더에 나눠진 파일들이 더 있는지 모아본다. 
	//// 같은 파일 이름이라면 지정된 국가의 리소스가 우선이므로 추가하지 않는다.
	//for( DWORD i = 0; i < vlBaseFileList.size(); ++i )
	//{
	//	char acBaseFileName[ MAX_PATH ] = { 0 };
	//	_GetFileName( acBaseFileName, vlBaseFileList[ i ].c_str() );
	//	_strlwr_s( acBaseFileName );

	//	if( strstr( acBaseFileName, acFilePrefixToVerify ) && strcmp( acBaseFileName, acFilePrefixToVerify ) )
	//	{		
	//		vector<string>::iterator iter = find( vlFileNamesToVerify.begin(), vlFileNamesToVerify.end(), acBaseFileName );
	//		if( iter == vlFileNamesToVerify.end() )
	//		{
	//			CFileNameString szFullPath;
	//			szFullPath = CEtResourceMng::GetInstance().GetFullName( vlBaseFileList[i].c_str() );
	//			vlFilePathsToVerify.push_back( szFullPath.c_str() );
	//		}
	//	}

	//	if( strTableFullPath.empty() && (0 == _stricmp( acBaseFileName, acTableFileName )) )
	//	{
	//		CFileNameString szFullPath;
	//		szFullPath = CEtResourceMng::GetInstance().GetFullName( pTableFileFullName );
	//		strTableFullPath.assign( szFullPath.c_str() );
	//	}
	//}

	//// 원상 복구.
	//iter = g_mapExtFolderPath.begin();
	//for( iter; iter != g_mapExtFolderPath.end(); ++iter )
	//{
	//	if( iter->first != g_strNation )
	//		CEtResourceMng::GetInstance().RemoveResourcePath( iter->second.c_str() );
	//}
}

DNTableFileFormat* LoadExtFile( const char* pTableFileFullName, char* pInverseSearchLabelFilter/* = NULL*/ )
{
	DNTableFileFormat* pTable = NULL;
	vector<string> vlFileSeriesFilePaths;
	string strTableFullPath;

	// 아이템 테이블이라면 ItemTable_Quest, ItemTable_Event, ItemTable_Equipment, ItemTable_Cash 시리즈별로 모두 모아서 로드 됨.
	GatherConcernedExt( pTableFileFullName, vlFileSeriesFilePaths, strTableFullPath );

	//_ASSERT( false == strTableFullPath.empty() );
	
	pTable = new DNTableFileFormat;
	pTable->Load( strTableFullPath.c_str() );
	// dnt 포맷에서는 역으로 찾아야 하는걸 넣어줘야 함. (GetItemIDListFromField() 함수로 찾는 것)
	pTable->SetGenerationInverseLabel( pInverseSearchLabelFilter );

	for( int i = 0; i < (int)vlFileSeriesFilePaths.size(); ++i )
	{
		pTable->Load( vlFileSeriesFilePaths.at(i).c_str(), false );
	}

	return pTable;
}



// 글로벌 환경 설정
class gtest_global_environment : public ::testing::Environment
{
	virtual void SetUp()
	{
		CEtResourceMng::CreateInstance();

		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\SharedEffect" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Prop", true );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Envi" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\MapData\\Resource\\Sound", true );

		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Char", true );

		char acBuffer[ MAX_PATH ] = { 0 };
		WideCharToMultiByte( CP_ACP, NULL, g_strResourcePath.c_str(), (int)_tcslen(g_strResourcePath.c_str()), 
							 acBuffer, sizeof(acBuffer), NULL, NULL );
		string strResourcePath( acBuffer );

		//map<wstring, string>::iterator iter = g_mapExtFolderPath.begin();
		//for( iter; iter != g_mapExtFolderPath.end(); ++iter )
		//	CEtResourceMng::GetInstance().AddResourcePath( iter->second.c_str() );

		CEtResourceMng::GetInstance().AddResourcePath( strResourcePath+"\\Resource\\Char", true );
		CEtResourceMng::GetInstance().AddResourcePath( g_strNationExtFolder.c_str() );

		//CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Ext" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Weapon" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Item", true );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Sound", true );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Particle" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\UI", true );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Effect" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\fonts" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Script", true );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\Dmv" );
		//CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Npc" );
		//CEtResourceMng::GetInstance().AddResourcePathByCodePage( "R:\\GameRes\\Resource\\Quest" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\StateEffect" );
		//CEtResourceMng::GetInstance().AddResourcePath( "R:\\GameRes\\Resource\\UIString" );

		g_ActionCoreMng.Initialize();
	};
};


// 구글 테스트 및 Verifier 자체적으로 처리하는 인자 방식은 다음과 같음.
// R:\GameRes JPN GachaJP --gtest_filter=gachapon_test.* --gtest_output="xml:result.xml"
// [리소스 폴더] [검사할 국가] [검사할 테이블 파일명] [gtest 인자들]
// [검사할 테이블 파일명] 은 multitable id duplicate test 인 경우엔 관련된 테이블을 모두 모아서 테스트 한다. ; 를 구분자로 여러 테이블을 입력받는다. (aaa;bbb ...)
// R:\GameRes MAIN aaa;ItemTable --gtest_filter=multitable_id_duplicate_test.* --gtest_output="xml:result.xml"
// 마지막으로 반영된 이슈 #26504

// 아이템 테이블 id 중복 검사 및 모든 테스트 수행.
// R:\Gameres MAIN EnchantJewelTable;ItemCompoundGroupTable;ItemCompoundTable;ItemDropGroupTable;ItemDropTable;ItemTable;MonsterGroupTable;MonsterSkillTable;MonsterTable;PartsTable;PotentialJewelTable;PotentialTable;SetItemTable;SkillLevelTable;SkillTable;testtable;VehicleTable;WeaponTable --gtest_filter=*.* --gtest_output="xml:D:\testresult.xml

int _tmain(int argc, _TCHAR* argv[])
{
	g_strResourcePath.assign( argv[ 1 ] );

	char acBuffer[ MAX_PATH ] = { 0 };
	WideCharToMultiByte( CP_ACP, NULL, g_strResourcePath.c_str(), (int)_tcslen(g_strResourcePath.c_str()), 
						 acBuffer, sizeof(acBuffer), NULL, NULL );
	g_strResourcePathA.assign( acBuffer );

	g_mapExtFolderPath.insert( make_pair( L"MAIN", g_strResourcePathA+"\\Resource\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"KOR", g_strResourcePathA+"\\Resource_KOR\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"JPN", g_strResourcePathA+"\\Resource_JPN\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"CHN", g_strResourcePathA+"\\Resource_CHN\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"USA", g_strResourcePathA+"\\Resource_USA\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"TWN", g_strResourcePathA+"\\Resource_TWN\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"SIN", g_strResourcePathA+"\\Resource_SIN\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"THA", g_strResourcePathA+"\\Resource_THA\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"IDN", g_strResourcePathA+"\\Resource_IDN\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"RUS", g_strResourcePathA+"\\Resource_RUS\\Ext" ) );
	g_mapExtFolderPath.insert( make_pair( L"EU", g_strResourcePathA+"\\Resource_EU\\Ext" ) );

	// 인자에서 국가 설정을 받아서 잘라낸 다음에 넘겨줌.
	map<wstring, string>::iterator iter = g_mapExtFolderPath.find( argv[ 2 ] );
	if( g_mapExtFolderPath.end() != iter )
		g_strNationExtFolder = iter->second;
	else
	{
		_tprintf( _T("[DnTableVerifier] Invalid nation setting!! Running with korea resource path!!\n\n") );
		g_strNationExtFolder = g_mapExtFolderPath[ L"KOR" ];
	}

	g_strNation = iter->first;

	char* pTableName = acBuffer;
	int iTableNameLength = (int)_tcslen(argv[ 3 ]);
	bool bTableNameBufferAllocated = false;
	if( sizeof(acBuffer) <= iTableNameLength )
	{
		pTableName = new char[ iTableNameLength+1 ];
		bTableNameBufferAllocated = true;
	}

	SecureZeroMemory( pTableName, (iTableNameLength+1) );
	WideCharToMultiByte( CP_ACP, NULL, argv[ 3 ], (int)_tcslen(argv[ 3 ]), 
						 pTableName, iTableNameLength, NULL, NULL );

	g_strTableName.assign( pTableName );

	::testing::AddGlobalTestEnvironment( new gtest_global_environment );

	printf( "[DnTableVerifier] Running with \"%s\" table folder\n", g_strNationExtFolder.c_str() );

	::testing::InitGoogleTest( &argc, argv );

	int iExitCode = RUN_ALL_TESTS();
	g_ActionCoreMng.Finalize();

	if( bTableNameBufferAllocated )
		delete [] pTableName;

	return iExitCode;
}

