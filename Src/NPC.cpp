
#include	"../Include/DEFAULT_INCLUDE.h"
#include	"../Include/Frame.h"
#include	"../Include/CharacterAnimationController.h"
#include	"../Include/Ball.h"
#include	"../Include/Character.h"
#include	"../Include/Player.h"
#include	"../Include/NPC.h"

CNpc::~CNpc()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

CNpc*		CNpc::Create( const Character& SrcModel, int id, LPCSTR pName )
{
	CNpc	*pNpc	= NULL;
	HRESULT	hr		= S_OK;

	pNpc			= new	CNpc( SrcModel, id, pName );
	if( NULL == pNpc )
		return	NULL;

	CharacterAbility	Ability;
	Ability.Block	= 10;
	Ability.Defense	= 10;
	Ability.Lucky	= 10;
	Ability.Speed	= 10;
	Ability.Spike	= 8;
	pNpc->SetAbility( Ability );
	pNpc->SetIsNpc( TRUE );
	pNpc->SetSelfUpdate( TRUE );

	//	컈릭터 몸체에 대한 경계구 생성
	hr	= pNpc->CreateBoundingObjects();
	if( FAILED( hr ) )
	{
		SAFE_DELETE( pNpc );
		return	NULL;
	}

	return	pNpc;
}
HRESULT		CNpc::FrameMove( float fElapsedTime )
{
	HRESULT	hr	= S_OK;

	float fMove = fElapsedTime * SPEED_GAME;

	//	애니메이션 상태 갱신( 여러 인스턴스에서 같이 사용하므로 FrameMove와 Render두군데서 갱신한다.
	m_pAniController->FrameMove(0.0f);
	UpdateFrameMatrices(m_pFrameRoot, &m_matWorld);

	m_fElapsedTime	= fElapsedTime;

	if( m_bUseMacro )
	{
		ProcessMacro(fMove);
	}
	else if( m_bSelfUpdate && (!m_bNowJumping) && (m_playInfo.TEAMMATE->GetPLAYSTATE() != SERVE) )
	{
		eBehavior	BEHAVIOR	= GetCurrentBehavior();

		if( WAITING==BEHAVIOR || WALKFORWARD==BEHAVIOR || WALKBACKWARD==BEHAVIOR || WALKLEFT==BEHAVIOR || WALKRIGHT==BEHAVIOR )
		{
			BEHAVIOR	= GetNextBehavior();

			if( WAITING != BEHAVIOR && this != g_pPlayerLastTouch )
			{
				SetMacro(BEHAVIOR);
			}
			else
			{
				D3DXVECTOR3	vPos		= D3DXVECTOR3( m_matT._41, 0, m_matT._43 );
				D3DXVECTOR3	vTeamPos	= m_playInfo.TEAMMATE->GetPosition();
				vTeamPos.y				= 0.f;
				D3DXVECTOR3	vSub		= D3DXVECTOR3(0.f, 0.f, 0.f);
				CPlayer*	pOppo	= NULL;

				if( (fabs(vPos.x)>fabs(vTeamPos.x) && m_playInfo.SIDE&FRONT) || (fabs(vPos.x)<fabs(vTeamPos.x) && m_playInfo.SIDE&BACK) )
				{
					m_playInfo.TEAMMATE->SetSide( m_playInfo.SIDE );
					m_playInfo.SIDE ^= (FRONT | BACK);
				}

				if( m_playInfo.PLAYSTATE==OFFENSE )
				{
					vSub = m_vAttackingPos-vPos;
				}
				else if( m_playInfo.PLAYSTATE==DEFENSE )
				{
					CPlayer* player1	= (CPlayer*)g_Teams[1-m_playInfo.TEAM].PLAYER1;
					CPlayer* player2	= (CPlayer*)g_Teams[1-m_playInfo.TEAM].PLAYER2;

					if( (m_playInfo.SIDE&FRONT) )
					{
						pOppo	= g_CntBallTouched<0? (player1->GetSide()&FRONT?player1:player2): ((player1!=g_pPlayerLastTouch)? player1: player2);
					}
					else
					{
						pOppo	= g_CntBallTouched<0? (player1->GetSide()&BACK?player1:player2): ((player1==g_pPlayerLastTouch)? player1: player2);
					}

					D3DXVECTOR3		vOppo	= pOppo->GetPosition();
					D3DXVECTOR3		vDest	= D3DXVECTOR3( (m_playInfo.SIDE&FRONT?10.f:50.f), 0.f, vOppo.z );
					vDest.x					= vDest.x*(m_playInfo.SIDE&LEFT?-1.f:1.f);
					vSub					= vDest-vPos;
				}

				if( pOppo!=NULL && pOppo->GetNowJumping() && g_CntBallTouched>0 )
				{
					SetMacro(BLOCKING);
				}
				else if( D3DXVec3Length( &vSub ) > (0==_stricmp( "WAITING", m_pAniController->GetCurrentAnimationSetName())?6.f:3.f) )
				{
					D3DXVec3Normalize( &vSub, &vSub );
					vSub		= 0.3f*vSub;
					m_matT._41 += vSub.x;
					m_matT._43 += vSub.z;
					m_pAniController->SetAnimationSet( "WALKFORWARD" );
				}
				else
				{
					if( 0!=_stricmp( "WAITING", m_pAniController->GetCurrentAnimationSetName() ) )
					{
						m_pAniController->SetAnimationSet( "WAITING" );
					}
				}

			}
		}
	}

	if( m_bNowJumping )
	{
		if( m_fJumpTime >= fMove)
		{
			m_matT._42	+= m_fJumpVelocity * fMove;

			if( m_matT._42 < 0.f )
			{
				m_matT._42		= 0.f;
				m_bNowJumping	= FALSE;
			}
		}
		else
		{
			m_matT._42	= 0.f;
			m_bNowJumping	= FALSE;
		}

		m_fJumpVelocity	+= m_fJumpAccel*fMove;
		m_fJumpTime		-= fMove;
	}

	D3DXMatrixRotationY( &m_matR, m_fRotationAngle );
	m_matWorld	= m_matS*m_matR*m_matT;

	m_pAniController->FrameMove(fMove);
	UpdateFrameMatrices( m_pFrameRoot,  &m_matWorld );	

	m_BoundingBody.UpdateBoundingCenter();
	m_BoundingHand.UpdateBoundingCenter();
	m_BoundingSpike.UpdateBoundingCenter();
	m_BoundingDefense.UpdateBoundingCenter();

	return	hr;
}

void		CNpc::SetMacro(eBehavior BEHAVIOR)
{
	ZeroMemory( &m_macro, sizeof (ValleyActionMacro ) );

	const		LPD3DXVECTOR3	pBallVelocity	= m_pBall->GetVelocity();
	const		LPD3DXVECTOR3	pBallAccel		= m_pBall->GetAccel();
	const		LPD3DXVECTOR3	pBallCenter		= m_pBall->GetBoundingCenter();
	float		fBallRadius						= m_pBall->GetBoundingRadius();

	float		a		= pBallAccel->y;
	float		d		= 0.f;
	float		v		= pBallVelocity->y;
	float		t		= 0;//(-2*v - sqrt( 4*v*v + 8*a*d) ) / (2*a);
	float		fDiff	= 0.f;
	float		fPeriod	= 0.f;
	float		_bb_4ac	= 0.f;
	D3DXVECTOR3	vHandPoint;
	D3DXVECTOR3	vDest;
	D3DXVECTOR3 velocity= D3DXVECTOR3( 0.f, 0.f, 0.f );
	LPCSTR		pAniSetName	= m_pAniController->GetCurrentAnimationSetName();

	if( 0 != _stricmp( "WAITING",  pAniSetName) && 0!=_strnicmp("WALK", pAniSetName, 4) && (THROWING!=BEHAVIOR) )
		return;

	switch( BEHAVIOR )
	{
		case BLOCKING:
		{
			vHandPoint				= m_vBlockingPoint;
			vHandPoint.y			+= m_ability.Defense;
			m_macro.ValleyAction	= BLOCKING;
			m_macro.MovingAction	= WALKFORWARD;
			t						= BLOCKINGTIME;
			fDiff					= t-0.015f;
			vDest					= D3DXVECTOR3( m_matT._41, m_matT._42, m_matT._43 );
			break;
		}
		case RECEIVE:
		{
			vHandPoint				= m_vReceivePoint;
			m_macro.ValleyAction	= RECEIVE;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= RECEIVETIME;
			break;
		}
		case TOSS:
		{
			vHandPoint				= m_vTossPoint;
			m_macro.ValleyAction	= TOSS;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= TOSSTIME;
			break;
		}
		case SPIKE:
		{
			vHandPoint				= m_vSpikePoint;
			vHandPoint.y			+= m_ability.Spike;
			m_macro.ValleyAction	= SPIKE;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= SPIKETIME;
			break;
		}
		case SERVE_OVER:
		{
			vHandPoint				= m_vOverServePoint;
			m_macro.ValleyAction	= SERVE_OVER;
			m_macro.MovingAction	= WAITING;
			fDiff					= SERVEOVERTIME;
			break;
		}
		case SERVE_UNDER:
		{
			vHandPoint				= m_vUnderServePoint;
			m_macro.ValleyAction	= SERVE_UNDER;
			m_macro.MovingAction	= WAITING;
			fDiff					= SERVEUNDERTIME;
			break;
		}
		case THROWING:
		{
			m_macro.MacroTime		= THROWINGTIME;
			m_macro.TransitionPoint	= THROWINGTIME;
			m_macro.MovingAction	= THROWING;
			m_macro.ValleyAction	= THROWING;
			m_macro.Destination		= D3DXVECTOR3( m_matT._41, m_matT._42, m_matT._43 ) ;
			m_bUseMacro				= TRUE;
			m_pBall->SetVelocity( D3DXVECTOR3(0,0,0) );
			return;
		}

		default : return;
	}

	//	공의 낙하시간과 낙하지점을 구한다
	if( BLOCKING != BEHAVIOR )
	{
		d		= SPIKE==BEHAVIOR? vHandPoint.y-pBallCenter->y: vHandPoint.y - (pBallCenter->y-fBallRadius);
		_bb_4ac	= 4*v*v + 8*a*d;
		t		= (-2*v - sqrt( (_bb_4ac<0.f)? 0.f: _bb_4ac) ) / (2*a);

		vDest	= *pBallCenter + t**pBallVelocity + 0.5f*t*t**pBallAccel;
		
		vDest.x -= vHandPoint.x;
		vDest.z -= vHandPoint.z;
	}

	if( SERVE_OVER != BEHAVIOR && SERVE_UNDER != BEHAVIOR)
	{
		velocity.x = vDest.x - m_matT._41;
		velocity.z = vDest.z - m_matT._43;
	}

	if( (t-fDiff) > EPSILON )
	{
		velocity = velocity/(t-fDiff);
		m_macro.MacroTime		= t;
		m_macro.TransitionPoint	= t-fDiff;
		m_macro.Velocity		= velocity;
		m_macro.Destination		= vDest;
		m_bUseMacro				= TRUE;
	}
	else
	{
		m_matT._41 = vDest.x;
		m_matT._43 = vDest.z;
		m_pAniController->SetAnimationSet( g_arrBehaviorName[m_macro.ValleyAction]);
	}
}



eBehavior	CNpc::GetNextBehavior()
{
	float		height		= m_pBall->GetHeight();
	D3DXVECTOR3 vFalling	= m_pBall->GetFallingSpot();
	float		distance	= D3DXVec3Length( &( m_BoundingHand.vCenter - vFalling ) );
	eBehavior	BEHAVIOR	= WAITING;

	if( this == g_pPlayerRedShift && (vFalling.x*m_matT._41)>=0.f )
	{
		if( height > (m_vSpikePoint.y+m_ability.Spike) )
		{
			if( SERVE==m_playInfo.PLAYSTATE)
			{
				int iServe	= rand()%3;
				BEHAVIOR	= iServe==0? SERVE_OVER: iServe==1? SERVE_UNDER: SPIKE;
			}
			else
			{
				if( 0==g_CntBallTouched || (m_eAttackingSide>ML && g_CntBallTouched<2) )
					BEHAVIOR	= TOSS;
				else
					BEHAVIOR	= SPIKE;
			}
		}
		else if( height > RECEIVEPOINTHIGH )
			BEHAVIOR	= TOSS;
		else if( height > RECEIVEPOINTLOW )
			BEHAVIOR	= RECEIVE;
		else
			BEHAVIOR	= SLIDING;
	}

	return BEHAVIOR;
}
void		CNpc::ProcessMacro(float fElapsedTime)
{
	if( m_macro.ElapsedTime == 0.f )
	{
		if( m_macro.TransitionPoint > 0.f )
		{
			m_pAniController->SetAnimationSet( g_arrBehaviorName[m_macro.MovingAction] );
		}
	}

	if( (m_macro.ElapsedTime + fElapsedTime) <= m_macro.TransitionPoint )
	{
		m_matT._41 += fElapsedTime*m_macro.Velocity.x;
		m_matT._43 += fElapsedTime*m_macro.Velocity.z;

		if( m_macro.MovingAction == THROWING )
		{
			m_pBall->SetPosition( m_BoundingHand.pTM->_41, m_BoundingHand.pTM->_42+0.5f, m_BoundingHand.pTM->_43 );
		}
	}
	else if( m_macro.ElapsedTime <= m_macro.TransitionPoint )
	{
		m_matT._41 = m_macro.Destination.x;
		m_matT._43 = m_macro.Destination.z;

		m_pAniController->SetAnimationSet( g_arrBehaviorName[m_macro.ValleyAction] );

		if( SPIKE == m_macro.ValleyAction || BLOCKING == m_macro.ValleyAction )
		{
			m_bNowJumping	= TRUE;
			m_fJumpTime		= m_macro.MacroTime - m_macro.ElapsedTime;
			m_fJumpVelocity	= ( (m_macro.ValleyAction==SPIKE? 2.f: 1.f)*m_ability.Spike)/m_fJumpTime;
			m_fJumpAccel	= (2*m_ability.Spike-2*m_fJumpVelocity*m_fJumpTime)/(m_fJumpTime*m_fJumpTime);
			m_fJumpTime		= ((-2.f*m_fJumpVelocity)-sqrt((4*m_fJumpVelocity*m_fJumpVelocity)+(4.f*m_fJumpAccel)))/(2.f*m_fJumpAccel);
		}
	}

	m_macro.ElapsedTime += fElapsedTime;

	if( (m_macro.ElapsedTime >= m_macro.MacroTime ) )
	{
		m_bUseMacro = FALSE;

		if( m_macro.ValleyAction == SLIDING )
		{
			m_fRotationAngle	= m_playInfo.SIDE&LEFT? 1.5f*D3DX_PI : 0.5f*D3DX_PI ;
		}
		else if( m_macro.ValleyAction == THROWING )
		{
			m_pBall->ReflectByXXX( NULL, THROWING, 1.f );
			g_pPlayerRedShift	= NULL;
			g_bNowServe			= FALSE;
		}
	}
}