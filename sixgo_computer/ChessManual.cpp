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
	int count = 0, x, y;
	char str, filename[_MAX_PATH];
	FILE *fp;

	for (int k = 0; k < 1000; k++)
	{
		sprintf(filename, "棋谱记录\\%s方棋谱\\%d.sgf", color == BLACK ? "黑" : "白", k);
		if ((fp = fopen(filename, "r")) == NULL)continue;	//如果没找到棋谱就找下一个
		for (y = 0; y < 8; y++)
			initialHashCode(HashCM[y]);
		while (1)
		{
			fscanf(fp, "%c,%d,%d\n", &str, &x, &y);
			if (str == 'V')break;
			else if (str == 'J' || x < 0)continue;
			for (int i = 0; i < 8; i++)
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
					count++;
					if ((count / 8) % 2 == 0)
					{
						step[i].first = point;
					}
					else if ((count / 8) % 2 == 1)
					{
						step[i].second = point;
						HashInfo *hashT = findHash(HashCM[i]);
						if (hashT == NULL)
						{
							HashInfo hashP;
							hashP.code = HashCM[i];
							hashP.cut = true;
							hashP.full = true;
							hashP.value = 0;
							hashP.denType = 0;
							hashP.timestamp = 0;
							hashP.stepList.push_back(step[i]);
							updateHash(hashP);
						}
						else
						{
							hashT->stepList.push_back(step[i]);
						}
					}
				}
			}
		}
		fclose(fp);
	}
}

