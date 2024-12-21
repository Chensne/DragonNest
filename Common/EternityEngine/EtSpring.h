#pragma once
#include <map>

class CEtMassPoint
{	
public:
	D3DXVECTOR3		Pos;		
	D3DXVECTOR3		Vel;
	D3DXVECTOR3		PrevVel;
	D3DXVECTOR3		gravity;

	int					nSpringCount;
	CEtMassPoint		*nodes[32];
	float					NaturalLength[32];
	float					fPower[32];
	bool					bLocked;
	bool					bForceLimit;
	DWORD				lastForceTime;

public:
	CEtMassPoint() {
		Vel = D3DXVECTOR3(0,0,0);
		PrevVel = D3DXVECTOR3(0,0,0);
		gravity = D3DXVECTOR3(0,0,0);
		Pos = D3DXVECTOR3(0,0,0);
		nSpringCount = 0;
		bLocked = false;
		bForceLimit = false;
		lastForceTime = 0;
		memset(NaturalLength, 0, sizeof(NaturalLength));
		memset(fPower, 0, sizeof(fPower));		
	}

public:
	void AddSpring(CEtMassPoint *node, float power ) {
		nodes[nSpringCount] = node;
		NaturalLength[nSpringCount] = D3DXVec3Length( &(Pos -  node->Pos) );
		fPower[nSpringCount] = power;
		nSpringCount++;
	}
	void AddSpringBoth(CEtMassPoint *node, float power ) {
		AddSpring( node, power);
		node->AddSpring( this, power );
	}
	void SetLocked( bool isLocked) {
		bLocked = isLocked;
	}
	void SetGravity( EtVector3 gravity_ ) { gravity = gravity_;}
	void Simulate();
};

class CEtSpring
{
public:
	CEtSpring();
	~CEtSpring(){}

private:
	std::vector< CEtMassPoint > m_Masses;
	bool	m_bEnable;
	float	m_fStep;

public:

	bool IsEnable() { return m_bEnable; }
	void Disable() { m_bEnable = false; }
	void Create( const char *szFileName, const std::vector< EtMatrix > &InvWorldMatList, std::map<std::string, int> &boneIndexMap );	
	void InputKeyframe( EtMatrix WorldMat, int nSaveMatIndex );
	void Simulate( EtMatrix WorldMat, const std::vector< EtMatrix > &InvWorldMatList  );
	EtVector3* GetPosition( int nIndex );
	EtMatrix GetMatrix( int nIndex, int nChildOffset);

};
