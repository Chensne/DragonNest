#pragma once

#include "DnDummyClientMgr.h"

#ifdef _ENG
const static TCHAR* gs_Column[] = { _T("Index"), _T("Connection"), _T("id"), _T("Character"), _T("Status"), _T("Location"), _T(" Connect Time "), _T("Response Time"), _T("Response Avr"),_T("Hited"), _T("Hit") };
#else
const static TCHAR* gs_Column[] = { _T("�ε���"), _T("�������"), _T("���̵�"), _T("ĳ����"), _T("�������"), _T("������ġ"), _T(" ����  �ð� "), _T("����ð�"), _T("�������"),_T("�¾Ҵ�"), _T("���ȴ�") };
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