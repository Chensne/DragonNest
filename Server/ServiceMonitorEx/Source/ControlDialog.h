#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "OXLayoutManager.h"
#include "afxcmn.h"
#include <map>
#include <vector>


// CControlDialog ��ȭ �����Դϴ�.

class CControlDialog : public CDialog
{
	DECLARE_DYNAMIC(CControlDialog)

public:
	// WORLD
	typedef	std::map<INT, WORLDINFO>					TP_LISTWRLD;
	typedef	TP_LISTWRLD::iterator						TP_LISTWRLD_ITR;
	typedef	TP_LISTWRLD::const_iterator					TP_LISTWRLD_CTR;
	typedef	std::vector<WORLDINFO>						TP_LISTWRLDAUTO;
	typedef	TP_LISTWRLDAUTO::iterator					TP_LISTWRLDAUTO_ITR;
	typedef	TP_LISTWRLDAUTO::const_iterator				TP_LISTWRLDAUTO_CTR;

	// SERVER
	typedef	std::map<INT, SERVERINFO>					TP_LISTSERV;
	typedef	TP_LISTSERV::iterator						TP_LISTSERV_ITR;
	typedef	TP_LISTSERV::const_iterator					TP_LISTSERV_CTR;
	typedef	std::vector<SERVERINFO>						TP_LISTSERVAUTO;
	typedef	TP_LISTSERVAUTO::iterator					TP_LISTSERVAUTO_ITR;
	typedef	TP_LISTSERVAUTO::const_iterator				TP_LISTSERVAUTO_CTR;

	// CHANNEL (VI)
	typedef	std::map<DWORD64, CHANNELINFO>				TP_LISTCHNL;
	typedef	TP_LISTCHNL::iterator						TP_LISTCHNL_ITR;
	typedef	TP_LISTCHNL::const_iterator					TP_LISTCHNL_CTR;
	typedef	std::vector<CHANNELINFO>					TP_LISTCHNLAUTO;
	typedef	TP_LISTCHNLAUTO::iterator					TP_LISTCHNLAUTO_ITR;
	typedef	TP_LISTCHNLAUTO::const_iterator				TP_LISTCHNLAUTO_CTR;

	// MERIT (GA, VI)
	typedef	std::map<INT, MERITINFO>					TP_LISTMRIT;
	typedef	TP_LISTMRIT::iterator						TP_LISTMRIT_ITR;
	typedef	TP_LISTMRIT::const_iterator					TP_LISTMRIT_CTR;
	typedef	std::vector<MERITINFO>						TP_LISTMRITAUTO;
	typedef	TP_LISTMRITAUTO::iterator					TP_LISTMRITAUTO_ITR;
	typedef	TP_LISTMRITAUTO::const_iterator				TP_LISTMRITAUTO_CTR;

	// NET LUNCHER
	typedef	std::map<INT, NETLUNCHERINFO>				TP_LISTNTLC;
	typedef	TP_LISTNTLC::iterator						TP_LISTNTLC_ITR;
	typedef	TP_LISTNTLC::const_iterator					TP_LISTNTLC_CTR;
	typedef	std::vector<NETLUNCHERINFO>					TP_LISTNTLCAUTO;
	typedef	TP_LISTNTLCAUTO::iterator					TP_LISTNTLCAUTO_ITR;
	typedef	TP_LISTNTLCAUTO::const_iterator				TP_LISTNTLCAUTO_CTR;

public:
	enum EF_UI_SIZE			// UI ũ�� ����
	{
		EV_UIS_MAINTITLELINE_PAD_X			= 8,		// �� ���� �ܺ� ���� X
		EV_UIS_MAINTITLELINE_PAD_Y			= 8,		// �� ���� �ܺ� ���� Y
		EV_UIS_MAINTITLELINE_HGT			= 18,		// �� ���� ����

		EV_UIS_MAINTITLETEXT_PAD_X			= 10,		// �� ���� �ܺ� ���� X

		EV_UIS_SUBTITLELINE_PAD_X			= 8,		// �� ���� �ܺ� ���� X
		EV_UIS_SUBTITLELINE_PAD_Y			= 34,		// �� ���� �ܺ� ���� Y
		EV_UIS_SUBTITLELINE_HGT				= 18,		// �� ���� ����

		EV_UIS_SUBTITLETEXT_PAD_X			= 10,		// �� ���� �ܺ� ���� X

		EV_UIS_TAB_PAD_TOP					= 64,		// �� ��� ���� Y
		EV_UIS_TAB_PAD_BTM					= 40,		// �� �ϴ� ���� Y
		EV_UIS_TAB_PAD_X					= 10,		// �� �ܺ� ���� X

		EV_UIS_CANCELBUTTON_PAD_A			= 10,		// ��� ��ư �ܺ� ����
		EV_UIS_CANCELBUTTON_WTH				= 80,		// ��� ��ư �ʺ�
		EV_UIS_CANCELBUTTON_HGT				= 20,		// ��� ��ư ����

		EV_UIS_SUBCONTROLVIEW_PAD_A			= 5,		// �� ���� �� �ܺ� ����
	};

public:
	CControlDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CControlDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CTRLDLG };

private:
	void DrawTitle(CDC* pDC);

public:
	void ActivateControlTab(EF_CONTROLTYPE pControlType);

	WORLDINFO* AtWorld(INT nWorldID);
	const WORLDINFO* AtWorld(INT nWorldID) const;
	void AddWorldList(const TP_LISTWRLDAUTO& pListWrld);
	void GetWorldList(TP_LISTWRLDAUTO& pListWrld);

	SERVERINFO* AtServer(INT pServerID);
	const SERVERINFO* AtServer(INT pServerID) const;
	void AddServerList(const TP_LISTSERVAUTO& pListServ);
	void GetServerList(TP_LISTSERVAUTO& pListServ);

	CHANNELINFO* AtChannel(INT pServerID, INT pChannelID);
	const CHANNELINFO* AtChannel(INT pServerID, INT pChannelID) const;
	void AddChannelList(const TP_LISTCHNLAUTO& pListChnl);
	void GetChannelList(TP_LISTCHNLAUTO& pListChnl);

	MERITINFO* AtMerit(INT pMeritID);
	const MERITINFO* AtMerit(INT pMeritID) const;
	void AddMeritList(const TP_LISTMRITAUTO& pListMrit);
	void GetMeritList(TP_LISTMRITAUTO& pListMrit);

	NETLUNCHERINFO* AtNetLuncher(INT pNetLuncherID);
	const NETLUNCHERINFO* AtNetLuncher(INT pNetLuncherID) const;
	void AddNetLuncherList(const TP_LISTNTLCAUTO& pListNtlc);
	void GetNetLuncherList(TP_LISTNTLCAUTO& pListNtlc);

	void ClearAllList();

	void UpdateDataAllView();

	void SetSession (CNetSession* pSession);

private:
	BOOL m_IsInitialized;

	COXLayoutManager m_LayoutManager;
	CTabCtrl m_ControlTab;

	CDialog* m_ControlView[EV_CTT_CNT];
	CDialog* m_CurControlView;

	TP_LISTWRLD m_ListWrld;
	TP_LISTSERV m_ListServ;
	TP_LISTCHNL m_ListChnl;
	TP_LISTMRIT m_ListMrit;
	TP_LISTNTLC m_ListNtlc;

	CNetSession* m_pSession;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//protected:
public:
	virtual void OnOK();
	virtual void OnCancel();
public:
//	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelChangeControlTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedControlCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
