#pragma once

namespace BubbleSystem
{

//------------------------------------------------------------------------------
/**
    버블 하나~
*/
//------------------------------------------------------------------------------
class CDnBubble
{
private:
	int m_iTypeID;
	float m_fDurationTime;
	float m_fRemainTime;
	int m_iIconIndex;

public:
	CDnBubble( void );
	virtual ~CDnBubble( void );

	void SetTypeID( int iType ) { m_iTypeID = iType; };
	int GetTypeID( void ) { return m_iTypeID; };
	void SetDurationTime( float fDurationTime ) { m_fDurationTime = fDurationTime; m_fRemainTime = fDurationTime; };
	float GetDurationTime( void ) { return m_fDurationTime; };
	float GetRemainTime( void ) { return m_fRemainTime; };
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsEnd( void );
	void SetIconIndex( int iIconIndex ) { m_iIconIndex = iIconIndex; };
	int GetIconIndex( void ) { return m_iIconIndex; };
};

} // namespace BubbleSystem