#include "proj1.h"
#define READ_END 0
#define WRITE_END 1

//function prototypes
int BGExec(char**execvbuf);

int main(int argc,char**argv){		
	char  inbuf[LINE_MAX];      	//hold the current command
	char* execvbuf[LINE_MAX];	//current command after processing
	int wstatus = -1;
	unsigned short cmdflags;	//set if BG or PIPE is specified

	//open the batchfile
	if(argc!=2){
		log_err("invalid number of arguments");
		return 1;
	}	
	FILE* fp = fopen(argv[1],"r");
	if(!fp){
		log_err("Invalid argument %s",argv[1]);
		return 1;
	}		
	while(fgets(inbuf,LINE_MAX, fp)!=NULL){
		int pipefd[2][2];	//2 pipes, one for previous and one for next process
		int wstatus;
		pid_t childpid;

		//null-terminate
		inbuf[strlen(inbuf)-1]='\0';
		int execvbufindex=0;
		execvbuf[execvbufindex++]=strtok(inbuf," ");

		//process input line
		while(execvbuf[execvbufindex++]=strtok(NULL," "));

		//BG option
		if(execvbufindex>2&&!strncmp(execvbuf[execvbufindex-2],"BG",2)){
			execvbuf[execvbufindex-2]='\0';
			BGExec(execvbuf);
			continue;
		}

		//no option specified
		switch(childpid=fork()){
			case -1:log_err("fork");
				return 1;
			case 0:	
				execvp(execvbuf[0],execvbuf);
				//execution should never reach this point
				log_err("exec with arguments %s", inbuf);
				return 1;
			default:waitpid(childpid,&wstatus,0);

				if(wstatus){
					log_err("child returned unsuccessfully with value");
					return 1;
				}
				break;
		}
	}return 0;
}
int BGExec(char**execvbuf){
	if(!freopen("/dev/null","w",stdout))
		log_err("BGExec freopen");
	if(!execvp(execvbuf[0],execvbuf))
		log_err("BGExec exec");
	//execution should never reach this point
	log_err("bgexec");
	return 1;
}

