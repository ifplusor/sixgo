#include "ChessManual.h"
#include "Seach.h"


//数据结构
const int lineVector[8][2]={{1,0},{1,1},{0,1},{1,-1},//正向向量
					{-1,0},{-1,-1},{0,-1},{-1,1}};//反向向量
int nBoard[edge][edge];//棋盘，第一坐标X，第二坐标Y
int curside=0;//当前行棋方
int HandNum;//行棋招法计数
int computerSide;//引擎执棋颜色
int start=0;

//棋局初始化
void initialGame()
{
	int i,j;
	for (i = 0; i<edge; i++)//清空棋盘
		for (j = 0; j<edge; j++)
			nBoard[i][j]=EMPTY;
	curside=BLACK;//设置先手行棋方
	HandNum=1;//设置第一手标志
	start=1;
	debugger.InitDir();
}

//显示棋盘状态
void showBoard()
{
	int i,j;

	printf("   A B C D E F G H I J K L M N O P Q R S\n");//输出列编号
	for (j = 0; j<edge; j++)
	{
		printf("%2c",j+'A');//输出行编号
		for (i = 0; i<edge; i++)
		{
			switch(nBoard[i][j])//按虚拟棋盘上对应的点状态进行显示
			{
			case BLACK://黑子
				printf("○");
				break;
			case WHITE://白子
				printf("●");
				break;
			case EMPTY://空点
				if(j==0)
				{
					if(i==0)
						printf("┏");
					else if (i == edge - 1)
						printf("┓");
					else
						printf("┯");
				}
				else if (j == edge - 1)
				{
					if(i==0)
						printf("┗");
					else if (i == edge - 1)
						printf("┛");
					else
						printf("┷");
				}
				else
				{
					if(i==0)
						printf("┠");
					else if (i == edge - 1)
						printf("┨");
					else
						printf("┼");
				}
				break;
			}
		}
		printf("\n");
	}
	fflush(stdout);
}

//落子
void moveStep(Step step)
{
	if (step.first.x >= 0 && step.first.x<edge&&step.first.y >= 0 && step.first.y<edge)
	{
		nBoard[step.first.x][step.first.y]=curside;
		//复制虚拟棋盘
		for (int i = 0; i<edge; i++)
			for (int j = 0; j<edge; j++)
				virtualBoard[i][j]=nBoard[i][j];
		UpdateLineForCross(step.first,BLACK,TOALL);
		UpdateLineForCross(step.first,WHITE,TOALL);
	}
	if (step.second.x >= 0 && step.second.x<edge&&step.second.y >= 0 && step.second.y<edge)
	{
		nBoard[step.second.x][step.second.y]=curside;
		//复制虚拟棋盘
		for (int i = 0; i<edge; i++)
			for (int j = 0; j<edge; j++)
				virtualBoard[i][j]=nBoard[i][j];
		UpdateLineForCross(step.second,BLACK,TOALL);
		UpdateLineForCross(step.second,WHITE,TOALL);
	}
	curside=1-curside;
	HandNum++;
}

//着法合法性判断
bool judgeLegal(Step step)
{
	if (step.first.x >= 0 && step.first.x<edge&&step.first.y >= 0 && step.first.y<edge)
	{
		if(nBoard[step.first.x][step.first.y]==EMPTY)
		{
			if (step.second.x >= 0 && step.second.x<edge&&step.second.y >= 0 && step.second.y<edge)
			{
				if(nBoard[step.second.x][step.second.y]==EMPTY)
					return true;
			}
			else
				return true;
		}
	}
	if (step.second.x >= 0 && step.second.x<edge&&step.second.y >= 0 && step.second.y<edge)
	{
		if(nBoard[step.second.x][step.second.y]==EMPTY)
			return true;
	}
	printf("非法棋步!\n");
	printf("Step: %c%c%c%c\n",step.first.x+'A',step.first.y+'A',step.second.x+'A',step.second.y+'A');
	fflush(stdout);
	return false;
}

//胜负判断
bool WinOrLose(Step step)
{
	int offset,a,b;
	int i,side;
	//第一子
	if(step.first.x!=-1&&step.first.y!=-1)
	{
		side=nBoard[step.first.x][step.first.y];
		for(i=0;i<4;i++)
		{
			offset=1;
			//按照向量进行移动
			a=step.first.x+lineVector[i][0];
			b=step.first.y+lineVector[i][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//本方棋子
				{
					offset++;//计数
					a+=lineVector[i][0];
					b+=lineVector[i][1];
				}
				else
					break;
			}
			//向量反向
			a=step.first.x+lineVector[FX(i)][0];
			b=step.first.y+lineVector[FX(i)][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//本方棋子
				{
					offset++;//计数
					a+=lineVector[FX(i)][0];
					b+=lineVector[FX(i)][1];
				}
				else
					break;
			}
			if(offset>=6)	//连六
			{
				return true;
			}
		}
	}
	if(step.second.x!=-1&&step.second.y!=-1)
	{
		side=nBoard[step.second.x][step.second.y];
		//第二子
		for(i=0;i<4;i++)
		{
			offset=1;
			//按照向量进行移动
			a=step.second.x+lineVector[i][0];
			b=step.second.y+lineVector[i][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//本方棋子
				{
					offset++;//计数
					a+=lineVector[i][0];
					b+=lineVector[i][1];
				}
				else
					break;
			}
			//向量反向
			a=step.second.x+lineVector[FX(i)][0];
			b=step.second.y+lineVector[FX(i)][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//本方棋子
				{
					offset++;//计数
					a+=lineVector[FX(i)][0];
					b+=lineVector[FX(i)][1];
				}
				else
					break;
			}
			if(offset>=6)	//连六
			{
				return true;
			}
		}
	}
	return false;
}

int main(int argc, char *argv[])
{
	Step step;
	char message[256]="";

	atexit(StopEndLib);
	printf("sixgo 2.0.1\n");
	printf("Copyright 2014 ifplusor\n");
	printf("initial...\n");
	initialLineTypeTable(preTable);
	initialLineInfoTable(linetypeInfo);
	StartEndLib();
	printf("successed!\n");
	while (1)
	{
		fflush(stdin);
		fflush(stdout);
		scanf("%s", message);
		if(strcmp(message,"move")==0)
		{
			scanf("%s", message);
			step.first.x=message[0]-'A';
			step.first.y=message[1]-'A';
			step.second.x=message[2]-'A';
			step.second.y=message[3]-'A';
			if(start==0)
				continue;
			if(judgeLegal(step))
			{
				moveStep(step);
				showBoard();
				if(WinOrLose(step))
				{
					start=0;
					continue;
				}
			}
			else
			{
				printf("error\n请重新输入!\n");
				continue;
			}
			if(HandNum==2)//第二手比较特殊，故使用人为规定，贴着先手进行落子
			{
				Point temp;
				int type=0;
				if(step.first.x!=-1&&step.first.y!=-1)
				{
					temp=step.first;
					if(temp.x>9)
						type=2;
					if(temp.y>9)
						type++;
					switch(type)
					{
					case 0:
						step.first.x=temp.x+1;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y+1;
						break;
					case 1:
						step.first.x=temp.x+1;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y-1;
						break;
					case 2:
						step.first.x=temp.x-1;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y+1;
						break;
					case 3:
						step.first.x=temp.x-1;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y-1;
						break;
					}
					if(judgeLegal(step))
					{
						moveStep(step);
						showBoard();
						if(WinOrLose(step))
							start=0;
					}
					else
						break;
				}
				else if(step.second.x!=-1&&step.second.y!=-1)
				{
					temp=step.second;
					if(temp.x>9)
						type=2;
					if(temp.y>9)
						type++;
					switch(type)
					{
					case 0:
						step.first.x=temp.x++;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y++;
						break;
					case 1:
						step.first.x=temp.x++;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y--;
						break;
					case 2:
						step.first.x=temp.x--;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y++;
						break;
					case 3:
						step.first.x=temp.x--;
						step.first.y=temp.y;
						step.second.x=temp.x;
						step.second.y=temp.y--;
						break;
					}
					if(judgeLegal(step))
					{
						moveStep(step);
						showBoard();
						if(WinOrLose(step))
							start=0;
					}
					else
						break;
				}
				else
				{
					step.first.x=step.first.y=9;
					step.second.x=step.second.y=10;
					if(judgeLegal(step))
					{
						moveStep(step);
						showBoard();
						if(WinOrLose(step))
							start=0;
					}
					else
						break;
				}
			}
			else
			{
				step=sixgo_carry(step,nBoard,computerSide);
				if(judgeLegal(step))
				{
					moveStep(step);
					showBoard();
					if(WinOrLose(step))
						start=0;
				}
			}
			printf("move %c%c%c%c\n",step.first.x+'A',step.first.y+'A',step.second.x+'A',step.second.y+'A');
		}
		else if(strcmp(message,"new")==0)
		{
			scanf("%s",message);
			if(strcmp(message,"black")==0)
				computerSide=BLACK;
			else
				computerSide=WHITE;
			initialGame();
			initialAllLine();//对92条线的信息进行初始化
			initialHash();//清空置换表
#ifdef CM
			ReadCM(computerSide);//读取定式
#endif
			showBoard();
			if(computerSide==curside)//程序先手
			{
				step.first.x=step.first.y=9;
				step.second.x=step.second.y=-1;
				moveStep(step);
				showBoard();
				printf("move JJ@@\n");
			}
		}
		else if (strcmp(message, "name?") == 0)
		{
			printf("name sixgo\n");
		}
		else if (strcmp(message, "end") == 0)
		{
			start = 0;
		}
		else if (strcmp(message, "quit") == 0)
		{
			printf("Quit!\n");
			break;
		}
	}
	return 0;
}