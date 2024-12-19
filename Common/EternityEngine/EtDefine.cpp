#include "StdAfx.h"
#include "EtDefine.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

/*
	엔진은 게임쪽과 독립적이서 Define 관리를 별개로 한다.
*/

#define PRE_DEFINE( def ) {s_Defines[ def ] = true;}

static bool s_Defines[ PRE_DEFINE_COUNT ] = { false , };
static struct MarkDefine
{
	MarkDefine() {
		PRE_DEFINE( PRE_PROP_ALPHA_BUG )
		/*PRE_DEFINE( PRE_PARTICLE_OPTIMIZE )
		PRE_DEFINE( PRE_SHADOW_BACK_PROJECTION_PROBLEM )
		PRE_DEFINE( PRE_OBJECT_JANSANG )
		PRE_DEFINE( PRE_WATER_WAVELENGTH )
		PRE_DEFINE( PRE_GRASS_ALPHABLEND )*/
	}
}s_markDefine;

const bool IsDefined( PreDefines Def )
{
	return s_Defines[ Def ];
}

const bool IsNotDefined( PreDefines Def )
{
	return !s_Defines[ Def ];	
}

void SetDefine( PreDefines Def, bool bEnable )
{
	s_Defines[ Def ] = bEnable;
}

void ToggleDefine( PreDefines Def )
{
	SetDefine( Def, !IsDefined( Def ) );
}
