#pragma once

//////////////////////////////////////////////////////////////////////////
// ������ ���� (���ҽ� ���̺귯�� �� PatchConfigList ����)

//	_USE_COMMAND_LINE			// Ŀ�ǵ���θ� ���
//	_USE_SINGLE_CLIENT			// �̱� Ŭ���̾�Ʈ�� ����
//	_USE_PARTITION_SELECT		// ��Ƽ�� ���� ��� ����
//	_USE_MULTILANGUAGE			// �ٱ��� ����
//	_USE_SPLIT_COMPRESS_FILE	// ���� ����(RAR) ���� �ٿ�ε� ����


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

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://dn-nexon14.ktics.co.kr/" )
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

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://down.hangame.co.jp/jp/purple/dist/j_dnest/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
#elif defined(_CHN)
	#define _USE_PARTITION_SELECT
	#define _USE_SINGLE_CLIENT
	#include "ResourceCHN/resource.h"
	#include "ResourceCHN/ResourceInit.h"
	#define DLL_RESOURCE_FILE	"ResourceCHN/ResourceCHN.rc"
	#if defined _DEBUG
		#pragma comment( lib, "Output/Debug_CHN/ResourceCHN.lib" )
	#else
		#pragma comment( lib, "Output/Release_CHN/ResourceCHN.lib" )
	#endif

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://dorado.sdo.com/dn/sndalist/" )
	#define	PATCHCONFIG_LIST		_T( "sndalist.xml")
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

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://tw.patch.dragonnest.beanfun.com/dragonnest/" )
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

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://download2.nexon.net/Game/DragonNest/patch/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigList.xml" )
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

	// URL ����
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

	// URL ����
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

	// URL ����
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

	// URL ����
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

	// URL ����
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

	// URL ����
	#define	PATCHCONFIG_LIST_URL	_T( "http://dragonnest.nefficient.co.kr/Test/" )
	#define	PATCHCONFIG_LIST		_T( "PatchConfigListTest.xml" )
#endif


//////////////////////////////////////////////////////////////////////////
// ���� ����

#define DNLAUNCHER_NAME			"DnLauncher.exe"
#define DNLAUNCHER_NAME_TMP		"dnlauncher.tmp"
#define DNCLIENT_NAME			"DragonNest.exe"
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
#define _FIRST_PATCH
//#define _USE_RTPATCH
//#define _USE_BITTORRENT
