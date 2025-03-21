#ifndef GPLUSCLIENT_H
#define GPLUSCLIENT_H

#include "GPLUSDef.h"

//c++ interface
// 应用接口
typedef interface IGPLUSApp IGPLUSApp;
typedef IGPLUSApp *PGPLUSApp, *LPGPLUSApp;

MIDL_INTERFACE("7D9035E6-78BB-4a2e-A09D-FE48BD618144")
IGPLUSApp : public IUnknown
{
public:
    /* 修改应用程序相关信息 */
    STDMETHOD_(void,ModifyAppInfo)(THIS_ const GPlusAppInfo* pAppInfo) PURE;
    /* 登录gplus */
    STDMETHOD_(void,Login)(THIS_ const GPlusUserInfo* pUserInfo) PURE;
    /* 注销gplus */
    STDMETHOD_(void,Logout)(THIS) PURE;
    /* 获取gplus登录状态(TRUE:已登录;FALSE:未登录) */
    STDMETHOD_(BOOL,GetLoginState)(VOID) PURE;

    /* 传入消息 ,返回GPLUS_OK或者GPLUS_FALSE*/
    STDMETHOD_(HRESULT,OnWindowProc)(THIS_ HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResult) PURE;
    /* 获取在某点是否有ui(TRUE:有;FALSE:无) */
    STDMETHOD_(BOOL,HasUI)(THIS_ const PPOINT ptPosition) PURE;
    /* 隐藏/显示gplus*/
    STDMETHOD_(void,Show)(THIS_ BOOL bShow) PURE;
    /* 设置gplus显示位置*/
    STDMETHOD_(void,MoveTo)(THIS_ const PPOINT ptPosition) PURE;
};


//c interface
typedef void  (WINAPI* LPGPlusModifyAppInfo)(const GPlusAppInfo* pAppInfo) ;
typedef void  (WINAPI* LPGPlusLogin)(const GPlusUserInfo* pUserInfo);
typedef void  (WINAPI* LPGPlusLogout)();
typedef BOOL  (WINAPI* LPGPlusGetLoginState)();
typedef HRESULT (WINAPI* LPGPlusOnWindowProc)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResult) ;
typedef BOOL (WINAPI* LPGPlusHasUI)(const PPOINT ptPosition) ;
typedef void (WINAPI* LPGPlusShow)(BOOL bShow);
typedef void (WINAPI* LPGPlusMoveTo)(const PPOINT ptPosition);

#endif /* GPLUSCLIENT_H */