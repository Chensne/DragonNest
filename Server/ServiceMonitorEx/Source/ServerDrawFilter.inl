

#include "StdAfx.h"
#include "VarArg.hpp"


// CServerDrawBaseFilter

inline void CServerDrawBaseFilter::DrawFrame(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	Graphics aGraphics((*pDC));
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	if (m_ServerDraw->m_IsPick) {

		HatchBrush aBrush(
			HatchStyle25Percent,
			Color(
				GetRValue(g_ServerStateBkColor[m_ServerDraw->m_ServerState]), 
				GetGValue(g_ServerStateBkColor[m_ServerDraw->m_ServerState]), 
				GetBValue(g_ServerStateBkColor[m_ServerDraw->m_ServerState])
				),
//			Color::Black,
			Color::Transparent
			);

		aGraphics.FillRectangle(&aBrush, pStartX, pStartY, m_Width, m_Height);
	}
	else {
		CBrush aNewBrush;
		if (m_ServerDraw->m_ServerState == EV_SVS_CNNT)
		{
			if (m_ServerDraw->m_MaxFarmCount > 0)
				aNewBrush.CreateSolidBrush(RGB(157, 200, 51));
			else if (m_ServerDraw->m_WorldID[1])
				aNewBrush.CreateSolidBrush(RGB(50, 100, 51));
			else
				aNewBrush.CreateSolidBrush(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
		}
		else
			aNewBrush.CreateSolidBrush(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(pStartX, pStartY, pStartX + m_Width, pStartY + m_Height);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}
}

inline void CServerDrawBaseFilter::DrawServerID(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	COLORREF aOldBkColor = pDC->SetBkColor(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	COLORREF aOldTxColor = pDC->SetTextColor(g_ServerStateTxColor[m_ServerDraw->m_ServerState][(!m_ServerDraw->m_IsPick)?(0):(1)]);
	int aOldBkMode = pDC->SetBkMode(TRANSPARENT);
	
	CVarArg<MAX_PATH> aServerID(_T("%d"), m_ServerDraw->m_ServerID);

	CRect ServerRect (pStartX, pStartY, pStartX + m_Width, pStartY + m_Height);
	pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	pDC->SetBkColor(aOldBkColor);
	pDC->SetTextColor(aOldTxColor);
	pDC->SetBkMode(aOldBkMode);
}



// CServerDrawDervFilter

// Login

inline void CServerDrawDervFilter<EV_SVT_LO>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	DrawUserCount(pDC, pStartX, pStartY);
}

inline void CServerDrawDervFilter<EV_SVT_LO>::DrawServerID(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	COLORREF aOldBkColor = pDC->SetBkColor(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	COLORREF aOldTxColor = pDC->SetTextColor(g_ServerStateTxColor[m_ServerDraw->m_ServerState][(!m_ServerDraw->m_IsPick)?(0):(1)]);
//	int aOldBkMode = pDC->SetBkMode(OPAQUE);
	int aOldBkMode = pDC->SetBkMode(TRANSPARENT);
	
	CVarArg<MAX_PATH> aServerID(_T("%d"), m_ServerDraw->m_ServerID);
	CRect ServerRect (pStartX, pStartY + EV_SERVERDRAW_SERVERID_LO_PAD_Y, pStartX + m_Width, pStartY + m_Height);
	pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

	pDC->SetBkColor(aOldBkColor);
	pDC->SetTextColor(aOldTxColor);
	pDC->SetBkMode(aOldBkMode);
}

inline void CServerDrawDervFilter<EV_SVT_LO>::DrawUserCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	int aStartX = pStartX + EV_SERVERDRAW_USERCNT_LO_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_USERCNT_LO_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_USERCNT_LO_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_USERCNT_LO_HGT + EV_SERVERDRAW_USERCNT_LO_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState)
		return;
	
	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerUserCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerUserCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);
		CVarArg<MAX_PATH> aServerID(_T("USER : %d"), m_ServerDraw->m_CurUserCount);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}


// DB (Middleware)

inline void CServerDrawDervFilter<EV_SVT_DB>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	DrawDelayCount(pDC, pStartX, pStartY);
}	

inline void CServerDrawDervFilter<EV_SVT_DB>::DrawServerID(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	COLORREF aOldBkColor = pDC->SetBkColor(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	COLORREF aOldTxColor = pDC->SetTextColor(g_ServerStateTxColor[m_ServerDraw->m_ServerState][(!m_ServerDraw->m_IsPick)?(0):(1)]);
	int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

	CVarArg<MAX_PATH> aServerID(_T("%d"), m_ServerDraw->m_ServerID);
	CRect ServerRect (pStartX, pStartY + EV_SERVERDRAW_SERVERID_DB_PAD_Y, pStartX + m_Width, pStartY + m_Height);
	pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

	pDC->SetBkColor(aOldBkColor);
	pDC->SetTextColor(aOldTxColor);
	pDC->SetBkMode(aOldBkMode);
}

inline void CServerDrawDervFilter<EV_SVT_DB>::DrawDelayCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	if(!IsSet()) return;
	if(!pDC) return;


	int aStartX = pStartX + EV_SERVERDRAW_DELAYCNT_DB_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_DELAYCNT_DB_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_DELAYCNT_DB_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_DELAYCNT_DB_HGT + EV_SERVERDRAW_DELAYCNT_DB_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState && EV_SVS_HIDE != m_ServerDraw->m_ServerState)
		return;

	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerRoomCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerRoomCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

		CVarArg<MAX_PATH> aServerID(_T("DELAY : %d"), m_ServerDraw->m_FrameDelay[SERVERTYPE_DB]);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}

// Master

inline void CServerDrawDervFilter<EV_SVT_MA>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	
}


// Village

inline void CServerDrawDervFilter<EV_SVT_VI>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	DrawUserCount(pDC, pStartX, pStartY);

	
}

inline void CServerDrawDervFilter<EV_SVT_VI>::DrawServerID(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	COLORREF aOldBkColor = pDC->SetBkColor(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	COLORREF aOldTxColor = pDC->SetTextColor(g_ServerStateTxColor[m_ServerDraw->m_ServerState][(!m_ServerDraw->m_IsPick)?(0):(1)]);
//	int aOldBkMode = pDC->SetBkMode(OPAQUE);
	int aOldBkMode = pDC->SetBkMode(TRANSPARENT);
	
	CVarArg<MAX_PATH> aServerID(_T("%d"), m_ServerDraw->m_ServerID);
	CRect ServerRect (pStartX, pStartY + EV_SERVERDRAW_SERVERID_VI_PAD_Y, pStartX + m_Width, pStartY + m_Height);
	pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

	pDC->SetBkColor(aOldBkColor);
	pDC->SetTextColor(aOldTxColor);
	pDC->SetBkMode(aOldBkMode);
}

inline void CServerDrawDervFilter<EV_SVT_VI>::DrawUserCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	int aStartX = pStartX + EV_SERVERDRAW_USERCNT_VI_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_USERCNT_VI_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_USERCNT_VI_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_USERCNT_VI_HGT + EV_SERVERDRAW_USERCNT_VI_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState)
		return;
	
	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerUserCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerUserCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);
		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

		CVarArg<MAX_PATH> aServerID(_T("USER : %d"), m_ServerDraw->m_CurUserCount);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}


// Game

inline void CServerDrawDervFilter<EV_SVT_GA>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	DrawUserCount(pDC, pStartX, pStartY);

	DrawRoomCount(pDC, pStartX, pStartY);

	DrawDelayCount(pDC, pStartX, pStartY);
}

inline void CServerDrawDervFilter<EV_SVT_GA>::DrawServerID(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	COLORREF aOldBkColor = pDC->SetBkColor(g_ServerStateBkColor[m_ServerDraw->m_ServerState]);
	COLORREF aOldTxColor = pDC->SetTextColor(g_ServerStateTxColor[m_ServerDraw->m_ServerState][(!m_ServerDraw->m_IsPick)?(0):(1)]);
//	int aOldBkMode = pDC->SetBkMode(OPAQUE);
	int aOldBkMode = pDC->SetBkMode(TRANSPARENT);
	
	CVarArg<MAX_PATH> aServerID(_T("%d"), m_ServerDraw->m_ServerID);
	CRect ServerRect (pStartX, pStartY + EV_SERVERDRAW_SERVERID_GA_PAD_Y, pStartX + m_Width, pStartY + m_Height);
	pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

	pDC->SetBkColor(aOldBkColor);
	pDC->SetTextColor(aOldTxColor);
	pDC->SetBkMode(aOldBkMode);
}

inline void CServerDrawDervFilter<EV_SVT_GA>::DrawUserCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	int aStartX = pStartX + EV_SERVERDRAW_USERCNT_GA_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_USERCNT_GA_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_USERCNT_GA_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_USERCNT_GA_HGT + EV_SERVERDRAW_USERCNT_GA_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState &&EV_SVS_HIDE != m_ServerDraw->m_ServerState)
		return;
	
	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerUserCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerUserCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

		CVarArg<MAX_PATH> aServerID(_T("USER : %d"), m_ServerDraw->m_CurUserCount);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}

inline void CServerDrawDervFilter<EV_SVT_GA>::DrawRoomCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	if(!IsSet()) return;
	if(!pDC) return;

	int aStartX = pStartX + EV_SERVERDRAW_ROOMCNT_GA_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_ROOMCNT_GA_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_ROOMCNT_GA_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_ROOMCNT_GA_HGT + EV_SERVERDRAW_ROOMCNT_GA_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState && EV_SVS_HIDE != m_ServerDraw->m_ServerState)
		return;
	
	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerRoomCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerRoomCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

		CVarArg<MAX_PATH> aServerID(_T("ROOM : %d"), m_ServerDraw->m_CurRoomCount);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}

inline void CServerDrawDervFilter<EV_SVT_GA>::DrawDelayCount(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	if(!IsSet()) return;
	if(!pDC) return;


	int aStartX = pStartX + EV_SERVERDRAW_DELAYCNT_GA_GAP_X;
	int aStartY = pStartY + EV_SERVERDRAW_DELAYCNT_GA_PAD_Y;
	int aEndX = pStartX + m_Width - EV_SERVERDRAW_DELAYCNT_GA_GAP_X;
	int aEndY = pStartY + EV_SERVERDRAW_DELAYCNT_GA_HGT + EV_SERVERDRAW_DELAYCNT_GA_PAD_Y;

	{
		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(RGB(255, 255, 255));
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX, aStartY, aEndX, aEndY);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();
	}

	if (EV_SVS_CNNT != m_ServerDraw->m_ServerState && EV_SVS_HIDE != m_ServerDraw->m_ServerState)
		return;

	{
		CPen aNewPen;
		aNewPen.CreatePen(PS_SOLID, 1, g_ServerRoomCountColor);
		CPen* pOldPen = pDC->SelectObject(&aNewPen);

		CBrush aNewBrush;
		aNewBrush.CreateSolidBrush(g_ServerRoomCountColor);
		CBrush* pOldBrush = pDC->SelectObject(&aNewBrush);

		pDC->Rectangle(aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);

		pDC->SelectObject(pOldBrush);
		aNewBrush.DeleteObject();

		pDC->SelectObject(pOldPen);
		aNewPen.DeleteObject();
	}

	{
		int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

		CVarArg<MAX_PATH> aServerID(_T("DELAY : %d"), m_ServerDraw->m_FrameDelay[SERVERTYPE_GAME]);
		CRect ServerRect (aStartX + 1, aStartY + 1, aEndX - 1, aEndY - 1);
		pDC->DrawText(aServerID.GetMsg(), &ServerRect, DT_SINGLELINE | DT_CENTER);

		pDC->SetBkMode(aOldBkMode);
	}
}

// Log
inline void CServerDrawDervFilter<EV_SVT_LG>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	
}


// Cash

inline void CServerDrawDervFilter<EV_SVT_CA>::Draw(CDC* pDC, int pStartX, int pStartY)
{
	if(!IsSet()) return;
	if(!pDC) return;

	DrawFrame(pDC, pStartX, pStartY);

	DrawServerID(pDC, pStartX, pStartY);

	
}

