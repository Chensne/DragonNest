#include "DnCustomDlg.h"
#include "DnCharStatusDlg.h"
#include "DnRenderAvatar.h"
#include "DnRenderToUI.h"

class CDnCharVehicleDlg;
class CDnCharVehiclePreviewDlg;


class CDnCharVehicleDlg : public CDnCustomDlg
{
public :
	CDnCharVehicleDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharVehicleDlg(void);

	enum VehicleSlotIndex
	{
		VehicleSlot = 0,
		VehiclePartsSlot = 1,
		VehicleEffectSlot = 2
	};

private :
	CEtUIButton *m_pCancle;
	CEtUITextBox *m_pTextVehicleInfo;

	CDnItemSlotButton *m_pVehicleSlot;
	CDnItemSlotButton *m_pVehiclePartsSlot;
	CDnItemSlotButton *m_pVehicleEffectSlot;
	CDnCharVehiclePreviewDlg *m_pPreviewDlg;

public :
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0  */) ;
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );

	void SetSlotMagneticMode( bool bMagnetic );
	void ProcessUI();


	void SetVehicleEquipItem( int nEquipIndex, MIInventoryItem *pItem );
	void RefreshVehicleEquip();
	void ResetVehicleEquipSlot( int nEquipIndex );
	void ForceUnRideVehicle(bool bIgnoreRideCheck = false);

	void SetAvatarAction(const char* szActionName);
	void RefreshVehicleEquipPreview();
};

