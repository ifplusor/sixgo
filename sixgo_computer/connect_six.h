//connect_six.h
//=================================================================================================
//��������Ҫ�����ݽṹ�������ݽṹ�Ĳ���������������к�����������
#ifndef SIXGO_H_BASE
#define SIXGO_H_BASE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


#define WINLOSE 1000000        //ʤ������Ĺ�ֵ���ǳ���������ֵ��

#define BLACK 0
#define WHITE 1
#define EMPTY 2
const int edge = 19;

#define FX(i) (i+4) //�������

#define GetABit( Line , x )			( ( (Line) >> (x) ) & 1 )	//��ȡ����Line���е�xλ�Ķ�����ֵ
#define PutABit( Line , x )			( (Line) | ( 1 << (x) ) )	//������Line���ĵ�x������λ��1
#define PutBBit( Line , x )			( (Line) ^ ( 1 << (x) ) )	//������Line���ĵ�x������λȡ��
#define AllOne( k )				( ( 1 << (k) ) -1 )

typedef struct _Point{ //��
	int x,y;
}Point;
typedef struct _Step{ //��
	Point first,second;
	int value;
}Step;

extern const int lineVector[8][2];
extern int SeachDepth,MaxDepth;
extern int virtualBoard[edge][edge];
extern int HandNum;

#endif
