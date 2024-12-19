#include "Stdafx.h"

#if defined(PRE_ADD_MAINQUEST_UI)
#include "DnMainQuestAnswerDlg.h"
#include "DnInterface.h"
#include "DnTooltipDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainQuestAnswerDlg::CDnMainQuestAnswerDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pBGTexCtrl(NULL)
, m_pTextBoxAnswer(NULL)
{
}

CDnMainQuestAnswerDlg::~CDnMainQuestAnswerDlg()
{
}

void CDnMainQuestAnswerDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NpcAnswerDlg.ui" ).c_str(), bShow );
}

void CDnMainQuestAnswerDlg::InitialUpdate()
{
	m_pBGTexCtrl	 = GetControl<CEtUITextureControl>("ID_TEXTUREL_IMAGE");
	m_pTextBoxAnswer = GetControl<CDnNpcAnswerHtmlTextBox>("ID_TEXTBOX_ANSWER");
}

bool CDnMainQuestAnswerDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP )
	{
		// Slot��ư�� HandleKeyboard�� ������ �ؼ�, Answer���ÿ� �����̸� �༭ ������ �ؽ�Ʈ�ڽ��� ȣ��
		return m_pTextBoxAnswer->HandleKeyboard(uMsg, wParam, lParam);
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMainQuestAnswerDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_TEXTBOX_SELECTION)
	{
		if(this->m_pParentDialog) {
			this->m_pParentDialog->GetCallBack()->OnUICallbackProc( this->GetParentDialog()->GetDialogID(), nCommand, pControl);
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMainQuestAnswerDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnMainQuestAnswerDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMainQuestAnswerDlg::SetAnswer(std::vector<TALK_ANSWER>& answers)
{
	ASSERT(m_pTextBoxAnswer);
	m_pTextBoxAnswer->ClearText();
	m_pTextBoxAnswer->Show(false);

	std::wstring wszAnswer;
	for( size_t i = 0 ; i < answers.size() ; i++ )
	{
		wszAnswer += answers[i].szAnswer;
		if( i < answers.size() - 1 )
			wszAnswer += L"<BR>";
	}

	if( wszAnswer.empty() )
	{
		//Show(false);
		m_pTextBoxAnswer->Show(false);
		m_pTextBoxAnswer->Focus(false);
		m_answers = answers;
		return;
	}

	RemoveStringW(wszAnswer, std::wstring(L"<html>"));
	RemoveStringW(wszAnswer, std::wstring(L"</html>"));
	RemoveStringW(wszAnswer, std::wstring(L"<HTML>"));
	RemoveStringW(wszAnswer, std::wstring(L"</HTML>"));

	wszAnswer.insert(0, L"<HTML>");
	wszAnswer += L"</HTML>";
	
	// ���������� �ٹٲ��̸� ����������..
	AllReplaceW(wszAnswer, std::wstring(L"<BR></HTML>"), std::wstring(L"</HTML>") );
	m_pTextBoxAnswer->ReadHtmlString(wszAnswer.c_str());
	m_answers = answers;
	
	Show(true);
	m_pTextBoxAnswer->Show(true);
	m_pTextBoxAnswer->Focus(true);

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	// �ٸ� â��� �޸� Show�Լ����� ����Ʈ�����ϴ°ɷδ� �ذ��� �ȵȴ�.
	// �������� �������� ������ ó���� �޶���ϱ⶧��.
	// ������������ ���⼭ ����ó�� �ϴ°ɷ� �ϰڴ�.
	//if( CGameOption::GetInstance().m_bSmartMoveMainMenu )
	//{
	//	if( m_answers.empty() )
	//	{
	//		SUICoord uiCoord;
	//		m_pCloseBtn->GetUICoord( uiCoord );
	//		m_fMovePosX = GetXCoord() + uiCoord.fX + uiCoord.fWidth / 2.0f;
	//		m_fMovePosY = GetYCoord() + uiCoord.fY + uiCoord.fHeight / 2.0f;
	//		m_bReadyMove = true;
	//	}
	//	else
	//	{
	//		std::vector<EtVector2> vecControlPos;
	//		m_pTextBoxAnswer->GetLinePos( vecControlPos );
	//		if( !vecControlPos.empty() )
	//		{
	//			m_fMovePosX = vecControlPos[0].x;
	//			m_fMovePosY = vecControlPos[0].y;

	//			// ������ �� �ڸ��� ����Ʈ�����Ű�� ����
	//			// ���信��Ű�� �ٽ� �������� ���콺�� �̵����� �� �� �ִ�.
	//			// ��, ó�� Answer�� �����ϰ� ���� ���� ��ġ�� ���� �������Ǵ� ��ġ�� �ణ �ٸ���,
	//			// �̰� ������ ���� ���� UpdateVisibleCount�Լ��� ȣ��ǰ� �̶� ������ ����Ǹ鼭 ������ �߻��ϱ� �����̴�.
	//			// �̰ͱ��� ����� ���߷��� ���θ� ��°� ����ϹǷ� �켱�� �׳� �̷��� ����� �ϰڴ�.
	//			//m_fMovePosY -= m_pTextBoxAnswer->GetProperty()->TextBoxProperty.fLineSpace;

	//			m_bReadyMove = true;
	//		}
	//	}
	//}
#endif
}

bool CDnMainQuestAnswerDlg::OnAnswer(int nNpcID)
{
	std::wstring curIndex, curTarget;
	GetAnswerIndex(curIndex, curTarget, -1, false);

	if(curIndex.empty() == false && curIndex.empty() == false)
	{
		std::transform(curIndex.begin(), curIndex.end(), curIndex.begin(), ::tolower);
		if(curIndex.compare(L"gift") == 0)
			return true;
	}

	return false;
}

bool CDnMainQuestAnswerDlg::GetAnswerIndex(OUT std::wstring& szIndex, OUT std::wstring& szTarget, int nCommand, bool bClearTextBoxSelect) const
{
	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(m_answers.empty())
			return false;

		// ��ư�� ������ ���� ��Ȳ�̸� Answer�� ������ �Ѱ�
		szIndex  = m_answers[0].szLinkIndex;
		szTarget = m_answers[0].szLinkTarget;
		return true;
	}
	else if( nCommand == EVENT_TEXTBOX_SELECTION)
	{
		ASSERT(m_pTextBoxAnswer);
		// ������ �Ѿ�� ��춧���� �̷��� �׳� �����ε����� ���ϸ� �ȵȴ�.
		//int nIdx =  m_pTextBoxAnswer->GetSelectedLineIndex();
		SLineData LineData;
		m_pTextBoxAnswer->GetSelectedLineData(LineData, bClearTextBoxSelect);
		int nIdx = LineData.m_nData;
		if( nIdx < 0 )
			return false;

		if( (int)m_answers.size() <= nIdx )
			return false;

		szIndex  = m_answers[nIdx].szLinkIndex;
		szTarget = m_answers[nIdx].szLinkTarget;

		return true;
	}
	return false;
}

void CDnMainQuestAnswerDlg::SetMainBGImage(const std::string& strFileName)
{
	if( !strFileName.empty() )
	{
		SAFE_RELEASE_SPTR( m_hMainBGImage );
		m_hMainBGImage = LoadResource( CEtResourceMng::GetInstance().GetFullName(strFileName.c_str() ).c_str(), RT_TEXTURE);
		if(m_hMainBGImage)
		{
			int W = m_hMainBGImage->Width();
			int H = m_hMainBGImage->Height();
			m_pBGTexCtrl->SetTexture(m_hMainBGImage, 0, 0, W, H);
			//m_pBGTexCtrl->SetTexture(m_hMainBGImage, 0, 0, 256, 128);
			m_pBGTexCtrl->SetManualControlColor( true );
			m_pBGTexCtrl->SetTextureColor( 0xAAFFFFFF );
			m_pBGTexCtrl->Show( true );
		}
		else
		{
			m_pBGTexCtrl->Show( false );
		}
	}
	else
	{
		SAFE_RELEASE_SPTR( m_hMainBGImage );
		m_pBGTexCtrl->Show( false );
	}
}

#endif // PRE_ADD_MAINQUEST_UI