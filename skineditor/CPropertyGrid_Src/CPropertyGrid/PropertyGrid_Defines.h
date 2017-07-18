
#pragma once

//  ���Ա��֪ͨ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
//  ���Ա����¼�����ʱ�������������ڷ���֪ͨ��ϢPGWM_PROPERTYGRID_NOTIFY��
//   
//  PGWM_PROPERTYGRID_NOTIFY 
//      nGridAction = HIWORD(wParam);         // ���Ա���¼�����
//      nID = LOWORD(wParam);                 // ���Ա�Ŀؼ�ID
//      pItem = (CPropertyGridItem*) lParam;  // ���¼����������������ָ��
//  
//  ����˵��
//
//  nGridAction
//  wParam�����ĸ��ֽڲ��֣���ʶ����֪ͨ��Ϣ�����Ա��¼�������Ϊ����ֵ֮һ��
//
//      PGN_SORTORDER_CHANGED - �������������ı�.
//      PGN_SELECTION_CHANGED - �����ѡ��.
//      PGN_ITEMVALUE_CHANGED - ����ֵ���ı�.
//
//  pItem
//  lParam��������һ��ָ����¼����������������ָ�룬����¼���
//  PGN_SORTORDER_CHANGED��pItem����NULL��
//  
//  ����ֵ
//  �����Ϣ�õ���������TRUE�����򷵻�FALSE��
//
//  ������һ��֪ͨ��Ϣ�Ĵ�������
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

const UINT PGWM_PROPERTYGRID_NOTIFY = (WM_APP + 2701);	// ���Ա���¼�֪ͨ��Ϣ
const UINT PGN_SORTORDER_CHANGED = 1;	// ���Ա��¼� - �������������ı�
const UINT PGN_SELECTION_CHANGED = 2;	// ���Ա��¼� - �����ѡ��
const UINT PGN_ITEMVALUE_CHANGED = 3;	// ���Ա��¼� - ����ֵ���ı�
