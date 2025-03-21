#pragma once

#include "DnDummyClientMgr.h"

#ifdef _ENG
const static TCHAR* gs_Column[] = { _T("Index"), _T("Connection"), _T("id"), _T("Character"), _T("Status"), _T("Location"), _T(" Connect Time "), _T("Response Time"), _T("Response Avr"),_T("Hited"), _T("Hit") };
#else
const static TCHAR* gs_Column[] = { _T("인덱스"), _T("연결상태"), _T("아이디"), _T("캐릭터"), _T("현재상태"), _T("현재위치"), _T(" 접속  시간 "), _T("응답시간"), _T("응답평균"),_T("맞았다"), _T("때렸다") };
#endif

class DnDummyInfoListCtrl : public wxListCtrl
{
public:
	DnDummyInfoListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size);
	virtual ~DnDummyInfoListCtrl();

	void ResetAll();
	void InsertDummyInfo(int nIdx, DnDummyClient::Property& property);
	void UpdateDummyInfo(int nIdx, DnDummyClient::Property& property);

	void GetSelectedIndex(OUT std::vector<int>& out);
	void OnFocusedItem(wxListEvent& event);
private:
	wxListItemAttr m_attr;

	DECLARE_EVENT_TABLE()
};

extern DnDummyInfoListCtrl*	g_pDummyInfoListCtrl;