#pragma once



// 렌더링 되는 오브젝트들의 인터페이스를 정의
class IDnCutSceneRenderObject
{
private:
	

public:
	IDnCutSceneRenderObject(void);
	virtual ~IDnCutSceneRenderObject(void);

	virtual int GetAniIndex( const char* pAniName ) = 0;
	virtual void ChangeAnimation( int iAniIndex, float fFrame, float fBlendFrame = 0.0f )  {}; 
};
