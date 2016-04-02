#pragma	once

class	CEffectManager
{
	typedef	std::map<LPCSTR, ID3DXEffect* , StrCmpI>	EffectMap;
	EffectMap			m_mapEffect;
	LPDIRECT3DDEVICE9	m_pd3dDevice;
public:
	CEffectManager() : m_pd3dDevice(NULL) { m_mapEffect.clear(); }
	~CEffectManager();

	HRESULT 	InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
    HRESULT 	RestoreDeviceObjects();
    HRESULT 	InvalidateDeviceObjects();
    HRESULT 	DeleteDeviceObjects();

	HRESULT		CreateEffect( char Key[MAX_PATH], char FileName[MAX_PATH], char Technique[MAX_PATH] );

	LPD3DXEFFECT operator[]( LPCSTR	KEY ) {return	m_mapEffect[KEY];}
};