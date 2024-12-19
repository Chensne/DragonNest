#pragma once

/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
* All rights reserved worldwide.
*
* This software is provided "as is" without express or implied
* warranties. You may freely copy and compile this source into
* applications you distribute provided that the copyright text
* below is included in the resulting source code, for example:
* "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
*/
// Pseudo-Random Number Generator
// (C) 2000 Guy W. Lecky-Thompson


#include <limits.h>

class CPseudoRandom {
protected:
	long m_lSeed;
	long m_lGen1;
	long m_lGen2;

public:
	CPseudoRandom();
	~CPseudoRandom();

	long rand(long lMax = INT_MAX);
	void srand(long lSeed);
	long GetSeed() { return m_lSeed; }
};

#pragma comment(lib,"winmm.lib")

