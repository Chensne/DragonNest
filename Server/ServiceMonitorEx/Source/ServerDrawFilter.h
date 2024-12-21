

#pragma once

#include "Define.h"
#include "ServiceInfo.h"


// CServerDrawBaseFilter

class CServerDrawBaseFilter
{

public:
	CServerDrawBaseFilter(EF_SERVERTYPE pServerType)
		: m_ServerType(pServerType), m_Width(g_ServerTypeSize[pServerType].cx), m_Height(g_ServerTypeSize[pServerType].cy)
	{
		ASSERT(EV_SVT_NONE < m_ServerType && EV_SVT_CNT > m_ServerType);
		ASSERT(0 < m_Width);
		ASSERT(0 < m_Height);
				
	}
//	virtual ~CServerDrawBaseFilter() { }

	VOID Set(const SERVERDRAW* pServerDraw)
	{
		ASSERT(!IsSet());
		if (!pServerDraw) return;
		ASSERT(pServerDraw->IsSet());
		
		m_ServerDraw = const_cast<SERVERDRAW*>(pServerDraw);
	}
	VOID Reset()
	{
// 		m_ServerType = EV_SVT_CNT;	// ���� �ʵ�
// 		m_Width = 0;				// ���� �ʵ�
// 		m_Height = 0;				// ���� �ʵ�
		m_ServerDraw = NULL;
	}
	BOOL IsSet() const { return(NULL != m_ServerDraw && m_ServerDraw->IsSet()); }

	const EF_SERVERTYPE GetServerType() const { return m_ServerType; }
	const SERVERDRAW* GetServerDraw() const { return m_ServerDraw; }
	const int GetWidth() const { return m_Width; }
	const int GetHeight() const { return m_Height; }

	virtual void Draw(CDC* pDC, int pStartX, int pStartY) = 0;

protected:
	virtual void DrawFrame(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawServerID(CDC* pDC, int pStartX, int pStartY);

protected:
	const EF_SERVERTYPE m_ServerType;
	const int m_Width;
	const int m_Height;

	SERVERDRAW* m_ServerDraw;

};


// CServerDrawDervFilter

template<EF_SERVERTYPE pTYPE>
class CServerDrawDervFilter : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(pTYPE, 0, 0)
	{
		char aCompileError[-1];		// ������ ���� ���� ����
	}
	void Draw(CDC* pDC, int pStartX, int pStartY)
	{
		char aCompileError[-1];		// ������ ���� ���� ����
	}
};


// Login

template<>
class CServerDrawDervFilter<EV_SVT_LO> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_LO)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);

protected:
	virtual void DrawServerID(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawUserCount(CDC* pDC, int pStartX, int pStartY);
};


// DB (Middleware)

template<>
class CServerDrawDervFilter<EV_SVT_DB> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_DB)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);
	void DrawServerID(CDC* pDC, int pStartX, int pStartY);
	void DrawDelayCount(CDC* pDC, int pStartX, int pStartY);
};

// Master

template<>
class CServerDrawDervFilter<EV_SVT_MA> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_MA)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);
};


// Village

template<>
class CServerDrawDervFilter<EV_SVT_VI> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_VI)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);

protected:
	virtual void DrawServerID(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawUserCount(CDC* pDC, int pStartX, int pStartY);
};


// Game

template<>
class CServerDrawDervFilter<EV_SVT_GA> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_GA)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);

protected:
	virtual void DrawServerID(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawUserCount(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawRoomCount(CDC* pDC, int pStartX, int pStartY);
	virtual void DrawDelayCount(CDC* pDC, int pStartX, int pStartY);
};


// Log

template<>
class CServerDrawDervFilter<EV_SVT_LG> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_LG)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);
};


// Cash

template<>
class CServerDrawDervFilter<EV_SVT_CA> : public CServerDrawBaseFilter
{

public:
	CServerDrawDervFilter() : CServerDrawBaseFilter(EV_SVT_CA)
	{
		
	}
	void Draw(CDC* pDC, int pStartX, int pStartY);
};


#include "ServerDrawFilter.inl"

