#pragma once
#include "EtUIDialog.h"

class CDnSlideCaptionDlg : public CEtUIDialog
{
	struct SSlideInfo
	{
		std::wstring m_wszMsg;
		DWORD m_dwColor;
		float m_fRemainShowTime;

		SSlideInfo()
			: m_dwColor(0xFFFFFFFF)
			, m_fRemainShowTime(0.0f)
		{
		}

		void SetInfo( LPCWSTR wszMsg, DWORD dwColor, float fShowTime )
		{
			WCHAR wszText[512];
			//swprintf_s( wszText, _countof(wszText), wszMsg );	// fix Format String Bug
			_wcscpy( wszText, _countof(wszText), wszMsg, (int)wcslen(wszMsg) );
			int nLen = (int)wcslen( wszText );
			for( int i = 0; i < nLen; ++i ) {
				if( wszText[i] == '\n' )
					wszText[i] = ' ';
			}
			m_wszMsg = wszText;
			m_dwColor = dwColor;
			m_fRemainShowTime = fShowTime;
		}
	};

public:
	CDnSlideCaptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSlideCaptionDlg(void);

protected:
	SUICoord m_uiBlind;

	EtColor m_srcColor;
	EtColor m_destColor;
	EtColor m_renderColor;
	float m_fConst;
	float m_fFadeTime;

	CEtUIStatic *m_pCation;
	std::list<SSlideInfo> m_listSlideInfo;

	// 설정하는 텍스트는 리스트 안에 있는 슬라이드들이 번갈아가면서 할 수 있도록, 별도의 인덱스로 관리한다.
	int m_nLastSlideIndex;

	// 렌더링용 슬라이드. 따로 가지고 있어야 슬라이드 중 리스트에서 삭제되더라도 지나갈때까지는 보여줄 수 있다.
	SSlideInfo m_RenderSlideInfo;
	float m_fTextWidth;
	bool m_bShowSlideText;	// 현재 보여지고 있다면 true

	// 아무래도 알파값으로 렌더링 여부 확인을 하니 뭔가 잘못될 경우가 있는 거 같다.
	bool m_bRender;

	virtual void UpdateBlind();
	void SetCaption( LPCWSTR wszMsg, DWORD dwColor );

public:
	void AddCaption( LPCWSTR wszMsg, DWORD dwColor, float fShowTime );
	void CloseCaption( bool bForce );	// bForce가 true이면 현재 보여지고 있는 슬라이드도 즉시 사라집니다.

	void DelCaption( LPCWSTR wszMsg );
	void DelCaption( int nIndex );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	virtual void OnChangeResolution();
};