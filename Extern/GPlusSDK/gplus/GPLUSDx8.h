#ifndef GPLUSDX8_H
#define GPLUSDX8_H


#include <d3d8.h>

typedef interface IGPLUSDx8 IGPLUSDx8;
typedef IGPLUSDx8 *PGPLUSDx8, *LPGPLUSDx8;

MIDL_INTERFACE("436C8ABC-5B8C-4f45-A872-1103BB56834B")
IGPLUSDx8 : public IUnknown
{
public:
	/* 初始化d3d设备 */
	STDMETHOD_(HRESULT,Initialize)(THIS_ IDirect3DDevice8* pDev8, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd) PURE;	
	/* 渲染界面内部已经调用BeginScene和EndScene，确认在EndScene之后调用它 */
	STDMETHOD_(HRESULT,Render)(THIS) PURE;	
	/* 确保在BeginScene、EndScene之间调用RenderEx */
	STDMETHOD_(HRESULT,RenderEx)(THIS) PURE;	
	/* 释放d3d相关资源 */
	STDMETHOD_(HRESULT,Finalize)(THIS) PURE;
	/* 重置设备 */
	STDMETHOD_(void,OnDeviceReset)(THIS_ D3DPRESENT_PARAMETERS* pParams) PURE;
	/* 设备丢失 */
	STDMETHOD_(void,OnDeviceLost)(THIS) PURE;
};



//c interface
typedef HRESULT (WINAPI* LPGPlusDx8Initialize)(IDirect3DDevice8* pDev8, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd);
typedef HRESULT (WINAPI* LPGPlusDx8Render)( ) ;
typedef HRESULT (WINAPI* LPGPlusDx8RenderEx)( ) ;
typedef HRESULT (WINAPI* LPGPlusDx8Finalize)( );
typedef void (WINAPI* LPGPlusDx8OnDeviceReset)( D3DPRESENT_PARAMETERS* pParams);
typedef void (WINAPI* LPGPlusDx8OnDeviceLost)( )  ;


#endif /* GPLUSDX8_H */