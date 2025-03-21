#pragma once
#include "EtUIHtmlTextBox.h"

// NpcAnswer의 경우 일반 HtmlTextBox와 달리
// 긴 대사에 대해 두줄짜리 선택 및 인덱스 처리를 해야되서 커스텀으로 구현.
class CDnNpcAnswerHtmlTextBox : public CEtUIHtmlTextBox
{
public:
	CDnNpcAnswerHtmlTextBox(CEtUIDialog *pParent);
	virtual ~CDnNpcAnswerHtmlTextBox(void);

protected:
	int m_nAnswerIndex;

	// 텍스트 라인 선택시 나오는 사운드.
	// Npc대화 답변에서만 사용하기 때문에, 컨트롤 속성에 사운드를 추가하지 않고
	// 이렇게 코드안에서 셋팅하는 것으로 하겠다.
	int m_nSelectionSoundIndex;

#define NPC_ANSWER_ARROW
#ifdef NPC_ANSWER_ARROW
	// Arrow위치
	int m_nArrowLineIndex;
	float m_fOffsetTime;
#endif

#define NPC_ANSWER_KEYBOARD_SELECT
#ifdef NPC_ANSWER_KEYBOARD_SELECT
	int m_nKeyboardSelectLineIndex;
#endif

protected:
	virtual void BeginParse(DWORD dwAppData, bool &bAbort);
	virtual void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort);
	virtual void EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort);
	virtual void Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort);
	virtual void EndParse(DWORD dwAppData, bool bIsAborted);

	virtual void RenderText( float fElapsedTime );
#ifdef NPC_ANSWER_ARROW
	virtual void RenderWord( float fElapsedTime, CWord& sWord, bool bRollOver = false, int nBorderFlag = 0 );
#endif

public:
	virtual void Initialize( SUIControlProperty *pProperty );
#ifdef NPC_ANSWER_KEYBOARD_SELECT
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
#ifdef NPC_ANSWER_ARROW
	virtual void Render( float fElapsedTime );
#endif

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	void GetLinePos( std::vector<EtVector2> &vecSmartMovePos );
#endif
};