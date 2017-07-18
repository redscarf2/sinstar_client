
#pragma once

// class forwards
class CPropertyGridView;
class CPropertyGridItem;

//////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceButton is a CStatic derived class. 
// It is an internal class used by Property Grid control
//
class CPropertyGridInplaceButton : public CStatic
{
	DECLARE_DYNAMIC(CPropertyGridInplaceButton)

public:
	
	CPropertyGridInplaceButton();
	virtual ~CPropertyGridInplaceButton();

	virtual void HideWindow();
	virtual void Create(CPropertyGridItem* pItem, CRect rect);
	virtual void DestroyItem();

private:
	
    CWnd*                m_pGrid;
    CPropertyGridItem* m_pItem;
    BOOL                 m_bPressed;
    BOOL                 m_bOver;

protected:

    //{{AFX_VIRTUAL(CPropertyGridInplaceButton)
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CPropertyGridInplaceButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CPropertyGridItem;
};
