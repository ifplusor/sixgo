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
		hashList[i].timestamp = -2;
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
HashInfo *findHash(BoardCode &code)
{
	unsigned long hash = hashCode(code);
	if (compareCode(hashList[hash].code, code))
		return hashList + hash;
	else
		return NULL;
}

unsigned long getHashCode(int index)
{
	return hashBoard32[index];
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


//残局库
//===================================================================

void initialEndLib()
{
	for (int i = 0; i<MAXSIZE; i++)
	{
		eLib[i] = NULL;
	}
}

void addEndLib(BoardCode &code)
{
	unsigned long hash = hashCode(code);
	EndLibInfo *p = (EndLibInfo *)malloc(sizeof(EndLibInfo));
	if (p == NULL)
		return;
	p->code = code;
	p->next = eLib[hash];
	eLib[hash] = p;
}

bool findEndLib(BoardCode &code)
{
	unsigned long hash = hashCode(code);
	EndLibInfo *p = eLib[hash];
	while (p != NULL)
	{
		if (compareCode(p->code, code))
			return true;
		p = p->next;
	}
	return false;
}

bool ReadEL()
{
	BoardCode code;
	FILE *rfp;
	int index, pos, count = 0;
	unsigned long t;

	rfp = fopen("Sixgo.ELib", "rb");
	if (rfp == NULL)
	{
		printf("open end lib error!\n");
		return true;
	}
	while (fread(code.a, sizeof(code.a), 1, rfp) != 0)
	{
		count++;
		code.hash32 = 0;
		for (index = 0; index < 23; index++)
		{
			t = code.a[index];
			pos = 0;
			while (t > 0)
			{
				if (t & 1)
				{
					code.hash32 ^= getHashCode(index * 32 + pos);
				}
				pos++;
				t >>= 1;
			}
		}
		addEndLib(code);
	}
	fclose(rfp);
	printf("read end node number: %d\n", count);
	return false;
}

FILE *ELFP = NULL;

void InsertEndLib(BoardCode &code)
{
	addEndLib(code);
	fwrite(code.a, sizeof(code.a), 1, ELFP);
}

bool StartEndLib()
{
	initialEndLib();
	if (ReadEL())
		ELFP = fopen("Sixgo.Elib", "wb");
	else
		ELFP = fopen("Sixgo.ELib", "ab");
	if (ELFP == NULL)
	{
		printf("Open end Lib error!\n");
		return true;
	}
	return false;
}

void StopEndLib()
{
	EndLibInfo *p;
	if (ELFP == NULL)
		return;
	fclose(ELFP);
	ELFP = NULL;
	for (int i = 0; i<MAXSIZE; i++)
	{
		while (eLib[i] != NULL)
		{
			p = eLib[i];
			eLib[i] = eLib[i]->next;
			free(p);
		}
	}
	printf("close end Lib successed!\n");
}
