#pragma once

#include <wx/dcbuffer.h>

#include "cwxTLObjectMoveEvent.h"
#include "cwxTLObjectResizeEvent.h"
#include "cwxTLObjectSelectEvent.h"
#include "cwxTLAxisEvent.h"


#define TL_SHOW_OBJECT_TIME_LENGTH 1
#define TL_SHOW_OBJECT_NAME 2



// �ᱹ ���� ����� Ÿ�� ���� ��Ʈ��.... -_-
class cwxTimeLineCtrl : public wxScrolledWindow
{
private:
	//DECLARE_DYNAMIC_CLASS( cwxTimeLineCtrl );

	enum
	{
		OBJECT_CENTER,
		OBJECT_BEGIN,
		OBJECT_END,
		OBJECT_NOT_SELECTED,
	};

	struct S_OBJECT_INFO
	{
		wxString Actor;
		wxString Name;
		int iID;
		float fStartTime;
		float fEndTime;
		int iUseType;		// ������� �뵵�� ���� ������ ����� �� ����.
		bool bSizeFixed;
		
		float fOriStartXPixel;
		float fOriEndXPixel;

		DWORD dwStartXPixel;
		DWORD dwEndXPixel;

		wxColour Color;

		bool operator != ( const S_OBJECT_INFO& Rhs )
		{
			return Actor != Rhs.Actor ||
				   Name != Rhs.Name ||
				   iID != Rhs.iID ||
				   fStartTime != Rhs.fStartTime ||
				   fEndTime != Rhs.fEndTime ||
				   iUseType != Rhs.iUseType ||
				   dwStartXPixel != Rhs.dwStartXPixel ||
				   dwEndXPixel != Rhs.dwEndXPixel;
		}
	};


	struct S_ITEM_INFO
	{
		wxString Name;
		wxColour Color;

		vector<S_OBJECT_INFO*>		m_vlObjectInfo;

		~S_ITEM_INFO()
		{
			for_each( m_vlObjectInfo.begin(), m_vlObjectInfo.end(), 
					  DeleteData<S_OBJECT_INFO*>() );
		}
	};
	

	DWORD				m_dwItemVerticalOffset;
	DWORD				m_dwWidth;
	DWORD				m_dwHeight;

	float				m_fOriRulerPixelStep;
	float				m_fRulerStep;

	float				m_fScale;
	wxString			m_strUnitName;

	float				m_fViewSize;

	S_OBJECT_INFO		m_PrevObjectInfo;
	S_OBJECT_INFO*	m_pSelectedObjectInfo;
	
	int					m_iLastClkXPos;
	bool				m_bIsLBtnDown;

	int					m_iSelectedObjectPart;

	//DWORD				m_dwNowTimeBarPosByUnit;
	DWORD				m_dwNowOriTimeBarPosByPixel;
	float				m_fNowTimeBarPosByPixel;

	bool				m_bTimeBarClked;
	bool				m_bShowTimeLineAsFloat;

	bool				m_bEnable;

	DWORD				m_dwObjectDrawStyle;

	DWORD				m_dwLastPaintTime;


	vector<S_ITEM_INFO*>				m_vlpItemInfo;
	map<wxString, S_ITEM_INFO*>			m_mapItemInfo;
	vector<S_OBJECT_INFO*>				m_vlpObjects;

	wxPoint				m_LastClkMousePos;
	bool				m_bMoved;

	int					m_iOnOverYLineIndex;
	float				m_fMousePosTime;

	int					m_iXScrollOffset;
	int					m_iYScrollOffset;

	// �� ��¥ �̰� ��ũ�� �̺�Ʈ ���� �ƴѰ�. THUMB ���� �޽��� ���� ����� �������� ���ƿ��� �ʳ�.
	bool				m_bNeedUpdateOffset;

	int					m_iRulerDivideFactor;

private:
	void _UpdateScrollBar( void );
	void _UpdateViewScale( void );

	void _DrawBaseLine( wxDC& BufferedDC, DWORD dwDrawLineXOffset, DWORD dwDrawLineYOffset, wxSize &ClientSize );
	void _DrawRuler( wxAutoBufferedPaintDC& BufferedDC, DWORD dwDrawLineXOffset, DWORD dwDrawLineYOffset, wxSize &ClientSize );
	//void _DrawGraduation( wxAutoBufferedPaintDC& BufferedDC, DWORD dwDrawLineYOffset, DWORD dwGraduationStep, DWORD dwStartPixel, DWORD dwEndPixel );
	void _DrawObjects( wxAutoBufferedPaintDC& BufferedDC, DWORD dwDrawLineXOffset, DWORD dwDrawLineYOffset, wxSize &ClientSize );
	void _DrawTimeBar( wxAutoBufferedPaintDC& BufferedDC, DWORD dwDrawLineYOffset, DWORD dwWholeHeight );

	// ��Ʈ���� ����ϰ� �ִ� �θ� �����쿡�� �̺�Ʈ�� ����
	void _NotifyEvent( int iEvent );

public:
	cwxTimeLineCtrl( void );
	cwxTimeLineCtrl( wxWindow* pParent, wxWindowID id );
	virtual ~cwxTimeLineCtrl(void);

	// from wxWidget base 
	bool Enable( bool bEnable = true );

	// �������̽�
	//void SetScale( float fScale );
	// �ð� ���̸� ������ ���� ���� �ڵ����� unit name ���������� ����. �ƴϸ� ���� ������ �ܺο��� ���� �� �ֵ��� �ұ�. ����Ʈ�� �ð����� �̸� ���������� ����������!
	//void SetUnitName( wxString& strUnitName );
	void SetViewScale( float fScaleSize );
	
	bool AddItem( const wxString& Name, const wxColour& Color );
	bool DelItem( const wxString& Name );
	
	bool AddObject( const wxString& ActorName, const wxString& ObjectName, int iObjectID, 
					 int iUseType, bool bSizeFixed, float fStartTime = 0.0f, float fLength = 20.0f, const wxColour& Color = wxColour(200, 200, 200) );
	bool ModifyObject( const wxString& ActorName, const wxString& ObjectName, int iObjectID, 
					   int iUseType, bool bSizeFixed, float fStartTime = 0.0f, float fLength = 20.0f, const wxColour& Color = wxColour(200, 200, 200) );
	bool DelObject( const wxString& ActorName, int iObjectID );

	bool FixObjectSize( int iObjectID, bool bFix );

	void SelectObject( int iObjectID );
	int GetSelectedObjectID( void );
	int GetSelectedObjectUseType( void );

	float GetNextObjectStartTime( const wxChar* pActorName );
	float GetPrevObjectEndTime( const wxChar* pActorName, int iID );
	int GetPrevObjectID( const wxChar* pActorName, int iID );

	void UpdateTimeBar( float fTime );
	void ResetTimeBar( void );

	// �̺�Ʈ �Լ���
	void OnPaint( wxPaintEvent& PaintEvent );
	void OnScroll( wxScrollWinEvent& ScrollEvent );

	void OnResize( wxSizeEvent& SizeEvent );
	void OnMouseEvent( wxMouseEvent& MouseEvent );

	//void SetTimeLineAsFloat( bool bShowTimeLineAsFloat ) { m_bShowTimeLineAsFloat = bShowTimeLineAsFloat; };

	float GetNowTime( void );
	//float GetScale( void ) { return m_fScale; };

	void Clear();

	// ��Ÿ�� ���� �Լ�
	void SetStyle( DWORD dwStyle );

	int GetYLineIndex( void ) { return m_iOnOverYLineIndex; };
	float GetNowMousePosTime( void ) { return m_fMousePosTime; }

	DECLARE_EVENT_TABLE();
};
