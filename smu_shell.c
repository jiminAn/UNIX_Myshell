#include"support.h"


int myexecu(list*);
void recurPipe(char *argv[],int);
char* rmspace(char *);
void welcome_message();

int main()
{
    int status;
    char command[100];
    pid_t pid2;
    head = NULL;
    char *bspace=" ";
    
    welcome_message();
  

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
        
        if(strcmp(argv[0],"exit")==0){ // $exit 명령어
            printf("Closing...\n");
            exit(-1);
        }
       if(strcmp(argv[0],"path")==0){// $path 명령어
                if(count==1)
                    printNode(); // path 전체 출력
                else if(strcmp(argv[1],"+")==0)
                    addNode(argv[2]);
                else if(strcmp(argv[1],"-")==0)
                    delNode(argv[2]);
                 printf("SMUshell$");
                continue;
        }
        if(strcmp(argv[0],"cd")==0){// $cd 명령어
            if(chdir(argv[1])<0)
                fprintf(stderr, "error: %s\n", strerror(errno));
            printf("SMUshell$");
            continue;
        }

        if( (pid2=fork()) < 0){ // fork 실행 실패
             fprintf(stderr, "error: %s\n", strerror(errno));
             return -1;

        }
        if(pid2 == 0){ // 자식 프로세스 돌아감
             recurPipe(argv,count);
             exit(0);
        }
        else{ // 부모 프로세스 들어감
            wait(&status);
            printf("SMUshell$");
        }
    }
}

// 이 부분은 아래의 코드를 참고하여 작성하였습니다
// http://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
void recurPipe(char *argv[],int count){   
        int status;
        pid_t pid;
        int pipeflag=0;
        int i,j,k;
        int count2;
        for(i=0;i<count;i++){
            if(strcmp(argv[i],"|")==0)
            {
                  pipeflag++;
            }
        }
   


        int total[pipeflag+2];
        total[0]=0;
        total[pipeflag+1]=count;
        int l=0,m=1;

        for(;l<count;l++){
            if(strcmp(argv[l],"|")==0){
                total[m]=l+1;
                m++;
            }
        }
        char **addr;
        char **newaddr;
        int o=0;
        int n=0;
        
        
        
        if(pipeflag==0){
            newaddr=&argv[total[n]];
            o=total[n+1]-total[n];
            addr=shortArray(newaddr,o);
             addNode2(argv[total[n]],addr);

        }
        else{
            for(;n<pipeflag+1;n++){
              newaddr=&argv[total[n]];
              o=total[n+1]-total[n]-1;
              if(n==pipeflag) o++;
              addr=shortArray(newaddr,o);
              addNode2(argv[total[n]],addr);
          }
        }
         
        int p=0;
        int numPipes=pipeflag;
        int pipefds[2*numPipes];

        for(i = 0; i < (numPipes); i++){
            if(pipe(pipefds + i*2) < 0) {
                perror("couldn't pipe");
                exit(1);
            }
        }
        
        while(arghead){
            pid=fork();
            if(pid==0){
                if(arghead->next){
                    if(dup2(pipefds[p*2+1],1)<0){
                        exit(1);

                    }
                }
        

            if(p!=0){
                if(dup2(pipefds[(p-1)*2], 0) < 0){
                    perror(" dup2");
                    exit(1);

                }
            }


            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }
         
            if( myexecu(arghead)<0)
            {       
                     fprintf(stderr, "error: %s\n", strerror(errno));

                    perror(arghead->path);
                    exit(1);
                
            }
            }
            else if(pid<0){
                perror("error");
                exit(1);
            }
            arghead = arghead->next;
            p++;
        }
              for(i = 0; i < 2 * numPipes; i++){
               close(pipefds[i]);
            }

            for(i = 0; i < numPipes + 1; i++)
                wait(&status);



}
// 문자열의 앞 공백 제거
// str : "   hi hello" -> rmspace(str) -> str : "hi hello"
char* rmspace(char *raw){
    int temp=0;
    while(raw[temp]==' '){temp++;}
    return &raw[temp];
}

//function to handle each pipeline
int myexecu(list* top){

        list* it=top;
        char **ars=top->arguments;
        int count=0;
        while(ars[count]!=(char*)0){
            count++;
        }
        int breakpoint=count;

        int tcounter=0;
        while(ars[tcounter]!=(char*)0){
             if(strcmp(ars[tcounter],">")==0 ||strcmp(ars[tcounter],"<")==0 ||strcmp(ars[tcounter],"2>")==0)
             {
                  breakpoint=tcounter;
                  break;
              }
             tcounter++;

        }

        int fileopen;
        int fileread;

        char * argv2[breakpoint+1];
        for(tcounter=0;tcounter<breakpoint;tcounter++){
           argv2[tcounter]=ars[tcounter]; 
        }
        argv2[breakpoint]=(char*)0;

        for(tcounter=0;tcounter<count;tcounter++){
                if(strcmp(ars[tcounter],">")==0)
                {
                
                    tcounter++;
                    if((fileopen=open(ars[tcounter],(O_CREAT|O_RDWR),0644))<0){ 
                         fprintf(stderr, "error: %s\n", strerror(errno));
                         exit(-1);
                    }
                    else{
                        if(dup2(fileopen,1) < 0) {
                             fprintf(stderr, "error: %s\n", strerror(errno));
                             exit(-1);                        }
                    }
    
                }
                
                if(strcmp(ars[tcounter],"<")==0)
                {
                    tcounter++;
                    if((fileopen=open(ars[tcounter],O_RDONLY))<0){ 
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
    
                if(strcmp(ars[tcounter],"2>")==0)
                {
                
                    tcounter++;
                    if((fileopen=open(ars[tcounter],(O_CREAT|O_RDWR),0644))<0){ 
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

          

            //system provided functions
            int retv=0;
            if(head==NULL){
                  fprintf(stderr, "error: No path to execute command\n");
                  exit(0);
            }
            list* iter=head;
            char* command=iter->path;
            strcat(command,top->path);

           
            while((retv=execv(command,argv2)==-1)&&iter!=NULL){
                iter=iter->next;
                command=iter->path;
                if(command==NULL) {
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
"\t\t \\   \\_\\_    _/_/         Version 1.0  \n "
"\t\t  \\      \\__/              Author. jimin An \n "
"\t\t   \\     (oo)\\_______       UNI ID. 201810954  \n "
"\t\t    \\    (__)\\       )\\/\\    Date. 2020/11/05 \n"
"\t\t             ||----- |\n"
"\t\t             ||     ||\n",message);
    printf("\n");
}
