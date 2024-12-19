#pragma once

#include "DnMODDlgBase.h"

class CDnDamageCount;
class CDnMODCustom2Dlg : public CDnMODDlgBase
{
public:
	CDnMODCustom2Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnMODCustom2Dlg();

protected:
	std::vector<CEtUIStatic *> m_pVecStaticText;
	std::vector<CEtUIProgressBar *> m_pVecStaticProgress;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

public:

};