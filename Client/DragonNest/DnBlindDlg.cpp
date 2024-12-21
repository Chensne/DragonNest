#include "StdAfx.h"
#include "DnBlindDlg.h"
#include "DnBlindCaptionDlg.h"
#include "TaskManager.h"
#include "DnCommonTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnBlindCallBack::~CDnBlindCallBack() 
{ 
	if( CDnInterface::IsActive() && GetInterface().GetBlindDialog() ) 
		GetInterface().GetBlindDialog()->RemoveBlindCallback( this ); 
}

CDnBlindDlg::CDnBlindDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_BlindMode(modeOpened)
	, m_BillBoardColor(0x00000000)
	, m_pBlindCaptionDlg(NULL)
	, m_fCaptionDelta(FLT_MAX)
{
}

CDnBlindDlg::~CDnBlindDlg(void)
{
	SAFE_DELETE(m_pBlindCaptionDlg);
}

void CDnBlindDlg::InitialUpdate()
{
	m_pBlindCaptionDlg = new CDnBlindCaptionDlg( UI_TYPE_CHILD, this );
	m_pBlindCaptionDlg->Initialize( false );
}

void CDnBlindDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( GetBlindMode() == modeClosed )	return;
	//if( GetBlindMode() == modeOpened )	return;

	EtVector2 CurVec;
	EtColor CurColor;

	EtVec2Lerp( &CurVec, &Vector1, &Vector2, 1.0f - powf( 0.8f, 30 * fElapsedTime ) );
	EtColorLerp( &CurColor, &Color1, &Color2, 1.0f - powf( 0.8f, 30 * fElapsedTime ) );

	Vector1 = CurVec;
	Color1 = CurColor;

	if( GetBlindMode() == modeOpen )
	{
		if( (Vector2.y-CurVec.y) > 1.0f)
		{
			UpdateBlind( (int)CurVec.y, Color1 );
		}
		else
		{
			UpdateBlind( (int)Vector2.y, Color1 );
			SetBlindMode(modeOpened);

			if ( m_BlindCallBackList.size() )
			{
				for ( size_t i = 0 ; i < m_BlindCallBackList.size() ; i++ )
				{
					if( m_BlindCallBackList[i] )
						m_BlindCallBackList[i]->OnBlindOpened();
				}
			}
		}
	}
	else if( GetBlindMode() == modeClose )
	{
		if( CurVec.y > 1.0f )
		{
			UpdateBlind( (int)CurVec.y, Color1 );
		}
		else
		{
			UpdateBlind( (int)Vector2.y, Color1 );
			SetBlindMode(modeClosed);

			if ( m_BlindCallBackList.size() )
			{
				for ( size_t i = 0 ; i < m_BlindCallBackList.size() ; i++ )
				{
					if( m_BlindCallBackList[i] )
						m_BlindCallBackList[i]->OnBlindClosed();
				}

				m_BlindCallBackList.clear();
			}
		}
	}
	if( m_pBlindCaptionDlg ) {
		bool bShowCaption = false;
		if( m_fCaptionDelta == FLT_MAX ) bShowCaption = true;
		else {
			if( m_fCaptionDelta > 0.f ) {
				m_fCaptionDelta -= fElapsedTime;
				if( m_fCaptionDelta <= 0.f ) m_fCaptionDelta = 0.f;
				else bShowCaption = true;
			}
		}
		ShowChildDialog( m_pBlindCaptionDlg, bShowCaption );
	}
}

void CDnBlindDlg::Render( float fElapsedTime )
{
	if( GetBlindMode() == modeClosed )
		return;

	DrawRect( m_BillBoard, m_BillBoardColor );
	DrawRect( m_UpBlind, m_BlindColor );
	DrawRect( m_DownBlind, m_BlindColor );

	CEtUIDialog::Render( fElapsedTime );
}

void CDnBlindDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		if( (GetBlindMode() == modeOpened) || (GetBlindMode() == modeOpen) )
			return;

		m_BillBoard.fX = -GetScreenWidthBorderSize();
		m_BillBoard.fY = -GetScreenHeightBorderSize();
		m_BillBoard.fWidth = GetScreenWidthRatio();
		m_BillBoard.fHeight = GetScreenHeightRatio();
	}
	else
	{
		if( (GetBlindMode() == modeClosed) || (GetBlindMode() == modeClose) )
			return;
	}

	m_bShow = bShow;

	if( bShow )
	{
		if( !m_bEndInitialize )
		{
			SetBlindMode(modeOpened);
			m_BlindColor = EtColor(0xff000000);
			m_UpBlind.fHeight = 0.0f;
			m_DownBlind.fHeight = (float)BLIND_HEIGHT * GetScreenHeightRatio();
		}
		else
		{
			SetBlindMode(modeOpen);
			CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
			if( pTask )			
				pTask->SendWindowState(WINDOW_BLIND);
			if ( m_BlindCallBackList.size() )
			{
				for ( size_t i = 0 ; i < m_BlindCallBackList.size() ; i++ )
				{
					if( m_BlindCallBackList[i] )
						m_BlindCallBackList[i]->OnBlindOpen();
				}
			}
		

			Vector1.x = 0.0f;
			Vector1.y = 0.0f;
			Vector2.x = 0.0f;
			Vector2.y = (float)BLIND_HEIGHT * GetScreenHeightRatio();

			//Color1 = EtColor(0x55000000);
			Color1 = EtColor(0xff000000);
			Color2 = EtColor(0xff000000);

			m_UpBlind.fHeight = 0.0f;
			m_DownBlind.fHeight = 0.0f;
		}
	}
	else
	{
		if( !m_bEndInitialize )
		{
			SetBlindMode(modeClosed);
			m_BlindColor = EtColor(0x55000000);
			m_UpBlind.fHeight = 0.0f;
			m_DownBlind.fHeight = 0.0f;
		}
		else
		{
			SetBlindMode(modeClose);
			CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
			if( pTask )			
				pTask->SendWindowState(WINDOW_NONE);		
			if ( m_BlindCallBackList.size() )
			{
				for ( size_t i = 0 ; i < m_BlindCallBackList.size() ; i++ )
				{
					if( m_BlindCallBackList[i] )
						m_BlindCallBackList[i]->OnBlindClose();
				}
			}

			Vector1.x = 0.0f;
			Vector1.y = (float)BLIND_HEIGHT * GetScreenHeightRatio();
			Vector2.x = 0.0f;
			Vector2.y = 0.0f;

			Color1 = EtColor(0xff000000);
			//Color2 = EtColor(0x55000000);
			Color2 = EtColor(0xff000000);

			//m_UpBlind.fHeight = Vector1.y;
			//m_DownBlind.fHeight = Vector1.y;

			ShowChildDialog( m_pBlindCaptionDlg, false );
		}
	}
}

void CDnBlindDlg::UpdateBlind( int nBlindHeight, EtColor &blindColor )
{
	m_UpBlind.fX = -GetScreenWidthBorderSize();
	m_UpBlind.fY = -GetScreenHeightBorderSize();
	m_UpBlind.fWidth = GetScreenWidthRatio();
	m_UpBlind.fHeight = nBlindHeight / (float)DEFAULT_UI_SCREEN_HEIGHT * GetScreenHeightRatio();

	m_DownBlind.fWidth = GetScreenWidthRatio();
	m_DownBlind.fHeight = nBlindHeight / (float)DEFAULT_UI_SCREEN_HEIGHT  * GetScreenHeightRatio();
	m_DownBlind.fX = -GetScreenWidthBorderSize();
	m_DownBlind.fY = GetScreenHeightRatio() - GetScreenHeightBorderSize() - m_DownBlind.fHeight;

	m_BlindColor = blindColor;
}

void CDnBlindDlg::SetCaption( const wchar_t *wszCaption, int nDelay )
{
	if( !m_pBlindCaptionDlg ) return;

	SUICoord capCoord;
	m_pBlindCaptionDlg->GetDlgCoord( capCoord );
	capCoord.fY = m_DownBlind.fY + ((m_DownBlind.fHeight - capCoord.fHeight)/2.0f);
	m_pBlindCaptionDlg->SetDlgCoord( capCoord );

	m_pBlindCaptionDlg->SetCaption( wszCaption );
	if( nDelay <= 0 )
		m_fCaptionDelta = FLT_MAX;
	else m_fCaptionDelta = nDelay / 1000.f;
}

void CDnBlindDlg::ClearCaption()
{
	if( !m_pBlindCaptionDlg ) return;
	m_pBlindCaptionDlg->ClearCaption();
}

void CDnBlindDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	UpdateBlind( (int)Vector1.y, m_BlindColor );
}


// #54501 퀘스트 진행 중 컷신 출력된 후, 파티원의 모습이 보이지 않음.
void CDnBlindDlg::CallBackClosed()
{
	if( !m_BlindCallBackList.empty() )
	{
		for ( size_t i = 0 ; i < m_BlindCallBackList.size() ; i++ )
		{
			if( m_BlindCallBackList[i] )
				m_BlindCallBackList[i]->OnBlindClosed();
		}
	}

}