#include "stdAfx.h"
#include "Random.h"
#include "MtRandom.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

__declspec(thread) CMtRandom g_Random;
