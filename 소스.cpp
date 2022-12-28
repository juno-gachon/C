//#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#define MAX 50 // 최대 학생 수
#define STUNUM_LEN 6 // 학생번호 길이

struct stu_score {
	char stu_num[STUNUM_LEN + 1];
	int math;
	int english;
};

// 구조체 배열은 점수 저장용 버퍼
struct stu_score scores[MAX];

struct sum_mean {
	int sum;
	int mean;
};

// 종료 시 출력될 학생의 점수 총합 및 평균 저장
struct sum_mean sum_means[MAX];


/*
	학생 번호(알파벳 및 숫자 6글자), 수학 점수(0~100), 영어점수(0~100)를 입력받아
	입력, 삭제, 종료를 수행하는 프로그램

	종료를 수행할 때는 함수 포인터를 사용한다
*/

void printMenu(); // 메뉴 출력

void insert();	// 학생 정보 입력 - student.txt와 score.bin에 반영
void del();		// 학생 정보 삭제 - student.txt와 score.bin에 반영
void quit();	// 프로그램 종료 - score.bin의 점수 총합 및 평균 출력

// 함수 포인터를 이용하여 구현
// 학생의 점수 총합 및 평균 구하는 데 사용
int func(int (*) (int, int));

// func에 사용되는 함수 - (학생별 수학점수와 영어점수의) 합, 평균 구하기
int sum_func(int, int);
int mean_func(int, int);

// 입력된 학생 정보의 수 등으로 쓰이며 반복문 등에서 사용
int cnt = 0;

// 버퍼 클리어용 함수
void buffer_clear();

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
	if (err1 != 0 || err2 != 0)
	{
		printf("File open error.");
		return;
	}

	// 입력할 때 마다 이 변수는 초기화를 해주자
	cnt = 0;

	printf("[입력] 학생 번호, 수학 점수, 영어점수를 빈칸으로 구별하며 입력하세요. 참고로, 각 줄의 끝은 줄바꿈 문자 '\\n'로 끝납니다.\n");
	printf("입력을 종료하시려면 엔터를 눌러주세요.\n");
	

	//	while (1) { // 이거 대신 for 구문 쓰기로

	//	}
	for (cnt ; cnt < MAX ; cnt++) {
		// buffer_clear(); // 여기다 하면 안됨!
		printf("학생 번호: 알파벳 및 숫자 6글자\n");
		printf(">> ");
		// + 1 필요
		fgets(scores[cnt].stu_num, STUNUM_LEN + 1, stdin);
		// 여기서 "0"이 stu_num에 저장되더라도 파일에 쓰기 전에 나가니 문제 X

		/*
		 -> 이거 제대로 동작을 안함 ???????
		 "0" 대신 "0\n"을 넣어야 잘 동작 - WHY?????

		*
		* \0이 아니라 \n이었잖아아아아아아아ㅏ아아ㅏ
		*
		*/

		if (strcmp(scores[cnt].stu_num, "\n") == 0) {
			break;
		}


		printf("수학점수, 영어점수를 입력하세요(빈칸으로 구분) : \n");
		scanf_s("%d %d", &scores[cnt].math, &scores[cnt].english);

		// 이렇게 하면 stu_input_cnt를 전역변수로 설정해야 하는데 이게 좋은 방법인지는 잘 모르겠다
		sum_means[cnt].sum = func(sum_func);
		sum_means[cnt].mean = func(mean_func);

		// (한번에 저장해도 되지만) 한명씩 저장 -> 그냥 입력받는 부분하고 저장하는 부분 분리
		// fwrite(&sum_means, sizeof(struct sum_mean), 1, score);

		buffer_clear();
	}

	for (int i = 0; i < cnt; i++) {
								//\n이 있어야 score.txt에 한 줄씩 입력된다
		fprintf(student, "%s %d %d\n", scores[i].stu_num, scores[i].math, scores[i].english);
	}
	
	fwrite(&sum_means, sizeof(struct sum_mean), cnt, score);

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
	char stu_num[STUNUM_LEN + 1] = ""; // 마지막에 \0이 저장되기에 + 1
	fgets(stu_num, STUNUM_LEN + 1, stdin); // 여기서도 마찬가지로 STUNUM_LEN을 하면 5글자만 입력받고 마지막은 \0이 되기에 + 1
	buffer_clear();

	bool found = false;

	cnt = 0;
	int new_cnt = 0;


	// int a = fscanf_s(student, "%s %d %d", scores[i].stu_num, STUNUM_LEN, &scores[i].math, &scores[i].english);

										//  &scores_read[i].stu_num보다는 이게 맞을 듯
	//	while (fscanf_s(student, "%s %d %d", scores_read[i].stu_num, STUDENTNUM_LEN, &scores_read[i].math, &scores_read[i].english) != EOF) {
//  while (fgets(str, MAX, student) != 0) {
//	 	sscanf_s(str, "%s %d %d", scores_read[i].stu_num, STUDENTNUM_LEN, &scores_read[i].math, &scores_read[i].english);
	/*****
	*		혹시나 했는데 STUNUM_LEN + 1을 해야 정상적으로 동작한다!!! - 2022년 12월 27일 오후 6시 29분
	*****/
	// printf("%d", fscanf_s(student, "%s %d %d", scores[i].stu_num, STUNUM_LEN + 1, &scores[i].math, &scores[i].english));

	while (fscanf_s(student, "%s %d %d", scores[cnt].stu_num, STUNUM_LEN + 1, &scores[cnt].math, &scores[cnt].english) != EOF) {

		if (strcmp(scores[cnt].stu_num, stu_num) == 0) {
			found = true;
			cnt++; // 이거 빼먹지 말자! <- 사실 빼먹어도 딱히 달라지는 건 없을 듯 어차피 중요하지 않으니
			continue;
		}
		else {
			// scores 구조체 배열에다가 그대로 저장
			strcpy_s(scores[new_cnt].stu_num, scores[cnt].stu_num);
			scores[new_cnt].math = scores[cnt].math;
			scores[new_cnt].english = scores[cnt].english;
			
			new_cnt++;
		}
		cnt++;
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

	for (cnt = 0; cnt < new_cnt; cnt++) {
		// fprintf(student, "%s %d %d\n", scores[cnt].stu_num, scores[cnt].math, scores[cnt].english); // 얘 때문에 중복되어 저장됨...

		sum_means[cnt].sum = func(sum_func);
		sum_means[cnt].mean = func(mean_func);
	}

	// 파일에 저장하는 부분
	// student.txt
	for (int i = 0; i < new_cnt; i++) {
		fprintf(student, "%s %d %d\n", scores[i].stu_num, scores[i].math, scores[i].english);
	}

	// score.bin
	fwrite(&sum_means, sizeof(struct sum_mean), new_cnt, score);

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

	// fread(&sum_means, sizeof(struct sum_mean), MAX, score);

	/*while(fread(&sum_means, sizeof(struct sum_mean), 1, score) != 0 ){
		sum += sum_means.sum;
		cnt++;
	}*/
						// &sum_means가 아니라
	for (int i = 0; fread(&sum_means[i], sizeof(struct sum_mean), 1, score) != 0 ; i++) {
		sum += sum_means[i].sum;
		cnt++;
	}

	// cnt or 2*cnt - 각 학생 점수 총합의 평균 or 각 학생 각 점수의 평균
	mean = sum / (2 * cnt);

	fclose(score);

	printf("학생들의 점수 총합은 %d점이며, 평균은 %d점입니다.\n", sum, mean);
	printf("합 함수 주소 : %p \t 평균 함수 주소 : %p \n", sum_func, mean_func);
	printf("[종료] 프로그램을 종료합니다.\n");
}



int func(int (*fp) (int, int))
{
	//scores구조체배열의 stu_input_cnt번째 원소에 저장된 수학점수와 영어점수를 가지고 연산 수행
	return fp(scores[cnt].math, scores[cnt].english);

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
