#include "StdAfx.h"
#include "EtUIMan.h"
#include "EtUIControl.h"
#include "EtUIDialog.h"
#include "EtUIXML.h"
#include "EtPrimitiveDraw.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

extern bool g_bSkipScene;

namespace EtInterface
{
	bool g_bEtUIInitTool = false;
	bool g_bEtUIRender = true;
	bool g_bEtUIMsgProc = true;
	bool g_bEtUIProcess = true;

	CEtFontMng *g_pFontMng = NULL;
	CEtUIDialog *g_pFocusDialog = NULL;
	CEtUINameLinkMgr* g_pNameLinkMgr = NULL;

	void Initialize()
	{
		// 원인은 잘 모르지만, 릴리즈모드에선 한번이라도 IME를 해제하고 나면, 아무리 다시 dll 로드를 해도 IME활성화가 안되서
		// 아예 RefCount를 0으로 안만드려고 이렇게 제일 처음에 한번 증가시켜둔다.
		CEtUIIME::Initialize();
		g_pFontMng = new CEtFontMng();
		g_pNameLinkMgr = new CEtUINameLinkMgr;
	}

	void Finalize()
	{
		DeleteAllDialog();
		SAFE_DELETE( g_pFontMng );
		SAFE_DELETE(g_pNameLinkMgr);
		CEtUIIME::Uninitialize();
	}

	void Process( float fElapsedTime )
	{
		if( !g_bEtUIProcess ) return;
		CEtUIDialogBase::ProcessAll( fElapsedTime );
	}

	void Render( float fElapsedTime )
	{
		if( g_bSkipScene ) return;

		bool bRender = false;
		if( g_bEtUIRender ) bRender = true;
		if( !g_bEtUIRender && CEtUIDialog::s_pFadeDlg && CEtUIDialog::s_pFadeDlg->IsShow() ) bRender = true;
		if( !bRender ) return;

		CEtDevice *pEtDevice = GetEtDevice();
		if( !pEtDevice ) return;	

		IDirect3DDevice9 *pd3dDevice = (IDirect3DDevice9*)pEtDevice->GetDevicePtr();
		if( !pd3dDevice ) return;

		pEtDevice->EnableAlphaBlend( true );
		CEtSprite::GetInstance().Begin( D3DXSPRITE_DONOTSAVESTATE );
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		if( g_bEtUIRender )
		{
			CEtUIDialogBase::RenderAll( fElapsedTime );
			drag::Render( fElapsedTime );
		}
		else
		{
			if( CEtUIDialog::s_pFadeDlg )
				CEtUIDialog::s_pFadeDlg->Render( fElapsedTime );
		}

		GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		CEtSprite::GetInstance().End();
		pEtDevice->EnableAlphaBlend( false );
	}

	bool StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if( !g_bEtUIMsgProc )
			return false;

		if( !g_bEtUIRender )
			return false;

		g_pFocusDialog = NULL;

		if( focus::IsValid() )
		{
			if( ( focus::GetType() == UI_CONTROL_IMEEDITBOX ) || ( focus::GetType() == UI_CONTROL_LINE_IMEEDITBOX ) )
			{
				if( CEtUIIME::StaticMsgProc( uMsg, wParam, lParam ) )
					return true;
			}
		}

		// Note : 모달로 등록된 윈도우들을 우선처리합니다.
		//
		if( CEtUIDialogBase::MsgProcModal( hWnd, uMsg, wParam, lParam ) )
			return true;

		if( CEtUIDialogBase::StaticMsgProc( hWnd, uMsg, wParam, lParam ) )
			return true;

		// Note : UI에서 처리 안되는 마우스 클릭일때 드래그를 릴리즈한다.
		//
		if( uMsg == WM_RBUTTONDOWN )
		{
			drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}
		else if( uMsg == WM_LBUTTONDOWN )
		{
			if( drag::IsValid() )
			{
				// Note : 아이템을 휴지통에 버립니다.
				//		실제로 아이템을 버리는 처리는 인벤토리 다이얼로그에서 합니다.
				//
				trash::SetControl( drag::GetControl() );
			}
		}

		return false;
	}

	void DeleteAllDialog()
	{
		// Note : 여기서 뻑나면 다이얼로그를 생성하고 삭제를 안해줬다는 것임.
		//
		CSmartPtrBase< CEtUIDialog >::DeleteAllObject();
	}

	const CEtUINameLinkMgr& GetNameLinkMgr()
	{
		_ASSERT(g_pNameLinkMgr);
		return *g_pNameLinkMgr;
	}

	namespace xml
	{
		CEtUIXML *s_pEtUIXML(NULL);

		CEtUIXML *GetXML() {return s_pEtUIXML; }
		void SetXML( CEtUIXML *pXML )
		{
			ASSERT(pXML&&"EtInterface::xml::SetEtUIXML");
			s_pEtUIXML = pXML;
		}
		bool IsValid() {return s_pEtUIXML!=NULL; }
		const wchar_t* GetString( int nCategoryID, int nIndex )
		{
			return s_pEtUIXML->GetUIString( (CEtUIXML::emCategoryId)nCategoryID, nIndex );
		}
	}

	namespace drag
	{
		CEtUIControl*	s_pDragControl(NULL);
		bool			bRender = true;

		void SetControl( CEtUIControl *pControl )		
		{ 
			ASSERT(pControl&&"EtInterface::drag::SetControl");
			s_pDragControl = pControl; 
			bRender = true;
			focus::ReleaseControl();
		}

		CEtUIControl *GetControl()						{ return s_pDragControl; }
		void Command(UIDragCmdParam param)				{ if (s_pDragControl) s_pDragControl->OnCmdDrag(param); }
		void ReleaseControl()							{ s_pDragControl = NULL; }
		bool IsValid()									{ return s_pDragControl!=NULL; }
		void SetRender(bool enable)						{ bRender = enable; }

		void Render( float fElapsedTime )
		{
			if( !s_pDragControl || bRender == false ) return;
			s_pDragControl->RenderDrag( fElapsedTime );
		}
	}

	namespace trash
	{
		CEtUIControl *s_pTrashControl(NULL);

		void SetControl( CEtUIControl *pControl )		
		{ 
			ASSERT(pControl&&"EtInterface::trash::SetControl");
			s_pTrashControl = pControl;
			focus::ReleaseControl();
		}

		CEtUIControl *GetControl()						{ return s_pTrashControl; }
		void ReleaseControl()							{ s_pTrashControl = NULL; }
		bool IsValid()									{ return s_pTrashControl!=NULL; }
	}

	namespace focus
	{
		CEtUIControl *s_pFocusControl(NULL);
		std::vector< CEtUIControl * > s_vecPrevFocusControl;

		std::vector<CEtUIControl*>& GetPrevControl()	{ return s_vecPrevFocusControl; }

		void PushControl(CEtUIDialog *pDialog)
		{
			// 현재 설정되어있는 s_pFocusControl을 가진 다이얼로그가 아닌 다른 다이얼로그가 오면,
			if( !IsSameParent(pDialog) )
			{
				// 비교문이 != 로 되있었는데, 이러면 의미상 이상하다.
				// 등록이 안되있는지 찾아서 등록하는건데 == 가 맞는거 아닌가.
				// 그러나, 아직 focus에 대한 분석이 덜 되고, 뭔가 이상한 점이 많아서
				// 그냥 그대로 두기로 하겠다.
				if( std::find( s_vecPrevFocusControl.begin(), s_vecPrevFocusControl.end(), s_pFocusControl ) != s_vecPrevFocusControl.end() )
				{
					s_vecPrevFocusControl.push_back( s_pFocusControl );
					UnsetFocus();
					ReleaseControl();
				}
			}
		}

		void PopControl(CEtUIDialog *pDialog)
		{
			// 제작자의 의도를 유추해보면,
			// 최종적으로 열렸던 창이 Push 후 다른 창의 열림 없이 바로 Show( false ) 될때 이 pop이 수행되도록 한 거 같은데,
			//
			// 메세지박스나 기타 대부분의 창이 뜰때는 특정 컨트롤에 포커스를 요청하지 않는다.( RequestFocus )
			// 이땐 s_pFocusControl 값이 NULL인 상태인데,
			// 아래 검사에거 통과가 안된다.
			// s_pFocusControl이 NULL이 때문.. 뭔가 이상한 코드다.
			if( IsSameParent(pDialog) )
			{
				ReleaseControl();

				if( !s_vecPrevFocusControl.empty() )
				{
					CEtUIControl *pFocusControl;
					pFocusControl = s_vecPrevFocusControl[ s_vecPrevFocusControl.size() - 1 ];
					//pFocusControl->GetParent()->RequestFocus( pFocusControl );
					// Push가 된적이 한번도 없어서 안전했던 거 같기도 하다.
					if ( pFocusControl ) pFocusControl->GetParent()->RequestFocus( pFocusControl );
					s_vecPrevFocusControl.pop_back();
				}
			}
			else
			{
				// 방금 열린 창이 Show(false)되는게 아니라,
				// 중간순서쯤에 열린 창이 Show(false)되는거라면, focus리스트를 뒤져서 싹 뺀다.
				// (무조건 다이얼로그가 무작위로 요청하지 않았으면 한개만 빼도 될거 같다.)
				for( int i = 0; i < ( int )s_vecPrevFocusControl.size(); i++ )
				{
					if( s_vecPrevFocusControl[ i ] && s_vecPrevFocusControl[ i ]->GetParent() == pDialog )
					{
						s_vecPrevFocusControl.erase( s_vecPrevFocusControl.begin() + i );
						break;
					}
				}
			}
		}

		CEtUIControl* GetControl()						
		{ 
			return s_pFocusControl; 
		}

		void SetControl(CEtUIControl* pControl)			
		{ 
			ASSERT(pControl&&"EtInterface::focus::SetControl");
			if( s_pFocusControl ) ReleaseControl();
			s_pFocusControl=pControl; 
		}

		void ReleaseControl()
		{
			if( s_pFocusControl )
			{
				s_pFocusControl->Focus(false);
				OnReleaseControl();
			}

			s_pFocusControl=NULL;
		}

		void OnReleaseControl()
		{ 
			// 포커스상태에서 릴리즈 될때 추가적인 처리가 필요하다면 여기서 한다.
			if( s_pFocusControl )
			{
				if( s_pFocusControl->GetParent() && s_pFocusControl->GetParent()->IsElementDialog() == false )
				{
					if( s_pFocusControl->IsPressed() || s_pFocusControl->IsRightPressed() || s_pFocusControl->IsHaveScrollBarControl() )
					{
						s_pFocusControl->SetPressed( false );
						s_pFocusControl->SetRightPressed( false );
						ReleaseCapture();
					}
				}
			}
		}

		bool IsValid()
		{ 
			return s_pFocusControl!=NULL; 
		}

		bool IsEnable()									
		{
			if( s_pFocusControl )
				return s_pFocusControl->IsEnable();

			return false;
		}

		bool IsSameParent( CEtUIDialog *pDlg)
		{ 
			ASSERT(pDlg&&"EtInterface::focus::IsSameParent");

			if( s_pFocusControl == NULL )
				return false;

			return (s_pFocusControl->GetParent() == pDlg);
		}

		bool IsSameControl( CEtUIControl* pControl )
		{
			ASSERT(pControl&&"EtInterface::focus::IsSameControl, pControl is NULL!");
			return s_pFocusControl==pControl;
		}

		void SetFocus()									
		{
			if( s_pFocusControl )
			{
				s_pFocusControl->Focus(true);
			}
		}

		void SetFocus(CEtUIControl* pControl)
		{
			ASSERT(pControl&&"EtInterface::focus::SetFocus");
			if (IsSameControl(pControl) == false)
			{
				ReleaseControl();
				s_pFocusControl = pControl;
				s_pFocusControl->Focus(true);
			}
		}

		void UnsetFocus()								
		{ 
			if( s_pFocusControl )
			{
				s_pFocusControl->Focus(false);
			}
		}
		
		CEtUIDialog* GetParent()						
		{
			if( s_pFocusControl )
			{
				return s_pFocusControl->GetParent();
			}

			return NULL;
		}

		UI_CONTROL_TYPE GetType()						
		{ 
			if( s_pFocusControl )
			{
				return s_pFocusControl->GetType();
			}

			return UI_CONTROL_SIZE;
		}

		bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{ 
			if( s_pFocusControl )
			{
				return s_pFocusControl->MsgProc(uMsg, wParam, lParam);
			}

			return false;
		}

		bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			if( s_pFocusControl )
				return s_pFocusControl->HandleKeyboard(uMsg, wParam, lParam);

			return false;
		}

		bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
		{
			if( s_pFocusControl )
				return s_pFocusControl->HandleMouse(uMsg, fX, fY, wParam, lParam);

			return false;
		}
	}
}