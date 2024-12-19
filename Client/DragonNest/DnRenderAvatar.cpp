#include "StdAfx.h"
#include "DnRenderAvatar.h"
#include "DnActorClassDefine.h"
#include "DnPlayerActor.h"
#include "DnItemTask.h"
#include "DnCharStatusDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnLoadingTask.h"
#include "DnInterface.h"
#include "DnPetActor.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

std::vector<CDnRenderAvatarNew::sTempTargetInfo> CDnRenderAvatarNew::s_vecTempTarget;
int CDnRenderAvatarNew::s_nTempTargetRefCount = 0;
CEtColorAdjustTexFilter *CDnRenderAvatarNew::s_pColorAdjFilter = NULL;
EtLightHandle CDnRenderAvatarNew::s_hTempLight[2];
EtTextureHandle CDnRenderAvatarNew::s_hTempEnviTexture;
EtColor CDnRenderAvatarNew::s_TempAmbient = 0xFFFFFFFF;
EtColor CDnRenderAvatarNew::s_TempLightDiffuse[2] = { 0xFFFFFFFF, 0xFFFFFFFF };
EtVector3 CDnRenderAvatarNew::s_TempLightDir[2] = { EtVector3( -0.58f, -0.58f, -0.58f ), EtVector3( 0.0f, -1.0f, 0.0f ) };
bool CDnRenderAvatarNew::s_bInitializeStaticObject = false;

CDnRenderAvatarNew::CDnRenderAvatarNew()
{
	m_bUseColorAdjFilter = false;
	m_bUseBorderAlpha = false;
	m_fDefaultCameraYPos = 56.0f;
	m_fDefaultCameraZPos = 150.0f;
	m_eActorType = ActorEnum::None;
	InitCameraControl();

	m_bUseTempLight = false;

	s_nTempTargetRefCount += 1;
	m_nDefaultViewPortWidth = 0;
	m_nDefaultViewPortHeight = 0;
	m_nDirLightCount = 0;
}

CDnRenderAvatarNew::~CDnRenderAvatarNew()
{
	ClearRenderObject();
	SAFE_RELEASE_SPTR( m_hActor );
	SAFE_RELEASE_SPTR( m_hEtc );

	// ��κ��� �����ؼ� �Ἥ �߰��� ������ ���� ��� ����ִ� ���·� ����. ���� �߰������� �ʿ��ϸ� �ٸ������� �ٲ���Ѵ�.
	s_nTempTargetRefCount -= 1;
	if( s_nTempTargetRefCount == 0 )
	{
		for( int i = 0; i < (int)s_vecTempTarget.size(); ++i )
		{
			SAFE_RELEASE_SPTR( s_vecTempTarget[i].hTexture );
		}
		s_vecTempTarget.clear();
		FinalizeStaticObject();
	}
}

void CDnRenderAvatarNew::InitializeStaticObject()
{
	if( s_bInitializeStaticObject ) return;

	// ���� ���忡 ����Ʈ�� ������ ����� �ӽ� ����Ʈ��ü�� �̸� ����.
	CEtLight *pLight[2];
	SLightInfo sDefaultLightInfo;
	for( int i = 0; i < 2; ++i )
	{
		pLight[i] = new CEtLight();
		pLight[i]->SetLightInfo( &sDefaultLightInfo );
		s_hTempLight[i] = pLight[i]->GetMySmartPtr();
	}

	// �����̳ʺ��� �˷��� Env���� �ε�.
	CEnvInfo EnvInfo;
	bool bResult = EnvInfo.Load( CEtResourceMng::GetInstance().GetFullName( "CashShop01.env" ).c_str() );
	if( bResult )
	{
		// Color Filter
		s_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		s_pColorAdjFilter->SetParam( *EnvInfo.GetColorAdjShadow(), *EnvInfo.GetColorAdjMidtones(), *EnvInfo.GetColorAdjHighlights(), EnvInfo.GetColorAdjSaturation() );
		s_pColorAdjFilter->Enable( false );

		// Environment Texture Setting
		if( EnvInfo.GetEnvironmentTexture() && strlen(EnvInfo.GetEnvironmentTexture()) > 0 )
			s_hTempEnviTexture = EternityEngine::LoadTexture( EnvInfo.GetEnvironmentTexture() );

		// Ambient
		EtColor Ambient = EnvInfo.GetAmbient();
		s_TempAmbient = EtColor( Ambient.b, Ambient.g, Ambient.r, 1.0f );

		// Light
		CEnvInfo::LightInfoStruct *pStruct;
		int nDirLightCount = EnvInfo.GetLightCount();
		nDirLightCount = min( nDirLightCount, 2 );
		for( int i = 0; i < nDirLightCount; ++i )
		{
			pStruct = EnvInfo.GetLight(i);
			pStruct->pInfo->Diffuse.r *= pStruct->pInfo->Diffuse.a;
			pStruct->pInfo->Diffuse.g *= pStruct->pInfo->Diffuse.a;
			pStruct->pInfo->Diffuse.b *= pStruct->pInfo->Diffuse.a;
			s_TempLightDiffuse[i] = pStruct->pInfo->Diffuse;
			s_TempLightDir[i] = pStruct->pInfo->Direction;
		}
		EnvInfo.Reset();
	}
	else
	{
		// Color Filter
		s_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		s_pColorAdjFilter->SetParam( EtVector4(-0.071f, -0.107f, -0.071f, 0.0f), EtVector4(0.0f, 0.0f, 0.0f, 0.0f), EtVector4(0.821f, 0.393f, 0.464f, 0.0f), 0.9f );
		s_pColorAdjFilter->Enable( false );

		// Ambient
		s_TempAmbient = EtColor( 0.568f, 0.752f, 0.850f, 1.0f );

		// Light
		for( int i = 0; i < 2; ++i )
		{
			s_TempLightDiffuse[i] = ( i == 0 ? EtColor( 1.164f, 1.116f, 0.828f, 1.2f ) : EtColor( 0.28f, 0.44f, 0.81f, 1.0f ) );
			s_TempLightDir[i] = ( i == 0 ? EtVector3( -0.143f, -0.806f, -0.573f ) : EtVector3( -0.607f, -0.716f, 0.343f ) );
		}
	}

	// ȯ����� ���� ���صΰ� ���� �ȵȴٰ� �ڲ� �׷��� �̷��� ������ üũ�ؼ� �ִ´�.
	if( !s_hTempEnviTexture )
		s_hTempEnviTexture = EternityEngine::LoadTexture( "Environment_Texture06.dds" );

	s_bInitializeStaticObject = true;
}

void CDnRenderAvatarNew::FinalizeStaticObject()
{
	if( !s_bInitializeStaticObject ) return;

	SAFE_DELETE( s_pColorAdjFilter );
	SAFE_RELEASE_SPTR( s_hTempLight[0] );
	SAFE_RELEASE_SPTR( s_hTempLight[1] );
	SAFE_RELEASE_SPTR( s_hTempEnviTexture );

	s_bInitializeStaticObject = false;
}

void CDnRenderAvatarNew::Initialize( int nWidth, int nHeight, SCameraInfo &CameraInfo, int nViewPortWidth, int nViewPortHeight, EtFormat Format, bool bUseColorAdjFilter, bool bUseBorderAlpha )
{
	CEtRTTRenderer::Initialize( CameraInfo, nWidth, nHeight, nViewPortWidth, nViewPortHeight, Format );

	// ���� Initialize �ɶ� ó���ϴ°ŷ� �Ѵ�.
	InitializeStaticObject();

	m_bUseColorAdjFilter = bUseColorAdjFilter;
	m_bUseBorderAlpha = bUseBorderAlpha;

	// ����Ʈ ī�޶� ����. z���� ������ �߸����� �ʰ� �ʹ� ���������� ������ �Ǳ⶧���� ������ ����ϰڴ�.
	m_hCamera->LookAt( EtVector3(0.0f, m_fDefaultCameraYPos, m_fDefaultCameraZPos), EtVector3(0.0f, m_fDefaultCameraYPos, 0.0f), EtVector3(0.0f, 1.0f, 0.0f) );
	m_hCamera->CalcInvViewMat();

	m_nDefaultViewPortWidth = nViewPortWidth;
	m_nDefaultViewPortHeight = nViewPortHeight;
}

void CDnRenderAvatarNew::CameraLookAt( EtVector3 &Eye, EtVector3 &At, EtVector3 &Up )
{
	m_hCamera->LookAt( Eye, At, Up );
}

bool CDnRenderAvatarNew::SetActor( DnActorHandle hPlayer, bool bShowWeapon, bool bForceCashWeapon )
{
	// �������½�ũ OnLoadRecvCharUserEnteredCallback ����,.(�켱 ���� ����. ���캸��� ĳ�ü� ���ź��� �Ѵ�.)
	DnActorHandle hActor = CreateActor( hPlayer->GetClassID(), false, false, false );
	if( !hActor ) return false;
	hActor->Show( false );
	//hActor->SetSoftAppear( true );

	hActor->Initialize();

	// AttachItem���� ���� ����.
	m_hActor = hActor;
	CheckType();

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pPlayer = (CDnPlayerActor *)hPlayer.GetPointer();
		CDnPlayerActor *pActor = (CDnPlayerActor *)hActor.GetPointer();

		// Note: �������� ������ �� �þ߿� ������ �� ���⼭ �� �༮�� ���� �����丮�� ���õǴ� �� �ƴϰ� ���� �ֱ� ������ ���õ˴ϴ�.
		// ���Ŀ� �ʿ��ϴٸ� �����丮�� �����ؾ��մϴ�..
		pActor->SetWeaponViewOrder( 0, ( bForceCashWeapon ) ? true : pPlayer->IsViewWeaponOrder(0) );
		pActor->SetWeaponViewOrder( 1, ( bForceCashWeapon ) ? true : pPlayer->IsViewWeaponOrder(1) );
		for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
			pActor->SetPartsViewOrder( i, ( bForceCashWeapon ) ? true : pPlayer->IsViewPartsOrder(i) );
		pActor->SetHideHelmet( ( bForceCashWeapon ) ? false : pPlayer->IsHideHelmet() );

		int nDefaultPartsInfo[4];
		for( int i = 0; i < 4; i++ ) {
			nDefaultPartsInfo[ i ] = pPlayer->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)(CDnParts::Body + i) );
		}
		pActor->SetDefaultPartsInfo( nDefaultPartsInfo );
		pActor->SetPartsColor( MAPartsBody::HairColor, pPlayer->GetPartsColor(MAPartsBody::HairColor) );
		pActor->SetPartsColor( MAPartsBody::EyeColor, pPlayer->GetPartsColor(MAPartsBody::EyeColor) );
		pActor->SetPartsColor( MAPartsBody::SkinColor, pPlayer->GetPartsColor(MAPartsBody::SkinColor) );

		// Normal_Stand ������ �Ҽ������� �����ɶ� ������ å�� �����ִ� ä�� ��ڿ� ��������, BattleMode-false�صΰ� AttachWeapon�� ȣ���ؾ��Ѵ�.
		// �̷��� �ؾ� DnWeapon::ResetDefaultAction ȣ��Ǹ鼭,
		// CDnPlayerActor::OnResetAttachWeaponAction �� ȣ���ϴµ�,
		// �̶� å �׼��� Normal_Stand�� �����ϰ� ���ش�.
		// �� ���� Attach�ϱ����� BattleMode���� �����صδ°� ����.
		pActor->SetBattleMode( false );

		// �÷��̾��� ��쿣 ���� RTTRenderMode ������ �ϵ��� �Ѵ�.(Ż���̳� ���� ���� �� �ʿ� ����.)
		// �̷��� �ؾ� Į�� ���� Attach�Ҷ� �޼չ��⸦ RenderTarget������ ������ �� �ִ�.
		pActor->SetRTTRenderMode( true, this );

		// ����� ���� ĳ�ü��� ���������� �ƴ϶�� ����Ʈ���� �θ��� ���¶�
		// ��޿� ���� ������ȭ ����� �븻���� ����� ����� ������ �ʴ´�.
		for( int i = 0; i < CDnParts::PartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			if( (CDnParts::PartsTypeEnum)i == CDnParts::Helmet && pActor->IsHideHelmet() )
				continue;
			AttachItem( hParts->GetClassID(), 0, 0, hParts->GetLookItemID() );
		}

		if( bShowWeapon )
		{
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pPlayer->GetWeapon( i );
				if( !hWeapon ) continue;

				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel() );
			}
		}

		for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			if( i < CDnParts::SwapParts_Amount && pActor->IsViewPartsOrder(i) == false )
				continue;

			if( (CDnParts::PartsTypeEnum)i == CDnParts::CashHelmet && pActor->IsHideHelmet() )
				continue;

			AttachItem( hParts->GetClassID(), 0, ( (i == CDnParts::CashRing2) ? 1 : 0 ), hParts->GetLookItemID() );
		}

		if( bShowWeapon )
		{
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pPlayer->GetCashWeapon( i );
				if( !hWeapon ) continue;

				if( pActor->IsViewWeaponOrder(i) == false )
					continue;

				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel(), 0, hWeapon->GetLookItemID() );
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// �α��� ĳ���� ����ȭ��� ���� CombineParts �� ���ϱ⶧���� ������ ��Ʋ�������� ������ �尩�޽� ���̵� �ϴ� ���� ������ ȣ���Ѵ�.
		pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		//EtVector2 vDir( sin( EtToRadian( pPacket->fRotate ) ), cos( EtToRadian( pPacket->fRotate ) ) );
		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetActionQueue( "Normal_Stand" );
		pActor->SetGuildSelfView( pPlayer->GetGuildSelfView() );

		//pActor->CombineParts();		// �����ƹ�Ÿ���� �Ĺ��� ���� �ʴ´�.
		pActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
		hActor->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,

		bool bShowFieldActor = false;
		if( bShowFieldActor )
		{
			// ���μ����� ������, �������� ó��(�浹���� ����ó��, ������ �Ǵ�)���� �ϱ⶧����, 0, 0, 0 �� �������� �; ������ġ ���� �ö���� �ȴ�.
			// �ƹ����� ���μ��� �ȵ����� ���� �����ϴ°� ������.

			pActor->SetPosition( *hPlayer->GetPosition() );	// �ʵ忡 ����� Ȯ���غ��� ���� ��ġ ����.
			pActor->SetHP( 100 );	// ���μ��� �ȵ����� ProcessDie �� �Ѿ�� �ʱ� ������,
			pActor->SetProcess( true );	// ���� ���μ��� �����ʿ� ������(�ٵ� ��ƼŬ ���̰� �Ϸ���... ������ ���� �ʳ�?, ��¥ ���μ����� �ϴ��� ������ ��� ��������?)
		}

		MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
		pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
		AddObject( pActorPartsBody->GetObjectHandle() );
		//AddObject( pActorPartsBody->GetCombineObject() );	// �Ĺ��ο�����Ʈ�� ����(�Ӹ�)�� ���Ⱑ ������.
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.

#ifdef PRE_ADD_CASH_COSTUME_AURA
		pActor->ComputeRTTModeCostumeAura(hPlayer);
#endif
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{

		CDnVehicleActor *pVehicleActor = (CDnVehicleActor *)hPlayer.GetPointer();
		CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();

		// �÷��̾��� ��쿣 ������ �̷�����־ �������� ����������, ���� ��쿣 ��ü���� �� �ؾ��Ѵ�.
		if( pActor->GetObjectHandle() ) pActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		// ������ ���ͼ� ����
		for( int i = Vehicle::Slot::Saddle; i < Vehicle::Slot::Max; ++i )
		{
			if( !pVehicleActor->GetMyPlayerActor() || pVehicleActor->GetMyPlayerActor()->IsDestroy() ) continue;
			if( !pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i].nItemID ) continue;
			pActor->EquipItem( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i] );

			DnSimplePartsHandle hParts = pActor->GetVehicleParts( (Vehicle::Parts::eVehicleParts)(i - Vehicle::Slot::Saddle) );
			if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

			if( i == Vehicle::Slot::Hair )	// ������ ��쿣 ������ ���ĺ���, ���н� �Ҵ�.
			{
				if( hParts && hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
				{
					hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
					hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
				}
			}
		}
		pActor->ChangeHairColor( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().dwPartsColor1 );

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		// �����ƹ�Ÿ���� ���Ǵ� ���ʹ� ���μ�������Ʈ�� ���Ե��� �ʰ� ������ �����⶧����, CDnActor::ProcessClass������ ���ܵȴ�.
		// ProcessClass���� MAActorRenderBase::Process�Լ��� ȣ��Ǵµ� �̰͵� ���ܵǴ� ��.
		// �̰� ������ �ִ� ������ ���Ҷ� ����� LocalTime�� ����ϴµ�, �� LocalTime�� �ʱⰪ ���¶�,
		// �ִϸ��̼� ������ �ѹ� �̻� �������� ����� �ִϸ��̼� ������ �ǰ� �ȴ�.
		// �׷��� �ִ� ����ϴ� �����ٰ� �Ʒ�ó�� ������ ����Ÿ���� �ֵ��� �ϰڴ�.
		// �÷��̾��� ��쿣 Normal_Stand�� ���� Ƽ���� �ʾҴ����� �ᱹ�� ���� ������ ������ �־���.
		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
			pActor->SetMyPlayerActor(CDnActor::s_hLocalActor);
		}

		pActor->SetRTTRenderMode( true, this );
		pActor->SetActionQueue( "Stand" );
		pActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
		hActor->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,

		AddObject( pActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.

	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor* pPlayerPetActor = (CDnPetActor *)hPlayer.GetPointer();
		CDnPetActor* pPetActor = (CDnPetActor *)hActor.GetPointer();

		pPetActor->SetDefaultMesh();
		if( pPetActor->GetObjectHandle() ) pPetActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		for( int i=Pet::Slot::Accessory1; i<Pet::Slot::Max; ++i )
		{
			if( !pPlayerPetActor->GetPetInfo().Vehicle[i].nItemID ) continue;
			pPetActor->EquipItem( pPlayerPetActor->GetPetInfo().Vehicle[i] );

			DnSimplePartsHandle hParts = pPetActor->GetPetParts( (Vehicle::Parts::eVehicleParts)(i - Pet::Slot::Accessory1) );
			if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
		}

		pPetActor->ChangeColor( ePetParts::PET_PARTS_BODY, pPlayerPetActor->GetPetInfo().dwPartsColor1 );
		pPetActor->ChangeColor( ePetParts::PET_PARTS_NOSE, pPlayerPetActor->GetPetInfo().dwPartsColor2 );

		EtVector2 vDir( 0.0f, 1.0f );
		pPetActor->Look( vDir );

		LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
		EtMatrix WorldMat = *pPetActor->GetMatEx();
		pPetActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		pPetActor->SetMyMaster( dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer()) );

		pPetActor->SetRTTRenderMode( true, this );
		pPetActor->SetActionQueue( "Stand" );
		pPetActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
		hActor->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,
		AddObject( pPetActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.
	}

	// �� ������ ��ũ�� ó���κп��� ������ ��ġ�� ���ؿ� ��ũ�� �ڽĵ��� ��� ����� �ϴµ�,
	// ���μ����� ������ �ʱ� ������ �̷��� ������ �ʱ���ġ�� �����س��ƾ��Ѵ�.
	if( hActor->GetObjectHandle() )
	{
		EtMatrix Mat;
		EtMatrixIdentity( &Mat );
		hActor->GetObjectHandle()->SetWorldMat( Mat );
	}

	return true;
}

bool CDnRenderAvatarNew::SetActor( int nClassID, bool bShowWeapon )
{
	DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );

	// �������½�ũ OnLoadRecvCharUserEnteredCallback ����,.(�켱 ���� ����. ���캸��� ĳ�ü� ���ź��� �Ѵ�.)
	DnActorHandle hActor = CreateActor( nClassID, false, false, false );
	if( !hActor ) return false;
	hActor->Show( false );
	//hActor->SetSoftAppear( true );

	hActor->Initialize();

	// DefaultCreate���̺� �ִ� �� ������ �ε��ϸ鼭 Attach�ؾ��ϹǷ�, m_hActor�� ���� �� �ٷ� �־�д�.
	m_hActor = hActor;
	CheckType();

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		// �׳� hActor->Initialize���� �صθ� �÷��̾� ĳ������ ��� �Ӹ� ����ä�� ������ ����� �Ⱥپ ���´�.
		// �׷��� ����Ʈ���� �о ���� ����������Ѵ�.
		CDnPlayerActor *pActor = (CDnPlayerActor *)hActor.GetPointer();

		pActor->SetWeaponViewOrder( 0, true );
		pActor->SetWeaponViewOrder( 1, true );
		for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
			pActor->SetPartsViewOrder( i, true );
		pActor->SetHideHelmet( false );

		char *szLabels[CDnParts::PartsTypeEnum_Amount] = { "_Face", "_Hair", "_Helmet", "_Body", "_Leg", "_Hand", "_Foot", NULL, NULL, NULL, NULL };
		char szBuffer[128] = { 0, };
		DNTableFileFormat*  pDefaultCreateTable = GetDNTable( CDnTableDB::TDEFAULTCREATE );

		int nDefaultPartsInfo[4];
		nDefaultPartsInfo[ CDnParts::Body-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultBody" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Leg-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultLeg" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Hand-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultHand" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Foot-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultFoot" )->GetInteger();
		pActor->SetDefaultPartsInfo( nDefaultPartsInfo );

		// �켱 ����Ʈ ���� �߿� �� �տ� �͸� �����ֵ��� �Ѵ�.
		float fIntensity = 1.1f;
		DWORD dwColorR10G10B10 = 0;
		D3DCOLOR dwHairColor = D3DCOLOR_XRGB(
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1R" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1G" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1B" )->GetInteger() );
		CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, dwHairColor, fIntensity );
		pActor->SetPartsColor( MAPartsBody::HairColor, dwColorR10G10B10 );

		fIntensity = 1.0f;
		D3DCOLOR dwEyeColor = D3DCOLOR_XRGB(
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1R" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1G" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1B" )->GetInteger() );
		CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, dwEyeColor, fIntensity );
		pActor->SetPartsColor( MAPartsBody::EyeColor, dwColorR10G10B10 );

		int nA = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1A" )->GetFloat() * 255);
		int nR = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1R" )->GetFloat() * 255);
		int nG = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1G" )->GetFloat() * 255);
		int nB = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1B" )->GetFloat() * 255);
		DWORD dwSkinColor = D3DCOLOR_ARGB( nA, nR, nG, nB );
		pActor->SetPartsColor( MAPartsBody::SkinColor, dwSkinColor );

		pActor->SetBattleMode( false );
		pActor->SetRTTRenderMode( true, this );

		for( int nIndex = 0; nIndex < CDnParts::PartsTypeEnum_Amount; ++nIndex )
		{
			if( NULL == szLabels[nIndex] )
				continue;

			sprintf_s( szBuffer, "%s%d", szLabels[nIndex], 1 );
			int nPartItemID = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, szBuffer )->GetInteger();
			AttachItem( nPartItemID );
		}

		if( bShowWeapon )
		{
			for( int nIndex = 0; nIndex < CDnWeapon::EquipSlot_Amount; ++nIndex )
			{
				sprintf_s( szBuffer, "_Weapon%d", nIndex+1 );
				int nWeaponItemID = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, szBuffer )->GetInteger();
				AttachItem( nWeaponItemID );
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// �α��� ĳ���� ����ȭ��� ���� CombineParts �� ���ϱ⶧���� ������ ��Ʋ�������� ������ �尩�޽� ���̵� �ϴ� ���� ������ ȣ���Ѵ�.
		pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetActionQueue( "Normal_Stand" );

		//pActor->CombineParts();		// �����ƹ�Ÿ���� �Ĺ��� ���� �ʴ´�.
		pActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
		hActor->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,

		MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
		pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
		AddObject( pActorPartsBody->GetObjectHandle() );
		//AddObject( pActorPartsBody->GetCombineObject() );	// �Ĺ��ο�����Ʈ�� ����(�Ӹ�)�� ���Ⱑ ������.
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{

		CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();
		if( pActor->GetObjectHandle() ) pActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetRTTRenderMode(true,this);
		pActor->SetActionQueue( "Stand" );
		pActor->SetHP( 1 );		// ��� �ڵ忡�� HP 0�����ΰɷ� ���� �Ǵ��ϴ� ���� �־ �н����� ����� ������ 1 �־��ش�.
		hActor->Show( true );	// ����Ÿ�ٿ� �׸���� �ؾ��ϱ� ������,

		AddObject( pActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.

	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor *pPetActor = (CDnPetActor *)hActor.GetPointer();
		pPetActor->SetDefaultMesh();
		if( pPetActor->GetObjectHandle() ) pPetActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		EtVector2 vDir( 0.0f, 1.0f );
		pPetActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pPetActor->GetMatEx();
			pPetActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pPetActor->SetRTTRenderMode( true, this );
		pPetActor->SetActionQueue( "Stand" );
		hActor->Show( true );

		AddObject( pPetActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1�� �����صθ� ������Ʈ Ǯ�������� ��� �������Ѵ�.
	}

	// �� ������ ��ũ�� ó���κп��� ������ ��ġ�� ���ؿ� ��ũ�� �ڽĵ��� ��� ����� �ϴµ�,
	// ���μ����� ������ �ʱ� ������ �̷��� ������ �ʱ���ġ�� �����س��ƾ��Ѵ�.
	if( hActor->GetObjectHandle() )
	{
		EtMatrix Mat;
		EtMatrixIdentity( &Mat );
		hActor->GetObjectHandle()->SetWorldMat( Mat );
	}

	return true;
}

void CDnRenderAvatarNew::CheckType()
{
	m_eActorType = ActorEnum::None;

	if( m_hActor )
		CheckActorType();
	else if( m_hEtc )
		CheckEtcType();
}

void CDnRenderAvatarNew::CheckActorType()
{
	CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor *>(m_hActor.GetPointer());

	CDnVehicleActor *pVehicleActor = dynamic_cast<CDnVehicleActor *>(m_hActor.GetPointer());
	CDnPetActor *pPetActor = dynamic_cast<CDnPetActor *>(m_hActor.GetPointer());

	if( pPlayerActor && !pVehicleActor )
		m_eActorType = ActorEnum::PlayerActor;
	else if( !pPlayerActor && pVehicleActor )
		m_eActorType = ActorEnum::VehicleActor;
	else if( !pPlayerActor && pPetActor )
		m_eActorType = ActorEnum::PetActor;
}

void CDnRenderAvatarNew::CheckEtcType()
{
	m_eActorType = ActorEnum::EtcObject;
}

bool CDnRenderAvatarNew::ResetActor( bool bInitCamera )
{
	if( m_hActor )
	{
		m_vecPartsInfo.clear();
		ClearRenderObject();
		//m_hActor->SetDestroy();	// SetProcess�� �ѳ��� Destroyüũ �� ProcessClass���� ����� ��ƾ�� ����ȴ�. SetProcess �ȵ������� �������� �ʴ´�.
		SAFE_RELEASE_SPTR( m_hActor );	// SetProcess�� �ѳ��� �����Ƿ� �̷��� ���� �����ؾ��Ѵ�.
		m_eActorType = ActorEnum::None;
		SetRenderFrameCount( 0 );	// -1�� �����ص״��� �ٽ� 0���� �ؼ� RTT�����Ҷ� ������ �ʰ� �Ѵ�.
	}
	if( bInitCamera ) InitCameraControl();
	return true;
}

int CDnRenderAvatarNew::GetActorID()
{
	if( m_hActor )
		return m_hActor->GetClassID();
	return 0;
}

bool CDnRenderAvatarNew::AttachItem( int nItemID, int nEnchantLevel, int nSubArg1, int nLookItemID )
{
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)m_hActor.GetPointer();
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return false;
		if( !pSox->IsExistItem( nItemID ) ) return false;

#ifdef PRE_ADD_CASH_COSTUME_AURA
		pActor->SetAuraRenderType(RT_TARGET);
#endif

		eItemTypeEnum ItemType = CDnItem::GetItemType( nItemID );
		if( ItemType == ITEMTYPE_HAIRDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::HairColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_EYEDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::EyeColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_SKINDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::SkinColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_PARTS )
		{
			DnPartsHandle hParts = CDnParts::CreateParts( nItemID, 0, 0, 0, 0, 0, false, nLookItemID);
			if( hParts )
			{
				hParts->SetRTTRenderMode( true, this );
				bool bCash = CDnItem::IsCashItem( nItemID );

				// ���͸� �״�� ���°ű� ������ ���ٸ� ó�� ���ص� �� �� �˾Ҵµ�,
				// �ӵ������� Combine���� �ʰ� ó���ؾ��ϴٺ���, �ѹ��� �����ؼ� ������ �־���.
				// Combine�Ҷ��� �ѹ��� ó���� ��� �� �Ǿ�������, Combine���� �������� �ѹ��� ó���� �ȵ��ִ� ��.
				// �̰� Combine���� �ʴ� �α��� ĳ���� ����ȭ�鿡���� �������� ������.
				// ��� �α��� ȭ�鿡���� �Ϲ����� ĳ�ÿʵ� �� ������ �� �ѹ����� �����ٰų�,(�� �ݴ��� ���. �ѹ��� ���� �� �Ϲ����� ĳ������ ����)
				// ���� �ʱ⶧���� �� ���� ������ ���̴�.
				//
				// ���� Combine���� �ʱ⶧���� ���ο� ������ �����Ҷ� ���� ������ ���� ����ص��� �ʰ� ����µ�,
				// �̷� ���� �ѹ��������� ���¿��� �ٸ� ĳ�û��������� AttachItem�Ҷ� ������ �Դ� ����, �尩, ������ ������ ���ٴ� ���̴�.
				//
				// �̰� �����Ϸ���, ĳ���� ����� ������ ����ִ� ��ó�� �븻����, ĳ������ �� �����ؼ� ����ص־��ϴµ�,
				// �̰� �������½�ũ�� ������� �� ������ ���� �ǹ�����.
				// �ִ��� ��������ƾ�����θ� �ǵ帮���ߴ� �����ƹ�Ÿ�ڵ��,(�׷��� AttachItem�� �����ص� DetachItem�� �Ϻη� ���Ŵ�.)
				// �� ����� �������� �ʱ�� �Ѵ�.
				//
				// �׷��� �ִ��� �ڿ������� ǥ���ϱ� ���� �ѹ��ʿ� ���ؼ��� ����ó���ؼ� ���� ����ֵ��� �ϰڴ�.

				if( !bCash )
				{
					// �����ƹ�Ÿ������ CombineParts �� ���ϱ⶧���� �븻�� ���� ��� ��Ī�Ǵ� ĳ������ �˻���
					// ĳ������ �ִٸ� �������� �ʰ� �ѱ��.
					bool bEquip = true;
					if( hParts->GetPartsType() >= CDnParts::Helmet && hParts->GetPartsType() <= CDnParts::Foot )
					{
						if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() - 2 ) ) )
						{
							bEquip = false;
						}
					}

					// Ȥ�� ������ �ѹ��� �������� Ȯ��.(���ϱ� �ѹ��� ������������ �������� �����µ�?)
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( m_vecPartsInfo[i].nType == hParts->GetPartsType() )
						{
							bEquip = false;
							break;
						}
					}

					if( bEquip )
					{
						pActor->DetachParts( hParts->GetPartsType() );
						pActor->AttachParts( hParts, hParts->GetPartsType(), true, false );
						if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

						if( hParts->GetPartsType() == CDnParts::Hair )
						{
							// �Ӹ�ī���� ��쿡 ������ ���ĺ����� ���� �ȵ��������� �ִ�. �׳� ������ �����Ѵ�.
							if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
								hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						}
					}
					else
					{
						SAFE_RELEASE_SPTR( hParts );
					}
				}
				else
				{
					// ���� �ѹ��� �԰� �־��� ���̶��,
					if( m_vecPartsInfo.size() )
					{
						// ���� �����ϴ� ������ �ѹ��ʿ� �ش�Ǵ� �������� Ȯ���Ѵ�.
						// ������ ��� �ѹ����� �� ���� ���������� ������ ������, ���� �̰� �޶����ٸ�,
						// hParts->GetPartsType()�� �˻��ϴ°� �ƴ϶�, ������������ �� �����ͼ� �˻��ؾ��ҰŴ�.
						bool bDetachOnepiece = false;
						for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
						{
							if( (m_vecPartsInfo[i].nType - 2) == hParts->GetPartsType() )
							{
								bDetachOnepiece = true;
								break;
							}
						}

						if( bDetachOnepiece )
						{
							std::vector<SPartsInfo> vecPartsInfo;
							vecPartsInfo = m_vecPartsInfo;
							m_vecPartsInfo.clear();

							// ���� �ѹ����� �����ϰ�,(ù��° �ε����� �ѹ��� ���� ������.)
							pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( vecPartsInfo[0].nType - 2 ) );

							// ���� ������ �� ���� ��
							// ���� Ÿ���� �� �־������ �ش��ϴ� Ÿ�Կ� �´� ������ID�� ������ �������ϸ� �ȴ�.
							for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
								AttachItem( vecPartsInfo[i].nItemID );

							// �Ϲ����� ĳ���� �����ϵ��� �Ʒ� ��ƾ �� ����.
						}
					}

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					bool bTransparentSkin = CDnParts::IsTransparentSkin( hParts->GetClassID(), pActor->GetClassID() );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

					// �������� ���� �ѹ����̶��, �ѹ��ʿ� �ش�Ǵ� �ڸ��� �������� ������ ����صд�.
					// ���������� �ϸ� �ڽŽ����� ������ �ڽŵ� ���� �߰��صд�.
					// (�̰� ����ص־� �ѹ��� �԰� �ִٰ� ĳ���尩 ������ ������ �Դ� ���Ƿ� �ǵ��� �� �ִ�.)
					if( hParts->IsExistSubParts() )
					{
						SPartsInfo Info;
						Info.nType = (int)hParts->GetPartsType() + 2;	// �ѹ����� ĳ�����̴� +2�ؼ� �븻�� Ÿ������ �����.
						Info.nItemID = 0;
						m_vecPartsInfo.push_back( Info );
						for( int i = 0; i < hParts->GetSubPartsCount(); ++i )
						{
							Info.nType = (int)hParts->GetSubPartsIndex(i) + 2;	// ����Ÿ�� �����͵� ĳ������ Ÿ���̴� +2�ؼ� �븻�� Ÿ������ �����.
							m_vecPartsInfo.push_back( Info );
						}

						// ���� Ÿ���� �� �־������ �ش��ϴ� Ÿ�Կ� �´� ������ID�� ������ �������ϸ� �ȴ�.
						for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
						{
							if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) ) )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) )->GetClassID();
								pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
								// �����ѹ������� �������Ҷ��� �Ϲ���������� ��������Ѵ�.
								if( bTransparentSkin )
								{
									DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
									if( hNormalParts && !hNormalParts->GetObjectHandle() )
									{
										DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
	#ifdef PRE_FIX_MEMOPT_EXT
										std::string szAniName;
										CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
	#else
										std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
	#endif
										hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
									}
								}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
							}
							else if( pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) ) )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->GetClassID();
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
								if( !bTransparentSkin ) pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->FreeObject();
#else
								pActor->DetachParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
							}
							// ���� ����� �����ʿ��� ����ġó�� �Ǿ��ٸ� �� �����̴��� ����Ʈ���� �� ������� ���̴�.
							// �׷��� Ȥ�� �𸣴� �Ʒ� ó�� �صд�.
							else if( m_vecPartsInfo[i].nType >= CDnParts::DefaultPartsType_Min && m_vecPartsInfo[i].nType <= CDnParts::DefaultPartsType_Max )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)m_vecPartsInfo[i].nType );
							}
						}
					}

					// �����ƹ�Ÿ������ CombineParts �� ���ϱ⶧���� ���⼭ �븻���� ���ִ½����� �մϴ�.
					if( hParts->GetPartsType() >= CDnParts::CashHelmet && hParts->GetPartsType() <= CDnParts::CashFoot )
					{
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
						// ���� �ѹ��ʶ����� Detach���� �ʰ� FreeObject�ϴ°ɷ� �ٲ۴�. �׷��� ���߿� �ʿ��Ҷ� �ٽ� CreateObject�� �� �ִ�.
						DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
						if( hNormalParts && !bTransparentSkin ) hNormalParts->FreeObject();
#else
						if( pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) ) )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
					}

					if( nSubArg1 == 0 )
					{
						pActor->DetachCashParts( hParts->GetPartsType() );
						pActor->AttachCashParts( hParts, hParts->GetPartsType(), true, false );
					}
					else if( nSubArg1 == 1 )
					{
						if( hParts->GetPartsType() == CDnParts::CashRing )
						{
							pActor->DetachCashParts( CDnParts::CashRing2 );
							pActor->AttachCashParts( hParts, CDnParts::CashRing2, true, false );
						}
					}
					if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					else if( bTransparentSkin )
					{
						DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
						if( hNormalParts && !hNormalParts->GetObjectHandle() )
						{
							DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
	#ifdef PRE_FIX_MEMOPT_EXT
							std::string szAniName;
							CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
	#else
							std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
	#endif
							hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
						}
					}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

					// �켱 ������ ���ؼ��� �Ѵ�. ������ �̷� ��� ���� ���� ��.
					if( hParts->GetPartsType() == CDnParts::CashHelmet )
					{
						// �Ӹ���� �� �Ϻ� �����鿣 �޽ó��κп� �����ؽ�ó�� ����ϴµ��� ���ĺ����� ������ ���� �ȵ��ִ� ��찡 �ִ�.
						// �̷��� �ص� ����� �������� ������ Ȱ��ȭ��Ų��.
						// ������ ���ϱ�, PostProcess���� ���İ� �״�� �����ϵ��� ó���ϰ���,
						// �׵θ� ���� �̻��ϰ� ���̴� ������ ���� �ذ�Ȱ� ����.
						// �׷��� ������ ���ĺ��� �Ѵ°� �����ϵ��� �ϰڴ�.
						//if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
						//	hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
					}
					else if( hParts->GetPartsType() == CDnParts::CashWing )
					{
						// ������ ��쿣 ������ ���ĺ���, ���н� �Ѽ� ����ϰ� ������ �Ѵ�.
						if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
						{
							hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
							hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
						}
					}
				}
			}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
			// �α��� ĳ���� ����ȭ��� ���� CombineParts �� ���ϱ⶧���� ������ ��Ʋ�������� ������ �尩�޽� ���̵� �ϴ� ���� ������ ȣ���Ѵ�.
			pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
		}
		else if( ItemType == ITEMTYPE_WEAPON )
		{
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nItemID, 0, 0, nEnchantLevel, 0, 0, false, false, true, ITEMCLSID_NONE );
			if( hWeapon )
			{
				hWeapon->SetRTTRenderMode( true, this );
				bool bCash = CDnItem::IsCashItem( nItemID );

				int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( hWeapon->GetEquipType() ) - EQUIP_WEAPON1;
				ASSERT( nEquipIndex == 0 || nEquipIndex == 1 );

				if( !bCash )
				{
					pActor->DetachWeapon( nEquipIndex );
					pActor->AttachWeapon( hWeapon, nEquipIndex, true );
				}
				else
				{
					pActor->DetachCashWeapon( nEquipIndex );
					pActor->AttachCashWeapon( hWeapon, nEquipIndex, true, false );
				}

				if( hWeapon->GetObjectHandle() )
				{
					// �Ϲݸ��̽��� ĳ���������� ������ ��찡 ������.
					// ���̽��ִϸ� ����ؼ� ���̽� ��Ʈ���ϱ��� ���� �������ϴ� ��, ���ҽ��ܿ��� ������ �ʹ� �������Ƿ� ������ �ùķ��̼� �Ⱦ��°� ���� �ʴ´�.
					//hWeapon->GetObjectHandle()->DisableSimulation();
					hWeapon->GetObjectHandle()->SetRenderType( RT_TARGET );
				}
			}
		}
		// �󱼻�ǰ�̶� �Ӹ���� ��ǰ.
		else if( ItemType == ITEMTYPE_FACIAL || ItemType == ITEMTYPE_HAIRDRESS )
		{
			int nParam = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			AttachItem( nParam );

			// ���� ��ü���� �Ÿ��� ���� �������� �ʱ� ������, Hair�ٲܰ�� ĳ�ø��ڸ� �ٽ� �������� Hair���� �� �ڿ� �������ǰ� �Ѵ�.
			if( ItemType == ITEMTYPE_HAIRDRESS )
			{
				if( pActor->GetCashParts( CDnParts::CashHelmet ) )
				{
					DnPartsHandle hHelmet = pActor->GetCashParts( CDnParts::CashHelmet );
					if (!hHelmet)
					{
#ifdef _WORK
						std::wstring str = FormatW(L"DnRenderAvatar::AttachItem - Character CashHelmet does not exist : (%d)", nItemID);
						GetInterface().AddChatMessage(CHATTYPE_NOTICE, L"", str.c_str(), true);
#endif
						return false;
					}

					int nAttachHelmetId = (hHelmet->GetLookItemID() != ITEMCLSID_NONE) ? hHelmet->GetLookItemID() : hHelmet->GetClassID();
					pActor->DetachCashParts( CDnParts::CashHelmet );
					AttachItem(nAttachHelmetId);
				}
			}
		}
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{
		// ���� Ż���� ������ ���͸� �Ѱܹ޾� �����ϴ� SetActor�� ���Ǽ� ���� AttachItem�� �̱������´�.
	}

	return true;
}

bool CDnRenderAvatarNew::DetachItem( int nItemID )
{
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)m_hActor.GetPointer();
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return false;
		if( !pSox->IsExistItem( nItemID ) ) return false;

		eItemTypeEnum ItemType = CDnItem::GetItemType( nItemID );
		if( ItemType == ITEMTYPE_PARTS )
		{
			// ĳ���� �� ���¿��� ĳ���� �ٲٴ°� �̹� �� ó���Ǿ��ֱ⶧���� �Ű� �Ƚᵵ �ȴ�.
			// ĳ���� �� ���¿��� �Ϲ������� �ٲٱ� ���ؼ� ������ ĳ�������� ������ ����ġ ��Ű�� �͸� ������ �ȴ�.
			if( CDnItem::IsCashItem( nItemID ) )
			{
				CDnParts::PartsTypeEnum PartsType = CDnParts::GetPartsType( nItemID );
				pActor->DetachCashParts( PartsType );

				// �ѹ����԰��������� �߰��� ó���ؾ��Ѵ�.
				bool bDetachOnepiece = false;
				if( m_vecPartsInfo.size() )
				{
					// ���� �����ϴ� ������ �ѹ��ʿ� �ش�Ǵ� �������� Ȯ���Ѵ�.
					int nItemID = 0;
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( (m_vecPartsInfo[i].nType - 2) == PartsType )
						{
							bDetachOnepiece = true;
							break;
						}
					}

					if( bDetachOnepiece )
					{
						std::vector<SPartsInfo> vecPartsInfo;
						vecPartsInfo = m_vecPartsInfo;
						m_vecPartsInfo.clear();

						for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
							AttachItem( vecPartsInfo[i].nItemID );
					}
				}
			}
		}
		else if( ItemType == ITEMTYPE_WEAPON )
		{
			if( CDnItem::IsCashItem( nItemID ) )
			{
				int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( CDnWeapon::GetEquipType( nItemID ) ) - EQUIP_WEAPON1;
				ASSERT( nEquipIndex == 0 || nEquipIndex == 1 );
				pActor->DetachCashWeapon( nEquipIndex );
			}
		}
	}
	return true;
}

bool CDnRenderAvatarNew::RefreshEquip( DnActorHandle hActor )
{
	if( !hActor ) return true;
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{
		CDnVehicleActor *pVehicleActor = (CDnVehicleActor *)hActor.GetPointer();
		CDnVehicleActor *pActor = (CDnVehicleActor *)m_hActor.GetPointer();

		// ������ ���ͼ� ����
		for( int i = Vehicle::Slot::Saddle; i < Vehicle::Slot::Max; ++i )
		{
			if( !pVehicleActor->GetMyPlayerActor() || pVehicleActor->GetMyPlayerActor()->IsDestroy()) continue;
			if( !pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i].nItemID )
			{
				Vehicle::Parts::eVehicleParts ePartsType = Vehicle::Parts::Default;
				switch(i)
				{
				case Vehicle::Slot::Saddle:	ePartsType = Vehicle::Parts::Saddle; break;
				case Vehicle::Slot::Hair: ePartsType = Vehicle::Parts::Hair; break;
				}
				pActor->UnEquipItem( ePartsType );
			}
			else
			{
				pActor->EquipItem( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i] );

				DnSimplePartsHandle hParts = pActor->GetVehicleParts( (Vehicle::Parts::eVehicleParts)(i - Vehicle::Slot::Saddle) );
				if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

				if( i == Vehicle::Slot::Hair )
				{
					if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					{
						hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					}
				}
			}
		}
		pActor->ChangeHairColor( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().dwPartsColor1 );
	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor* pPetActor = (CDnPetActor *)hActor.GetPointer();
		CDnPetActor *pActor = (CDnPetActor *)m_hActor.GetPointer();

		for( int i = Pet::Slot::Accessory1; i < Pet::Slot::Max; ++i )
		{
			if( !pPetActor->GetPetInfo().Vehicle[i].nItemID )
			{
				Pet::Parts::ePetParts ePartsType = Pet::Parts::Default;
				switch( i )
				{
					case Pet::Slot::Accessory1: ePartsType = Pet::Parts::PetAccessory1; break;
					case Pet::Slot::Accessory2: ePartsType = Pet::Parts::PetAccessory2; break;
				}
				pActor->UnEquipItem( ePartsType );
			}
			else
			{
				pActor->EquipItem( pPetActor->GetPetInfo().Vehicle[i] );

				DnSimplePartsHandle hParts = pActor->GetPetParts( (Vehicle::Parts::eVehicleParts)(i - Pet::Slot::Accessory1) );
				if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
			}
		}

		pActor->ChangeColor( ePetParts::PET_PARTS_BODY, pPetActor->GetPetInfo().dwPartsColor1 );
		pActor->ChangeColor( ePetParts::PET_PARTS_NOSE, pPetActor->GetPetInfo().dwPartsColor2 );
	}

	return true;
}

void CDnRenderAvatarNew::InitCameraControl()
{
	m_nRotateYaw = 0;
	m_fSmoothRotateYaw = 0.0f;
	m_nZoomLevel = 0;
	m_fSmoothZoomLevel = 0.0f;
	m_fPanX = 0.0f;
	m_fPanY = 0.0f;
	m_fSmoothPanX = 0.0f;
	m_fSmoothPanY = 0.0f;
}

void CDnRenderAvatarNew::CalcUVCoord( float fUIWidth, float fUIHeight )
{
	int nWidth = int(fUIWidth * DEFAULT_UI_SCREEN_WIDTH);
	int nHeight = int(fUIHeight * DEFAULT_UI_SCREEN_HEIGHT);

	if( fUIWidth == 0.0f || fUIHeight == 0.0f )
		nWidth = nHeight = 0;

	if( nWidth == nHeight )
	{
		m_RTTUVCoord.fX = m_RTTUVCoord.fY = 0.0f;
		m_RTTUVCoord.fWidth = m_RTTUVCoord.fHeight = 1.0f;
	}
	else if( nWidth < nHeight )
	{
		m_RTTUVCoord.fY = 0.0f;
		m_RTTUVCoord.fHeight = 1.0f;
		m_RTTUVCoord.fWidth = fUIWidth / fUIHeight;
		m_RTTUVCoord.fX = (1.0f - m_RTTUVCoord.fWidth) / 2.0f;
	}
	else if( nWidth > nHeight )
	{
		m_RTTUVCoord.fX = 0.0f;
		m_RTTUVCoord.fWidth = 1.0f;
		m_RTTUVCoord.fHeight = fUIHeight / fUIWidth;
		m_RTTUVCoord.fY = (1.0f - m_RTTUVCoord.fHeight) / 2.0f;
	}
}

void CDnRenderAvatarNew::Process( float fElapsedTime )
{
	LOCAL_TIME LocalTime = 0;

	if( CDnActor::s_hLocalActor )
	{
		LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
	}
	else
	{
		CTask *pTask = CTaskManager::GetInstance().GetTask( "ItemTask" );
		if(pTask)
			LocalTime = pTask->GetLocalTime();
	}

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		if( !m_hActor )
			return;

		// -- �ʿ��� ���μ��� --
		// ���Ͱ� ��ӹ��� "ActionBase"�� ���μ��� �׼� (���� ���� ����ٰ� ���� ��ƼŬ �޾Ƽ� ����)
		// "ĳ�ù���" ���μ���, "�Ϲݹ���" ���μ���
		// "����" ���μ���
		CDnActor*		pActor  = (CDnActor*)m_hActor.GetPointer();
		CDnPlayerActor*	pPlayer = (CDnPlayerActor*)m_hActor.GetPointer();

		pActor->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );
#ifdef PRE_ADD_CASH_COSTUME_AURA 
		pPlayer->ShowCostumeAura();
		CDnEtcObject::ProcessClass( LocalTime, fElapsedTime );
#endif
		pActor->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		// �ִϸ��̼� ���
		pActor->MAActorRenderBase::PreProcess( LocalTime, fElapsedTime );

		// �Ʒ� ��Ʋ� MAPartsBody�� Process�� ������?
		EtMatrix WorldMat = *pActor->GetMatEx();
		pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );

		// �Ϲ� ����
		for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
		{
			DnWeaponHandle hWeapon = pPlayer->GetWeapon( i );
			if( !hWeapon ) continue;

			hWeapon->Process( LocalTime, fElapsedTime );
		}

		// �켱 ĳ�������� ���� ��������.
		for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			hParts->Process( LocalTime, fElapsedTime );
		}

		// ĳ�ù��⵵ �߰�.
		for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
		{
			DnWeaponHandle hWeapon = pPlayer->GetCashWeapon( i );
			if( !hWeapon ) continue;

			hWeapon->Process( LocalTime, fElapsedTime );
		}

#ifdef PRE_MOD_KALI_WEAPON
		// CDnPlayerActor::Process�� �ʹ� ������ ����־ ��ó�� �ʿ��� Process�Լ��� ���� ȣ���ϴٺ���,
		// TDnPlayerKali::Process�ȿ� �ִ� LeftWeapon-Processó���� �� ���� ����.
		// �׷��ٰ� CDnPlayerActor::Process�� ȣ���� �� ���, �̷��� ������ �Լ��� ȣ���ϴ°ŷ� �ϰڴ�.
		pPlayer->ProcessAdditionalWeapon( LocalTime, fElapsedTime );
#endif
	}
	else if( m_eActorType == ActorEnum::VehicleActor || m_eActorType == ActorEnum::PetActor )
	{
		if( !m_hActor )
			return;

		CDnActor *pActor = (CDnActor *)m_hActor.GetPointer();

		pActor->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );
		pActor->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		// �ִϸ��̼� ���
		pActor->MAActorRenderBase::PreProcess( LocalTime, fElapsedTime );

		EtMatrix WorldMat = *pActor->GetMatEx();
		pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );
	}
	else if( m_eActorType == ActorEnum::EtcObject )
	{
		if( !m_hEtc )
			return;

		CDnEtcObject * pObject = m_hEtc.GetPointer();
		if( NULL == pObject )
			return;
		EtObjectHandle hObject = pObject->GetObjectHandle();
		if( CEtObject::Identity() == hObject )
			return;

		LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();

		pObject->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );

		pObject->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		EtMatrix WorldMat = *pObject->GetMatEx();
		pObject->CDnActionRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );
		return;
	}

	// ī�޶� ����.
	// ȸ��
	float fCurrentRotateYaw = EtToRadian( (float)m_nRotateYaw / ROTATE_SCALE );
	float fDistYaw = (fCurrentRotateYaw - m_fSmoothRotateYaw );
	m_fSmoothRotateYaw += fDistYaw * EtMin(1.0f, (fElapsedTime * 15.f));

	EtMatrix CamMat, CamPos, CamRotate, CamPI;
	EtMatrixRotationY( &CamPI, ET_PI );	// �ణ �� z�� ��� �Ÿ�����(ȭ�鿡�� ����) ������ �ٶ�����ϹǷ� ó������ �ݴ� ���� �صд�.
	EtMatrixTranslation( &CamPos, 0.0f, m_fDefaultCameraYPos, m_fDefaultCameraZPos );
	EtMatrixRotationY( &CamRotate, -m_fSmoothRotateYaw );	// �ݴ�� �ױ� ������ Yaw���� ���� �ݴ��.
	EtMatrixMultiply( &CamMat, &CamPI, &CamPos );
	EtMatrixMultiply( &CamMat, &CamMat, &CamRotate );
	UpdateCamera( CamMat );

	// ��
	static float fScale = 1.2f;
	float fZoomLevel[5] = {	1.0f, 
		fScale, 
		fScale*fScale, 
		fScale*fScale*fScale,
		fScale*fScale*fScale*fScale };

	m_fSmoothZoomLevel += ( m_nZoomLevel * 0.999f - m_fSmoothZoomLevel ) * EtMin( 1.0f, fElapsedTime * 25.0f );	// m_nZoomLevel�� ���� 0.0 ~ 4.0 ���̰�.
	float fMod = fmodf( m_fSmoothZoomLevel, 1.0f );	// 1.0, 2.0, 3.0, 4.0�϶��� �������ҰŰ� ���̿� ������ �����ؾ��ϴ� ������ ���ϰ�,
	float fResultScale = fZoomLevel[ (int)m_fSmoothZoomLevel ] * (1.0f-fMod) + fZoomLevel[ (int)(m_fSmoothZoomLevel+1) ] * fMod;	// ����.

	// ��ġ.
	static float fXLimit = 1.4f;
	static float fYLimit = 2.0f;
	float fPanLimitWidth = float(m_nDefaultViewPortWidth) / fResultScale * 0.5f * ( EtMin(fXLimit, fResultScale) - 1.0f );
	float fPanLimitHeight = float(m_nDefaultViewPortHeight) / fResultScale * 0.5f * ( EtMin(fYLimit, fResultScale) - 1.0f );
	m_fPanX = EtClamp( m_fPanX, -fPanLimitWidth, fPanLimitWidth );
	m_fPanY = EtClamp( m_fPanY, -fPanLimitHeight, fPanLimitHeight );
	m_fSmoothPanX += ( m_fPanX - m_fSmoothPanX ) * EtMin( 1.0f, fElapsedTime * 10.0f );
	m_fSmoothPanY += ( m_fPanY - m_fSmoothPanY ) * EtMin( 1.0f, fElapsedTime * 10.0f );

	// ī�޶� ����.
	m_hCamera->SetOrthogonalView( int(m_nDefaultViewPortWidth / fResultScale), int(m_nDefaultViewPortHeight / fResultScale), m_fSmoothPanX, -m_fSmoothPanY );
}

bool CDnRenderAvatarNew::IsLoading()
{
	bool bLoading = false;
	if( CDnLoadingTask::IsActive() )
	{
		// ��¥ �ε�ȭ�����̶�� �ؽ�ó �������� �ʴ´�.
		if( CDnLoadingTask::GetInstance().IsEnableBackgroundLoading() == false )
			bLoading = true;
		if( GetInterface().IsPVPLobby() )
			bLoading = false;
	}
	return bLoading;
}

void CDnRenderAvatarNew::BeginRTT()
{
	// �����ƹ�Ÿ�ʿ� ���͸��̶� �����ؼ� �۷ι� ����Ʈ �� ȯ����� �ٲٴ°� ���Ƽ�
	// ����Ʈ�� ���ﶧ�� �������� �ʰ� �ؾ��Ѵ�.
	if( IsLoading() ) return;

	CEtRTTRenderer::BeginRTT();

	// �÷����͸� �����ų�� �ƴ϶�, ���忡 ����Ǵ� ����Ʈ�� ���� �ٲ�����Ѵ�. �Ʒ����� ���������� ������ ���� ������ ���̴�.(���͵� ����������.)
	m_TempAmbient = *CEtLight::GetGlobalAmbient();
	CEtLight::SetGlobalAmbient( &s_TempAmbient );

	// ���� �Ʒ� ������ Initialize�Լ����� �ϴ��ǵ�,
	// ȯ��ε����� RenderAvatar::Initialize�� ���� ȣ��Ǹ�, ���� ����Ʈ �ΰ��� �� �߰��Ǿ������,
	// �̷��� BeginRTT���� üũ�ϱ�� �ߴ�.
	// ����Ʈ��ü�� �ƿ� ���� ���(�� �ε� ���ؼ� ȯ���� �ε���� �ʾ�����) ���� �ӽö���Ʈ�� �����ؼ� ó���Ѵ�.
	if( CEtLight::GetDirLightCount() == 0 )
	{
		if( s_hTempLight[0] && s_hTempLight[1] )
		{
			CEtLight::ForcePushDirLightInfo( s_hTempLight[0].GetPointer() );
			CEtLight::ForcePushDirLightInfo( s_hTempLight[1].GetPointer() );
			m_bUseTempLight = true;
		}
	}

	m_nDirLightCount = CEtLight::GetDirLightCount();
	m_nDirLightCount = min( m_nDirLightCount, 2 );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		m_TempLightDiffuse[i] = CEtLight::GetDirLightInfo(i)->Diffuse;
		m_TempLightDir[i] = CEtLight::GetDirLightInfo(i)->Direction;
		CEtLight::GetDirLightInfo(i)->Diffuse = s_TempLightDiffuse[i];
		CEtLight::GetDirLightInfo(i)->Direction = s_TempLightDir[i];
	}

	// Environment Texture Setting
	if( CEtTexture::GetEnvTexture() == NULL && s_hTempEnviTexture )
	{
		CEtTexture::SetEnvTexture( s_hTempEnviTexture->GetTexturePtr() );
	}
}

void CDnRenderAvatarNew::EndRTT()
{
	if( IsLoading() ) return;

	CEtLight::SetGlobalAmbient( &m_TempAmbient );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		CEtLight::GetDirLightInfo(i)->Diffuse = m_TempLightDiffuse[i];
		CEtLight::GetDirLightInfo(i)->Direction = m_TempLightDir[i];
	}
	if( m_bUseTempLight )
	{
		CEtLight::ForceClearDirLightInfo();
		m_bUseTempLight = false;
	}
	CEtRTTRenderer::EndRTT();
}

void CDnRenderAvatarNew::RenderRTT( float fElapsedTime )
{
	if( IsLoading() ) return;

	if( m_hActor )
	{
		DnActorHandle hActor = m_hActor;

		if( m_eActorType == ActorEnum::PlayerActor )
		{
			MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
			EtObjectHandle hObject = pPartsBody->GetObjectHandle();
			hObject->InitRender();
			CEtRTTRenderer::RenderRTT( fElapsedTime );

			// ��ó�� ���ؼ� �÷����� ����
			PostProcess();

			// �� �½�ũ EternityEngine::RenderFrame( fDelta ); ȣ���Ҷ�
			// �� ���� ��Ʈ���� ��� �ȵǾ������� CEtObject::InitRenderObjectList(); �Լ� ȣ���Ҷ� �� �����ӿ� ����� ��Ʈ������ ���Ǽ� ����ȴ�.
			// �̶� �� CEtAniObject�� ���� CEtAniObject::InitRender�� ȣ��Ǵµ� �̶� CEtAniObject::CalcAni�� ���Ǵ� ��.
			//
			// ��ó�� UI Process�ܿ��� �ϸ�, �ش� �����ӿ��� �ѹ��� ������ ���� �����̱⶧����, -1 �� ����ְ� �ȴ�.
			// ����� ȣ���ؾ��� �κп��� ȣ��Ǹ� �� ��������Ŵ�.
		}
		else if( m_eActorType == ActorEnum::VehicleActor || m_eActorType == ActorEnum::PetActor )
		{
			CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();
			EtObjectHandle hObject = pActor->GetObjectHandle();

			hObject->InitRender();
			CEtRTTRenderer::RenderRTT( fElapsedTime );
			PostProcess();

		}
	}
	else if( m_hEtc )
	{
		if( m_eActorType == ActorEnum::EtcObject )
		{
			CDnEtcObject * pObject = m_hEtc.GetPointer();
			if( NULL == pObject )
				return;
			EtObjectHandle hObject = pObject->GetObjectHandle();
			if( CEtObject::Identity() == hObject )
				return;

			CEtRTTRenderer::InitRenderObject();
			CEtRTTRenderer::RenderRTT( fElapsedTime );
			PostProcess();
		}
	}
}

void CDnRenderAvatarNew::PostProcess()
{
	// ������ �ǿ� �׸��°Ŷ� �Ϲ����� ���� ������δ� ���Ŀ�����Ʈ�� �帮�� ���� �� �ۿ� ����.
	// �Դٰ� ��״� �������� ���Ͱ� �־ ���� UI�� �׸��� �� ���� ��ƼŬ�� �׸��°� �Ҽ��� ������.(������ UI�׸��ͱ��� ���͸� �Ծ...)
	// �׷��� �����Ѱ�,
	// ������ �ǿ� �׸��� ó�� �׷��� ���İ��� �״�� �����ϴ� ���̴�.(���͸� ���̴� BorderAlpha�� �ִ�.)
	// �̰� ���ؼ� ���ĺ����� D3DBLEND_ONE, D3DBLEND_ONE ���� ����ϰ� �Ǿ���.
	// ������ �����ؽ�ó���� Ÿ���ؽ�ó�� 1ȸ �׸��°ű⶧����,
	// �÷���, ���İ� ��� �״�� �ѹ��� ����Ǹ� ���İ��� �Ҿ������ �� ���� ����� �׷����� �ȴ�.

	if( m_bUseColorAdjFilter )
	{
		// �ӽ��ؽ�ó�� ���� RTT ������ ������ ��
		EtTextureHandle hTempTarget = GetTempRenderTarget( m_nWidth, m_nHeight, m_Format );
		hTempTarget->Copy( GetRTT() );

		// ���������� ���°ɷ� �����ؼ� ����Ѵ�.
		float fSat = 0.f;
		fSat = s_pColorAdjFilter->GetSceneSaturation();
		s_pColorAdjFilter->SetSceneSaturation( 1.0f );
		bool bEnableZ = GetEtDevice()->EnableZ( false );	
		bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
		// ������ 0x0�̹Ƿ� One-One���� �׷��� Source �ؽ�ó�� �ȼ��� �״�� �ű��.
		GetEtDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetDepthStencilSurface( NULL );
		s_pColorAdjFilter->SetTexture( hTempTarget->GetMyIndex() );
		s_pColorAdjFilter->DrawFilter( GetRTT(), 1, EtVector2(0, 0), EtVector2(1, 1) , true );
		GetEtDevice()->RestoreRenderTarget();
		GetEtDevice()->RestoreDepthStencil();
		GetEtDevice()->EnableZ( bEnableZ );
		GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
		s_pColorAdjFilter->SetSceneSaturation( fSat );
	}

	if( m_bUseBorderAlpha )
	{
		// �ӽ��ؽ�ó�� ���� RTT ������ ������ ��
		EtTextureHandle hTempTarget = GetTempRenderTarget( m_nWidth, m_nHeight, m_Format );
		hTempTarget->Copy( GetRTT() );

		// RTT�ؽ�ó�� ����Ÿ������ �缳��.
		bool bEnableZ = GetEtDevice()->EnableZ( false );
		bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
		// ������ 0x0�̹Ƿ� One-One���� �׷��� Source �ؽ�ó�� �ȼ��� �״�� �ű��.
		GetEtDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetDepthStencilSurface( NULL );

		GetEtDevice()->SetRenderTarget( GetRTT()->GetSurfaceLevel() );
		GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, false, false );

		// ���ؽ� ����ó���ؼ� ��� ����ó��.
		STextureDiffuseVertex2D Vertices[ 16 ];
		int nWidth, nHeight;
		EtViewPort Viewport;

		GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		GetEtDevice()->SetPixelShader( NULL );
		GetEtDevice()->SetVertexShader( NULL );
		GetEtDevice()->SetTexture( 0, hTempTarget->GetTexturePtr() );
		GetEtDevice()->GetViewport( &Viewport );

		nWidth = Viewport.Width;
		nHeight = Viewport.Height;

		static float fBorder = 0.04f;	// �׵θ����� 4% ��� ����ó��.
		float XPos[4] = { m_RTTUVCoord.fX, 0.0f, 0.0f, m_RTTUVCoord.fX + m_RTTUVCoord.fWidth };	// RTTUVCoord�� �� �����ߴٰ� ����
		float YPos[4] = { m_RTTUVCoord.fY, 0.0f, 0.0f, m_RTTUVCoord.fY + m_RTTUVCoord.fHeight };
		XPos[1] = XPos[0] + (XPos[3]-XPos[0]) * fBorder;
		XPos[2] = XPos[3] - (XPos[3]-XPos[0]) * fBorder;
		YPos[1] = YPos[0] + (YPos[3]-YPos[0]) * fBorder;
		YPos[2] = YPos[3] - (YPos[3]-YPos[0]) * fBorder;

		float UVXPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float UVYPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for( int i = 0; i < 4; ++i )
		{
			UVXPos[i] = XPos[i];
			UVYPos[i] = YPos[i];
			XPos[i] = XPos[i] * nWidth - 0.5f;
			YPos[i] = YPos[i] * nHeight - 0.5f;
		}

		for( int i = 0; i < 4; i++)
		{
			Vertices[i*4+0].Position.y = Vertices[i*4+1].Position.y = Vertices[i*4+2].Position.y = Vertices[i*4+3].Position.y = YPos[i];
			Vertices[ i ].Position.x = Vertices[4 + i].Position.x = Vertices[8 + i].Position.x = Vertices[12 + i].Position.x = XPos[i];
			Vertices[ i ].Position.z = Vertices[4 + i].Position.z = Vertices[8 + i].Position.z = Vertices[12 + i].Position.z = 0.0f;
			Vertices[ i ].Position.w = Vertices[4 + i].Position.w = Vertices[8 + i].Position.w = Vertices[12 + i].Position.w = 1.0f;
			Vertices[ i ].Color = Vertices[4 + i].Color = Vertices[8 + i].Color = Vertices[12 + i].Color = D3DCOLOR_ARGB(255, 255, 255, 255);

			Vertices[i*4+0].TexCoordinate.y = Vertices[i*4+1].TexCoordinate.y = Vertices[i*4+2].TexCoordinate.y = Vertices[i*4+3].TexCoordinate.y = UVYPos[i];
			Vertices[ i ].TexCoordinate.x = Vertices[4 + i].TexCoordinate.x = Vertices[8 + i].TexCoordinate.x = Vertices[12 + i].TexCoordinate.x = UVXPos[i];
		}
		Vertices[ 0 ].Color = Vertices[ 1 ].Color = Vertices[ 2 ].Color = Vertices[ 3 ].Color = Vertices[ 4 ].Color = Vertices[ 7 ].Color = D3DCOLOR_ARGB(0, 255, 255, 255);
		Vertices[ 8 ].Color = Vertices[ 11 ].Color = Vertices[ 12 ].Color = Vertices[ 13 ].Color = Vertices[ 14 ].Color = Vertices[ 15 ].Color = D3DCOLOR_ARGB(0, 255, 255, 255);

		unsigned short Indices[54] = {
			0, 5, 4, 0, 1, 5, 1, 2, 5, 6, 5, 2, 2, 3, 6, 7, 6, 3,
			4, 5, 8, 9, 8, 5, 5, 6, 9, 10, 9, 6, 6, 7, 10, 11, 10, 7,
			8, 9, 12, 13, 12, 9, 9, 10, 13, 14, 13, 10, 10, 15, 14, 10, 11, 15
		};
		GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, 16, 18, Indices, FMT_INDEX16, Vertices, sizeof( STextureDiffuseVertex2D ) );

		// Ÿ�� ����
		GetEtDevice()->RestoreRenderTarget();
		GetEtDevice()->RestoreDepthStencil();
		GetEtDevice()->EnableZ( bEnableZ );
		GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
	}
}

void CDnRenderAvatarNew::SetFrontView()
{
	m_nRotateYaw = (m_nRotateYaw + (ROTATE_SCALE * 180) * (m_nRotateYaw >= 0 ? 1 : -1 ) ) / (ROTATE_SCALE*360) * (ROTATE_SCALE*360);
}

void CDnRenderAvatarNew::SetRearView()
{
	m_nRotateYaw = (m_nRotateYaw ) / (ROTATE_SCALE*360) * (ROTATE_SCALE*360) + (ROTATE_SCALE * 180) * (m_nRotateYaw >= 0 ? 1 : -1 ) ;
}

void CDnRenderAvatarNew::ZoomIn()
{
	if( m_nZoomLevel < 4 )
		++m_nZoomLevel;
}

void CDnRenderAvatarNew::ZoomOut()
{
	if( m_nZoomLevel > 0 )
		--m_nZoomLevel;
}

void CDnRenderAvatarNew::Panning( float fX, float fY )
{
	m_fPanX += fX / 5.0f;
	m_fPanY += fY / 5.0f;
}

EtTextureHandle CDnRenderAvatarNew::GetTempRenderTarget( int nWidth, int nHeight, EtFormat Format )
{
	for( int i = 0; i < (int)s_vecTempTarget.size(); ++i )
	{
		if( s_vecTempTarget[i].nWidth == nWidth &&
			s_vecTempTarget[i].nHeight == nHeight &&
			s_vecTempTarget[i].Format == Format )
		{
			return s_vecTempTarget[i].hTexture;
		}
	}

	sTempTargetInfo Info;
	Info.nWidth = nWidth;
	Info.nHeight = nHeight;
	Info.Format = Format;
	Info.hTexture = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, Format );
	s_vecTempTarget.push_back( Info );
	return Info.hTexture;
}

void CDnRenderAvatarNew::SetEtc( const char *szSkinName, const char *szAniName, const char *szActName )
{
	m_hEtc = (new CDnEtcObject)->GetMySmartPtr();
	m_hEtc->Initialize( szSkinName, szAniName, szActName );

	CheckType();

	CDnEtcObject * pObject = m_hEtc.GetPointer();
	if( NULL == pObject )
		return;

	m_hEtc->SetRTTRenderMode( true, this );
	m_hEtc->SetActionQueue( "Stand" );
	m_hEtc->Show( true );
	AddObject( pObject->GetObjectHandle() );
	SetRenderFrameCount( -1 );

	return;
}

void CDnRenderAvatarNew::ResetEtc( bool bInitCamera )
{
	if( m_hActor )
	{
		SAFE_RELEASE_SPTR( m_hEtc );
		m_eActorType = ActorEnum::None;
		SetRenderFrameCount( 0 );
	}

	if( bInitCamera )
		InitCameraControl();
}