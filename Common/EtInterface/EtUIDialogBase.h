#pragma once
#include "SmartPtr.h"
#include <list>

class CEtUIDialog;
class CEtUIControl;

// Note : 다이얼로그들을 관리하기 위한 클래스다.
//		다이얼로그가 생성될때 생성자에 들어가는 타입(UI_DIALOG_TYPE)에 따라 리스트로 분류된다.
//		랜더 순서와 메세지 처리 순서가 리스트에 따라 나누어 진다.
//
class CEtUIDialogBase : public CSmartPtrBase< CEtUIDialog >
{
public:
	CEtUIDialogBase( UI_DIALOG_TYPE dialogType );
	virtual ~CEtUIDialogBase();

protected:
	UI_DIALOG_TYPE m_emDialogType;

public:
	UI_DIALOG_TYPE GetDialogType() { return m_emDialogType; }

protected:
	void RemoveDialog( std::list< CEtUIDialog* > &listDialog );

protected:
	static void RenderList( std::list< CEtUIDialog* > &listDialog, float fElapsedTime );
	static void ProcessList( std::list< CEtUIDialog* > &listDialog, float fElapsedTime );
	static bool MsgProcList( std::list< CEtUIDialog* > &listDialog, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void ShowList( std::list< CEtUIDialog* > &listDialog, bool bShow );
	static bool FindControlList( std::vector< CEtUIControl* > &vecControl, std::list< CEtUIDialog* > &listDialog, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord );

	void SetRenderPriority(CEtUIDialog* pDlg, bool bTail);

protected:
	void ShowModal( CEtUIDialog *pDialog, bool bShow );

public:
	static void RenderAll( float fElapsedTime );
	static void ProcessAll( float fElapsedTime );
	static bool MsgProcModal( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static bool StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	static void ShowDialogList( UI_DIALOG_TYPE dialogType, bool bShow );
	static void CloseAllDialog();

	static void CloseModalDialog();

	// bCheckCoveredControl이 true인 경우엔 해당 컨트롤이 다른 UI에 가려져있는지를 체크한다.
	static bool StaticFindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl = false );

public:
	static std::list< CEtUIDialog* > s_plistDialogBottomMsg;
	static std::list< CEtUIDialog* > s_plistDialogBottom;
	static std::list< CEtUIDialog* > s_plistDialogTopMsg;
	static std::list< CEtUIDialog* > s_plistDialogTop;
	static std::list< CEtUIDialog* > s_plistDialogFocus;
	static std::list< CEtUIDialog* > s_plistDialogModal;
	static std::list< CEtUIDialog* > s_plistDialogMostTop;
	static std::list< CEtUIDialog* > s_plistDialogSelf;

	static bool s_bShowModal;

#if defined(PRE_FIX_57852)
protected:
	struct _AddDialogInfo
	{
		bool m_bTail;
		CEtUIDialog* m_pDialog;
		_AddDialogInfo()
		{
			m_pDialog = NULL;
			m_bTail = false;
		}
		_AddDialogInfo(CEtUIDialog* pDialog, bool bTail)
		{
			m_pDialog = pDialog;
			m_bTail = bTail;
		}
	};
	typedef std::list<_AddDialogInfo> DIALOG_LIST;
	typedef std::map<int, DIALOG_LIST> MAP_DIALOG_LIST;
	static MAP_DIALOG_LIST ms_AddDialogList;

	static void AddDialogList(int nType, CEtUIDialog* pDialog, bool bTail);
	static void ProcessAddDialogList();
#endif // PRE_FIX_57852
};