#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "SubControlBaseView.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include <map>
#include <vector>


// CSubControlProcMakeDump 대화 상자입니다.

class CSubControlProcMakeDump : public CSubControlBaseView
{
	DECLARE_DYNAMIC(CSubControlProcMakeDump)

public:
	// SERVER
	typedef	std::pair<INT, INT>				TP_LISTELEM;
	typedef	std::vector<TP_LISTELEM>				TP_LISTAUTO;
	typedef	TP_LISTAUTO::iterator					TP_LISTAUTO_ITR;
	typedef	TP_LISTAUTO::const_iterator				TP_LISTAUTO_CTR;
	typedef	std::map<INT, SERVERINFO>		TP_LISTSERV;
	typedef	TP_LISTSERV::iterator					TP_LISTSERV_ITR;
	typedef	TP_LISTSERV::const_iterator				TP_LISTSERV_CTR;
	typedef	std::vector<SERVERINFO>					TP_LISTSERVAUTO;
	typedef	TP_LISTSERVAUTO::iterator				TP_LISTSERVAUTO_ITR;
	typedef	TP_LISTSERVAUTO::const_iterator			TP_LISTSERVAUTO_CTR;

private:
	class CSortList
	{
	public:
		bool operator()(const TP_LISTELEM& pLv, const TP_LISTELEM& pRv) const {
			if (static_cast<INT>(pLv.first) < static_cast<INT>(pRv.first)) {
				return false;
			}
			return true;
		}
	};

public:
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_LIST_PAD_TOP				= 30,		// 표시 목록 외부 간격 상단
		EV_UIS_LIST_PAD_X				= 10,		// 표시 목록 외부 간격 X
		EV_UIS_LIST_PAD_Y				= 10,		// 표시 목록 외부 간격 Y
		EV_UIS_LIST_WTH					= 305,		// 표시 목록 너비

		EV_UIS_GROUP_PAD_TOP			= 25,		// 선택 그룹 외부 간격 상단
		EV_UIS_GROUP_PAD_X				= 10,		// 선택 그룹 외부 간격 X
		EV_UIS_GROUP_PAD_Y				= 10,		// 선택 그룹 외부 간격 Y
		EV_UIS_GROUP_GAP_X				= 10,		// 선택 그룹 사이 간격 X

		EV_UIS_RADIO_PAD_LFT			= 20,		// 선택 라디오 버튼 외부 간격 좌측
		EV_UIS_RADIO_PAD_TOP			= 50,		// 선택 라디오 버튼 외부 간격 상단
		EV_UIS_RADIO_PAD_Y				= 25,		// 선택 라디오 버튼 외부 간격 Y
		EV_UIS_RADIO_WTH				= 200,		// 선택 라디오 버튼 너비
		EV_UIS_RADIO_HGT				= 15,		// 선택 라디오 버튼 높이

		EV_UIS_REQUESTBUTTON_PAD_A		= 10,		// 취소 버튼 외부 간격
		EV_UIS_REQUESTBUTTON_WTH		= 80,		// 취소 버튼 너비
		EV_UIS_REQUESTBUTTON_HGT		= 20,		// 취소 버튼 높이
	};

public:
	CSubControlProcMakeDump(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSubControlProcMakeDump();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CTRLFRM_PROCMAKEDUMP };

public:
	void UpdateData();

	void DoPopupMenu();
	void DeleteCurSelItem();

	void SetSession(CNetSession* pSession) {m_pSession = pSession;}

private:
	void AddGridList(const struct SERVERINFO* pServerInfo);
	void ClearGridList() { m_GridList.DeleteAllItems(); }

	static LRESULT CALLBACK GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam);

private:
	COXGridList	m_GridList;
	COXGridEdit	m_GridEdit;

	TP_LISTSERV m_ListServ;

	WNDPROC m_OldGridListSubProc;

	CNetSession* m_pSession;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedProcMakeDumpRequest();
	afx_msg void OnDestroy();
	afx_msg void OnPopupProcMakeDumpDeleteItem();
};
