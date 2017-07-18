
#pragma once

class CPropertyGridItemFile : public CPropertyGridItem  
{
public:
	// 构造函数
	//     strCaption   －属性名称
	//     lpszFileName －初始文件名
	//     bOpen        －表明是打开文件(TRUE)还是保存文件(FALSE)
	CPropertyGridItemFile(CString strCaption, LPCTSTR lpszFileName = NULL, BOOL bOpen = TRUE);

	// 构造函数
	//     nID          －属性ID
	//     lpszFileName －初始文件名
	//     bOpen        －表明是打开文件(TRUE)还是保存文件(FALSE)
	CPropertyGridItemFile(UINT nID, LPCTSTR lpszFileName = NULL, BOOL bOpen = TRUE);

	virtual ~CPropertyGridItemFile();

	// 设置文件名
	void SetFileName(CString strFileName);

	// 获取文件名
	CString GetFileName();

	// 把一个字符串对象绑定到属性项
	void BindToFileName(CString* pBindFileName);

protected:
	BOOL		m_bOpen;		// 表明是打开文件(TRUE)还是保存文件(FALSE)
	CString*	m_pBindFileName;// 绑定到属性项的字符串对象

	// 当属性值由于用户输入而改变时，调用此函数
	virtual void SetValue(CString strValue);

	// 当用户点击按钮时，调用此虚函数
	virtual void OnInplaceButtonDown();
};
