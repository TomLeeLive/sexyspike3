
#pragma		once

#ifndef		__FRAME_H_
#define		__FRAME_H_

//----------------------------------------------------------------------------
// D3DXFRAME:
// ----------
// This struct is the encapsulates a transform frame in a transformation frame
// hierarchy. The app can derive from this structure to add other app specific
// data to this
//----------------------------------------------------------------------------
//typedef struct _D3DXFRAME
//{
//    LPSTR                   Name;
//    D3DXMATRIX              TransformationMatrix;
//
//    LPD3DXMESHCONTAINER     pMeshContainer;
//
//    struct _D3DXFRAME       *pFrameSibling;
//    struct _D3DXFRAME       *pFrameFirstChild;
//} D3DXFRAME, *LPD3DXFRAME;

struct	CFrame	: public	_D3DXFRAME
{
	D3DXMATRIX		m_CombinedTransformationMatrix;
};

#endif