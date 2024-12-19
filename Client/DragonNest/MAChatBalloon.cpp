#include "StdAfx.h"
#include "MAChatBalloon.h"
#include "DnActor.h"
#include "DnChatOption.h"
#include "EtUINameLinkMgr.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnTableDB.h"

int MAChatBalloon::m_siTotalBalloon = 0;
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


MAChatBalloon::MAChatBalloon(void)
{
	m_hChatBalloonTexture[0] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloon.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTailTexture[0] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTail.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTexture[1] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonParty.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTailTexture[1] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTailParty.dds" ).c_str(), RT_TEXTURE );

	m_hChatBalloonTexture[2] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonRoom.dds" ).c_str(), RT_TEXTURE );	// 같은 텍스처로 통일.
	m_hChatBalloonTailTexture[2] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTailRoom.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTexture[3] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonRoom.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTailTexture[3] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTailRoom.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTexture[4] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonRoom.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTailTexture[4] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTailRoom.dds" ).c_str(), RT_TEXTURE );

	m_hChatBalloonTexture[5] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonRepute.dds" ).c_str(), RT_TEXTURE );
	m_hChatBalloonTailTexture[5] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ChatBalloonTailRepute.dds" ).c_str(), RT_TEXTURE );

	m_dwTalkStartTime = 0;
	m_fTalkWidth = 0.f;
	m_fTalkTailPos = 0.f;	
	m_fTalkTailPosPrev = 0.f;
	m_nChatType = CHAT_NORMAL;
	m_dwDuringTime = DEFAULT_TIME;
	m_Dialog = NULL;
	m_bConutBalloon = false;
	m_bEnableCustomChatBalloon = false;
	m_fTalkWidthForWordBreak = 0.0f;

	m_bForceShow = false;
	m_nCustomTextIndex = 0;
	m_fCustomTextDelayTime = 0.0f;

	m_fAdditionalYOffset = 0.0f;
}

MAChatBalloon::~MAChatBalloon(void)
{
	for( int i = 0; i < NUM_CHATBALLOON_TEX; ++i )
	{
		SAFE_RELEASE_SPTR( m_hChatBalloonTexture[i] );
		SAFE_RELEASE_SPTR( m_hChatBalloonTailTexture[i] );
	}

	SAFE_DELETE(m_Dialog);

	if( m_bConutBalloon )
		DecreaseBalloon();
}

static DWORD TEXT_WIDTH = 250;
void MAChatBalloon::RenderChatBalloon( EtVector3 vPos, EtVector3 vHeadPos, float fElapsedTime )
{
	static BYTE TEXT_ALPHA = 255;	// 텍스처의 알파를 사용하면서 기본 버텍스는 255로..
	static bool CENTER_ALIGN = true;
	static float LINE_PIXEL = 0.025f;	
	float TALK_HEIGHT = 0.05f + m_fAdditionalYOffset;
	
	//몬스터들의 다양한 말풍선
	if( m_Dialog && m_Dialog->IsShow() )
	{
		if( GetTickCount() < m_dwDuringTime + m_dwTalkStartTime || m_bForceShow )
		{
			ProcessCustomChatBalloonText( fElapsedTime );
			if( !CheckDistance( vPos, m_bForceShow ) ) return;	// m_bForceShow 값이 true면 채팅방으로 판단한다.

			EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
			if( !hCamera ) return;

			EtViewPort vp;
			EtVector3 vSpaceToClient;
			EtVector2 vPosition;
			EtMatrix * matView, * matProj, matViewProj;
			float heightRatio = 1.0f;

			GetEtDevice()->GetViewport(&vp);
			matView = hCamera->GetViewMat();
			matProj = hCamera->GetProjMat();

			matViewProj = (*matView) * (*matProj);

			//현재 넓이를 구하고 절반 만큼 옆으로 옮겨서 가운데 정렬한다.
			//현재 높이를 구하고 그만큼 올려준다.
			//넓이가 감소하는것은 처리하지 않는다.
			//넓이가 증가한다면 다이알로그 박스의 길이도 증가 시키고 그만큼의 절반을 옆으로 옮겨서 가운데 정렬한다.
			//높이가 증가한다면 다이알로그 박스의 길이도 증가 시키고 그만큼의 높이를 올려준다.

			//기본 좌표 값
			EtVec3TransformCoord( &vSpaceToClient, &vPos, &matViewProj );
			vPosition.x = ( vSpaceToClient.x + 1.0f ) / 2.0f + vp.X;
			vPosition.y = ( 2.0f - ( vSpaceToClient.y + 1.0f ) ) / 2.0f + vp.Y;

			vPosition.x *= m_Dialog->GetScreenWidthRatio();
			vPosition.y *= m_Dialog->GetScreenHeightRatio();

			vPosition.x	-= m_Dialog->Width() / 2.0f;
			vPosition.y -= m_Dialog->Height() + TALK_HEIGHT;

			//넓이, 높이가 증가하는지 검사
			/*if( 1 != m_wszTalkText.size() )
			{
				heightRatio += m_wszTalkText.size() * 0.025f;
				vPosition.y -= m_wszTalkText.size() * TALK_HEIGHT;
			}*/

			m_Dialog->SetPosition(vPosition.x, vPosition.y);
			m_Dialog->SetDepth(vSpaceToClient.z);

			m_Dialog->Render(fElapsedTime);
		}
		else
		{
			m_Dialog->Show(false);
			if( m_bConutBalloon )
				DecreaseBalloon();
		}
		return;
	}

	// ChatBalloon (이전 Default 말풍선)
	if( GetTickCount() < m_dwDuringTime + m_dwTalkStartTime || m_bForceShow )  {

		if( !CheckDistance( vPos, m_bForceShow ) ) return;

		int nTextureIndex = 0;
		switch( m_nChatType )
		{
		case CHAT_NORMAL:		nTextureIndex = 0; break;
		case CHAT_PARTY:		nTextureIndex = 1; break;
		case CHAT_CHATROOM1:	nTextureIndex = 2; break;
		case CHAT_CHATROOM2:	nTextureIndex = 3; break;
		case CHAT_CHATROOM3:	nTextureIndex = 4; break;
		case CHAT_REPUTE:		nTextureIndex = 5; break;
		}
		EtTextureHandle hChatBalloonTexture = m_hChatBalloonTexture[nTextureIndex];
		EtTextureHandle hChatBalloonTailTexture = m_hChatBalloonTailTexture[nTextureIndex];

		if( hChatBalloonTexture && hChatBalloonTailTexture ) {
			DWORD dwAlpha = TEXT_ALPHA;
			float fDuration = (float) (GetTickCount() - m_dwTalkStartTime) / m_dwDuringTime;
			/*if( fDuration > 0.7f ) {
				if( fDuration < 0.75f ) {
					dwAlpha = (2 * TEXT_ALPHA / 3) + (int)((TEXT_ALPHA / 3) * (1.f-20.f*(fDuration-0.7f)));
				}
				else if( fDuration < 0.95f ) {
					dwAlpha = 2 * TEXT_ALPHA / 3;
				}
				else {
					dwAlpha = (int)((2 * TEXT_ALPHA / 3) * (1.f - 20.f * (fDuration-0.95f)));
				}
			}*/
			if( fDuration > 0.975f ) {
				dwAlpha = (DWORD)(TEXT_ALPHA * (1.0f - (fDuration - 0.975f)*40.f));
			}

			dwAlpha = (DWORD)TEXT_ALPHA;
			
			vPos.x += 10.f;

			EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
			if( !hCamera ) return;

			EtMatrix *pViewMat = hCamera->GetViewMat();
			EtMatrix *pProjMat = hCamera->GetProjMat();
			EtVector3 vProjPos, vHeadProjPos;

			EtVec3TransformCoord( &vProjPos, &vPos, pViewMat );
			EtVec3TransformCoord( &vProjPos, &vProjPos, pProjMat );
			vProjPos.y += TALK_HEIGHT;

			EtVec3TransformCoord( &vHeadProjPos, &vHeadPos, pViewMat );
			EtVec3TransformCoord( &vHeadProjPos, &vHeadProjPos, pProjMat );

			float fScaleX = (float)hChatBalloonTexture->Width() / 1024.f;
			float fScaleY = (float)hChatBalloonTexture->Height() /  768.f;

			float UPos[4] = {0.0f, 0.4f, 0.6f, 1.0f};
			float VPos[4] = {0.0f, 0.4f, 0.6f, 1.0f};

			//float fSizeX = m_wszTalkText.size() == 1 ? m_fTalkWidth : ((float)TEXT_WIDTH / GetEtDevice()->Width() ) ;
			float fSizeX = 0.0f;
			if( m_wszTalkText.size() == 1 )
				fSizeX = m_fTalkWidth;
			else {
				if( CEtFontMng::s_bUseUniscribe )
					fSizeX = m_fTalkWidthForWordBreak;
				else
					fSizeX = (float)TEXT_WIDTH / GetEtDevice()->Width();
			}
			float fSizeY = LINE_PIXEL * m_wszTalkText.size();

			float fAlignY = fSizeY + fScaleY * (VPos[3]-VPos[2]);
			float XPos[4] = {(UPos[1]-UPos[0])*-fScaleX-fSizeX, 
				-fSizeX, 
				fSizeX, 
				fSizeX + fScaleX * (UPos[3]-UPos[2]) };
			float YPos[4] = { 2.f * fSizeY + ((VPos[3]-VPos[2])+(VPos[1]-VPos[0])) * fScaleY,  
				2.f * fSizeY + (VPos[3]-VPos[2]) * fScaleY, 
				(VPos[3]-VPos[2]) * fScaleY, 
				0.f };

			static bool bVertx = false;

			

			STextureDiffuseVertex Vertices[ 16 ];
			EtMatrix MatIdent;
			EtMatrixIdentity(&MatIdent);
			GetEtDevice()->SetWorldTransform( &MatIdent );
			GetEtDevice()->SetViewTransform( &MatIdent );
			GetEtDevice()->SetProjTransform( &MatIdent );

			GetEtDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
			GetEtDevice()->SetVertexShader( NULL );
			GetEtDevice()->SetPixelShader( NULL );
					
			GetEtDevice()->SetTexture( 0, hChatBalloonTexture->GetTexturePtr() );

			for( int i = 0; i < 4; i++) {
				Vertices[i*4+0].Position.y = Vertices[i*4+1].Position.y = Vertices[i*4+2].Position.y 	= Vertices[i*4+3].Position.y = YPos[i] + vProjPos.y;
				Vertices[ i ].Position.x = Vertices[4 + i].Position.x = Vertices[8 + i].Position.x 	= Vertices[12 + i].Position.x = XPos[i] + vProjPos.x;
				Vertices[ i ].Position.z = Vertices[4 + i].Position.z = Vertices[8 + i].Position.z 	= Vertices[12 + i].Position.z = vProjPos.z;
				Vertices[ i ].Color = Vertices[4 + i].Color = Vertices[8 + i].Color 	= Vertices[12 + i].Color = D3DCOLOR_ARGB(dwAlpha, 255, 255, 255);

				Vertices[i*4+0].TexCoordinate.y = Vertices[i*4+1].TexCoordinate.y = Vertices[i*4+2].TexCoordinate.y 	= Vertices[i*4+3].TexCoordinate.y = UPos[i];
				Vertices[ i ].TexCoordinate.x = Vertices[4 + i].TexCoordinate.x = Vertices[8 + i].TexCoordinate.x 	= Vertices[12 + i].TexCoordinate.x = VPos[i];
			}

			unsigned short Indices[54] = {0, 1, 4, 5, 4, 1, 1, 2	, 5	, 6	, 
				5, 2, 2, 3, 6, 7, 6, 3	, 4	, 5	, 
				8, 9, 8, 5, 5, 6, 9, 10, 9, 6,
				6, 7, 10	, 11, 10, 7, 8, 9, 12, 13	, 
				12, 9	, 9	, 10, 13, 14	, 13, 10, 10	, 11, 
				14, 15, 14, 11	};

			GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, 16, 18, Indices, FMT_INDEX16, Vertices, sizeof( STextureDiffuseVertex ) );	

			// Render Tail
			float fTailPos = vHeadProjPos.x - vProjPos.x;
			if( fabsf( fTailPos - m_fTalkTailPosPrev) > 0.03f ) {
				m_fTalkTailPosPrev = fTailPos;
				m_fTalkTailPosPrev = max( min( fSizeX- 0.01f, m_fTalkTailPosPrev), -fSizeX );
			}

			m_fTalkTailPos = m_fTalkTailPos + (m_fTalkTailPosPrev - m_fTalkTailPos) * min(1.0f, ( fElapsedTime * 10.f ));
			m_fTalkTailPos = max( min( fSizeX - 0.01f, m_fTalkTailPos ), - fSizeX );

			static float fAddY = 0.006f;
			float fTailPosY = vProjPos.y + fAddY;
			STextureDiffuseVertex TailVertices[ 4 ];
			float fTailScaleX = (float)hChatBalloonTailTexture->Width() / 1024.f;
			float fTailScaleY = (float)hChatBalloonTailTexture->Height() / 768.f;	
			TailVertices[ 0 ].Position = EtVector3( vProjPos.x + m_fTalkTailPos - fTailScaleX * 0.5f, fTailPosY, vProjPos.z);
			TailVertices[ 0 ].TexCoordinate = EtVector2( 0.01f, 0.01f );
			TailVertices[ 0 ].Color = D3DCOLOR_ARGB( dwAlpha, 255, 255, 255);
			TailVertices[ 1 ].Position = EtVector3( vProjPos.x + m_fTalkTailPos + fTailScaleX * 0.5f, fTailPosY, vProjPos.z);
			TailVertices[ 1 ].TexCoordinate = EtVector2( 0.99f, 0.01f );
			TailVertices[ 1 ].Color = D3DCOLOR_ARGB( dwAlpha, 255, 255, 255);
			TailVertices[ 2 ].Position = EtVector3( vProjPos.x + m_fTalkTailPos + fTailScaleX * 0.5f, fTailPosY - fTailScaleY, vProjPos.z);
			TailVertices[ 2 ].TexCoordinate = EtVector2( 0.99f, 0.99f - 1.f / 16.f );
			TailVertices[ 2 ].Color = D3DCOLOR_ARGB( dwAlpha, 255, 255, 255);
			TailVertices[ 3 ].Position = EtVector3( vProjPos.x + m_fTalkTailPos - fTailScaleX * 0.5f, fTailPosY - fTailScaleY, vProjPos.z);
			TailVertices[ 3 ].TexCoordinate = EtVector2( 0.01f, 0.99f - 1.f / 16.f);
			TailVertices[ 3 ].Color = D3DCOLOR_ARGB( dwAlpha, 255, 255, 255);
			GetEtDevice()->SetTexture( 0, hChatBalloonTailTexture->GetTexturePtr() );
			GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, TailVertices, sizeof( STextureDiffuseVertex ) );
			GetEtDevice()->SetTexture( 0, NULL);

			SFontDrawEffectInfo Info;

			Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,0);
			switch( m_nChatType )
			{
			case CHAT_NORMAL:		Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 90,82,76);	break;
			case CHAT_PARTY:		Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,128);		break;
			case CHAT_CHATROOM1:	Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,0);	break;
			case CHAT_CHATROOM2:	Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,0);	break;
			case CHAT_CHATROOM3:	Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,0);	break;
			case CHAT_REPUTE:		Info.dwFontColor = D3DCOLOR_ARGB(dwAlpha, 0,0,0);	break;
			}

			for( int i= 0; i < (int)m_wszTalkText.size(); i++) {
				SUICoord Coord;
				Coord.fX = ( Vertices[5].Position.x + 1.f ) / 2.f;
				Coord.fY = 1.f - ( ( Vertices[5].Position.y - (i+0.5f) * LINE_PIXEL * 2.f + 1.0f ) / 2.f );
				Coord.fY += 4.0f / DEFAULT_UI_SCREEN_HEIGHT;	// 캐시말 나오면 이런식으로 해야할듯..
				if( CENTER_ALIGN ) {				
					if( CEtFontMng::s_bUseUniscribe )
					{
						Coord.fWidth = m_fTalkWidthForWordBreak;
					}
					else
					{
						Coord.fWidth = m_fTalkWidth;
					}
					CEtFontMng::GetInstance().DrawTextW( CDnActor::s_nFontIndex, 14, m_wszTalkText[ i ].c_str(), DT_CENTER | DT_VCENTER, Coord, -1, Info, true, vProjPos.z );
				}
				else {
					CEtFontMng::GetInstance().DrawTextW(CDnActor::s_nFontIndex, 14, m_wszTalkText[i].c_str(), DT_LEFT | DT_VCENTER, Coord, -1, Info, true, vProjPos.z);
				}
			}

			if( hCamera ) {
				GetEtDevice()->SetViewTransform( hCamera->GetViewMat() );
				GetEtDevice()->SetProjTransform( hCamera->GetProjMat() );
			}
		}
	}
	else if( m_bConutBalloon )
	{
		DecreaseBalloon();
	}
}

void MAChatBalloon::ProcessCustomChatBalloonText( float fElapsedTime )
{
	if( m_vecCustomChatBalloonText.empty() ) return;

	const float fDelay = 4.0f;

	if( m_fCustomTextDelayTime < 0.0f )
	{
		++m_nCustomTextIndex;
		if( (int)m_vecCustomChatBalloonText.size() <= m_nCustomTextIndex )
			m_nCustomTextIndex = 0;

		if( m_Dialog )
			m_Dialog->SetTextLine( m_vecCustomChatBalloonText[m_nCustomTextIndex].c_str(), m_Dialog->GetTextColor(), UITEXT_CENTER );

		m_fCustomTextDelayTime += fDelay;
	}

	m_fCustomTextDelayTime -= fElapsedTime;
}

void MAChatBalloon::StopChatBalloonCustom()
{
	m_dwTalkStartTime = 0;
	if( m_Dialog )
	{
		m_Dialog->Show( false );
		SAFE_DELETE( m_Dialog );
	}
	m_nCustomTextIndex = 0;
	m_fCustomTextDelayTime = 0.0f;
	m_bEnableCustomChatBalloon = false;
}

void MAChatBalloon::AddCustomChatBalloonText( LPCWSTR wszMessage )
{
	m_vecCustomChatBalloonText.push_back( wszMessage );
	m_Dialog->SetMaxWidthFromText( wszMessage );

	// 인덱스를 맨 마지막걸로 설정해두고 Process돌려서 다음번 갱신에 첫번째 문구가 선택되게 한다.
	m_nCustomTextIndex = (int)m_vecCustomChatBalloonText.size()-1;
}

void MAChatBalloon::SetChatBalloonText( LPCWSTR wszMessage, DWORD dwStartTime, int nChatType, DWORD dwTalkTime )
{	
	static int MAX_LINE = 4;

	m_dwTalkStartTime = dwStartTime;
	m_dwDuringTime = dwTalkTime;
	m_nChatType = nChatType;

	m_fTalkWidth = 0.f;
	m_wszTalkText.clear();
	m_fTalkWidthForWordBreak = 0.0f;
	
	std::wstring strMsg = wszMessage;
	EtInterface::GetNameLinkMgr().TranslateText(strMsg, wszMessage);

	if( CEtFontMng::s_bUseUniscribe )
	{
		int nMaxWidth;
		if( !CEtFontMng::GetInstance().GetWordBreakText( strMsg, CDnActor::s_nFontIndex, 14, static_cast<float>( TEXT_WIDTH ), m_wszTalkText, nMaxWidth ) )
			return;

		m_fTalkWidthForWordBreak = max( 30.f / GetEtDevice()->Width(), static_cast<float>( nMaxWidth ) / GetEtDevice()->Width() );

		if( m_wszTalkText.size() == 1 ) 
			m_fTalkWidth = m_fTalkWidthForWordBreak;
		else
			m_fTalkWidth = 0.f;
	}
	else
	{
		if( CEtFontMng::s_bUseWordBreak ) {
			for( int i = 0; i < MAX_LINE && !strMsg.empty() ; i++) {
				int nTrail=0;
				int nCaret = CEtFontMng::GetInstance().GetCaretFromCaretPos( strMsg.c_str(), CDnActor::s_nFontIndex, 14, TEXT_WIDTH, nTrail );
				if( nCaret < (int)strMsg.size()-1 ) {
					nCaret = CEtUITextBox::GetCaretWithWordBreak( strMsg, nCaret, true );
					m_wszTalkText.push_back( strMsg.substr(0, nCaret) );
					SUICoord Coord;
					CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 14, m_wszTalkText[m_wszTalkText.size() - 1].c_str(), DT_LEFT | DT_TOP, Coord, -1);
					Coord.fWidth = max( 30.f / 1024.f, Coord.fWidth);
					m_fTalkWidthForWordBreak = max(m_fTalkWidthForWordBreak, Coord.fWidth);
					strMsg = strMsg.substr( nCaret ).c_str();
				}
				else {
					m_wszTalkText.push_back( strMsg );
					if( m_wszTalkText.size() == 1 ) {
						SUICoord Coord;
						CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 14, m_wszTalkText[0].c_str(), DT_LEFT | DT_TOP, Coord, -1);
						m_fTalkWidth = max( 30.f / 1024.f, Coord.fWidth );
						m_fTalkWidthForWordBreak = m_fTalkWidth;
					}
					else {
						m_fTalkWidth = 0.f;
						// 이상하게 0으로 TalkWidth를 설정해서, 길이를 별도로 체크합니다.
						SUICoord Coord;
						CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 14, m_wszTalkText[m_wszTalkText.size() - 1].c_str(), DT_LEFT | DT_TOP, Coord, -1);
						Coord.fWidth = max( 30.f / 1024.f, Coord.fWidth);
						m_fTalkWidthForWordBreak = max(m_fTalkWidthForWordBreak, Coord.fWidth);
					}
					break;
				}
			}
		}
		else {
			for( int i = 0; i < MAX_LINE && !strMsg.empty() ; i++) {
				int nTrail=0;
				int nCaret = CEtFontMng::GetInstance().GetCaretFromCaretPos( strMsg.c_str(), CDnActor::s_nFontIndex, 14, TEXT_WIDTH, nTrail );
				if( nCaret < (int)strMsg.size()-1 ) {
					m_wszTalkText.push_back( strMsg.substr(0, nCaret+nTrail) );
					strMsg = strMsg.substr( nCaret+nTrail ).c_str();

					SUICoord Coord;
					CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 14, m_wszTalkText[m_wszTalkText.size() - 1].c_str(), DT_LEFT | DT_TOP, Coord, -1);
					m_fTalkWidth = max( m_fTalkWidth, Coord.fWidth );
				}
				else {
					m_wszTalkText.push_back( strMsg );
					if( m_wszTalkText.size() == 1 ) {
						SUICoord Coord;
						CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 14, m_wszTalkText[0].c_str(), DT_LEFT | DT_TOP, Coord, -1);
						m_fTalkWidth = max( 30.f / 1024.f, Coord.fWidth );
					}
					break;
				}
			}
		}
	}
}

void MAChatBalloon::SetChatBalloonCustomText( const char * wszFileName, LPCWSTR wszMessage, DWORD dwTextColor, DWORD dwStartTime, int nChatType, DWORD dwTalkTime )
{
	if( NULL == wszFileName )
		return;

	m_dwTalkStartTime = dwStartTime;
	m_dwDuringTime = dwTalkTime;
	m_nChatType = nChatType;
	m_bEnableCustomChatBalloon = true;

	//다이알로그 초기화
	if( NULL == m_Dialog )
	{
		m_Dialog = new CDnChatBalloonDlg(UI_TYPE_SELF);
		m_Dialog->Initialize(wszFileName , true );
	}
	else
	{
		m_Dialog->Show(true);
	}
	m_Dialog->SetChatMsg(wszMessage, dwTextColor , 8);
}

void MAChatBalloon::SetChatBalloonCustom( const char * wszFileName, LPCWSTR wszMessage, DWORD dwTextColor, DWORD dwStartTime, int nChatType, DWORD dwTalkTime )
{
	if( NULL == wszFileName )
		return;

	m_dwTalkStartTime = dwStartTime;
	m_dwDuringTime = dwTalkTime;
	m_nChatType = nChatType;
	m_bEnableCustomChatBalloon = true;

	//다이알로그 초기화
	if( NULL == m_Dialog )
	{
		m_Dialog = new CDnChatBalloonDlg(UI_TYPE_SELF);
		m_Dialog->Initialize(wszFileName , true );
	}
	else
	{
		m_Dialog->Show(true);
	}
	m_Dialog->SetTextLine(wszMessage, dwTextColor);
}



bool MAChatBalloon::IsRenderChatBalloon()
{
	if( GetTickCount() < m_dwDuringTime + m_dwTalkStartTime )
		return true;
	return false;
}

void MAChatBalloon::StopRenderChatBalloon()
{
	m_dwTalkStartTime = 0;

	if( m_bConutBalloon )
		DecreaseBalloon();
}

bool MAChatBalloon::CheckDistance( EtVector3 vPos, bool bNormalChatBalloon )
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	bool bFarmGameTask = false;
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
		bFarmGameTask = true;

	if( bFarmGameTask && CDnActor::s_hLocalActor )
	{
		float fMaxDistance = (float)CGlobalWeightTable::GetInstance().GetValue( bNormalChatBalloon ? CGlobalWeightTable::FarmChatBalloonDistance : CGlobalWeightTable::FarmChatRoomDistance );
		float fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - vPos) );
		if( fDistance > fMaxDistance )
			return false;
	}

	return true;
}