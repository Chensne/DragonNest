#pragma once

class CEtCustomRender
{
public:
	CEtCustomRender(void);
	virtual ~CEtCustomRender(void);

protected:
	bool m_bShow;
	static std::vector<CEtCustomRender *> s_pVecStaticList;
	static CSyncLock s_CustomLock;
	float m_fCustomRenderDepth;

public:
	float GetCustomRenderDepth() {return m_fCustomRenderDepth;}
	virtual void RenderCustom( float fElapsedTime ) {}

	void Enable( bool bShow ) { m_bShow = bShow; }
	bool IsEnable() { return m_bShow; }

	static bool s_bEnableCustomRender;
	static void RenderCustomList( float fElapsedTime );

	void EraseCustomRenderList();
};
