#include "hash.h"

HashInfo hashList[MAXSIZE];//哈希表中只保存本轮和上一轮的信息，再向前的信息直接丢弃
int hashnum=0,realnum=0;

long hashBoard32[23*32];
__int64 hashBoard64[23*32];

__int64 rand64()
{
	return (__int64)rand()^((__int64)rand()<<15)^((__int64)rand()<<30)^((__int64)rand()<<45)^((__int64)rand()<<60);
}
long rand32()
{
	return (__int64)rand()^((__int64)rand()<<15)^((__int64)rand()<<30);
}

void initialCode(BoardCode &code)
{
	for(int i=0;i<23;i++)
		code.a[i]=0;
	code.hash32=0;
}

void moveCodeP(BoardCode &code,Point point,int side)
{
	int index,pos;
	int temp;
	temp = point.x + point.y*edge;
	if(side==WHITE)
		temp+=361;
	index=temp/32;
	pos=temp%32;
	code.a[index]=PutBBit(code.a[index],pos);
	code.hash32^=hashBoard32[temp];
}
void moveCodeS(BoardCode &code,Step step,int side)
{
	moveCodeP(code,step.first,side);
	moveCodeP(code,step.second,side);
}

bool compareCode(BoardCode &a,BoardCode &b)
{
	for(int i=0;i<23;i++)
		if(a.a[i]!=b.a[i])
			return false;
	return true;
}

inline long hashCode(BoardCode code)
{
	return code.hash32&1048575;
}

void initialHashBoard()
{
	int i,j;
	srand((unsigned)time(NULL));
	for(i=0;i<23*32;i++)
	{
		hashBoard32[i]=rand32();
		hashBoard64[i]=rand64();
	}

loop:
	for(i=0;i<23*32;i++)
		for(j=i+1;j<23*32;j++)
		{
			if(*(hashBoard32+i)==*(hashBoard32+j))
			{
				printf("hashBoard32 hane same code!\n");
				(*(hashBoard32+j))=rand32();
				goto loop;
			}
			if(*(hashBoard64+i)==*(hashBoard64+j))
			{
				printf("hashBoard64 hane same code!\n");
				(*(hashBoard64+j))=rand64();
				goto loop;
			}
		}
}

void initialHash()
{
	for(int i=0;i<MAXSIZE;i++)
	{
		for(int j=0;j<23;j++)
			hashList[i].code.a[j]=0;
		hashList[i].code.hash32=0;
		hashList[i].index=-2;
	}
	initialHashBoard();
	hashnum=realnum=0;
}


HashInfo *findHash(BoardCode &code,int index)
{
	long hash=hashCode(code);
	if(compareCode(hashList[hash].code,code))
		return &hashList[hash];
	else
		return NULL;
}



void HashImage(int i,int &x,int &y)
{
	int t;
	if(i==0){x=x;y=y;} //原处不变
	if(i==1){x=18-x;y=y;}  //关于x=9对称
	if(i==2){x=x;y=18-y;}  //关于y=9对称
	if(i==3){x=18-x;y=18-y;}  //关于(9,9)中心对称
	if(i==4){t=x;x=y;y=t;}  //关于y=19-x对称
	if(i==5){t=x;x=y;y=18-t;}  //先关于x=9对称，再关于19-x对称
	if(i==6){t=x;x=18-y;y=t;}  //先关于y=9对称，再关于19-x对称
	if(i==7){t=x;x=18-y;y=18-t;}  //先关于(9,9),中心对称，再关于y=19-x对称
}
void ReadCM(int color)
{
	BoardCode HashCM[8];
	Step step[8];
	Point point;
	long hash;
	int count=0,handNum;
	char str;
	char num[8];
	char filename[_MAX_PATH];
	int x,y;
	FILE *F1;
	if(color==BLACK)	//读取黑方的棋谱
	{
		for(int k=0;k<1000;k++)
		{
			itoa(k+1,num,10);
			strcpy(filename,"棋谱记录\\黑方棋谱\\");strcat(filename,num);strcat(filename,".sgf");
			if((F1=fopen(filename,"r"))==NULL)continue;	//如果没找到棋谱就找下一个
			for(y=0;y<8;y++)
				initialCode(HashCM[y]);
			handNum=0;
			while(1)
			{		
				handNum++;
				fscanf(F1,"%c,%d,%d\n",&str,&x,&y);
				if(str=='V')break;
				if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					point.x=x;
					point.y=y;
					HashImage(i,point.x,point.y);	//需要镜像成8个对称的
					if(str=='B')
						moveCodeP(HashCM[i],point,BLACK);
					if(str=='W')
						moveCodeP(HashCM[i],point,WHITE);
					if(str=='P')	//对应的招法
					{
						if((count/8)%2==0)
						{
							step[i].first=point;
							count++;
						}
						else if((count/8)%2==1)
						{
							step[i].second=point;
							count++;
							hash=hashCode(HashCM[i]);
							hashList[hash].cut=true;
							hashList[hash].full=true;
							hashList[hash].myType=0;
							hashList[hash].denType=0;
							if(hashList[hash].stepList.size()==0)
							{
								hashList[hash].stepList.push_back(step[i]);
								hashList[hash].code=HashCM[i];
								hashList[hash].index=handNum/2;
							}
						}
					}
				}
			}
			fclose(F1);
		}
	}
	if(color==WHITE)	//读取白方棋谱
	{
		for(int k=0;k<1000;k++)
		{
			itoa(k,num,10);
			strcpy(filename,"棋谱记录\\白方棋谱\\");strcat(filename,num);strcat(filename,".sgf");
			if((F1=fopen(filename,"r"))==NULL)continue;
			for(y=0;y<8;y++)
				initialCode(HashCM[y]);
			handNum=0;
			while(1)
			{		
				fscanf(F1,"%c,%d,%d\n",&str,&x,&y);
				if(str=='V')break;
				if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					point.x=x;
					point.y=y;
					HashImage(i,point.x,point.y);	//需要镜像成8个对称的
					if(str=='B')
						moveCodeP(HashCM[i],point,BLACK);
					if(str=='W')
						moveCodeP(HashCM[i],point,WHITE);
					if(str=='P')	//对应的招法
					{
						if((count/8)%2==0)
						{
							step[i].first=point;
							count++;
						}
						else if((count/8)%2==1)
						{
							step[i].second=point;
							count++;
							hash=hashCode(HashCM[i]);
							hashList[hash].cut=true;
							hashList[hash].full=true;
							hashList[hash].myType=0;
							hashList[hash].denType=0;
							if(hashList[hash].stepList.size()==0)
							{
								hashList[hash].stepList.push_back(step[i]);
								hashList[hash].code=HashCM[i];
								hashList[hash].index=handNum/2;
							}
						}
					}
				}		
			}
			fclose(F1);
		}
	}
}
