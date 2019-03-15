#include "proj1.h"
#define READ_END 0
#define WRITE_END 1
//global variables
int wstatus = -1;
unsigned short cmdflags;	//set if  PIPE is specified
long pos;			//for going back after counting the number of pipes
//function prototypes
int BGExec(char**execvbuf);
int pipeExec(int infile, int outfile, char*cmd);

int main(int argc,char**argv){		
	char  inbuf[LINE_MAX];      	//hold the current command
	char* execvbuf[LINE_MAX];	//current command after processing

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
	//read the file, executing each line as per spec
	while(fgets(inbuf,LINE_MAX, fp)!=NULL){
		fflush(stdin);
		fflush(stdout);
		fflush(stderr);
		int wstatus;
		pid_t pid;

		//null-terminate
		inbuf[strlen(inbuf)-1]='\0';
		int numPipes=0;
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

		//if pipe is specified
		if(execvbufindex>2&&!strncmp(execvbuf[execvbufindex-2],"PIPE",4)){
			execvbuf[execvbufindex-2]='\0';
			if(!cmdflags)	pos = ftell(fp);
			cmdflags = 1;
			numPipes++;

			continue;
		}

		//if pipe was specified in a previous command
		if(cmdflags){
			long tmp = ftell(fp);
			fseek(fp,pos,SEEK_SET);
			pos = tmp;
			int pipes[numPipes][2];
			for(int i = 0; i < numPipes; i++)
				//create pipes for each command in the chain
				if(!pipe(pipes[i])){
					log_err("unable to pipe on the %dth pipe",i);
					return -1;
				}
		//allow the first and last commands in the chain to read 
			//pass each command along to a helper subroutine, including the last in the chain 
			if(!fgets(inbuf,LINE_MAX,fp)){
				log_err("unable to re-get the first command in a chain");
				return -1;
			}
			if(!pipeExec(0,pipes[0][READ_END],inbuf)){
				log_err("unable to exec %s as the first cmd in a chain",inbuf);
				return -1;
			}
			for(int i = 1; i < numPipes-1;i++){
				if(!fgets(inbuf,LINE_MAX,fp)){
					log_err("unable to re-get the %dth commandin a chain",i);
					return -1;
				}
				if(!pipeExec(pipes[i][READ_END],pipes[i][WRITE_END],inbuf)){
					log_err("unable to exec %s as the %dth cmd in a chain",inbuf,i);
					return -1;
				}
			}
			if(!fgets(inbuf,LINE_MAX,fp)){
				log_err("unable to re-get the first command in a pipe chain");
				return -1;
			}
			if(!pipeExec(pipes[numPipes-1][WRITE_END],1,inbuf)){
				log_err("couldn't exec %s as last cmd in a pipe chain",inbuf);
				return -1;
			}
			cmdflags = 0;
			numPipes = 0;
			continue;
		}
		//no option specified
		switch(pid=fork()){
			case -1:log_err("fork");
				return 1;
			case 0:	
				execvp(execvbuf[0],execvbuf);
				//execution should never reach this point
				log_err("exec with arguments %s", inbuf);
				return 1;
			default:waitpid(pid,&wstatus,0);
				if(wstatus){
					log_err("child returned unsuccessfully with value");
					return 1;
				}
				break;
		}
		fflush(stdin);
	}return 0;
}
//runs a process in the background
int BGExec(char**execvbuf){
	if(!freopen("/dev/null","w",stdout))
		log_err("BGExec freopen");
	if(!execvp(execvbuf[0],execvbuf))
		log_err("BGExec exec");
	//execution should never reach this point
	log_err("bgexec");
	return 1;
}
//runs a process with input and output redirected to the specified filedescriptor
int pipeExec(int infile, int outfile, char*cmd){
	int pid = -1;
	int wstatus;
	int execvbufindex = 0;
	char* execvbuf[LINE_MAX];
	//current command after processing
	execvbuf[execvbufindex++]=strtok(cmd," ");
	while(execvbuf[execvbufindex++]=strtok(NULL," "));
	if(execvbufindex>2&&!strncmp(execvbuf[execvbufindex-2],"PIPE",4))
		execvbuf[execvbufindex-2]='\0';
	//fork-dup2-exec
	switch(pid=fork()){
		case -1:log_err("fork in pipe");
			return 1;
		case 0://child
			if((!dup2(infile,0))||(!dup2(outfile,1)))
				log_err("dup2 %i,%i",infile,outfile);
			if(!execvp(execvbuf[0],execvbuf))
				log_err("pipeExec exec %s",cmd);
			//execution should never reach this point
			log_err("pipeExec %s",cmd);
			return 1;
		default:
			waitpid(pid,&wstatus,0);
			close(infile);
			close(outfile);
	}
	fflush(stdin);
	fflush(stdout);
	fflush(stderr);
	return 0;
}
