#include "StdAfx.h"
#include "DnHideMonsterActor.h"
#include "DnCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnHideMonsterActor::CDnHideMonsterActor( int nClassID, bool bProcess )
: CDnSimpleRushMonsterActor( nClassID, bProcess )
{
}

CDnHideMonsterActor::~CDnHideMonsterActor()
{
}
