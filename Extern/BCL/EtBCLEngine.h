#pragma once

#include "Singleton.h"
#include "bandicap.h"

class CEtDevice;
class CEtBCLEngine : public CSingleton< CEtBCLEngine >
{
public:
	CEtBCLEngine();
	virtual ~CEtBCLEngine();

	enum CaptureOptionEnum {
		Default,
		HalfSize,
		Res_320x240,
		Res_400x300,
		Res_512x384,
		Res_576x432,
		Res_640x480,
		Res_800x600,

		Mpeg1,
		MJpeg,
		MJpeg_HighQ,
		Mpeg4,

		Youtube,
		Youtube_HighQ,
		Youtube_HighD,
		Naver,
		Daum,
		MnCast,
	};

	enum CaptureErrorEnum {
		ErrorUnknown,
		ErrorVideo,
		ErrorAudio,
		ErrorCreateFile,
		ErrorWriteFile,
		ErrorNotEnoughDiskSpace,
	};
public:
	static void (__stdcall *s_pCaptureInfoCallback)();

protected:
	CBandiCaptureLibrary *m_pBCL;
	CEtDevice *m_pEtDevice;
	BCAP_CONFIG m_Config;
	HWND m_hWnd;
	std::wstring m_szDefaultPath;
	std::wstring m_szSaveFileName;
	void ( __stdcall *m_pPrevPrePresentCallback )();


public:
	bool Initialize( HWND hWnd, CEtDevice *pDev, WCHAR *wszDefaultPath = NULL );
	void Finalize();

	void SetDefaultPath( WCHAR *wszPath ) { m_szDefaultPath = wszPath; }
	WCHAR *GetDefaultPath() { return (WCHAR *)m_szDefaultPath.c_str(); }

	WCHAR *GetSaveFileName() { return (WCHAR *)m_szSaveFileName.c_str(); }

	void Start( TCHAR *szFileName = NULL );
	void Stop();
	bool IsCapturing();

	void SetOption( CaptureOptionEnum Option );
	BCAP_CONFIG *GetConfig() { return &m_Config; }
	void RefreshConfig();

	int GetCaptureTime();
	INT64 GetCaptureFileSize();

	void SetPriority( int nPriority );
	int GetPriority();

	void SetMinMaxFPS( int nMin, int nMax );
	void GetMinMaxFPS( int &nMin, int &nMax );

	void Work();
	static void __stdcall PrePresentCallback();

	void SetCaptureInfoCallback( void (__stdcall *pCallback)() ) { s_pCaptureInfoCallback = pCallback; }

};

#define UM_CAPTURE_ERROR WM_USER + 2009