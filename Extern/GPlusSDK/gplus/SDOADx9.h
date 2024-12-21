#ifndef SDOADX9_H
#define SDOADX9_H


#include <d3d9.h>

typedef interface ISDOADx9 ISDOADx9;
typedef ISDOADx9 *PSDOADx9, *LPSDOADx9;

MIDL_INTERFACE("59B789AB-BEE9-4C68-AA23-BCA9AB1A3E50")
ISDOADx9 : public IUnknown
{
public:
	/* initialize d3d device */
	STDMETHOD_(HRESULT,Initialize)(THIS_ IDirect3DDevice9* pDev9, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd) PURE;	
	/* call it after BeginScene and EndScene while rendering */
	STDMETHOD_(HRESULT,Render)(THIS) PURE;	
	/* call renderEx between BeginScene and EndScene */
	STDMETHOD_(HRESULT,RenderEx)(THIS) PURE;	
	/* release d3d resource */
	STDMETHOD_(HRESULT,Finalize)(THIS) PURE;
	/* reset divice */
	STDMETHOD_(void,OnDeviceReset)(THIS_ D3DPRESENT_PARAMETERS* pParams) PURE;
	/* device lost */
	STDMETHOD_(void,OnDeviceLost)(THIS) PURE;
	/* process message */
	STDMETHOD_(HRESULT,OnWindowProc)(THIS_ HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResult) PURE;
};

#endif /* SDOADX9_H */