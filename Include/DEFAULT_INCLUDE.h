
#pragma		once

#ifndef		__DEFAULT_INCLUDE_H_
#define		__DEFAULT_INCLUDE_H_

#include	<WinSock2.h>

#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>

#include	<commctrl.h>
#include	<math.h>
#include	<assert.h>
#include	<map>
#include	<string>
#include	<vector>

//09-08-08 이재준
#include	<time.h>

#include	<D3DX9.h>
#include	<DxErr.h>
#include	"../Common/DXUtil.h"
#include	"../Common/D3DEnumeration.h"
#include	"../Common/D3DSettings.h"
#include	"../Common/D3DApp.h"
#include	"../Common/D3DFile.h"
#include	"../Common/D3DFont.h"
#include	"../Common/D3DUtil.h"

//09-09-04 이웅재
#include "../Resource/Networkresource.h"

#include	"../Resource/resource.h"

#include	"MACRO.h"
#include	"DEFAULT_INILINE.h"
#include	"EffectManager.h"
#include	"KeyManager.h"

#ifdef	_DEBUG
extern	char			g_buf[256];
extern	int				g_cnt;
extern	bool			g_bool;
extern	char			g_filename[];
extern	char			g_filemode[];
extern	FILE			*g_fp;
extern	D3DXMATRIX		g_matT;
extern	HWND			g_hWnd;
#define	FO {fopen_s( &g_fp, g_filename, g_filemode );}
#define	FC {fclose(g_fp);}
#endif

typedef	std::map< LPCSTR, void*, StrCmpI > PLAYERSMAP;

extern	CEffectManager	g_Effects;
extern	D3DXMATRIX		g_matInvView;
extern	D3DXMATRIX		g_matViewProj;
extern	D3DXMATRIX		g_matShadow;
extern	D3DXMATRIX		*g_pPalette;
extern	DWORD			g_numPalette;
extern	BoundingPlane	g_BoundingMesh;
extern	D3DLIGHT9		g_light;
extern	CKeyManager		g_keys;
extern	int				g_CntBallTouched;
extern	void*			g_pPlayerLastTouch;
extern	void*			g_pPlayerRedShift;
extern	void**			g_ppPlayers;
extern	TeamInfo		g_Teams[2];
extern	int				g_score[2];
extern	BOOL			g_bNowServe;
extern	D3DXVECTOR3		g_dir[4];
extern	const	void*	LookUpPlayer( PLAY_STATE STATE, eSide SIDE );


//09-09-07 이재준
extern	void	Send( const char* pName, char DEFINITION,  char SIZE, void* pData, char EXETYPE=3, char ORDER=3 );

#define SPEED_USER_MOVE 100.0f
#define SPEED_GAME 1.5f
#define SPEED_ANIMATION 0.6f

#endif