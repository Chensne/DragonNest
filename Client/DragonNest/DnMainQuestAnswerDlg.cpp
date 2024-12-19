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
		// Slot버튼의 HandleKeyboard가 간섭을 해서, Answer선택에 딜레이를 줘서 강제로 텍스트박스만 호출
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
	
	// 마지막줄이 줄바꿈이면 무시해주자..
	AllReplaceW(wszAnswer, std::wstring(L"<BR></HTML>"), std::wstring(L"</HTML>") );
	m_pTextBoxAnswer->ReadHtmlString(wszAnswer.c_str());
	m_answers = answers;
	
	Show(true);
	m_pTextBoxAnswer->Show(true);
	m_pTextBoxAnswer->Focus(true);

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	// 다른 창들과 달리 Show함수에서 스마트무브하는걸로는 해결이 안된다.
	// 선택지가 있을때와 없을때 처리가 달라야하기때문.
	// 지저분하지만 여기서 예외처리 하는걸로 하겠다.
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

	//			// 실제로 이 자리로 스마트무브시키고 나서
	//			// 오토에임키를 다시 눌러보면 마우스가 이동됨을 알 수 있다.
	//			// 즉, 처음 Answer을 설정하고 나서 구한 위치가 실제 렌더링되는 위치와 약간 다른데,
	//			// 이건 렌더링 시작 전에 UpdateVisibleCount함수가 호출되고 이때 영역이 재계산되면서 오차가 발생하기 때문이다.
	//			// 이것까지 제대로 맞추려면 내부를 뜯는게 깔끔하므로 우선은 그냥 이렇게 가기로 하겠다.
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

		// 버튼을 눌러서 가는 상황이면 Answer는 무조건 한개
		szIndex  = m_answers[0].szLinkIndex;
		szTarget = m_answers[0].szLinkTarget;
		return true;
	}
	else if( nCommand == EVENT_TEXTBOX_SELECTION)
	{
		ASSERT(m_pTextBoxAnswer);
		// 한줄이 넘어가는 경우때문에 이렇게 그냥 라인인덱스로 구하면 안된다.
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