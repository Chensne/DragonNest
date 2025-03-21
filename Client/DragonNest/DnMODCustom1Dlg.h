#pragma once

#include "DnMODDlgBase.h"

class CDnDamageCount;
class CDnMODCustom1Dlg : public CDnMODDlgBase
{
public:
	CDnMODCustom1Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnMODCustom1Dlg();

protected:
	std::vector<CDnDamageCount *> m_pVecStaticCounter;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

public:

};