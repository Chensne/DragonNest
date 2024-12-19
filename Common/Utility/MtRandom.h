#pragma once 

struct CMtRandom
{
public:

#ifndef _CLIENT
	CMtRandom()
	{
		m_next = m_state;
		m_initf = 0;
		m_left = 0;
		m_nLockInitf = 0;
		m_nLockLeft = 0;
		m_nLockSeed = 0;
		m_seed = 0;
	}
#endif // #ifndef _CLIENT

	unsigned long m_state[624]; /* the array for the state vector  */
	int m_left;
	int m_initf;
	unsigned long *m_next;
	unsigned long m_seed;

	enum
	{
		N = 624,
		M = 397,
		MATRIX_A = 0x9908b0dfUL,
		UMASK = 0x80000000UL,
		LMASK = 0x7fffffffUL,
	};

	void next_state(void);
	float genrand_real2(void);

	long m_nLockSeed;
	int m_nLockLeft;
	int m_nLockInitf;
	//unsigned long *m_pLockNext;
	void LockSeed() { 
		m_nLockSeed = m_seed;
		m_nLockLeft = m_left;
		m_nLockInitf = m_initf;
		//m_pLockNext = m_next;
	}
	void UnlockSeed() { 
		m_seed = m_nLockSeed;
		m_left = m_nLockLeft;
		m_initf = m_nLockInitf;
		//m_next = m_pLockNext;
	}

	void srand(unsigned long seed);
	int rand(void);
	int rand(unsigned long range);

	// 이 함수는 genrand_real2() 함수의 정규화 과정에 문제가 있어서 제대로 동작하지 않습니다.
	// rand_float 함수로 대체 사용 부탁드립니다. by kalliste
	//float rand(float smallVal, float largeVal);

	int rand(int smallVal, int largeVal);
	float rand_float(float smallVal, float largeVal);
	int GetSeed() { return m_seed; }
};

#ifdef PRE_ADD_MTRANDOM_CLIENT
class CMtRandomLocalUsable : public CMtRandom
{
public:
	CMtRandomLocalUsable()
	{
		memset(&m_state, 0, sizeof(m_state));
		m_next = m_state;
		m_initf = 0;
		m_left = 0;
		m_nLockInitf = 0;
		m_nLockLeft = 0;
		m_nLockSeed = 0;
		m_seed = 0;
	}
};
#endif