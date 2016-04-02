
#include	"../Include/DEFAULT_INCLUDE.h"
#include	"../Include/Frame.h"
#include	"../Include/CharacterAnimationController.h"
#include	"../Include/Ball.h"
#include	"../Include/Character.h"
#include	"../Include/Player.h"
#include	"../Include/USER.h"

CUser::~CUser()
{
}

CUser*		CUser::Create( const Character& SrcModel, int id, LPCSTR pName )
{
	CUser	*pUser	= NULL;
	HRESULT	hr		= S_OK;

	pUser			= new	CUser( SrcModel, id, pName );
	if( NULL == pUser )
		return	NULL;

	CharacterAbility	Ability;
	Ability.Block	= 8;
	Ability.Defense	= 8;
	Ability.Lucky	= 8;
	Ability.Speed	= 8;
	Ability.Spike	= 8;
	pUser->SetAbility( Ability );
	pUser->SetIsNpc( FALSE );
	pUser->SetSelfUpdate( TRUE );
	pUser->SetMoveLock( FALSE );

	//	컈릭터 몸체에 대한 경계구 생성
	hr	= pUser->CreateBoundingObjects();
	if( FAILED( hr ) )
	{
		SAFE_DELETE( pUser );
		return	NULL;
	}

	return	pUser;
}
void		CUser::ConfirmCharacterKeyState()
{
	if( !g_bNowServe && g_keys.GetCurrentState( 'Z' ) )
	{
		SetMacro(GetNextBehavior());
		return;
	}

	if( g_keys.GetCurrentState( 'X' ) )
	{
		//	공격시 스파이크, 수비시 블록킹
		SetMacro(GetNextBehavior());
		return;
	}

	if( g_keys.GetCurrentState( 'C' ) )
	{
		m_pAniController->SetAnimationSet( ("THROWING") );
	}

	if( g_keys.GetCurrentState( 'N' ) )
	{
		//m_pAniController->SetAnimationSet( ("BLOCKING") );
	}

	if( g_keys.GetCurrentState( 'B' ) )
	{
		m_playInfo.PLAYSTATE	=  SERVE == m_playInfo.PLAYSTATE? OFFENSE : SERVE;
	}

	eBehavior	eCurrent = this->GetCurrentBehavior();
	if(	!m_bNowJumping && !m_bMoveLock )
	{
		if( g_keys.GetCurrentState( VK_UP ) )
		{
			//	이전에 키가 줄려진 상태면 동작전환은 안한다.
			if( !g_keys.GetPreviousState( VK_UP ) )
			{
				m_pAniController->SetAnimationSet( ("WALKFORWARD") );
			}
			m_matT._41 += g_dir[U].x;
			m_matT._43 += g_dir[U].z;
		}

		if( g_keys.GetCurrentState( VK_LEFT ) )
		{
			if( !g_keys.GetPreviousState( VK_LEFT ) )
			{
				m_pAniController->SetAnimationSet( ("WALKLEFT") );
			}
			m_matT._41 += g_dir[L].x;
			m_matT._43 += g_dir[L].z;
		}

		if( g_keys.GetCurrentState( VK_DOWN ) )
		{
			if( !g_keys.GetPreviousState( VK_DOWN ) )
			{
				m_pAniController->SetAnimationSet( ("WALKBACKWARD") );
			}
			m_matT._41 += g_dir[B].x;
			m_matT._43 += g_dir[B].z;
		}

		if( g_keys.GetCurrentState( VK_RIGHT ) )
		{
			if( !g_keys.GetPreviousState( VK_RIGHT ) )
			{
				m_pAniController->SetAnimationSet( ("WALKRIGHT") );
			}
			m_matT._41 += g_dir[R].x;
			m_matT._43 += g_dir[R].z;
		}

		//	캐릭터가 움직인 경우
		if( g_keys.GetCurrentState( VK_UP ) || g_keys.GetCurrentState( VK_LEFT ) || g_keys.GetCurrentState( VK_DOWN ) || g_keys.GetCurrentState( VK_RIGHT )  )
		{
		}
		//	방향키를 눌렀다 뗀 경우
		else if( ( g_keys.GetPreviousState( VK_UP ) || g_keys.GetPreviousState( VK_LEFT ) || g_keys.GetPreviousState( VK_DOWN ) || g_keys.GetPreviousState( VK_RIGHT ) ) )
		{
			m_pAniController->SetAnimationSet( ("WAITING") );
		}
	}
}


HRESULT		CUser::FrameMove( float fElapsedTime ) 
{
	HRESULT	hr	= S_OK;

	m_pAniController->FrameMove(0.0f);
	UpdateFrameMatrices(m_pFrameRoot, &m_matWorld);

	m_fElapsedTime	= fElapsedTime;

	if( m_bUseMacro )
		ProcessMacro(fElapsedTime);
	else
		ConfirmCharacterKeyState();

	if( m_bNowJumping )
	{
		if( m_fJumpTime >= fElapsedTime )
		{
			m_matT._42	+= m_fJumpVelocity * fElapsedTime;
		}
		else
		{
			m_matT._42	= 0.f;
			m_bNowJumping	= FALSE;
		}

		m_fJumpVelocity	+= m_fJumpAccel*fElapsedTime;
		m_fJumpTime		-= fElapsedTime;
	}

	D3DXMatrixRotationY( &m_matR, m_fRotationAngle );
	m_matWorld	= m_matS*m_matR*m_matT;

	m_pAniController->FrameMove( m_fElapsedTime );	
	UpdateFrameMatrices( m_pFrameRoot,  &m_matWorld );	

	m_BoundingBody.UpdateBoundingCenter();
	m_BoundingHand.UpdateBoundingCenter();
	m_BoundingSpike.UpdateBoundingCenter();
	m_BoundingDefense.UpdateBoundingCenter();

	return	hr;
}
eBehavior	CUser::GetNextBehavior()
{
	float		height		= m_pBall->GetHeight();
	eBehavior	eBehavior	= WAITING;

	if( g_keys.GetCurrentState('X') )
	{
		if( OFFENSE == m_playInfo.PLAYSTATE )
		{
			eBehavior	= SPIKE;
		}
		else if( SERVE == m_playInfo.PLAYSTATE )
		{
			if( g_keys.GetCurrentState( VK_UP ) )
			{
				eBehavior	= SPIKE;
			}
			else if( g_keys.GetCurrentState( VK_DOWN ) )
			{
				eBehavior	= SERVE_UNDER;
			}
			else
			{
				eBehavior	= SERVE_OVER;
			}
		}
		else if( DEFENSE == m_playInfo.PLAYSTATE )
		{
			eBehavior	= BLOCKING;
		}
	}
	else if( !g_bNowServe && g_keys.GetCurrentState('Z') )
	{
		if( height > RECEIVEPOINTHIGH )
		{
			eBehavior	= TOSS;
		}
		else if( height <= RECEIVEPOINTHIGH && height >= RECEIVEPOINTLOW )
		{
			eBehavior	= RECEIVE;
		}
		else
		{
			float d	= D3DXVec3Length( &( *(m_pBall->GetBoundingCenter()) - m_BoundingHand.vCenter ) );
			if( d > SLIDINGRADIUSNEAR && d < SLIDINGRADIUSFAR )
				eBehavior	= SLIDING;
			else
				eBehavior	= RECEIVE;
		}
	}

	return eBehavior;
}
void		CUser::SetMacro(eBehavior BEHAVIOR)
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
	float		_bb_4ac	= 0.f;
	D3DXVECTOR3	vHandPoint;
	D3DXVECTOR3	vDest;
	D3DXVECTOR3 velocity= D3DXVECTOR3( 0.f, 0.f, 0.f );

	//	매크로가 필요 없는 경우
	if( (this!=g_pPlayerRedShift) && THROWING!=BEHAVIOR && SLIDING!=BEHAVIOR )
	{
		m_pAniController->SetAnimationSet( g_arrBehaviorName[ BEHAVIOR ]);
		return;
	}

	switch( BEHAVIOR )
	{
		case BLOCKING:
		{
			vHandPoint				= m_vBlockingPoint;
			vHandPoint.y			+= m_ability.Defense;
			m_macro.ValleyAction	= BLOCKING;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= (float)m_pAniController->GetPeriod( "BLOCKING" );

			t						= fDiff;
			fDiff					= fDiff-0.015f;
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
		case SLIDING:
		{
			vHandPoint				= m_vSlidingPoint;
			m_macro.ValleyAction	= SLIDING;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= (float)m_pAniController->GetPeriod( "SLIDING" );
			break;
		}
		case SPIKE:
		{
			float	height			= m_pBall->GetHeight();
			vHandPoint				= m_vSpikePoint;
			vHandPoint.y			+= m_ability.Spike;
			if( vHandPoint.y > height )
			{
				m_pAniController->SetAnimationSet( g_arrBehaviorName[ GetNextBehavior() ] );
				return;
			}

			m_macro.ValleyAction	= SPIKE;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= SPIKETIME;
			break;
		}
		case SERVE_OVER:
		{
			vHandPoint				= m_vOverServePoint;
			m_macro.ValleyAction	= SERVE_OVER;
			m_macro.MovingAction	= WALKFORWARD;
			fDiff					= SERVEOVERTIME;
			break;
		}
		case SERVE_UNDER:
		{
			vHandPoint				= m_vUnderServePoint;
			m_macro.ValleyAction	= SERVE_UNDER;
			m_macro.MovingAction	= WALKFORWARD;
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
	}

	//	공의 낙하시간과 낙하지점을 구한다
	d		= (SPIKE==BEHAVIOR||SERVE_OVER==BEHAVIOR)? vHandPoint.y-pBallCenter->y: vHandPoint.y - (pBallCenter->y-fBallRadius);
	_bb_4ac	= 4*v*v + 8*a*d;
	t		= (-2*v - sqrt((_bb_4ac<0.f)? 0.f : _bb_4ac) ) / (2*a);
	vDest	= *pBallCenter + t**pBallVelocity + 0.5f*t*t**pBallAccel;
	
	//	몸의 위치와 손의 위치에 따른 보정치, 슬라이딩은 회전도 필요
	if( BEHAVIOR == SLIDING )
	{
		//	회전( 이동보다 먼저 계산해야 한다 )
		D3DXVECTOR3 vA	= D3DXVECTOR3( 0, 0, -1.f );
		D3DXVECTOR3	vB	= vDest - D3DXVECTOR3( m_matT._41, vDest.y, m_matT._43 );
		D3DXVec3Normalize( &vB, &vB );
		float fCos = D3DXVec3Dot( &vA, &vB );
		float fAcos = acosf( fCos ) * (vDest.x > m_BoundingBody.vCenter.x? -1.f: 1.f);
		m_fRotationAngle = fAcos;

		//	이동
		D3DXVECTOR3 vDirection = D3DXVECTOR3(vDest.x - m_matT._41, 0.f, vDest.z - m_matT._43 );
		D3DXVECTOR3 vDirectionNormal;
		D3DXVec3Normalize( &vDirectionNormal, &vDirection );
		float length1 = D3DXVec3Length( &vHandPoint );
		float length2 = D3DXVec3Length( &vDirection );
		D3DXVECTOR3 vMove = (length2-length1)*vDirectionNormal;
		if( length2 > SLIDINGRADIUSFAR )		// 슬라이딩거리보다 먼경우
		{
			vMove = D3DXVECTOR3( 0, 0, 0 );
		}
		vDest.x = m_matT._41 + vMove.x;
		vDest.z = m_matT._43 + vMove.z;

		//	슬라이딩은 무조건 매크로사용( 몸 회전 방향을 원래대로 돌리기 위해 )
		t = fDiff;
		fDiff = t - 2*EPSILON;
	}
	else
	{
		vDest.x -= vHandPoint.x;
		vDest.z -= vHandPoint.z;
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
		m_bMoveLock				= TRUE;
	}
	else
	{
		m_matT._41 = vDest.x;
		m_matT._43 = vDest.z;
		m_pAniController->SetAnimationSet( g_arrBehaviorName[m_macro.ValleyAction]);
	}
}

void		CUser::ProcessMacro(float fElapsedTime)
{
	if( m_macro.ElapsedTime == 0.f )
	{
		if( m_macro.TransitionPoint > 0.f )
		{
			m_pAniController->SetAnimationSet( g_arrBehaviorName[m_macro.MovingAction] );
		}

		m_bMoveLock	= TRUE;
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

		if( m_macro.ValleyAction == THROWING )
		{
			m_pBall->ReflectByXXX( NULL, THROWING, 1.f );
			g_pPlayerRedShift	= NULL;
			g_bNowServe			= FALSE;
			m_bMoveLock			= TRUE;
		}
	}
}

