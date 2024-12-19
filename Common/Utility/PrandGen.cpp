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
#include <stdlib.h>
// Standard ANSI C header file 
#include <limits.h>
#include "stdafx.h"
// Standard ANSI C header for type limits
#include "PrandGen.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// The constructor implementation
CPseudoRandom::CPseudoRandom()
{
	m_lSeed = 0;
	m_lGen1 = 0;
	m_lGen2 = 0;
}

// The destructor implementation
CPseudoRandom::~CPseudoRandom()
{
	m_lSeed = 0;
}

long CPseudoRandom::rand(long lMax)
{
	long lNewSeed = m_lSeed;
	long lReturn;

	lNewSeed = (m_lGen1 * lNewSeed) + m_lGen2;
	// Use modulo operator to ensure < ulMax

	m_lSeed = lNewSeed;
	lReturn = m_lSeed % lMax;
	if (lReturn < 1) lReturn = lReturn * -1; // Keep it positive
	return lReturn;
}

void CPseudoRandom::srand(long lSeed)
{
	m_lSeed = lSeed;
	// Pick two large integers such that
	// one is double the other
	m_lGen2 = 3719;
	m_lGen1 = (m_lGen2 / 2);
}