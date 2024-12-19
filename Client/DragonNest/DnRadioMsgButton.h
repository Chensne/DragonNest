#pragma once
#include "DnSlotButton.h"

class CDnRadioMsgButton : public CDnSlotButton
{
public:
	CDnRadioMsgButton( CEtUIDialog *pParent );
	virtual ~CDnRadioMsgButton(void);

protected:
	//SUICoord m_ShortCutUV;
	//SUICoord m_ShortCutCoord;

	// 부모와 결합도 떨어뜨리기 위해 별도 변수 쓰겠다.
	EtTextureHandle m_hRadioMsgIcon;
	EtTextureHandle m_hRadioMsgIconList[RADIOMSG_TEXTURE_COUNT];

public:
	// 일반적인 SlotButton 이라면, MIInventoryItem의 파생 객체 하나 만들고 SetItem 해서 사용해야한다.
	// 그러나 이 방법은 클래스 결합도가 계속 높아지는 거 같아 별로다.
	// 어차피 라디오메세지 객체의 경우 정보가 몇개 없기때문에, 직접 받는 형태로 하겠다.
	// 그래서 m_pItem멤버를 사용하지 않는다.
	void SetRadioMsgInfo( int nRadioMsgID );

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void RenderDrag( float fElapsedTime );
};