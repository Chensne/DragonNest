#pragma once
#include "CriticalSection.h"
enum eBufferError
{
	SIZEERR	= -2,
	NONE = 0,
	COMPLETE,
};

class CBuffer
{
private:
	CSyncLock m_Lock;

	char *m_pBuffer;
	int m_Head;
	int m_Tail;
	int m_Count;
	int m_MaxSize;

public:
	CBuffer(const int nSize);
	~CBuffer(void);

	int GetCount();
	void Clear(bool bBufferClear = false);

	int Push(const char *pData, int nSize, bool bIsUserSession=false, bool bIncreaseBuffer=false );
	int Insert( const char *pData, int nSize );
	int Pop(char *pData, const int nSize);
	int View(char *pData, const int nSize);
	int Skip(const int nSize);

	bool IsComplete();	// SendBuffer용
	int IsComplete(bool boServer);	// RecvBuffer용
	USHORT GetComplete(char *pData);	// 완료패킷가져오기
};
