#include "ChessManual.h"


/**
* ChessImage - ����ӳ��
* @i:	ӳ����
* @x:	����x����
* @y:	����y����
*/
void ChessImage(int i, int &x, int &y)
{
	int t;
	if (i == 0){ x = x; y = y; } //ԭ������
	else if (i == 1){ x = 18 - x; y = y; }  //����x=9�Գ�
	else if (i == 2){ x = x; y = 18 - y; }  //����y=9�Գ�
	else if (i == 3){ x = 18 - x; y = 18 - y; }  //����(9,9)���ĶԳ�
	else if (i == 4){ t = x; x = y; y = t; }  //����y=19-x�Գ�
	else if (i == 5){ t = x; x = y; y = 18 - t; }  //�ȹ���x=9�Գƣ��ٹ���19-x�Գ�
	else if (i == 6){ t = x; x = 18 - y; y = t; }  //�ȹ���y=9�Գƣ��ٹ���19-x�Գ�
	else if (i == 7){ t = x; x = 18 - y; y = 18 - t; }  //�ȹ���(9,9),���ĶԳƣ��ٹ���y=19-x�Գ�
}

/**
* ReadCM - ��ȡ���ֿ�
* @color:	ִ����ɫ
*/
void ReadCM(int color)
{
	BoardCode HashCM[8];
	Step step[8];
	Point point;
	unsigned long hash;
	int count = 0, handNum;
	char str;
	char num[8];
	char filename[_MAX_PATH];
	int x, y;
	FILE *F1;
	if (color == BLACK)	//��ȡ�ڷ�������
	{
		for (int k = 0; k<1000; k++)
		{
			itoa(k + 1, num, 10);
			strcpy(filename, "���׼�¼\\�ڷ�����\\"); strcat(filename, num); strcat(filename, ".sgf");
			if ((F1 = fopen(filename, "r")) == NULL)continue;	//���û�ҵ����׾�����һ��
			for (y = 0; y<8; y++)
				initialCode(HashCM[y]);
			handNum = 0;
			while (1)
			{
				handNum++;
				fscanf(F1, "%c,%d,%d\n", &str, &x, &y);
				if (str == 'V')break;
				else if (str == 'J' || x<0)continue;
				for (int i = 0; i<8; i++)
				{
					point.x = x;
					point.y = y;
					ChessImage(i, point.x, point.y);	//��Ҫ�����8���ԳƵ�
					if (str == 'B')
						moveCodeP(HashCM[i], point, BLACK);
					else if (str == 'W')
						moveCodeP(HashCM[i], point, WHITE);
					else if (str == 'P')	//��Ӧ���з�
					{
						if ((count / 8) % 2 == 0)
						{
							step[i].first = point;
							count++;
						}
						else if ((count / 8) % 2 == 1)
						{
							step[i].second = point;
							count++;
							hash = hashCode(HashCM[i]);
							hashList[hash].cut = true;
							hashList[hash].full = true;
							hashList[hash].myType = 0;
							hashList[hash].denType = 0;
							if (hashList[hash].stepList.size() == 0)
							{
								hashList[hash].stepList.push_back(step[i]);
								hashList[hash].code = HashCM[i];
								hashList[hash].timestamp = handNum / 2;
							}
						}
					}
				}
			}
			fclose(F1);
		}
	}
	else if (color == WHITE)	//��ȡ�׷�����
	{
		for (int k = 0; k<1000; k++)
		{
			itoa(k, num, 10);
			strcpy(filename, "���׼�¼\\�׷�����\\"); strcat(filename, num); strcat(filename, ".sgf");
			if ((F1 = fopen(filename, "r")) == NULL)continue;
			for (y = 0; y<8; y++)
				initialCode(HashCM[y]);
			handNum = 0;
			while (1)
			{
				fscanf(F1, "%c,%d,%d\n", &str, &x, &y);
				if (str == 'V')break;
				else if (str == 'J' || x<0)continue;
				for (int i = 0; i<8; i++)
				{
					point.x = x;
					point.y = y;
					ChessImage(i, point.x, point.y);	//��Ҫ�����8���ԳƵ�
					if (str == 'B')
						moveCodeP(HashCM[i], point, BLACK);
					else if (str == 'W')
						moveCodeP(HashCM[i], point, WHITE);
					else if (str == 'P')	//��Ӧ���з�
					{
						if ((count / 8) % 2 == 0)
						{
							step[i].first = point;
							count++;
						}
						else if ((count / 8) % 2 == 1)
						{
							step[i].second = point;
							count++;
							hash = hashCode(HashCM[i]);
							hashList[hash].cut = true;
							hashList[hash].full = true;
							hashList[hash].myType = 0;
							hashList[hash].denType = 0;
							if (hashList[hash].stepList.size() == 0)
							{
								hashList[hash].stepList.push_back(step[i]);
								hashList[hash].code = HashCM[i];
								hashList[hash].timestamp = handNum / 2;
							}
						}
					}
				}
			}
			fclose(F1);
		}
	}
}

