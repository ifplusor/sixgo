//connect_six.h
//=================================================================================================
//定义程序必要的数据结构及对数据结构的操作，保存程序所有函数的声明。
#ifndef SIXGO_H_MAIN
#define SIXGO_H_MAIN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


#define WINLOSE 1000000        //胜负局面的估值。是程序中最大的值。

#define BLACK 0
#define WHITE 1
#define EMPTY 2
const int edge = 19;

#define FX(i) (i+4) //方向方向宏

#define GetABit( Line , x )			( ( (Line) >> (x) ) & 1 )	//提取数‘Line’中第x位的二进制值
#define PutABit( Line , x )			( (Line) | ( 1 << (x) ) )	//将数‘Line’的第x二进制位赋1
#define PutBBit( Line , x )			( (Line) ^ ( 1 << (x) ) )	//将数‘Line’的第x二进制位取反
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

typedef struct _Point{ //点
	int x,y;
}Point;
typedef struct _Step{ //步
	Point first,second;
	int value;
}Step;


extern int HandNum;
extern int MaxDepth;
extern const int lineVector[8][2];


#endif
