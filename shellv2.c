#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define BLOCKSIZE 16

typedef struct list{
	char *word;
	struct list *next;
} list;

typedef struct redirection{
	const char *input_file;
	const char *output_file;
	int input_fd;
	int output_fd;
	int append;
	int error;
} redirection ;

char *createString(){
	char *string=NULL;
	int ch,i=0,mul=0;
	ch=getchar();
	while (ch!=EOF && ch!='\n'){
		if (i==mul*BLOCKSIZE){
			mul++;
			string=realloc(string,mul*BLOCKSIZE+1);
		}
		string[i]=ch;
		i++;
		ch=getchar();
	}
	if (ch==EOF){
		free(string);
		return NULL;
	}
	if (!string){
		string=malloc(1);
	}
	string[i]='\0';
	return string;
}

list *addToList(list *main,const char *parsword){
	list *tmp;
	if (main){
		tmp=main;
		while (tmp->next){
		tmp=tmp->next;
	}
	tmp->next=malloc(sizeof(list));
	tmp=tmp->next;
	}else{
		main=malloc(sizeof(list));
		tmp=main;
	}
	tmp->word=strdup(parsword);
	tmp->next=NULL;
	return main;
}

int sizeList(list *parslist){
	int i=0;
	while (parslist){
		i++;
		parslist=parslist->next;
	}
	return i;
}

int checkQuotes(const char *string){
	int i,qcount=0;
	for (i=0; string[i]; i++){
		if (string[i]=='"'){
			qcount++;
		}
	}
	return qcount%2;
}

int specialSymbol(const char *string,int current,list **parslist){
	switch (string[current]){
		case '&':
			if (string[current+1]=='&'){
				*parslist=addToList(*parslist,"&&");
				return current+2;
			}
			*parslist=addToList(*parslist,"&");
			return current+1;
		case '|':
			if (string[current+1]=='|'){
				*parslist=addToList(*parslist,"||");
				return current+2;
			}
			*parslist=addToList(*parslist,"|");
			return current+1;
		case '>':
			if (string[current+1]=='>'){
				*parslist=addToList(*parslist,">>");
				return current+2;
			}
			*parslist=addToList(*parslist,">");
			return current+1;
		case '<':
			*parslist=addToList(*parslist,"<");
			return current+1;
		case ';':
			*parslist=addToList(*parslist,";");
			return current+1;
		case '(':
			*parslist=addToList(*parslist,"(");
			return current+1;
		case ')':
			*parslist=addToList(*parslist,")");
			return current+1;
		default:
			return current;
	}
}

int delimiter(int sym){
	return (sym=='\0'||sym=='<'||sym=='&'||sym=='|'||
			sym=='\t'||sym=='>'||sym==';'||sym==' ');
}

list *parsing(const char *string){
	int i=0,j;
	char parsword[strlen(string)+1];
	list *parslist=NULL;
	if (checkQuotes(string)){
		printf("quotes are not balanced\n");
		return NULL;
	}
	while (string[i]!='\0'){
		j=0;
		while (!delimiter(string[i])){
			while(string[i]=='"'){
				i++;
				while(string[i]!='"'){
					parsword[j]=string[i];
					j++;
                                        i++;
                                }
                                i++;
                        }
                        if (!delimiter(string[i])){
                                parsword[j]=string[i];
                                j++;
                                i++;
                        }
                }
                parsword[j]='\0';
                if (parsword[0]){
                        parslist=addToList(parslist,parsword);
                }
                i=specialSymbol(string,i,&parslist);
                if (string[i]==' ' || string[i]=='\t'){
                         i++;
                }
        }
        return parslist;
}

int countProg(list *parslist){
        int result=1;
        while (parslist){
                if (strcmp(parslist->word,"|")==0){
                        result++;
                }
                parslist=parslist->next;
        }
        return result;
}

int wordsBeforePipe(list *parslist){
	int i=0;
	while (parslist && strcmp(parslist->word,"|")){
		i++;
		parslist=parslist->next;
	}
	return i;
}

char **listToMassive(list **parslist,int prog){
	int i=0;
	char **result;
	result=malloc((wordsBeforePipe(*parslist)+1)*sizeof(char*));
	while (*parslist && strcmp(((*parslist)->word),"|")){
		result[i]=strdup((*parslist)->word);
		*parslist=(*parslist)->next;
		i++;
	}
	result[i]=NULL;
	if (*parslist){
		*parslist=(*parslist)->next;
	}
	return result;
}

void printWords(char **mass){
	int i;
	for (i=0; mass[i]; i++){
		printf("%s\n",mass[i]);
	}
}

void freeList(list *main){
	list *tmp;
	while (main){
		tmp=main;
		main=main->next;
		free(tmp->word);
		free(tmp);
	}
}

int isArrow(list *tmp){
	char *str=tmp->word;
	return (!strcmp(str,">")||!strcmp(str,"<")||!strcmp(str,">>"));
}

list *deleteElem(list *main,list *del){
	list *tmp;
	if (main==del){
		main=main->next;
		free(del->word);
		free(del);
	}else{
		tmp=main;
		while (tmp->next!=del){
			tmp=tmp->next;
		}
		tmp->next=del->next;
		free(del->word);
		free(del);
	}
	return main;
}

list *deleteArrows(list *main){
	char mask[sizeList(main)];
	list *tmp,*del;
	int i,flag=0;
	tmp=main;
	for(i=0;tmp;i++){
		mask[i]=0;
		if (flag){
			flag=0;
			mask[i]=1;
		}
		if (isArrow(tmp)){
			flag=1;
			mask[i]=1;
		}
		tmp=tmp->next;
	}
	tmp=main;
	for(i=0;tmp;i++){
		if (mask[i]){
			del=tmp;
			tmp=tmp->next;
			main=deleteElem(main,del);
		}else{
			tmp=tmp->next;
		}
	}
	return main;
}

void initializRedirection(redirection *redir){
	redir->input_file=NULL;
	redir->output_file=NULL;
	redir->input_fd=-1;
	redir->output_fd=-1;
	redir->error=0;
	redir->append=0;
}

redirection analysisArrows(list *main){
	redirection redir;
	initializRedirection(&redir);
	while (main){
		if (!strcmp(main->word,"<")){
			if (redir.input_file){
				fprintf(stderr,"Same input redirection\n");
				redir.error=1;
				return redir;
			}
			if (!main->next){
				fprintf(stderr,"No input file set\n");
				redir.error=1;
				return redir;
			}
			main=main->next;
			redir.input_file=strdup(main->word);
		}
		if (!strcmp(main->word,">")){
			if (redir.output_file){
				fprintf(stderr,"Same output redirection\n");
				redir.error=1;
				return redir;
			}
			if (!main->next){
				fprintf(stderr,"No output file set\n");
				redir.error=1;
				return redir;
			}
			main=main->next;
			redir.output_file=strdup(main->word);
		}
		if (!strcmp(main->word,">>")){
			if (redir.output_file){
				fprintf(stderr,"Same output redirection\n");
				redir.error=1;
				return redir;
			}
			if (!main->next){
				fprintf(stderr,"No output file set\n");
				redir.error=1;
				return redir;
			}
			main=main->next;
			redir.output_file=strdup(main->word);
			redir.append=1;
		}
		main=main->next;
	}
	return redir;
}

void freeDuplicate(char **words){
	int i;
	for (i=0; words[i]; i++){
		free(words[i]);
	}
	free(words);
}

void changeDirectory(const char *directory){
        int result;
        if (directory){
                result=chdir(directory);
                if (result==-1){
                        perror(directory);
                }
        }else{
                result=chdir(getenv("HOME"));
        }
}

int sizeProg(char **prog){
        int i,result=0;
        for (i=0; prog[i];i++){
                result++;
        }
        return result;
}

int isBackGround(list **command){
	list *tmp=*command;
	if (!tmp){
		return 0;
	}
	while (tmp->next){
		tmp=tmp->next;
	}
	if (!strcmp(tmp->word,"&")){
		*command=deleteElem(*command,tmp);
		return 1;
	}
	return 0;
}

void checkKill(int pid,int status){
	int signal;
	if (WIFSIGNALED(status)){
		signal=WTERMSIG(status);
		fprintf(stderr,"Child [%i] killed: %i\n",pid,signal);
	}
}

void checkExit(int pid,int status){
	int code;
	if (WIFEXITED(status)){
		code=WEXITSTATUS(status);
		if (code!=0){
			fprintf(stderr,"Child [%i] exited with return code: %i\n",pid,code);
		}
	}
}

void checkExitBackground(int pid,int status){
	int code;
	if (WIFEXITED(status)){
		code=WEXITSTATUS(status);
		fprintf(stderr,"Child [%i] exited with return code: %i\n",pid,code);
	}
}

int checkPids(const int *pids,int amount){
	int i;
	for(i=0;i<amount;i++){
		if (pids[i]!=-1){
		return 1;
		}
	}
	return 0;
}

void waiting(int *pids,int amount){
	int i,found,pid,status;
	while (checkPids(pids,amount)){
		pid=wait(&status);
		found=0;
		checkKill(pid,status);
		checkExit(pid,status);
		for(i=0;i<amount;i++){
			if (pids[i]==pid){
				pids[i]=-1;
				found=1;
				break;
			}
		}
		checkKill(pid,status);
		found ? checkExit(pid,status) : checkExitBackground(pid,status);
	}
}

void execute(list *command,redirection data,int background){
	int count=countProg(command);
	int pids[count],fd[2];
	int i,prev=-1,fdin,fdout,flag;
	char **argument;
	if (!command){
		return;
	}
	if (!strcmp(command->word,"cd")){
		command=command->next;
		changeDirectory(command ? command->word : NULL);
		return;
	}
	for(i=0;i<count;i++){
		pipe(fd);
		argument=listToMassive(&command,count);
		if (!argument[0]){
			fprintf(stderr,"No command between '|'\n");
			return;
		}
		if ((pids[i]=fork())==0){
			if (prev==-1){
				if (data.input_file){
					fdin=open(data.input_file,O_RDONLY);
					if (fdin==-1){
						perror(data.input_file);
						exit(1);
					}
					dup2(fdin,0);
				}
			}else{
				dup2(prev,0);
				close(prev);
			}
			if (i==count-1){
				if (data.output_file){
					flag = data.append ? O_APPEND : O_TRUNC;
					fdout=open(data.output_file,O_WRONLY|O_CREAT|flag,0644);
					if (fdout==-1){
						perror(data.output_file);
						exit(1);
					}
					dup2(fdout,1);
				}
			}else{
				dup2(fd[1],1);
			}
			close(fd[0]);
			close(fd[1]);
			execvp(argument[0],argument);
			perror(argument[0]);
			exit(1);
		}
	}	
	close(fd[1]);
	if (prev!=-1)
		close(prev);
	prev=fd[0];
	if (i==count-1)
		close(prev);
	freeDuplicate(argument);
	if (background){
		for(i=0;i<count;i++){
			fprintf(stderr,"[%i]\n",pids[i]);
		}
	}else{
		waiting(pids,count);
	}
}

void cleanZombies(){
	int pid,status;
	do{
		pid=wait4(-1,&status,WNOHANG,NULL);
		if (pid>0){
			checkKill(pid,status);
			checkExitBackground(pid,status);
		}
	}while(pid>0);
}

void mainLoop(){
	char *str=NULL;
	list *pars=NULL;
	int background;
	redirection redir;
	printf(">> ");
	while ((str=createString())){
		pars=parsing(str);
		free(str);
		redir=analysisArrows(pars);
		background=isBackGround(&pars);
		if (!redir.error){
			pars=deleteArrows(pars);
			execute(pars,redir,background);
		}
		freeList(pars);
		cleanZombies();
		printf(">>  ");
	}
}
        
int main(int argc,char **argv){
	int fd;
	if (argc==2){
		fd=open(argv[1],O_RDONLY);
		if (fd==-1){
			perror(argv[1]);
			exit(1);
		}
		dup2(fd,0);
	}
	mainLoop();
	return 0;
}

