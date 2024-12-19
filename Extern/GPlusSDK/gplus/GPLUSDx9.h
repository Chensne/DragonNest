#ifndef GPLUSDX9_H
#define GPLUSDX9_H


#include <d3d9.h>

//c++ interface
typedef interface IGPLUSDx9 IGPLUSDx9;
typedef IGPLUSDx9 *PGPLUSDx9, *LPGPLUSDx9;

MIDL_INTERFACE("2C62F176-6801-47f2-BB84-C52DBAFE7585")
IGPLUSDx9 : public IUnknown
{
public:
	/* 初始化d3d设备 ,返回GPLUS_OK或者GPLUS_FALSE*/
	STDMETHOD_(HRESULT,Initialize)(THIS_ IDirect3DDevice9* pDev9, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd) PURE;	
	/* 渲染界面内部已经调用BeginScene和EndScene，确认在EndScene之后调用它 ,返回GPLUS_OK或者GPLUS_FALSE*/
	STDMETHOD_(HRESULT,Render)(THIS) PURE;	
	/* 确保在BeginScene、EndScene之间调用RenderEx ,返回GPLUS_OK或者GPLUS_FALSE*/
	STDMETHOD_(HRESULT,RenderEx)(THIS) PURE;	
	/* 释放d3d相关资源 ,返回GPLUS_OK或者GPLUS_FALSE*/
	STDMETHOD_(HRESULT,Finalize)(THIS) PURE;
	/* 重置设备 */
	STDMETHOD_(void,OnDeviceReset)(THIS_ D3DPRESENT_PARAMETERS* pParams) PURE;
	/* 设备丢失 */
	STDMETHOD_(void,OnDeviceLost)(THIS) PURE;
};

//c interface
typedef HRESULT (WINAPI* LPGPlusDx9Initialize)(IDirect3DDevice9* pDev9, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd);
typedef HRESULT (WINAPI* LPGPlusDx9Render)( ) ;
typedef HRESULT (WINAPI* LPGPlusDx9RenderEx)( ) ;
typedef HRESULT (WINAPI* LPGPlusDx9Finalize)( );
typedef void (WINAPI* LPGPlusDx9OnDeviceReset)( D3DPRESENT_PARAMETERS* pParams);
typedef void (WINAPI* LPGPlusDx9OnDeviceLost)( )  ;



#endif /* GPLUSDX9_H */