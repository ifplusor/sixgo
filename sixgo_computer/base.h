#ifndef SIXGO_H_BASE
#define SIXGO_H_BASE

#include "connect_six.h"

//棋盘上某点4条线的方向特征。
#define ANGLE0	 0			// 0度角
#define ANGLE45	 1			// 45度角
#define ANGLE90	 2			// 90度角
#define ANGLE135 3			// 135度角

//按照线行走时，线上的点的坐标变化规律公式。
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
int GetLineKey(const Point point, Point *start, const BYTE lineDirec);		//线映射,确定点所在的线的编号
void UniquePoint(vector<Point> &pointList);
void UniqueStep(vector<Step> &stepList);
bool cmpStepValue(const Step &s1, const Step &s2);

#endif