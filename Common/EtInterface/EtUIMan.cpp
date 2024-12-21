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
		// ������ �� ������, �������忡�� �ѹ��̶� IME�� �����ϰ� ����, �ƹ��� �ٽ� dll �ε带 �ص� IMEȰ��ȭ�� �ȵǼ�
		// �ƿ� RefCount�� 0���� �ȸ������ �̷��� ���� ó���� �ѹ� �������ѵд�.
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

		// Note : ��޷� ��ϵ� ��������� �켱ó���մϴ�.
		//
		if( CEtUIDialogBase::MsgProcModal( hWnd, uMsg, wParam, lParam ) )
			return true;

		if( CEtUIDialogBase::StaticMsgProc( hWnd, uMsg, wParam, lParam ) )
			return true;

		// Note : UI���� ó�� �ȵǴ� ���콺 Ŭ���϶� �巡�׸� �������Ѵ�.
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
				// Note : �������� �����뿡 �����ϴ�.
				//		������ �������� ������ ó���� �κ��丮 ���̾�α׿��� �մϴ�.
				//
				trash::SetControl( drag::GetControl() );
			}
		}

		return false;
	}

	void DeleteAllDialog()
	{
		// Note : ���⼭ ������ ���̾�α׸� �����ϰ� ������ ������ٴ� ����.
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
			// ���� �����Ǿ��ִ� s_pFocusControl�� ���� ���̾�αװ� �ƴ� �ٸ� ���̾�αװ� ����,
			if( !IsSameParent(pDialog) )
			{
				// �񱳹��� != �� ���־��µ�, �̷��� �ǹ̻� �̻��ϴ�.
				// ����� �ȵ��ִ��� ã�Ƽ� ����ϴ°ǵ� == �� �´°� �ƴѰ�.
				// �׷���, ���� focus�� ���� �м��� �� �ǰ�, ���� �̻��� ���� ���Ƽ�
				// �׳� �״�� �α�� �ϰڴ�.
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
			// �������� �ǵ��� �����غ���,
			// ���������� ���ȴ� â�� Push �� �ٸ� â�� ���� ���� �ٷ� Show( false ) �ɶ� �� pop�� ����ǵ��� �� �� ������,
			//
			// �޼����ڽ��� ��Ÿ ��κ��� â�� �㶧�� Ư�� ��Ʈ�ѿ� ��Ŀ���� ��û���� �ʴ´�.( RequestFocus )
			// �̶� s_pFocusControl ���� NULL�� �����ε�,
			// �Ʒ� �˻翡�� ����� �ȵȴ�.
			// s_pFocusControl�� NULL�� ����.. ���� �̻��� �ڵ��.
			if( IsSameParent(pDialog) )
			{
				ReleaseControl();

				if( !s_vecPrevFocusControl.empty() )
				{
					CEtUIControl *pFocusControl;
					pFocusControl = s_vecPrevFocusControl[ s_vecPrevFocusControl.size() - 1 ];
					//pFocusControl->GetParent()->RequestFocus( pFocusControl );
					// Push�� ������ �ѹ��� ��� �����ߴ� �� ���⵵ �ϴ�.
					if ( pFocusControl ) pFocusControl->GetParent()->RequestFocus( pFocusControl );
					s_vecPrevFocusControl.pop_back();
				}
			}
			else
			{
				// ��� ���� â�� Show(false)�Ǵ°� �ƴ϶�,
				// �߰������뿡 ���� â�� Show(false)�Ǵ°Ŷ��, focus����Ʈ�� ������ �� ����.
				// (������ ���̾�αװ� �������� ��û���� �ʾ����� �Ѱ��� ���� �ɰ� ����.)
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
			// ��Ŀ�����¿��� ������ �ɶ� �߰����� ó���� �ʿ��ϴٸ� ���⼭ �Ѵ�.
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