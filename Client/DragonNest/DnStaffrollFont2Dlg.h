#include "EtUIDialog.h"
#include "DnStaffrollFontBase.h"

class CDnStaffrollFont2Dlg : public CEtUIDialog, public CDnStaffrollFontBase
{
public:
	CDnStaffrollFont2Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStaffrollFont2Dlg();

protected:
	CEtUIStatic *m_pStaticText;
	DWORD m_dwOrigColor;
	DWORD m_dwOrigShadowColor;
	DWORD m_dwOrigTexColor;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	void SetText( const WCHAR *wszStr );
	void SetAlpha( float fAlpha );
};