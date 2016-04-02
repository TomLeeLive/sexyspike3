// Interface for the CMiMp3 class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MIMP3_PLAYER_H_
#define _MIMP3_PLAYER_H_

// Need DShowStrmBase.lib....
// You must be copy below codes..
//
// Begin------------------------------------------------------------------------
//	#ifdef _DEBUG
//	#pragma comment(lib, "DShowStrmbase_.lib")
//	#else
//	#pragma comment(lib, "DShowStrmbase.lib")
//	#endif
// End--------------------------------------------------------------------------

#ifdef _DEBUG
#pragma comment(lib, "lib/DShowStrmbase_.lib")
#else
#pragma comment(lib, "lib/DShowStrmbase.lib")
#endif

#define MIMP3_MAX_VOLUME	10000L

#define WM_DSHOW_EVENT		(WM_USER+768)

class CMiMp3
{
protected:
	char	m_sFile[256];

	void*	m_pGB		;														// IGraphBuilder*
	void*	m_pMC		;														// IMediaControl*
	void*	m_pMS		;														// IMediaSeeking*
	void*	m_pSC		;														// IBaseFilter*		Source Current
	void*	m_pBA		;														// IBasicAudio*
	void*	m_pME		;														// IMediaEventEx*
public:
	BOOL	m_bRepeat	;
	BOOL	m_bPlayState	;

public:
	CMiMp3();
	virtual ~CMiMp3();

	INT		Create(char* sFile);
	INT		ProcessEvent();

	void	Play();
	void	Stop();
	void	Reset();
	void	Pause();

	void	SetVolume(DWORD dVol);
	LONG	GetVolume();
	void	SetRepeat(DWORD dRepeat=TRUE);
	DWORD	GetRepeat();

protected:
	INT		InitialGraph();

};





#endif