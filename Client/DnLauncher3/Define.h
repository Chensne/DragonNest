#pragma once

//////////////////////////////////////////////////////////////////////////
// 국가별 정의 (리소스 라이브러리 및 PatchConfigList 정보)

//	_USE_COMMAND_LINE			// 커맨드라인만 사용
//	_USE_SINGLE_CLIENT			// 싱글 클라이언트만 지원
//	_USE_PARTITION_SELECT		// 파티션 선택 기능 지원
//	_USE_MULTILANGUAGE			// 다국어 지원
//	_USE_SPLIT_COMPRESS_FILE	// 분할 압축(RAR) 파일 다운로드 지원


#if defined(_KOR)
	#define _USE_COMMAND_LINE
	#define _USE_SINGLE_CLIENT
	#include "ResourceKOR/resource.h"
	#include "ResourceKOR/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceKOR/ResourceKOR.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_KOR/ResourceKOR.lib" )
	#else
		#pragma comment( lib, "Output/Release_KOR/ResourceKOR.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://dn-nexon14.ktics.co.kr/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_KRAZ)
	#define _USE_COMMAND_LINE
	#define _USE_SINGLE_CLIENT
	#include "ResourceKOR/resource.h"
	#include "ResourceKOR/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceKOR/ResourceKOR.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_KRAZ/ResourceKOR.lib" )
	#else
		#pragma comment( lib, "Output/Release_KRAZ/ResourceKOR.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://actoz.dn.nowcdn.co.kr/ClientPatch/Release_Client/" ) // 라이브용.
	//#define	PATCHCONFIG_LIST_URL	_T( "http://actoz.dn.nowcdn.co.kr/ClientPatch/Test_Client/" ) // 테스트용.
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_JPN)
	#define _USE_SINGLE_CLIENT
	#include "ResourceJPN/resource.h"
	#include "ResourceJPN/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceJPN/ResourceJPN.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_JPN/ResourceJPN.lib" )
	#else
		#pragma comment( lib, "Output/Release_JPN/ResourceJPN.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://down.hangame.co.jp/jp/purple/dist/j_dnest/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_CHN)
	//#define _USE_PARTITION_SELECT
	#define _USE_SINGLE_CLIENT
	#include "ResourceCHN/resource.h"
	#include "ResourceCHN/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceCHN/ResourceCHN.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_CHN/ResourceCHN.lib" )
	#else
		#pragma comment( lib, "Output/Release_CHN/ResourceCHN.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://update.51newsx.com:88/patch/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml")
	#define GPK_URL					"http://dorado.sdo.com/dn/GPK"
#elif defined(_TWN)
	#define _USE_SINGLE_CLIENT
	#include "ResourceTWN/resource.h"
	#include "ResourceTWN/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceTWN/ResourceTWN.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_TWN/ResourceTWN.lib" )
	#else
		#pragma comment( lib, "Output/Release_TWN/ResourceTWN.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://update.51newsx.com:88/patch/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_USA)
	#define _USE_SINGLE_CLIENT
	#include "ResourceUSA/resource.h"
	#include "ResourceUSA/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceUSA/ResourceUSA.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_USA/ResourceUSA.lib" )
	#else
		#pragma comment( lib, "Output/Release_USA/ResourceUSA.lib" )
	#endif

	// URL 정의
#ifdef _SKY
	#define	PATCHCONFIG_LIST_URL	_T( "http://cdn.dragonnest.ro/patch/" )
#else
	#define	PATCHCONFIG_LIST_URL	_T( "http://patch.dragonnestbrasil.net/" )

#endif
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )

	#define STORE_ACCOUNT "Settings.dat"
	#define STORE_ACCOUNT_VERSION 2
#ifdef _SKY
	#define XOR_KEY 0xFF
#else
	#define XOR_KEY 0xAB
#endif
#elif defined(_SG)
	#define _USE_SINGLE_CLIENT
	#include "ResourceSG/resource.h"
	#include "ResourceSG/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceSG/ResourceSG.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_SG/ResourceSG.lib" )
	#else
		#pragma comment( lib, "Output/Release_SG/ResourceSG.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://plaster.cherrycredits.com/dnest/patch/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
	#define GPK_URL					"http://gpk.cherrycredits.com/dn/updates"
#elif defined(_THAI)
	#define _USE_SINGLE_CLIENT
	#define _USE_SPLIT_COMPRESS_FILE
	#include "ResourceTHAI/resource.h"
	#include "ResourceTHAI/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceTHAI/ResourceTHAI.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_THAI/ResourceTHAI.lib" )
	#else
		#pragma comment( lib, "Output/Release_THAI/ResourceTHAI.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://auto-dgn.asiasoft.co.th/real/Client/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_EU)
	
	#define _USE_SINGLE_CLIENT
	#define _USE_MULTILANGUAGE
	#include "ResourceEU/resource.h"
	#include "ResourceEU/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceEU/ResourceEU.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_EU/ResourceEU.lib" )
	#else
		#pragma comment( lib, "Output/Release_EU/ResourceEU.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://cdn.dragonnest.ro/patch/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_IDN)
	#define _USE_SINGLE_CLIENT
	#define _USE_SPLIT_COMPRESS_FILE
	#include "ResourceIDN/resource.h"
	#include "ResourceIDN/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceIDN/ResourceIDN.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_IDN/ResourceIDN.lib" )
	#else
		#pragma comment( lib, "Output/Release_IDN/ResourceIDN.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://patch.gemscool.com/dragonnest/live/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_RUS)
	#define _USE_SINGLE_CLIENT
	#include "ResourceRUS/resource.h"
	#include "ResourceRUS/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceRUS/ResourceRUS.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_RUS/ResourceRUS.lib" )
	#else
		#pragma comment( lib, "Output/Release_RUS/ResourceRUS.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://dn-nexon14.ktics.co.kr/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_TEST)
	#define _USE_PARTITION_SELECT
	#define _USE_SPLIT_COMPRESS_FILE
	#define _USE_MULTILANGUAGE
	#include "ResourceTest/resource.h"
	#include "ResourceTest/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceTest/ResourceTest.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_Test/ResourceTest.lib" )
	#else
		#pragma comment( lib, "Output/Release_Test/ResourceTest.lib" )
	#endif

	// URL 정의
	#define	PATCHCONFIG_LIST_URL	_T( "http://dragonnest.nefficient.co.kr/Test/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigListTest.xml" )
#endif


//////////////////////////////////////////////////////////////////////////
// 공통 정의

#define DNLAUNCHER_NAME			"DnLauncher.exe"
#define DNLAUNCHER_NAME_TMP		"dnlauncher.tmp"
#ifdef _SKY
#define DNCLIENT_NAME			"Dragon.exe"
#else
#define DNCLIENT_NAME			"DragonNest.exe" //dragonnest.exe //rlkt
#endif
#define DNAUTOUPDATE_NAME		"DnAutoUpdate.exe"
#define CLIENT_VERSION_NAME		_T( "Version.cfg" )
#define SERVER_VERSION_NAME		_T( "PatchInfoServer.cfg" )
#define PATCHINFONAME			_T( "PatchConfig.cfg" )
#define FIRSTPATCHLIST_NAME		_T( "FirstPatchList.txt" )

#define FIRSTPATCH_CONTROL_NAME _T( "FirstPatchControlData.xml" )

#define RETRY_MAX_COUNT		3
#define __MAX_PATH			8192
#define LAUNCHER_VERSION	2
#define _USE_LOG
#define _USE_AUTOUPDATE
//#define ACTOZ_TEST_MESSAGE
//#define _FIRST_PATCH
//#define _USE_RTPATCH
