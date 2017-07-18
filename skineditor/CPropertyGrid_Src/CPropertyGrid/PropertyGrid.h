
#pragma once

// PG_PROPERTYSORT is an enumeration used by CPropertyGrid for determining of grouping style
// 
enum PG_PROPERTYSORT
{
	// to group items by category.
	pgsortCategorized,
	// to sort items alphabetical.
	pgsortAlphabetical,
	// disable sorting
	pgsortNoSort
};

//////////////////////////////////////////////////////////////////////
// CPropertyGridView is a CListBox derived class.
// It is an internal class used by Property Grid control
//
class CPropertyGridView : public CListBox
{
	DECLARE_DYNAMIC(CPropertyGridView)

public:
	CPropertyGridView();
	virtual ~CPropertyGridView();

	int GetDividerPos();
	CPropertyGridItem* ItemFromPoint(CPoint point);

private:

	CPropertyGridItem* GetSelectedItem();
	CPropertyGridItem* AddCategory(CString strCaption);
	void SetPropertySort(PG_PROPERTYSORT sort, BOOL bRrefresh = FALSE);
	int	InsertItem(CPropertyGridItem* pItem, int nIndex);
	int _DoExpand(CPropertyGridItem* pItem, int nIndex);
	void _DoCollapse(CPropertyGridItem* pItem);
	void _RefreshIndexes();
	void SwitchExpandState(int nItem);
	CPropertyGridItem* GetItem(int nIndex);
	int HitTest(CPoint point);
	void ResetContent();

	void ShowToolTip(CPoint pt);
	void _ShowToolTip(CRect rcBound, CRect rcText);
	CSize _GetTextExtent(CString str);
	void RelayToolTipEvent(MSG* pMsg);


public:

    CPGListItems		m_lstCategories;
    PG_PROPERTYSORT		m_properetySort;
    COLORREF			m_clrLine;
    COLORREF			m_clrFore;
    COLORREF			m_clrBack;
    double				m_dDivider;
    CFont				m_fontNormal;
    CFont				m_fontBold;
    BOOL				m_bTracking;
    CPropertyGridItem*	m_pSelected;
	
	CToolTipCtrl		m_wndTip;
	CString				m_strTipText;
	CRect				m_rcToolTip;

	CPropertyGridInplaceEdit	m_wndEdit;
	CPropertyGridInplaceButton	m_wndButton;
	CPropertyGridInplaceList	m_wndListBox;

protected:

    //{{AFX_VIRTUAL(CPropertyGridView)
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT) {}
	virtual int CompareItem(LPCOMPAREITEMSTRUCT) { return 0;}
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CPropertyGridView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelectionChanged();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CPropertyGridItem;
	friend class CPropertyGrid;
	friend class CPropertyGridInplaceEdit;
};

//////////////////////////////////////////////////////////////////////
// CPGToolBar从CToolBar派生，用作属性表的内置工具条
//
class CPGToolBar : public CToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {}
};

//////////////////////////////////////////////////////////////////////
// CPropertyGrid is a CWnd derived class.  It is used to create a property grid
// control similar to the property window in VS.NET.
//
// See the "PropertyGrid" demo for a example of usage.
//
class CPropertyGrid : public CWnd
{
	DECLARE_DYNAMIC(CPropertyGrid)

public:
	// Constructs a CPropertyGrid object.
    //
	CPropertyGrid();

	// Destroys a CPropertyGrid object, handles cleanup and de-allocation.
	//
	virtual ~CPropertyGrid();

	// Call this member function to create the Property Grid control.  Returns
	// TRUE if successful, otherwise returns FALSE.
	//
	virtual BOOL Create(
		// Size of the control.
		const RECT& rect, 
		// Parent of the control.			
		CWnd* pParentWnd, 
		// Identifier of the Property Grid control.			
		UINT nID);

public:

	// Call this member function to add new category to the Property Grid control.
	// Returns the pointer to the item object of the newly inserted category.
	//
	CPropertyGridItem* AddCategory(
		// Name of the category to add.		
		CString strCaption);
	
	// 该函数用于设置属性项的排序方式
	//
	void SetPropertySort (
		// 新的排序方式，只能是下列中的一个:
		//
		// pgsortCategorized - 按分类顺序
		// pgsortAlphabetical - 按字母顺序
		// pgsortNoSort - 不排序
		//		
		PG_PROPERTYSORT sort);

	// Call this member function to get the current grouping style for the Property Grid control.
	// Returns a PG_PROPERTYSORT value representing the current grouping style.
	//
	PG_PROPERTYSORT GetPropertySort();

	// Call this member function to show or hide toolbar control in Property Grid window.
	//
	void ShowToolBar(
		// TRUE to show toolbar control, or FALSE to hide it.		
		BOOL bShow = TRUE);
	
	// Call this member function to show or hide description pane in Property Grid window.
	//
	void ShowHelp(
		// TRUE to show description pane, or FALSE to hide it.		
		BOOL bShow = TRUE);

	// Call this member function to set custom colors of Property Grid control.
	//
	void SetCustomColors( 
		// An RGB value that represents the background color of description pane.
		COLORREF clrHelpBack, 
		// An RGB value that represents the text color of description pane.
		COLORREF clrHelpFore, 
		// An RGB value that represents the color of separating lines in control.
		COLORREF clrViewLine,
		// An RGB value that represents the background color of Property Grid control.
		COLORREF clrViewBack, 
		// An RGB value that represents the text color of Property Grid control.
		COLORREF clrViewFore );

	// Call this member function to set default colors of Property Grid control.
	//
	void SetStandardColors();

	// Call this member function to find item by caption.
	// Returns the pointer to the Item object.
	//
	CPropertyGridItem* FindItem(
		// Caption for the item to find
		CString strCaption);

	// Call this member function to find item by identifier.
	// Returns the pointer to the Item object.
	//
	CPropertyGridItem* FindItem(
		// Identifier for the item to find
		UINT nID);

	// Removes all items from the list box of a property grid control.
	//
	void ResetContent();

	// Call this member function to determine if the help pane is visible.  Returns
	// TRUE if the help pane is visible, otherwise returns FALSE.
	//
	BOOL IsHelpVisible() const;

	// Call this member function to determine if the toolbar is visible.  Returns
	// TRUE if the toolbar is visible, otherwise returns FALSE.
	//
	BOOL IsBarVisible() const;

	// Call this member function to return the height of the help pane when visible.
	// Returns an integer value that represents the height in pixels.
	//
	int GetHelpHeight() const;

	// Call this member function to return the background color for the property grid.
	// Returns an RGB value.
	//
	COLORREF GetHelpBackColor();

	// Call this member function to return the foreground color for the property grid.
	// Returns an RGB value.
	//
	COLORREF GetHelpForeColor();

	CPropertyGridView& GetGridView();
	CToolBar& GetToolBar();

	// 展开所有的属性项分类
	void ExpandAllCategories();

	// 折叠所有的属性项分类
	void CollapseAllCategories();

private:

	void OnInvertTracker(CRect rc);
	int HitTest(CPoint pt);
	void OnSelectionChanged(CPropertyGridItem* pItem);
	void OnSortChanged();
	void Reposition(int cx, int cy);
	void CreateToolbar();
	void RefreshToolBarButtons();

private:
	
	CPropertyGridView   m_wndView;
	BOOL				m_bHelpVisible;
	BOOL				m_bToolBarVisible;
	int					m_nHelpHeight;
	COLORREF			m_clrHelpBack;
	COLORREF			m_clrHelpFore;
	CRect				m_rectTracker;
	CPGToolBar			m_wndToolbar;

protected:

    //{{AFX_VIRTUAL(CPropertyGrid)
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CPropertyGrid)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSortAlphabetic();
	afx_msg void OnSortCategorized();
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CPropertyGridView;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CPropertyGridItem* CPropertyGrid::FindItem(CString strCaption) {
	return m_wndView.m_lstCategories.FindItem(strCaption);
}
AFX_INLINE CPropertyGridItem* CPropertyGrid::FindItem(UINT nID) {
	return m_wndView.m_lstCategories.FindItem(nID);
}
AFX_INLINE CPropertyGridItem* CPropertyGrid::AddCategory(CString strCaption) { 
	return m_wndView.AddCategory(strCaption); 
}
AFX_INLINE void CPropertyGrid::SetPropertySort (PG_PROPERTYSORT sort) { 
	m_wndView.SetPropertySort(sort); 
}
AFX_INLINE PG_PROPERTYSORT CPropertyGrid::GetPropertySort () { 
	return m_wndView.m_properetySort; 
}
AFX_INLINE BOOL CPropertyGrid::IsHelpVisible() const {
	return m_bHelpVisible;
}
AFX_INLINE BOOL CPropertyGrid::IsBarVisible() const {
	return m_bToolBarVisible;
}
AFX_INLINE int CPropertyGrid::GetHelpHeight() const {
	return m_nHelpHeight;
}
AFX_INLINE COLORREF CPropertyGrid::GetHelpBackColor() {
	return m_clrHelpBack;
}
AFX_INLINE COLORREF CPropertyGrid::GetHelpForeColor() {
	return m_clrHelpFore;
}
AFX_INLINE CPropertyGridView& CPropertyGrid::GetGridView() {
	return m_wndView;
}
AFX_INLINE CToolBar& CPropertyGrid::GetToolBar() {
	return m_wndToolbar;
}
