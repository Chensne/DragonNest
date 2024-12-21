#ifndef GPLUSDEF_H
#define GPLUSDEF_H

#include <objbase.h>
#include "windows.h"

#pragma pack(push,1)

// GPLUS_SDK_VERSION 表示一套接入的SDK版本号
#define GPLUS_SDK_VERSION                    0x0001

// GPLUS 返回值
#define GPLUS_OK                             0
#define GPLUS_FALSE                          1


// 游戏图形引擎类型
#define GPLUS_RENDERTYPE_UNKNOWN             0
#define GPLUS_RENDERTYPE_GDI                 1
#define GPLUS_RENDERTYPE_OPENGL              2
#define GPLUS_RENDERTYPE_DDRAW               6
#define GPLUS_RENDERTYPE_D3D7                7
#define GPLUS_RENDERTYPE_D3D8                8
#define GPLUS_RENDERTYPE_D3D9                9
#define GPLUS_RENDERTYPE_D3D10               10
#define GPLUS_RENDERTYPE_D3D11               11

//应用程序信息
struct GPlusAppInfo {
    DWORD		cbSize;				// = sizeof(AppInfo)
    int			nAppID;				// 盛大统一游戏ID
    LPCWSTR		pwcsAppName;		// 游戏名称
    LPCWSTR     pwcsAppVer;         // 游戏版本
    int			nRenderType;		// 游戏图形引擎类型 (取值GPLUS_RENDERTYPE_UNKNOWN...)
    int         nDeviceIndex;       // 显卡索引.(内部使用.填0)
    int			nAreaId;			// 游戏区ID	(注意：不可用时传-1)
    int			nGroupId;           // 游戏组(游戏服)ID	(注意：不可用时传-1)
};


// 用户信息
struct GPlusUserInfo {
   DWORD		cbSize;              // = sizeof(GPLusUserInfo)
   LPCWSTR      pwcsUserId;		     // 帐号标识(数字id)
   LPCWSTR      pwcsUserName;	     // 用户名(昵称)
};

/* 注意LPGPlusInitialize中dwSdkVersion参数使用宏 GPLUS_SDK_VERSION ,返回GPLUS_OK或者GPLUS_FALSE*/
typedef int  (WINAPI* LPGPlusInitialize)(DWORD dwSdkVersion, const GPlusAppInfo* pAppInfo);
/* 查询接口 ,返回GPLUS_OK或者GPLUS_FALSE*/
typedef int  (WINAPI* LPGPlusGetModule)(REFIID riid, void** intf);
/* 销毁gplus */
typedef void (WINAPI* LPGPlusTerminal)();

#pragma pack(pop)
#endif /* GPLUSCLIENT_H */