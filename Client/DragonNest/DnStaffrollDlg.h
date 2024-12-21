#pragma once
#include "EtUIDialog.h"
#include "DnLoginTask.h"
#include "DnStaffrollFontBase.h"

#define FACE_TEXTURE_ICON_COUNT 64
#define FACE_TEXTURE_ICON_XSIZE 64
#define FACE_TEXTURE_ICON_YSIZE 64
class CDnStaffrollDlg : public CEtUIDialog
{
public:
	CDnStaffrollDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStaffrollDlg(void);

protected:
	enum ElementTypeEnum {
		Space,
		Face,
		Font1,
		Font2,
		Font3,
	};

	struct ElementStruct {
		ElementTypeEnum Type;
		std::string szValue;
		float fOffset;
		float fHeight;

		virtual void Update( bool bShow, float fDelta ) {}
		virtual ~ElementStruct() {}
	};

	struct FaceElementStruct : ElementStruct {
		SUICoord Coord;
		SUICoord UVCoord;
		EtTextureHandle hTexture;

		virtual void Update( bool bShow, float fDelta ) 
		{
			if( bShow ) {
				Coord.fY = 1.f - ( fDelta - fOffset );

				float fAlpha = 1.f;
				if( Coord.fY + fHeight > 1.f ) fAlpha = 1.f - ( ( 1.f / fHeight ) * ( Coord.fY + fHeight - 1.f ) );
				else if( Coord.fY < 0.f ) fAlpha = ( 1.f / fHeight ) * ( Coord.fY + fHeight );

				CEtSprite::GetInstance().DrawSprite( (EtTexture*)hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), UVCoord, D3DCOLOR_ARGB( (BYTE)(fAlpha * 255), 255, 255, 255 ), Coord, 0.f );
			}
		}
		void SetTexture( EtTextureHandle hTex, SUICoord uvCoord ) 
		{
			hTexture = hTex;
			UVCoord = uvCoord;

			/*
			UVCoord.fX = ( 1.f / 8.f ) * ( nIndex % 8 );
			UVCoord.fY = ( 1.f / 8.f ) * ( nIndex / 8 );
			UVCoord.fWidth = UVCoord.fHeight = 1.f / 8.f;
			*/
		}
	};
	struct FontElementStruct : ElementStruct {
		CEtUIDialog *pFontDlg;

		virtual void Update( bool bShow, float fDelta ) 
		{
			pFontDlg->Show( bShow );

			if( bShow ) {
				SUICoord Coord;
				pFontDlg->GetDlgCoord( Coord );
				Coord.fY /= pFontDlg->GetScreenHeightRatio();
				Coord.fY = 1.f - ( fDelta - fOffset );
				float fAlpha = 1.f;
				if( Coord.fY + fHeight > 1.f ) fAlpha = 1.f - ( ( 1.f / fHeight ) * ( Coord.fY + fHeight - 1.f ) );
				else if( Coord.fY < 0.f ) fAlpha = ( 1.f / fHeight ) * ( Coord.fY + fHeight );
				Coord.fY *= pFontDlg->GetScreenHeightRatio();
				pFontDlg->SetDlgCoord( Coord );

				CDnStaffrollFontBase *pFontBase = dynamic_cast<CDnStaffrollFontBase *>(pFontDlg);
				if( pFontBase ) pFontBase->SetAlpha( fAlpha );
			}
		}
		virtual ~FontElementStruct()
		{
			SAFE_DELETE( pFontDlg );
		}
	};


protected:
	bool m_bLoaded;
	CDnLoginTask::LoginStateEnum m_LastLoginState;

	bool m_bPlay;
	float m_fDelta;
	float m_fTotalDelta;
	std::vector<ElementStruct *> m_pVecElementList;
	std::vector<EtTextureHandle> m_hVecFaceTextureList;

	CEtUIButton *m_pButtonStaffroll;

protected:
	void Play();
	void Stop();
	void ResetStaffrollDelta();

	wstring GetParseString( const char *szStr );
	void CalcFontElement( CEtUIDialog *pDlg, ElementStruct *pStruct );
	EtTextureHandle GetFaceTexture( int nIconIndex, SUICoord &UVCoord );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

	void LoadStaffrollData();
	void FreeStaffrollData();
};
