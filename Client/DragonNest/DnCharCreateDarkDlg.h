#pragma once
#include "DnCustomDlg.h"

#define DARK_CLASS_MAX 2
#ifdef PRE_MOD_SELECT_CHAR

class CDnCharCreateDarkDlg : public CDnCustomDlg
{
public:
	CDnCharCreateDarkDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharCreateDarkDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );
	void SetClassName( int id );
	void ResetSelection( );
	void ResetCheckbox();
private:
	CEtUITextureControl * m_pTextureBack; //big picture ID_TEXTUREL_BACK
	CEtUITextureControl * m_pTextureClass[DARK_CLASS_MAX];//rlkt_dark need increase if more than 2 characters.
	CEtUIStatic * m_pClassName;
	
	EtTextureHandle m_hBgTextureClass[DARK_CLASS_MAX];
	EtTextureHandle m_hBgTexture;

	
	CEtUIRadioButton * m_pRadioButton[DARK_CLASS_MAX];

};

#endif // PRE_MOD_SELECT_CHAR