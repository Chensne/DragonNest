#include "stdafx.h"
#include "ResourceInit.h"
#include "../DnControlData.h"
#include "../DnDefine_Launcher.h"

#define MAX_CTRL_NUM	17
CRect g_CtrlRect[MAX_CTRL_NUM];

void InitCtrlRect()
{
	DNCTRLDATA.SetControlDataFromResource(17001 /*IDR_XML_CONTROL_DATA*/, PATCH_CONTROL_NAME, g_CtrlRect, MAX_CTRL_NUM);
	
	/*
	g_CtrlRect[0].SetRect( 28, 30, 670, 430 );		// WebPageHtml
	g_CtrlRect[1].SetRect( 587, 485, 674, 511 );	// BtnQuit
	g_CtrlRect[2].SetRect( 586, 453, 673, 498 );	// BtnStartGame
	g_CtrlRect[3].SetRect( 586, 498, 673, 524 );	// BtnOption
	g_CtrlRect[4].SetRect( 674, 4, 697, 27 );		// BtnWinClose
	g_CtrlRect[5].SetRect( 650, 4, 673, 27 );		// BtnWinMiniMum
	g_CtrlRect[6].SetRect( 116, 478, 520, 491 );	// ProgressDownload
	g_CtrlRect[7].SetRect( 116, 502, 520, 515 );	// ProgressCount
	g_CtrlRect[8].SetRect( 28, 10, 178, 25 );		// Text_LauncherVersion
	g_CtrlRect[9].SetRect( 530, 503, 680, 518 );	// Text_FileCount
	g_CtrlRect[10].SetRect( 33, 455, 233, 470 );	// Text_FileName
	g_CtrlRect[11].SetRect( 370, 455, 520, 470 );	// Text_TimeLeft
	g_CtrlRect[12].SetRect( 230, 455, 371, 470 );	// Text_FileSize
	g_CtrlRect[13].SetRect( 530, 479, 570, 494 );	// Text_DownloadPercentage
	g_CtrlRect[14].SetRect( 130, 520, 430, 535 );	// Text_DownloadState
	g_CtrlRect[15].SetRect( 130, 537, 430, 552 );	// Text_DirectStart
	g_CtrlRect[16].SetRect( 113, 537, 126, 550 );	// CheckBox_DIrectStart
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
