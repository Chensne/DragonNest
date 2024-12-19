#pragma once

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <tchar.h>
#include <time.h>
#include <sql.h>
#include <sqlucode.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <assert.h>
#include <conio.h>
#include <hash_map>
#include <mswsock.h>


#include <set>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <map>
#include <string>
#include <utility>

#include "SundriesFunc.h"
#include "CriticalSection.h"

#define DNHEADER	__DEFINED__		// 프로젝트 내 하위 소스코드 (.h 등등) 에서 'DNPrjSafe.h' 를 기본헤더 포함없이 포함시켰을 경우 오류방지

#include "DNConfig.h"
#include "DNCommonDef.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"
#if defined( _SERVER )
#include "DNServerPacketDB.h"
#include "DNServerPacketGame.h"
#include "DNServerPacketLogin.h"
#include "DNServerPacketMaster.h"
#include "DNServerPacketVillage.h"
#include "DNServerPacketCash.h"
#endif // #if defined( _SERVER )
#include "DNServerProtocol.h"

using namespace std;

#pragma warning( disable:4819 )
#pragma warning( disable:4996 )

// D3D
#if defined(_VILLAGESERVER) || defined(_GAMESERVER)
#define DIRECTINPUT_VERSION 0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#pragma warning(default:4819)
// DirectX Lib 
#pragma comment(lib,"d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib,"d3dx9d.lib")
#else 
#pragma comment(lib,"d3dx9.lib")
#endif // _DEBUG
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")
#endif // _VILLAGESERVER, _GAMESERVER

#pragma comment(lib,"winmm.lib")

// Network Link
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib, "Gdi32.lib")

#pragma comment(lib, "user32.lib")
