#pragma once

class CDnLoadingButton : public CEtUIButton
{
public:
	CDnLoadingButton(CEtUIDialog *pParent);
	virtual ~CDnLoadingButton(void);

	virtual void Initialize(SUIControlProperty *pProperty);

	void SetAnimation(bool bSet);
	void SetAnimation(bool bSet, float sec);

	void IncreseAnimationSpeed(float incSecForOneRound);
	void DecreaseAnimationSpeed(float decSecForOneRound);

	bool IsAnimating() const { return m_bAnimation; }

protected:
	virtual void Render( float fElapsedTime );

private:
	int		m_FrameCount;
	float	m_Interval;
	float	m_AnimationTimer;
	int		m_CurrentElemIdx;
	bool	m_bAnimation;
};
