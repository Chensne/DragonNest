#pragma once

#include "SmartPtr.h"

class CDnActor;
typedef CSmartPtr< CDnActor > DnActorHandle;

class CDnPlayerActor;
typedef CSmartPtr< CDnPlayerActor > DnPlayerActorHandle;

class CDnWeapon;
typedef CSmartPtr< CDnWeapon > DnWeaponHandle;

class CDnCamera;
typedef CSmartPtr< CDnCamera > DnCameraHandle;

class CDnWorldProp;
typedef CSmartPtr< CDnWorldProp > DnPropHandle;

class CDnMonsterActor;
typedef CSmartPtr< CDnMonsterActor > DnMonsterActorHandle;

class CDnSkillBase;
typedef CSmartPtr< CDnSkillBase > DnSkillHandle_;

class CDnSkill;
typedef CSmartPtr< CDnSkill > DnSkillHandle;

class CDnDropItem;
typedef CSmartPtr< CDnDropItem > DnDropItemHandle;

class CDnParts;
typedef CSmartPtr<CDnParts> DnPartsHandle;

class CDnGlyph;
typedef CSmartPtr<CDnGlyph> DnGlyphHandle;

class CDnBlow;
typedef CSmartPtr<CDnBlow> DnBlowHandle;

class CDnEtcObject;
typedef CSmartPtr<CDnEtcObject> DnEtcHandle;

class CDnSlotButton;
typedef CSmartPtr< CDnSlotButton > DnSlotButtonHandle;

class CDnRenderAvatar;
typedef CSmartPtr< CDnRenderAvatar > DnRenderAvatarHandle;

class CDnEyeLightTrailObject;
typedef CSmartPtr<CDnEyeLightTrailObject> DnEyeLightTrailHandle;

class CDnVehicleActor;
typedef CSmartPtr<CDnVehicleActor> DnVehicleHandle;

class CDnPartsVehicleEffect;
typedef CSmartPtr<CDnPartsVehicleEffect> DnPartsVehicleEffectHandle;

class CDnSimpleParts;
typedef CSmartPtr<CDnSimpleParts> DnSimplePartsHandle;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
class CDnTalisman;
typedef CSmartPtr<CDnTalisman> DnTalismanHandle;
#endif // PRE_ADD_TALISMAN_SYSTEM

#if defined(PRE_ADD_DRAGON_GEM)
class CDnDragonGem;
typedef CSmartPtr<CDnDragonGem> DnDragonGemHandle;
#endif // PRE_ADD_DRAGON_GEM
