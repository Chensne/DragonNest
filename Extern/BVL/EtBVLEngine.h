#pragma once

#include "BandiVideoLibrary.h"
#include "BandiVideoTexture_DX9.h"
#include "EtResourceMng.h"
#include "EternityEngine.h"
#include "Timer.h"

class CEtDevice;
class CEtBandiVideoStream;

class CEtBVLEngine : public CLostDeviceProcess, public CSingleton<CEtBVLEngine> {
public:
	CEtBVLEngine();
	virtual ~CEtBVLEngine();

	enum BVL_OPEN_OPT
	{
		BVL_OPEN_OPT_NONE,				// ���� ���⸸ �ϰ� ����� ���� ����
		BVL_OPEN_OPT_AUTOPLAY,			// ���� ���Ⱑ �Ϸ�Ǹ� �ڵ� ���
		BVL_OPEN_OPT_AUTOPLAY_REPEAT,	// ���� ���Ⱑ �Ϸ�Ǹ� �ڵ� �ݺ� ���
	};

protected:
	CBandiVideoLibrary *m_pBVL;
	CBandiVideoTexture *m_pBVT;
	BVL_VIDEO_INFO m_VideoInfo;
	CEtBandiVideoStream *m_pBVS;
	CEtDevice *m_pEtDevice;
	EtCameraHandle m_hCamera;
	bool m_bRepeat;
	bool m_bOnLostDevice;

public:
	bool Initialize( HWND hWnd, CEtDevice *pDev, EtCameraHandle hCamera );
	bool Initialize( HWND hWnd, CEtDevice *pDev );
	void Finalize();

	void Process( LOCAL_TIME LocalTime, float fDelta, int nValue = -1 );
	bool GetVideoTexture( EtTexture **ppTexture );

	bool Open( const char *szFileName, BVL_OPEN_OPT option = BVL_OPEN_OPT_NONE );
	void Close();

	HRESULT Play( bool bRepeat = false );
	HRESULT GetVideoInfo(BVL_VIDEO_INFO& info);
	void Stop();
	void Pause();
	void Resume();
	void SetVolume( float fValue );

	bool IsPlaying();
	bool IsOnLostDevice() const { return m_bOnLostDevice; }

	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

class CEtBandiVideoStream : public CResMngStream, public IBandiVideoFileReader
{
public:
	CEtBandiVideoStream();

	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** p) { return E_NOTIMPL; }
	ULONG   STDMETHODCALLTYPE AddRef(void)	{return 0;}
	ULONG   STDMETHODCALLTYPE Release(void)	{return 0;}

	virtual HRESULT	Open(LPCSTR path_name);
	virtual void	Close();
	virtual	INT		Read(BYTE* pBuf, INT nBytesToRead);
	virtual INT64	SetPosition(INT64 nPos);
	virtual INT64	GetPosition();
	virtual INT64	GetFileSize();
};