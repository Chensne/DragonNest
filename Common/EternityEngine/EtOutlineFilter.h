#pragma once
#include "SmartPtr.h"
#include "EtPostProcessFilter.h"
class CEtOutlineObject;
typedef CSmartPtr< CEtOutlineObject > EtOutlineHandle;
class CEtOutlineObject : public CSmartPtrBase< CEtOutlineObject >
{
private:
	EtObjectHandle m_hObject;
	float m_fIntensity;
	float m_fIntensitySpeed;
	EtColor m_vColor;
	float m_fWidth;
	bool m_bShow;

public:
	CEtOutlineObject( EtObjectHandle object );
public:
	bool ProcessIntensity( float fElapsedTime );

	// 여러가지 액션에 아웃라인필터가 먹여져있는경우에 액션이 바뀔때마다 ProcessIntensity가 돌면서 깜박이게 되므로 바로 맥스치 설정해줌
	void SkipProcessIntensity(){m_fIntensity = 1.0f;}

	void ClassifyChild(  int nIndex, std::vector< std::pair<EtObjectHandle, int> > &vecObjects );
	void SetColor( EtColor color ) { m_vColor = color;}
	void SetWidth( float width ) { m_fWidth = width; }
	void Show(bool bShow, float fSpeed = 1.0f);
	bool IsShow() { return m_bShow; }
	
	float GetIntensity() { return m_fIntensity; }
	float GetIntensitySpeed() { return m_fIntensitySpeed; }
	float GetBorderWidth() { return m_fWidth; }
	EtColor GetColor() { return m_vColor; }
	EtObjectHandle GetObject() { return m_hObject; }
	void SetObject( EtObjectHandle hObject ) { m_hObject = hObject; }

	static EtOutlineHandle Create( EtObjectHandle hObject );
};

class CEtOutlineFilter : public CEtPostProcessFilter, public CLostDeviceProcess
{	
public:
	CEtOutlineFilter();
	virtual ~CEtOutlineFilter();
	virtual void Clear();

protected:
	float m_fIntensityParam;
	float	m_fBorderWidthParam;
	EtColor m_BorderColorParam;
	EtVector4 m_PixelSize;

	EtTextureHandle m_hOutlineSource;
	EtTextureHandle m_hBlurTarget;

	EtVector3  m_Bounds[ 8 ];

public:
	virtual void Initialize();
	void Render( float fElapsedTime );

	void DrawOutline( EtObjectHandle hObject, EtOutlineHandle hOutline, SAABox &Box );
	
	virtual void OnResetDevice();
}; 
/*
#pragma once
#include "SmartPtr.h"
#include "EtPostProcessFilter.h"
class CEtOutlineObject;
typedef CSmartPtr< CEtOutlineObject > EtOutlineHandle;
class CEtOutlineObject : public CSmartPtrBase< CEtOutlineObject >
{
public:
	enum OutlineTypeEnum {
		NORMAL = 0,
		SOLID = 5,
		DEPTH_CHECK = 7,
	};
private:
	EtObjectHandle m_hObject;
	float m_fIntensity;
	float m_fIntensitySpeed;
	EtColor m_vColor;
	float m_fWidth;
	bool m_bSolid;
	bool m_bSkipStaticObject;
	bool m_bShow;
	OutlineTypeEnum m_Type;

public:
	CEtOutlineObject( EtObjectHandle object );
public:
	bool ProcessIntensity( float fElapsedTime );
	void ClassifyChild(  int nIndex, std::vector< std::pair<EtObjectHandle, int> > &vecObjects );
	void SetColor( EtColor color ) { m_vColor = color;}
	void SetWidth( float width ) { m_fWidth = width; }
	void Show(bool bShow, float fSpeed = 1.0f);
	bool IsShow() { return m_bShow; }

	void SetSolid( bool solid ) {m_bSolid = solid; }
	bool IsSolid() { return m_bSolid;}
	void SkipStaticObject( bool bSkip ) { m_bSkipStaticObject = bSkip; }
	bool IsSkipStaticObject() { return m_bSkipStaticObject; }

	float GetIntensity() { return m_fIntensity; }
	float GetIntensitySpeed() { return m_fIntensitySpeed; }
	float GetBorderWidth() { return m_fWidth; }
	EtColor GetColor() { return m_vColor; }
	EtObjectHandle GetObject() { return m_hObject; }

	void SetType( OutlineTypeEnum Type ) { m_Type = Type; }
	int GetType() { return (int)m_Type;}

	static EtOutlineHandle Create( EtObjectHandle hObject );
};

class CEtOutlineFilter : public CEtPostProcessFilter, public CLostDeviceProcess
{	
public:
	CEtOutlineFilter();
	virtual ~CEtOutlineFilter();
	virtual void Clear();

protected:
	float m_fBloomScale;
	float m_fIntensityParam;
	float	m_fBorderWidthParam;
	EtColor m_BorderColorParam;

	EtTextureHandle m_hBrightTarget;
	EtTextureHandle m_hHoriBlurTarget;
	EtTextureHandle m_hVertBlurTarget;
	EtDepthHandle m_hDepth;

	EtVector3  m_Bounds[ 8 ];

public:
	virtual void Initialize();
	void Render( float fElapsedTime );

	void DrawOutline( EtObjectHandle hObject, EtOutlineHandle hOutline, SAABox &Box );
	// Global
	void SetBloomScale( float fScale ) { m_fBloomScale = fScale; }
	float GetBloomScale() { return m_fBloomScale; }

	virtual void OnResetDevice();
}; 
*/