
#pragma once

// class forwards.
class CPropertyGridView;
class CPropertyGridItem;

//////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceList is a CListBox derived class. 
// It is an internal class used by Property Grid control
//
class CPropertyGridInplaceList : public CListBox
{
	DECLARE_DYNAMIC(CPropertyGridInplaceList)

public:

	CPropertyGridInplaceList();
	virtual ~CPropertyGridInplaceList();

	virtual void Create(CPropertyGridItem* pItem, CRect rect);
	virtual void DestroyItem();

private:

	CPropertyGridItem* m_pItem;

protected:

    //{{AFX_VIRTUAL(CPropertyGridInplaceList)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

	virtual void Cancel(void);
	virtual void Apply(void);

    //{{AFX_MSG(CPropertyGridInplaceList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//===========================================================================
// Summary:
//     The class CPGMouseMonitor is used to monitor mouse messages for
//     in-place popup control.
//===========================================================================
class CPGMouseMonitor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set up a hook to monitor mouse messages.
	// Parameters:
	//     pWndMonitor - A pointer to a CWnd object.  Used to determine
	//                   which monitor currently contains the mouse cursor.
	//-----------------------------------------------------------------------
	static void SetupHook(CWnd* pWndMonitor);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     The MouseProc hook procedure is an application-defined or
	//     library-defined callback function used with the SetWindowsHookEx
	//     function. The system calls this function whenever an application
	//     calls the GetMessage or PeekMessage function and there is a mouse
	//     message to be processed.
	//-----------------------------------------------------------------------
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	static HHOOK m_hHookMouse;      // A handle to a hook.
	static CWnd* m_pWndMonitor;     // A pointer to the in-place control.
};
