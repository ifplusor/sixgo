#include "ChessManual.h"


/**
* ChessImage - 棋子映射
* @i:	映射标记
* @x:	棋子x坐标
* @y:	棋子y坐标
*/
void ChessImage(int i, int &x, int &y)
{
	int t;
	if (i == 0){ x = x; y = y; } //原处不变
	else if (i == 1){ x = 18 - x; y = y; }  //关于x=9对称
	else if (i == 2){ x = x; y = 18 - y; }  //关于y=9对称
	else if (i == 3){ x = 18 - x; y = 18 - y; }  //关于(9,9)中心对称
	else if (i == 4){ t = x; x = y; y = t; }  //关于y=19-x对称
	else if (i == 5){ t = x; x = y; y = 18 - t; }  //先关于x=9对称，再关于19-x对称
	else if (i == 6){ t = x; x = 18 - y; y = t; }  //先关于y=9对称，再关于19-x对称
	else if (i == 7){ t = x; x = 18 - y; y = 18 - t; }  //先关于(9,9),中心对称，再关于y=19-x对称
}

/**
* ReadCM - 读取开局库
* @color:	执棋颜色
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
	if (color == BLACK)	//读取黑方的棋谱
	{
		for (int k = 0; k<1000; k++)
		{
			itoa(k + 1, num, 10);
			strcpy(filename, "棋谱记录\\黑方棋谱\\"); strcat(filename, num); strcat(filename, ".sgf");
			if ((F1 = fopen(filename, "r")) == NULL)continue;	//如果没找到棋谱就找下一个
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
					ChessImage(i, point.x, point.y);	//需要镜像成8个对称的
					if (str == 'B')
						moveCodeP(HashCM[i], point, BLACK);
					else if (str == 'W')
						moveCodeP(HashCM[i], point, WHITE);
					else if (str == 'P')	//对应的招法
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
	else if (color == WHITE)	//读取白方棋谱
	{
		for (int k = 0; k<1000; k++)
		{
			itoa(k, num, 10);
			strcpy(filename, "棋谱记录\\白方棋谱\\"); strcat(filename, num); strcat(filename, ".sgf");
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
					ChessImage(i, point.x, point.y);	//需要镜像成8个对称的
					if (str == 'B')
						moveCodeP(HashCM[i], point, BLACK);
					else if (str == 'W')
						moveCodeP(HashCM[i], point, WHITE);
					else if (str == 'P')	//对应的招法
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

