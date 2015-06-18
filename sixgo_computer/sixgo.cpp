#include "ChessManual.h"
#include "Seach.h"


//���ݽṹ
const int lineVector[8][2]={{1,0},{1,1},{0,1},{1,-1},//��������
					{-1,0},{-1,-1},{0,-1},{-1,1}};//��������
int nBoard[edge][edge];//���̣���һ����X���ڶ�����Y
int curside=0;//��ǰ���巽
int HandNum;//�����з�����
int computerSide;//����ִ����ɫ
int start=0;

//��ֳ�ʼ��
void initialGame()
{
	int i,j;
	for (i = 0; i<edge; i++)//�������
		for (j = 0; j<edge; j++)
			nBoard[i][j]=EMPTY;
	curside=BLACK;//�����������巽
	HandNum=1;//���õ�һ�ֱ�־
	start=1;
	debugger.InitDir();
}

//��ʾ����״̬
void showBoard()
{
	int i,j;

	printf("   A B C D E F G H I J K L M N O P Q R S\n");//����б��
	for (j = 0; j<edge; j++)
	{
		printf("%2c",j+'A');//����б��
		for (i = 0; i<edge; i++)
		{
			switch(nBoard[i][j])//�����������϶�Ӧ�ĵ�״̬������ʾ
			{
			case BLACK://����
				printf("��");
				break;
			case WHITE://����
				printf("��");
				break;
			case EMPTY://�յ�
				if(j==0)
				{
					if(i==0)
						printf("��");
					else if (i == edge - 1)
						printf("��");
					else
						printf("��");
				}
				else if (j == edge - 1)
				{
					if(i==0)
						printf("��");
					else if (i == edge - 1)
						printf("��");
					else
						printf("��");
				}
				else
				{
					if(i==0)
						printf("��");
					else if (i == edge - 1)
						printf("��");
					else
						printf("��");
				}
				break;
			}
		}
		printf("\n");
	}
	fflush(stdout);
}

//����
void moveStep(Step step)
{
	if (step.first.x >= 0 && step.first.x<edge&&step.first.y >= 0 && step.first.y<edge)
	{
		nBoard[step.first.x][step.first.y]=curside;
		//������������
		for (int i = 0; i<edge; i++)
			for (int j = 0; j<edge; j++)
				virtualBoard[i][j]=nBoard[i][j];
		UpdateLineForCross(step.first,BLACK,TOALL);
		UpdateLineForCross(step.first,WHITE,TOALL);
	}
	if (step.second.x >= 0 && step.second.x<edge&&step.second.y >= 0 && step.second.y<edge)
	{
		nBoard[step.second.x][step.second.y]=curside;
		//������������
		for (int i = 0; i<edge; i++)
			for (int j = 0; j<edge; j++)
				virtualBoard[i][j]=nBoard[i][j];
		UpdateLineForCross(step.second,BLACK,TOALL);
		UpdateLineForCross(step.second,WHITE,TOALL);
	}
	curside=1-curside;
	HandNum++;
}

//�ŷ��Ϸ����ж�
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
	printf("�Ƿ��岽!\n");
	printf("Step: %c%c%c%c\n",step.first.x+'A',step.first.y+'A',step.second.x+'A',step.second.y+'A');
	fflush(stdout);
	return false;
}

//ʤ���ж�
bool WinOrLose(Step step)
{
	int offset,a,b;
	int i,side;
	//��һ��
	if(step.first.x!=-1&&step.first.y!=-1)
	{
		side=nBoard[step.first.x][step.first.y];
		for(i=0;i<4;i++)
		{
			offset=1;
			//�������������ƶ�
			a=step.first.x+lineVector[i][0];
			b=step.first.y+lineVector[i][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//��������
				{
					offset++;//����
					a+=lineVector[i][0];
					b+=lineVector[i][1];
				}
				else
					break;
			}
			//��������
			a=step.first.x+lineVector[FX(i)][0];
			b=step.first.y+lineVector[FX(i)][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//��������
				{
					offset++;//����
					a+=lineVector[FX(i)][0];
					b+=lineVector[FX(i)][1];
				}
				else
					break;
			}
			if(offset>=6)	//����
			{
				return true;
			}
		}
	}
	if(step.second.x!=-1&&step.second.y!=-1)
	{
		side=nBoard[step.second.x][step.second.y];
		//�ڶ���
		for(i=0;i<4;i++)
		{
			offset=1;
			//�������������ƶ�
			a=step.second.x+lineVector[i][0];
			b=step.second.y+lineVector[i][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//��������
				{
					offset++;//����
					a+=lineVector[i][0];
					b+=lineVector[i][1];
				}
				else
					break;
			}
			//��������
			a=step.second.x+lineVector[FX(i)][0];
			b=step.second.y+lineVector[FX(i)][1];
			while(a>-1&&a<19&&b>-1&&b<19)
			{
				if(nBoard[a][b]==side)//��������
				{
					offset++;//����
					a+=lineVector[FX(i)][0];
					b+=lineVector[FX(i)][1];
				}
				else
					break;
			}
			if(offset>=6)	//����
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
				printf("error\n����������!\n");
				continue;
			}
			if(HandNum==2)//�ڶ��ֱȽ����⣬��ʹ����Ϊ�涨���������ֽ�������
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
			initialAllLine();//��92���ߵ���Ϣ���г�ʼ��
			initialHash();//����û���
#ifdef CM
			ReadCM(computerSide);//��ȡ��ʽ
#endif
			showBoard();
			if(computerSide==curside)//��������
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