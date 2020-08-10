#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>//�ṩ���⺯��
#include<stdlib.h>//�ṩ�ļ������ຯ��
#include<math.h>//�ṩ��ѧ���㺯��
#include<string.h>//�ṩ�ַ�����������
#include<malloc.h>//�ṩ��̬�ڴ���亯��

#define Actual_DATA_MAX 15000//����ʵ���ź����ݵ�������
#define Reference_DATA_MAX 2048//�ο��ź����ݸ���
#define Actual 12//����ʵ���źŸ���
#define Reference 3//�ο��źŸ���

typedef struct Signal_sorce {
	char name[50];//�ź�����(����)
	double complex_I[Actual_DATA_MAX];//ʵ������
	double complex_Q[Actual_DATA_MAX];//�鲿����
	double Power;//�������źŹ���
	double Dependency;//�ο��ź�������ѡ���ź������
}Signal;

Signal Actual_Signal[Actual];//��ʵ�źŽṹ��
Signal Reference_Signal[Reference];//��׼�źŽṹ��
Signal Correlation[20];//������ؼ�������ṹ��
Signal ch;//�����ýṹ��

void Read_name();//��ȡ�����ļ���
void Read_data();//��ȡ�ļ�����
void Computing_power();//����ʵ���źŹ���
void Power_sorting();//��������
void Correlation_computing_IQ();//���������Correlation[](I Q)����
void Correlation_computing();//�ҳ�������������ֵ,������Correlation[].Dependency
int Correlation_sorting();//���������,���������


FILE* fp1, * fp2, * fp3;
errno_t err1=0,err2=0,err3=0;

int main()
{
	int Serial_number;//��¼����Լ��������
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
		printf("�������ļ�ʧ��!\n");
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
			printf("�������ļ�%dʧ��!\n",i);
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
			printf("��ʧ��!\n");
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
	printf("������ʵ�źŵĹ��ʷֱ���:\n");
	for (i=0;i< Actual;i++)
	{
		Actual_Signal[i].Power = 0;
		for (j = 0; j < Actual_DATA_MAX; j++)
		{
			Actual_Signal[i].Power += sqrt(Actual_Signal[i].complex_I[j] * Actual_Signal[i].complex_I[j] + Actual_Signal[i].complex_Q[j] * Actual_Signal[i].complex_Q[j]);
		}
		//����ÿ����ʵ�źŵĹ���
		printf("data%d�Ĺ�����%lf\n", i,Actual_Signal[i].Power);
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
			if (Actual_Signal[j].Power > Actual_Signal[k].Power)//���Ӵ�С����
			{
				k = j;//��¼������±�λ��
			}
		}
		if (k != i)//����������±겻���±�λ��i
		{
			ch = Actual_Signal[k];
			Actual_Signal[k] = Actual_Signal[i];
			Actual_Signal[i] = ch;
		}
	}//ѡ������
	printf("����ǿ������ǰ������:\n");
	for (i = 0; i < 6; i++)
	{
		printf("%s������%lf\n", Actual_Signal[i].name, Actual_Signal[i].Power);
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
			Correlation[i * j + j].complex_Q[0] = 0.0;//����ֵ
			for (k=0;k< Actual_DATA_MAX- Reference_DATA_MAX;k++)
			{
				for (h=0;h< Reference_DATA_MAX;h++)
				{
					Correlation[i * j + j].complex_I[k] += Actual_Signal[j].complex_I[h]* Reference_Signal[i].complex_I[h];
					Correlation[i * j + j].complex_Q[k] += Actual_Signal[j].complex_Q[h]* Reference_Signal[i].complex_Q[h];//�������������IQ����
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
			if (K < (Correlation[i].complex_I[j] * Correlation[i].complex_I[j] + Correlation[i].complex_Q[j] * Correlation[i].complex_Q[j]))//����KС��I^2+Q^2,K=I^2+Q^2
			{
				K = Correlation[i].complex_I[j] * Correlation[i].complex_I[j] + Correlation[i].complex_Q[j] * Correlation[i].complex_Q[j];//����Լ���,����ֵ
			}
		}
		Correlation[i].Dependency = K;//������Kֵ��ֵ��Correlation[i].Dependency
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
