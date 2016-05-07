#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Network/_NetworkAfx.h"
#include		"../Network/Packet.h"
#include		"../Network/RingBuffer.h"
#include		"../Network/Client.h"
#include		"../Include/Ball.h"
CBall::CBall()
{
	m_pBall					= NULL;	
	m_pMaterials			= NULL;
	m_ppTex					= NULL;
	m_pFallingTex			= NULL;
	m_numMaterials			= 0;
	m_fRotationAngle		= 0.f;
	m_balltype				= COMMON;
	m_pBlackholeTex			= NULL;
	m_pBallVB				= NULL;
	m_fBallWeight			= 0.f;
	m_fBallIncr			= 0.f;
	m_vInitPosition			= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_vVelocity				= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_vAccel				= D3DXVECTOR3( 0.f, -12.f, 0.f );
	m_bUseMacro				= FALSE;
	m_bShowFallingPoint		= TRUE;
	ZeroMemory( &m_macro,	sizeof(BallMacro) );
	ZeroMemory( &m_BoundingBall, sizeof( BoundingSphere) );

	D3DXMatrixIdentity( &m_matS );
	D3DXMatrixIdentity( &m_matR );
	D3DXMatrixIdentity( &m_matT );
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixIdentity( &m_matFalling );
	ZeroMemory( m_pName,	sizeof( m_pName) );
	ZeroMemory( m_pFalling,	4* sizeof(SimpleVertex) );
	ZeroMemory( &m_vRotationAxis, sizeof D3DXVECTOR3 );

	strcpy_s( m_pName, sizeof(m_pName), "BALL" );

	ComputeRotationAxis();
}

CBall::~CBall()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

HRESULT		CBall::InitDeviceObjects( LPDIRECT3DDEVICE9	pDevice )
{
	HRESULT		hr	= S_OK;

	assert( pDevice );

	D3DXMatrixIdentity( &m_matS );
	D3DXMatrixIdentity( &m_matR );
	D3DXMatrixIdentity( &m_matT );
	D3DXMatrixIdentity( &m_matWorld );

	//	Ball 메쉬생성
	LPD3DXMESH		pMesh			= NULL;
	LPD3DXBUFFER	pMaterialBuf	= NULL;
	LPD3DXBUFFER	pAdjacency		= NULL;
	hr	= D3DXLoadMeshFromX( 
				"./XFILE/BALL/BALL.x"
			,	D3DXMESH_MANAGED
			,	pDevice
			,	&pAdjacency
			,	&pMaterialBuf
			,	NULL
			,	&m_numMaterials
			,	&pMesh
			);
	CHECK_FAILED( hr );

	hr	= pMesh->Optimize( 
				D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE
			,	(DWORD*)pAdjacency->GetBufferPointer()
			,	NULL
			,	NULL
			,	NULL
			,	&m_pBall
			);


	m_pMaterials	= new D3DMATERIAL9[ m_numMaterials ];
	m_ppTex			= new LPDIRECT3DTEXTURE9[ m_numMaterials ];

	if( NULL == m_pMaterials || NULL == m_ppTex )
	{
		SAFE_DELETE_ARRAY( m_pMaterials );
		SAFE_DELETE_ARRAY( m_ppTex );
		SAFE_RELEASE( pMaterialBuf );
		return	E_OUTOFMEMORY;
	}

	ZeroMemory( m_pMaterials, m_numMaterials * sizeof(D3DMATERIAL9) );
	ZeroMemory( m_ppTex, m_numMaterials * sizeof(LPDIRECT3DTEXTURE9) );
	D3DXMATERIAL	*pMat	= (D3DXMATERIAL*)pMaterialBuf->GetBufferPointer();
	for( DWORD i=0; i<m_numMaterials; ++i )
	{
		m_pMaterials[i]			= pMat[i].MatD3D;
		m_pMaterials[i].Ambient	= m_pMaterials[i].Diffuse;

		if( NULL != pMat[i].pTextureFilename && 0 < strlen(pMat->pTextureFilename) )
		{
			hr	= D3DXCreateTextureFromFile( pDevice, pMat[i].pTextureFilename, &m_ppTex[i] );
			if( FAILED(hr) )
			{
				//	경로를 바꿔서 다시 시도
				CHAR	tempFilename[MAX_PATH] = {"./XFILE/BALL/"};
				strcat_s( tempFilename, MAX_PATH, pMat[i].pTextureFilename );
				hr = D3DXCreateTextureFromFileA( pDevice, tempFilename, &m_ppTex[i] );

				//	최종적으로 텍스쳐파일을 찾지못한 경우에는 NULL을 입력
				if( FAILED(hr) ) 
					m_ppTex[i] = NULL;
			}
		}
		else
		{
			m_ppTex[i]	= NULL;
		}
	}

	//	경계구 생성
	LPDIRECT3DVERTEXBUFFER9	pVB	= NULL;

	char	*pFirstPosition		= NULL;
	m_pBall->LockVertexBuffer( 0, (void**) &pFirstPosition );

	D3DXComputeBoundingSphere(
				(LPD3DXVECTOR3)pFirstPosition
			,	m_pBall->GetNumVertices()
			,	D3DXGetFVFVertexSize( m_pBall->GetFVF())
			,	&m_BoundingBall.vCenter
			,	&m_BoundingBall.fRadius
			);
	m_BoundingBall.vDiff.x	= m_matT._41 - m_BoundingBall.vCenter.x;
	m_BoundingBall.vDiff.y	= m_matT._42 - m_BoundingBall.vCenter.y;
	m_BoundingBall.vDiff.z	= m_matT._43 - m_BoundingBall.vCenter.z;
	m_BoundingBall.pTM		= &m_matT;

	m_pBall->UnlockVertexBuffer();

	//	낙하지점 생성
	hr	= D3DXCreateTextureFromFileEx(
						pDevice
					,	"./TEXTURE/FallingSpot.png"
					,	D3DX_DEFAULT
					,	D3DX_DEFAULT
					,	D3DX_DEFAULT
					,	0
					,	D3DFMT_UNKNOWN
					,	D3DPOOL_MANAGED
					,	D3DX_DEFAULT
					,	D3DX_DEFAULT
					,	0xFFFFFFFF				//	흰색을 뺀다
					,	NULL
					,	NULL
					,	&m_pFallingTex
					);

	m_pFalling[0].p	= D3DXVECTOR3(-1.f, 0.f, -1.f);
	m_pFalling[1].p	= D3DXVECTOR3(-1.f, 0.f,  1.f);
	m_pFalling[2].p	= D3DXVECTOR3( 1.f, 0.f,  1.f);
	m_pFalling[3].p	= D3DXVECTOR3( 1.f, 0.f, -1.f);

	m_pFalling[0].tu= 0.f;	m_pFalling[0].tv= 1.f;
	m_pFalling[1].tu= 0.f;	m_pFalling[1].tv= 0.f;
	m_pFalling[2].tu= 1.f;	m_pFalling[2].tv= 0.f;
	m_pFalling[3].tu= 1.f;	m_pFalling[3].tv= 1.f;

	SAFE_RELEASE( pMaterialBuf );
	SAFE_RELEASE( pAdjacency );
	SAFE_RELEASE( pMesh );

	//	Create BLACKHOLE Image
	D3DXCreateTextureFromFile( pDevice, "./TEXTURE/BLACKHOLE.tga", &m_pBlackholeTex );

	return		hr;
}
HRESULT		CBall::RestoreDeviceObjects(LPDIRECT3DDEVICE9	pDevice)
{
	HRESULT		hr	= S_OK;

	SimpleVertex	VB[4];
	float	W = 1.2f*m_BoundingBall.fRadius;
	VB[0].p	= D3DXVECTOR3(-W, -W, 0.f);
	VB[1].p	= D3DXVECTOR3(-W,  W, 0.f);
	VB[2].p	= D3DXVECTOR3( W,  W, 0.f);
	VB[3].p	= D3DXVECTOR3( W, -W, 0.f);
	VB[0].tu= 0.f;	VB[0].tv= 1.f;
	VB[1].tu= 0.f;	VB[1].tv= 0.f;
	VB[2].tu= 1.f;	VB[2].tv= 0.f;
	VB[3].tu= 1.f;	VB[3].tv= 1.f;

	hr	= pDevice->CreateVertexBuffer(4*sizeof(SimpleVertex), 0, SimpleVertex::SimpleVertexFVF, D3DPOOL_DEFAULT, &m_pBallVB, NULL );
	CHECK_FAILED(hr);

	SimpleVertex	*pBLACKHOLEVB = NULL;
	m_pBallVB->Lock(0, 0, (void**)&pBLACKHOLEVB, 0 );
	memcpy( pBLACKHOLEVB, VB, 4*sizeof(SimpleVertex) );
	m_pBallVB->Unlock();

	return		hr;
}
HRESULT		CBall::InvalidateDeviceObjects()
{
	HRESULT		hr	= S_OK;

	SAFE_RELEASE( m_pBallVB );

	return		hr;
}
HRESULT		CBall::DeleteDeviceObjects()
{
	HRESULT		hr	= S_OK;

	if( m_ppTex )
	{
		for( DWORD i=0; i<m_numMaterials; ++i )
		{
			SAFE_RELEASE( m_ppTex[i] );
		}
	}

	SAFE_DELETE_ARRAY( m_ppTex );
	SAFE_DELETE_ARRAY( m_pMaterials );
	
	SAFE_RELEASE( m_pBall );
	SAFE_RELEASE( m_pFallingTex );

	SAFE_RELEASE( m_pBlackholeTex );

	return		hr;
}

HRESULT		CBall::FrameMove( float	fElapsedTime )
{
	HRESULT		hr	= S_OK;

	m_fRotationAngle	+= 0.1f*D3DXVec3Length( &m_vVelocity );
	D3DXMatrixRotationAxis( &m_matR, &m_vRotationAxis, D3DXToRadian(m_fRotationAngle) );

	if( !m_bUseMacro )
	{
		m_matT._41	+= fElapsedTime*m_vVelocity.x;
		m_matT._42	+= fElapsedTime*m_vVelocity.y;
		m_matT._43	+= fElapsedTime*m_vVelocity.z;
		m_vVelocity	+= fElapsedTime*m_vAccel;
	}
	else
	{
		ProcessMacro(fElapsedTime);
	}

	m_matWorld	= m_matR*m_matT;
	m_BoundingBall.UpdateBoundingCenter();

	ComputeFallingSpot( &m_matFalling );

	if( !m_bUseMacro )
	{
		m_eSideBefore	= m_eSideCurrent;
		m_eSideCurrent	= m_BoundingBall.vCenter.x>0.f? RIGHT: LEFT;
	}

	switch( m_balltype )
	{
	case BLACKHOLE:
		{
			SimpleVertex	*pVB	= NULL;
			m_pBallVB->Lock(0, 0, (void**)&pVB, 0);
			pVB[0].p *= 1.05f;
			pVB[1].p *= 1.05f;
			pVB[2].p *= 1.05f;
			pVB[3].p *= 1.05f;
			pVB[0].tu	= fabs(pVB[0].tu-0.875f)<EPSILON? 0.f: pVB[0].tu+0.125f;
			pVB[1].tu	= pVB[0].tu;
			pVB[2].tu	= pVB[0].tu+0.125f;
			pVB[3].tu	= pVB[0].tu+0.125f;
			m_pBallVB->Unlock();
			//m_fBallWeight += m_fBallIncr;
			//m_fBallIncr	= (m_fBallWeight>2.f||m_fBallWeight<0.5f)?-m_fBallIncr:m_fBallIncr;
		}
		break;
	}

	if( g_bNowServe )
	{
#ifdef	_DEBUG
		sprintf_s( g_buf, 256, "%3.f, %3.f, %3.f, %3.f, %3.f, %3.f", m_matT._41, m_matT._42, m_matT._43, m_vVelocity.x, m_vVelocity.y, m_vVelocity.z);
#endif
		//SetWindowText(GetActiveWindow(), g_buf);
	}

	return		hr;
}

HRESULT		CBall::Render(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT		hr	= S_OK;
	LPD3DXEFFECT pEffect	= g_Effects["BALL"];

	//	공 렌더
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	D3DXMATRIX	matShadow	= m_matWorld*g_matShadow*g_matViewProj;
	pEffect->SetMatrix( "SHADOW", &matShadow );

	switch( m_balltype )
	{
	case COMMON:
		{
			pEffect->SetTechnique("TDrawNormal");
			pEffect->Begin(NULL, 0);
			for( DWORD i=0; i<m_numMaterials; ++i )
			{
				pDevice->SetMaterial( &m_pMaterials[i] );
				pDevice->SetTexture( 0, m_ppTex[i] );
				pEffect->BeginPass(0);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
				pEffect->BeginPass(1);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
			}
			pEffect->End();
		}
		break;
	case REDSHIFT:
		{
			pEffect->SetTechnique("TDrawRedShift");
			pEffect->Begin(NULL, 0);
			for( DWORD i=0; i<m_numMaterials; ++i )
			{
				pDevice->SetMaterial( &m_pMaterials[i] );
				pDevice->SetTexture( 0, m_ppTex[i] );
				pEffect->BeginPass(0);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
				pEffect->BeginPass(1);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
			}
			pEffect->End();
		}
		break;
	case BLACKHOLE:
		{
			D3DXMATRIX	matBlackHole	= g_matInvView;
			matBlackHole._41	= m_matT._41;
			matBlackHole._42	= m_matT._42;
			matBlackHole._43	= m_matT._43;
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			pDevice->SetTransform(D3DTS_WORLD, &matBlackHole);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			hr	= pDevice->SetStreamSource( 0, m_pBallVB, 0, sizeof(SimpleVertex) );
			hr	= pDevice->SetFVF(SimpleVertex::SimpleVertexFVF);
			hr	= pDevice->SetTexture(0, m_pBlackholeTex);
			hr	= pDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		break;
	case PHANTOM:
		{
			pEffect->SetTechnique("TDrawNormal");
			pEffect->Begin(NULL, 0);
			for( DWORD i=0; i<m_numMaterials; ++i )
			{
				pDevice->SetMaterial( &m_pMaterials[i] );
				pDevice->SetTexture( 0, m_ppTex[i] );
				pEffect->BeginPass(1);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
			}
			pEffect->End();
		}
		break;
	case ILLUSION:
		{
			D3DXMATRIX	matPhantom	= m_matT;
			matPhantom._41	+= (float)(rand()%10-5);
			matPhantom._42	+= (float)(rand()%10-5);
			matPhantom._43	+= (float)(rand()%10-5);
			pDevice->SetTransform( D3DTS_WORLD, &matPhantom );
			pEffect->SetTechnique("TDrawNormal");
			pEffect->Begin(NULL, 0);
			for( DWORD i=0; i<m_numMaterials; ++i )
			{
				pDevice->SetMaterial( &m_pMaterials[i] );
				pDevice->SetTexture( 0, m_ppTex[i] );
				pEffect->BeginPass(0);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
				pEffect->BeginPass(1);
				m_pBall->DrawSubset(i);
				pEffect->EndPass();
			}
			pEffect->End();
		}
		break;
	}

	//	낙하지점 표시
	if( m_bShowFallingPoint )
	{
		pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		pDevice->SetTransform( D3DTS_WORLD, &m_matFalling );	
		pDevice->SetFVF( SimpleVertex::SimpleVertexFVF );
		pDevice->SetTexture( 0, m_pFallingTex );
		pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_pFalling, sizeof( SimpleVertex ) );
	}

	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	return		hr;
}

float		CBall::GetHeight()
{
	return m_BoundingBall.vCenter.y - m_BoundingBall.fRadius;
}
void		CBall::ReflectByXXX(const D3DXVECTOR3* pN, eBehavior XXX, float fWeight)
{
	m_bShowFallingPoint	= TRUE;
	float		stride	= 100.f;
	float		t		= 0.f;
	float		_bb_4ac	= 0.f;

	if( D3DXVec3Length( &m_vVelocity ) < EPSILON )
		return;

	switch( m_balltype )
	{
	case BLACKHOLE:
		{
			SimpleVertex	*pVB = NULL;
			m_pBallVB->Lock(0, 0, (void**)&pVB, 0 );
			float	W = 1.2f*m_BoundingBall.fRadius;
			pVB[0].p	= D3DXVECTOR3(-W, -W, 0.f);
			pVB[1].p	= D3DXVECTOR3(-W,  W, 0.f);
			pVB[2].p	= D3DXVECTOR3( W,  W, 0.f);
			pVB[3].p	= D3DXVECTOR3( W, -W, 0.f);
			m_pBallVB->Unlock();
		}
		break;
	}

	m_balltype	= COMMON;
	m_bNeedUpdate	= true;

	switch( XXX )
	{
		case	RECEIVE:
		{
			m_vDest			+=m_vExtraDest;
			m_vDest.y		= RECEIVEPOINTLOW;
			D3DXVECTOR3	s	= m_vDest - m_BoundingBall.vCenter;
			float		v0	= 15.f*(1+(1-max((D3DXVec3Length(&s)/stride),0.2f)));
			_bb_4ac			= 4*v0*v0 + 8*m_vAccel.y*s.y;
			t				= (-2*v0 - sqrt( (_bb_4ac<0.f)? 0.f: _bb_4ac ) ) / (2*m_vAccel.y);
			m_vVelocity		= -0.5f*m_vAccel*t + s/t;
			break;
		}
		case	TOSS:
		{
			m_vDest			+=m_vExtraDest;
			m_vDest.y		= RECEIVEPOINTHIGH;
			D3DXVECTOR3	s	= m_vDest - m_BoundingBall.vCenter;
			float		v0	= 15.f*(1+(1-max((D3DXVec3Length(&s)/stride),0.2f)));
			_bb_4ac			= 4*v0*v0 + 8*m_vAccel.y*s.y;
			t				= (-2*v0 - sqrt( (_bb_4ac<0.f)? 0.f: _bb_4ac ) ) / (2*m_vAccel.y);
			m_vVelocity		= -0.5f*m_vAccel*t + s/t;
			break;
		}
		case	SLIDING:
		{
			m_vVelocity	*= 0.1f;
			m_vVelocity.y = 15.f;
			break;
		}
		case	SPIKE:
		{
			m_vDest				+=m_vExtraDest;
			D3DXVECTOR3 s		= m_vDest-m_BoundingBall.vCenter;
			t					= (D3DXVec3Length(&s)/stride);
			m_vVelocity			= -0.5f*m_vAccel*t + s/t;
			D3DXVECTOR3	v		= m_vVelocity;
			D3DXVECTOR3	vDir	= D3DXVECTOR3(0.f, 0.f, 0.f);
			float	length		= D3DXVec3Length( &m_vVelocity );

			for( float fRatio=1.f; fRatio>0.5f; fRatio -= 0.05f )
			{
				v					= m_vVelocity;
				v.y					= m_vVelocity.y*fRatio;
				t					= (-m_BoundingBall.vCenter.x)/v.x;
				float	fHeight		= m_BoundingBall.vCenter.y + ((v.y*t) + (0.5f*m_vAccel.y*t*t));
				if( fHeight > (g_BoundingMesh.RT.y+(0.9f*m_BoundingBall.fRadius)) )
				{
					m_vVelocity	= v;
					break;
				}
			}
			m_fBallWeight	= 0.5f;
			m_fBallIncr	= 0.03f;
			break;
		}
		case	SERVE_OVER:
		{
			m_vDest				+=m_vExtraDest;						//	최종목적지
			m_vDest.x			*= 0.8f;
			D3DXVECTOR3	vSub	= m_vDest-m_BoundingBall.vCenter;
			D3DXVECTOR3	vMiddle	= m_BoundingBall.vCenter + (0.5f*vSub);
			vMiddle.y			= g_BoundingMesh.RT.y+10.f;
			vSub				= vMiddle-m_BoundingBall.vCenter;
			float		s		= vMiddle.y - m_BoundingBall.vCenter.y;
			float		v0		= sqrt( -2.f* m_vAccel.y*s );
			float		_bb_4ac	= (4.f*v0*v0 + 8.f*m_vAccel.y*s);
			t					= (-2.f*v0 - sqrt( (_bb_4ac <0.f)? 0.f: _bb_4ac))/ (2.f*m_vAccel.y);
			m_vVelocity			= (vSub/t) - (m_vAccel*t*0.5f);
			break;
		}
		case	SERVE_UNDER:
		{
			m_vDest				+=m_vExtraDest;						//	최종목적지
			m_vDest.x			*= 0.8f;
			D3DXVECTOR3	vSub	= m_vDest-m_BoundingBall.vCenter;
			D3DXVECTOR3	vMiddle	= m_BoundingBall.vCenter + (0.5f*vSub);
			vMiddle.y			= g_BoundingMesh.RT.y+30.f;
			vSub				= vMiddle-m_BoundingBall.vCenter;
			float		s		= vMiddle.y - m_BoundingBall.vCenter.y;
			float		v0		= sqrt( -2.f* m_vAccel.y*s );
			float		_bb_4ac	= (4.f*v0*v0 + 8.f*m_vAccel.y*s);
			t					= (-2.f*v0 - sqrt( (_bb_4ac <0.f)? 0.f: _bb_4ac))/ (2.f*m_vAccel.y);
			m_vVelocity			= (vSub/t) - (m_vAccel*t*0.5f);	
			break;
		}
		case	THROWING:
		{
			m_bShowFallingPoint	= TRUE;
			m_vVelocity			= D3DXVECTOR3( 0.f, 30.f, 0.f );
			break;
		}
		case	BLOCKING:
		{
			D3DXPLANE	plane	= D3DXPLANE(1.f, 0.f, 0.f, 0.f);
			D3DXMATRIX	matRet;
			D3DXMatrixReflect(&matRet, &plane);
			D3DXVec3TransformCoord( &m_vVelocity, &m_vVelocity, &matRet);
			break;
		}
		default:
		{
			m_bNeedUpdate	= false;
			g_pPlayerLastTouch	= NULL;
			m_vVelocity = Reflect( pN, &m_vVelocity )*fWeight;
		}
	}
	m_eLastTouchedByXXX	= XXX;
	ZeroMemory( &m_vDest,		sizeof(D3DXVECTOR3) );
	ZeroMemory( &m_vExtraDest,	sizeof(D3DXVECTOR3) );

	ComputeRotationAxis();
	ComputeFallingSpot( &m_matFalling );
}
void		CBall::ComputeRotationAxis()
{
	m_fRotationAngle	= 0.f;
	D3DXVECTOR3	up	= D3DXVECTOR3( 0.f, 1.f, 0.f );
	D3DXVec3Cross( &m_vRotationAxis, &up, &m_vVelocity );
	D3DXVec3Normalize( &m_vRotationAxis, &m_vRotationAxis );
	if( D3DXVec3Length( &m_vRotationAxis ) < EPSILON )
		m_vRotationAxis = m_BoundingBall.vCenter.x>0? D3DXVECTOR3(0, 0, 1): D3DXVECTOR3(0, 0, -1);
}
void		CBall::SetMacro(eBallMacroType eUsage, const void *param)
{
	if( m_bUseMacro )
		return;

	switch( eUsage )
	{
		case BALLANDNET :
		{
			const BoundingPlane* NetMesh = (const BoundingPlane*)param;
			float	fTop			= NetMesh->RT.y;
			float	fBottom			= NetMesh->LB.y;
			D3DXVECTOR3& vCenter	= m_BoundingBall.vCenter;
			float	fRadius			= m_BoundingBall.fRadius;
			m_bUseMacro				= TRUE;
			ZeroMemory( &m_macro, sizeof(BallMacro) );

			//	볼이 넷위나 아래에 걸렸을 경우( 볼크기의 반이하로 걸린 경우 )
			if( vCenter.y > fTop )
			{
				m_macro.MacroTime		= 0.2f;
				m_macro.TransitionPoint	= 0.f;
				float	weight	= 0.2f+(fRadius-(vCenter.y-fTop) )/(fRadius*1.2f);
				m_vVelocity.y	= m_vVelocity.y + m_vVelocity.y*(weight);
				m_vVelocity		*= (1-weight);
				m_matT._41		= m_vVelocity.x>0? 0.5f: -0.5f;
				return;
			}
			else if( vCenter.y < fBottom )
			{
				m_macro.MacroTime		= 0.2f;
				m_macro.TransitionPoint	= 0.f;
				float	weight	= 0.2f+(fRadius+(vCenter.y-fBottom) )/(fRadius*1.2f);
				m_vVelocity.y	= m_vVelocity.y - m_vVelocity.y*(weight);
				m_matT._41		= m_vVelocity.x>0? 0.5f: -0.5f;
				m_vVelocity		*= (1-weight);
				return;
			}
			else
			{
				// 그이외에의 경우에는 네트에 잠깐 머문후 튕겨나온다.
				m_macro.MacroTime		= 0.3f;
				m_macro.TransitionPoint	= 0.1f;
				m_macro.TempVelocity	= m_vVelocity*0.2f;
				ReflectByXXX( &NetMesh->n, WAITING, 0.6f );
				break;
			}
		}
	}
}
void		CBall::ProcessMacro(float fElapsedTime)
{
	if( m_bUseMacro )
	{
		D3DXVECTOR3	vVelocity	= m_macro.ElapsedTime+fElapsedTime <= m_macro.TransitionPoint
							? m_macro.TempVelocity: m_vVelocity;

		m_matT._41	+= fElapsedTime*vVelocity.x;
		m_matT._42	+= fElapsedTime*vVelocity.y;
		m_matT._43	+= fElapsedTime*vVelocity.z;

		if( m_matT._42 <= 0+m_BoundingBall.fRadius )
		{
			m_matT._42	= m_BoundingBall.fRadius;
			if( m_macro.ElapsedTime+fElapsedTime <= m_macro.TransitionPoint )
				m_macro.TempVelocity = 8.8f * Reflect( &D3DXVECTOR3(0,1,0), &vVelocity );
			else
				ReflectByXXX( &D3DXVECTOR3(0,1,0), WAITING, 0.8f );
		}

		m_macro.ElapsedTime += fElapsedTime;
			
		if( m_macro.ElapsedTime >= m_macro.MacroTime )
		{
			m_bUseMacro	= FALSE;
		}
	}
}
void		CBall::ComputeFallingSpot(D3DXMATRIX *m)
{
	float		t	= 0;
	float		s	= m_BoundingBall.fRadius-m_BoundingBall.vCenter.y;
	D3DXVECTOR3	v0	= m_vVelocity;
	D3DXVECTOR3	a	= m_vAccel;
	t				= ( -2*v0.y - sqrt(4*v0.y*v0.y + 8*a.y*s) ) / (2*a.y);

	D3DXVECTOR3	v= m_vVelocity + m_vAccel*t;
	m->_41	= m_matT._41+( v.x*t );
	m->_42	= 0.f;
	m->_43	= m_matT._43+( v.z*t );
}

void		CBall::UpdateBallStateFromNetwork(const BallPacket *packet)
{
	ZeroMemory( &m_vDest,		sizeof(D3DXVECTOR3) );
	ZeroMemory( &m_vExtraDest,	sizeof(D3DXVECTOR3) );

	switch( packet->BALLTYPE )
	{
	case BLACKHOLE:
		{
			SimpleVertex	*pVB= NULL;
			m_pBallVB->Lock(0, 0, (void**)&pVB, 0 );
			float	W = 1.2f*m_BoundingBall.fRadius;
			pVB[0].p	= D3DXVECTOR3(-W, -W, 0.f);
			pVB[1].p	= D3DXVECTOR3(-W,  W, 0.f);
			pVB[2].p	= D3DXVECTOR3( W,  W, 0.f);
			pVB[3].p	= D3DXVECTOR3( W, -W, 0.f);
			m_pBallVB->Unlock();
		}
		break;
	}

	m_balltype			= packet->BALLTYPE;
	m_fRotationAngle	= packet->ROTATION;
	m_vRotationAxis		= packet->AXIS;
	m_matT._41	= packet->POSITION.x;
	m_matT._42	= packet->POSITION.y;
	m_matT._43	= packet->POSITION.z;
	m_vVelocity	= packet->VELOCITY;
	m_BoundingBall.UpdateBoundingCenter();
	ComputeFallingSpot( &m_matFalling );
}