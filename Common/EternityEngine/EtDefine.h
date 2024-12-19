#pragma once

enum PreDefines
{
	PRE_GRASS_ALPHABLEND,
	PRE_PARTICLE_OPTIMIZE,
	PRE_SHADOW_BACK_PROJECTION_PROBLEM,
	PRE_PROP_ALPHA_BUG,
	PRE_OBJECT_JANSANG,
	PRE_PROP_ALPHABLEND,
	PRE_WATER_WAVELENGTH, 
	PRE_DEFINE_COUNT,
};

const bool IsDefined( PreDefines Def );
const bool IsNotDefined( PreDefines Def );
void SetDefine( PreDefines Def, bool bEnable );
void ToggleDefine( PreDefines Def );
