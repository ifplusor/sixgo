#ifndef SIXGO_H_LINETYPR
#define SIXGO_H_LINETYPE

#include "connect_six.h"

#define NEW
#define _K_		//��������


#define TableSize 1048576     //�����ͱ��С
#define TabSize 1048512		//�����ͱ��С��Ϊ�˶�ȡ�����ͱ��ļ�����

#ifdef NEW
//�����ͱ��Ƕ������ͱ����չ��ͨ���������ͱ����ʹӱ�ǿɵþ����ͱ��ʿɸ��������ͱ����ɴ������������ͱ�

#define WIN 27						//��ʤ����
#define THREAT_five_THIRD 26
#define THREAT_five_DOUBLE 25
#define THREAT_five_SINGLE 24
#define THREAT_four_THIRD 23
#define THREAT_four_SUPERPOTEN 22
#define THREAT_four_DOUBLE 21
#define THREAT_four_BIGPOTEN 20
#define THREAT_four_ADDITION 19
#define THREAT_four_POTEN 18
#define THREAT_four_SINGLE 17
#define POTEN_three_SUPERPOTEN 16
#define POTEN_three_DOUBLE 15
#define POTEN_three_BIGPOTEN 14
#define POTEN_three_ADDITION 13
#define POTEN_three_ANDPOTEN 12
#define POTEN_three_ORPOTEN 11
#define POTEN_three_SINGLE 10
#define POTEN_two_TOBIGPOTEN 9
#define POTEN_two_TOADDITION 8
#define POTEN_two_TOANDPOTEN 7
#define POTEN_two_TOPOTEN 6
#define VOID_one_TOBIGPOTEN 5
#define VOID_one_TOADDITION 4
#define VOID_one_TOANDPOTEN 3
#define VOID_one_TOPOTEN 2
#define ZERO 1						//������
#define VALUELESS 0

#else

#define THREAT 9
#define WIN 15
#define THREAT_THIRD 14
#define THREAT_five_DOUBLE 13
#define THREAT_five_SINGLE 12
#define THREAT_four_DOUBLE 11
#define THREAT_four_POTEN 10
#define THREAT_four_SINGLE 9
#define POTEN_three_DOUBLE 8
#define POTEN_three_twoDOUBLE 7
#define POTEN_three_SINGLE 6
#define POTEN_two_DOUBLE 5
#define POTEN_two_SINGLE 4
#define POTEN_two_SINGLE2 3
#define one_DOUBLE 2
#define one_SINGLE 1
#define ZERO one_SINGLE
#define VALUELESS -1

#define THREAT_five_THIRD THREAT_THIRD
#define THREAT_four_THIRD THREAT_THIRD
#define THREAT_four_SUPERPOTEN THREAT_four_DOUBLE
#define THREAT_four_BIGPOTEN THREAT_four_POTEN
#define THREAT_four_ADDITION THREAT_four_POTEN
#define POTEN_three_SUPERPOTEN POTEN_three_DOUBLE
#define POTEN_three_BIGPOTEN POTEN_three_twoDOUBLE
#define POTEN_three_ADDITION POTEN_three_twoDOUBLE
#define POTEN_three_ANDPOTEN POTEN_three_twoDOUBLE
#define POTEN_three_ORPOTEN POTEN_three_twoDOUBLE
#define POTEN_two_TOBIGPOTEN POTEN_two_DOUBLE
#define POTEN_two_TOADDITION POTEN_two_SINGLE
#define POTEN_two_TOANDPOTEN POTEN_two_SINGLE
#define POTEN_two_TOPOTEN POTEN_two_SINGLE2
#define VOID_one_TOBIGPOTEN one_DOUBLE
#define VOID_one_TOADDITION one_SINGLE
#define VOID_one_TOANDPOTEN one_SINGLE
#define VOID_one_TOPOTEN one_SINGLE

#endif



typedef unsigned char BYTE;
typedef BYTE* LPBYTE;

typedef int iPoint;
typedef struct _iStep
{
	iPoint first,second;
}iStep;

typedef struct _linetypeinfo
{
	int lineType;			//�ߵ�����  0������в��1����Ǳ����2��˫Ǳ����3����Ǳ����4:����в��5��˫��в��6������в��7����ʤ��

	int win;//5->6����ʤ��
	int willWin;//4->5:������ʤ�㣨ֻ�ڵ���в������ʤ��ʱʹ�ã�
	int triThreat;//4.2->5.1/4.1 4.4->4.1:����в�㣨ֻ�ڲ�����в�������ʹ�ã�
	//����������������������һ������ֻȡһ������

	vector<iStep> defStepList;//�������������ƽ�˫��в
	vector<iPoint> defPointList;//�����㣬�����ƽⵥ��в

	vector<iPoint> duoThreatList;//˫��в�㣬��������˫��в�ĵ�
	vector<iPoint> solThreatList;//����в�㣬�������ɵ���в�ĵ�
	vector<iPoint> duoPotenList;//˫Ǳ���㣬��������˫Ǳ���ĵ�
	vector<iPoint> solPotenList;//��Ǳ���㣬�������ɵ�Ǳ���ĵ�
	vector<iPoint> toDuoTwoList;//Ǳ˫Ǳ����

	//���³�Ա����Ҫ
	vector<iPoint> toSolTwoList;//Ǳ��Ǳ����
	int toOne;//һ�׵�
}LineTypeInfo;

typedef struct _linesum
{
	unsigned int duoThreat;//��������˫��в�ĵ�
	unsigned int solThreat;//�������ɵ���в�ĵ�
	unsigned int duoPoten;//��������˫Ǳ���ĵ�
	unsigned int solPoten;//�������ɵ�Ǳ���ĵ�
	unsigned int toDuoTwo;
	unsigned int toSolTwo;
	unsigned int defPoint;
	unsigned int defStep;
}LineSum;

extern BYTE preTable[TableSize];					//���ͱ���
extern LineTypeInfo linetypeInfo[TableSize];				//���Ͷ�Ӧ�Ĺؼ���Ϣ

int initialLineTypeTable(LPBYTE preTable);
void initialLineInfoTable(LineTypeInfo *linetypeInfo);


#endif