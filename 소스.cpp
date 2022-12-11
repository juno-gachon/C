//#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#define MAX 50 // �ִ� �л� ��
#define STUDENTNUM_LEN 6

struct stu_score {
	char stu_num[STUDENTNUM_LEN + 1];
	int math;
	int english;
};

struct sum_mean {
	int sum;
	int mean;
};

// ������ ����ü �迭�� ����
struct stu_score scores[MAX];
struct stu_score scores_read[MAX];

struct sum_mean sum_means[MAX];

/*
	�л� ��ȣ(���ĺ� �� ���� 6����), ���� ����(0~100), ��������(0~100)�� �Է¹޾�
	�Է�, ����, ���Ḧ �����ϴ� ���α׷�
*/

void printMenu(); // �޴� ���

void insert();	// �л� ���� �Է� - student.txt�� score.bin�� �ݿ�
void del();		// �л� ���� ���� - student.txt�� score.bin�� �ݿ�
void quit();	// ���α׷� ���� - scoe.bin�� ���� ���� ���� �� ��� ���

int func(int (*) (int, int));

int sum_func(int, int);
int mean_func(int, int);


// ���� Ŭ����� �Լ�
void buffer_clear();

int i = 0;
int count = 0;

int main()
{
	printf("�л� ������ �Է¹޾� �Է�, ����, ���Ḧ �����ϴ� ���α׷��Դϴ�.\n");
	printf("----------------------------------------------------------------------------------\n\n");

	char menu;
	printMenu();
	menu = getchar();
	buffer_clear();


	while (menu != 'q')
	{
		switch (menu)
		{
		case 'i':
		{
			insert();
			break;
		}
		case 'd':
		{
			del();
			break;
		}
		default:
		{
			printf("�ٽ� �Է����ּ���.\n");
			break;
		}
		}

		printMenu();
		menu = getchar();
		buffer_clear();
	}

	quit();

	return 0;
}

void printMenu()
{
	printf("�޴��� �Է��ϼ���: i(�Է�), d(����), q(����)\n");
	printf(">> ");
}

void insert()
{
	FILE* student = NULL, * score = NULL;
	errno_t err1, err2;

	err1 = fopen_s(&student, "student.txt", "a");
	err2 = fopen_s(&score, "score.bin", "ab");
	if (student == 0 || score == 0)
	{
		printf("File open error.");
		return;
	}

	i = 0;

	printf("[�Է�] �л� ��ȣ, ���� ����, ���������� ��ĭ���� �����ϸ� �Է��ϼ���. �����, �� ���� ���� �ٹٲ� ���� '\\n'�� �����ϴ�.\n");
	
	while (1) {
		// buffer_clear(); // ����� �ϸ� �ȵ�!
		printf("�л� ��ȣ: ���ĺ� �� ���� 6����(0�̸� ����)\n");
		printf(">> ");
								// + 1 �ʿ�
		fgets(scores[i].stu_num, STUDENTNUM_LEN + 1, stdin);
		// ���⼭ "0"�� stu_num�� ����Ǵ��� ���Ͽ� ���� ���� ������ ���� X

		// -> �̰� ����� ������ ���� ???????
		// "0" ��� "0\n"�� �־�� �� ���� - WHY?????

		/*******************************************
		*
		* \0�� �ƴ϶� \n�̾��ݾƾƾƾƾƾƾƤ��ƾƤ�
		*
		********************************************/

		if (strcmp(scores[i].stu_num, "0\n") == 0) {
			break;
		}


		printf("��������, ���������� �Է��ϼ��� : ");
		scanf_s("%d %d", &scores[i].math, &scores[i].english);

		fprintf(student, "%s %d %d\n", scores[i].stu_num, scores[i].math, scores[i].english);

		sum_means[i].sum = func(sum_func);
		sum_means[i].mean = func(mean_func);

		// (�ѹ��� �����ص� ������) �Ѹ� ����
		fwrite(&sum_means[i], sizeof(struct sum_mean), 1, score);

		count = i;

		i++;

		buffer_clear();
	}
	
	fclose(student);
	fclose(score);

}

void del()
{
	FILE* student = NULL;
	errno_t err = fopen_s(&student, "student.txt", "r");
	if (student == 0)
	{
		printf("File open error\n");
		return;
	}

	printf("[����] ������ �л� ��ȣ�� �Է��ϼ���.\n");
	printf("�л� ��ȣ>> ");
	char stu_num[MAX] = "";
	fgets(stu_num, STUDENTNUM_LEN + 1, stdin);
	buffer_clear();

	int new_stu_num = 0;
	char str[MAX] = "";
	bool found = false;

	i = 0;
	int j = 0;
										//  &scores_read[i].stu_num���ٴ� �̰� ���� ��
	//	while (fscanf_s(student, "%s %d %d", scores_read[i].stu_num, STUDENTNUM_LEN, &scores_read[i].math, &scores_read[i].english) != EOF) {
//  while (fgets(str, MAX, student) != 0) {
//	 	sscanf_s(str, "%s %d %d", scores_read[i].stu_num, STUDENTNUM_LEN, &scores_read[i].math, &scores_read[i].english);
	while (fscanf_s(student, "%s %d %d", scores_read[i].stu_num, STUDENTNUM_LEN, &scores_read[i].math, &scores_read[i].english) != EOF) {
		
		if (strcmp(scores_read[i].stu_num, stu_num) == 0) {
			found = true;
			continue;
		}
		else {
			strcpy_s(scores[j].stu_num, scores_read[i].stu_num);
			scores[j].math = scores_read[i].math;
			scores[j].english = scores_read[i].english;
			
			j++;
		}
		i++;
	}


	if (!found) {
		printf("������ �л� ������ �����ϴ�.\n");

		fclose(student);
		return;
	}
	
	fclose(student);

	/*
	student.txt�� score.bin�� ������� �ݿ�
	*/

	FILE* score = NULL;
	errno_t err1, err2;

	err1 = fopen_s(&student, "student.txt", "w");
	err2 = fopen_s(&score, "score.bin", "wb");
	if (student == 0 || score == 0)
	{
		printf("File open error.\n");
		return;
	}

	i = 0;

	for (i = 0; i < j; i++) {


		fprintf(student, "%s %d %d\n", scores[i].stu_num, scores[i].math, scores[i].english);


		sum_means[i].sum = func(sum_func);
		sum_means[i].mean = func(mean_func);

		fwrite(&sum_means, sizeof(struct sum_mean), 1, score);

		count = i;

		i++;
	}

	fclose(student);
	fclose(score);
}


void quit()
{
	FILE* score = NULL;
	errno_t err;

	err = fopen_s(&score, "score.bin", "rb");
	if (score == 0)
	{
		printf("File open error.\n");
		return;
	}

	int sum = 0;
	int mean = 0;
	int cnt = 0;
	int stu_score = 0;
	int sum_mean = 0;

	// fread(&sum_means, sizeof(struct sum_mean), MAX, score);

	// �̷��� sum_means[0]�� ����ȴ�!
	while(fread(&sum_means, sizeof(struct sum_mean), 1, score) != 0 ){
		sum += sum_means[0].sum;
		cnt++;
	}

	// cnt or 2*cnt
	mean = sum / (2 * cnt);

	fclose(score);

	printf("�л����� ���� ������ %d���̸�, ����� %d���Դϴ�.\n", sum, mean);
	printf("�� �Լ� �ּ� : %p \t ��� �Լ� �ּ� : %p \n", sum_func, mean_func);
	printf("[����] ���α׷��� �����մϴ�.\n");
}



int func(int (*fp) (int, int))
{

	return (fp(scores[i].math, scores[i].english));

}

int sum_func(int a, int b)
{
	return a + b;
}

int mean_func(int a, int b)
{
	return (a + b)/2;
}



void buffer_clear()
{
	while (getchar() != '\n');
}