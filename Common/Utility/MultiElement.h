#pragma once

class CMultiRoom;

class CMultiElement {
public:
	CMultiElement() { m_pBaseRoom = NULL; }
	CMultiElement( CMultiRoom *pRoom ) { Initialize( pRoom ); }
	virtual ~CMultiElement() {}

protected:
	CMultiRoom *m_pBaseRoom;

public:
	void Initialize( CMultiRoom *pRoom ) { m_pBaseRoom = pRoom; }

	CMultiRoom *GetRoom() const { return m_pBaseRoom; }
};
