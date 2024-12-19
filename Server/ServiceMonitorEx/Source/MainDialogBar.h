#pragma once


// CMainDialogBar

class CMainDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CMainDialogBar)

public:
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_TITLELINE_PAD_X		= 8,		// 메인 다이얼로그 바 제목 외부 간격 X
		EV_UIS_TITLELINE_PAD_Y		= 8,		// 메인 다이얼로그 바 제목 외부 간격 Y
		EV_UIS_TITLELINE_HGT		= 18,		// 메인 다이얼로그 바 제목 높이

		EV_UIS_TITLETEXT_PAD_X		= 10,		// 메인 다이얼로그 바 제목 외부 간격 X
	};

public:
	CMainDialogBar();
	virtual ~CMainDialogBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


