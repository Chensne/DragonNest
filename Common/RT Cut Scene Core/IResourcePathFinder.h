#pragma once


// 리소스 패스 관리자 래핑 인터페이스
// RTCutSceneCore 에는 패스 관리자 같은 것이 있을 수 없기 때문에 이런 방식으로 해결..
class IResourcePathFinder
{
public:
	IResourcePathFinder(void);
	virtual ~IResourcePathFinder(void);

	virtual void GetFullPath( const char* pFileName, /*IN OUT*/string& strFullPath ) { };
};
