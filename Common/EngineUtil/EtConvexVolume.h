#pragma once

class CEtConvexVolume
{
public:
	CEtConvexVolume();
	virtual ~CEtConvexVolume();

protected:

	DNVector(EtVector4) m_Planes;

public:
	void Initialize( EtMatrix &ViewProjMat );
	bool TesToBox( EtVector3 &Origin, EtVector3 &Extent );

};
