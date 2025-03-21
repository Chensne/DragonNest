#pragma once

class CDnSmartMoveCursor
{
public:
	CDnSmartMoveCursor();

protected:
	CEtUIControl *m_pControl;
	CEtUIDialog *m_pParentDlg;

	// 이동할 위치
	float m_fMovePosX;
	float m_fMovePosY;

	// 기억해둘 이전 위치
	float m_fPrePosX;
	float m_fPrePosY;

public:
	void SetControl( CEtUIControl* pControl );
	void MoveCursor();
	void ReturnCursor();
};

// 메인메뉴같이 커서모드 아닌 상태에서 커서모드가 될때는 위 스마트무브가 제대로 작동하지 않는다.
// 이유를 알아보니
// 커서가 하이드 된 상태에서 커서이동은 당연히 안되고,
// GetEtDevice()->ShowCursor( true );를 호출하더라도, 그 해당 프레임에선 커서이동이 안된다.
// 즉, 해당 프레임이 Present되고 난 다음 프레임부터 실제로 커서가 보이게 되고 이때 커서이동이 된다는 것이다.
//
// 그래서 Process함수에서 다음번 프레임에 커서를 이동시키기로 해 함수가 위와 달라지게 되었다.
class CDnSmartMoveCursorEx
{
public:
	void SetControl( CEtUIControl* pControl );
	void MoveCursor();
	void Process();

	CDnSmartMoveCursorEx();

protected:
	CEtUIControl *m_pControl;
	CEtUIDialog *m_pParentDlg;
	bool m_bReadyMove;
};