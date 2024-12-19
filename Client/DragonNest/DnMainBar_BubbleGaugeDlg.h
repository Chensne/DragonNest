#pragma once

#include "EtUIDialog.h"

class CDnMainBar_BubbleGaugeDlg : public CEtUIDialog
{
public:
	CDnMainBar_BubbleGaugeDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMainBar_BubbleGaugeDlg(void);

protected:
	CEtUIStatic *m_pBubbleCount;//ID_TEXT_COUNT 
	CEtUIProgressBar *m_pProgressBarCount;//ID_PROGRESSBAR_COUNT 
	CEtUIProgressBar *m_pProgressBarTime;//ID_PROGRESSBAR_TIME
	int m_nMaxBubbles;

	int m_nBubbleCount;
	float m_fExpireTime;
public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void Render(float fElapsedTime);
	void SetBubble(int Count, float fExpireTime, float fTotalTime);
	void ResetBubbles();
	bool IsValidClass(int ClassID);

	void UpdateBubble(int nBubble) { m_nBubbleCount = nBubble; };
	void UpdateExpireTime(int fExpire) { m_fExpireTime = fExpire; };
	int GetBubbleCount() { return m_nBubbleCount; };
	float GetExpireTime() { return m_fExpireTime; };

	void SetMaxBubbles(int bubbles) { m_nMaxBubbles = bubbles; }
};
