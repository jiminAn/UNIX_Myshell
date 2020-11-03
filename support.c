#include"support.h"

list *head;
list *arghead;

// 연결 리스트 역순으로 출력
void revOrder(char *strp[], int num){
    num--;
    for(int i = 0; i < num/2; i++){
        char *temp = strp[i];
        strp[i] = strp[num-i];
        strp[num-i] = temp;
    }
    
}
// 문자열 저장 후 해당 문자열 리턴
char** shortArray(char *str[], int len){
    char** sarray=(char**)malloc((len + 1) * sizeof(char*));
    int i=0;
    for(; i<len; i++)
        sarray[i] = str[i];
    
    sarray[i] = NULL;
    return sarray;
}
//연결리스트 순회하며 path값 출력
void printNode(){
    list* it = head;
    while(it != NULL){
        printf("%s:",it->path);
        it = it->next;
    }
    printf("\n");
}

//arguments list를 위한 출력 함수
void printNode2(){
    list* it = arghead;
    while(it != NULL){
        printf("%s:",it->path);
        it = it->next;
    }
    printf("\n");
}


//path list에 노드 추가
void addNode(char*str){

    list* next = (list*)malloc(sizeof(list));
    strncpy(next->path, str, 100);
    next->next = NULL;
    if(head == NULL){
        head = next;
        return;
    }
    list* it = head;

    while(it->next != NULL)
        it = it->next;
  
    it->next = next;
}

//argument list에 노드 추가
void addNode2(char*str,char ** arg){

    list* next = (list*)malloc(sizeof(list));
    strncpy(next->path, str, 100);
    next->arguments = arg;
    next->next = NULL;
    if(arghead == NULL){
        arghead = next;
        return;
    }
    list* it = arghead;

    while(it->next != NULL)
        it = it->next;
    
    it->next = next;
    
}

//path list에서 노드 삭제
void delNode(char *str){
    list* it = head;
    if(head == NULL) {
          fprintf(stderr, "error: path %s not found\n", str);
          return;
    }
    if(strcmp(head->path, str) == 0)
    {
        head=head->next;
        free(it);
    }
    else
    {
        while(it->next != NULL && strcmp(it->next->path, str) != 0 )
            it = it->next;

        if(it->next == NULL || strcmp(it->next->path, str) != 0)
             fprintf(stderr, "error: path %s not found\n", str);
        else
        {
            it->next = it->next->next;
            free(it->next);
        }
    }
}

//argument list에서 노드 삭제
void delNode2(char *str,char ** arg){
    list* it = head;
    if(head == NULL){
        printf("No node!");
        return;
    }
    if(strcmp(head->path, str) == 0)
    {
        head = head->next;
        free(it);
    }
    else
    {
        while(strcmp(it->next->path, str) != 0 && it != NULL)
            it = it->next;
            
        if(it == NULL)
            printf("Path Not Found!\n");
        else
        {
            it->next = it->next->next;
            free(it->next);
        }
    
    }

}

//이전 string에서 substring 만들기
char* substring(char* str,int a,int b){
    char* ret = (char*)malloc((b - a + 2)*sizeof(char));
    int j=0;
    for(int i = a; i <= b; i++){
        ret[j]=str[i];
        j++;
    }
    ret[j] = '\0';
    return ret;

}

//공백 수 세기
int countSpace(char * str){
    int j = 0;
    int i = 0;
    for(; str[i] != '\0'; i++)
        if(str[i] == ' ' && str[i-1] != ' ' && str[i+1] != '\n')
            j++;
    return j;

}

// endofline을 endoffile로 수정
void returnMod(char* str){
    while(1){
        if(*str == '\n'){
            *str = '\0';
            return;
        }
        str++;
    }
}
