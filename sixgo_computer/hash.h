#ifndef SIXGO_H_HASH
#define SIXGO_H_HASH

#define MAXSIZE 0x100000

#include "connect_six.h"

typedef struct _BoardCode
{
	unsigned long a[23];//˫������λͼ��361*2=722,23*32=736
	unsigned long hash32;//��ϣ����
}BoardCode;

typedef struct _HashInfo
{
	BoardCode code;//����״̬����
	vector<Step> stepList;//�ŷ��б�
	int value,denType;//������в���͵���ҪĿ����Ϊ��ָ���з�����
	int timestamp;//��ϣ�����ʱ�����������������HandNum��
	bool cut;//cut����֦��ǣ����ڷ����ŷ���������߼�֦Ч��
	bool full;//full���ŷ��б������Ա�ǣ������������������ŷ��б���չ�������������ŷ�
}HashInfo;

//�о�Ϊ�ذܾ֣�ͨ��DTSS����ѧϰ
typedef struct _EndLibInfo
{
	BoardCode code;
	_EndLibInfo *next;
}EndLibInfo;


void initialHashBoard();
void initialHashList();
void initialHashCode(BoardCode &code);
unsigned long getHashCode(int index);
unsigned long hashCode(const BoardCode &code);
bool compareCode(const BoardCode &a,const BoardCode &b);
HashInfo *findHash(const BoardCode &code);
void updateHash(const HashInfo &data);
void moveCodeP(BoardCode &code,Point point,int side);
void moveCodeS(BoardCode &code,Step step,int side);

bool StartEndLib();
void StopEndLib();
bool findEndLib(const BoardCode &code);
void InsertEndLib(const BoardCode &code);

#endif