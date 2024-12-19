#include "stdafx.h"
#include "ResourceInit.h"
#include "../DnControlData.h"
#include "../DnDefine_Launcher.h"



#define MAX_CTRL_NUM	18
CRect g_CtrlRect[MAX_CTRL_NUM];

void InitCtrlRect()
{
	DNCTRLDATA.SetControlDataFromResource(17001 /*IDR_XML_CONTROL_DATA*/, PATCH_CONTROL_NAME, g_CtrlRect, MAX_CTRL_NUM);

	/*
	g_CtrlRect[0].SetRect( 12, 44, 678, 410 );		// WebPageHtml
	g_CtrlRect[1].SetRect( 390, 505, 516, 536 );	// BtnQuit
	g_CtrlRect[2].SetRect( 390, 505, 516, 536 );	// BtnStartGame
	g_CtrlRect[3].SetRect( 325, 512, 374, 536 );	// BtnOption
	g_CtrlRect[4].SetRect( 659, 10, 682, 33 );		// BtnWinClose
	g_CtrlRect[5].SetRect( 635, 10, 658, 33 );		// BtnWinMiniMum
	g_CtrlRect[6].SetRect( 116, 448, 524, 458 );	// ProgressDownload
	g_CtrlRect[7].SetRect( 116, 475, 524, 485 );	// ProgressCount
	g_CtrlRect[8].SetRect( 172, 15, 240, 30 );		// Text_LauncherVersion
	g_CtrlRect[9].SetRect( 535, 472, 580, 487 );	// Text_FileCount
	g_CtrlRect[10].SetRect( 13, 422, 213, 437 );	// Text_FileName
	g_CtrlRect[11].SetRect( 331, 422, 550, 437 );	// Text_TimeLeft
	g_CtrlRect[12].SetRect( 210, 422, 330, 437 );	// Text_FileSize
	g_CtrlRect[13].SetRect( 535, 445, 580, 460 );	// Text_DownloadPercentage
	g_CtrlRect[14].SetRect( 120, 493, 430, 508 );	// Text_DownloadState
	g_CtrlRect[15].SetRect( 137, 518, 297, 533 );	// Text_DirectStart
	g_CtrlRect[16].SetRect( 120, 520, 133, 533 );	// CheckBox_DIrectStart
	g_CtrlRect[17].SetRect( 500, 15, 630, 30 );		// Static_Text_HomepageLink
	*/
}

void GetCtrlRect( CRect* pRect, int nMaxNum )
{
	if( pRect == NULL || nMaxNum != MAX_CTRL_NUM )
		return;

	for( int i=0; i<nMaxNum; i++ )
	{
		pRect[i] = g_CtrlRect[i];
	}
}
