#ifndef SIXGO_H_HASH
#define SIXGO_H_HASH

#define MAXSIZE 1048576

#include "connect_six.h"
#include <time.h>

typedef struct _BoardCode
{
	unsigned long a[23];
	long hash32;
}BoardCode;

typedef struct _HashInfo
{
	BoardCode code;
	vector<Step> stepList;
	int value,myType,denType;//������в���͵���ҪĿ����Ϊ��ָ���з�����
	bool cut,full;//cut����֦��ǣ�full���ŷ��б������Ա�ǣ������������������ŷ��б���չ�������������ŷ�
	int index;//��ϣ�����ʱ�����������������HandNum��
	int depth;
}HashInfo;

extern HashInfo hashList[MAXSIZE];

void initialCode(BoardCode &code);
void moveCodeP(BoardCode &code,Point point,int side);
void moveCodeS(BoardCode &code,Step step,int side);
void initialHash();
long hashCode(BoardCode code);
bool compareCode(BoardCode &a,BoardCode &b);
HashInfo *findHash(BoardCode &code,int index);
void ReadCM(int color);

#endif