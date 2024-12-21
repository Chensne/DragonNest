#pragma once

class CDnCalcCatmullRom
{
public:
	CDnCalcCatmullRom() {} 
	virtual ~CDnCalcCatmullRom(){}

protected:
	LOCAL_TIME m_nRemainTime;
	LOCAL_TIME m_nLastTime;
	LOCAL_TIME m_nLocalTime;

	EtVector3 m_v0;
	EtVector3 m_v1;
	EtVector3 m_v2;
	EtVector3 m_v3;
	EtVector3 m_vCurPos;
	EtVector3 m_vPrevPos;
	EtVector3 m_vCurDir;
	EtVector3 m_vPrevDir;

	float m_fLastMoveValue;

public:
	void Init ( EtVector3& v0, EtVector3& v1, EtVector3& v2, EtVector3& v3, LOCAL_TIME nRemainTime, LOCAL_TIME nCurTime );
	void GetInfo( OUT EtVector3& v0, OUT EtVector3& v1, OUT EtVector3& v2, OUT EtVector3& v3, OUT LOCAL_TIME& nRemainTime, OUT LOCAL_TIME& nCurTime );

	// fMin~fMax 범위값을 기준으로 0.0 ~ 1.0 값을 뽑아준다.
	// CalcWeight(0.34 , 0.67 , 0.34) ==> 0.0
	// CalcWeight(0.34 , 0.67 , 0.67) ==> 1.0
	inline float CalcWeight(float fMin, float fMax, float fValue );
	void Process( LOCAL_TIME nTime );
	inline EtVector3 GetPos() { return m_vCurPos; }
	inline EtVector3 GetPrevPos() { return m_vPrevPos; }
	inline EtVector3 GetCurDir() { return m_vCurDir; }
	inline EtVector3 GetPrevDir() { return m_vPrevDir; }
	float GetWeight();
};



class CDnHoming
{
public:
	CDnHoming();
	virtual ~CDnHoming();

	void Init( LOCAL_TIME RemainTime, LOCAL_TIME CurTime, EtVector3 &StartVector, EtVector3 &EndVector, EtVector3 &ViewVec );
	void SetEndPos( EtVector3 &vVector ) { m_EndVec = vVector; }
	void Process( LOCAL_TIME LocalTime ) { m_LocalTime = LocalTime; }

	EtVector3 GetPos();
	EtVector3 GetVVec();
	float GetWeight();

private:
	EtVector3 m_StartVec;
	EtVector3 m_EndVec;
	EtVector3 m_ViewVec;

	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_LifeTime;
	LOCAL_TIME m_CreateTime;
};
