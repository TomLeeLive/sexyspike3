/*! 
* @file CDrawText.h
* @brief draw text
*/


#ifndef __DRAWTEXTEXT_H__
#define __DRAWTEXTEXT_H__

#include <cassert>
#include <d3d9.h>
#include <d3dx9.h>



/*!
* @brief D3D draw text
*
* ID3DXSprite, ID3DXFONT를 사용하여 3D 텍스트 출력
* \li usage: 
* @code
*	pDevice->BeginScene();
*	CDrawText text;
*	text.Create(pDevice, 10, 15);
*	D3DXCOLOR color(1.0f, 1.0f, 0.0f, 1.0f);	// default color D3DCOLOR_RGBA(255,255,0,255);
*	text.Draw(TEXT("Draw Text"), 0, 0, color);	// positon x, y is left-top. DrawText format 'DT_NOCLIP'
*	pDevice->EndScene();
*	pDevice->Present(0, 0, 0, 0);
* @endcode
* 
* - device reset과 lost 경우 Reset(), Lost() 호출.
*/
class CDrawText
{
public:
	CDrawText() : m_pFont(0), m_pTextSprite(0) { }
	~CDrawText() { Release(); }

	HRESULT Create(IDirect3DDevice9 *pDevice, int width = 10, int height = 15)
	{
		HRESULT font = D3DXCreateFont(pDevice, height, width,
			FW_DONTCARE, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			TEXT("Arial"), &m_pFont);

		if (FAILED(font)) 
		{
			m_pFont = 0;
			return E_FAIL;
		}
		else 
		{
			D3DXCreateSprite(pDevice, &m_pTextSprite);
			return S_OK;
		}
	}

	void Release() 
	{ 
		SAFE_RELEASE(m_pTextSprite);
		SAFE_RELEASE(m_pFont);
	}

	HRESULT Reset();
	HRESULT Lost();

	HRESULT Begin();
	HRESULT End();

	HRESULT Draw(LPCTSTR str, int x, int y, D3DXCOLOR color = D3DXCOLOR(0,0,0,1))
	{
		assert(m_pFont != 0);
		RECT rc = { x, y, x+100, y+100 };	// DT_NOCLIP이므로 의미 없음
		return m_pFont->DrawText(m_pTextSprite, str, -1, &rc, DT_NOCLIP, color);
	}

private:
	ID3DXFont* m_pFont;
	ID3DXSprite* m_pTextSprite;
};



// inline member function 
inline HRESULT CDrawText::Begin()
{
	assert(m_pTextSprite);
	return m_pTextSprite->Begin(D3DXSPRITE_ALPHABLEND);
}



inline HRESULT CDrawText::End()
{ 
	assert(m_pTextSprite);
	return m_pTextSprite->End();
} 



inline HRESULT CDrawText::Reset()
{
	HRESULT hr = D3D_OK;
	if (m_pFont) 
		if (FAILED(hr = m_pFont->OnResetDevice()))
			return hr;
	if (m_pTextSprite) 
		if (FAILED(hr = m_pTextSprite->OnResetDevice()))
			return hr;

	return hr;
}



inline HRESULT CDrawText::Lost()
{
	HRESULT hr = D3D_OK;
	if (m_pFont)
		if (FAILED(m_pFont->OnLostDevice()))
			return hr;
	if (m_pTextSprite) 
		if (FAILED(m_pTextSprite->OnLostDevice()))
			return hr;

	return hr;
}



#endif // __DRAWTEXTEXT_H__