// Interface for the Utilities.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MCUTIL_H_
#define _MCUTIL_H_

#define GMAIN			g_pApp
#define GHINST			g_pApp->m_hInst
#define GHWND			g_pApp->m_hWnd
#define GDEVICE		g_pApp->m_pd3dDevice

#define SAFE_NEWCREATE1(p, CLASSTYPE, v0)							\
{																	\
	if(NULL == (p))													\
	{																\
		p = new CLASSTYPE;											\
		if(!(p))													\
			return -1;												\
																	\
		if(FAILED((p)->Create(v0)))									\
		{															\
			delete p;												\
			p = NULL;												\
			return -1;												\
		}															\
	}																\
}

//#define SAFE_RESTORE(p)												\
//{																	\
//	if(p && FAILED((p)->Restore() ))								\
//	{																\
//		return -1;													\
//	}																\
//}
//
#define SAFE_FRMOV(p)												\
{																	\
	if(p && FAILED((p)->FrameMove() ))								\
	{																\
		return -1;													\
	}																\
}

//#define SAFE_INVALID(p)			{	if(p)	(p)->Invalidate();		}
#define SAFE_RENDER(p)			{	if(p)	(p)->Render();			}



#endif