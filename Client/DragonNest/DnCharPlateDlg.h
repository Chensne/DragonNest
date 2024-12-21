#include "DnCustomDlg.h"
#include "DnCharStatusDlg.h"

class CDnCharPlateInfoDlg;
class CDnTooltipPlateDlg;

class CDnCharPlateDlg : public CDnCustomDlg, public CEtUICallback
{
public :
	CDnCharPlateDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharPlateDlg(void);

private :

	CDnCharPlateInfoDlg	* m_pCharPlateInfoDlg;
	CDnTooltipPlateDlg *  m_pTooltipPlateDlg;
	std::vector< CDnItemSlotButton * > m_VecGlyphSlotButton;
	std::vector< CEtUIStatic * > m_VecGlyphCover;

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	std::vector< CEtUIStatic * > m_vecDragonGlyphGB;
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)

	enum{
		SPECIAL_GLYPH_COUNT = 4,
	};
	CDnSkillSlotButton * m_pSkillSlotButton[SPECIAL_GLYPH_COUNT];
	CEtUIStatic * m_pCashPeriod[CASHGLYPHSLOTMAX];

	CDnItem *	m_pRemoveItem;

	CDnSlotButton * m_pTempSlotButton;
	int	m_nTempSlotIndex;

	bool m_bSelectAttachGlyph;
	int m_nSelectAttachGlyphSlotIndex;

	bool m_bWithDrawOpen;	// 문장 해제 창으로 열렸는지 체크

	CDnGlyph * m_pLiftItem;
	CDnItemSlotButton * m_pLiftSlot;
	CEtUIStatic * m_pWithDrawGlyphText;
	CEtUIStatic * m_pWithDarwGlyphBackground;

public :
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0  */) ;
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	CDnCharStatusDlg::eRetWearable ValidWearableGlyphItem( const MIInventoryItem *pItem, std::wstring *strErrorMsg );
	CDnCharStatusDlg::eRetWearable GetWearableGlyphEquipType( const MIInventoryItem *pItem, eGlyph &equipType, std::wstring *strErrorMsg = NULL );
	CDnCharStatusDlg::eRetWearable GetSelectWearableGlyphEquipType( const MIInventoryItem *pItem, const eGlyph equipType, std::wstring *strErrorMsg = NULL );

	static int GLYPH_EQUIPINDEX_2_SLOTINDEX( int nEquipIndex );
	static int GLYPH_SLOTINDEX_2_EQUIPINDEX( int nSlotIndex );

	void SetSlotMagneticMode( bool bMagnetic );

	void SetGlyphEquipItem( int nEquipIndex, MIInventoryItem *pItem );
	void RefreshGlyphEquip();
	void ResetGlyphEquipSlot( int nEquipIndex );
	void RefreshCover( bool bInit = false );

	void EquipGlyphItem( int nSlotIndex, MIInventoryItem *pItem, CDnSlotButton *pSlotButton );

	MIInventoryItem *GetEquipGlyph( int nTypeParam );

	std::wstring GetPeriodTooltip( int nCashIndex );

	void SetSelectGlyphAttach( int nSelectGlyphAttachSlotIndex );
	void ClearSelectGlyphAttach();

	void GlyphLift( CDnGlyph * pItem );

	void SetWithDrawOpen( bool bOpen );
	bool GetWithDrawOpen();

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	void InitDragonGlyphSlot();
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
};