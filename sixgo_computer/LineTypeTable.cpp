/*
 * file: LineTypeTable.cpp
 * author: Jame Yin
 * date: 2015/5/6
 * description: ���ʹ����ļ��������ǻ���һ����������ģʽ���������ͷ�������ģ�͡�
 */

#include "linetype.h"


BYTE preTable[TableSize];					//���ͱ���
LineTypeInfo linetypeInfo[TableSize];


//���͹���
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
	char temp[5], filename[20];//ע�⣺�����ɵ����ζ����Ƶı�ʾ�У������ֵ�1��ʾ�з�һ�����ӻ�߽硣
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
			while (shapeIndex > 1)	//Ŀǰ�����������̣��κη����ߵĵ���������20����19������
			{
				if (shapeIndex & 1)
					fprintf(fp, "��");
				else
					fprintf(fp, "��");
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


//��������
//===============================================================================

//������͵ĳ���
int getLinetypeLen(const int index)
{
	int len = 19;
	while (!GetABit(index, len))--len;
	return len;
}

//�������[s,t)�����ڵ������
int getLinetypePoint(const int index, const int s, const int t)
{
	int sum = 0;
	for (int i = s; i < t; ++i)
		if (GetABit(index, i))++sum;
	return sum;
}

//�������[s,t)�����Ƿ������в��
bool getLinetypeThreat(const int index, const int s, const int t)
{
	int sum = getLinetypePoint(index, s, s + 6);
	if (sum >= 4)return true;//�����������ϰ�������4���㣬Ϊ��в�ͣ��ƽ�ʧ��
	for (int i = s; i < t - 6; ++i)
	{
		sum -= GetABit(index, i);
		sum += GetABit(index, i + 6);
		if (sum >= 4)return true;//�����������ϰ�������4���㣬Ϊ��в�ͣ��ƽ�ʧ��
	}
	return false;
}

//����ߵ����ͣ������ڷּ�
void GetMode(LPBYTE lpArray)
{
	int len, i, shapeIndex, sum, max;
	for (shapeIndex = 0; shapeIndex < 64; ++shapeIndex)//64����Ϊ�޼�ֵ����
		lpArray[shapeIndex] = VALUELESS;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		len = getLinetypeLen(shapeIndex);
		sum = getLinetypePoint(shapeIndex, 0, 6);
		if (sum >= 6)//����Ϊ6��
		{
			lpArray[shapeIndex] = 6;
			continue;
		}
		max = sum;
		for (i = 0; i < len - 6; ++i)
		{
			sum -= GetABit(shapeIndex, i);
			sum += GetABit(shapeIndex, i + 6);
			if (sum > max)//�������������к��еı������������ֵȷ���ߵġ��͡�
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

//������ʤ����
void createWIN(LPBYTE lpArray)
{
	int shapeIndex;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] == 6)//6�ͼ�Ϊ��ʤ����
			lpArray[shapeIndex] = WIN;
	}
	printf("Finish create WIN .\n");
}

//���ɶ���в���ͣ�ͨ�������Ӳ��߽����жϣ�
void createTHREAT_THIRD(LPBYTE lpArray)
{
	int shapeIndex, len, i, j, sum;
	for (shapeIndex = 64; shapeIndex<TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 5 && lpArray[shapeIndex] != 4)continue;//����в����ֻ�ֲ���5�ͺ�6����
		len = getLinetypeLen(shapeIndex);
		sum = getLinetypePoint(shapeIndex, 0, len);
		if (3 + sum>len)continue;//�����������յ㣬���������������յ��ϼ����ƽ�

		//һ�����������Ϊ��������Ч����(û��������Ч���͵Ŀ���)
		for (i = 0; i < len - 1; ++i)
		{
			if (GetABit(shapeIndex, i) != 0)continue;
			for (j = i + 1; j < len; ++j)
			{
				if (GetABit(shapeIndex, j) != 0)continue;
				if (i < 6 && (j - i) <= 6 && (len - j) > 6)//���λ������Ч
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len))goto loop;
				}
				else if (i < 6 && (j - i) > 6 && (len - j) <= 6)//�м�������Ч
				{
					if (!getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else if (i >= 6 && (j - i) <= 6 && (len - j) <= 6)//���λ������Ч
				{
					if (!getLinetypeThreat(shapeIndex, 0, i))goto loop;
				}
				else if (i < 6 && (j - i) > 6 && (len - j) > 6)//�ߡ�����Ч
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len) && !getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else if (i >= 6 && (j - i) <= 6 && (len - j) > 6)//�ߡ�����Ч
				{
					if (!getLinetypeThreat(shapeIndex, j + 1, len) && !getLinetypeThreat(shapeIndex, 0, i))goto loop;
				}
				else if (i > 5 && (j - i) > 6 && (len - j) <= 6)//�С�����Ч
				{
					if (!getLinetypeThreat(shapeIndex, 0, i) && !getLinetypeThreat(shapeIndex, i + 1, j))goto loop;
				}
				else//����Ч
				{
					goto loop;
				}
			}
		}
		if (lpArray[shapeIndex] == 5)//5������һ��5
			lpArray[shapeIndex] = THREAT_five_THIRD;
		else//4������һ��4
			lpArray[shapeIndex] = THREAT_four_THIRD;
	loop:;//ֻҪ��һ�ֿ��ƽ��ŷ�������Ϊ����в����
	}
	printf("Finish create THREAT_THIRD( THREAT_five_THIRD & THREAT_four_THIRD ).\n");
}

//����˫��в���ͣ�ͨ����һ�Ӳ��߽����жϣ�
void createTHREAT_DOUBLE(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 5 && lpArray[shapeIndex] != 4)continue;//˫��в����ֻ�ֲ���5�ͺ�4����
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)//һ�����������Ϊ��������Ч����
		{
			if (GetABit(shapeIndex, i))continue;
			if (i < 6 && (len - i) > 6)//���λ������Ч
			{
				if (!getLinetypeThreat(shapeIndex, i + 1, len))goto loop;
			}
			else if (i >= 6 && (len - i) <= 6)//���λ������Ч
			{
				if (!getLinetypeThreat(shapeIndex, 0, i))goto loop;
			}
			else if (i >= 6 && (len - i) > 6)//�ߡ�����Ч
			{
				if (!getLinetypeThreat(shapeIndex, 0, i) && !getLinetypeThreat(shapeIndex, i + 1, len))goto loop;
			}
			else//����Ч
			{
				goto loop;
			}
		}
		if (lpArray[shapeIndex] == 5)
			lpArray[shapeIndex] = THREAT_five_DOUBLE;
		else
			lpArray[shapeIndex] = THREAT_four_DOUBLE;
	loop:;//ֻҪ��һ�ֿ��ƽ��ŷ�������Ϊ����в����
	}
	printf("Finish create THREAT_DOUBLE( THREAT_five_DOUBLE & THREAT_four_DOUBLE )\n");
}

//�������ӵ���в���ͣ�ʣ��5�ͼ��ǣ�
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
		if (lpArray[shapeIndex] != THREAT_four_DOUBLE)continue;//˫��в����ֻ�ֲ���5�ͺ�6����
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_five_THIRD || lpArray[shapeIndex + (1 << i)] == THREAT_four_THIRD)//��ԭ��������Ϊ������
				lpArray[shapeIndex] = THREAT_four_SUPERPOTEN;
		}
	}
	printf("Finish create THREAT_four_SUPERPOTEN.\n");
}

//�������ӵ���в��Ǳ������
void createTHREAT_four_BIGPOTEN(LPBYTE lpArray)
{
	int i, shapeIndex, len;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != 4)continue;//˫��в����ֻ�ֲ���5�ͺ�6����
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if (GetABit(shapeIndex, i))continue;
			if (lpArray[shapeIndex + (1 << i)] == THREAT_five_THIRD || lpArray[shapeIndex + (1 << i)] == THREAT_four_THIRD)//��ԭ��������Ϊ������
				lpArray[shapeIndex] = THREAT_four_BIGPOTEN;
		}
	}
	printf("Finish create THREAT_four_BIGPOTEN.\n");
}

//�������ӵ���в˫Ǳ������
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

//����������˫��в˫Ǳ�����ͣ�����7����λ������Ϊ�գ��м�����������һ��Ϊ�գ�
void createTHREAT_four_ADDITION(LPBYTE lpArray)
{
	int i, shapeIndex, len, sum = 0;
	for (shapeIndex = 64; shapeIndex < TableSize; ++shapeIndex)
	{
		if (lpArray[shapeIndex] != THREAT_four_POTEN)continue;//THREAT_four_ADDITION�����Ǵ�THREAT_four_POTEN�����������ģ�ʣ��4���в�������
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

//�������ӵ���в���ͣ�ʣ��4�ͼ��ǣ�
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
 * AnalyDownOne - ����ָ���������͵�һ�¼���ָ��
 *  1. �������͵�����
 *  2. ��һ�ӿ�ת����ĳ�����͵���������
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
		++num;//�ۼ���������
		for (i = 0; i <= WIN; ++i) makedo[i] = 0;
		len = getLinetypeLen(shapeIndex);
		for (i = 0; i < len; ++i)
		{
			if ((shapeIndex >> i) & 1) continue;//�յ�����
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

//�������Ϳ�
void createLinetype(LPBYTE lpArray)
{
	//*���׽��з���
	GetMode(lpArray);//��ȡ���͵Ľ�

	//*�����ʽ�����ϸ�Ķ��η���
	createWIN(lpArray);//�궨��ʤ����
	createTHREAT_THIRD(lpArray);//�궨����в���ͣ����һ�����Ľ�һ����
	createTHREAT_DOUBLE(lpArray);//�궨˫��в���ͣ���׶������Ľ�...��
	createTHREAT_five_SINGLE(lpArray);//�궨�����������
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

//��ʼ�����ͱ�
int initialLineTypeTable(LPBYTE lpArray)
{
	if (readLinetype(lpArray))//���ȶ�ȡ���߱�����߳�ʼ��ʱ��
	{
		createLinetype(lpArray);
		writeLinetype(lpArray);
	}
	return 0;
}


//������Ϣ����
//=====================================================================================================================

//��ʼ��������Ϣ����
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

//����������
void AnalyDefentStep(int shapeIndex, int len, LineTypeInfo *the)
{
	int i, j;
	iStep tempStep;
	for (i = 0; i < len - 1; ++i)//һ�����������Ϊ��������Ч����(û��������Ч���͵Ŀ���)
	{
		if (GetABit(shapeIndex, i) != 0)continue;
		for (j = i + 1; j < len; ++j)
		{
			if (GetABit(shapeIndex, j) != 0)continue;
			if (i < 6 && (j - i) <= 6 && (len - j) > 6)//���λ������Ч
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i < 6 && (j - i) > 6 && (len - j) <= 6)//�м�������Ч
			{
				if (getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) <= 6 && (len - j) <= 6)//���λ������Ч
			{
				if (getLinetypeThreat(shapeIndex, 0, i))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i < 6 && (j - i) > 6 && (len - j) > 6)//�ߡ�����Ч
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len) || getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) <= 6 && (len - j) > 6)//�ߡ�����Ч
			{
				if (getLinetypeThreat(shapeIndex, j + 1, len) || getLinetypeThreat(shapeIndex, 0, i))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else if (i >= 6 && (j - i) > 6 && (len - j) <= 6)//�С�����Ч
			{
				if (getLinetypeThreat(shapeIndex, 0, i) || getLinetypeThreat(shapeIndex, i + 1, j))continue;
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
			else//����Ч
			{
				tempStep.first = i;
				tempStep.second = j;
				the->defStepList.push_back(tempStep);
			}
		}
	}
}

//����������
void AnalyDefentPoint(int shapeIndex, int len, LineTypeInfo *the)
{
	int i;
	for (i = 0; i < len; ++i)//һ�����������Ϊ��������Ч����
	{
		if (GetABit(shapeIndex, i))continue;
		if (i < 6 && (len - i) > 6)//���λ������Ч
		{
			if (getLinetypeThreat(shapeIndex, i + 1, len))continue;
			the->defPointList.push_back(i);
		}
		else if (i >= 6 && (len - i) <= 6)//���λ������Ч
		{
			if (getLinetypeThreat(shapeIndex, 0, i))continue;
			the->defPointList.push_back(i);
		}
		else if (i >= 6 && (len - i) > 6)//�ߡ�����Ч
		{
			if (getLinetypeThreat(shapeIndex, 0, i) || getLinetypeThreat(shapeIndex, i + 1, len))continue;
			the->defPointList.push_back(i);
		}
		else//����Ч
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

void analyTHREAT_four_ADDITION(int shapeIndex, int len, LineTypeInfo *the)//�Ľ��弶  ��������ʤ���㣬˫��в�㣻�ƽⲽ���ƽ��
{
	int i, sum, t;
	iStep tempStep;

	the->lineType = 4;

	the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//����ʤ����

	for (i = 0; i < len; i++)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t == THREAT_five_DOUBLE || t == THREAT_four_DOUBLE)//˫��в�㣻����������׶������ͣ���©��ԭ��
			the->duoThreatList.push_back(i);
	}

	AnalyDefentPoint(shapeIndex, len, the);

	//�����ƽⲽ
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

void analyTHREAT_four_POTEN(int shapeIndex, int len, LineTypeInfo *the)//�Ľ�����  ����ʤ���㣬˫��в�㣬�ƽ��
{
	int i, t;

	the->lineType = 4;

	the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//����ʤ����
	if (the->willWin == -1)
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//����ʤ����

	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t == THREAT_five_DOUBLE || t == THREAT_four_BIGPOTEN || t == THREAT_four_DOUBLE)//˫��в��
			the->duoThreatList.push_back(i);
	}

	AnalyDefentPoint(shapeIndex, len, the);
}

void analyPOTEN_three_SUPERPOTEN(int shapeIndex, int len, LineTypeInfo *the)//����һ������ϣ�  ��˫��в�㣬����в�㣬����Ǳ����
{
	the->lineType = 3;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_SUPERPOTEN)//��Ǳ��
			the->duoThreatList.push_back(i);
	}
}

void analyPOTEN_three_DOUBLE(int shapeIndex, int len, LineTypeInfo *the)//���׶���  ˫��в�㣬����в��
{
	int i, t;
	the->lineType = 2;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_DOUBLE)//˫��в��
			the->duoThreatList.push_back(i);
		else if (t >= THREAT_four_SINGLE)//����в��
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_three_BIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)//������������ϣ�  ����в�㣬��Ǳ����
{
	the->lineType = 1;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_BIGPOTEN)
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_three_ADDITION(int shapeIndex, int len, LineTypeInfo *the)//�����ļ�  ����в�㣬˫Ǳ����
{
	int i, t;
	the->lineType = 1;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_SINGLE)//����в��
			the->solThreatList.push_back(i);
		else if (t >= POTEN_three_DOUBLE)//˫Ǳ����
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_three_ANDPOTEN(int shapeIndex, int len, LineTypeInfo *the)//�����弶  ����в�㣬˫Ǳ����
{
	int i, t;
	the->lineType = 1;
	for (i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		t = preTable[shapeIndex + (1 << i)];
		if (t >= THREAT_four_SINGLE)//����в��
			the->solThreatList.push_back(i);
		else if (t >= POTEN_three_DOUBLE)//˫Ǳ����
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_three_ORPOTEN(int shapeIndex, int len, LineTypeInfo *the)//�������������ͣ�  ����в�㣬˫Ǳ����
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

void analyPOTEN_three_SINGLE(int shapeIndex, int len, LineTypeInfo *the)//�����߼�  ����в��
{
	the->lineType = 1;
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == THREAT_four_SINGLE)//����в��
			the->solThreatList.push_back(i);
	}
}

void analyPOTEN_two_TOBIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)//����һ��
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_three_DOUBLE)//˫Ǳ����
			the->duoPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOADDITION(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] >= POTEN_three_SINGLE)//��Ǳ����
			the->solPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOANDPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] >= POTEN_three_SINGLE)//��Ǳ����
			the->solPotenList.push_back(i);
	}
}

void analyPOTEN_two_TOPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_three_SINGLE)//��Ǳ����
			the->solPotenList.push_back(i);
	}
}

void analyVOID_one_TOBIGPOTEN(int shapeIndex, int len, LineTypeInfo *the)
{
	for (int i = 0; i < len; ++i)
	{
		if (GetABit(shapeIndex, i))continue;
		if (preTable[shapeIndex + (1 << i)] == POTEN_two_TOBIGPOTEN)//Ǳ˫Ǳ����
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


//���ͷ���
void AnalyLineInfo(int shapeIndex, LineTypeInfo *the)
{
	int len = getLinetypeLen(shapeIndex);//���͵ĳ��ȡ�	

	switch (preTable[shapeIndex])
	{
	case WIN:
		the->lineType = 7;
		break;

	//��в�ͣ�5��ȡ����ʤ�㣬4��ȡ�ü���ʤ����
	case THREAT_five_THIRD://���һ��(����)  ��ʤ��
		the->lineType = 6;
		the->win = getPointIndex(shapeIndex, len, WIN);
		break;
	case THREAT_five_DOUBLE://��׶���  ��ʤ�㣻�ƽⲽ
		the->lineType = 5;
		the->win = getPointIndex(shapeIndex, len, WIN);
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_five_SINGLE://�������  ��ʤ�㣻�ƽ��
		the->lineType = 4;
		the->win = getPointIndex(shapeIndex, len, WIN);
		AnalyDefentPoint(shapeIndex, len, the);
		break;
	case THREAT_four_THIRD://�Ľ�һ�������ͣ�  ����ʤ���㣨һ��5��
		the->lineType = 6;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_THIRD);
		break;
	case THREAT_four_SUPERPOTEN://�Ľ׶��������ͣ�  ����ʤ����Ͷ���в�㣻�ƽⲽ
		the->lineType = 5;
		the->willWin = the->triThreat = getPointIndex(shapeIndex, len, THREAT_five_THIRD);//�������һ��������ʤ���㣬����в�㣩
		if (the->willWin == -1)//�����������һ�����֣������Ľ�һ��
		{
			the->triThreat = getPointIndex(shapeIndex, len, THREAT_four_THIRD);//�����Ľ�һ��������в�㣩
			the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//������׶���������ʤ���㣩
		}
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_four_DOUBLE://�Ľ�����  ����ʤ������ƽⲽ
		the->lineType = 5;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//������׶���������ʤ���㣩
		AnalyDefentStep(shapeIndex, len, the);
		break;
	case THREAT_four_BIGPOTEN://�Ľ��ļ������ͣ�  ��������ʤ����Ͷ���в�㣻�ƽ��
		the->lineType = 4;
		the->triThreat = getPointIndex(shapeIndex, len, THREAT_four_THIRD);//�����Ľ�һ��������в�㣩
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_DOUBLE);//�����������������ʤ���㣩
		if (the->willWin == -1)
			the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//������׶���������ʤ���㣩
		AnalyDefentPoint(shapeIndex, len, the);
		break;
	case THREAT_four_ADDITION://�Ľ��弶  ��������ʤ���㣬˫��в�㣬�ƽⲽ���ƽ��
		analyTHREAT_four_ADDITION(shapeIndex, len, the);
		break;
	case THREAT_four_POTEN://�Ľ�����  ����ʤ���㣬˫��в�㣻�ƽ��
		analyTHREAT_four_POTEN(shapeIndex, len, the);
		break;
	case THREAT_four_SINGLE://�Ľ��߼�  ��������ʤ���㣨����5�����ƽ��
		the->lineType = 4;
		the->willWin = getPointIndex(shapeIndex, len, THREAT_five_SINGLE);//����ʤ����
		AnalyDefentPoint(shapeIndex, len, the);
		break;

	//����в��
	case POTEN_three_SUPERPOTEN://����һ������ϣ�  ��˫��в�㣬����в�㣬����Ǳ����
		analyPOTEN_three_SUPERPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_DOUBLE://���׶���  ˫��в�㣬����в��
		analyPOTEN_three_DOUBLE(shapeIndex, len, the);
		break;
	case POTEN_three_BIGPOTEN://������������ϣ�  ����в�㣬��Ǳ����
		analyPOTEN_three_BIGPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_ADDITION://�����ļ�  ����в�㣬˫Ǳ����
		analyPOTEN_three_ADDITION(shapeIndex, len, the);
		break;
	case POTEN_three_ANDPOTEN://�����弶  ����в�㣬˫Ǳ����
		analyPOTEN_three_ANDPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_ORPOTEN://�������������ͣ�  ����в�㣬˫Ǳ����
		analyPOTEN_three_ORPOTEN(shapeIndex, len, the);
		break;
	case POTEN_three_SINGLE://�����߼�  ����в��
		analyPOTEN_three_SINGLE(shapeIndex, len, the);
		break;
	case POTEN_two_TOBIGPOTEN://����һ��
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
	case VOID_one_TOADDITION://���
		analyVOID_one_TOADDITION(shapeIndex, len, the);
		break;
	case VOID_one_TOANDPOTEN://���
		analyVOID_one_TOANDPOTEN(shapeIndex, len, the);
		break;
	case VOID_one_TOPOTEN://���
		analyVOID_one_TOPOTEN(shapeIndex, len, the);
		break;
	case ZERO://������ͣ���ϣ�
		if (GetABit(shapeIndex, len / 2))
			the->toOne = len / 2 + 1;
		else
			the->toOne = len / 2;
		break;
	}
}

//����������Ϣ��
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

//��ʼ��������Ϣ��
void initialLineInfoTable(LineTypeInfo *linetypeInfo)
{
	createLineInfoTable(linetypeInfo);
}


//�¾ɴ��루δ�����Ƴ���
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

BYTE preTab[TableSize];//����������ͱ��ļ���ȡ��������Ϣ
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
