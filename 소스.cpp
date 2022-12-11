//#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#define MAX 50 // 최대 학생 수
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

// 점수를 구조체 배열에 저장
struct stu_score scores[MAX];
struct stu_score scores_read[MAX];

struct sum_mean sum_means[MAX];

/*
	학생 번호(알파벳 및 숫자 6글자), 수학 점수(0~100), 영어점수(0~100)를 입력받아
	입력, 삭제, 종료를 수행하는 프로그램
*/

void printMenu(); // 메뉴 출력

void insert();	// 학생 정보 입력 - student.txt와 score.bin에 반영
void del();		// 학생 정보 삭제 - student.txt와 score.bin에 반영
void quit();	// 프로그램 종료 - scoe.bin의 수학 점수 총합 및 평균 출력

int func(int (*) (int, int));

int sum_func(int, int);
int mean_func(int, int);


// 버퍼 클리어용 함수
void buffer_clear();

int i = 0;
int count = 0;

int main()
{
	printf("학생 정보를 입력받아 입력, 삭제, 종료를 수행하는 프로그램입니다.\n");
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
			printf("다시 입력해주세요.\n");
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
	printf("메뉴를 입력하세요: i(입력), d(삭제), q(종료)\n");
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

	printf("[입력] 학생 번호, 수학 점수, 영어점수를 빈칸으로 구별하며 입력하세요. 참고로, 각 줄의 끝은 줄바꿈 문자 '\\n'로 끝납니다.\n");
	
	while (1) {
		// buffer_clear(); // 여기다 하면 안됨!
		printf("학생 번호: 알파벳 및 숫자 6글자(0이면 종료)\n");
		printf(">> ");
								// + 1 필요
		fgets(scores[i].stu_num, STUDENTNUM_LEN + 1, stdin);
		// 여기서 "0"이 stu_num에 저장되더라도 파일에 쓰기 전에 나가니 문제 X

		// -> 이거 제대로 동작을 안함 ???????
		// "0" 대신 "0\n"을 넣어야 잘 동작 - WHY?????

		/*******************************************
		*
		* \0이 아니라 \n이었잖아아아아아아아ㅏ아아ㅏ
		*
		********************************************/

		if (strcmp(scores[i].stu_num, "0\n") == 0) {
			break;
		}


		printf("수학점수, 영어점수을 입력하세요 : ");
		scanf_s("%d %d", &scores[i].math, &scores[i].english);

		fprintf(student, "%s %d %d\n", scores[i].stu_num, scores[i].math, scores[i].english);

		sum_means[i].sum = func(sum_func);
		sum_means[i].mean = func(mean_func);

		// (한번에 저장해도 되지만) 한명씩 저장
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

	printf("[삭제] 삭제할 학생 번호를 입력하세요.\n");
	printf("학생 번호>> ");
	char stu_num[MAX] = "";
	fgets(stu_num, STUDENTNUM_LEN + 1, stdin);
	buffer_clear();

	int new_stu_num = 0;
	char str[MAX] = "";
	bool found = false;

	i = 0;
	int j = 0;
										//  &scores_read[i].stu_num보다는 이게 맞을 듯
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
		printf("삭제할 학생 정보가 없습니다.\n");

		fclose(student);
		return;
	}
	
	fclose(student);

	/*
	student.txt와 score.bin에 변경사항 반영
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

	// 이러면 sum_means[0]만 변경된다!
	while(fread(&sum_means, sizeof(struct sum_mean), 1, score) != 0 ){
		sum += sum_means[0].sum;
		cnt++;
	}

	// cnt or 2*cnt
	mean = sum / (2 * cnt);

	fclose(score);

	printf("학생들의 점수 총합은 %d점이며, 평균은 %d점입니다.\n", sum, mean);
	printf("합 함수 주소 : %p \t 평균 함수 주소 : %p \n", sum_func, mean_func);
	printf("[종료] 프로그램을 종료합니다.\n");
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