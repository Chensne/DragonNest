#include "stdafx.h"
#include "MtRandom.h"
#include <time.h>

// 생성자, 파괴자 등이 있는 클래스는 TLS에 할당되지 못하며,
// 전역 구조체의 변수는 모두 0 으로 초기화되는데 그것에 대한 처리도 해주어야 된다.

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void CMtRandom::srand(unsigned long seed)
{
	m_seed = seed;
	int j;
	m_state[0]= seed & 0xffffffffUL;
	for (j=1; j<N; j++) {
		m_state[j] = (1812433253UL * (m_state[j-1] ^ (m_state[j-1] >> 30)) + j);
		m_state[j] &= 0xffffffffUL;
	}
	m_left = 1;
	m_initf = 1;
}

#define MT_MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define MT_TWIST(u,v) ((MT_MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

void CMtRandom::next_state(void)
{
	unsigned long *p=m_state;
	int j;

	if (m_initf==0) srand((unsigned long)time(0));
	m_left = N;
	m_next = m_state;

	for (j=N-M+1; --j; p++)
		*p = p[M] ^ MT_TWIST(p[0], p[1]);

	for (j=M; --j; p++)
		*p = p[M-N] ^ MT_TWIST(p[0], p[1]);

	*p = p[M-N] ^ MT_TWIST(p[0], m_state[0]);
}

float CMtRandom::genrand_real2(void)
{
	return (float)rand() * (1.f/4294967296.f);
	/* divided by 2^32 */
}

int CMtRandom::rand(void)
{
	if( m_initf == 0 ) m_left = 1;
	
	int y;
	if (--m_left == 0) next_state();
	y = *m_next++;

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	if( y < 0 ) y = -y;
	return y;
}

int CMtRandom::rand(unsigned long range)
{
	// Find which bits are used in n
	// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
	unsigned int used = range;
	used |= used >> 1;
	used |= used >> 2;
	used |= used >> 4;
	used |= used >> 8;
	used |= used >> 16;

	// Draw numbers until one is found in [0, range-1]
	int n;
	do {
		n = (unsigned int)rand() & used;  // toss unused bits to shorten search
	}
	while( n >= (int)range );
	return n;

	/*return rand() % range;*/
}

// float CMtRandom::rand(float smallVal, float largeVal)
// {
// 	float interval = genrand_real2();
// 	return smallVal * interval + largeVal * (1-interval);	
// }

int CMtRandom::rand(int smallVal, int largeVal)
{
	return rand(largeVal-smallVal+1) + smallVal;
}

float CMtRandom::rand_float(float smallVal, float largeVal)
{
	return (smallVal + float(rand()) / (2147483648.f/(largeVal - smallVal)));
}