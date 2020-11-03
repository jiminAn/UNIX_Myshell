#include <stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// 경로를 저장해주기 위한 연결리스트 자료구조
typedef struct node{
    char path[100];
    char** arguments;
    struct node *next;
}list;

extern list *head;
extern list *arghead;

char **shortArray(char *str[],int);// 문자열 저장 후 해당 문자열 리턴
void printNode(); //연결리스트 순회하며 path값 출력
void addNode(char* str); //path list에 노드 추가
void delNode(char* str);//path list에서 노드 삭제
void addNode2(char* str,char **);//argument list에 노드 추가
void delNode2(char* str,char **);//argument list에서 노드 삭제
void printNode2();//arguments list를 위한 출력 함수
int countSpace(char * str);//공백 수 세기
char *substring(char* ,int,int);//이전 string에서 substring 만들기
void returnMod(char* str);// endofline을 endoffile로 수정
void revOrder(char *strp[],int); // 연결 리스트 역순으로 출력
