#include "linetype.h"


BYTE preTable[TableSize];					//���ͱ���
LineTypeInfo linetypeInfo[TableSize];

int readOld(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if((dataFile=fopen("_ConnPreTbl.me","rb"))!=NULL)
	{
		numRead=fread(lpArray,sizeof(BYTE),TabSize,dataFile);
		fclose(dataFile);
		printf("Successed in Opening the file preTable !\n");
	}
	else
	{
		printf("Failed in Opening the file preTable !\n");
		return 1;
	}
	return 0;
}
int readNew(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if((dataFile=fopen("_LineTypeTable.me","rb"))!=NULL)
	{
		numRead=fread(lpArray,sizeof(BYTE),TableSize,dataFile);
		fclose(dataFile);
		printf("Successed in Opening the file preTable !\n");
	}
	else
	{
		printf("Failed in Opening the file preTable !\n");
		return 1;
	}
	return 0;
}
int write(LPBYTE lpArray)
{
	FILE *dataFile;
	int numRead;
	if((dataFile=fopen("_LineTypeTable.me","wb"))!=NULL)
	{
#ifdef NEW
		numRead=fwrite(lpArray,sizeof(BYTE),TableSize,dataFile);
#else
		numRead=fwrite(lpArray,sizeof(BYTE),TabSize,dataFile);
#endif
		fclose(dataFile);
		printf("Successed in writing the file preTable !\n");
	}
	else
	{
		printf("Failed in writing the file preTable !\n");
		return 1;
	}
	return 0;
}
void out(LPBYTE lpArray,BYTE tag)
{
	FILE * fp;
	int j,sum;
	char temp[5],filename[20];//ע�⣺�����ɵ����ζ����Ƶı�ʾ�У������ֵ�1��ʾ�з�һ�����ӻ�߽硣
	int shapeIndex;
	itoa(tag,temp,10);
	strcpy(filename,"LineType\\");
	strcat(filename,temp);
	strcat(filename,".txt");
	fp=fopen(filename,"wt");
	if(fp==NULL)
	{
		printf("out fail !\n");
		return;
	}
	sum=0;
	for(j=0;j<TableSize;j++)
	{
		shapeIndex=j;
		if(lpArray[j]==tag)
		{
			sum++;
			fprintf(fp,"%8d:",shapeIndex);
			while(shapeIndex>1)	//Ŀǰ�����������̣��κη����ߵĵ���������20����19������
			{
				if(shapeIndex&1)
					fprintf(fp,"��");
				else
					fprintf(fp,"��");
				shapeIndex=shapeIndex>>1;
			}
			fprintf(fp,"\n");
		}
	}
	fprintf(fp,"\n\ntotal:%d\n",sum);
	fclose(fp);
	printf("Successed out!\n");
}

int compare(LPBYTE lpA,LPBYTE lpB,int num)
{
	int i,ta=1;
	printf("Matching %d...\n",num);
	for(i=64;i<TableSize;i++)
		if(lpB[i]==num)
			if(lpA[i-64]!=num)
			{
				ta=0;
				printf("B-A Not match %d !  %d\n",i,num);
				getchar();
			}
	for(i=0;i<TabSize;i++)
		if(lpA[i]==num)
			if(lpB[i+64]!=num)
			{
				ta=0;
				printf("A-B Not match %d !  %d\n",i+64,num);
				getchar();
			}
	if(ta)
	{
		printf("Successed match %d!\n",num);
		return 0;
	}
	return -1;
}

//��������
//===============================================================================
void GetMode(LPBYTE lpArray)//����ߵ����ͣ������ڷּ�
{
	int len,i,shapeIndex,sum,sum_max;
	for(shapeIndex=0;shapeIndex<64;shapeIndex++)
		lpArray[shapeIndex]=VALUELESS;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		sum_max=0;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		sum=0;
		for(i=0;i<6;i++)
			if(GetABit(shapeIndex,i))sum++;
		if(sum>5)//����Ϊ6��
		{
			lpArray[shapeIndex]=6;
			continue;
		}
		if(sum>sum_max)//�������������к��еı������������ֵȷ���ߵġ��͡�
			sum_max=sum;
		for(i=0;i<len-6;i++)
		{
			sum-=GetABit(shapeIndex,i);
			sum+=GetABit(shapeIndex,i+6);
			if(sum>5)
			{
				sum_max=6;
				break;
			}
			if(sum>sum_max)//�������������к��еı������������ֵȷ���ߵġ��͡�
				sum_max=sum;
		}
		lpArray[shapeIndex]=sum_max;
	}
	printf("Finish get line Mode.\n");
}
 
void createWIN(LPBYTE lpArray)//������ʤ����
{
	int shapeIndex;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]==6)//6�ͼ�Ϊ��ʤ����
			lpArray[shapeIndex]=WIN;
	}
	printf("Finish create WIN .\n");
}

void createTHREAT_THIRD(LPBYTE lpArray)//���ɶ���в���ͣ�ͨ�������Ӳ��߽����жϣ�
{
	int i,j,m,n,shapeIndex,len,sum;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=5&&lpArray[shapeIndex]!=4)//����в����ֻ�ֲ���5�ͺ�6����
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		sum=0;
		for(i=0;i<len;i++)
			if(GetABit(shapeIndex,i))sum++;
		if(sum+3>len)//�����������յ㣬���������������յ��ϼ����ƽ�
			continue;
		for(i=0;i<len-1;i++)//һ�����������Ϊ��������Ч����(û��������Ч���͵Ŀ���)
		{
			if(GetABit(shapeIndex,i)!=0)
				continue;
			for(j=i+1;j<len;j++)
			{
				if(GetABit(shapeIndex,j)!=0)
					continue;
				if(i<6&&j-i<7&&len-j>6)//���λ������Ч
				{
					n=1;
					sum=0;
					for(m=j;m<j+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=j;m<len-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else if(i<6&&j-i>6&&len-j<7)//�м�������Ч
				{
					n=1;
					sum=0;
					for(m=i;m<i+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=i;m<j-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else if(i>5&&j-i<7&&len-j<7)//���λ������Ч
				{
					n=1;
					sum=0;
					for(m=0;m<6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=0;m<i-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else if(i<6&&j-i>6&&len-j>6)//�ߡ�����Ч
				{
					n=1;
					sum=0;
					for(m=j;m<j+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=j;m<len-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					sum=0;
					for(m=i;m<i+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=i;m<j-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else if(i>5&&j-i<7&&len-j>6)//�ߡ�����Ч
				{
					n=1;
					sum=0;
					for(m=j;m<j+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=j;m<len-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					sum=0;
					for(m=0;m<6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;
						continue;
					}
					for(m=0;m<i-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else if(i>5&&j-i>6&&len-j<7)//�С�����Ч
				{
					n=1;
					sum=0;
					for(m=i;m<i+6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=i;m<j-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					sum=0;
					for(m=0;m<6;m++)
						if(GetABit(shapeIndex,m))sum++;
					if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					{
						n=0;continue;
					}
					for(m=0;m<i-6;m++)
					{
						sum-=GetABit(shapeIndex,m);
						sum+=GetABit(shapeIndex,m+6);
						if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
						{
							n=0;continue;
						}
					}
					if(n==1)
						goto loop;
				}
				else//����Ч
					goto loop;
			}
		}
		if(lpArray[shapeIndex]==5)//5������һ��5
			lpArray[shapeIndex]=THREAT_five_THIRD;
		else//4������һ��4
			lpArray[shapeIndex]=THREAT_four_THIRD;
loop:	;//ֻҪ��һ�ֿ��ƽ��ŷ�������Ϊ����в����
	}
	printf("Finish create THREAT_THIRD( THREAT_five_THIRD & THREAT_four_THIRD ).\n");
}

void createTHREAT_DOUBLE(LPBYTE lpArray)//����˫��в���ͣ�ͨ����һ�Ӳ��߽����жϣ�
{
	int i,shapeIndex,len,m,n,sum;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=5&&lpArray[shapeIndex]!=4)//˫��в����ֻ�ֲ���5�ͺ�4����
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)//һ�����������Ϊ��������Ч����
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(i<6&&len-i>6)//���λ������Ч
			{
				n=1;
				sum=0;
				for(m=i;m<i+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>3)
				{
					n=0;
					continue;
				}
				for(m=i;m<len-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>3)
					{
						n=0;continue;
					}
				}
				if(n==1)
					goto loop;
			}
			else if(i>5&&len-i<7)//���λ������Ч
			{
				n=1;
				sum=0;
				for(m=0;m<6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>3)
				{
					n=0;continue;
				}
				for(m=0;m<i-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>3)
					{
						n=0;continue;
					}
				}
				if(n==1)
					goto loop;
			}
			else if(i>5&&len-i>6)//�ߡ�����Ч
			{
				n=1;
				sum=0;
				for(m=i;m<i+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>3)
				{
					n=0;continue;
				}
				for(m=i;m<len-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>3)
					{
						n=0;continue;
					}
				}
				sum=0;
				for(m=0;m<6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>3)
				{
					n=0;continue;
				}
				for(m=0;m<i-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>3)
					{
						n=0;continue;
					}
				}
				if(n==1)
					goto loop;
			}
			else//����Ч
				goto loop;
		}
		if(lpArray[shapeIndex]==5)
			lpArray[shapeIndex]=THREAT_five_DOUBLE;
		else
			lpArray[shapeIndex]=THREAT_four_DOUBLE;
loop:	;//ֻҪ��һ�ֿ��ƽ��ŷ�������Ϊ����в����
	}
	printf("Finish create THREAT_DOUBLE( THREAT_five_DOUBLE & THREAT_four_DOUBLE )\n");
}

void createTHREAT_five_SINGLE(LPBYTE lpArray)//�������ӵ���в���ͣ�ʣ��5�ͼ��ǣ�
{
	int shapeIndex;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]==5)
			lpArray[shapeIndex]=THREAT_five_SINGLE;
	}
	printf("Finish create THREAT_five_SINGLE.\n");
}

void createTHREAT_four_SUPERPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=THREAT_four_DOUBLE)//˫��в����ֻ�ֲ���5�ͺ�6����
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_five_THIRD||lpArray[shapeIndex+(1<<i)]==THREAT_four_THIRD)//��ԭ��������Ϊ������
				lpArray[shapeIndex]=THREAT_four_SUPERPOTEN;
		}
	}
	printf("Finish create THREAT_four_SUPERPOTEN.\n");
}

void createTHREAT_four_BIGPOTEN(LPBYTE lpArray)//�������ӵ���в��Ǳ������
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=4)//˫��в����ֻ�ֲ���5�ͺ�6����
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_five_THIRD||lpArray[shapeIndex+(1<<i)]==THREAT_four_THIRD)//��ԭ��������Ϊ������
				lpArray[shapeIndex]=THREAT_four_BIGPOTEN;
		}
	}
	printf("Finish create THREAT_four_BIGPOTEN.\n");
}

void createTHREAT_four_POTEN(LPBYTE lpArray)//�������ӵ���в˫Ǳ������
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=4)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_DOUBLE||lpArray[shapeIndex+(1<<i)]==THREAT_four_SUPERPOTEN||lpArray[shapeIndex+(1<<i)]==THREAT_five_DOUBLE)
			{
				lpArray[shapeIndex]=THREAT_four_POTEN;
				break;
			}
		}
	}
	printf("Finish create THREAT_four_POTEN.\n");
}

void createTHREAT_four_ADDITION(LPBYTE lpArray)//����������˫��в˫Ǳ������
{
	int i,shapeIndex,len,sum=0;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=THREAT_four_POTEN)//THREAT_four_ADDITION�����Ǵ�THREAT_four_POTEN�����������ģ�ʣ��4���в�������
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		sum=0;
		for(i=1;i<6;i++)
			if(GetABit(shapeIndex,i)==1)sum++;
		if(sum==4&&GetABit(shapeIndex,1)==1&&GetABit(shapeIndex,5)==1&&GetABit(shapeIndex,0)==0&&GetABit(shapeIndex,6)==0)
		{
			lpArray[shapeIndex]=THREAT_four_ADDITION;
			continue;
		}
		for(i=1;i<len-6;i++)
		{
			sum-=GetABit(shapeIndex,i);
			sum+=GetABit(shapeIndex,i+5);
			if(sum==4&&GetABit(shapeIndex,i+1)==1&&GetABit(shapeIndex,i+5)==1&&GetABit(shapeIndex,i)==0&&GetABit(shapeIndex,i+6)==0)
			{
				lpArray[shapeIndex]=THREAT_four_ADDITION;
				break;
			}
		}
	}
	printf("Finish create THREAT_four_ADDITION .\n");
}

void createTHREAT_four_SINGLE(LPBYTE lpArray)//�������ӵ���в���ͣ�ʣ��4�ͼ��ǣ�
{
	int shapeIndex;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]==4)
			lpArray[shapeIndex]=THREAT_four_SINGLE;
	}
	printf("Finish create THREAT_four_SINGLE.\n");
}

void createPOTEN_three_SUPERPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_SUPERPOTEN)
			{
				lpArray[shapeIndex]=POTEN_three_SUPERPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_SUPERPOTEN.\n");
}

void createPOTEN_three_DOUBLE(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_DOUBLE)
			{
				lpArray[shapeIndex]=POTEN_three_DOUBLE;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_DOUBLE.\n");
}

void createPOTEN_three_BIGPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_BIGPOTEN)
			{
				lpArray[shapeIndex]=POTEN_three_BIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_BIGPOTEN.\n");
}

void createPOTEN_three_ADDITION(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_ADDITION)
			{
				lpArray[shapeIndex]=POTEN_three_ADDITION;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ADDITION.\n");
}

void createPOTEN_three_ANDPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==THREAT_four_POTEN)
			{
				lpArray[shapeIndex]=POTEN_three_ANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ANDPOTEN.\n");
}

void createPOTEN_three_ORPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=3)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_three_SUPERPOTEN)
			{
				lpArray[shapeIndex]=POTEN_three_ORPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_three_ORPOTEN.\n");
}

void createPOTEN_three_SINGLE(LPBYTE lpArray)
{
	int shapeIndex;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]==3)
			lpArray[shapeIndex]=POTEN_three_SINGLE;
	}
	printf("Finish create POTEN_three_SINGLE.\n");
}

void createPOTEN_two_TOBIGPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=2)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_three_DOUBLE)
			{
				lpArray[shapeIndex]=POTEN_two_TOBIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOBIGPOTEN.\n");
}

void createPOTEN_two_TOADDITION(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=2)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_three_ADDITION)
			{
				lpArray[shapeIndex]=POTEN_two_TOADDITION;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOADDITION.\n");
}

void createPOTEN_two_TOANDPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=2)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_three_ANDPOTEN)
			{
				lpArray[shapeIndex]=POTEN_two_TOANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOANDPOTEN.\n");
}

void createPOTEN_two_TOPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=2)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_three_SINGLE)
			{
				lpArray[shapeIndex]=POTEN_two_TOPOTEN;
				break;
			}
		}
	}
	printf("Finish create POTEN_two_TOPOTEN.\n");
}

void createVOID_one_TOBIGPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=1)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_two_TOBIGPOTEN)
			{
				lpArray[shapeIndex]=VOID_one_TOBIGPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOBIGPOTEN.\n");
}

void createVOID_one_TOADDITION(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=1)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_two_TOADDITION)
			{
				lpArray[shapeIndex]=VOID_one_TOADDITION;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOADDITION.\n");
}

void createVOID_one_TOANDPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=1)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_two_TOANDPOTEN)
			{
				lpArray[shapeIndex]=VOID_one_TOANDPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOANDPOTEN.\n");
}

void createVOID_one_TOPOTEN(LPBYTE lpArray)
{
	int i,shapeIndex,len;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=1)
			continue;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(lpArray[shapeIndex+(1<<i)]==POTEN_two_TOPOTEN)
			{
				lpArray[shapeIndex]=VOID_one_TOPOTEN;
				break;
			}
		}
	}
	printf("Finish create VOID_one_TOPOTEN.\n");
}

void createZERO(LPBYTE lpArray)
{
	int shapeIndex;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]==0)
			lpArray[shapeIndex]=ZERO;
	}
	printf("Finish create ZERO.\n");
}


void AnalyDownOne(LPBYTE lpArray,BYTE tag)
{
	int i,shapeIndex,len;
	int num,sum[WIN+1],makedo[WIN+1];
	for(i=0;i<=WIN;i++)
		sum[i]=0;
	num=0;
	for(shapeIndex=64;shapeIndex<TableSize;shapeIndex++)
	{
		if(lpArray[shapeIndex]!=tag)//THREAT_four_ADDITION�����Ǵ�THREAT_four_POTEN�����������ģ�ʣ��4���в�������
			continue;
		num++;
		for(i=0;i<=WIN;i++)
			makedo[i]=0;
		i=19;
		while(!GetABit(shapeIndex,i))
			i--;
		len=i;
		for(i=0;i<len;i++)
		{
			if((shapeIndex>>i)&1)
				continue;
			makedo[lpArray[shapeIndex+(1<<i)]]|=1;
		}
		for(i=0;i<=WIN;i++)
			sum[i]+=makedo[i];
	}
	printf("- %d:%d  ",tag,num);
	for(i=WIN;i>0;i--)
	{
		if(sum[i]!=0)
			printf("%d:%d ",i,sum[i]);
	}
	printf("\n");
}


//�������Ϳ�
void create(LPBYTE lpArray)
{
	//*���׽��з���
	GetMode(lpArray);//��ȡ���͵Ľ�
	//*�����ʽ�����ϸ�Ķ��η���
	createWIN(lpArray);//�궨��ʤ����
	createTHREAT_THIRD(lpArray);//�궨����в���ͣ����һ�����Ľ�һ����
	createTHREAT_DOUBLE(lpArray);//�궨˫��в���ͣ���׶������Ľ�...��
	createTHREAT_five_SINGLE(lpArray);//�궨�����������
	createTHREAT_four_SUPERPOTEN(lpArray);
	createTHREAT_four_BIGPOTEN(lpArray);
	createTHREAT_four_POTEN(lpArray);
	createTHREAT_four_ADDITION(lpArray);
	createTHREAT_four_SINGLE(lpArray);
	createPOTEN_three_SUPERPOTEN(lpArray);
	createPOTEN_three_DOUBLE(lpArray);
	createPOTEN_three_BIGPOTEN(lpArray);
	createPOTEN_three_ADDITION(lpArray);
	createPOTEN_three_ANDPOTEN(lpArray);
	createPOTEN_three_ORPOTEN(lpArray);
	createPOTEN_three_SINGLE(lpArray);
	createPOTEN_two_TOBIGPOTEN(lpArray);
	createPOTEN_two_TOADDITION(lpArray);
	createPOTEN_two_TOANDPOTEN(lpArray);
	createPOTEN_two_TOPOTEN(lpArray);
	createVOID_one_TOBIGPOTEN(lpArray);
	createVOID_one_TOADDITION(lpArray);
	createVOID_one_TOANDPOTEN(lpArray);
	createVOID_one_TOPOTEN(lpArray);
	createZERO(lpArray);
#ifdef OUT
	out(lpArray,WIN);
	out(lpArray,THREAT_five_THIRD);
	out(lpArray,THREAT_five_DOUBLE);
	out(lpArray,THREAT_five_SINGLE);
	out(lpArray,THREAT_four_THIRD);
	out(lpArray,THREAT_four_SUPERPOTEN);
	out(lpArray,THREAT_four_DOUBLE);
	out(lpArray,THREAT_four_BIGPOTEN);
	out(lpArray,THREAT_four_ADDITION);
	out(lpArray,THREAT_four_POTEN);
	out(lpArray,THREAT_four_SINGLE);
	out(lpArray,POTEN_three_SUPERPOTEN);
	out(lpArray,POTEN_three_DOUBLE);
	out(lpArray,POTEN_three_BIGPOTEN);
	out(lpArray,POTEN_three_ADDITION);
	out(lpArray,POTEN_three_ANDPOTEN);
	out(lpArray,POTEN_three_ORPOTEN);
	out(lpArray,POTEN_three_SINGLE);
	out(lpArray,POTEN_two_TOBIGPOTEN);
	out(lpArray,POTEN_two_TOADDITION);
	out(lpArray,POTEN_two_TOANDPOTEN);
	out(lpArray,POTEN_two_TOPOTEN);
	out(lpArray,VOID_one_TOBIGPOTEN);
	out(lpArray,VOID_one_TOADDITION);
	out(lpArray,VOID_one_TOANDPOTEN);
	out(lpArray,VOID_one_TOPOTEN);
	out(lpArray,ZERO);
	out(lpArray,VALUELESS);
	write(lpArray);
#endif

#ifdef ANALY
	AnalyDownOne(lpArray,THREAT_five_THIRD);
	AnalyDownOne(lpArray,THREAT_five_DOUBLE);
	AnalyDownOne(lpArray,THREAT_five_SINGLE);
	AnalyDownOne(lpArray,THREAT_four_THIRD);
	AnalyDownOne(lpArray,THREAT_four_SUPERPOTEN);
	AnalyDownOne(lpArray,THREAT_four_DOUBLE);
	AnalyDownOne(lpArray,THREAT_four_BIGPOTEN);
	AnalyDownOne(lpArray,THREAT_four_ADDITION);
	AnalyDownOne(lpArray,THREAT_four_POTEN);
	AnalyDownOne(lpArray,THREAT_four_SINGLE);
	AnalyDownOne(lpArray,POTEN_three_SUPERPOTEN);
	AnalyDownOne(lpArray,POTEN_three_DOUBLE);
	AnalyDownOne(lpArray,POTEN_three_BIGPOTEN);
	AnalyDownOne(lpArray,POTEN_three_ADDITION);
	AnalyDownOne(lpArray,POTEN_three_ANDPOTEN);
	AnalyDownOne(lpArray,POTEN_three_ORPOTEN);
	AnalyDownOne(lpArray,POTEN_three_SINGLE);
	AnalyDownOne(lpArray,POTEN_two_TOBIGPOTEN);
	AnalyDownOne(lpArray,POTEN_two_TOADDITION);
	AnalyDownOne(lpArray,POTEN_two_TOANDPOTEN);
	AnalyDownOne(lpArray,POTEN_two_TOPOTEN);
	AnalyDownOne(lpArray,VOID_one_TOBIGPOTEN);
	AnalyDownOne(lpArray,VOID_one_TOADDITION);
	AnalyDownOne(lpArray,VOID_one_TOANDPOTEN);
	AnalyDownOne(lpArray,VOID_one_TOPOTEN);
	AnalyDownOne(lpArray,ZERO);
#endif

}

#ifndef NEW
BYTE preTab[TableSize];
void compareOld(LPBYTE lpArray)
{
	int i;
	readOld(preTab);
	if(compare(preTab,lpArray,WIN))
		return;
	if(compare(preTab,lpArray,THREAT_THIRD))
		return;
	if(compare(preTab,lpArray,THREAT_five_DOUBLE))
		return;
	if(compare(preTab,lpArray,THREAT_five_SINGLE))
		return;
	if(compare(preTab,lpArray,THREAT_four_DOUBLE))
		return;
	if(compare(preTab,lpArray,THREAT_four_POTEN))
		return;
	if(compare(preTab,lpArray,THREAT_four_SINGLE))
		return;
	if(compare(preTab,lpArray,POTEN_three_DOUBLE))
		return;
	if(compare(preTab,lpArray,POTEN_three_twoDOUBLE))
		return;
	if(compare(preTab,lpArray,POTEN_three_SINGLE))
		return;
	if(compare(preTab,lpArray,POTEN_two_DOUBLE))
		return;
	if(compare(preTab,lpArray,POTEN_two_SINGLE))
		return;
	if(compare(preTab,lpArray,POTEN_two_SINGLE2))
		return;
	if(compare(preTab,lpArray,one_DOUBLE))
		return;
	if(compare(preTab,lpArray,one_SINGLE))
		return;
}
#endif


int initialLineTypeTable(LPBYTE lpArray)
{
	create(lpArray);
#ifndef NEW
	compareOld(lpArray);
#endif
	return 0;
}


//������Ϣ����
//=====================================================================================================================
void initialLineInfo(LineTypeInfo *linetypeInfo)
{
	linetypeInfo->lineType=0;

	linetypeInfo->win=-1;//5->6
	linetypeInfo->willWin=-1;//4->5
	linetypeInfo->triThreat=-1;
//	linetypeInfo->triPoten=-1;
	linetypeInfo->toOne=-1;//0->1

	linetypeInfo->defStepList.clear();
	linetypeInfo->defPointList.clear();
	linetypeInfo->duoThreatList.clear();
	linetypeInfo->solThreatList.clear();
	linetypeInfo->duoPotenList.clear();
	linetypeInfo->solPotenList.clear();
	linetypeInfo->toDuoTwoList.clear();
	linetypeInfo->toSolTwoList.clear();
}
void AnalyDefentStep(int shapeIndex,int len,LineTypeInfo *the)
{
	int i,j,m,sum;
	iStep tempStep;
	for(i=0;i<len-1;i++)//һ�����������Ϊ��������Ч����(û��������Ч���͵Ŀ���)
	{
		if(GetABit(shapeIndex,i)!=0)
			continue;
		for(j=i+1;j<len;j++)
		{
			if(GetABit(shapeIndex,j)!=0)
				continue;
			if(i<6&&j-i<7&&len-j>6)//���λ������Ч
			{
				sum=0;
				for(m=j;m<j+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=j;m<len-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<len-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else if(i<6&&j-i>6&&len-j<7)//�м�������Ч
			{
				sum=0;
				for(m=i;m<i+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>3)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=i;m<j-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<j-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else if(i>5&&j-i<7&&len-j<7)//���λ������Ч
			{
				sum=0;
				for(m=0;m<6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=0;m<i-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<i-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else if(i<6&&j-i>6&&len-j>6)//�ߡ�����Ч
			{
				sum=0;
				for(m=j;m<j+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=j;m<len-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<len-6)
					continue;
				sum=0;
				for(m=i;m<i+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=i;m<j-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<j-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else if(i>5&&j-i<7&&len-j>6)//�ߡ�����Ч
			{
				sum=0;
				for(m=j;m<j+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=j;m<len-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<len-6)
					continue;
				sum=0;
				for(m=0;m<6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=0;m<i-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<i-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else if(i>5&&j-i>6&&len-j<7)//�С�����Ч
			{
				sum=0;
				for(m=i;m<i+6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=i;m<j-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<j-6)
					continue;
				sum=0;
				for(m=0;m<6;m++)
					if(GetABit(shapeIndex,m))sum++;
				if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
					continue;
				for(m=0;m<i-6;m++)
				{
					sum-=GetABit(shapeIndex,m);
					sum+=GetABit(shapeIndex,m+6);
					if(sum>=4)//�����������ϰ�������4���㣬Ϊ��в��
						break;
				}
				if(m<i-6)
					continue;
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
			else//����Ч
			{
				tempStep.first=i;
				tempStep.second=j;
				the->defStepList.push_back(tempStep);
			}
		}
	}
}
void AnalyDefentPoint(int shapeIndex,int len,LineTypeInfo *the)
{
	int i,m,sum;
	for(i=0;i<len;i++)//һ�����������Ϊ��������Ч����
	{
		if(GetABit(shapeIndex,i))
			continue;
		if(i<6&&len-i>6)//���λ������Ч
		{
			sum=0;
			for(m=i;m<i+6;m++)
				if(GetABit(shapeIndex,m))sum++;
			if(sum>=4)
				continue;
			for(m=i;m<len-6;m++)
			{
				sum-=GetABit(shapeIndex,m);
				sum+=GetABit(shapeIndex,m+6);
				if(sum>=4)
					break;
			}
			if(m<len-6)
				continue;
			the->defPointList.push_back(i);
		}
		else if(i>5&&len-i<7)//���λ������Ч
		{
			sum=0;
			for(m=0;m<6;m++)
				if(GetABit(shapeIndex,m))sum++;
			if(sum>=4)
				continue;
			for(m=0;m<i-6;m++)
			{
				sum-=GetABit(shapeIndex,m);
				sum+=GetABit(shapeIndex,m+6);
				if(sum>=4)
					break;
			}
			if(m<i-6)
				continue;
			the->defPointList.push_back(i);
		}
		else if(i>5&&len-i>6)//�ߡ�����Ч
		{
			sum=0;
			for(m=i;m<i+6;m++)
				if(GetABit(shapeIndex,m))sum++;
			if(sum>=4)
				continue;
			for(m=i;m<len-6;m++)
			{
				sum-=GetABit(shapeIndex,m);
				sum+=GetABit(shapeIndex,m+6);
				if(sum>=4)
					break;
			}
			if(m<len-6)
				continue;
			sum=0;
			for(m=0;m<6;m++)
				if(GetABit(shapeIndex,m))sum++;
			if(sum>=4)
				continue;
			for(m=0;m<i-6;m++)
			{
				sum-=GetABit(shapeIndex,m);
				sum+=GetABit(shapeIndex,m+6);
				if(sum>=4)
					break;
			}
			if(m<i-6)
				continue;
			the->defPointList.push_back(i);
		}
		else//����Ч
			the->defPointList.push_back(i);
	}
}
void AnalyLineInfo(int shapeIndex,LineTypeInfo *the)		//���ͷ���
{
	int i,len,sum;
	//len:���͵ĳ��ȡ�
	iStep tempStep;
	int ShapeStyleId;	//�������ͱ�ţ�1~14����

	ShapeStyleId=preTable[shapeIndex];
	if(ShapeStyleId==WIN)	//������������ʤ
	{
		the->lineType=7;
		return;
	}
	i=19;
	while(!GetABit(shapeIndex,i))
		i--;
	len=i;
	//����Ϊ��в��
	if(ShapeStyleId==THREAT_five_THIRD)//���һ��  ��ʤ��
	{
		the->lineType=6;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==WIN)
			{
				the->win=i;
				break;
			}
		}
	}
	else if(ShapeStyleId==THREAT_five_DOUBLE)//��׶���  ��ʤ�㣬�ƽⲽ
	{
		the->lineType=5;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==WIN)
			{
				the->win=i;
				break;
			}
		}
		AnalyDefentStep(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_five_SINGLE)//�������  ������ʤ�㣬�ƽ��
	{
		the->lineType=4;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==WIN)
			{
				the->win=i;
				break;
			}
		}
		AnalyDefentPoint(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_four_THIRD)//�Ľ�һ��  ���ɼ���ʤ���㣨һ��5��
	{
		the->lineType=6;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_THIRD)
			{
				the->willWin=i;
				break;
			}
		}
	}
	else if(ShapeStyleId==THREAT_four_SUPERPOTEN)//�Ľ׶���  ���ɼ���ʤ����Ͷ���в�㣬�ƽⲽ
	{
		the->lineType=5;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_THIRD)//�������һ��������ʤ���㣬����в�㣩
			{
				the->willWin=i;
				the->triThreat=i;
				break;
			}
		}
		if(the->triThreat==-1)//�����������һ�����֣������Ľ�һ��
		{
			for(i=0;i<len;i++)
			{
				if(GetABit(shapeIndex,i))
					continue;
				if(preTable[shapeIndex+(1<<i)]==THREAT_four_THIRD)//�����Ľ�һ��������в�㣩
				{
					the->triThreat=i;
					if(the->willWin!=-1)
						break;
				}
				else if(preTable[shapeIndex+(1<<i)]==THREAT_five_DOUBLE)//������׶���������ʤ���㣩
				{
					the->willWin=i;
					if(the->triThreat!=-1)
						break;
				}
			}
		}
		AnalyDefentStep(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_four_DOUBLE)//�Ľ�����  ��������ʤ���㣨��׶��������ƽⲽ
	{
		the->lineType=5;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_DOUBLE)
			{
				the->willWin=i;
				break;
			}
		}
		AnalyDefentStep(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_four_BIGPOTEN)//�Ľ��ļ�  ��������ʤ����Ͷ���в�㣬�ƽ��
	{
		the->lineType=4;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_THIRD)//���� �Ľ�һ��������в�㣩
			{
				the->triThreat=i;
				if(the->willWin!=-1)
					break;
			}
			else if(preTable[shapeIndex+(1<<i)]>=THREAT_five_SINGLE)//���� ��׶��� �� �������������ʤ���㣩
			{
				the->willWin=i;
				if(the->triThreat!=-1)
					break;
			}
		}
		AnalyDefentPoint(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_four_ADDITION)//�Ľ��弶  ��������ʤ���㣬˫��в�㣬�ƽⲽ���ƽ��
	{
		the->lineType=4;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_DOUBLE)//����ʤ���㣬˫��в�㣻����������׶�������
			{
				the->willWin=i;
				the->duoThreatList.push_back(i);
			}
#ifndef _K_
			else if(preTable[shapeIndex+(1<<i)]==THREAT_four_DOUBLE)//˫��в�㣬��©��ԭ��
				the->duoThreatList.push_back(i);
#endif
		}
		AnalyDefentPoint(shapeIndex,len,the);
		sum=0;
		for(i=1;i<6;i++)
			if(GetABit(shapeIndex,i)==1)sum++;
		if(sum==4&&GetABit(shapeIndex,1)==1&&GetABit(shapeIndex,5)==1&&GetABit(shapeIndex,0)==0&&GetABit(shapeIndex,6)==0)
		{
			tempStep.first=0;
			tempStep.second=6;
			the->defStepList.push_back(tempStep);
		}
		for(i=1;i<len-6;i++)
		{
			sum-=GetABit(shapeIndex,i);
			sum+=GetABit(shapeIndex,i+5);
			if(sum==4&&GetABit(shapeIndex,i+1)==1&&GetABit(shapeIndex,i+5)==1&&GetABit(shapeIndex,i)==0&&GetABit(shapeIndex,i+6)==0)
			{
				tempStep.first=i;
				tempStep.second=i+6;
				the->defStepList.push_back(tempStep);
			}
		}
	}
	else if(ShapeStyleId==THREAT_four_POTEN)//�Ľ�����  ����ʤ���㣬˫��в�㣬�ƽ��
	{
		the->lineType=4;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_DOUBLE)//����ʤ���㣬˫��в��
			{
				the->duoThreatList.push_back(i);
				the->willWin=i;
			}
		}
		if(the->duoThreatList.size()==0)
		{
			for(i=0;i<len;i++)
			{
				if(GetABit(shapeIndex,i))
					continue;
				if(preTable[shapeIndex+(1<<i)]==THREAT_four_BIGPOTEN)//˫��в��
					the->duoThreatList.push_back(i);
			}
		}
		if(the->duoThreatList.size()==0)
		{
			for(i=0;i<len;i++)
			{
				if(GetABit(shapeIndex,i))
					continue;
				if(preTable[shapeIndex+(1<<i)]==THREAT_four_DOUBLE)//˫��в��
					the->duoThreatList.push_back(i);
			}
		}
		if(the->willWin==-1)
		{
			for(i=0;i<len;i++)
			{
				if(GetABit(shapeIndex,i))
					continue;
				if(preTable[shapeIndex+(1<<i)]==THREAT_five_SINGLE)//����ʤ����
				{
					the->willWin=i;
					break;
				}
			}
		}
		AnalyDefentPoint(shapeIndex,len,the);
	}
	else if(ShapeStyleId==THREAT_four_SINGLE)//�Ľ��߼�  ��������ʤ���㣨����5�����ƽ��
	{
		the->lineType=4;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_five_SINGLE)//����ʤ����
			{
				the->willWin=i;
				break;
			}
		}
		AnalyDefentPoint(shapeIndex,len,the);
	}
	//����Ϊ����в��
	else if(ShapeStyleId==POTEN_three_SUPERPOTEN)//����һ��  ˫��в�㣬����в�㣬��Ǳ����
	{
		the->lineType=3;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_DOUBLE)//˫��в��
				the->duoThreatList.push_back(i);
			else if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_SUPERPOTEN)
				the->duoThreatList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_DOUBLE)//���׶���  ˫��в�㣬����в��
	{
		the->lineType=2;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_DOUBLE)//˫��в��
				the->duoThreatList.push_back(i);
#ifndef _K_
			else if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_BIGPOTEN)//��������  ����в�㣬��Ǳ����
	{
		the->lineType=1;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в�㣬������Ǳ��
				the->solThreatList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_BIGPOTEN)
				the->solThreatList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_ADDITION)//�����ļ�  ����в�㣬˫Ǳ����
	{
		the->lineType=1;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
			else if(preTable[shapeIndex+(1<<i)]>=POTEN_three_DOUBLE)//˫Ǳ����
				the->duoPotenList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_ADDITION)
				the->solThreatList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_ANDPOTEN)//�����弶  ����в�㣬˫Ǳ����
	{
		the->lineType=1;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
			else if(preTable[shapeIndex+(1<<i)]>=POTEN_three_DOUBLE)//˫Ǳ����
				the->duoPotenList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_POTEN)
				the->solThreatList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_ORPOTEN)//��������  ����в�㣬˫Ǳ����
	{
		the->lineType=1;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
			else if(preTable[shapeIndex+(1<<i)]>=POTEN_three_DOUBLE)//˫Ǳ����
				the->duoPotenList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_SINGLE)
				the->solThreatList.push_back(i);
			if(preTable[shapeIndex+(1<<i)]==POTEN_three_SUPERPOTEN)
				the->duoPotenList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_three_SINGLE)//�����߼�  ����в��
	{
		the->lineType=1;
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==THREAT_four_SINGLE)//����в��
				the->solThreatList.push_back(i);
		}
	}
	else if(ShapeStyleId==POTEN_two_TOBIGPOTEN)///����һ��
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_three_DOUBLE)//˫Ǳ����
				the->duoPotenList.push_back(i);
		}
	}
	else if(ShapeStyleId==POTEN_two_TOADDITION)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=POTEN_three_SINGLE)//��Ǳ����
				the->solPotenList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==POTEN_three_ADDITION)
				the->solPotenList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_two_TOANDPOTEN)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
#ifndef _K_
			if(preTable[shapeIndex+(1<<i)]>=POTEN_three_SINGLE)//��Ǳ����
				the->solPotenList.push_back(i);
#else
			if(preTable[shapeIndex+(1<<i)]==POTEN_three_ANDPOTEN)
				the->solPotenList.push_back(i);
#endif
		}
	}
	else if(ShapeStyleId==POTEN_two_TOPOTEN)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_three_SINGLE)//��Ǳ����
				the->solPotenList.push_back(i);
		}
	}
	else if(ShapeStyleId==VOID_one_TOBIGPOTEN)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_two_TOBIGPOTEN)//Ǳ˫Ǳ����
				the->toDuoTwoList.push_back(i);
		}
	}
	else if(ShapeStyleId==VOID_one_TOADDITION)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_two_TOADDITION)
				the->toSolTwoList.push_back(i);
		}
	}
	else if(ShapeStyleId==VOID_one_TOANDPOTEN)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_two_TOANDPOTEN)
				the->toSolTwoList.push_back(i);
		}
	}
	else if(ShapeStyleId==VOID_one_TOPOTEN)
	{
		for(i=0;i<len;i++)
		{
			if(GetABit(shapeIndex,i))
				continue;
			if(preTable[shapeIndex+(1<<i)]==POTEN_two_TOPOTEN)
				the->toSolTwoList.push_back(i);
		}
	}
	else if(ShapeStyleId==ZERO)//�������
	{
		i=len/2;
		if(GetABit(shapeIndex,i))
			the->toOne=i+1;
		else
			the->toOne=i;
	}
	return;
}
void updataSum(LineSum *sum,LineTypeInfo *lineInfo)
{
	if(lineInfo->duoThreatList.size()>sum->duoThreat)
		sum->duoThreat=lineInfo->duoThreatList.size();
	if(lineInfo->solThreatList.size()>sum->solThreat)
		sum->solThreat=lineInfo->solThreatList.size();
	if(lineInfo->duoPotenList.size()>sum->duoPoten)
		sum->duoPoten=lineInfo->duoPotenList.size();
	if(lineInfo->solPotenList.size()>sum->solPoten)
		sum->solPoten=lineInfo->solPotenList.size();
	if(lineInfo->toDuoTwoList.size()>sum->toDuoTwo)
		sum->toDuoTwo=lineInfo->toDuoTwoList.size();
	if(lineInfo->toSolTwoList.size()>sum->toSolTwo)
		sum->toSolTwo=lineInfo->toSolTwoList.size();
	if(lineInfo->defStepList.size()>sum->defStep)
		sum->defStep=lineInfo->defStepList.size();
	if(lineInfo->defPointList.size()>sum->defPoint)
		sum->defPoint=lineInfo->defPointList.size();
}
void createLineInfoTable(LineTypeInfo *linetypeInfo)
{
	int i,t=0;
//	LineSum sum;
//	memset(&sum,0,sizeof(sum));
	for(i=0;i<64;i++)
		initialLineInfo(&linetypeInfo[i]);
	for(i=64;i<TableSize;i++)
	{
		initialLineInfo(&linetypeInfo[i]);
		AnalyLineInfo(i,&linetypeInfo[i]);
//		updataSum(&sum,&linetypeInfo[i]);
	}
/*	printf("defStep=%d\n",sum.defStep);
	printf("defPoint=%d\n",sum.defPoint);
	printf("duoThreat=%d\n",sum.duoThreat);
	printf("solThreat=%d\n",sum.solThreat);
	printf("duoPoten=%d\n",sum.duoPoten);
	printf("solPoten=%d\n",sum.solPoten);
	printf("toDuoTwo=%d\n",sum.toDuoTwo);
	printf("toSolTwo=%d\n",sum.toSolTwo);
	printf("toOne=%d\n",sum.toOne);*/
}

void initialLineInfoTable(LineTypeInfo *linetypeInfo)
{
	createLineInfoTable(linetypeInfo);
}

//=========================================================================
