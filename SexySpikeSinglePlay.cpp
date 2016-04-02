
#include	"./Include/DEFAULT_INCLUDE.h"
#include	"./Include/INCLUDE.h"

//싱글 플레이 프레임 무브
HRESULT SexySpike::SinglePlayFrameMove()
{
	/////////////////////////////////////////////////////////////////////////////
	//09-08-04 이재준
	SAFE_FRMOV(	GMAIN->m_pInput	);
	SAFE_FRMOV(	GMAIN->m_pCamMn	);
	/////////////////////////////////////////////////////////////////////////////

	HRESULT	hr	= S_OK;

	//	키입력을 먼저 확인한다
	g_keys.ScanKeyState();

	//m_pd3dDevice->SetTransform( D3DTS_VIEW, m_camera.GetViewMatrix() );

	//m_pd3dDevice->SetTransform( D3DTS_PROJECTION, m_camera.GetProjMatrix() );


	switch( m_eFlow )
	{
	case READYPLAY:
		{
			ReadyPlay();
			break;
		}
	case PLAY:
		{
			srand( timeGetTime() );

			if( g_bNowServe && g_keys.GetCurrentState( 'Z' ) )
			{
				for( int i=0; i<NUMPLAYERS; ++i )
				{
					if( (m_ppPlayers[i]->GetPLAYSTATE() == SERVE) && (m_ppPlayers[i]->GetSelfUpdate()) )
					{
						m_ppPlayers[i]->SetMacro( THROWING );
						break;
					}
				}
			}

			//m_camera.FrameMove( m_fElapsedTime );

			D3DXMatrixMultiply(&g_matViewProj,&m_pCamMn->m_pCam0->m_mtViw,&m_pCamMn->m_pCam0->m_mtPrj);
			D3DXMatrixInverse(&g_matInvView,NULL,&m_pCamMn->m_pCam0->m_mtViw);
			g_matInvView._41 = 0;
			g_matInvView._42 = 0;
			g_matInvView._43 = 0;

			//D3DXMatrixMultiply( &g_matViewProj, m_camera.GetViewMatrix(), m_camera.GetProjMatrix() );
			//D3DXMatrixInverse( &g_matInvView, NULL, m_camera.GetViewMatrix() );
			//g_matInvView._41 = 0;
			//g_matInvView._42 = 0;
			//g_matInvView._43 = 0;

			ComputeCameraBasedDirections();

			if( g_CntBallTouched>3 )
			{
				CalcScore( eTeam(BLUE-((CPlayer*)g_pPlayerLastTouch)->GetTeam()) );
			}

			CHECK_FAILED( m_pBackground->FrameMove(m_fElapsedTime) );

			FRAMEMOVE( m_pNet, m_fElapsedTime );

			FRAMEMOVE( m_pBall, m_fElapsedTime );

			for( int i=0; i<NUMPLAYERS; ++i )
			{
				FRAMEMOVE( m_ppPlayers[i], m_fElapsedTime );
			}

			//	충돌검출
			CheckCollisions();

			m_eRenderMode =	PHANTOM==m_pBall->GetBalltype()? INVERSION: AFTERIMAGE;

			if( !g_bNowServe && m_pBall->GetSideBefore() != m_pBall->GetSideCurrent() )
			{
				float	fBallHeight	= m_pBall->GetHeight();
				float	fNetBottom	= m_pNet->GetBoundingMesh()->LB.y;
				if( fBallHeight > fNetBottom )
				{
					PlayerStateChange();
					g_CntBallTouched = 0;
				}
				else
				{
					eSide	SIDE = m_pBall->GetSideCurrent();
					for( int i=0; i<NUMPLAYERS; ++i )
					{
						if( !(SIDE & m_ppPlayers[i]->GetSide()) )
						{
							CalcScore( eTeam(BLUE-m_ppPlayers[i]->GetTeam()) );
							break;
						}
					}

				}
			}

			D3DXPLANE	ground= D3DXPLANE( 0, 1, 0, 0 );
			D3DXVECTOR4 light = D3DXVECTOR4( -g_light.Direction.x, -g_light.Direction.y, -g_light.Direction.z, 0 );
			D3DXMatrixShadow( &g_matShadow, &light, &ground );

			if( m_bMultiGame )
			{
				for( int i=0; i<NUMPLAYERS; ++i )
				{
					m_ppPlayers[i]->SendPlayerStateToNetwork();
				}

				if( m_pBall->GetNeedUpdate() )
				{
					BallPacket	packet(	
						NULL!=g_pPlayerLastTouch? (char*)((CPlayer*)g_pPlayerLastTouch)->GetName(): NULL
							,	m_pBall->GetBalltype()
							,	m_pBall->GetRotationAngle()
							,	m_pBall->GetRotationAxis()
							,	m_pBall->GetPosition()
							,	*m_pBall->GetVelocity()
							,	g_CntBallTouched
							,	g_bNowServe
							);

					for( int i=0; i<NUMPLAYERS; ++i )
					{
						if( !m_ppPlayers[i]->GetIsNpc() && (m_pBall->GetSideCurrent()&m_ppPlayers[i]->GetSide()) )
						{
							g_pNetAdapter->SendPacket(
								NULL
								,CLIENT_ROOM
								,CLIENT_ROOM_PLAY
								,NULL
								,NULL
								,NULL
								,U_BALL
								,sizeof(BallPacket)
								,&packet);
						}
					}

					m_pBall->SetNeedUpdate(false);
				}
			}

			break;
		}
	case CLEANPLAY:
		{
			g_ppPlayers	= NULL;
			for( int i=0; i<NUMPLAYERS; ++i )
			{
				SAFE_DELETE( m_ppPlayers[i] );
			}
			break;
		}
	}

	return hr;
}

//싱글 플레이 렌더
HRESULT SexySpike::SinglePlayRender()
{
	HRESULT hr = S_OK;
	/////////////////////////////////////////////////////////////////////////
	//09-08-04 이재준
	GMAIN->m_pCamMn->SpecificCameraON(0);
	GMAIN->m_pCamMn->m_pCam0->SetTransformViw();
	GMAIN->m_pCamMn->m_pCam0->SetTransformPrj();
	g_matViewProj	= GMAIN->m_pCamMn->m_pCam0->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam0->GetMatrixPrj();
	/////////////////////////////////////////////////////////////////////////

	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00004422, 1.f, 0 );

	switch( m_eFlow )
	{
	case PLAY:
		{
			switch( m_eRenderMode )
			{
			case	NORMAL:
				CHECK_FAILED(RenderNormal());
				break;
			case	AFTERIMAGE:
				CHECK_FAILED(RenderAfterImage());
				break;
			case	INVERSION:
				CHECK_FAILED(RenderInversion());
				break;
			}

			if(8>m_slapcount)
			{
				D3DXMATRIX	matHand	= g_matInvView;
				matHand._41	= m_vSlapPos.x;
				matHand._42	= m_vSlapPos.y;
				matHand._43	= m_vSlapPos.z;
				m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
				m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x0);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				m_pd3dDevice->SetTransform(D3DTS_WORLD, &matHand);
				m_pd3dDevice->SetStreamSource(0, m_pSlap, 0, sizeof(SimpleVertex) );
				m_pd3dDevice->SetFVF(SimpleVertex::SimpleVertexFVF);
				m_pd3dDevice->SetTexture(0, m_pSlapTex);
				m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
				m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
				m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
				++m_slapcount;
				SimpleVertex *pSlap = NULL;
				CHECK_FAILED(m_pSlap->Lock(0,0,(void**)&pSlap, 0));
				pSlap[0].tu	= m_slapcount*0.125f;
				pSlap[1].tu	= m_slapcount*0.125f;
				pSlap[2].tu	= (m_slapcount+1)*0.125f;
				pSlap[3].tu	= (m_slapcount+1)*0.125f;
				CHECK_FAILED(m_pSlap->Unlock());
			}
		}
	}

	//09-09-02 이재준
	m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	SpriteDraw();
	m_pd3dSprite->End();

	// Output statistics
	m_pFont->DrawText(   2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
	m_pFont->DrawText(   2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

	//09-09-10 이재준
	MultiPlayIDTextDraw();

	return hr;
}

HRESULT	SexySpike::RenderNormal()
{
	HRESULT	hr	= S_OK;
	CHECK_FAILED ( m_pBackground->Render(m_pd3dDevice) );

	CHECK_FAILED ( m_pNet->Render(m_pd3dDevice) );

	for( int i=0; i<NUMPLAYERS; ++i )
	{
		CHECK_FAILED(m_ppPlayers[i]->Render(m_pd3dDevice));
	}

	CHECK_FAILED( hr = m_pBall->Render(m_pd3dDevice) );

	return	hr;
}
HRESULT	SexySpike::RenderAfterImage()
{
	HRESULT	hr	= S_OK;
	D3DVIEWPORT9	NewVp;
	D3DVIEWPORT9	OldVp;
	LPDIRECT3DSURFACE9	OldSuf	= NULL;
	LPDIRECT3DSURFACE9	OldDep	= NULL;
	LPD3DXEFFECT		pEffect	= g_Effects["MAIN"];

	++m_LoofCounter;
	int ix	= m_LoofCounter%2;

	CHECK_FAILED_GOTO(GetRenderTarget( &OldSuf, &OldDep, &OldVp), RELEASE);
	NewVp	= OldVp;
	CHECK_FAILED_GOTO(SetRenderTarget( m_pPostSuf[ix], m_pPostDep[ix], &NewVp), RELEASE);

	CHECK_FAILED_GOTO(pEffect->SetTechnique("TAfterImage"), RELEASE);
	CHECK_FAILED_GOTO(pEffect->Begin(NULL, 0), RELEASE);
	CHECK_FAILED_GOTO(pEffect->Pass(0), RELEASE);
	CHECK_FAILED_GOTO(m_pd3dDevice->SetTexture(0, m_pPostTex[1-ix]), RELEASE);
	CHECK_FAILED_GOTO(m_pd3dDevice->SetStreamSource(0, m_pPost, 0, sizeof(SimpleVertex2)), RELEASE);
	CHECK_FAILED_GOTO(m_pd3dDevice->SetFVF(SimpleVertex2::SimpleVertex2FVF), RELEASE);
	CHECK_FAILED_GOTO(m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2), RELEASE);
	CHECK_FAILED_GOTO(pEffect->End(), RELEASE);
	if( (50.f<D3DXVec3Length(m_pBall->GetVelocity())) && (BLACKHOLE!=m_pBall->GetBalltype()))
	{
		m_pBall->SetShowFallingPoint(FALSE);
		CHECK_FAILED_GOTO(m_pBall->Render(m_pd3dDevice), RELEASE);
	}

	CHECK_FAILED_GOTO(SetRenderTarget( OldSuf, OldDep, &OldVp), RELEASE);
	SAFE_RELEASE( OldSuf );
	SAFE_RELEASE( OldDep );

	CHECK_FAILED(m_pBackground->Render(m_pd3dDevice));

	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CHECK_FAILED(m_pd3dDevice->SetTexture(0, m_pPostTex[ix]));
	CHECK_FAILED(m_pd3dDevice->SetStreamSource(0, m_pPost, 0, sizeof(SimpleVertex2)));
	CHECK_FAILED(m_pd3dDevice->SetFVF(SimpleVertex2::SimpleVertex2FVF));
	CHECK_FAILED(m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2));
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	CHECK_FAILED(m_pNet->Render(m_pd3dDevice));

	for( int i=0; i<NUMPLAYERS; ++i )
	{
		m_ppPlayers[i]->Render(m_pd3dDevice);
	}

	m_pBall->SetShowFallingPoint(TRUE);
	CHECK_FAILED(m_pBall->Render(m_pd3dDevice));

RELEASE:
	SAFE_RELEASE( OldSuf );
	SAFE_RELEASE( OldDep );

	return	hr;
}
HRESULT	SexySpike::RenderInversion()
{
	HRESULT	hr	= S_OK;
	D3DVIEWPORT9	NewVp;
	D3DVIEWPORT9	OldVp;
	LPDIRECT3DSURFACE9	OldSuf	= NULL;
	LPDIRECT3DSURFACE9	OldDep	= NULL;
	LPD3DXEFFECT		pEffect	= g_Effects["MAIN"];

	++m_LoofCounter;
	int ix	= m_LoofCounter%2;

	CHECK_FAILED_GOTO(GetRenderTarget( &OldSuf, &OldDep, &OldVp), RELEASE);
	NewVp	= OldVp;
	CHECK_FAILED_GOTO(SetRenderTarget( m_pPostSuf[ix], m_pPostDep[ix], &NewVp), RELEASE);

	CHECK_FAILED_GOTO( m_pd3dDevice->Clear(
		0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.f, 0L ), RELEASE);

	CHECK_FAILED( RenderNormal() );

	CHECK_FAILED_GOTO( m_pd3dDevice->SetRenderTarget(0, OldSuf), RELEASE );
	CHECK_FAILED_GOTO( m_pd3dDevice->SetDepthStencilSurface(OldDep), RELEASE );
	CHECK_FAILED_GOTO( m_pd3dDevice->SetViewport(&OldVp), RELEASE );

	pEffect->SetTechnique( "TInversion" );
	pEffect->Begin( NULL, 0);
	pEffect->Pass(0);
	CHECK_FAILED(m_pd3dDevice->SetTexture(0, m_pPostTex[ix]));
	CHECK_FAILED(m_pd3dDevice->SetStreamSource(0, m_pPost, 0, sizeof(SimpleVertex2)));
	CHECK_FAILED(m_pd3dDevice->SetFVF(SimpleVertex2::SimpleVertex2FVF));
	CHECK_FAILED(m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 ));
	pEffect->End();


RELEASE:
	SAFE_RELEASE( OldSuf );
	SAFE_RELEASE( OldDep );
	
	return	hr;
}
void	SexySpike::ReadyPlay()
{
	ZeroMemory(	g_score, sizeof(g_score) );
	ZeroMemory( g_Teams, sizeof(g_Teams) );

	//09-09-04	이재준
	CharacterSet();

	AlignCoat();
	m_ppPlayers[0]->ComputeValleyActionPoint( m_ppPlayers[0]->GetRotationAngle() );
	m_ppPlayers[1]->ComputeValleyActionPoint( m_ppPlayers[1]->GetRotationAngle() );
	m_ppPlayers[2]->ComputeValleyActionPoint( m_ppPlayers[2]->GetRotationAngle() );
	m_ppPlayers[3]->ComputeValleyActionPoint( m_ppPlayers[3]->GetRotationAngle() );
	m_ppPlayers[0]->SetTeammate( m_ppPlayers[1] );
	m_ppPlayers[1]->SetTeammate( m_ppPlayers[0] );
	m_ppPlayers[2]->SetTeammate( m_ppPlayers[3] );
	m_ppPlayers[3]->SetTeammate( m_ppPlayers[2] );

	g_Teams[RED ] = TeamInfo( m_ppPlayers[0], m_ppPlayers[1], RED , m_ppPlayers[0], SERVE );
	g_Teams[BLUE] = TeamInfo( m_ppPlayers[2], m_ppPlayers[3], BLUE, NULL, DEFENSE );

	g_ppPlayers	= (void**)m_ppPlayers;
	g_bNowServe	= TRUE;
	m_eFlow	= PLAY;
	FrameMove();
}
//
void	SexySpike::CheckCollisions()
{
	eBallType				balltype	= COMMON;
	const	BoundingSphere	*Ball		= m_pBall->GetBoundingBall();
	const	D3DXVECTOR3		*vVelocity	= m_pBall->GetVelocity();
	const	BoundingPlane	*Net		= m_pNet->GetBoundingNet();
	eBehavior				BEHAVIOR	= WAITING;
	g_pPlayerRedShift					= NULL;

	for( int i=0; i<NUMPLAYERS; ++i )
	{
		BEHAVIOR	= m_ppPlayers[i]->GetCurrentBehavior();

		if( !m_ppPlayers[i]->GetSelfUpdate() )
			continue;

		//	사람, 넷
		if( m_pCrashManager->DetectPlaneCollisionSphere( Net, m_ppPlayers[i]->GetBoundingBody() ) )
			m_ppPlayers[i]->MoveTo( (m_ppPlayers[i]->GetPlayInfo()->SIDE&LEFT? -CHARACTERSTRIDE: CHARACTERSTRIDE), 0, 0 );

		//	사람, 공
		if( (m_ppPlayers[i] != g_pPlayerLastTouch) && (vVelocity->y<=0.f) && m_ppPlayers[i]->GetSide()&m_pBall->GetSideCurrent())
		{
			if( m_pCrashManager->DetectCylinderCollisionSphere( m_ppPlayers[i]->GetBoundingSpike(), m_ppPlayers[i]->GetBoundingDefense(), Ball ) )
			{
				balltype			= REDSHIFT;
				g_pPlayerRedShift	= m_ppPlayers[i];
			}

			if( (m_pCrashManager->DetectSphereCollision( m_ppPlayers[i]->GetBoundingHand(), Ball )) )
			{
				CPlayer	*TEAMMATE	= m_ppPlayers[i]->GetPlayInfo()->TEAMMATE;
				D3DXVECTOR3	vDest	= D3DXVECTOR3(0.f, 0.f, 0.f);

				int		eAttack		= MC;
				if( g_keys.GetCurrentState(VK_UP	) ) eAttack += -3;
				if( g_keys.GetCurrentState(VK_DOWN	) ) eAttack += 3;
				if( g_keys.GetCurrentState(VK_LEFT	) ) eAttack -= 1;
				if( g_keys.GetCurrentState(VK_RIGHT	) ) eAttack += 1;

				//	공 방향 결정
				switch( BEHAVIOR )
				{
				case RECEIVE:
				case TOSS:
					{
						if( m_ppPlayers[i]->GetIsNpc() )
						{
							vDest	= (TEAMMATE->GetIsNpc()? *TEAMMATE->GetAttackingPosition(): TEAMMATE->GetPosition() );
						}
						else
						{
							vDest	= ComputeAttackingDest( m_ppPlayers[i]->GetSide(), eAttackingSide(eAttack) );
						}
						if( g_CntBallTouched == 2)
							vDest.x = m_ppPlayers[i]->GetSide()&LEFT? 40.f: -40.f;
					}
					break;
				case SPIKE:
				case SERVE_OVER:
				case SERVE_UNDER:
					{
						if( m_ppPlayers[i]->GetIsNpc() )
						{
							eAttack	= m_ppPlayers[i]->GetAttackingSide();
							eAttack	= eAttack<ML? (rand()%BR+1) : eAttack<BL? (rand()%BL+ML): (rand()%ML+BL);
						}

						vDest	= ComputeAttackingDest( m_ppPlayers[i]->GetSide(), eAttackingSide(eAttack) );
						vDest.x	+= m_ppPlayers[i]->GetSide()&LEFT? 80.f: -80.f;
						vDest.x = max(20.f, fabs(vDest.x)) * (vDest.x/fabs(vDest.x));
					}
					break;
				case SLIDING:
					break;
				default:
					continue;
				}

				++g_CntBallTouched;
				g_pPlayerLastTouch	= m_ppPlayers[i];
				m_pBall->SetDest( vDest );
				m_pBall->ReflectByXXX( &(Ball->vCenter-m_ppPlayers[i]->GetBoundingHand()->vCenter), BEHAVIOR );

				//09-09-03 이재준
				SoundPlay(BEHAVIOR);


				//	후 처리
				switch( BEHAVIOR )
				{
				case RECEIVE:
				case TOSS:
					{
						TEAMMATE->SetAttackingSide( eAttackingSide(eAttack) );
						TEAMMATE->SetAttackingPos( vDest );

						if( g_CntBallTouched < 2 )
						{
							int NewAttackSide = m_ppPlayers[i]->GetAttackingSide();
							if( TR < NewAttackSide )
							{
								NewAttackSide -= (MR < NewAttackSide)?BL : ML;
								D3DXVECTOR3 vAttack	= ComputeAttackingDest( m_ppPlayers[i]->GetSide(), eAttackingSide(NewAttackSide) );
								m_ppPlayers[i]->SetAttackingSide( eAttackingSide(NewAttackSide) );
								m_ppPlayers[i]->SetAttackingPos( vAttack );
							}
						}
					}
					break;
				case SLIDING:
					{
						if( TEAMMATE->GetIsNpc() )
						{
							m_ppPlayers[i]->GetTeammate()->SetMacro( RECEIVE );
						}
					}
					break;
				case SPIKE:
					m_slapcount	= 0;
					m_vSlapPos	= m_pBall->GetPosition();
					balltype	= m_ppPlayers[i]->GetSpecialAttack();
				case SERVE_OVER:
				case SERVE_UNDER:
					{
						if( SERVE == m_ppPlayers[i]->GetPLAYSTATE() )
						{
							TeamInfo& Defense		= g_Teams[ eTeam(BLUE - m_ppPlayers[i]->GetTeam()) ];
							D3DXVECTOR3	vFalling	= m_pBall->GetFallingSpot();
							D3DXVECTOR3	vPos1		= ((CPlayer*)Defense.PLAYER1)->GetPosition();
							D3DXVECTOR3	vPos2		= ((CPlayer*)Defense.PLAYER2)->GetPosition();
							D3DXVECTOR3	vSub1		= vFalling-vPos1;
							D3DXVECTOR3	vSub2		= vFalling-vPos2;
							CPlayer*	RECEIVER	= (D3DXVec3Length(&vSub1) < D3DXVec3Length(&vSub2))? (CPlayer*)Defense.PLAYER1: (CPlayer*)Defense.PLAYER2;
							if( RECEIVER->GetIsNpc() )
							{
								RECEIVER->SetMacro( RECEIVE );
							}
						}
						else
						{
							CPlayer* player1	= (CPlayer*)g_Teams[1-m_ppPlayers[i]->GetTeam()].PLAYER1;
							CPlayer* player2	= (CPlayer*)g_Teams[1-m_ppPlayers[i]->GetTeam()].PLAYER2;
							CPlayer* receiver	= player1->GetSide()&BACK? player1: player2;
							if( receiver->GetIsNpc() && receiver->GetSelfUpdate() )
							{
								receiver->SetMacro(RECEIVE);
							}
						}
					}
					break;
				}	// end of switch

			}
		}	//	end of if( !bRedShift && (m_ppPlayers[i] != g_pPlayerLastTouch) && (vVelocity->y<=0.f) )
	}	// end of for()

	if( COMMON == m_pBall->GetBalltype() || REDSHIFT == m_pBall->GetBalltype() )
		m_pBall->SetBalltype( balltype );

	if( (Ball->vCenter.y - Ball->fRadius) <= 0.f )
	{
		D3DXVECTOR3 vGround	 = D3DXVECTOR3(0.f, 1.f, 0.f);
		m_pBall->MoveTo(0.f, Ball->fRadius, 0.f);
		m_pBall->ReflectByXXX( &vGround, WAITING, 0.7f );
		for( int i=0; i<NUMPLAYERS && !g_bNowServe; ++i )
		{
			if( m_ppPlayers[i]->GetPlayInfo()->SIDE&m_pBall->GetSideCurrent() )
			{
				CalcScore(eTeam(BLUE-(m_ppPlayers[i]->GetTeam())));
				break;
			}
		}
	}

	if( m_pCrashManager->DetectPlaneCollisionSphere(&g_BoundingMesh, Ball) )
	{
		m_pBall->SetMacro( BALLANDNET, &g_BoundingMesh );
		m_pNet->MakeNetDance(m_pBall->GetVelocity());
	}
}

void	SexySpike::AlignCoat()
{
	for( int i=0; i<NUMPLAYERS; ++i )
	{
		int			SIDE	= m_ppPlayers[i]->GetSide();
		float		fRot	= SIDE&LEFT? 1.5f*D3DX_PI : 0.5f*D3DX_PI ;
		D3DXVECTOR3	vPos	= D3DXVECTOR3(0.f, 0.f, 0.f);
		switch( SIDE )
		{
		case LEFT | FRONT :
			vPos		= D3DXVECTOR3(  -20.f, 0.f,  -70.f );
			break;
		case LEFT | BACK :
			vPos		= D3DXVECTOR3(  -50.f, 0.f, -100.f );
			break;
		case RIGHT | FRONT :
			vPos		= D3DXVECTOR3(   20.f, 0.f,  -70.f );
			break;
		case RIGHT | BACK :
			vPos		= D3DXVECTOR3(   50.f, 0.f, -100.f );
			break;
		}

		m_ppPlayers[i]->SetRotation( fRot );
		m_ppPlayers[i]->SetPosition( vPos.x, vPos.y, vPos.z );
		m_ppPlayers[i]->SetAttackingPos( vPos );

		// 볼설정
		if( m_ppPlayers[i]->GetPLAYSTATE() & SERVE )
		{
			/*		m_pBall->SetSideBefore( SIDE&LEFT? LEFT: RIGHT );
			m_pBall->SetSideCurrent( m_pBall->GetSideBefore() );
			m_pBall->SetShowFallingPoint( FALSE );
			m_pBall->SetPosition( 0.f, 10000.f, 0.f );
			m_pBall->SetVelocity( D3DXVECTOR3(0.f, 10000.f, 0.f) );*/
		}
	}

	g_CntBallTouched	= -1;
}

void	SexySpike::PlayerStateChange()
{
	eSide		eSideBall	= m_pBall->GetSideCurrent();
	for( int i=0; i<NUMPLAYERS; ++i )
	{
		m_ppPlayers[i]->SetPLAYSTATE( m_ppPlayers[i]->GetPlayInfo()->SIDE&eSideBall? OFFENSE: DEFENSE );

		if( m_ppPlayers[i]->GetPLAYSTATE() == DEFENSE )
		{
			m_ppPlayers[i]->SetIsAttacker(FALSE);
		}
		else
		{
			if( !(m_ppPlayers[i]->GetIsAttacker()) && !(m_ppPlayers[i]->GetPlayInfo()->TEAMMATE->GetIsAttacker()) )
			{
				CPlayer	*TEAMMATE	= m_ppPlayers[i]->GetPlayInfo()->TEAMMATE;
				m_ppPlayers[i]->SetIsAttacker( rand()%2 );
				TEAMMATE->SetIsAttacker( !(m_ppPlayers[i]->GetIsAttacker()) );
				m_ppPlayers[i]->SetAttackingSide( eAttackingSide(rand()%(BR+1)) );
				TEAMMATE->SetAttackingSide( eAttackingSide(BR-m_ppPlayers[i]->GetAttackingSide()) );
				m_ppPlayers[i]->SetAttackingPos( ComputeAttackingDest( m_ppPlayers[i]->GetPlayInfo()->SIDE, m_ppPlayers[i]->GetAttackingSide()));
				TEAMMATE->SetAttackingPos( ComputeAttackingDest( TEAMMATE->GetPlayInfo()->SIDE, TEAMMATE->GetAttackingSide() ));
			}
		}
	}
}
D3DXVECTOR3	SexySpike::ComputeAttackingDest( int SIDE, eAttackingSide eAttack )
{
	float	T = SIDE&LEFT? -10.f: 10.f;
	float	M = T*3.f;
	float	B = T*5.f;
	float	C = (g_BoundingMesh.LB+(0.5f*(g_BoundingMesh.RT-g_BoundingMesh.LB))).z;
	float	L = C+(SIDE&LEFT?20.f:-20.f);
	float	R = C+(SIDE&LEFT?-20.f:20.f);
	D3DXVECTOR3	vAttackPos = D3DXVECTOR3( 0, 0, 0 );
	switch( eAttack )
	{
	case TL:
		{
			vAttackPos = D3DXVECTOR3( T, 0.f, L );
			break;
		}
	case TC:
		{
			vAttackPos = D3DXVECTOR3( T, 0.f, C );
			break;
		}
	case TR:
		{
			vAttackPos = D3DXVECTOR3( T, 0.f, R );
			break;
		}
	case ML:
		{
			vAttackPos = D3DXVECTOR3( M, 0.f, L );
			break;
		}
	case MC:
		{
			vAttackPos = D3DXVECTOR3( M, 0.f, C );
			break;
		}
	case MR:
		{
			vAttackPos = D3DXVECTOR3( M, 0.f, R );
			break;
		}
	case BL:
		{
			vAttackPos = D3DXVECTOR3( B, 0.f, L );
			break;
		}
	case BC:
		{
			vAttackPos = D3DXVECTOR3( B, 0.f, C );
			break;
		}
	case BR:
		{
			vAttackPos = D3DXVECTOR3( B, 0.f, R );
			break;
		}
	}

	return	vAttackPos;
}
void	SexySpike::CalcScore( eTeam TEAM )
{
	++g_score[TEAM];

	//	공격팀 설정
	if( DEFENSE == g_Teams[TEAM].PLAYSTATE )
	{
		g_Teams[TEAM].PLAYSTATE	= SERVE;
		g_Teams[TEAM].LASTSERVER= g_Teams[TEAM].PLAYER1==g_Teams[TEAM].LASTSERVER? g_Teams[TEAM].PLAYER2: g_Teams[TEAM].PLAYER1;
		((CPlayer*)g_Teams[TEAM].PLAYER1)->SetPLAYSTATE( g_Teams[TEAM].PLAYER1==g_Teams[TEAM].LASTSERVER? SERVE: OFFENSE );
		((CPlayer*)g_Teams[TEAM].PLAYER2)->SetPLAYSTATE( g_Teams[TEAM].PLAYER2==g_Teams[TEAM].LASTSERVER? SERVE: OFFENSE );
	}
	else
	{
		g_Teams[TEAM].PLAYSTATE	= SERVE;
		((CPlayer*)g_Teams[TEAM].PLAYER1)->SetPLAYSTATE( g_Teams[TEAM].PLAYER1==g_Teams[TEAM].LASTSERVER? SERVE: OFFENSE );
		((CPlayer*)g_Teams[TEAM].PLAYER2)->SetPLAYSTATE( g_Teams[TEAM].PLAYER2==g_Teams[TEAM].LASTSERVER? SERVE: OFFENSE );
	}

	eSide	SIDE	= ((CPlayer*)g_Teams[TEAM].PLAYER1)->GetSide()&LEFT?LEFT:RIGHT;
	((CPlayer*)g_Teams[TEAM].PLAYER1)->SetSide( SIDE | (g_Teams[TEAM].PLAYER1==g_Teams[TEAM].LASTSERVER?BACK:FRONT));
	((CPlayer*)g_Teams[TEAM].PLAYER2)->SetSide( SIDE | (g_Teams[TEAM].PLAYER2==g_Teams[TEAM].LASTSERVER?BACK:FRONT));

	//	수비팀 설정
	g_Teams[BLUE-TEAM].PLAYSTATE = DEFENSE;
	((CPlayer*)g_Teams[BLUE-TEAM].PLAYER1)->SetPLAYSTATE(DEFENSE);
	((CPlayer*)g_Teams[BLUE-TEAM].PLAYER2)->SetPLAYSTATE(DEFENSE);

	//	코트정렬
	g_bNowServe=TRUE;
	AlignCoat();
}
void	SexySpike::ComputeCameraBasedDirections()
{
	D3DXVECTOR3	vBase1	= D3DXVECTOR3( 0.f, 0.f, -1.f );
	D3DXVECTOR3 vBase2	= D3DXVECTOR3( -1.f, 0.f, 0.f );

	//D3DXVECTOR3	vLookAt	= m_camera.GetWorldAhead();
	D3DXVECTOR3	vLookAt = m_pCamMn->m_pCam0->GetInverseViewMatrix();

	vLookAt.y			= 0.f;
	D3DXVec3Normalize( &vLookAt, &vLookAt );
	float	fCos1	= D3DXVec3Dot( &vBase1, &vLookAt );
	float	fCos2	= D3DXVec3Dot( &vBase2, &vLookAt );

	if( fCos1 > 0.85f )
	{
		g_dir[U] = D3DXVECTOR3( 0.f, 0.f, -CHARACTERSTRIDE );
		g_dir[B] = D3DXVECTOR3( 0.f, 0.f, CHARACTERSTRIDE);
		g_dir[L] = D3DXVECTOR3( CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[R] = D3DXVECTOR3( -CHARACTERSTRIDE, 0.f, 0.f );
	}
	else if( fCos1 < -0.85f )
	{
		g_dir[U] = D3DXVECTOR3( 0.f, 0.f, CHARACTERSTRIDE );
		g_dir[B] = D3DXVECTOR3( 0.f, 0.f, -CHARACTERSTRIDE);
		g_dir[L] = D3DXVECTOR3( -CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[R] = D3DXVECTOR3( CHARACTERSTRIDE, 0.f, 0.f );
	}
	else if( (fCos1>0.f && fCos2>0.f) || (fCos1<0.f && fCos2>0.f) )
	{
		g_dir[U] = D3DXVECTOR3( -CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[B] = D3DXVECTOR3( CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[L] = D3DXVECTOR3( 0.f, 0.f, -CHARACTERSTRIDE );
		g_dir[R] = D3DXVECTOR3( 0.f, 0.f, CHARACTERSTRIDE);
	}
	else
	{
		g_dir[U] = D3DXVECTOR3( CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[B] = D3DXVECTOR3( -CHARACTERSTRIDE, 0.f, 0.f );
		g_dir[L] = D3DXVECTOR3( 0.f, 0.f, CHARACTERSTRIDE );
		g_dir[R] = D3DXVECTOR3( 0.f, 0.f, -CHARACTERSTRIDE);
	}
}
HRESULT	SexySpike::GetRenderTarget(LPDIRECT3DSURFACE9* ppSuf, LPDIRECT3DSURFACE9* ppDep, D3DVIEWPORT9*	pVp)
{
	CHECK_FAILED( m_pd3dDevice->GetRenderTarget(0, ppSuf) );
	CHECK_FAILED( m_pd3dDevice->GetDepthStencilSurface(ppDep));
	CHECK_FAILED( m_pd3dDevice->GetViewport(pVp));

	return S_OK;
}
HRESULT	SexySpike::SetRenderTarget(LPDIRECT3DSURFACE9	pSuf, LPDIRECT3DSURFACE9 pDep, D3DVIEWPORT9*	pVp)
{
	CHECK_FAILED( m_pd3dDevice->SetRenderTarget(0, pSuf));
	CHECK_FAILED( m_pd3dDevice->SetDepthStencilSurface(pDep));
	CHECK_FAILED( m_pd3dDevice->SetViewport(pVp));

	return S_OK;
}

const	void*	LookUpPlayer( PLAY_STATE STATE, eSide SIDE )
{
	CPlayer*		pPlayer	= NULL;
	const PLAYINFO*	pInfo	= NULL;

	for( int i=0; i<NUMPLAYERS; ++i )
	{
		pPlayer	= (CPlayer*)g_ppPlayers[i];
		pInfo	= pPlayer->GetPlayInfo();
		PLAY_STATE OppoState = pInfo->PLAYSTATE == SERVE? OFFENSE: pInfo->PLAYSTATE;
		if( OppoState == STATE && (pInfo->SIDE&SIDE) )
		{
			return	pPlayer;
		}
	}

	return NULL;
}


