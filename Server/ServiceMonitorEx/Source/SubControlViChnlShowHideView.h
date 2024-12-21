#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "SubControlBaseView.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include <map>
#include <vector>


// CSubControlViChnlShowHideView ��ȭ �����Դϴ�.

class CSubControlViChnlShowHideView : public CSubControlBaseView
{
	DECLARE_DYNAMIC(CSubControlViChnlShowHideView)

public:
	typedef	std::pair<INT, DWORD64>				TP_LISTELEM;
	typedef	std::vector<TP_LISTELEM>				TP_LISTAUTO;
	typedef	TP_LISTAUTO::iterator					TP_LISTAUTO_ITR;
	typedef	TP_LISTAUTO::const_iterator				TP_LISTAUTO_CTR;

	// SERVER
	typedef	std::map<INT, SERVERINFO>		TP_LISTSERV;
	typedef	TP_LISTSERV::iterator					TP_LISTSERV_ITR;
	typedef	TP_LISTSERV::const_iterator				TP_LISTSERV_CTR;
	typedef	std::vector<SERVERINFO>					TP_LISTSERVAUTO;
	typedef	TP_LISTSERVAUTO::iterator				TP_LISTSERVAUTO_ITR;
	typedef	TP_LISTSERVAUTO::const_iterator			TP_LISTSERVAUTO_CTR;

	// CHANNEL (VI)
	typedef	std::map<DWORD64, CHANNELINFO>		TP_LISTCHNL;
	typedef	TP_LISTCHNL::iterator					TP_LISTCHNL_ITR;
	typedef	TP_LISTCHNL::const_iterator				TP_LISTCHNL_CTR;
	typedef	std::vector<CHANNELINFO>				TP_LISTCHNLAUTO;
	typedef	TP_LISTCHNLAUTO::iterator				TP_LISTCHNLAUTO_ITR;
	typedef	TP_LISTCHNLAUTO::const_iterator			TP_LISTCHNLAUTO_CTR;

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
	enum EF_UI_SIZE			// UI ũ�� ����
	{
		EV_UIS_LIST_PAD_TOP				= 30,		// ǥ�� ��� �ܺ� ���� ���
		EV_UIS_LIST_PAD_X				= 10,		// ǥ�� ��� �ܺ� ���� X
		EV_UIS_LIST_PAD_Y				= 10,		// ǥ�� ��� �ܺ� ���� Y
		EV_UIS_LIST_WTH					= 445,		// ǥ�� ��� �ʺ�

		EV_UIS_GROUP_PAD_TOP			= 25,		// ���� �׷� �ܺ� ���� ���
		EV_UIS_GROUP_PAD_X				= 10,		// ���� �׷� �ܺ� ���� X
		EV_UIS_GROUP_PAD_Y				= 10,		// ���� �׷� �ܺ� ���� Y
		EV_UIS_GROUP_GAP_X				= 10,		// ���� �׷� ���� ���� X

		EV_UIS_RADIO_PAD_LFT			= 20,		// ���� ���� ��ư �ܺ� ���� ����
		EV_UIS_RADIO_PAD_TOP			= 50,		// ���� ���� ��ư �ܺ� ���� ���
		EV_UIS_RADIO_PAD_Y				= 25,		// ���� ���� ��ư �ܺ� ���� Y
		EV_UIS_RADIO_WTH				= 200,		// ���� ���� ��ư �ʺ�
		EV_UIS_RADIO_HGT				= 15,		// ���� ���� ��ư ����

		EV_UIS_REQUESTBUTTON_PAD_A		= 10,		// ��� ��ư �ܺ� ����
		EV_UIS_REQUESTBUTTON_WTH		= 80,		// ��� ��ư �ʺ�
		EV_UIS_REQUESTBUTTON_HGT		= 20,		// ��� ��ư ����
	};
	enum EF_REQUESTTYPE		// ��û ���
	{
		EV_RQT_SHOW,		// ����
		EV_RQT_HIDE,		// ����
		EV_RQT_CNT,
	};

public:
	CSubControlViChnlShowHideView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSubControlViChnlShowHideView();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CTRLFRM_VICHNLSHOWHIDE };

public:
	void UpdateData();

	void DoPopupMenu();
	void DeleteCurSelItem();

	void SetSession(CNetSession* pSession) {m_pSession = pSession;}

private:
	void AddGridList(const struct SERVERINFO* pServerInfo, const struct CHANNELINFO* pChannelInfo);
	void ClearGridList() { m_GridList.DeleteAllItems(); }

	static LRESULT CALLBACK GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam);

private:
	COXGridList	m_GridList;
	COXGridEdit	m_GridEdit;

	TP_LISTSERV m_ListServ;
	TP_LISTCHNL m_ListChnl;

	EF_REQUESTTYPE m_RequestType;

	WNDPROC m_OldGridListSubProc;

	CNetSession* m_pSession;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedViChnlShowHideRequest();
	afx_msg void OnBnClickedRadioGroup(UINT pCtrlID);
	afx_msg void OnDestroy();
	afx_msg void OnPopupVichnlShowHideDeleteItem();
};
