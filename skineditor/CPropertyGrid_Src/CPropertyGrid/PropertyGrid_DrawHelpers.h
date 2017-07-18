
#pragma once

//////////////////////////////////////////////////////////////////////
// CPGEmptyRect class

class CPGEmptyRect : public CRect
{
public:
	CPGEmptyRect()
	{
		SetRectEmpty();
	}
};

//////////////////////////////////////////////////////////////////////
// CPGWindowRect class

class CPGWindowRect : public CRect
{
public:
	CPGWindowRect (HWND hWnd)
	{
		::GetWindowRect (hWnd, this);
	};
	CPGWindowRect (const CWnd* pWnd)
	{
		::GetWindowRect (pWnd->GetSafeHwnd(), this);
	};
};

//////////////////////////////////////////////////////////////////////
// CPGClientRect class

class CPGClientRect : public CRect
{
public:
	CPGClientRect (const CWnd* pWnd)
	{
		::GetClientRect (pWnd->GetSafeHwnd(), this);
	};
};

//////////////////////////////////////////////////////////////////////
// CPGBufferDC class

class CPGBufferDC : public CDC
{
	HDC     m_hDestDC;
	CBitmap m_bitmap;     // Bitmap in Memory DC
	CRect   m_rect;
	HGDIOBJ m_hOldBitmap; // Previous Bitmap
	
public:
	CPGBufferDC (HDC hDestDC, const CRect rcPaint) : m_hDestDC (hDestDC)
	{
		m_rect = rcPaint;
		VERIFY(Attach (::CreateCompatibleDC (m_hDestDC)));
		m_bitmap.Attach (::CreateCompatibleBitmap (m_hDestDC, m_rect.right, m_rect.bottom));
		m_hOldBitmap = ::SelectObject (m_hDC, m_bitmap);
	}
	~CPGBufferDC ()
	{
		// VERIFY(::BitBlt (m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC, m_rect.left, m_rect.top, SRCCOPY));
		::BitBlt (m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC, m_rect.left, m_rect.top, SRCCOPY);
	    ::SelectObject (m_hDC, m_hOldBitmap);
	}
};

//////////////////////////////////////////////////////////////////////
// CPGFontDC class

class CPGFontDC
{
	CDC*     m_pDC;
	CFont* m_pOldFont;
public:
	CPGFontDC (CDC* pDC, CFont* pFont) : m_pDC (pDC)
	{
		m_pOldFont = (CFont*) m_pDC->SelectObject (pFont);
	}
	~CPGFontDC()
	{
		m_pDC->SelectObject (m_pOldFont);
	}
};

//////////////////////////////////////////////////////////////////////
// CPGPenDC class

class CPGPenDC
{
protected:
	CPen m_pen;
	HDC m_hDC;
	HPEN m_hOldPen;
	
public:
	CPGPenDC (HDC hDC, COLORREF crColor): m_hDC (hDC)
	{
		VERIFY(m_pen.CreatePen (PS_SOLID, 1, crColor));
		m_hOldPen = (HPEN)::SelectObject (m_hDC, m_pen);
	}
	~CPGPenDC ()
	{
		::SelectObject (m_hDC, m_hOldPen);
	}
};

//////////////////////////////////////////////////////////////////////
// CPGBrushDC class

class CPGBrushDC
{
protected:
	CBrush m_brush;
	HDC m_hDC;
	HBRUSH m_hOldBrush;
	
public:
	CPGBrushDC (HDC hDC, COLORREF crColor): m_hDC (hDC)
	{
		VERIFY(m_brush.CreateSolidBrush (crColor));
		m_hOldBrush = (HBRUSH)::SelectObject (m_hDC, m_brush);
	}
	~CPGBrushDC ()
	{
		::SelectObject (m_hDC, m_hOldBrush);
	}
};
