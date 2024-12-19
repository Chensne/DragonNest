#include "stdafx.h"
#include "ResourceInit.h"
#include "../DnControlData.h"
#include "../DnDefine_Launcher.h"

#define MAX_CTRL_NUM	22
CRect g_CtrlRect[MAX_CTRL_NUM];


void InitCtrlRect()
{
	DNCTRLDATA.SetControlDataFromResource(17001 /*IDR_XML_CONTROL_DATA*/, PATCH_CONTROL_NAME, g_CtrlRect, MAX_CTRL_NUM);

	/*
	g_CtrlRect[0].SetRect( 12, 44, 678, 410 );		// WebPageHtml Fit( 7, 39, 683, 414 )
	g_CtrlRect[1].SetRect( 500, 493, 664, 533 );	// BtnQuit
	g_CtrlRect[2].SetRect( 500, 493, 664, 533 );	// BtnStartGame
	g_CtrlRect[3].SetRect( 620, 549, 669, 573 );	// BtnOption
	g_CtrlRect[4].SetRect( 660, 10, 681, 31 );		// BtnWinClose
	g_CtrlRect[5].SetRect( 636, 10, 657, 31 );		// BtnWinMiniMum
	g_CtrlRect[6].SetRect( 88, 519, 462, 525 );		// ProgressDownload
	g_CtrlRect[7].SetRect( 88, 456, 605, 462 );		// ProgressCount
	g_CtrlRect[8].SetRect( 170, 18, 240, 33 );		// Text_LauncherVersion
	g_CtrlRect[9].SetRect( 0, 0, 0, 0 );			// Text_FileCount
	g_CtrlRect[10].SetRect( 90, 495, 500, 510 );	// Text_FileName
	g_CtrlRect[11].SetRect( 552, 431, 690, 446 );	// Text_TimeLeft
	g_CtrlRect[12].SetRect( 425, 431, 480, 446 );	// Text_FileSize
	g_CtrlRect[13].SetRect( 630, 452, 690, 467 );	// Text_DownloadPercentage
	g_CtrlRect[14].SetRect( 85, 430, 320, 445 );	// Text_DownloadState
	g_CtrlRect[15].SetRect( 397, 556, 660, 571 );	// Text_DirectStart
	g_CtrlRect[16].SetRect( 380, 555, 393, 568 );	// CheckBox_DIrectStart
	g_CtrlRect[17].SetRect( 24, 430, 233, 445 );	// Static_Text_CurrentInfo
	g_CtrlRect[18].SetRect( 24, 495, 88, 510 );		// Static_Text_FileName
	g_CtrlRect[19].SetRect( 350, 431, 425, 446 );	// Static_Text_FileSize
	g_CtrlRect[20].SetRect( 480, 431, 550, 446 );	// Static_Text_TimeLeft
	g_CtrlRect[21].SetRect( 530, 15, 636, 36 );		// Static_Text_HomepageLink
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
