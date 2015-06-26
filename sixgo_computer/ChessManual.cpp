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
	vector<Step>::iterator iter;
	int count = 0, i, l;
	char str[10], pos[10], filename[_MAX_PATH];
	FILE *fp;

	for (int k = 0; k < 1000; k++)
	{
		sprintf(filename, "CM\\%s\\%d.sgf", color == BLACK ? "Black" : "White", k);
		if ((fp = fopen(filename, "r")) == NULL)continue;	//���û�ҵ����׾�����һ��
		for (l = 0; l < 8; l++)
			initialHashCode(HashCM[l]);
		for (i = 0; fscanf(fp, "%s %s", str, pos) != EOF; i++)
		{
			//��Ҫ�����8���ԳƵ�
			for (int l = 0; l < 8; l++)
			{
				step[l].first.x = pos[0] - 'A';
				step[l].first.y = pos[1] - 'A';
				step[l].second.x = pos[2] - 'A';
				step[l].second.y = pos[3] - 'A';
				ChessImage(l, step[l].first.x, step[l].first.y);
				if (i != 0)
				{
					ChessImage(l, step[l].second.x, step[l].second.y);
					if (color == BLACK ? str[0] == 'B' : str[0] == 'W')//ָ��ִ����ɫ
					{
						HashInfo *hashT = findHash(HashCM[l]);
						if (hashT == NULL)
						{
							HashInfo hashP;
							hashP.code = HashCM[l];
							hashP.cut = true;
							hashP.full = true;
							hashP.value = 0;
							hashP.denType = 0;
							hashP.timestamp = 0;
							hashP.stepList.push_back(step[l]);
							updateHash(hashP);
						}
						else
						{
							hashT->stepList.push_back(step[l]);
							UniqueStep(hashT->stepList);
						}
					}
					if (str[0] == 'B')
						moveCodeS(HashCM[l], step[l], BLACK);
					else if (str[0] == 'W')
						moveCodeS(HashCM[l], step[l], WHITE);
				}
				else
					moveCodeP(HashCM[l], step[l].first, str[0] == 'B' ? BLACK : WHITE);
			}
		}
		fclose(fp);
	}
}

