#include"support.h"


int myexecu(list*);//각 파이프 라인 handle function
void recurPipe(char *argv[],int);// 파이프 라인 처리 함수
char* rmspace(char *); // 문자열의 맨 앞 공백들 제거
void welcome_message(); // SMUshell 실행 시 웰컴 메세지 출력 함수


int main()
{
    int status;
    char command[100];
    pid_t pid2;
    head = NULL;
    char *bspace=" ";
    
    welcome_message();
    signal(SIGINT, SIG_IGN); // ctrl+c 시그널 무시
  

    printf("SMUshell$");
    //경로 노드에 추가
    addNode("/bin/");
    addNode("/usr/bin/");
    while(1){
        arghead = NULL;
        fflush(stdin); //입력 버퍼 지워주기
        char* cmdBegin;
        
        // 커맨드 라인 읽어오기
        char cmd2[4096];
        fgets(cmd2,4096,stdin);
        
        char* cmd = rmspace(cmd2);// 문자열 앞 공백 제거(쓸데없는 공백 제거)
        int argc = countSpace(cmd) + 1; // (공백 수 세기 + 1) == 명령어의 개수
        char *argv[argc];

        int space;
        space = strcspn(cmd,bspace); // 공백 앞의 문자 수 반환
        
        // 받은 인자 모두 읽기
        char* arg = strtok(cmd, bspace); // 공백을 기준으로 토큰
        int count = 0;
        while (arg){
            argv[count] = arg;
            arg = strtok(NULL, bspace);
              count++;
       }
        count = argc;
        returnMod(argv[count - 1]);// endofline을 endoffile로 수정
        argv[count] = NULL;
        
        // exit, path, cd 명령어 재정의
        if(strcmp(argv[0],"exit") == 0){ // $exit 명령어
            printf("Closing...\n");
            exit(-1);
        }
        if(strcmp(argv[0],"path") == 0){// $path 명령어
                if(count == 1)
                    printNode(); // path 전체 출력
                else if(strcmp(argv[1],"+") == 0)
                    addNode(argv[2]);
                else if(strcmp(argv[1],"-") == 0)
                    delNode(argv[2]);
                 printf("SMUshell$");
                continue;
        }
        if(strcmp(argv[0],"cd") == 0){// $cd 명령어
            if(chdir(argv[1]) < 0)// 디렉토리 변경 작업 오류 작업
                fprintf(stderr, "error: %s\n", strerror(errno));
            printf("SMUshell$");
            continue;
        }
        // 다른 명령어일 경우 fork실행
        if( (pid2 = fork()) < 0){ // fork 실행 실패할 경우
             fprintf(stderr, "error: %s\n", strerror(errno));
             return -1;

        }
        if(pid2 == 0){ // 자식 프로세스 돌아감
             recurPipe(argv,count); // 파이프라인 처리 함수 호출
             exit(0);
        }
        else{ // 부모 프로세스 들어감
            wait(&status);
            printf("SMUshell$");
        }
    }
}

// 파이프 라인 처리 함수
// 이 부분은 아래의 코드를 참고하여 작성하였습니다
//  http://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
void recurPipe(char *argv[], int count){
    int status;
    pid_t pid;
    int pipeflag = 0;
    int i,j,k;
    int count2;
    for(i = 0; i < count; i++) // pipe line이 있을 경우 pipeflag 증가
        if(strcmp(argv[i],"|") == 0)
            pipeflag++;

    int total[pipeflag+2];
    total[0] = 0;
    total[pipeflag + 1] = count;
    int l = 0, m = 1;

    for(; l < count; l++){
        if(strcmp(argv[l],"|") == 0){
            total[m] = l+1;
            m++;
        }
    }
    char **addr;
    char **newaddr;
    int o, n;
    o = n = 0;
              
    if(pipeflag == 0){ // pipe line이 없을 경우
//        printf("no pipe line");
        newaddr = &argv[total[n]];
        o = total[n+1] - total[n];
        addr = shortArray(newaddr,o);
        addNode2(argv[total[n]],addr);
    }
    else{//pipe line이 있을 경우
//        printf("pipe line");
        for(; n < pipeflag + 1; n++){
            newaddr = &argv[total[n]];
            o = total[n+1] - total[n] - 1;
            if(n == pipeflag)
                o++;
            addr = shortArray(newaddr,o);
            addNode2(argv[total[n]],addr);
        }
    }
        
    int p = 0;
    int numPipes = pipeflag;
    int pipefds[2*numPipes];

    // 시작부분에서 parent는 필요한 모든 파이프들을 생성
    for(i = 0; i < (numPipes); i++){
        if(pipe(pipefds + i*2) < 0) {
            perror("couldn't pipe");
            exit(1);
        }
    }
        
    while(arghead){
        pid = fork();
        if(pid == 0){ // 자식 프로세스 돌아감
            if(arghead->next)
                // 첫 번째 명령어가 아닐 경우에 child는 이전의 명령어로부터 인풋값을 받아옴
                if(dup2(pipefds[p*2+1],1) < 0)
                    exit(1);

            if(p != 0){
                // 마지막 명령이 아닌 경우 다음 명령으로 output
                if(dup2(pipefds[(p-1)*2], 0) < 0){
                    perror(" dup2");
                    exit(1);
                }
            }
            // parent는 마지막에 모든 복사본을 닫음
            for(i = 0; i < 2*numPipes; i++)
                close(pipefds[i]);

            if( myexecu(arghead) < 0){
                fprintf(stderr, "error: %s\n", strerror(errno));
                perror(arghead->path);
                exit(1);
            }
        }
        else if(pid < 0){// 포크 실패
            perror("error");
            exit(1);
        }
        arghead = arghead->next;
        p++;
    }
    // parent clost the pipes and wait for children
    for(i = 0; i < 2 * numPipes; i++)
        close(pipefds[i]);
          
    for(i = 0; i < numPipes + 1; i++)
        wait(&status);

}
// 문자열의 앞 공백 제거
// str : "   hi hello" -> rmspace(str) -> str : "hi hello"
char* rmspace(char *raw){
    int temp=0;
    while(raw[temp]==' ')
        temp++;
    return &raw[temp];
}

//각 파이프라인 handle function
int myexecu(list* top){

    list* it=top;
    char **ars = top->arguments;
    int count=0;
    while(ars[count] != NULL)
        count++;

    int breakpoint = count;

    int tcounter = 0;
    while(ars[tcounter] != NULL){
        // 리다이렉션 처리
        if(strcmp(ars[tcounter],">") == 0 || strcmp(ars[tcounter],"<") == 0 || strcmp(ars[tcounter],"2>") == 0){
            breakpoint = tcounter;
            break;
        }
        tcounter++;
    }

    int fileopen, fileread;

    char * argv2[breakpoint + 1];
    for(tcounter = 0; tcounter < breakpoint; tcounter++)
        argv2[tcounter] = ars[tcounter];

    argv2[breakpoint] = NULL;

    for(tcounter = 0; tcounter < count; tcounter++){
        // option > file_name : 파일 쓰기
        if(strcmp(ars[tcounter],">") == 0){
            tcounter++;
            if((fileopen = open(ars[tcounter],(O_CREAT|O_RDWR),0644)) < 0){
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(-1);
            }
            else{
                if(dup2(fileopen,1) < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(-1);
                    
                }
            }
        }
        // option < file_name : 파일 읽기
        if(strcmp(ars[tcounter],"<") == 0){
                tcounter++;
                if((fileopen = open(ars[tcounter],O_RDONLY)) < 0){
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(-1);
                }
                else{
                    if(dup2(fileopen,0) < 0) {
                        fprintf(stderr, "error: fail to redirect\n");
                        exit(-1);
                    }
                }
        }
        // 2>&1
        // 표준에러를 표준출력으로 리다이렉션
        if(strcmp(ars[tcounter],"2>") == 0){
                tcounter++;
                if((fileopen = open(ars[tcounter],(O_CREAT|O_RDWR),0644)) < 0){
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(-1);
                }
                else{
                    if(dup2(fileopen,2) < 0) {
                        fprintf(stderr, "error: %s\n", strerror(errno));
                        exit(-1);
                    }
                }
        }
    }

          
    //시스템에서 제공하는 함수들
    int retv = 0;
    if(head == NULL){
        fprintf(stderr, "error: No path to execute command\n");
        exit(0);
    }
    list* iter = head;
    char* command = iter->path;
    strcat(command,top->path);

           
    while((retv = execv(command,argv2) == -1) && iter != NULL){
        iter = iter->next;
        command = iter->path;
        if(command == NULL) {
            fprintf(stderr, "Command %s not found.\n",ars[0]);
            exit(0);
        }
        strcat(command,ars[0]);
    }
    close(fileopen);
    exit(0);
            

}
void welcome_message(){
    char message[50] = "Hi, welcome to SMU Shell!";
    printf(
"< %s >\n"
"\t\t\\\n"
"\t\t \\   \\_\\_    _/_/         Version 1.1  \n "
"\t\t  \\      \\__/              Author. jimin An \n "
"\t\t   \\     (oo)\\_______       UNI ID. 201810954  \n "
"\t\t    \\    (__)\\       )\\/\\    Date. 2020/11/07 \n"
"\t\t             ||----- |\n"
"\t\t             ||     ||\n",message);
    printf("\n");
}
