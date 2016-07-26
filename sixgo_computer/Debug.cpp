#include "Debug.h"


extern int virtualBoard[edge][edge];

Debugger debugger = Debugger();

void Debugger::showBoard(FILE *fp, const int nBoard[edge][edge])
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

void Debugger::OutputStep(vector<Step> &stepList, const int side)
{
	vector<Step>::iterator iterS;
	string filename = getPath() + "\\step.sif";
	FILE * fp = fopen(filename.c_str(), "wt");

	if(fp == NULL)
	{
		fp = fopen(filename.c_str(), "wt");
		if (fp == NULL)
		{
			printf("error: create file failed!\n");
			return;
		}
	}
	fprintf(fp, "side=%s\n", side == 0 ? "黑方" : "白方");
	showBoard(fp, virtualBoard);
	fprintf(fp, "list size: %ld\n", stepList.size());
	for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
	{
		fprintf(fp, "%c%c%c%c %d\n", iterS->first.x + 'A', iterS->first.y + 'A', iterS->second.x + 'A', iterS->second.y + 'A', iterS->value);
	}
	fclose(fp);
}


void Debugger::OutputMessage(char *message, const int side)
{
	string filename = getPath() + "\\message.sif";
	FILE * fp = fopen(filename.c_str(), "wt");
	if(fp!=NULL)
	{
		//fprintf(fp,"side=%s\n",side==0?"黑方":"白方");
		//showBoard(fp,virtualBoard);
		fprintf(fp, "message: %s\n", message);
		fclose(fp);
	}
	else
		printf("error: create file failed!\n");
}

string Debugger::getPath()
{
	char temp[10];
	sprintf(temp, "\\%d", timestamp);
	string path(pathRoot + temp);
	for (int i = 0; i <= stepStack.top; ++i)
	{
		path = path + (i==0?'_':'\\') + (char)(stepStack.stepStack[i].first.x + 'A') + (char)(stepStack.stepStack[i].first.y + 'A')
			+ (char)(stepStack.stepStack[i].second.x + 'A') + (char)(stepStack.stepStack[i].second.y + 'A');
	}
	return path;
}

void Debugger::MakeMove(Step step)
{
	if (stepStack.push(step))
	{
		printf("error: stack overflow\n");
		return;
	}
	string path = getPath();

#ifdef __WIN32__
	if(_mkdir(path.c_str()))
#endif // WIN32

#ifdef __gnu_linux__
    if(mkdir(path.c_str(), 644))
#endif // linux
		printf("error: make folder failed!\n");
}

void Debugger::BackMove(Step step)
{
#ifdef OUTPUTVALUE
	string filename = getPath() + "\\value.sif";
	FILE *fp = fopen(filename.c_str(), "wt");
	if (fp == NULL)
	{
		printf("error: create file failed!\n");
	}
	else
	{
		fprintf(fp, "value: %d\n", step.value);
		fclose(fp);
	}
#endif
	stepStack.pop();
}

void Debugger::InitDir()
{
	timeDir = time(NULL);
	char temp[_MAX_PATH];

#ifdef __WIN32__
	sprintf(temp, "SIXGO_DEBUG\\%I64d", timeDir);
	pathRoot = temp;
	if(_mkdir(temp))
#endif // WIN32

#ifdef __gnu_linux__
	sprintf(temp, "SIXGO_DEBUG\\%ld", timeDir);
	pathRoot = temp;
    if(mkdir(temp, 644))
#endif // linux
		printf("error: make folder failed!\n");
}

void Debugger::InitDebugger(int timestamp, Step step)
{
	this->timestamp = timestamp;
	MakeMove(step);
}

void Debugger::OutSelect(Step step)
{
	string filename = getPath() + "\\select.sif";
	FILE * fp = fopen(filename.c_str(), "wt");
	if (fp != NULL)
	{
		showBoard(fp, virtualBoard);
		fprintf(fp, "select: %c%c%c%c\nvalue: %d\n", step.first.x + 'A', step.first.y + 'A',
			step.second.x + 'A', step.second.y + 'A', step.value);
		fclose(fp);
	}
	else
		printf("error: create file failed!\n");
}
