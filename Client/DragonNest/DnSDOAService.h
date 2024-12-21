#pragma once

#if defined(_CH) && defined(_AUTH)

#include "Singleton.h"
#include "SDOA4Client.h"
#include "SDOADx9.h"
#ifdef PRE_ADD_SHANDA_GPLUS
#include "GPLUSDx9.h"
#include "GPLUSClient.h"
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

class CDnSDOAService : public CSingleton<CDnSDOAService>, public CLostDeviceProcess
{
public:
	CDnSDOAService();
	virtual ~CDnSDOAService();

protected:
#ifdef PRE_ADD_SHANDA_GPLUS
	HINSTANCE m_hGPlus;
	LPGPlusInitialize m_fpGPlusInitialize;
	LPGPlusGetModule m_fpGPlusGetModule;
	LPGPlusTerminal m_fpGPlusTerminal;

	IGPLUSApp* m_pGPlusApp;
	IGPLUSDx9* m_pGPlusRender;

	string m_strShandaID;
	RoleInfo m_RoleInfo;
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

	HINSTANCE m_hIGW;
	LPigwInitialize m_pIGWInitialize;
	LPigwGetModule m_pIGWGetModule;
	LPigwTerminal m_pIGWTerminal;

	ISDOADx9 *m_pSDOARender;
	ISDOAApp *m_pSDOAApp;

	bool m_bSDOLogin;
	bool m_bShow;
	bool m_bShowLoginDialog;
	bool m_bShowChargeDlg;

	void ( __stdcall *m_pPrevPrePresentCallback )();

protected:
	int GetPartitionID();
#ifdef PRE_ADD_SHANDA_GPLUS
	bool InitializeGPlus();
	void FinalizeGPlus();
	void GPlusLogin_deprecated( const LoginResult* pLoginResult );
	void SaveRoleInfo( const RoleInfo& Info ) { m_RoleInfo = Info; };
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
	bool InitializeIGW();
	void  FinalizeIGW();

public:
	bool Initialize( bool bShow );
	void Finalize();

	void OnCreateDevice();
	void OnDestroyDevice();

	void Render();
	static void __stdcall PrePresentCallback();

	void Show( bool bShow );
	bool IsShow() { return m_bShow; }

	bool IsFocus();

	void ShowLoginDialog( bool bShow );
	bool IsShowLoginDialog() { return m_bShowLoginDialog; }
	void ForceSetShowLoginDialogState( bool bShow ) { m_bShowLoginDialog = bShow; }
	void Logout();

	// Ŭ���̾�Ʈ ���� ��ǥ �ʿ�.
	void SetTaskBarPosition( int nX, int nY );
	bool SetLoginDialogPosition( int nX, int nY );
	bool GetLoginDialogRect( int &nX, int &nY, int &nWidth, int &nHeight );

	LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pbNoFurtherProcessing );
	static BOOL CALLBACK OnLogin( int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved );

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	ISDOAApp *GetSDOAApp() { return m_pSDOAApp; }
	void SetLoginState( bool bLogin ) { m_bSDOLogin = bLogin; }
	bool GetLoginState() { return m_bSDOLogin; }

	bool ShowChargeDlg(bool bShow);
	bool IsShowChargeDlg() const;

#ifdef PRE_ADD_SHANDA_GPLUS
	void GPlusLogin( LPCWSTR pwcsRoleName );
	void SetLoginResultShandaID( LPCSTR pShandaID );
	IGPLUSApp* GetGPlusApp() { return m_pGPlusApp; };
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
};

#endif	// _CH