#include "hash.h"

EndLibInfo *eLib[MAXSIZE];//残局表，要求无冲

HashInfo hashList[MAXSIZE];//置换表中只保存本轮和上一轮的信息，再向前的信息直接丢弃
int hashnum = 0;//哈希深度
int realnum = 0;//真实深度

unsigned long hashBoard32[23*32];
unsigned __int64 hashBoard64[23 * 32];

unsigned __int64 rand64()
{
	return (unsigned __int64)rand() ^ ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 30) ^ ((unsigned __int64)rand() << 45) ^ ((unsigned __int64)rand() << 60);
}
unsigned long rand32()
{
	return (unsigned __int64)rand() ^ ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 30);
}

/**
* initialHashBoard - 初始化哈希棋盘
*/
void initialHashBoard()
{
	int i, j;
	srand((unsigned)time(NULL));
	for (i = 0; i<23 * 32; i++)
	{
		hashBoard32[i] = rand32();
		hashBoard64[i] = rand64();
	}

loop:
	for (i = 0; i<23 * 32; i++)
		for (j = i + 1; j<23 * 32; j++)
		{
			if (*(hashBoard32 + i) == *(hashBoard32 + j))
			{
				printf("hashBoard32 hane same code!\n");
				(*(hashBoard32 + j)) = rand32();
				goto loop;
			}
			if (*(hashBoard64 + i) == *(hashBoard64 + j))
			{
				printf("hashBoard64 hane same code!\n");
				(*(hashBoard64 + j)) = rand64();
				goto loop;
			}
		}
}

void initialCode(BoardCode &code)
{
	for (int i = 0; i<23; i++)
		code.a[i] = 0;
	code.hash32 = 0;
}

void initialHash()
{
	for (int i = 0; i<MAXSIZE; i++)
	{
		initialCode(hashList[i].code);
		hashList[i].index = -2;
	}
	initialHashBoard();
	hashnum = realnum = 0;
}

bool compareCode(BoardCode &a, BoardCode &b)
{
	if (a.hash32 != b.hash32)
		return false;
	for (int i = 0; i<23; i++)
		if (a.a[i] ^ b.a[i])
			return false;
	return true;
}

inline unsigned long hashCode(BoardCode &code)
{
	return code.hash32 & 0xFFFFF;
}

/**
* findHash - 查找置换
* @code:	棋盘状态编码
* @index:	时间戳
*/
HashInfo *findHash(BoardCode &code, int index)
{
	unsigned long hash = hashCode(code);
	if (compareCode(hashList[hash].code, code))
		return hashList + hash;
	else
		return NULL;
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


/**
 * HashImag - 棋子映射
 * @i:	映射标记
 * @x:	棋子x坐标
 * @y:	棋子y坐标
 */
void HashImage(int i,int &x,int &y)
{
	int t;
	if(i==0){x=x;y=y;} //原处不变
	else if(i==1){x=18-x;y=y;}  //关于x=9对称
	else if(i==2){x=x;y=18-y;}  //关于y=9对称
	else if(i==3){x=18-x;y=18-y;}  //关于(9,9)中心对称
	else if(i==4){t=x;x=y;y=t;}  //关于y=19-x对称
	else if(i==5){t=x;x=y;y=18-t;}  //先关于x=9对称，再关于19-x对称
	else if(i==6){t=x;x=18-y;y=t;}  //先关于y=9对称，再关于19-x对称
	else if(i==7){t=x;x=18-y;y=18-t;}  //先关于(9,9),中心对称，再关于y=19-x对称
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
				else if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					point.x=x;
					point.y=y;
					HashImage(i,point.x,point.y);	//需要镜像成8个对称的
					if(str=='B')
						moveCodeP(HashCM[i],point,BLACK);
					else if(str=='W')
						moveCodeP(HashCM[i],point,WHITE);
					else if(str=='P')	//对应的招法
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
	else if(color==WHITE)	//读取白方棋谱
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
				else if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					point.x=x;
					point.y=y;
					HashImage(i,point.x,point.y);	//需要镜像成8个对称的
					if(str=='B')
						moveCodeP(HashCM[i],point,BLACK);
					else if(str=='W')
						moveCodeP(HashCM[i],point,WHITE);
					else if(str=='P')	//对应的招法
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


//残局库
//===================================================================

void initialEndLib()
{
	for (int i = 0; i<MAXSIZE; i++)
	{
		eLib[i] = NULL;
	}
}

void addEndLib(BoardCode &code,int side)
{
	unsigned long hash = hashCode(code);
	EndLibInfo *p = (EndLibInfo *)malloc(sizeof(EndLibInfo));
	p->code = code;
	p->side = side;
	p->next = eLib[hash];
	eLib[hash] = p;
}

EndLibInfo *findEndLib(BoardCode &code)
{
	unsigned long hash = hashCode(code);
	EndLibInfo *p = eLib[hash];
	while (p != NULL)
	{
		if (compareCode(p->code, code))
			return p;
		p = p->next;
	}
	return NULL;
}

bool ReadEL()
{
	BoardCode code;
	FILE *rfp;
	int index, pos, side;
	unsigned long t;

	rfp = fopen("Sixgo.ELib", "rt");
	if (rfp == NULL)
	{
		printf("open end lib error!\n");
		return true;
	}
	while (fscanf(rfp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", code.a, code.a + 1, code.a + 2, code.a + 3, code.a + 4, code.a + 5, code.a + 6,
		code.a + 7, code.a + 8, code.a + 9, code.a + 10, code.a + 11, code.a + 12, code.a + 13, code.a + 14, code.a + 15, code.a + 16, code.a + 17, code.a + 18, code.a + 19,
		code.a + 20, code.a + 21, code.a + 22, &side) != EOF)
	{
		code.hash32 = 0;
		for (index = 0; index < 23; index++)
		{
			t = code.a[index];
			pos = 0;
			while (t > 0)
			{
				if (t & 1)
				{
					code.hash32 ^= hashBoard32[index * 32 + pos];
				}
				pos++;
				t >>= 1;
			}
		}
		addEndLib(code,side);
	}
	fclose(rfp);
	return false;
}

FILE *ELFP=NULL;

void InsertEndLib(BoardCode &code, int side)
{
	addEndLib(code, side);
	fprintf(ELFP, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", code.a[0], code.a[1], code.a[2], code.a[3], code.a[4], code.a[5], code.a[6],
		code.a[7], code.a[8], code.a[9], code.a[10], code.a[11], code.a[12], code.a[13], code.a[14], code.a[15], code.a[16], code.a[17], code.a[18], code.a[19],
		code.a[20], code.a[21], code.a[22], side);
}

bool StartEndLib()
{
	initialEndLib();
	if (ReadEL())
		ELFP = fopen("Sixgo.Elib", "wt");
	else
		ELFP = fopen("Sixgo.ELib", "at");
	if (ELFP == NULL)
	{
		printf("Open end Lib error!\n");
		return true;
	}
	return false;
}

void StopEndLib()
{
	if (ELFP == NULL)
		return;
	fclose(ELFP);
	ELFP = NULL;
	printf("close end Lib successed!\n");
}