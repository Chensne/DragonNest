#pragma once
#include "EtUIControl.h"
#include "DnCustomControlCommon.h"
#include "EtRTTRenderer.h"
#include "CameraData.h"

class CDnGaugeFace : public CEtUIControl, public CEtRTTRenderer, public CLostDeviceProcess
{
public:
	CDnGaugeFace( CEtUIDialog *pParent );
	virtual ~CDnGaugeFace(void);

protected:
	int m_nFaceID;
	GAUGE_FACE_TYPE m_FaceType;
	EtTextureHandle m_hFaceTexture;
	EtTextureHandle m_hMaskTexture;
	EtTextureHandle m_hBossMaskTexture;
	EtTextureHandle m_hTempTextureForDeviceLost;
	bool m_bUseTempTextureForDeviceLost;
	
	SUICoord m_FaceUV;
	SUICoord m_FaceCoord;
	DWORD m_dwFaceColor;
	float m_fFaceTime;
	bool m_bDamage;
	bool m_bDeleteObject;
	bool m_bBoss;

	SCameraFileHeader m_CamHeader;
	EtObjectHandle m_hFaceObject;
	int m_nAniIndex;
	float m_fFrame;

	CEtColorAdjustTexFilter *m_pColorAdjFilter;

	// GaugeFace는 다른 프리뷰들과 달리 간단하게 찍는거라, 엠비언트만 변경하기로 하려다,
	// 다크레어 같은 경우엔 방향광에 색상이 너무 많이 들어있어서 RenderAvatar처럼 다 설정하기로 한다.
	EtColor m_TempAmbient;
	int m_nDirLightCount;
	EtColor m_TempLightDiffuse[2];
	EtVector3 m_TempLightDir[2];

	// 일부 파티클로만 이루어진 보스들은 skn, cam파일 대신 dds텍스처를 가지고 있다.
	EtTextureHandle m_hBossPortraitTexture;

	bool IsLocalPlayerGaugeFace();
 
public:
	void SetFaceType( GAUGE_FACE_TYPE FaceType );
	void SetFaceID( int nID );
	int GetFaceID() { return m_nFaceID; }
	void CalcFaceCoord();
	void OnDamage();
	void UpdatePortrait( EtObjectHandle hHandle, bool bDelete, bool bBoss = false, int nAniIndex = 0, float fFrame = 0.0f );
	void BakePortrait();
	bool IsUpdatePortrait();	
	EtTextureHandle GetPortraitTexture() {return m_hFaceTexture; }
	void SetTexture( EtTextureHandle hTexture);
	void BackupPortrait();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );

	virtual void BeginRTT();
	virtual void EndRTT();
	virtual void RenderRTT( float fElapsedTime );
};
