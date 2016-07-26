//connect_six.h
//=================================================================================================
//��������Ҫ�����ݽṹ�������ݽṹ�Ĳ���������������к�����������
#ifndef SIXGO_H_MAIN
#define SIXGO_H_MAIN

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

#ifdef _WIN32
typedef unsigned __int64 UINT64;
#endif

#ifdef __gnu_linux__
#define _MAX_PATH 260
typedef unsigned long long UINT64;
#endif

typedef unsigned char BYTE;
typedef BYTE* LPBYTE;

typedef struct _Point{ //��
	int x,y;
}Point;
typedef struct _Step{ //��
	Point first,second;
	int value;
}Step;


extern int HandNum;
extern int MaxDepth;
extern const int lineVector[8][2];


#endif
