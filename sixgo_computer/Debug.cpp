#include <Windows.h>
#include "Debug.h"


extern int virtualBoard[edge][edge];

void showBoard(FILE *fp, const int nBoard[edge][edge])
{
	fprintf(fp,"   A B C D E F G H I J K L M N O P Q R S\n");//输出列编号
	for (int j = 0; j<edge; j++)
	{
		fprintf(fp,"%2c",j+'A');//输出行编号
		for (int i = 0; i<edge; i++)
		{
			switch(nBoard[i][j])//按虚拟棋盘上对应的点状态进行显示
			{
			case BLACK://黑子
				fprintf(fp,"●");
				break;
			case WHITE://白子
				fprintf(fp,"○");
				break;
			case EMPTY://空点
				if(j==0)
				{
					if(i==0)
						fprintf(fp,"┏");
					else if (i == edge - 1)
						fprintf(fp,"┓");
					else
						fprintf(fp,"┯");
				}
				else if (j == edge - 1)
				{
					if(i==0)
						fprintf(fp,"┗");
					else if (i == edge - 1)
						fprintf(fp,"┛");
					else
						fprintf(fp,"┷");
				}
				else
				{
					if(i==0)
						fprintf(fp,"┠");
					else if (i == edge - 1)
						fprintf(fp,"┨");
					else
						fprintf(fp,"┼");
				}
				break;
			}
		}
		fprintf(fp,"\n");
	}
}

void OutputStep(vector<Step> &stepList,const int side,int tag)
{
	vector<Step>::iterator iterS;
	FILE *fp=NULL;
	char filename[_MAX_PATH];

	SYSTEMTIME sys;
	GetLocalTime( &sys );
	sprintf(filename,"H:\\尹伟和\\sixgo_computer2.0.1\\Release\\SIXGO_DEBUG\\Log_%d_%d_%d_%d_%d_%d_%d__%d.txt",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds,tag);
	
	fp=fopen(filename,"r");
	if(fp==NULL)
	{
		fp=fopen(filename,"wt");
		if(fp==NULL)
		{
			printf("create file failed!\n");
			return;
		}
	}
	else
	{
		return OutputStep(stepList,side,tag+1);
	}
	fprintf(fp,"side=%s\n",side==0?"黑方":"白方");
	showBoard(fp,virtualBoard);
	for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
	{
		fprintf(fp,"%c%c%c%c %d\n",iterS->first.x+'A',iterS->first.y+'A',iterS->second.x+'A',iterS->second.y+'A',iterS->value);
	}
	fclose(fp);
}

int CheckChess(const int nBoard[edge][edge], const int side)
{
	int whiteNum=0,blackNum=0;
	for(int i=0;i<edge;i++)
	{
		for(int j=0;j<edge;j++)
		{
			if(nBoard[i][j]==BLACK)
				blackNum++;
			else if(nBoard[i][j]==WHITE)
				whiteNum++;
		}
	}
	if( ((blackNum-whiteNum)!=1) && ((whiteNum-blackNum)!=1) )
	{
		FILE *fp;
		char filename[_MAX_PATH];

		SYSTEMTIME sys;
		GetLocalTime( &sys );
		sprintf(filename,"SIXGO_DEBUG\\Log_%d_%d_%d_%d_%d_%d_%d__error.txt",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
		fp=fopen(filename,"wt");
		if(fp!=NULL)
		{
			fprintf(fp,"side=%s\n",side==0?"黑方":"白方");
			showBoard(fp,nBoard);
			fclose(fp);
		}
		else
			printf("create file failed!\n");
		return 1;
	}
	return 0;
}

void OutputBoard(const int nBoard[edge][edge], const int side)
{
	FILE *fp;
	char filename[_MAX_PATH];
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	sprintf(filename,"SIXGO_DEBUG\\Log_%d_%d_%d_%d_%d_%d_%d__board.txt",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	fp=fopen(filename,"wt");
	if(fp!=NULL)
	{
		fprintf(fp,"side=%s\n",side==0?"黑方":"白方");
		showBoard(fp,nBoard);
		fclose(fp);
	}
	else
		printf("create file failed!\n");
}