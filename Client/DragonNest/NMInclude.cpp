//
//	프로젝트에서 사용하는 프리컴파일 헤더 파일을 추가해 줍니다
//
#include "stdafx.h"

#if defined(_KR) || defined(_US)
//
//	다음 라인은 수정 없이 추가해 줍니다
//
#include "KOR_NEXON/NMClass/NMFiles.cpp"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#endif // _KR, _US