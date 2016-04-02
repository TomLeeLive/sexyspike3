#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/EffectManager.h"

CEffectManager::~CEffectManager()
{
	if( !m_mapEffect.empty() )
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
	}
}

HRESULT	CEffectManager::InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT	hr	= S_OK;

	m_pd3dDevice	= pDevice;
	m_pd3dDevice->AddRef();

	char	Keys[][MAX_PATH]		=
		{
				"BALL"
			,	"CHARACTER"
			,	"MAIN"
		};

	char	FileNames[][MAX_PATH]	= 
		{
				"./Effect/Effect_Ball.fx"
			,	"./Effect/Effect_Character.fx"
			,	"./Effect/Effect_Main.fx"
		};

	char	Techniques[][MAX_PATH]	= 
		{
				"TDrawNormal,TDrawRedShift,TBLACKHOLE"
			,	"IndexedSkinning"
			,	"TAfterImage,TInversion"
		};

	for( DWORD i=0; i<(sizeof(FileNames)/sizeof(FileNames[0])); ++i)
	{
		CHECK_FAILED(CreateEffect( Keys[i], FileNames[i], Techniques[i] ))
	}

	return	hr;
}

HRESULT	CEffectManager::RestoreDeviceObjects()
{
	HRESULT	hr	= S_OK;

	EffectMap::iterator	iter	= m_mapEffect.begin();
	EffectMap::iterator	end		= m_mapEffect.end();

	while( iter != end )
	{
		CHECK_FAILED(iter->second->OnResetDevice());
		iter++;
	}

	return	hr;
}

HRESULT	CEffectManager::InvalidateDeviceObjects()
{
	HRESULT	hr	= S_OK;

	EffectMap::iterator	iter	= m_mapEffect.begin();
	EffectMap::iterator	end		= m_mapEffect.end();

	while( iter != end )
	{
		CHECK_FAILED(iter->second->OnLostDevice());
		iter++;
	}

	return	hr;
}

HRESULT	CEffectManager::DeleteDeviceObjects()
{
	HRESULT	hr	= S_OK;

	EffectMap::iterator	iter	= m_mapEffect.begin();
	EffectMap::iterator	end		= m_mapEffect.end();

	while( iter != end )
	{
		if( iter->first )
			delete[] iter->first;

		if( iter->second )
			SAFE_RELEASE(iter->second);

		iter++;
	}

	m_mapEffect.clear();

	SAFE_RELEASE( m_pd3dDevice );

	return	hr;
}

HRESULT	CEffectManager::CreateEffect( char Key[MAX_PATH], char FileName[MAX_PATH], char Technique[MAX_PATH] )
{
	HRESULT	hr	= S_OK;

	LPD3DXBUFFER	pError	= NULL;
	LPD3DXEFFECT	pEffect	= NULL;
	int szie = 0;

	hr	= D3DXCreateEffectFromFile(
			m_pd3dDevice
		,	FileName
		,	NULL
		,	NULL
		,	0
		,	NULL
		,	&pEffect
		,	&pError
		);

	if( FAILED(hr) )
	{
		MessageBox(
			GetActiveWindow()
			,	pError? (LPCSTR)pError->GetBufferPointer(): "Effect File haven't Found"
			,	"Errect Error"
			,	MB_ICONERROR
			);

		SAFE_RELEASE( pError );
		return	hr;
	}

	char	*token		= NULL;
	char	*next_token	= NULL;
	token	= strtok_s( Technique, ",", &next_token );
	while( NULL != token )
	{
		D3DXHANDLE	hTechnique	= pEffect->GetTechniqueByName(token);
		hr	= pEffect->ValidateTechnique(hTechnique);
		if( FAILED(hr) )
		{
			SAFE_RELEASE( pEffect );
			return hr;
		}

		token	= strtok_s( NULL, ",", &next_token );
	}

	LPSTR	KeyName	= NULL;
	CopyString( Key, &KeyName );
	m_mapEffect.insert( EffectMap::value_type( KeyName, pEffect ) );

	return	hr;
}