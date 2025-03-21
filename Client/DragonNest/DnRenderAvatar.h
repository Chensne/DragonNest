#pragma once

#include "EtRTTRenderer.h"
class CDnRenderAvatarNew : public CEtRTTRenderer	// 나중에 이름이나 바꿔둘까.. 그냥 New붙였는데, 바꾸기엔 이미 늦은듯;
{
public:
	CDnRenderAvatarNew();
	virtual ~CDnRenderAvatarNew();

	void Initialize( int nWidth, int nHeight, SCameraInfo &CameraInfo, int nViewPortWidth, int nViewPortHeight, EtFormat Format = FMT_A8B8G8R8, bool bUseColorAdjFilter = true, bool bUseBorderAlpha = true );
	void CameraLookAt( EtVector3 &Eye, EtVector3 &At, EtVector3 &Up );

	bool SetActor( DnActorHandle hActor, bool bShowWeapon = true, bool bForceCashWeapon = false );
	bool SetActor( int nClassID, bool bShowWeapon = true );	// 주변에 액터핸들로 생성되지 않은 대상에 대해선 디폴트모습으로 직접 생성한다.
	bool ResetActor( bool bInitCamera = true );
	int GetActorID();
	DnActorHandle GetActor() { return m_hActor; }

	// 인챈트 레벨. 무기의 경우 강화이펙트가 붙기때문에 강화레벨 인자로 줄 수 있다. 방어구의 경우 그냥 무시.
	bool AttachItem( int nItemID, int nEnchantLevel = 0, int nSubArg1 = 0, int nLookItemID = ITEMCLSID_NONE );
	// 캐시샵 프리뷰에서 장바구니 비우기같은것을 할때같이 꼭 필요해서, 결국 Detach함수를 만들게 되었다.
	// 차라리 처음부터 이걸 고려했다면, 디폴트파츠부터 다 가지고 있는 편이 나았던거 같은데..
	// 뭐 어쨌든 다음번 리팩토링 기회가 온다면 파츠 탈부착이 RenderAvatar내부에서 다 이뤄지게 해야겠다.
	bool DetachItem( int nItemID );
	// 인자로 받은 액터의 장비 정보를 복사한다. 현재 플레이어액터에 대해선 구현되어있지 않고, 탈것만 되어있다.
	bool RefreshEquip( DnActorHandle hActor );

	SUICoord &GetUVCoord() { return m_RTTUVCoord; }
	void CalcUVCoord( float fUIWidth, float fUIHeight );
	void SetCameraYPos( float fYPos = 56.0f ) { m_fDefaultCameraYPos = fYPos; }

	virtual void Process( float fElapsedTime );
	virtual void BeginRTT();
	virtual void EndRTT();
	virtual void RenderRTT( float fElapsedTime );


	// 기존코드와의 호환성때문에 그대로 가져간다.
	void AddRotateYaw( float fAngle ) { m_nRotateYaw += (int)( fAngle * ROTATE_SCALE ); }
	bool IsFrontView() { return (m_nRotateYaw % (360*ROTATE_SCALE)) == 0; }
	void SetFrontView();
	void SetRearView();
	void ZoomIn();
	void ZoomOut();
	void Panning( float fX, float fY );

	// EtcObject
	void SetEtc( const char *szSkinName, const char *szAniName, const char *szActName );
	DnEtcHandle GetEtc() { return m_hEtc; }
	void ResetEtc( bool bInitCamera );

protected:

	enum {
		ROTATE_SCALE = 10,
	};

	// 파생받아 처리하는건 Have관계에서 소스 지저분해지기 때문에, 그냥 내부에서 타입으로 구분해 사용하기로 하겠다.
	enum ActorEnum {
		None,
		PlayerActor,
		VehicleActor,
		PetActor,
		EtcObject,
		//MonsterActor,	// 싱글바디 액터는 전부 포함이다. 아직 지원하지 않음.
	};

	// 게이지에 표시되는 캐릭터얼굴과 달리 살펴보기나 캐시샵의 프리뷰에서는 렌더링에 사용할 새 물체를 만들어써야한다.
	// 처음에는 MAPartsBody(or SingleBody)와 액션시그널을 따로 받는 형태로 만들까하다가,
	// 결국 각 파츠에 있는 액션시그널을 따로따로 전달하기 어렵다는 점, 웨폰 링크 따로 해줘야한다는 점.
	// 몬스터액터의 경우엔 시그널이 파츠에 있는게 아니라, Actor자체에 있다는 점. 등 몇가지 문제점때문에, 액터하나를 통째로 만들어 쓰기로 했다.
	DnActorHandle m_hActor;
	DnEtcHandle m_hEtc;
	ActorEnum m_eActorType;

	// 한벌옷 장착할때 예외처리때문에 입고 있던 파츠에 대한 정보를 따로 가지고 있는다.
	typedef struct _SPartsInfo {
		int nType;
		int nItemID;
	} SPartsInfo;
	std::vector<SPartsInfo> m_vecPartsInfo;

	SUICoord m_RTTUVCoord;

	struct sTempTargetInfo
	{
		int nWidth;
		int nHeight;
		EtFormat Format;
		EtTextureHandle hTexture;
	};
	static std::vector<sTempTargetInfo> s_vecTempTarget;
	static int s_nTempTargetRefCount;

	static CEtColorAdjustTexFilter *s_pColorAdjFilter;
	// 아예 라이트 객체가 생성되지 않았을때는 직접 생성하기도 한다.
	static EtLightHandle s_hTempLight[2];
	// EnviTexture도.
	static EtTextureHandle s_hTempEnviTexture;
	// 그 외 컬러 필터링에 필요한 환경값들
	static EtColor s_TempAmbient;
	static EtColor s_TempLightDiffuse[2];
	static EtVector3 s_TempLightDir[2];

	static bool s_bInitializeStaticObject;
	static void InitializeStaticObject();
	static void FinalizeStaticObject();

	bool m_bUseColorAdjFilter;
	bool m_bUseBorderAlpha;

	void PostProcess();	// 컬러필터 후처리. 게임에서 필터 먹은 것처럼 렌더타겟에서도 필터 먹인다.
	void InitCameraControl();
	void CheckType();
	void CheckActorType();
	void CheckEtcType();
	bool IsLoading();

	// 공용 임시 렌더타겟 얻는 함수
	EtTextureHandle GetTempRenderTarget( int nWidth, int nHeight, EtFormat Format );

	// 카메라 처리 관련해서 나머지는 RTTRenderer가 가지고 있는 카메라 쓰고,
	// 줌인 및 기타 값들만 따로 들고있기로 한다.
	float m_fDefaultCameraYPos;
	float m_fDefaultCameraZPos;
	int m_nRotateYaw;
	float m_fSmoothRotateYaw;

	// 줌인 줌아웃은 원근투영이 아니기때문에 카메라를 옮길수도, fov를 조정할수도 없다. 뷰포트 크기 바꿔서 줌인하는 것처럼 보이는 수 밖에..
	// 예전 렌더아바타에선 캐릭터 스케일을 변경했는데, 그건 여러가지 이펙트나 기타 등등의 다른 문제가 생길거 같아서 하지 않겠다.
	int m_nDefaultViewPortWidth;
	int m_nDefaultViewPortHeight;
	int m_nZoomLevel;
	float m_fSmoothZoomLevel;

	float m_fPanX;
	float m_fPanY;
	float m_fSmoothPanX;
	float m_fSmoothPanY;

	// 엠비언트랑, 방향광 2개까지 임시로 복사해두고 찍는다. Orig값 담아둘 임시변수.
	EtColor m_TempAmbient;
	int m_nDirLightCount;
	EtColor m_TempLightDiffuse[2];
	EtVector3 m_TempLightDir[2];
	bool m_bUseTempLight;
};
