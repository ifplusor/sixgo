#ifndef SIXGO_H_BASE
#define SIXGO_H_BASE

#include "connect_six.h"

//������ĳ��4���ߵķ���������
#define ANGLE0	 0			// 0�Ƚ�
#define ANGLE45	 1			// 45�Ƚ�
#define ANGLE90	 2			// 90�Ƚ�
#define ANGLE135 3			// 135�Ƚ�

//����������ʱ�����ϵĵ������仯���ɹ�ʽ��
#define Increment0(x, y)		{(x)++;}
#define Increment45(x, y)		{(x)++;(y)++;}
#define Increment90(x, y)		{(y)++;}
#define Increment135(x, y)		{(x)++;(y)--;}

#define Decrement0(x, y)		{(x)--;}
#define Decrement45(x, y)		{(x)--;(y)--;}
#define Decrement90(x, y)		{(y)--;}
#define Decrement135(x, y)		{(x)--;(y)++;}

void Increment(int &x, int &y, BYTE lineDirec);
void deIncrement(int &x, int &y, BYTE lineDirec);
int GetLineKey(const Point point, Point *start, const BYTE lineDirec);		//��ӳ��,ȷ�������ڵ��ߵı��
void UniquePoint(vector<Point> &pointList);
void UniqueStep(vector<Step> &stepList);
bool cmpStepValue(const Step &s1, const Step &s2);

#endif