#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>//提供特殊函数
#include<stdlib.h>//提供文件操作类函数
#include<math.h>//提供数学计算函数
#include<string.h>//提供字符串操作函数
#include<malloc.h>//提供动态内存分配函数

#define Actual_DATA_MAX 15000//输入实际信号数据的最大个数
#define Reference_DATA_MAX 2048//参考信号数据个数
#define Actual 12//输入实际信号个数
#define Reference 3//参考信号个数

typedef struct Signal_sorce {
	char name[50];//信号名称(代码)
	double complex_I[Actual_DATA_MAX];//实部数据
	double complex_Q[Actual_DATA_MAX];//虚部数据
	double Power;//本输入信号功率
	double Dependency;//参考信号与输入选中信号相关性
}Signal;

Signal Actual_Signal[Actual];//真实信号结构体
Signal Reference_Signal[Reference];//标准信号结构体
Signal Correlation[20];//滑动相关计算后结果结构体
Signal ch;//交换用结构体

void Read_name();//读取数据文件名
void Read_data();//读取文件数据
void Computing_power();//计算实际信号功率
void Power_sorting();//功率排序
void Correlation_computing_IQ();//相关性数组Correlation[](I Q)计算
void Correlation_computing();//找出滑动相关性最大值,并存入Correlation[].Dependency
int Correlation_sorting();//相关性排序,并返回序号


FILE* fp1, * fp2, * fp3;
errno_t err1=0,err2=0,err3=0;

int main()
{
	int Serial_number;//记录相关性计算结果序号
	Read_name();
	Read_data();
	Computing_power();
	Power_sorting();
	Correlation_computing_IQ();
	Correlation_computing();
	Correlation_sorting();
	Serial_number = Correlation_sorting();
	printf("The two most relevant sets of signals are %s and %s .",
		Actual_Signal[Serial_number%6].name, Reference_Signal[Serial_number/6].name);
	return 0;
}

void Read_name()
{
	char names[15],*name;
	if ((err1 = fopen_s(&fp1,"Fname_s.txt","r")) !=0)
	{
		printf("打开名字文件失败!\n");
		system("pause");
		exit(0);
	}
	for (int i = 0; i < Actual; i++) {
		fgets(names, 15, fp1);
		name=strtok(names, "\n");
		strcpy_s(Actual_Signal[i].name, name);
		/*printf("%s\n", signal_s[i].name);*/
	}
	for (int i = 0; i < Reference; i++) {
		fgets(names, 15, fp1);
		name=strtok(names, "\n");
		strcpy_s(Reference_Signal[i].name, names);
		/*printf("%s\n", signal_r[i].name);*/
	}
	fclose(fp1);
}

void Read_data()
{
	for (int i = 0; i < Actual; i++)
	{
		if ((err2 = fopen_s(&fp2, Actual_Signal[i].name, "r")) !=0)
		{
			printf("打开数据文件%d失败!\n",i);
			system("pause");
			exit(0);
		}
		for (int j = 0; j < Actual_DATA_MAX; j++) {
			fscanf_s(fp2,"%lf%lf",&Actual_Signal[i].complex_I[j],&Actual_Signal[i].complex_Q[j]);
		}
	}
	fclose(fp2);
	for(int i = 0; i < Reference; i++)
	{
		if ((err3 = fopen_s(&fp3,Reference_Signal[i].name, "r")) !=0)
		{
			printf("打开失败!\n");
			system("pause");
			exit(0);
		}
		for (int j = 0; j < Reference_DATA_MAX; j++) {
			fscanf_s(fp3,"%lf%lf",&Reference_Signal[i].complex_I[j],&Reference_Signal[i].complex_Q[j]);
		}
	}
	fclose(fp3);
}

void Computing_power()
{
	int i,j;
	printf("各个真实信号的功率分别是:\n");
	for (i=0;i< Actual;i++)
	{
		Actual_Signal[i].Power = 0;
		for (j = 0; j < Actual_DATA_MAX; j++)
		{
			Actual_Signal[i].Power += sqrt(Actual_Signal[i].complex_I[j] * Actual_Signal[i].complex_I[j] + Actual_Signal[i].complex_Q[j] * Actual_Signal[i].complex_Q[j]);
		}
		//计算每个真实信号的功率
		printf("data%d的功率是%lf\n", i,Actual_Signal[i].Power);
	}
}

void Power_sorting()
{
	int i, j,k;
	for (i = 0; i < Actual - 1; i++)
	{
		k = i;
		for (j=1+i;j< Actual;j++)
		{
			if (Actual_Signal[j].Power > Actual_Signal[k].Power)//按从大到小排序
			{
				k = j;//记录最大功率下标位置
			}
		}
		if (k != i)//若果最大数下标不在下标位置i
		{
			ch = Actual_Signal[k];
			Actual_Signal[k] = Actual_Signal[i];
			Actual_Signal[i] = ch;
		}
	}//选择排序法
	printf("功率强度排在前六的是:\n");
	for (i = 0; i < 6; i++)
	{
		printf("%s功率是%lf\n", Actual_Signal[i].name, Actual_Signal[i].Power);
	}
}

void Correlation_computing_IQ()
{
	int i, j, k,h;
	for (i=0;i< Reference;i++)
	{
		for (j=0;j< 6;j++)
		{
			Correlation[i * j + j].complex_I[0] = 0.0;
			Correlation[i * j + j].complex_Q[0] = 0.0;//赋初值
			for (k=0;k< Actual_DATA_MAX- Reference_DATA_MAX;k++)
			{
				for (h=0;h< Reference_DATA_MAX;h++)
				{
					Correlation[i * j + j].complex_I[k] += Actual_Signal[j].complex_I[h]* Reference_Signal[i].complex_I[h];
					Correlation[i * j + j].complex_Q[k] += Actual_Signal[j].complex_Q[h]* Reference_Signal[i].complex_Q[h];//滑动相关性数组IQ计算
				}
			}
		}
	}
}

void Correlation_computing()
{
	int i, j;
	double K = 0;
	for (i=0;i<18;i++)
	{
		K = 0;
		for (j= 0; j < Actual_DATA_MAX - Reference_DATA_MAX; j++)
		{
			if (K < (Correlation[i].complex_I[j] * Correlation[i].complex_I[j] + Correlation[i].complex_Q[j] * Correlation[i].complex_Q[j]))//若果K小于I^2+Q^2,K=I^2+Q^2
			{
				K = Correlation[i].complex_I[j] * Correlation[i].complex_I[j] + Correlation[i].complex_Q[j] * Correlation[i].complex_Q[j];//相关性计算,并赋值
			}
		}
		Correlation[i].Dependency = K;//将最大的K值赋值给Correlation[i].Dependency
	}
}

int Correlation_sorting()
{
	int i,j=0;
	double ch = 0;
	for (i=0;i<18;i++)
	{
		if (ch < Correlation[i].Dependency)
		{
			ch = Correlation[i].Dependency;
			j = i;
		}
	}
	return j;
}
