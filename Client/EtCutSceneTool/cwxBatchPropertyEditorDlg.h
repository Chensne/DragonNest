#pragma once
#include <wx/PropGrid/PropGrid.h>
#include <wx/ListCtrl.h>



// 프로퍼티들을 일괄 편집하기 위한 다이얼로그
class cwxBatchPropertyEditorDlg : public wxDialog
{
public:
	enum
	{
		FILTER_PROP_GRID_ID = 3000,
		CHANGE_VALUE_GRID_ID,
		LIST_FILTERED_RESULT_ID,
	};

	enum
	{
		START_TIME_DELTA,
		STANDARD_TIME,
		INCLUDE_ACTIONS,
		ACTION_PARENT_PROP,
		INCLUDE_KEYS,
		KEY_PARENT_PROP,
		INCLUDE_EVENTS,			// TODO: 하위 이벤트들의 세부 속성도 만들어주자.
		EVENT_PARENT_PROP,

		// 순서는 CToolData의 이벤트 열거체와 동일! 이래야 인덱스갖고 같이 짝맞춰서 움직이기 편함.
		// 바꿔말하면 이벤트 갯수가 늘어나거나 순서가 변경된다거나하면 여기도 변경시켜줘야 함!
		INCLUDE_CAMERA,
		INCLUDE_PARTICLE,
		INCLUDE_DOF,
		INCLUDE_FADE,
		INCLUDE_PROP,
		INCLUDE_SOUND,
		INCLUDE_SUBTITLE,
		INCLUDE_IMAGE,
		PROP_COUNT,
	};

private:
	wxBoxSizer*			m_pTopSizer;
	wxBoxSizer*			m_pPropertySizer;
	wxStaticBoxSizer*	m_pFilterPropGridSizer;
	wxStaticBoxSizer*	m_pChangeValueGridSizer;
	wxStaticBoxSizer*	m_pResultListSizer;
	wxSizer*			m_pBasicBtnSizer;

	wxPropertyGrid*		m_pFilterPropGrid;
	wxPropertyGrid*		m_pChangeValueGrid;

	wxListView*			m_pResultListView;

	//wxButton*			m_pOKButton;
	//wxButton*			m_pCancelButton;

	map<int, int>		m_mapEventIndex;
	wxPGId				m_aPGID[ PROP_COUNT ];
	vector<wxPGId>		m_vlActionActorNamePGID;
	vector<wxPGId>		m_vlKeyActorNamePGID;

	// 변경될 데이터 모음
	float				m_fStartTimeDelta;
	vector<const ActionInfo*> m_vlpFilteredActionInfo;
	vector<const KeyInfo*> m_vlpFilteredKeyInfo;
	vector<const EventInfo*> m_vlpFilteredEventInfo;

protected:
	void _InitFilterProperty( void );
	void _InitChangeValueProperty( void );
	void _BuildFilterProperty( void );
	void _BuildChangeValueProperty( void );
	void _SetupResultListColumn( void );
	void _ReflectFilter( void );

public:
	cwxBatchPropertyEditorDlg( wxWindow* pParent, wxWindowID id, const wxString& title,
							   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							   long style = wxDEFAULT_DIALOG_STYLE );

	virtual ~cwxBatchPropertyEditorDlg(void);

	void OnFilterPropertyChanged( wxPropertyGridEvent& PGEvent );
	void OnChangeValuePropertyChanged( wxPropertyGridEvent& PGEvent );
	void OnOK( wxCommandEvent& ButonEvent );

	DECLARE_EVENT_TABLE()
};
