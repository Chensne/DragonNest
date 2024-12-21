#ifndef GPLUSDEF_H
#define GPLUSDEF_H

#include <objbase.h>
#include "windows.h"

#pragma pack(push,1)

// GPLUS_SDK_VERSION ��ʾһ�׽����SDK�汾��
#define GPLUS_SDK_VERSION                    0x0001

// GPLUS ����ֵ
#define GPLUS_OK                             0
#define GPLUS_FALSE                          1


// ��Ϸͼ����������
#define GPLUS_RENDERTYPE_UNKNOWN             0
#define GPLUS_RENDERTYPE_GDI                 1
#define GPLUS_RENDERTYPE_OPENGL              2
#define GPLUS_RENDERTYPE_DDRAW               6
#define GPLUS_RENDERTYPE_D3D7                7
#define GPLUS_RENDERTYPE_D3D8                8
#define GPLUS_RENDERTYPE_D3D9                9
#define GPLUS_RENDERTYPE_D3D10               10
#define GPLUS_RENDERTYPE_D3D11               11

//Ӧ�ó�����Ϣ
struct GPlusAppInfo {
    DWORD		cbSize;				// = sizeof(AppInfo)
    int			nAppID;				// ʢ��ͳһ��ϷID
    LPCWSTR		pwcsAppName;		// ��Ϸ����
    LPCWSTR     pwcsAppVer;         // ��Ϸ�汾
    int			nRenderType;		// ��Ϸͼ���������� (ȡֵGPLUS_RENDERTYPE_UNKNOWN...)
    int         nDeviceIndex;       // �Կ�����.(�ڲ�ʹ��.��0)
    int			nAreaId;			// ��Ϸ��ID	(ע�⣺������ʱ��-1)
    int			nGroupId;           // ��Ϸ��(��Ϸ��)ID	(ע�⣺������ʱ��-1)
};


// �û���Ϣ
struct GPlusUserInfo {
   DWORD		cbSize;              // = sizeof(GPLusUserInfo)
   LPCWSTR      pwcsUserId;		     // �ʺű�ʶ(����id)
   LPCWSTR      pwcsUserName;	     // �û���(�ǳ�)
};

/* ע��LPGPlusInitialize��dwSdkVersion����ʹ�ú� GPLUS_SDK_VERSION ,����GPLUS_OK����GPLUS_FALSE*/
typedef int  (WINAPI* LPGPlusInitialize)(DWORD dwSdkVersion, const GPlusAppInfo* pAppInfo);
/* ��ѯ�ӿ� ,����GPLUS_OK����GPLUS_FALSE*/
typedef int  (WINAPI* LPGPlusGetModule)(REFIID riid, void** intf);
/* ����gplus */
typedef void (WINAPI* LPGPlusTerminal)();

#pragma pack(pop)
#endif /* GPLUSCLIENT_H */