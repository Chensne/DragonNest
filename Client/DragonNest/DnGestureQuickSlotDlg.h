#pragma once

#include "DnCustomDlg.h"
#include "DnCustomControlCommon.h"

class CDnSlotButton;
class CDnLifeSkillButton;
class MIInventoryItem;
class CDnItem;

// LifeSkill 퀵슬롯 다이얼로그(Normal_Stand 에서만 나가는 스킬들)
class CDnLifeSkillQuickSlotDlg : public CDnCustomDlg
{
public:
	CDnLifeSkillQuickSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeSkillQuickSlotDlg(void);

protected:
	std::vector< CDnLifeSkillButton* > m_vecSlotButton;

protected:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	// 제스처버튼과 2차스킬버튼 둘을 동시에 보여줄 수 있는 구조기때문에, 이렇게 아이템타입을 추가로 전달해야한다.
	int FindLifeSkillSlotItem( int nLifeSkillID, MIInventoryItem::InvenItemTypeEnum ItemType );
	int FindVehicleSlotItem( INT64 nItemSerialID, MIInventoryItem::InvenItemTypeEnum ItemType );

	// 기본 퀵슬롯과 달리, 다이얼로그가 하나뿐이라, MainDlg에 안만들고 직접 처리할 수 있다.
	void ChangeQuickSlotButton( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	bool SetSlot( int nIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType );

	// 스킬을 슬롯에 넣을때 소리, 집을때 소리
	int m_nSkillLaydownSoundIndex;
	int m_nSkillDragSoundIndex;

	// 처음 이거 구현할때 생각했던 방법들이다.
	// 방법 1.
	// 퀵슬롯 버튼 하나를 생성해서 그냥 렌더링용으로 들고있는다
	//
	// 방법 2.
	// 완전한 통합. LifeSkillButton따로뺀걸 전부 다 QuickSlotButton으로 통합 후 ui 커스텀 컨트롤 설정 바꾸고 통합한다.
	// => 너무 뜯을게 많다. ui도 다 뜯어야함. 나중에 혹시 퀵슬롯 개편하면 완전 두번 일하는 것이다.
	//
	// 방법 3.
	// 탈것, 펫만 등록할 수 있게 LifeSkillButton을 확장한다.
	// 구현하기 애매하다. 결국 ItemSlotButton의 텍스처랑 다 들고있어야하는데, 이건 중복코드니 제외한다.
	//
	// 그래서 선택한건 방법 1이다.
	// 문제는 템플릿이 없으면 CreateControl해도 템플릿 연결을 할 수 없기때문에 ui에 미리 넣어두고 연결해서 사용하는 방법으로 해야한다.
	// 그리고 장착하고나면 인벤에서 없어지기때문에 현재 구조상 퀵슬롯에서 자동으로 빠지게 되어있다.
	// 이걸 해결하기 위해 DnItem도 들고있기로 한다.
	struct LifeSkillEx
	{
		CDnQuickSlotButton *pSlotButton;
		CDnItem *pItem;
	};
	std::map<CDnLifeSkillButton*, LifeSkillEx> m_mapMatchedButton;
	float m_fVehicleCoolTime;
	int m_nGestureSkillIndex;

public:
	bool InitSlot( int nIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType );
	void ResetSlot( int nIndex, bool bSendPacket = true );
	bool ResetSecondarySkillQuickSlot( int nSecondarySkillID );
	void EnableQuickSlot(bool bEnable);
	void RefreshVehicleItem();

public:
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
};