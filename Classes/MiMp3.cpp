// Implementation of the CMiMp3 class.
//
////////////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#pragma comment(lib, "DShowStrmbase_.lib")
#else
#pragma comment(lib, "DShowStrmbase.lib")
#endif


#include <windows.h>
#include <dshow.h>
//#include <tchar.h>
#include <malloc.h>


#include "MiMp3.h"


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p){ if(p){ (p)->Release(); p=NULL;	}}
#endif

#define FAIL_RETURN(p){ if( FAILED( (p) )) return -1;	}
#define FAIL_RETURNL(p){ if( FAILED( (p) )) return;		}

CMiMp3::CMiMp3()
{
	m_pGB		= NULL;
	m_pMC		= NULL;
	m_pMS		= NULL;
	m_pSC		= NULL;
	m_pBA		= NULL;
	m_bRepeat	= TRUE;
	m_bPlayState = TRUE;
}


CMiMp3::~CMiMp3()
{
	Stop();

	IGraphBuilder*	pGB	= (IGraphBuilder*)m_pGB	;
	IMediaControl*	pMC	= (IMediaControl*)m_pMC	;
	IMediaSeeking*	pMS	= (IMediaSeeking*)m_pMS	;
	IBaseFilter*	pSC	= (IBaseFilter  *)m_pSC	;
	IBasicAudio*	pBA	= (IBasicAudio  *)m_pBA	;
	IMediaEventEx*	pME	= (IMediaEventEx*)m_pME	;

	if (pME)
		pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

	SAFE_RELEASE(	pME	);
	SAFE_RELEASE(	pBA	);
	SAFE_RELEASE(	pSC	);
	SAFE_RELEASE(	pMS	);
	SAFE_RELEASE(	pMC	);
	SAFE_RELEASE(	pGB	);

	m_pME	= NULL;
	m_pGB	= NULL;
	m_pMC	= NULL;
	m_pMS	= NULL;
	m_pSC	= NULL;
	m_pBA	= NULL;
}


INT	CMiMp3::ProcessEvent()
{
	long lEventCode;
	long lParam1;
	long lParam2;
	HRESULT hr=0;

	IMediaEventEx* pME = (IMediaEventEx*)m_pME;
	IMediaControl* pMC = (IMediaControl*)m_pMC;
	IMediaSeeking* pMS = (IMediaSeeking*)m_pMS;

	if (!pME)
		return 1;

	// Check for completion events
	while(SUCCEEDED( pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0)))
	{
		// Free any memory associated with this event
		hr = pME->FreeEventParams(lEventCode, lParam1, lParam2);

		// If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode) 
		{
			if(TRUE == m_bRepeat)
			{
				LONGLONG llPos = 0;
				hr = pMS->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

				if (FAILED(hr))
				{
					if (FAILED(hr = pMC->Stop()))
						break;

					if (FAILED(hr = pMC->Run()))
						break;
				}
			}
			else
			{
				m_bPlayState=FALSE;
				return -1;
			}
		}
	}


	//	HWND hWnd = GetActiveWindow();
	//	pME->SetNotifyWindow((OAHWND)hWnd, WM_DSHOW_EVENT, (LONG)this);

	return hr;
}


void CMiMp3::Play()
{
	((IMediaControl*)m_pMC)->Run();
}

void CMiMp3::Stop()
{
	((IMediaControl*)m_pMC)->Stop();
}

void CMiMp3::Reset()
{
	LONGLONG llPos = 0;
	((IMediaSeeking*)m_pMS)->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
}

void CMiMp3::Pause()
{
	((IMediaControl*)m_pMC)->Pause();
}


void CMiMp3::SetVolume(DWORD dVol)
{
	LONG lVolume = dVol - MIMP3_MAX_VOLUME;

	FAIL_RETURNL( ((IBasicAudio*)m_pBA)->put_Volume(lVolume) );
}

LONG CMiMp3::GetVolume()
{
	LONG lVolume;

	FAIL_RETURN( ((IBasicAudio*)m_pBA)->get_Volume(&lVolume) );

	lVolume += MIMP3_MAX_VOLUME;
	return lVolume;
}


void CMiMp3::SetRepeat(DWORD dRepeat)
{
	m_bRepeat = dRepeat;
}

DWORD CMiMp3::GetRepeat()
{
	return m_bRepeat;
}

INT CMiMp3::Create(char* sFile)
{
	HRESULT hr;

	strcpy(m_sFile, sFile);

	FAIL_RETURN(hr = CoCreateInstance(CLSID_FilterGraph, NULL,	CLSCTX_INPROC, IID_IGraphBuilder, &m_pGB) );	// Create DirectShow Graph
	FAIL_RETURN(hr = ((IGraphBuilder*)m_pGB)->QueryInterface(IID_IMediaControl, &m_pMC) );			// Get the IMediaControl Interface
	FAIL_RETURN(hr = ((IGraphBuilder*)m_pGB)->QueryInterface(IID_IMediaSeeking,  &m_pMS) );			// Get the IMediaControl Interface
	FAIL_RETURN(hr = ((IGraphBuilder*)m_pGB)->QueryInterface(IID_IBasicAudio, &m_pBA) );				// Get the IBasicAudio Interface
	FAIL_RETURN(hr = ((IGraphBuilder*)m_pGB)->QueryInterface(IID_IMediaEvent, &m_pME) );				// Get the IMediaEvent Interface

	// Create the intial graph
	FAIL_RETURN(hr = InitialGraph() );

	HWND hWnd = GetActiveWindow();

	((IMediaEventEx*)m_pME)->SetNotifyWindow((OAHWND)hWnd, WM_DSHOW_EVENT, (LONG)this);

	return 1;
}



INT CMiMp3::InitialGraph()
{
	HRESULT hr = S_OK;
	IPin *pPin = NULL;
	IBaseFilter*	pSN	= NULL;

	// Can't find the media file
	if(0xFFFFFFFF == GetFileAttributes(m_sFile))
		return -1;

	WCHAR	wsFile[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, m_sFile, -1, wsFile, MAX_PATH);				// convert sFile to unicode string

	// OPTIMIZATION OPPORTUNITY
	// This will open the file, which is expensive. To optimize, this
	// should be done earlier, ideally as soon as we knew this was the
	// next file to ensure that the file load doesn't add to the
	// filter swapping time & cause a hiccup.
	//
	// Add the new source filter to the graph. (Graph can still be running)

	hr = ((IGraphBuilder*)m_pGB)->AddSourceFilter(wsFile, wsFile, (IBaseFilter**)&pSN);

	if(FAILED(hr))
		return -1;

	// Get the first output pin of the new source filter. Audio sources 
	// typically have only one output pin, so for most audio cases finding 
	// any output pin is sufficient.
	hr = pSN->FindPin(L"Output", &pPin);  

	if(FAILED(hr))
		return -1;

	// Stop the graph
	hr = ((IMediaControl*)m_pMC)->Stop();

	if(FAILED(hr))
		return -1;

	// Break all connections on the filters. You can do this by adding 
	// and removing each filter in the graph
	{
		IEnumFilters *pFilterEnum = NULL;
		hr = ((IGraphBuilder*)m_pGB)->EnumFilters(&pFilterEnum);

		if(SUCCEEDED(hr))
		{
			int iFiltCount = 0;
			int iPos = 0;

			// Need to know how many filters. If we add/remove filters during the
			// enumeration we'll invalidate the enumerator
			while(S_OK == pFilterEnum->Skip(1))
			{
				iFiltCount++;
			}

			// Allocate space, then pull out all of the 
			IBaseFilter** ppFilters = (IBaseFilter**)(_alloca(sizeof(IBaseFilter*) * iFiltCount));
			pFilterEnum->Reset();

			while(S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));

			SAFE_RELEASE(pFilterEnum);

			for(iPos = 0; iPos < iFiltCount; iPos++)
			{
				((IGraphBuilder*)m_pGB)->RemoveFilter(ppFilters[iPos]);

				// Put the filter back, unless it is the old source
				if(ppFilters[iPos] != ((IBaseFilter*)m_pSC))
				{
					((IGraphBuilder*)m_pGB)->AddFilter(ppFilters[iPos], NULL);
				}

				SAFE_RELEASE(ppFilters[iPos]);
			}
		}
	}

	// We have the new output pin. Render it
	if(SUCCEEDED(hr))
	{
		// Release the old source filter, if it exists
		IBaseFilter* pSC = (IBaseFilter*)m_pSC;
		SAFE_RELEASE( pSC);

		hr = ((IGraphBuilder*)m_pGB)->Render(pPin);
		m_pSC = pSN;
		pSN = NULL;
	}
	else
	{
		// In case of errors
		SAFE_RELEASE(pSN);
	}

	SAFE_RELEASE(pPin);

	return 1;
}