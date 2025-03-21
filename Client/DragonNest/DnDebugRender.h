#pragma once

// 그냥 디버깅용으로 그려주는 클래스..
class CDnDebugRender : public CEtCustomRender
{
public:
	CDnDebugRender();
	virtual ~CDnDebugRender();

	virtual void RenderCustom( float fElapsedTime );

	EtTextureHandle m_hTexture;
};

// CEtCustomRender static 변수들 보다 더 늦게 생성자가 호출 되어야 하므로 new 로 생성/삭제 한다.
extern CDnDebugRender* g_pDnDebugRender;