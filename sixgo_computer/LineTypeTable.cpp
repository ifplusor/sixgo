/*
 * file: LineTypeTable.cpp
 * author: Jame Yin
 * date: 2015/5/6
 * description: 线型处理文件。线型是基于一子增量连珠模式建立的棋型分析评估模型。
 */

#include "linetype.h"


BYTE preTable[TableSize];					//线型表项
LineTypeInfo linetypeInfo[TableSize];


//线型管理
//=====================================================================
int readLinetype(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if ((dataFile = fopen("_LineTypeTable.me", "rb")) != NULL)
	{
		numRead = fread(lpArray, sizeof(BYTE), TableSize, dataFile);
		fclose(dataFile);
		printf("Successed in Opening the file preTable !\n");
	}
	else
	{
		printf("Failed in Opening the file preTable !\n");
		return 1;
	}
	return 0;
}
int writeLinetype(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if ((dataFile = fopen("_LineTypeTable.me", "wb")) != NULL)
	{
		numRead = fwrite(lpArray, sizeof(BYTE), TableSize, dataFile);
		fclose(dataFile);
		printf("Successed in writing the file preTable !\n");
	}
	else
	{
		printf("Failed in writing the file preTable !\n");
		return 1;
	}
	return 0;
}

void writeLinetypeT(LPBYTE lpArray, BYTE tag)
{
	FILE * fp;
	int j, sum;
	char temp[5], filename[20];//注意：所生成的棋形二进制的表示中，最后出现的1表示敌方一个棋子或边界。
	int shapeIndex;
	itoa(tag, temp, 10);
	strcpy(filename, "LineType\\");
	strcat(filename, temp);
	strcat(filename, ".txt");
	fp = fopen(filename, "wt");
	if (fp == NULL)
	{
		printf("out fail !\n");
		return;
	}
	sum = 0;
	for (j = 0; j < TableSize; j++)
	{
		shapeIndex = j;
		if (lpArray[j] == tag)
		{
			sum++;
			fprintf(fp, "%8d:", shapeIndex);
			while (shapeIndex > 1)	//目前的六子棋棋盘，任何方向线的点数不超过20个（19个）。
			{
				if (shapeIndex & 1)
					fprintf(fp, "●");
				else
					fprintf(fp, "┼");
				shapeIndex = shapeIndex >> 1;
			}
			fprintf(fp, "\n");
		}
	}
	fprintf(fp, "\n\ntotal:%d\n", sum);
	fclose(fp);
	printf("Successed write Linetype for txt!\n");
}

void outLinetypeT(LPBYTE lpArray)
{
	for (int i = WIN; i >= ZERO; --i)
		writeLinetypeT(lpArray, i);
}


//线型生成
//===============================================================================

//获得线型的长度
int getLinetypeLen(const int index)
{
	int len = 19;
	while (!GetABit(index, len))--len;
	return len;
}

//获得线型[s,t)区间内点的数量
int getLinetypePoint(const int index, const int s, const int t)
{
	int sum = 0;
	for (int i = s; i < t; ++i)
		if (GetABit(index, i))++sum;
	return sum;
}

//获得线型[s,t)区间是否具有威胁性
bool getLinetypeThreat(const int index, const int s, const int t)
{
	int sum = getLinetypePoint(index, s, s + 6);
	if (sum >= 4)return true;//连续六个点上包含至少4个点，为威胁型，破解失败
	for (int i = s; i < t - 6; ++i)
	{
		sum -= GetABit(index, i);
		sum += GetABit(index, i + 6);
		if (sum >= 4)return true;//连续六个点上包含至少4个点，为威胁型，破解失败
	}
	return false;
}

//获得线的类型，后续在分级
void GetMode(LPBYTE lpArray)
{
	int len, i, shapeIndex, sum, max;
	for (shapeIndex = 0; shapeIndex < 64; ++shapeIndex)//64以下为无价值索引
		lpArray[shapeIndex] = VALUELESS;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		len = getLinetypeLen(shapeIndex);
		sum = getLinetypePoint(shapeIndex, 0, 6);
		if (sum >= 6)//六子为6型
		{
			lpArray[shapeIndex] = 6;
			continue;
		}
		max = sum;
		for (i = 0; i < len - 6; ++i)
		{
			sum -= GetABit(shapeIndex, i);
			sum += GetABit(shapeIndex, i + 6);
			if (sum > max)//由连续六个点中含有的本方子数的最大值确定线的‘型’
			{
				if (sum >= 6)
				{
					max = 6;
					break;
				}
				else
					max = sum;
			}
		}
		lpArray[shapeIndex] = max;
	}
	printf("Finish get line Mode.\n");
}

//生成已胜线型
void createWIN(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 6)//6型即为已胜线型
			lpArray[shapeIndex] = WIN;
	}
	printf("Finish create WIN .\n");
}

//生成多威胁线型（通过断两子拆线进行判断）
void createTHREAT_THIRD(LPBYTE lpArray)
{
	int shapeIndex, len, i, j, sum;
	for (shapeIndex = 64; shapeIndex<TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 5 && lpArray[shapeIndex] != 4)continue;//多威胁线型只分布在5型和6型中
		len = getLinetypeLen(shapeIndex);
		sum = getLinetypePoint(shapeIndex, 0, len);
		if (3 + sum>len)continue;//至少有三个空点，否则落子在两个空点上即可破解

		//一条线型最多拆分为有两组有效线型(没有三组有效线型的可能)
		for (i = 0; i < len - 1; ++i)
		{
			if (GetABit(shapeIndex, i) != 0)continue;
			for (j = i + 1; j < len; ++j)
			{
				if (GetABit(shapeIndex, j) != 0)continue;
				if (i < 6 && (j - i) <= 6 && (len - j) > 6)//最高位线型有效
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len))goto loop;
				}
				else if (i < 6 && (j - i) > 6 && (len - j) <= 6)//中间线型有效
				{
					if (!getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else if (i >= 6 && (j - i) <= 6 && (len - j) <= 6)//最低位线型有效
				{
					if (!getLinetypeThreat(shapeIndex, 0, i))goto loop;
				}
				else if (i < 6 && (j - i) > 6 && (len - j) > 6)//高、中有效
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len) && !getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else if (i >= 6 && (j - i) <= 6 && (len - j) > 6)//高、低有效
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len) && !getLinetypeThreat(shapeIndex, 0, i))goto loop;
				}
				else if (i > 5 && (j - i) > 6 && (len - j) <= 6)//中、低有效
				{
					if (!getLinetypeThreat(shapeIndex, 0, i) && !getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else//均无效
				{
					goto loop;
				}
			}
		}
		if (lpArray[shapeIndex] == 5)//5型衍生一级5
			lpArray[shapeIndex] = THREAT_five_THIRD;
		else//4型衍生一级4
			lpArray[shapeIndex] = THREAT_four_THIRD;
	loop:;//只要有一种可破解着法，即不为多威胁线型
	}
	printf("Finish create THREAT_THIRD( THREAT_five_THIRD & THREAT_four_THIRD ).\n");
}

//生成双威胁线型（通过断一子拆线进行判断）
void createTHREAT_DOUBLE(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 5 && lpArray[shapeIndex] != 4)continue;//双威胁线型只分布在5型和4型中
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)//一条线型最多拆分为有两组有效线性
		{
			if (GetABit(shapeIndex, i))continue;
			if (i < 6 && (len - i) > 6)//最高位线型有效
			{
				if (!getLinetypeThreat(shapeIndex, i + 1, len))goto loop;
			}
			else if (i >= 6 && (len - i) <= 6)//最低位线型有效
			{
				if (!getLinetypeThreat(shapeIndex, 0, i))goto loop;
			}
			else if (i >= 6 && (len - i) > 6)//高、低有效
			{
				if (!getLinetypeThreat(shapeIndex, 0, i) && !getLinetypeThreat(shapeIndex, i + 1, len))goto loop;
			}
			else//均无效
			{
				goto loop;
			}
		}
		if (lpArray[shapeIndex] == 5)
			lpArray[shapeIndex] = THREAT_five_DOUBLE;
		else
			lpArray[shapeIndex] = THREAT_four_DOUBLE;
	loop:;//只要有一种可破解着法，即不为多威胁线型
	}
	printf("Finish create THREAT_DOUBLE( THREAT_five_DOUBLE & THREAT_four_DOUBLE )\n");
}

//生成五子单威胁线型（剩余5型即是）
void createTHREAT_five_SINGLE(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 5)
			lpArray[shapeIndex] = THREAT_five_SINGLE;
	}
	printf("Finish create THREAT_five_SINGLE.\n");
}

void createTHREAT_four_SUPERPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != THREAT_four_DOUBLE)continue;//双威胁线型只分布在5型和6型中
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_five_THIRD || lpArray[shapeIndex + (1 << i)] == THREAT_four_THIRD)//从原线型衍生为新线型
				lpArray[shapeIndex] = THREAT_four_SUPERPOTEN;
		}
	}
	printf("Finish create THREAT_four_SUPERPOTEN.\n");
}

//生成四子单威胁多潜力线型
void createTHREAT_four_BIGPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 4)continue;//双威胁线型只分布在5型和6型中
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_five_THIRD || lpArray[shapeIndex + (1 << i)] == THREAT_four_THIRD)//从原线型衍生为新线型
				lpArray[shapeIndex] = THREAT_four_BIGPOTEN;
		}
	}
	printf("Finish create THREAT_four_BIGPOTEN.\n");
}

//生成四子单威胁双潜力线型
void createTHREAT_four_POTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 4)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_DOUBLE || lpArray[shapeIndex + (1 << i)] == THREAT_four_SUPERPOTEN || lpArray[shapeIndex + (1 << i)] == THREAT_five_DOUBLE)
			{
				lpArray[shapeIndex] = THREAT_four_POTEN;
				break;
			}
		}
	}
	printf("Finish create THREAT_four_POTEN.\n");
}

//生成四子弱双威胁双潜力线型（连续7个点位，两端为空，中间三个点用有一个为空）
void createTHREAT_four_ADDITION(LPBYTE lpArray)
{
	int i, shapeIndex, len, sum = 0;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != THREAT_four_POTEN)continue;//THREAT_four_ADDITION线型是从THREAT_four_POTEN线型中衍生的，剩余4型中不能衍生
		len = getLinetypeLen(shapeIndex);
		sum = getLinetypePoint(shapeIndex, 1, 6);
		if (sum == 4 && GetABit(shapeIndex, 1) == 1 && GetABit(shapeIndex, 5) == 1 && GetABit(shapeIndex, 0) == 0 && GetABit(shapeIndex, 6) == 0)
		{
			lpArray[shapeIndex] = THREAT_four_ADDITION;
			continue;
		}
		for (i = 1; i < len - 6; ++i)
		{
			sum -= GetABit(shapeIndex, i);
			sum += GetABit(shapeIndex, i + 5);
			if (sum == 4 && GetABit(shapeIndex, i + 1) == 1 && GetABit(shapeIndex, i + 5) == 1 && GetABit(shapeIndex, i) == 0 && GetABit(shapeIndex, i + 6) == 0)
			{
				lpArray[shapeIndex] = THREAT_four_ADDITION;
				break;
			}
		}
	}
	printf("Finish create THREAT_four_ADDITION .\n");
}

//生成四子单威胁线型（剩余4型即是）
void createTHREAT_four_SINGLE(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 4)
			lpArray[shapeIndex] = THREAT_four_SINGLE;
	}
	printf("Finish create THREAT_four_SINGLE.\n");
}

void createPOTEN_three_SUPERPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_SUPERPOTEN)
			{
				lpArray[shapeIndex] = POTEN_three_SUPERPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_SUPERPOTEN.\n");
}

void createPOTEN_three_DOUBLE(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_DOUBLE)
			{
				lpArray[shapeIndex] = POTEN_three_DOUBLE;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_DOUBLE.\n");
}

void createPOTEN_three_BIGPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_BIGPOTEN)
			{
				lpArray[shapeIndex] = POTEN_three_BIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_BIGPOTEN.\n");
}

void createPOTEN_three_ADDITION(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_ADDITION)
			{
				lpArray[shapeIndex] = POTEN_three_ADDITION;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ADDITION.\n");
}

void createPOTEN_three_ANDPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_four_POTEN)
			{
				lpArray[shapeIndex] = POTEN_three_ANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ANDPOTEN.\n");
}

void createPOTEN_three_ORPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 3)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_three_SUPERPOTEN)
			{
				lpArray[shapeIndex] = POTEN_three_ORPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ORPOTEN.\n");
}

void createPOTEN_three_SINGLE(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 3)
			lpArray[shapeIndex] = POTEN_three_SINGLE;
	}
	printf("Finish create POTEN_three_SINGLE.\n");
}

void createPOTEN_two_TOBIGPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 2)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_three_DOUBLE)
			{
				lpArray[shapeIndex] = POTEN_two_TOBIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOBIGPOTEN.\n");
}

void createPOTEN_two_TOADDITION(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 2)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_three_ADDITION)
			{
				lpArray[shapeIndex] = POTEN_two_TOADDITION;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOADDITION.\n");
}

void createPOTEN_two_TOANDPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 2)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_three_ANDPOTEN)
			{
				lpArray[shapeIndex] = POTEN_two_TOANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOANDPOTEN.\n");
}

void createPOTEN_two_TOPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 2)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_three_SINGLE)
			{
				lpArray[shapeIndex] = POTEN_two_TOPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOPOTEN.\n");
}

void createVOID_one_TOBIGPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 1)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_two_TOBIGPOTEN)
			{
				lpArray[shapeIndex] = VOID_one_TOBIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOBIGPOTEN.\n");
}

void createVOID_one_TOADDITION(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 1)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_two_TOADDITION)
			{
				lpArray[shapeIndex] = VOID_one_TOADDITION;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOADDITION.\n");
}

void createVOID_one_TOANDPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 1)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_two_TOANDPOTEN)
			{
				lpArray[shapeIndex] = VOID_one_TOANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOANDPOTEN.\n");
}

void createVOID_one_TOPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 1)continue;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == POTEN_two_TOPOTEN)
			{
				lpArray[shapeIndex] = VOID_one_TOPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOPOTEN.\n");
}

void createZERO(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 0)
			lpArray[shapeIndex] = ZERO;
	}
	printf("Finish create ZERO.\n");
}


/** 
 * AnalyDownOne - 分析指定类型线型的一下几个指标
 *  1. 该类线型的数量
 *  2. 落一子可转化成某类线型的线型数量
 */
void AnalyDownOne(LPBYTE lpArray, BYTE tag)
{
	int i, shapeIndex, len;
	int num, sum[WIN + 1], makedo[WIN + 1];
	for (i = 0; i <= WIN; i++)
		sum[i] = 0;
	num = 0;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != tag) continue;
		++num;//累计线型数量
		for (i = 0; i <= WIN; ++i) makedo[i] = 0;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if ((shapeIndex >> i) & 1) continue;//空点落子
			makedo[lpArray[shapeIndex + (1 << i)]] |= 1;
		}
		for (i = 0; i <= WIN; ++i)sum[i] += makedo[i];
	}
	printf("- %d:%d  ", tag, num);
	for (i = WIN; i > 0; --i)
	{
		if (sum[i] != 0) printf("%d:%d ", i, sum[i]);
	}
	printf("\n");
}

void AnalyLinetype(LPBYTE lpArray)
{
	for (int i = WIN; i >= ZERO; --i)
		AnalyDownOne(lpArray, i);
}

//生成线型库
void createLinetype(LPBYTE lpArray)
{
	//*按阶进行分类
	GetMode(lpArray);//获取线型的阶

	//*按性质进行详细的二次分类
	createWIN(lpArray);//标定已胜线型
	createTHREAT_THIRD(lpArray);//标定多威胁线型（五阶一级、四阶一级）
	createTHREAT_DOUBLE(lpArray);//标定双威胁线型（五阶二级，四阶...）
	createTHREAT_five_SINGLE(lpArray);//标定五阶三级线型
	createTHREAT_four_SUPERPOTEN(lpArray);
	createTHREAT_four_BIGPOTEN(lpArray);
	createTHREAT_four_POTEN(lpArray);
	createTHREAT_four_ADDITION(lpArray);
	createTHREAT_four_SINGLE(lpArray);
	createPOTEN_three_SUPERPOTEN(lpArray);
	createPOTEN_three_DOUBLE(lpArray);
	createPOTEN_three_BIGPOTEN(lpArray);
	createPOTEN_three_ADDITION(lpArray);
	createPOTEN_three_ANDPOTEN(lpArray);
	createPOTEN_three_ORPOTEN(lpArray);
	createPOTEN_three_SINGLE(lpArray);
	createPOTEN_two_TOBIGPOTEN(lpArray);
	createPOTEN_two_TOADDITION(lpArray);
	createPOTEN_two_TOANDPOTEN(lpArray);
	createPOTEN_two_TOPOTEN(lpArray);
	createVOID_one_TOBIGPOTEN(lpArray);
	createVOID_one_TOADDITION(lpArray);
	createVOID_one_TOANDPOTEN(lpArray);
	createVOID_one_TOPOTEN(lpArray);
	createZERO(lpArray);
}

//初始化线型表
int initialLineTypeTable(LPBYTE lpArray)
{
	if (readLinetype(lpArray))//优先读取离线表，以提高初始化时间
	{
		createLinetype(lpArray);
		writeLinetype(lpArray);
	}
	return 0;
}


//线型信息分析
//=====================================================================================================================

//初始化线型信息表项
void initialLineInfo(LineTypeInfo *linetypeInfo)
{
	linetypeInfo->lineType = 0;

	linetypeInfo->win = -1;//5->6
	linetypeInfo->willWin = -1;//4->5
	linetypeInfo->triThreat = -1;
	linetypeInfo->toOne = -1;//0->1

	linetypeInfo->defStepList.clear();
	linetypeInfo->defPointList.clear();
	linetypeInfo->duoThreatList.clear();
	linetypeInfo->solThreatList.clear();
	linetypeInfo->duoPotenList.clear();
	linetypeInfo->solPotenList.clear();
	linetypeInfo->toDuoTwoList.clear();
	linetypeInfo->toSolTwoList.clear();
}

//解析防御步
void AnalyDefentStep(int shapeIndex, int len, LineTypeInfo *the)
{
	int i, j;
	iStep tempStep;
	for (i = 0; i < len - 1; ++i)//一条线型最多拆分为有两组有效线性(没有三组有效线型的可能)
	{
		if (GetABit(shapeIndex, i) != 0)continue;
		for (j = i + 1; j < len; ++j)
		{
			if (GetABit(shapeIndex, j) != 0)continue;
			if (i < 6 && (j - i) <= 6 && (len - j) > 6)//最高位线型有效
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i < 6 && (j - i) > 6 && (len - j) <= 6)//中间线型有效
			{
				if (getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) <= 6 && (len - j) <= 6)//最低位线型有效
			{
				if (getLinetypeThreat(shapeIndex, 0, i))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i < 6 && (j - i) > 6 && (len - j) > 6)//高、中有效
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len) || getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) <= 6 && (len - j) > 6)//高、低有效
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len) || getLinetypeThreat(shapeIndex, 0, i))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) > 6 && (len - j) <= 6)//中、低有效
			{
				if (getLinetypeThreat(shapeIndex, 0, i) || getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else//均无效
			{
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
		}
	}
}

//解析防御点
void AnalyDefentPoint(int shapeIndex, int len, LineTypeInfo *the)
{
	int i;
	for (i = 0; i < len; ++i)//一条线型最多拆分为有两组有效线性
	{
		if (GetABit(shapeIndex, i))continue;
		if (i < 6 && (len - i) > 6)//最高位线型有效
		{
			if (getLinetypeThreat(shapeIndex, i + 1, len))continue;
			the->defPointList.push_back(i);
		}
		else if (i >= 6 && (len - i) <= 6)//最低位线型有效
		{
			if (getLinetypeThreat(shapeIndex, 0, i))continue;
			the->defPointList.push_back(i);
		}
		else if (i >= 6 && (len - i) > 6)//高、低有效
		{
			if (getLinetypeThreat(shapeIndex, 0, i) || getLinetypeThreat(shapeIndex, i + 1, len))continue;
			the->defPointList.push_back(i);
		}
		else//均无效
			the->defPointList.push_back(i);
	}
}

int getPointIndex(int shapeIndex, int len, int tag)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == tag)
			return i;
	}
	return -1;
}

void analyTHREAT_four_ADDITION(int shapeIndex, int len, LineTypeInfo *the)//四阶五级  衍生即将胜利点，双威胁点；破解步，破解点
{
	int i, sum, t;
	iStep tempStep;

	the->lineType = 4;

	the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//即将胜利点

	for (i = 0; i < len; i++)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t == THREAT_five_DOUBLE || t == THREAT_four_DOUBLE)//双威胁点；必能生成五阶二级线型，不漏点原则
			the->duoThreatList.push_back(i);
	}

	AnalyDefentPoint(shapeIndex, len, the);

	//解析破解步
	sum = getLinetypeThreat(shapeIndex, 1, 6);
	if (sum == 4 && GetABit(shapeIndex, 1) == 1 && GetABit(shapeIndex, 5) == 1 && GetABit(shapeIndex, 0) == 0 && GetABit(shapeIndex, 6) == 0)
	{
		tempStep.first = 0;
		tempStep.second = 6;
		the->defStepList.push_back(tempStep);
	}
	for (i = 1; i < len - 6; i++)
	{
		sum -= GetABit(shapeIndex, i);
		sum += GetABit(shapeIndex, i + 5);
		if (sum == 4 && GetABit(shapeIndex, i + 1) == 1 && GetABit(shapeIndex, i + 5) == 1 && GetABit(shapeIndex, i) == 0 && GetABit(shapeIndex, i + 6) == 0)
		{
			tempStep.first = i;
			tempStep.second = i + 6;
			the->defStepList.push_back(tempStep);
		}
	}
}

void analyTHREAT_four_POTEN(int shapeIndex, int len, LineTypeInfo *the)//四阶六级  即将胜利点，双威胁点，破解点
{
	int i, t;

	the->lineType = 4;

	the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//即将胜利点
	if (the->willWin == -1)
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//即将胜利点

	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t == THREAT_five_DOUBLE || t == THREAT_four_BIGPOTEN || t == THREAT_four_DOUBLE)//双威胁点
			the->duoThreatList.push_back(i);
	}

	AnalyDefentPoint(shapeIndex, len, the);
}

void analyPOTEN_three_SUPERPOTEN(int shapeIndex, int len, LineTypeInfo *the)//三阶一级（半废）  （双威胁点，单威胁点，）多潜力点
{
	the->lineType = 3;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_SUPERPOTEN)//多潜力
			the->duoThreatList.push_back(i);
	}
}

void analyPOTEN_three_DOUBLE(int shapeIndex, int len, LineTypeInfo *the)//三阶二级  双威胁点，单威胁点
{
	int i, t;
	the->lineType = 2;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_DOUBLE)//双威胁点
			the->duoThreatList.push_back(i);
		else if (t >= THREAT_four_SINGLE)//单威胁点
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_three_BIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)//三阶三级（半废）  单威胁点，多潜力点
{
	the->lineType = 1;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_BIGPOTEN)
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_three_ADDITION(int shapeIndex, int len, LineTypeInfo *the)//三阶四级  单威胁点，双潜力点
{
	int i, t;
	the->lineType = 1;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_SINGLE)//单威胁点
			the->solThreatList.push_back(i);
		else if (t >= POTEN_three_DOUBLE)//双潜力点
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_three_ANDPOTEN(int shapeIndex, int len, LineTypeInfo *the)//三阶五级  单威胁点，双潜力点
{
	int i, t;
	the->lineType = 1;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_SINGLE)//单威胁点
			the->solThreatList.push_back(i);
		else if (t >= POTEN_three_DOUBLE)//双潜力点
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_three_ORPOTEN(int shapeIndex, int len, LineTypeInfo *the)//三阶六级（废型）  单威胁点，双潜力点
{
	int i, t;
	the->lineType = 1;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t == THREAT_four_SINGLE)
			the->solThreatList.push_back(i);
		else if (t == POTEN_three_SUPERPOTEN)
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_three_SINGLE(int shapeIndex, int len, LineTypeInfo *the)//三阶七级  单威胁点
{
	the->lineType = 1;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_SINGLE)//单威胁点
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_two_TOBIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)//二阶一级
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_three_DOUBLE)//双潜力点
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOADDITION(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] >= POTEN_three_SINGLE)//单潜力点
			the->solPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOANDPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] >= POTEN_three_SINGLE)//单潜力点
			the->solPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_three_SINGLE)//单潜力点
			the->solPotenList.push_back(i);
	}
}

void analyVOID_one_TOBIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_two_TOBIGPOTEN)//潜双潜力点
			the->toDuoTwoList.push_back(i);
	}
}

void analyVOID_one_TOADDITION(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_two_TOADDITION)
			the->toSolTwoList.push_back(i);
	}
}

void analyVOID_one_TOANDPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_two_TOANDPOTEN)
			the->toSolTwoList.push_back(i);
	}
}

void analyVOID_one_TOPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i<len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_two_TOPOTEN)
			the->toSolTwoList.push_back(i);
	}
}


//线型分析
void AnalyLineInfo(int shapeIndex, LineTypeInfo *the)
{
	int len = getLinetypeLen(shapeIndex);//线型的长度。	

	switch (preTable[shapeIndex])
	{
	case WIN:
		the->lineType = 7;
		break;

	//威胁型，5型取得致胜点，4型取得即将胜利点
	case THREAT_five_THIRD://五阶一级(废型)  致胜点
		the->lineType = 6;
		the->win = getPointIndex(shapeIndex, len, WIN);
		break;
	case THREAT_five_DOUBLE://五阶二级  致胜点；破解步
		the->lineType = 5;
		the->win = getPointIndex(shapeIndex, len, WIN);
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_five_SINGLE://五阶三级  致胜点；破解点
		the->lineType = 4;
		the->win = getPointIndex(shapeIndex, len, WIN);
		AnalyDefentPoint(shapeIndex, len, the);
		break;
	case THREAT_four_THIRD://四阶一级（废型）  即将胜利点（一级5）
		the->lineType = 6;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_THIRD);
		break;
	case THREAT_four_SUPERPOTEN://四阶二级（废型）  即将胜利点和多威胁点；破解步
		the->lineType = 5;
		the->willWin = the->triThreat = getPointIndex(shapeIndex, len, THREAT_five_THIRD);//衍生五阶一级（即将胜利点，多威胁点）
		if (the->willWin == -1)//不能衍生五阶一级部分，衍生四阶一级
		{
			the->triThreat = getPointIndex(shapeIndex, len, THREAT_four_THIRD);//衍生四阶一级（多威胁点）
			the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//衍生五阶二级（即将胜利点）
		}
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_four_DOUBLE://四阶三级  即将胜利点和破解步
		the->lineType = 5;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//衍生五阶二级（即将胜利点）
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_four_BIGPOTEN://四阶四级（废型）  衍生即将胜利点和多威胁点；破解点
		the->lineType = 4;
		the->triThreat = getPointIndex(shapeIndex, len, THREAT_four_THIRD);//衍生四阶一级（多威胁点）
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//衍生五阶三级（即将胜利点）
		if (the->willWin == -1)
			the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//衍生五阶二级（即将胜利点）
		AnalyDefentPoint(shapeIndex, len, the);
		break;
	case THREAT_four_ADDITION://四阶五级  衍生即将胜利点，双威胁点，破解步，破解点
		analyTHREAT_four_ADDITION(shapeIndex, len, the);
		break;
	case THREAT_four_POTEN://四阶六级  即将胜利点，双威胁点；破解点
		analyTHREAT_four_POTEN(shapeIndex, len, the);
		break;
	case THREAT_four_SINGLE://四阶七级  衍生即将胜利点（三级5）；破解点
		the->lineType = 4;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//即将胜利点
		AnalyDefentPoint(shapeIndex, len, the);
		break;

	//非威胁型
	case POTEN_three_SUPERPOTEN://三阶一级（半废）  （双威胁点，单威胁点，）多潜力点
		analyPOTEN_three_SUPERPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_DOUBLE://三阶二级  双威胁点，单威胁点
		analyPOTEN_three_DOUBLE(shapeIndex, len, the);
		break;
	case POTEN_three_BIGPOTEN://三阶三级（半废）  单威胁点，多潜力点
		analyPOTEN_three_BIGPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_ADDITION://三阶四级  单威胁点，双潜力点
		analyPOTEN_three_ADDITION(shapeIndex, len, the);
		break;
	case POTEN_three_ANDPOTEN://三阶五级  单威胁点，双潜力点
		analyPOTEN_three_ANDPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_ORPOTEN://三阶六级（废型）  单威胁点，双潜力点
		analyPOTEN_three_ORPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_SINGLE://三阶七级  单威胁点
		analyPOTEN_three_SINGLE(shapeIndex, len, the);
		break;
	case POTEN_two_TOBIGPOTEN://二阶一级
		analyPOTEN_two_TOBIGPOTEN(shapeIndex, len, the);
		break;
	case POTEN_two_TOADDITION:
		analyPOTEN_two_TOADDITION(shapeIndex, len, the);
		break;
	case POTEN_two_TOANDPOTEN:
		analyPOTEN_two_TOANDPOTEN(shapeIndex, len, the);
		break;
	case POTEN_two_TOPOTEN:
		analyPOTEN_two_TOPOTEN(shapeIndex, len, the);
		break;
	case VOID_one_TOBIGPOTEN:
		analyVOID_one_TOBIGPOTEN(shapeIndex, len, the);
		break;
	case VOID_one_TOADDITION://半废
		analyVOID_one_TOADDITION(shapeIndex, len, the);
		break;
	case VOID_one_TOANDPOTEN://半废
		analyVOID_one_TOANDPOTEN(shapeIndex, len, the);
		break;
	case VOID_one_TOPOTEN://半废
		analyVOID_one_TOPOTEN(shapeIndex, len, the);
		break;
	case ZERO://零阶线型（半废）
		if (GetABit(shapeIndex, len / 2))
			the->toOne = len / 2 + 1;
		else
			the->toOne = len / 2;
		break;
	}
}

//创建线型信息表
void createLineInfoTable(LineTypeInfo *linetypeInfo)
{
	int i, t = 0;
	for (i = 0; i < 64; i++)
		initialLineInfo(&linetypeInfo[i]);
	for (i = 64; i < TableSize; i++)
	{
		initialLineInfo(&linetypeInfo[i]);
		AnalyLineInfo(i, &linetypeInfo[i]);
	}
}

//初始化线型信息表
void initialLineInfoTable(LineTypeInfo *linetypeInfo)
{
	createLineInfoTable(linetypeInfo);
}


//陈旧代码（未来可移除）
//=========================================================================
#ifndef NEW
int readLinetypeO(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if ((dataFile = fopen("_ConnPreTbl.me", "rb")) != NULL)
	{
		numRead = fread(lpArray, sizeof(BYTE), TabSize, dataFile);
		fclose(dataFile);
		printf("Successed in Opening the file preTable !\n");
	}
	else
	{
		printf("Failed in Opening the file preTable !\n");
		return 1;
	}
	return 0;
}

int compare(LPBYTE lpA, LPBYTE lpB, int num)
{
	int i, ta = 1;
	printf("Matching %d...\n", num);
	for (i = 64; i<TableSize; i++)
		if (lpB[i] == num)
			if (lpA[i - 64] != num)
			{
				ta = 0;
				printf("B-A Not match %d !  %d\n", i, num);
				getchar();
			}
	for (i = 0; i<TabSize; i++)
		if (lpA[i] == num)
			if (lpB[i + 64] != num)
			{
				ta = 0;
				printf("A-B Not match %d !  %d\n", i + 64, num);
				getchar();
			}
	if (ta)
	{
		printf("Successed match %d!\n", num);
		return 0;
	}
	return -1;
}

BYTE preTab[TableSize];//保存从老线型表文件读取的类型信息
void compareOld(LPBYTE lpArray)
{
	int i;
	readLinetypeO(preTab);
	if (compare(preTab, lpArray, WIN))
		return;
	if (compare(preTab, lpArray, THREAT_THIRD))
		return;
	if (compare(preTab, lpArray, THREAT_five_DOUBLE))
		return;
	if (compare(preTab, lpArray, THREAT_five_SINGLE))
		return;
	if (compare(preTab, lpArray, THREAT_four_DOUBLE))
		return;
	if (compare(preTab, lpArray, THREAT_four_POTEN))
		return;
	if (compare(preTab, lpArray, THREAT_four_SINGLE))
		return;
	if (compare(preTab, lpArray, POTEN_three_DOUBLE))
		return;
	if (compare(preTab, lpArray, POTEN_three_twoDOUBLE))
		return;
	if (compare(preTab, lpArray, POTEN_three_SINGLE))
		return;
	if (compare(preTab, lpArray, POTEN_two_DOUBLE))
		return;
	if (compare(preTab, lpArray, POTEN_two_SINGLE))
		return;
	if (compare(preTab, lpArray, POTEN_two_SINGLE2))
		return;
	if (compare(preTab, lpArray, one_DOUBLE))
		return;
	if (compare(preTab, lpArray, one_SINGLE))
		return;
}
#endif

void updataSum(LineSum *sum, LineTypeInfo *lineInfo)
{
	if (lineInfo->duoThreatList.size()>sum->duoThreat)
		sum->duoThreat = lineInfo->duoThreatList.size();
	if (lineInfo->solThreatList.size() > sum->solThreat)
		sum->solThreat = lineInfo->solThreatList.size();
	if (lineInfo->duoPotenList.size() > sum->duoPoten)
		sum->duoPoten = lineInfo->duoPotenList.size();
	if (lineInfo->solPotenList.size() > sum->solPoten)
		sum->solPoten = lineInfo->solPotenList.size();
	if (lineInfo->toDuoTwoList.size() > sum->toDuoTwo)
		sum->toDuoTwo = lineInfo->toDuoTwoList.size();
	if (lineInfo->toSolTwoList.size() > sum->toSolTwo)
		sum->toSolTwo = lineInfo->toSolTwoList.size();
	if (lineInfo->defStepList.size() > sum->defStep)
		sum->defStep = lineInfo->defStepList.size();
	if (lineInfo->defPointList.size() > sum->defPoint)
		sum->defPoint = lineInfo->defPointList.size();
}
