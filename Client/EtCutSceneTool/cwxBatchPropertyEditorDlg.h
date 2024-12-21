#pragma once
#include <wx/PropGrid/PropGrid.h>
#include <wx/ListCtrl.h>



// ������Ƽ���� �ϰ� �����ϱ� ���� ���̾�α�
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
		INCLUDE_EVENTS,			// TODO: ���� �̺�Ʈ���� ���� �Ӽ��� ���������.
		EVENT_PARENT_PROP,

		// ������ CToolData�� �̺�Ʈ ����ü�� ����! �̷��� �ε������� ���� ¦���缭 �����̱� ����.
		// �ٲ㸻�ϸ� �̺�Ʈ ������ �þ�ų� ������ ����ȴٰų��ϸ� ���⵵ ���������� ��!
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

	// ����� ������ ����
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
