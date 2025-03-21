#ifndef SDOA4CLIENT_H
#define SDOA4CLIENT_H


#include <objbase.h>
#include "windows.h"

#pragma pack(push,1)

// define SDOA SDK VERSION 
#define SDOA_SDK_VERSION                 0x0200

// define returned value
#define SDOA_OK                      0
#define SDOA_FALSE                   1

// buffer is insufficient
#define SDOA_INSUFFICIENT_BUFFER     122  

// message of SDOA runs
#define SDOA_WM_CLIENT_RUN			0xA604  // WM_USER + 0xA204
// message sent to game client by igw
#define SDOA_WM_CLIENT_NOTIFY		0xA610  // WM_USER + 0xA210
// wParam: send event ID; lParam: expanding information��0:no expanding information
#define SDOA_NOTIFY_SHOWLOGIN     1    //  display login dialog box
#define SDOA_NOTIFY_LOGINCLICK    2    //  click "login" button
// message sent to igw by game client
#define SDOA_WM_HOST_NOTIFY			0xA611	// WM_USER + 0xA211


// define game graphics engine type
#define SDOA_RENDERTYPE_UNKNOWN             0x00
#define SDOA_RENDERTYPE_GDI                 0x01
#define SDOA_RENDERTYPE_DDRAW               0x04
#define SDOA_RENDERTYPE_D3D7                0x08
#define SDOA_RENDERTYPE_D3D8                0x10
#define SDOA_RENDERTYPE_D3D9                0x20
#define SDOA_RENDERTYPE_D3D10               0x40
#define SDOA_RENDERTYPE_OPENGL              0x80

//  screen status controlled by user
#define SDOA_SCREEN_NONE				0		//  not initialized Screen
#define SDOA_SCREEN_NORMAL				1		// displayed screen normally
#define SDOA_SCREEN_MINI				2		// minimized screen


//  status control for IWidgetWindows internal
#define SDOA_WINDOWSSTATUS_NONE			0		// not initialized a window 
#define SDOA_WINDOWSSTATUS_SHOW			1		// show the window 
#define SDOA_WINDOWSSTATUS_HIDE			2		// hide the window
// window types
#define SDOA_WINDOWSTYPE_NONE		0       // unknown type
#define SDOA_WINDOWSTYPE_TOOLBAR	1		// toolbar type
#define SDOA_WINDOWSTYPE_WIDGET		2		// Widget type
#define SDOA_WINDOWSTYPE_WINDOWS	3		// MSwindows type

// open widget items
#define SDOA_OPENWIDGET_DEFAULT			0		// default
#define SDOA_OPENWIDGET_NOBARICON		1		// open a widget without add it to the toolbar

// constants for  SetSelfCursor
#define SDOA_CURSOR_AUTO		0	// auto-detect cursor rendor 
#define SDOA_CURSOR_SELF		1	// cursor rendor 
#define SDOA_CURSOR_HIDESELF	2	// not cursor rendor 

//  login method
#define SDOA_LOGINMETHOD_SSO        0   // login method��SSO
#define SDOA_LOGINMETHOD_SDO        1   // login method��SDO(CAS)

// login status
#define SDOA_LOGINSTATE_UNKNOWN     -1  // unknown(mistake prm or state exception )
#define SDOA_LOGINSTATE_NONE        0   //  not call the login method
#define SDOA_LOGINSTATE_OK          1   //  login seccussfully
#define SDOA_LOGINSTATE_CONNECTING  2   // longing in
#define SDOA_LOGINSTATE_FAILED      3   //failed login
#define SDOA_LOGINSTATE_LOGOUT      4   // logout

// Quanquan toolbar mode
#define SDOA_BARMODE_ICONBOTTOM		0   // bar before Quanquan icon
#define SDOA_BARMODE_ICONTOP		1   // bar above Quanquan icon

struct AppInfo {
	DWORD		cbSize;				// = sizeof(AppInfo)
	int			nAppID;				// only identification of app integrating sdoa 
	LPCWSTR		pwcsAppName;		// app name
	LPCWSTR		pwcsAppVer;		   	// app version
	int			nRenderType;		// app's graphics engin type (type:SDOA_RENDERTYPE_UNKNOWN..SDOA_RENDERTYPE_OPENGL)
	int			nMaxUser;			// number of players per computer in one game(1 is normal;but KOF allows 2)
	int			nAreaId;			//  district id of app(-1:unavailable)
	int			nGroupId;           // group id of app(-1:unavailable)
};


struct RoleInfo{
	DWORD		cbSize;                 // sizeof(RoleInfo)
	LPCWSTR		pwcsRoleName;			// nickname
	int			nSex;                   // gender(0:female;1:male)
	// more
};

struct SDOAWinProperty{
	int nWinType;				// window type(indentifying only but can't be set)
	int nStatus;				// window status
	// position,width,height
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;	
};


struct LoginResult {
	DWORD  cbSize;			// sizeof(LoginResult)��for augmentability
	LPCSTR szSessionId;		//  for token validating
	LPCSTR szSndaid;		//  user id
	LPCSTR szIdentityState;	// 0:adult��1:youth 
	LPCSTR szAppendix;		// additional information , reserved
	// added in 2009.09.02
	LPCSTR szAdditional;    // additional information;return it while call LoginDirect
};

 struct LoginMessage {
   DWORD  dwSize;
   int    nErrorCode;   // error code returned by login component,and details by pbstrContent
   BSTR*  pbstrTitle;	// this is reminding title of reminding message;and it can be changed by calling SysReallocString.IGW will call SysFreeString to release allocated resource after call back the result
   BSTR*  pbstrContent;	// this is reminding message content;and it can be changed by calling SysReallocString.IGW will call SysFreeString to release allocated resource after call back the result
 };

//  definition of ErrorCode in LPLOGINCALLBACKPROC
#define SDOA_ERRORCODE_OK               0    // login succussfully
#define SDOA_ERRORCODE_CANCEL           -1   // user closes login dialog box
#define SDOA_ERRORCODE_UILOST           -2   //  UI files lost or invalid while loading dialog box
#define SDOA_ERRORCODE_FAILED           -3   // fail to login
#define SDOA_ERRORCODE_UNKNOWN          -4   // unknown error 
#define SDOA_ERRORCODE_INVALIDPARAM     -5   // invalid prm.
#define SDOA_ERRORCODE_UNEXCEPT         -6   // unknown exception
#define SDOA_ERRORCODE_ALREAYEXISTS     -7   // already exist
#define SDOA_ERRORCODE_SHOWMESSAGE      -10  // show the message dialog box


//  callback function of login
// nErrorCode:     return errorc code of login
// pLoginResult:   return value of login which is accountable while nErrorCode = SDOA_ERRORCODE_OK 
// nUserData:      user data while calling ShowLoginDialog
// nReserved:      reseved for the future,it is 0 currently(it is not prm. Reserved in ShowLoginDialog)
// return value:    while return value is true:close the dialog box,while it is false,open it
//
// 2009.03.30
// extended feature��while nErrorCode = ERRORCODE_SHOWMESSAGE,Reserved returns result of LoginMessage
//  return true:disable IGW to pop up dialog box,and game can pop up its own dialog box for users
// return false:IGW still can pop up dialog box,and game can call SysReallocString to change Title,Content so as to difine its own error information for users
//  IGW will call SysFreeString to release allocated resource after calling result back 
typedef BOOL (CALLBACK *LPLOGINCALLBACKPROC)(int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved);

// application interfaces
typedef interface ISDOAApp ISDOAApp;
typedef ISDOAApp *PSDOAApp, *LPSDOAApp;

MIDL_INTERFACE("C8187484-1C5F-48DF-8497-2136A72A5733")
ISDOAApp : public IUnknown
{
public:
	// display login dialog box
	STDMETHOD_(int,ShowLoginDialog)(THIS_ LPLOGINCALLBACKPROC fnCallback, int nUserData, int nReserved) PURE;

	STDMETHOD_(void,ModifyAppInfo)(THIS_ const AppInfo* pAppInfo) PURE;
	STDMETHOD_(void,Logout)(THIS) PURE;
	STDMETHOD_(int,GetLoginState)(THIS_ int nLoginMethod) PURE;
	STDMETHOD_(void,SetRoleInfo)(THIS_ const RoleInfo* pRoleInfo) PURE;
	

	// display payment dialog box
	STDMETHOD_(int,ShowPaymentDialog)(THIS_ LPCWSTR pwcsSrc) PURE;

	// SDOA UI congfiguration
	STDMETHOD_(int,GetScreenStatus)(THIS) PURE;
	STDMETHOD_(void,SetScreenStatus)(THIS_ int nValue) PURE;
	STDMETHOD_(bool,GetScreenEnabled)(THIS) PURE;
	STDMETHOD_(void,SetScreenEnabled)(THIS_ bool bValue) PURE;
	STDMETHOD_(bool,GetTaskBarPosition)(THIS_ PPOINT ptPosition) PURE;
	STDMETHOD_(bool,SetTaskBarPosition)(THIS_ const PPOINT ptPosition) PURE;
	STDMETHOD_(bool,GetFocus)(THIS) PURE;
	STDMETHOD_(void,SetFocus)(THIS_ bool bValue) PURE;
	STDMETHOD_(bool,HasUI)(THIS_ const PPOINT ptPosition) PURE;
	
	STDMETHOD_(int,GetTaskBarMode)(THIS) PURE;
	STDMETHOD_(void,SetTaskBarMode)(THIS_ int nBarMode) PURE;
	STDMETHOD_(int,GetSelfCursor)(THIS) PURE;
	STDMETHOD_(void,SetSelfCursor)(THIS_ int nValue) PURE;

	// general functions
	STDMETHOD_(int,OpenWidget)(THIS_ LPCWSTR pwcsWidgetNameSpace) PURE;
	STDMETHOD_(int,OpenWidgetEx)(THIS_ LPCWSTR pwcsWidgetNameSpace, LPCWSTR pwcsParam, int nFlag) PURE;
	STDMETHOD_(int,CloseWidget)(THIS_ LPCWSTR pwcsWidgetNameSpace) PURE;
	STDMETHOD_(int,ExecuteWidget)(THIS_ LPCWSTR pwcsWidgetNameSpaceOrGUID, LPCWSTR pwcsParam) PURE;
	STDMETHOD_(int,WidgetExists)(THIS_ LPCWSTR pwcsWidgetNameSpaceOrGUID) PURE;
	STDMETHOD_(int,OpenWindow)(THIS_ LPCWSTR pwcsWinType, LPCWSTR pwcsWinName, LPCWSTR pwcsSrc, int nLeft, int nTop, int nWidth, int nHeight, LPCWSTR pwcsMode) PURE;
	STDMETHOD_(int,CloseWindow)(THIS_ LPCWSTR pwcsWinName) PURE;

	STDMETHOD_(int,GetWinProperty)(THIS_ LPCWSTR pwcsWinName, SDOAWinProperty* pWinProperty) PURE;
	STDMETHOD_(int,SetWinProperty)(THIS_ LPCWSTR pwcsWinName, SDOAWinProperty* pWinProperty) PURE;
};


typedef interface ISDOAApp2 ISDOAApp2;
typedef ISDOAApp2 *PSDOAApp2, *LPSDOAApp2;

MIDL_INTERFACE("3F35136C-7061-4E69-BF3A-CC78D026F48F")
ISDOAApp2 : public ISDOAApp
{
public:
	STDMETHOD_(int,LoginDirect)(THIS_ LPCSTR szSessionId, LPCSTR szAdditional, int nReserved) PURE;
	STDMETHOD_(int,GetClientSignature)(THIS_ LPCWSTR szIndication, BSTR* Signature) PURE;
};


// sound-controlling callback by application
// volumn scope:0-100
typedef DWORD (CALLBACK *LPGETAUDIOSOUNDVOLUME)();
typedef void  (CALLBACK *LPSETAUDIOSOUNDVOLUME)(DWORD nNewVolume);
typedef DWORD (CALLBACK *LPGETAUDIOEFFECTVOLUME)();
typedef void  (CALLBACK *LPSETAUDIOEFFECTVOLUME)(DWORD nNewVolume);

// channel interface for common data
class ICommonChannel
{
public:
	// the following methods is for Delphi rather than c++;reserved
	virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject) = 0;
	virtual ULONG _stdcall AddRef(void) = 0;
	virtual ULONG _stdcall Release(void) = 0;

	virtual void _stdcall SendData(BSTR Request, BSTR* Response) = 0;
	virtual void _stdcall Set_RevertChannel(ICommonChannel *pReverter) = 0;
	virtual ICommonChannel* _stdcall Get_RevertChannel(void) = 0;
	virtual void _stdcall Close(void) = 0;
	virtual void _stdcall Get_ChannelType(LPSTR szChannelType, DWORD nBufferLen) = 0; // max BufferLen = 64
};
typedef ICommonChannel *PCommonChannel;

typedef BOOL (WINAPI *LPCREATECHANNEL)(LPCSTR szChannelType, ICommonChannel **pChannel);


typedef interface ISDOAAppUtils ISDOAAppUtils;
typedef ISDOAAppUtils *PSDOAAppUtils, *LPSDOAAppUtils;

MIDL_INTERFACE("1170C2F9-28AD-4EA8-8392-E9A219C8FF65")
ISDOAAppUtils : public IUnknown
{
public:
	//  audio control
	// set audio control call-back
 	STDMETHOD_(void,SetAudioSetting)(THIS_ 
		LPGETAUDIOSOUNDVOLUME  fnGetAudioSoundVolume,
		LPSETAUDIOSOUNDVOLUME  fnSetAudioSoundVolume,
		LPGETAUDIOEFFECTVOLUME fnGetAudioEffectVolume,
		LPSETAUDIOEFFECTVOLUME fnSetAudioEffectVolume ) PURE;
	// inform game that audio changed                           
	STDMETHOD_(void,NodifyAudioChanged)(THIS) PURE;	
	//  set creating channel call-back
	STDMETHOD_(void,SetCreateChannelCallback)(THIS_ const LPCREATECHANNEL fnCreateChannel) PURE;	
};

// asychronous call-back
typedef void (CALLBACK *LPQUERYCALLBACK)(int nRetCode, int nUserData);

typedef interface ISDOAClientService ISDOAClientService;
typedef ISDOAClientService *PSDOAClientService, *LPSDOAClientService;

MIDL_INTERFACE("AF56D291-823A-41AA-85A0-EBE5C6163425")
ISDOAClientService : public IUnknown
{
public:
	STDMETHOD_(int, SetValue)(THIS_ const BSTR bstrKey, const BSTR bstrValue ) PURE;
	STDMETHOD_(int, Query)(THIS_ const BSTR bstrService) PURE;	
	STDMETHOD_(int, AsyncQuery)(THIS_ const BSTR bstrService, LPQUERYCALLBACK fnCallback, int nUserData) PURE;	
	STDMETHOD_(int, GetValue)(THIS_ const BSTR bstrKey, BSTR* bstrValue) PURE;	
};

/* dwSdkVersion in igwInitialize uses macro definition SDOA_SDK_VERSION */
typedef int  (WINAPI* LPigwInitialize)(DWORD dwSdkVersion, const AppInfo* pAppInfo);
typedef bool (WINAPI* LPigwGetModule)(REFIID riid, void** intf);
typedef int  (WINAPI* LPigwTerminal)();

#pragma pack(pop)
#endif /* SDOA4CLIENT_H */