#ifndef SIXGO_H_HASH
#define SIXGO_H_HASH

#define MAXSIZE 0x100000

#include "connect_six.h"
#include <time.h>

typedef struct _BoardCode
{
	unsigned long a[23];//˫������λͼ��361*2=722,23*32=736
	unsigned long hash32;//��ϣ����
}BoardCode;

typedef struct _HashInfo
{
	BoardCode code;//����״̬����
	vector<Step> stepList;//�ŷ��б�
	int value,myType,denType;//������в���͵���ҪĿ����Ϊ��ָ���з�����
	int timestamp;//��ϣ�����ʱ�����������������HandNum��
	bool cut;//cut����֦��ǣ����ڷ����ŷ���������߼�֦Ч��
	bool full;//full���ŷ��б������Ա�ǣ������������������ŷ��б���չ�������������ŷ�
}HashInfo;

typedef struct _EndLibInfo
{
	BoardCode code;
	_EndLibInfo *next;
}EndLibInfo;

extern HashInfo hashList[MAXSIZE];

void initialCode(BoardCode &code);
void initialHash();
bool compareCode(BoardCode &a,BoardCode &b);
unsigned long hashCode(BoardCode &code);
HashInfo *findHash(BoardCode &code);
unsigned long getHashCode(int index);
void moveCodeP(BoardCode &code,Point point,int side);
void moveCodeS(BoardCode &code,Step step,int side);

bool findEndLib(BoardCode &code);
bool StartEndLib();
void StopEndLib();
void InsertEndLib(BoardCode &code);

#endif