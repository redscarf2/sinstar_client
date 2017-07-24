//*******************************************************************
//	SArray.hpp
//	ģ�����ƣ����̶�̬����C++ģ���� 
//	version:2.3
//  ˵��������������1.0�汾�Ļ�����ǿ�������ݲ�����ɾ���������棬
//	�������ṩ�Զ�̬�����֧�֣����ڴ��г������ݷֿ��ţ�
//�������˴���ڴ�����룬ͬʱҲ����ͨ��˫������ͬ���Ǳ������ṩ
//���˶��ڲ����ݵĿ��������������������ݷ����ٶȡ�
//		��ǿ��������������֧�֣�������Ŀռ�������<�û�ָ���ı���ʱ�Զ���������
//	����ԱҲ���������ж��Ƿ���Ҫ��������������麯��IsNeedCompack��ʹ���Լ��Ĺ���
//	���������ҪĿ���Ǵ������ݵĹ����ṩһ�����ٵ����ݲ���ɾ���Ƚӿڣ���ʹ����ʱ
//	����һ�׺õĲ����������߳����ִ��Ч�ʣ���ʵ֤�����������ִ��Ч�ʴ�����
//	MFC�ṩ��CArray
//		�������������ʹ�á��޸ġ������������߲���ʹ�ø�������ɵĺ���е��κ�ֱ�ӻ������Ρ�
//		�ƽ��ۡ�huangjianxiong@sina.com
//��update
//		2.3 �ƽ���		2007-04-12	����Attach,Detach�ӿ�
//		2.2 �ƽ���		2007-01-31	��д���ظ��Ķ�����ҽӿ�
//		2.1 �ƽ���		2006-10-19  ����������ӿ�,�޸�SetSize�е�һ��bug
//		2.0 �ƽ���		2004-06-02
//		1.0 �ƽ���		2003-09-25
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
#define AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ASSERT
#define ASSERT(a) void()
#endif

#ifndef BOOL
#define BOOL int
#endif

//�����������
#define SFT_ANY		0	//�������ظ�������λ��
#define SFT_HEAD	1	//���뵽�ظ���ͷ��
#define SFT_TAIL	2	//���뵽�ظ���β��

template<class T>
class CSArray  
{
	typedef struct _SARRAYNODE{
		struct _SARRAYNODE *	pPrevNode;	//ǰһ�ڵ�
		T				   *	pData;		//�洢ʵ��������������ݿ��ָ��	
		DWORD					dwUsed;		//��ռ�õĿռ���,��ʼʱΪ0�����ֵΪ�ý��ĳ���
		struct _SARRAYNODE *	pNextNode;	//��һ�ڵ�
	}SARRAYNODE;

	typedef int (*FUNCMP)(T *pData1,T *pData2,LPARAM lParam);

public:
	CSArray(WORD nGrowBy=5){
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
		m_nGrowBy=nGrowBy;
		m_byZipKey=10;	//Ĭ��ѹ����ֵΪ10%
	}

	virtual ~CSArray(){Free();}

	//******************************************
	//	name:Add
	//	function:�������
	//	input:��T newElement-������
	//  return: �ɹ�=����������,ʧ��=-1
	//	remark: 
	//******************************************
	int Add(T newElement)
	{
		if(m_nGrowBy==0)//
			return -1;
		if(m_pHeadNode==NULL)
		{
			if(!(m_pHeadNode=NewNode())) return -1;
			*m_pHeadNode->pData=newElement;
			m_pHeadNode->dwUsed=1;
			m_nCurIndex=0;
			m_nCount=1;
			m_nEmpty=m_nGrowBy-1;
			m_pCurNode=m_pTailNode=m_pHeadNode;
		}
		else
		{
			if(m_pTailNode->dwUsed==m_nGrowBy)
			{
				SARRAYNODE *pNewNode=NewNode();
				if(!pNewNode) return -1;
				pNewNode->pPrevNode=m_pTailNode;
				m_pTailNode->pNextNode=pNewNode;
				m_pTailNode=pNewNode;
				m_nEmpty+=m_nGrowBy;
			}
			m_pTailNode->pData[m_pTailNode->dwUsed++]=newElement;
			m_nCount++;
			m_nEmpty--;
			m_nCurIndex=m_nCount-m_pTailNode->dwUsed;
			m_pCurNode=m_pTailNode;
		}
		return m_nCount-1;
	}

	//******************************************
	//	name:AddBatch
	//	function:�����������
	//	input:��T *pElement-Դ����ָ��
	//			int count-�����С
	//  return: BOOL��TRUE���ɹ���FALSE��ʧ��
	//	remark: 
	//******************************************
	BOOL AddBatch(T *pElement,int count)
	{
		for(int i=0;i<count;i++)
			if(Add(pElement[i])==-1)
				return FALSE;
			return TRUE;
	}


	//******************************************
	//	name:Copy
	//	function:���ݸ���
	//	input:��CSArray & src-Դ��̬����
	//  return: 
	//	remark: ʹ��ǰ����ȷ��������������ͬ����������
	//******************************************
	void Copy(CSArray &src )
	{
		T *pt;
		RemoveAll();
		int size=src.GetSize();
		SetSize(size);
		for(UINT i=0;i<m_nCount;i++)
		{
			pt=src.GetPtAt(i);
			SetAt(i,*pt);
		}
	}

	//******************************************
	//	name:GetAt
	//	function:��ȡ����ָ��λ�õ�����
	//	input:��int index-ָ��λ��
	//  return: T ����
	//	remark: 
	//******************************************
	T GetAt(DWORD index){
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)[index-m_nCurIndex];
	}
	
	//******************************************
	//	name:GetPtAt
	//	function:��ȡ����ָ��λ�õ����ݵ�ָ��
	//	input:��int index-ָ��λ��
	//  return: T ����
	//	remark: �ṩ���ڲ����ݵ�ֱ�ӷ��ʣ�С��ʹ��!!
	//******************************************
	T *GetPtAt(DWORD index){
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)+index-m_nCurIndex;
	}
	
	T & ElementAt(DWORD index)
	{
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)[index-m_nCurIndex];
	}

	T & operator[](DWORD index){ return ElementAt(index);}
	
	//******************************************
	//	name:GetSize
	//	function:��ȡ�������������
	//	input:��
	//  return: int ��������
	//	remark: 
	//******************************************
	int GetSize(){return m_nCount;}
	
	void SetCompackKey(BYTE byKey){ m_byZipKey=byKey;}
	//******************************************
	//	name:SetAt
	//	function:�޸�����ָ��λ�õ�����
	//	input:��T newElement-������
	//			int index-ָ��������
	//  return: BOOL TURE-�ɹ���FALSE��ʧ��
	//	remark: 
	//******************************************
	BOOL SetAt(DWORD index,T &newElement)
	{
		SARRAYNODE      *pDest=NULL;
		if(index<0||index>m_nCount-1)
			return FALSE;
		pDest=GetDestSegEntry(index);
		*(pDest->pData+index-m_nCurIndex)=newElement;
		return TRUE;
	}
	

	//******************************************
	//	name:SortInsert
	//	function:���Ѿ���������ݲ���һ���½�㲢��������
	//	input:	T newElement-�����������
	//			BOOL bRepeat:�����ظ�
	//			CBCMP funCmp:�Ƚϻص��ӿ�
	//			LPARAM lParam:�û�����
	//	return:����λ��
	//	2006.10.19
	//******************************************
	int SortInsert(T newElement,BOOL bRepeat=TRUE,FUNCMP funCmp=NULL,LPARAM lParam=0,UINT sfType=SFT_ANY)
	{
		if(m_nCount==0)
		{
			Add(newElement);
			return 0;
		}
		int nInsertPos=-1;
		int nPos=SortFindEx(&newElement,0,m_nCount,funCmp,lParam,sfType,&nInsertPos);
		if(nPos!=-1)
		{
			if(!bRepeat) return -1;
			if(sfType==SFT_TAIL)//�ҵ����һ����Ŀ����ȵ�Ԫ��,��Ҫ���뵽��Ԫ�غ�
				nPos=nPos++; 
		}else
		{
			ASSERT(nInsertPos!=-1);
			nPos=nInsertPos;
		}
		InsertAt(nPos,newElement);
		return nPos;
	}
	
	//******************************************
	//	name:SortFind
	//	function:����һ�������ڱ��е�λ�ã��������ݱ�������
	//	input:	T *pData �Ƚ�����
	//			CBCMP funCmp:�Ƚϻص��ӿ�
	//			LPARAM lParam:�û�����
	//			int nBegin:��ʼλ��
	//			BOOL bHeadFirst:����ͬ��Ԫ��ʱȷ���ǵ�һ���������һ��
	//	return:����λ�ã�-1��û���ҵ�
	//	2006.10.19
	//******************************************
	int SortFind(T *pData,FUNCMP funCmp=NULL,LPARAM lParam=0,int nBegin=0,UINT sfType=SFT_ANY)
	{
		if(nBegin>=(int)m_nCount)
			return -1;
		else
			return SortFindEx(pData,nBegin,m_nCount,funCmp,lParam,sfType);
	}

	//************************************************
	//	_SortFindEx
	//	��[nBegin,nEnd)֮�����һ������λ��
	//************************************************
	int SortFindEx(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam,UINT sfType,int *pInsertPos=NULL)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) //û�д���ռ�
		{
			if(pInsertPos) *pInsertPos=nBegin;
			return -1;
		}
		int nMiddle=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMiddle);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(sfType==SFT_ANY) 
				return nMiddle;
			int nRet=-1;
			if(sfType==SFT_HEAD)
				nRet=_SortFindHead(pData,nBegin,nMiddle,funCmp,lParam);
			else //if(sfType==SFT_TAIL)
				nRet=_SortFindTail(pData,nMiddle+1,nEnd,funCmp,lParam);
			if(nRet==-1) nRet=nMiddle;
			return nRet;
		}
		if(nBegin==nEnd-1)
		{//ֻ��һ��Ԫ�ؿռ�
			if(pInsertPos)
			{
				*pInsertPos=nMiddle;
				if(nCmp>0) (*pInsertPos)++;
			}
			return -1;
		}
		if(nCmp>0)
			return SortFindEx(pData,nMiddle+1,nEnd,funCmp,lParam,sfType,pInsertPos);
		else
			return SortFindEx(pData,nBegin,nMiddle,funCmp,lParam,sfType,pInsertPos);
	}

	//******************************************
	//	name:InsertAt
	//	function:������ָ��λ�ò���һ��������
	//	input:��int index-ָ��������
	//			T newElement-�����������
	//  return: BOOL TURE-�ɹ���FALSE��ʧ��
	//	remark: 
	//******************************************
	int InsertAt(DWORD index,T newElement)
	{
		if(index>m_nCount)
			return -1;
		if(index==m_nCount) 
			return Add(newElement);
		SARRAYNODE *pDest=GetDestSegEntry(index);
		if(pDest->dwUsed==m_nGrowBy)
		{
			SARRAYNODE *pAddNode=NewNode();
			if(!pAddNode) return -1;
			pAddNode->dwUsed=pDest->dwUsed-(WORD)(index-m_nCurIndex);
			memcpy(pAddNode->pData,
				pDest->pData+index-m_nCurIndex,
				sizeof(T)*pAddNode->dwUsed);
			*(pDest->pData+index-m_nCurIndex)=newElement;
			pDest->dwUsed=(WORD)(index-m_nCurIndex+1);
			SARRAYNODE *pNext=pDest->pNextNode;
			pDest->pNextNode=pAddNode;
			pAddNode->pPrevNode=pDest;
			if(pNext){
				pNext->pPrevNode=pAddNode;
				pAddNode->pNextNode=pNext;
			}else
			{
				m_pTailNode=pAddNode;
			}
			m_nEmpty+=m_nGrowBy-1;
		}else
		{
			memmove(pDest->pData+index-m_nCurIndex+1,
				pDest->pData+index-m_nCurIndex,
				(pDest->dwUsed-(index-m_nCurIndex))*sizeof(T));
			*(pDest->pData+index-m_nCurIndex)=newElement;
			pDest->dwUsed++;
			m_nEmpty--;
		}
		m_nCount++;
		if(IsNeedCompack()) Compack();
		return index;
	}

	//******************************************
	//	name:RemoveAt
	//	function:ɾ��������ָ���������а���������
	//	input:��int index-ָ��������
	//  return: BOOL TURE-�ɹ���FALSE��ʧ��
	//	remark: 
	//******************************************
	BOOL RemoveAt(DWORD index)
	{
		if(index<0||index>=m_nCount)
			return FALSE;
		SARRAYNODE *pDest=GetDestSegEntry(index);
		m_nCount--;
		if(pDest->dwUsed==1)
		{
			SARRAYNODE *pPrev=pDest->pPrevNode;
			SARRAYNODE *pNext=pDest->pNextNode;
			if(!pPrev) 
			{
				m_pHeadNode=pNext;
				if(m_pHeadNode) m_pHeadNode->pPrevNode=NULL;
			}else
			{
				pPrev->pNextNode=pNext;
			}
			if(!pNext)
			{
				m_pTailNode=pPrev;
				if(m_pTailNode) m_pTailNode->pNextNode=NULL;
			}else
			{
				pNext->pPrevNode=pPrev;
			}
			if(pDest==m_pCurNode)
			{
				m_pCurNode=m_pHeadNode;
				m_nCurIndex=m_pHeadNode?0:-1;
			}
			FreeNode(pDest);
			m_nEmpty-=(m_nGrowBy-1);
		}else
		{
			memmove(pDest->pData+index-m_nCurIndex,
				pDest->pData+index-m_nCurIndex+1,
				sizeof(T)*(pDest->dwUsed-(index-m_nCurIndex+1)));
			pDest->dwUsed--;
			m_nEmpty++;
			if(IsNeedCompack()) Compack();
		}
		return TRUE;
	}

	//******************************************
	//	name:SetSize()
	//	function:�������ݵ�����
	//	input:��int size -���ݵ�����
	//  return: BOOL TURE-�ɹ���FALSE��ʧ��
	//	remark:ֻ������������
	//******************************************
	BOOL SetSize(DWORD size){
		SARRAYNODE *pNewNode=NULL;
		if(m_nCount>=size)
			return FALSE;
		if(m_pTailNode)
		{
			if((WORD)(size-m_nCount)<=m_nGrowBy-m_pTailNode->dwUsed)
			{//neet not to enlarge the buffer
				m_pTailNode->dwUsed+=(WORD)(size-m_nCount);
				m_nEmpty-=size-m_nCount;
				m_nCount=size;
				return TRUE;
			}else if(m_pTailNode->dwUsed!=m_nGrowBy)
			{//fill the tail node to full
				m_nEmpty+=m_nGrowBy-m_pTailNode->dwUsed;
				m_nCount+=m_nGrowBy-m_pTailNode->dwUsed;
				m_pTailNode->dwUsed=m_nGrowBy;
			}
		}
		int newsegs=(size-m_nCount+m_nGrowBy-1)/m_nGrowBy;
		for(int i=0;i<newsegs;i++)
		{
			pNewNode=NewNode();
			if(!pNewNode) return FALSE;
			pNewNode->dwUsed=(i<newsegs-1)?m_nGrowBy:(WORD)(size-m_nCount);
			if(!m_pHeadNode)
			{
				m_pHeadNode=m_pTailNode=m_pCurNode=pNewNode;
				m_nCount+=pNewNode->dwUsed;
				m_nCurIndex=0;
				continue;
			}
			ASSERT(m_pTailNode);
			m_pTailNode->pNextNode=pNewNode;
			pNewNode->pPrevNode=m_pTailNode;
			m_pTailNode=pNewNode;
			m_nCount+=pNewNode->dwUsed;
		}
		return TRUE;
	}
	
	//******************************************
	//	name:SetGrowBy()
	//	function:����������������
	//	input:��
	//  return: 
	//	remark: �ڳ�ʼ��ʱʹ��
	//******************************************
	void SetGrowBy(WORD nGrowBy)
	{
		ASSERT(m_nCount==0);
		m_nGrowBy=nGrowBy;
	}

	//******************************************
	//	name:RemoveAll()
	//	function:��ն����е�����
	//	input:��
	//  return: BOOL TURE-�ɹ���FALSE��ʧ��
	//	remark:
	//******************************************
	BOOL RemoveAll(){
		if(m_pHeadNode==NULL)
			return TRUE;
		Free();
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
		return TRUE;
	}
	
	//*********************************************
	// name: Attach
	// function:��һ���ⲿ���齻��CSArray����,����������ʱ��������CSArray�ṩ��2�ֲ�ѯ����
	// input: T *pData,������; int nLen,���ݳ���
	// return:BOOL TURE-�ɹ���FALSE��ʧ��
	// remark:����Ϊ��ʱ����,�ⲿ������Ҫʹ��new�����ڴ�,������Ҫ����Detach���Լ��ֶ��ͷ��ڴ�
	//*********************************************
	BOOL Attach(T *pData,int nLen)
	{
		if(m_pHeadNode!=NULL) return FALSE;
		m_pCurNode=m_pHeadNode=m_pTailNode=new SARRAYNODE;
		m_pHeadNode->pNextNode=NULL;
		m_pHeadNode->pPrevNode=NULL;
		m_pHeadNode->pData=pData;
		m_pHeadNode->dwUsed=nLen;
		m_nGrowBy=nLen;
		m_nCount=nLen;
		m_nCurIndex=0;
		return TRUE;
	}
	
	//*********************************************
	// name: Detach
	// function:��ȡ�ڲ�����
	// param:
	//   int *pnLen[output]:���ݳ���
	// return:
	//	 NULL-ʧ��,����-����ָ��
	// remark:һ����Attach���ʹ��
	//*********************************************
	T *Detach(int *pnLen)
	{
		if(m_pHeadNode==NULL) return NULL;
		if(m_pHeadNode!=m_pTailNode) return NULL;
		if(pnLen) *pnLen=m_nCount;
		T *pRet=m_pHeadNode->pData;
		delete m_pHeadNode;
		m_pHeadNode=m_pTailNode=m_pCurNode=NULL;
		m_nCount=0;
		m_nCurIndex=0;
		return pRet;
	}

	//*********************************************
	//  name:Compack
	//	function :ѹ������
	//	remark:�������и����εĿ���ռ���������ͷŶ���Ľڵ�
	//*********************************************
	void Compack()
	{
		SARRAYNODE *pTmp1,*pTmp2=NULL;
		if(m_pHeadNode==m_pTailNode) return;
		pTmp1=FindNotFullNode(m_pHeadNode,m_pTailNode);
		while(pTmp1)
		{
			if(pTmp2==NULL)	pTmp2=pTmp1->pNextNode;
			ASSERT(pTmp2);
			while(pTmp1->dwUsed!=(DWORD)m_nGrowBy&&pTmp2)
			{
				if(pTmp2->dwUsed<=(m_nGrowBy-pTmp1->dwUsed))
				{//the node can free
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*pTmp2->dwUsed);
					pTmp1->dwUsed+=pTmp2->dwUsed;
					if(m_pTailNode==pTmp2)
					{
						m_pTailNode=pTmp2->pPrevNode;
						m_pTailNode->pNextNode=NULL;
						FreeNode(pTmp2);
						pTmp2=NULL;
					}else
					{
						pTmp2->pPrevNode->pNextNode=pTmp2->pNextNode;
						pTmp2->pNextNode->pPrevNode=pTmp2->pPrevNode;
						SARRAYNODE *p=pTmp2->pNextNode;
						FreeNode(pTmp2);
						pTmp2=p;
					}
				}else
				{//dest node remain data
					DWORD dwMoveItems=m_nGrowBy-pTmp1->dwUsed;
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*dwMoveItems);
					memmove(pTmp2->pData,
						pTmp2->pData+dwMoveItems,
						sizeof(T)*(pTmp2->dwUsed-dwMoveItems));
					pTmp2->dwUsed-=(WORD)dwMoveItems;
					pTmp1->dwUsed=m_nGrowBy;
				}
			}//while
			SARRAYNODE *pFind=FindNotFullNode(pTmp1,pTmp2);
			if(!pFind)
			{
				pTmp1=FindNotFullNode(pTmp1,m_pTailNode);
			}else
			{
				pTmp1=pFind;
			}
			pTmp2=NULL;
		}//while
		m_pCurNode=m_pHeadNode;//avoid the current node been deleted
		m_nCurIndex=0;
		if(m_pTailNode)
			m_nEmpty=m_nGrowBy-m_pTailNode->dwUsed;
		else
			m_nEmpty=0;
	}	
protected:


	//********************************************
	//	name:IsNeedCompack
	//	function:�ж��Ƿ���Ҫ�����ݽ���ѹ��
	//*********************************************
	virtual BOOL IsNeedCompack()
	{
		if(m_nEmpty<m_nGrowBy) return FALSE;
		return ((m_nEmpty-m_nGrowBy)*100>m_nCount*m_byZipKey);
	}
	//******************************************
	//	name:Free()
	//	function:�ͷŶ���ռ�õĿռ䣬
	//	input:
	//  return:void
	//	remark:�ڲ�ʹ��,�ⲿҪ��ն�����ʹ��RemoveAll()�ӿ�
	//******************************************
	virtual void Free()
	{
		SARRAYNODE *temp1,*temp2;
		temp1=m_pHeadNode;
		while(temp1!=NULL)
		{
			temp2=temp1->pNextNode;
			FreeNode(temp1);
			temp1=temp2;
		}
	}

	//T *pItem1 :�����õ�����
	//T *pItem2 :���������е�����
	int ItemCompare(T *pItem1,T *pItem2,FUNCMP funCmp,LPARAM lParam)
	{
		if(funCmp)
			return funCmp(pItem1,pItem2,lParam);
		else
			return memcmp(pItem1,pItem2,sizeof(T));
	}
	//************************************************
	//	_SortFindHead
	//	��[nBegin,nEnd)֮��������һ����pData��ȵ�Ԫ��
	//************************************************
	int _SortFindHead(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) return -1;
		int nMid=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMid);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(nBegin==nEnd-1)	//ֻ��һ��Ԫ��
				return nBegin;
			int nRet=_SortFindHead(pData,nBegin,nMid,funCmp,lParam);
			if(nRet==-1) nRet=nMid;
			return nRet;
		}else
		{
			return _SortFindHead(pData,nMid+1,nEnd,funCmp,lParam);
		}
	}

	//************************************************
	//	_SortFindTail
	//	��[nBegin,nEnd)֮���������һ����pData��ȵ�Ԫ��
	//************************************************
	int _SortFindTail(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) return -1;
		int nMid=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMid);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(nBegin==nEnd-1)	//ֻ��һ��Ԫ��
				return nBegin;
			int nRet=_SortFindTail(pData,nMid+1,nEnd,funCmp,lParam);
			if(nRet==-1) nRet=nMid;
			return nRet;
		}else
		{
			return _SortFindTail(pData,nBegin,nMid,funCmp,lParam);
		}
	}

	//************************************************
	//	FreeNode
	//	�ͷŸý��ռ�õ��ڴ�
	//************************************************
	void FreeNode(SARRAYNODE *pNode)
	{
		delete []pNode->pData;
		delete pNode;
	}
	//*********************************************
	//	�����п��пռ�Ľ��
	//*********************************************
	SARRAYNODE *FindNotFullNode(SARRAYNODE *pBegin,SARRAYNODE *pEnd)
	{
		SARRAYNODE *pRet=pBegin;
		if(!pEnd) pEnd=m_pTailNode;
		while(pRet&&pRet->dwUsed==m_nGrowBy&&pRet!=pEnd) pRet=pRet->pNextNode;
		if(pRet==m_pTailNode||pRet==pEnd) pRet=NULL;
		return pRet;
	}

	//*******************************************
	//	name NewNode
	//	function:Ϊһ���½�����ռ�
	//********************************************
	SARRAYNODE *NewNode()
	{
		SARRAYNODE *pRet=new SARRAYNODE;
		if(!pRet) return NULL;
		pRet->pPrevNode=pRet->pNextNode=NULL;
		pRet->dwUsed=0;
		pRet->pData=new T[m_nGrowBy];
		if(pRet->pData==NULL)
		{
			delete pRet;
			return NULL;
		}
		return pRet;
	}
	//******************************************
	//	name:GetDestSegEntry()
	//  function:��ȡ������������Ľڵ�ָ��
	//	input:	int index -��������
	//  return: SARRAYNODE * -������������Ľڵ�ָ��
	//	remark:�ڲ�ʹ�ã�
	//******************************************
	SARRAYNODE * GetDestSegEntry(DWORD index)
	{
		SARRAYNODE * ret=NULL;
		int			i=0;
		DWORD			offset=0;
		if(index<m_nCurIndex)// dest pData is in before cur pData segment 
		{
			if(m_nCurIndex>2*index)
			{	//find the seg from head;
				ret=m_pHeadNode;
				while(offset+ret->dwUsed<=index)
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}else	
			{	//find the seg from cur seg;
				ret=m_pCurNode;
				offset=m_nCurIndex;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}else if(index>=(m_nCurIndex+m_pCurNode->dwUsed))
		{
			if(m_nCurIndex+m_nCount<2*index)
			{//find the seg from cur
				ret=m_pCurNode;
				offset=m_nCurIndex;
				while(offset+ret->dwUsed<=index)
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}else
			{//find the seg from tail
				ret=m_pTailNode;
				offset=m_nCount-ret->dwUsed;;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}else
		{//in cur pData seg
			ret=m_pCurNode;
		}
		m_pCurNode=ret;
		return ret;
	}
	
/////////////////////////////////////////////////////////////////////
//  private data
	DWORD m_nCurIndex;	//��ǰ�ڵ�ĵ�һ��Ԫ�ص�������
	DWORD m_nCount;		//������������������
	DWORD m_nGrowBy;	//ÿ�������ĳߴ�
	DWORD m_nEmpty;		//�ճ��Ŀռ�����

	BYTE m_byZipKey;	//ѹ����ֵ
	SARRAYNODE * m_pCurNode;	//�����е�ǰ�ڵ��ָ�룬�����ݼ���ʱȷ���������������
	SARRAYNODE * m_pHeadNode;	//ͷ�ڵ�
	SARRAYNODE * m_pTailNode;	//β���
};

#endif // !defined(AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
