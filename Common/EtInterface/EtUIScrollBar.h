#pragma once
#include "etuicontrol.h"

#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

class CEtUIScrollBar : public CEtUIControl
{
public:
	CEtUIScrollBar( CEtUIDialog *pParent );
	virtual ~CEtUIScrollBar(void);

protected:
	enum ARROWSTATE 
	{ 
		CLEAR, 
		CLICKED_UP, 
		CLICKED_DOWN, 
		HELD_UP, 
		HELD_DOWN 
	};

	enum emElementType
	{
		typeBase,
		typeUpButton,
		typeDownButton,
		typeThumb,
		typeUpButtonLight,
		typeDownButtonLight,
		typeThumbLight,
		typeMaxElementCount,
	};

	enum emBaseType
	{
		typeBaseUp,
		typeBaseMid,
		typeBaseDown,
		typeBaseMaxCount,
	};

	bool m_bShowThumb;
	bool m_bDrag;

	SUICoord m_UpButtonCoord;
	SUICoord m_UpButtonLightCoord;
	SUICoord m_DownButtonCoord;
	SUICoord m_DownButtonLightCoord;
	SUICoord m_ThumbCoord;
	SUICoord m_ThumbLightCoord;
	SUICoord *m_ElementCoord[typeMaxElementCount];

	SUICoord m_TrackCoord;
	SUICoord m_BaseCoordUp;
	SUICoord m_BaseCoordMid;
	SUICoord m_BaseCoordDown;
	SUICoord *m_BaseCoord[typeBaseMaxCount];
	SUICoord m_BaseUVCoord[typeBaseMaxCount];

	int m_nPosition;  // Position of the first displayed item
	int m_nPageSize;  // How many items are displayable in one page
	int m_nStart;     // First item
	int m_nEnd;       // The index after the last item
	float m_fLastMouseX;
	float m_fLastMouseY;
	ARROWSTATE m_Arrow; // State of the arrows
	LARGE_INTEGER m_liArrowTime;

	CEtUIControl *m_pParentControl;

public:
	void SetTrackRange( int nStart, int nEnd );
	void GetTrackRange( int &nStart, int &nEnd );
	int GetTrackPos() { return m_nPosition; }
	void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
	int GetPageSize() { return m_nPageSize; }
	void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

	void Scroll( int nDelta );
	void ShowItem( int nIndex );

	bool IsArrowStateClear()			{ return m_Arrow==CLEAR; }
	void SetParentControl( CEtUIControl *pControl ) { m_pParentControl = pControl; }
	bool IsShowThumb() { return m_bShowThumb; };

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void SetTemplate( int nTemplateIndex );
	virtual void UpdateRects();

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void Render( float fElapsedTime );

protected:
	void UpdateThumbRect();
	void Cap();  // Clips position at boundaries. Ensures it stays within legal range.
};
