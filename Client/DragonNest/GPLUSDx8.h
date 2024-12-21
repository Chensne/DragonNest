#ifndef GPLUSDX8_H
#define GPLUSDX8_H


#include <d3d8.h>

typedef interface IGPLUSDx8 IGPLUSDx8;
typedef IGPLUSDx8 *PGPLUSDx8, *LPGPLUSDx8;

MIDL_INTERFACE("436C8ABC-5B8C-4f45-A872-1103BB56834B")
IGPLUSDx8 : public IUnknown
{
public:
	/* ��ʼ��d3d�豸 */
	STDMETHOD_(HRESULT,Initialize)(THIS_ IDirect3DDevice8* pDev8, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd) PURE;	
	/* ��Ⱦ�����ڲ��Ѿ�����BeginScene��EndScene��ȷ����EndScene֮������� */
	STDMETHOD_(HRESULT,Render)(THIS) PURE;	
	/* ȷ����BeginScene��EndScene֮�����RenderEx */
	STDMETHOD_(HRESULT,RenderEx)(THIS) PURE;	
	/* �ͷ�d3d�����Դ */
	STDMETHOD_(HRESULT,Finalize)(THIS) PURE;
	/* �����豸 */
	STDMETHOD_(void,OnDeviceReset)(THIS_ D3DPRESENT_PARAMETERS* pParams) PURE;
	/* �豸��ʧ */
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