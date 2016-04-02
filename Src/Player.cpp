
#include	"../Include/DEFAULT_INCLUDE.h"
#include	"../Network/_NetworkAfx.h"
#include	"../Network/Packet.h"
#include	"../Network/RingBuffer.h"
#include	"../Network/Client.h"
#include	"../Include/Frame.h"
#include	"../Include/CharacterAnimationController.h"
#include	"../Include/Ball.h"
#include	"../Include/Character.h"
#include	"../Include/Player.h"

CPlayer::CPlayer(const Character &SrcModel, int id, LPCSTR pName ) : Character( SrcModel )
{
	m_id	= id;
	CopyString( pName, &m_pName );
	m_bUseMacro			= FALSE;
	m_bIsAttacker		= FALSE;
	m_bDrawShadow		= TRUE;
	m_vAttackingPos		= D3DXVECTOR3(0, 0, 0);
	ZeroMemory( &m_playInfo, sizeof( PLAYINFO ) );
	ZeroMemory( &m_macro, sizeof( ValleyActionMacro ) );
}

CPlayer::~CPlayer()
{
	SAFE_DELETE_ARRAY( m_pName );
}

bool		CPlayer::operator ==(LPCSTR cmp)
{
	return	0 == _stricmp( m_pName, cmp );
}

bool		CPlayer::operator ==( CPlayer* cmp )
{
	return	this == cmp;
}

void		CPlayer::SetPosition( float x, float y, float z )
{
	m_matT._41	= x;
	m_matT._42	= y;
	m_matT._43	= z;
}

eBehavior	CPlayer::GetCurrentBehavior()
{
	LPCSTR	pName	= m_pAniController->GetCurrentAnimationSetName();

	for( int i=0; i<sizeof(g_arrBehaviorName)/sizeof(g_arrBehaviorName[0]); ++i )
	{
		if( 0 == _strcmpi( g_arrBehaviorName[i], pName ) )
		{
			return (eBehavior)i;
		}
	}

	return	WAITING;
}

void		CPlayer::SendPlayerStateToNetwork()
{
	if( m_bSelfUpdate )
	{
		eBehavior	B	= GetCurrentBehavior();

		if
		(		m_lastPacket.BEHAVIOR != B
			||	m_lastPacket.ROTATION != m_fRotationAngle
			||	fabs(m_lastPacket.POSITION.x - m_matT._41) > EPSILON
			||	fabs(m_lastPacket.POSITION.y - m_matT._42) > EPSILON
			||	fabs(m_lastPacket.POSITION.z - m_matT._43) > EPSILON
		)
		{
			m_lastPacket = PlayerPacket( m_pName, B, m_fRotationAngle, m_matT._41, m_matT._42, m_matT._43 );
			g_pNetAdapter->SendPacket(
									NULL
									,CLIENT_ROOM
									,CLIENT_ROOM_PLAY
									,NULL
									,NULL
									,NULL
									,U_CHARACTER
									,sizeof(m_lastPacket)
									,&m_lastPacket);
		}
	}
}

void		CPlayer::UpdatePlayerStateFromNetwork(const PlayerPacket *packet)
{
	if( !m_bSelfUpdate )
	{
		m_pAniController->SetAnimationSet( g_arrBehaviorName[packet->BEHAVIOR] );
		m_fRotationAngle	= packet->ROTATION;
		m_matT._41			= packet->POSITION.x;
		m_matT._42			= packet->POSITION.y;
		m_matT._43			= packet->POSITION.z;
	}
}