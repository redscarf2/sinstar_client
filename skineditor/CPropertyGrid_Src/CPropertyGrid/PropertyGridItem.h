
#pragma once

#include <afxtempl.h>

class CPropertyGridView;
class CPropertyGridItem;

const int PGI_EXPAND_BORDER = 14;

// Flags that indicate what styles to set for the grid item.
// 
enum PG_PROPERTYGRID_ITEMFLAGS
{
	// Item has edit control.
	pgitemHasEdit         = 1,
	// Item has expand button.
	pgitemHasExpandButton = 2,
	// Item has combo button.
	pgitemHasComboButton  = 4
};

//////////////////////////////////////////////////////////////////////
// CPGListItems is a CList derived class. It represents items' collection 
// with some internal functionality.
//
class CPGListItems : public CList<CPropertyGridItem*, CPropertyGridItem*> 
{
public:
	// Call this member function to sort items alphabetical 
	//
	void Sort();

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
};

//////////////////////////////////////////////////////////////////////
// CPGItemConstraints is a CList derived class. It represents item 
// constraints' collection.
//
class CPGItemConstraints : public CStringList
{
public:

	// Call this member function to add new item's constraint.
	// Returns the integer value that represents the index of the added constraint.
	//
	int AddConstraint(
		// new constraint
		CString str);

	// Call this member function to find constraint in the list.
	// Returns the zero-based index of the finded constraint; -1 if the constraint is not found.
	//
	int FindConstraint(
		// A constraint to search for
		CString str);
};

//////////////////////////////////////////////////////////////////////
// CPropertyGridItem is the base class representing an item of the 
// Property Grid Control 
// 
class CPropertyGridItem
{
public:

	// Constructs a CPropertyGridItem object.
	//
	CPropertyGridItem(
		// Caption of the item.
		CString strCaption, 
		// Initial value
		LPCTSTR strValue = NULL,
		// 属性项的最大字符数，0表示无限制
		// 只有字符串属性项可以使用该限制，其余属性项必须设为0
		// 另外要注意，在ANSI模式下，一个汉字占两个字符
		UINT nMaxLength = 0);
	
	// Constructs a CPropertyGridItem object.
	//
	CPropertyGridItem(
		// Identifier of the item.
		UINT nID, 
		// Initial value
		LPCTSTR strValue = NULL,
		// 属性项的最大字符数，0表示无限制
		// 只有字符串属性项可以使用该限制，其余属性项必须设为0
		// 另外要注意，在ANSI模式下，一个汉字占两个字符
		UINT nMaxLength = 0);

	// NB: class CPropertyGridItem hasn't default constructor. 
	// Identifier of second constructor can be linked with 
	// STRINGTABLE resource with same id in such form "Caption\nDescription". 

	// Destroys a CPropertyGridItem object, handles cleanup and de-allocation.
	//
	virtual ~CPropertyGridItem(void);

public:

	// Call this method to set the caption of the item. 
	//
	void SetCaption(
		// The new caption of the item. 	
		CString strCaption);
	
	// Call this method to set the description of the item. 
	//
	void SetDescription(
		// The new description of the item. 	
		CString strDescription);

	// Calls this method to set the read-only state of the item
	//
	void SetReadOnly(
		// Specifies whether to set or remove the read-only state of the item. 
		// A value of TRUE sets the state to read-only; a value of FALSE sets the state to read/write. 
		BOOL bReadOnly);

	// Calls this method to set the identifier of the item
	//
	void SetID(
		// The new identifier of the item. 	
		UINT m_nID);

	// This member function is called to set a specific flag for the item.
	//
	void SetFlags(
		// Flag to set. It can be one or more of the following:
		//
        // pgitemHasEdit - Item has edit control.
        // pgitemHasExpandButton - Item has expand button.
        // pgitemHasComboButton - Item has combo button.
		//
		UINT nFlags);

	// Call this method to get the caption of the item.
	// 
	CString GetCaption();

	// Call this method to get the description of the item.
	// 
	CString GetDescription();

	// Call this method to get the read-only state of the item.
	// 
	BOOL GetReadOnly();

	// Call this method to get the identifier of the item.
	// 
	UINT GetID();

	// Call this method to get current value of the item.
	// 
	CString GetValue();

	// Call this method to get flags of the item.
	// 
	UINT GetFlags();

	// The IsCategory function determines whether the item is a category. 
	// 
	BOOL IsCategory();

	// Call this method to add child item.
	// 
	CPropertyGridItem* AddChildItem(
		// The item to be added as a child.
		CPropertyGridItem* pItem);

	// Gets the position of the head element of child's list.
	//
	POSITION GetFirstChild() const;

	// Gets the child item identified by rPosition, then sets rPosition to the POSITION value of the next entry in the list.
	//
	CPropertyGridItem* GetNextChild(
		// A reference to a POSITION value returned by a previous GetNextChild, GetFirstChild, or other member function call.
		POSITION& rPosition) const;

	// The HasChilds function determines whether the item has children. 
	// 
	BOOL HasChilds();
	
	// The HasParent function determines whether the item has pParent item as parent. 
	// Returns TRUE if pParent is parent of the item.
	BOOL HasParent(
		// item  to be tested. 
		CPropertyGridItem* pParent);

	// Call this method to get constraints's list
	//
	// Example:
	// 
	// CPGItemConstraints* pList = pItem->GetConstraints();
	//
	CPGItemConstraints* GetConstraints();

	// Call this method to expand the item.
	// 
	void Expand();

	// Call this method to collapse the item.
	// 
	void Collapse();

	// Call this method to select the item.
	// 
	void Select();

	// Retrieves the bounding rectangle of the item
	// 
	CRect GetItemRect();

	// Retrieves the bounding rectangle of the item's value text
	// Override this member function to change it.
	//
	virtual CRect GetValueRect();

	// Call this method to change item's value
	// Override this method to add new functionality.
	// You should call the base class version of this function from your override.
	//
	virtual void SetValue(
		// New value of the item
		CString strValue);

	// This method is called when item's value is changed. Override this function if needed.
	//
	virtual void OnValueChanged(
		// New value of the item
		CString strValue);
	
	// Returns the parent window. 
	//
	CWnd* GetGrid();

	// Call this member function to determine if the item is visible.  Returns
	// TRUE if the item is visible, otherwise returns FALSE.
	//
	BOOL IsVisible() const;

	// Call this member function to determine if the item is expanded.  Returns
	// TRUE if the item is expanded, otherwise returns FALSE.
	//
	BOOL IsExpanded() const;

	// Call this member function to retreive the parent property grid item.  Returns
	// a CPropertyGridItem pointer that represents the parent item, can be NULL.
	//
	CPropertyGridItem* GetParentItem();

	// 设置一个用户定义的32位数据
	//
	void SetItemData(DWORD dwItemData)
	{ m_dwItemData = dwItemData; }

	// 获取用户定义的32位数据
	//
	DWORD GetItemData()
	{ return m_dwItemData; }

	// 把一个字符串对象绑定到属性项
	void BindToString(CString* pBindString)
	{
		m_pBindString = pBindString;
		if (m_pBindString)
			SetValue(*m_pBindString);
	}

protected:

	// This method is called when item is drawing. Override this function if needed.
	// Returns TRUE if item is self-drawing.
	//
	virtual BOOL OnDrawItemValue(
		// Reference to the device context to be used for rendering an image of the item.
		CDC& dc,
		// Bounding rect of the item
		CRect rcValue);

	// This method is called when item is selecting. 
	//
	virtual void OnSelect();

	// This method is called when item is deselecting. 
	//
	virtual void OnDeselect();

	// This method is called when key is pressed.
	//
	virtual BOOL OnChar(
		// Contains the character code value of the key.
		UINT nChar);

	// This method is called when the user double-clicks the left mouse button. 
	//
	virtual void OnLButtonDblClk();

	// This method is called when the user presses the left mouse button.
	//
	virtual BOOL OnLButtonDown(
		// Indicates whether various virtual keys are down (see Visual C++ documentation)
		UINT nFlags,
		// Specifies the x- and y-coordinate of the cursor. 
		CPoint point);

	// This method is called when item is added to the parent.
	//
	virtual void OnAddChildItem();

	// This method is called when the user presses inplace button.
	// Override the method to show item-specific dialog.
	//
	virtual void OnInplaceButtonDown();

	virtual void OnValidateEdit();

	// 根据字符数限制调整属性值
	CString AdjustValue(CString strValue);

protected:

	int						m_nIndex;
	int						m_nIndent;
	UINT					m_nID;
	UINT					m_nFlags;
	BOOL					m_bReadOnly;
	BOOL					m_bVisible;
	BOOL					m_bCategory;
	BOOL					m_bExpanded;
	CString					m_strValue;
	CString					m_strCaption;
	CString					m_strDescription;
	CPropertyGridItem*		m_pParent;
	CPropertyGridView*		m_pGrid;
	CPGListItems			m_lstChilds;
	CPGItemConstraints		m_lstContraints;

	CPropertyGridInplaceEdit&	GetInplaceEdit();
	CPropertyGridInplaceButton&	GetInplaceButton();
	CPropertyGridInplaceList&	GetInplaceList();

	DWORD		m_dwItemData;	// 一个用户定义的32位数据
	CString*	m_pBindString;	// 绑定到属性项的字符串对象

	UINT		m_nMaxLength;	// 属性项的最大字符数，0表示无限制
								// 只有字符串属性项可以使用该限制，其余属性项必须设为0
								// 另外要注意，在ANSI模式下，一个汉字占两个字符

public:

	void Init();
	virtual void SetEditText(CString str);

	friend class CPGListItems;
	friend class CPropertyGridView;
	friend class CPropertyGridInplaceEdit;
	friend class CPropertyGridInplaceButton;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE POSITION CPropertyGridItem::GetFirstChild() const { 
	return m_lstChilds.GetHeadPosition();
}
AFX_INLINE CPropertyGridItem* CPropertyGridItem::GetNextChild(POSITION& rPosition) const { 
	return m_lstChilds.GetNext(rPosition); 
}
AFX_INLINE BOOL CPropertyGridItem::HasChilds() { 
	return !m_lstChilds.IsEmpty(); 
}
AFX_INLINE void CPropertyGridItem::SetCaption(CString strCaption) { 
	m_strCaption = strCaption; 
}
AFX_INLINE CString CPropertyGridItem::GetCaption() { 
	return m_strCaption; 
}
AFX_INLINE void CPropertyGridItem::SetDescription(CString strDescription) {
	m_strDescription = strDescription; 
}
AFX_INLINE CString CPropertyGridItem::GetDescription() {
	return m_strDescription;
}
AFX_INLINE BOOL CPropertyGridItem::GetReadOnly() {
	return m_bReadOnly;
}
AFX_INLINE void CPropertyGridItem::SetID(UINT nID) {
	m_nID = nID;
}
AFX_INLINE UINT CPropertyGridItem::GetID() {
	return m_nID;
}
AFX_INLINE CString CPropertyGridItem::GetValue() {
	return m_strValue;
}
AFX_INLINE BOOL CPropertyGridItem::IsCategory() { 
	return m_bCategory; 
}
AFX_INLINE CWnd* CPropertyGridItem::GetGrid() {
	return (CWnd*)m_pGrid;
}
AFX_INLINE BOOL CPropertyGridItem::OnDrawItemValue(CDC&, CRect) { 
	return FALSE; 
}
AFX_INLINE void CPropertyGridItem::OnAddChildItem() {

}
AFX_INLINE void CPropertyGridItem::SetFlags(UINT nFlags) {
	ASSERT(!m_bCategory); ASSERT((nFlags & (pgitemHasComboButton | pgitemHasExpandButton)) != (pgitemHasComboButton | pgitemHasExpandButton)); m_nFlags = nFlags;
}
AFX_INLINE UINT CPropertyGridItem::GetFlags() {
	return m_nFlags;
}
AFX_INLINE BOOL CPropertyGridItem::IsVisible() const {
	return m_bVisible;
}
AFX_INLINE BOOL CPropertyGridItem::IsExpanded() const {
	return m_bExpanded;
}
AFX_INLINE CPropertyGridItem* CPropertyGridItem::GetParentItem() {
	return m_pParent;
}

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CPGItemConstraints::AddConstraint(CString str){
	AddTail(str); return (int)GetCount() - 1;
}
AFX_INLINE CPGItemConstraints* CPropertyGridItem::GetConstraints(){
	return &m_lstContraints;
}
