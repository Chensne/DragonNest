#pragma once
#include <wx/wx.h>
#include "EternityEngine.h"
#include "ipropertydelegate.h"



class CActorProperty : public IPropertyDelegate
{
private:
	enum
	{
		ACTOR_NAME,
		ACTOR_RES_NAME,			// ���� ������ ���� ���� ����Ʈ
		POSITION,
		//POS_X,
		//POS_Y,
		//POS_Z,
		FIT_YPOS_TO_MAP,
		ROTATION,
		INFLUENCE_LIGHTMAP,		// ����Ʈ �� ������ �޴��� �ȹ޴���. (actor �� �⺻������ �ް� �Ǿ�����)
		
		SCALE_LOCK,				// ������ ���� �ɷ������� ������ �⺻ ������(1.0)���� Ŭ���̾�Ʈ���� ����ϵ��� �Ѵ�.

		// ���������� ���� ������ ������. ���� ���͵��� ��������� �ִ� ��� ���� ���� ���̺� ID �� ����ϸ鼭 ������ ���⿡ ������ ���� �ٸ��Ƿ� ������
		// �ش� �����Ϸ� �������ְ� Ŭ���̾�Ʈ������ ������ ���� �� �� ������ �� �� �ֵ��� ó���Ѵ�.
		// ������ ���� ���� ���� ��쿡�� ���� �����ϴ�.
		SELECT_BOSS,
		COUNT,
	};

	wxPGId		m_aPGID[ COUNT ];

	// ������Ʈ ���� �����͸� ���� �ִ´�.
	wxString		m_strPrevActorName;
	EtVector3		m_vPrevPos;
	float			m_fPrevRotation;
	bool			m_bPrevInfluenceLightmap;
	bool			m_bPrevScaleLock;

private:
	void _RebuildProperty( void );
	void _UpdateProp( void );


public:
	CActorProperty( wxWindow* pParent, int id );
	virtual ~CActorProperty(void);

	// from IPropertyDelegate
	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
	//
};
