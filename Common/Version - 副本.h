#pragma once

const int g_nBuildVersionMajor = 0;
const int g_nBuildVersionMinor = 0;
static char *g_szBuildVersionRevision = {"0"};

const int g_nBuildTimeYear = 2013;
const int g_nBuildTimeMonth = 2;
const int g_nBuildTimeDay = 15;
const int g_nBuildTimeHour = 16;
const int g_nBuildTimeMinute = 11;

const int g_nBuildCount = 5850;
const int g_nBuildResourceRevision = 219877;
const int g_nBuildSourceRevision = 61711;
static char *g_szBuildString = {""};

//--------------------------------------------------------------
//?a¨¢?¦Ì???¨º¡À¦Ì?¡ã?¡À?o?D¡ê?¨¦o¨ª?t¡ä??¨¹??
#if defined(_LOGINSERVER)
static char * szVersion = { "2.1892.19" };
#else
static char * szVersion = { "2.1892.19" };  //{ "1.1186.18" };
#endif	// #if defined(_LOGINSERVER)

Version_Server: nNation=31 cVersion=6 Major=1 Minor=1186
Version_Client: nNation=31 cVersion=6 Major=1 Minor=1392

static char * szVersion_Client_TW = { "2.1892.19" };  //[debug] ?¨ª?¡ì??¡Á?D?¦Ì?¡ã?¡À?o?
//--------------------------------------------------------------



static char * szLoginVersion =  { "2.1892.19" };  //{ "1.1186.18" };
static char * szMasterVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szVillageVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szGameVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szDBVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szLogVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szCashVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szServiceManagerVersion = { "2.1892.19" };  //{ "1.1186.18" };
static char * szNetLauncherVersion = { "2.1892.19" };  //{ "1.1186.18" };
