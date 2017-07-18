
#pragma once

//  属性表的通知消息处理
/////////////////////////////////////////////////////////////////////////////
//  属性表有事件发生时，向其物主窗口发送通知消息PGWM_PROPERTYGRID_NOTIFY。
//   
//  PGWM_PROPERTYGRID_NOTIFY 
//      nGridAction = HIWORD(wParam);         // 属性表的事件代码
//      nID = LOWORD(wParam);                 // 属性表的控件ID
//      pItem = (CPropertyGridItem*) lParam;  // 跟事件关联的属性项对象指针
//  
//  参数说明
//
//  nGridAction
//  wParam参数的高字节部分，标识引发通知消息的属性表事件，可以为下列值之一：
//
//      PGN_SORTORDER_CHANGED - 属性项排序发生改变.
//      PGN_SELECTION_CHANGED - 属性项被选中.
//      PGN_ITEMVALUE_CHANGED - 属性值被改变.
//
//  pItem
//  lParam参数，是一个指向跟事件关联的属性项对象指针，如果事件是
//  PGN_SORTORDER_CHANGED，pItem等于NULL。
//  
//  返回值
//  如果消息得到处理，返回TRUE，否则返回FALSE。
//
//  以下是一个通知消息的处理例程
//
//  int nGridAction = HIWORD(wParam);
//  int nID = LOWORD(wParam);
//  CPropertyGridItem* pItem = (CPropertyGridItem*)lParam;
//
//  switch (nGridAction)
//  {
//  case PGN_SORTORDER_CHANGED:
//		{
//          m_nSort = m_wndPropertyGrid.GetPropertySort();
//          UpdateData(FALSE);
//		}
//      break;
//  case PGN_SELECTION_CHANGED:
//	    {
//          TRACE(_T("Selection Changed. Item = %s\n"), pItem->GetCaption());
//		}
//      break;
//  case PGN_ITEMVALUE_CHANGED:
//	    {
//          TRACE(_T("Value Changed. Caption = %s, ID = %i, Value = %s\n"),
//                   pItem->GetCaption(), pItem->GetID(), pItem->GetValue());
//		}
//      break;
//  }
//  return TRUE;
//

const UINT PGWM_PROPERTYGRID_NOTIFY = (WM_APP + 2701);	// 属性表的事件通知消息
const UINT PGN_SORTORDER_CHANGED = 1;	// 属性表事件 - 属性项排序发生改变
const UINT PGN_SELECTION_CHANGED = 2;	// 属性表事件 - 属性项被选中
const UINT PGN_ITEMVALUE_CHANGED = 3;	// 属性表事件 - 属性值被改变
