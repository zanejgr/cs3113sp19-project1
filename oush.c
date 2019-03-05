#include "proj1.h"
#define READ_END 0
#define WRITE_END 1
#define BGFlag 1
#define PIPEFlag 2
int wstatus = -1;
int BGExec(char**execvbuf);
int main(int argc,char**argv){		
	char  inbuf[LINE_MAX];      	//hold the current command
	char* execvbuf[LINE_MAX];	//current command after processing
	unsigned short cmdflags;	//set if BG or PIPE is specified

	FILE* fp = stdin;		//batchfile, or stdin if none
	while(fgets(inbuf,LINE_MAX, fp)){
		int pipefd[2][2];	//2 pipes, one for previous and one for next process
		int wstatus;
		pid_t childpid;

		//null-terminate
		inbuf[strlen(inbuf)-1]='\0';
		int execvbufindex=0;
		execvbuf[execvbufindex++]=strtok(inbuf," ");
		//process input line
		while(execvbuf[execvbufindex++]=strtok(NULL," "));
		//there's probably an error below. delete this comment if it works
		if(!strncmp(execvbuf[execvbufindex-2],"BG",2)){
			execvbuf[execvbufindex-2]='\0';
			perror("going into bg");
			BGExec(execvbuf);
			continue;
		}
		if(!strncmp(execvbuf[execvbufindex-4],"PIPE",4))
			cmdflags|=PIPEFlag;
		switch(childpid=fork()){
			case -1:log_err("fork");
				return 1;
			case 0:	
				execvp(execvbuf[0],execvbuf);
				//execution should never reach this point
				log_err("exec");
				return 1;
			default:waitpid(childpid,&wstatus,0);
				break;
		}
		fflush(stdout);
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

