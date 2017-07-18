#pragma once

//�ṩһ�������ļ�������ʽ�����ݻ�������������
typedef struct tagCOMFILE
{
	int	  nBufSize;	//�������ܳ���
	BYTE *pbyData;	//���ݻ�����
	int   nLen;		//��Ч���ݳ���
	int	  nOffset;	//��ǰ��дλ��
}COMFILE,*PCOMFILE;

typedef enum tagCFMODE{CFM_READ=0,CFM_WRITE=1}CFMODE;
#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

COMFILE CF_Init(LPBYTE pbyBuf,int nBufSize,int nValidSize,int nOffset);

int CF_Read(COMFILE *pcf,void *pBuf,int nSize);
BYTE CF_ReadChar(COMFILE *pcf);
int CF_Write(COMFILE *pcf,void *pBuf,int nSize);
int CF_WriteChar(COMFILE *pcf,BYTE byData);
BOOL CF_EOF(COMFILE *pcf);
int CF_Tell(COMFILE *pcf);
int CF_Seek(COMFILE *pcf,int nOffset,int nOrigin);

#ifdef __cplusplus
}
#endif//__cplusplus
