#pragma once
#include "EtUILineEditBox.h"
#include "EtUIIME.h"

class CEtUILineIMEEditBox : public CEtUILineEditBox, public CEtUIIME
{
public:
	CEtUILineIMEEditBox( CEtUIDialog *pParent );
	virtual ~CEtUILineIMEEditBox(void);

protected:
	DWORD m_dwCompTargetNonBkColor;
	DWORD m_dwCompTargetBkColor;
	DWORD m_dwCompTargetColor;
	DWORD m_dwCompTargetNonColor;
	SUICoord m_IndicatorCoord;

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Focus( bool fFocus );

	void PumpMessage();

	virtual void Render( float fElapsedTime );
	virtual void RenderCandidateReadingWindow( float fElapsedTime, bool bReading );
	virtual void RenderComposition( float fElapsedTime );

protected:
	void TruncateCompString( bool bUseBackSpace = true, int iNewStrLen = 0 );
	void FinalizeString( bool bSend );
	void SendCompString();
};
