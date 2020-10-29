#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

const int sizeOfString 10

typedef struct list{
        char *string;
        int vogue;
        struct list *next;
} list;

char *content_string(){
        int stringCounter,sizeCounter,size=sizeOfString;
        char letter,*stringOfFile=NULL;
        stringCounter=sizeCounter=0;
        letter=getchar();
        while (letter!=EOF && letter!='\n'){
                if (stringCounter==0){
                        stringOfFile=malloc(size+1);
                }else{
                        if (sizeCounter==sizeOfString-1){
                                size+=sizeOfString;
                                stringOfFile=realloc(stringOfFile,size+1);
                                sizeCounter=0;
                        }
                }
                stringOfFile[stringCounter]=letter;
                stringCounter++;
                sizeCounter++;
                letter=getchar();
        }
        if (stringOfFile){
                stringOfFile[stringCounter]='\0';
        }
        if (letter=='\n' && !stringOfFile){
                stringOfFile=malloc(1);
                stringOfFile[0]='\0';
        }
        return stringOfFile;
}

list *add_first(list *mainlist, char *filestring){
        list *tempary;
        tempary=malloc(sizeof(list));
        tempary->string=filestring;
        tempary->vogue=1;
        tempary->next=mainlist;
        return tempary;
}

list *create_list(){
        int flag;
        char *filestring;
        list *mainlist=NULL, *tempary;
        while ((filestring=contentstring())){
                flag=0;
                tempary=mainlist;
                while (tempary){
                        if (!strcmp(tempary->string,filestring)){
                                tempary->vogue++;
                                flag=1;
                                free(filestring);
                                break;
                        }
                        tempary=tempary->next;
                }
                if (!flag){
                        mainlist=add_first(mainlist,filestring);
                }
        }
        return mainlist;
}

int search_max(list *mainlist){
        int max=0;
        while (mainlist){
                if ((mainlist->vogue)>max){
                        max=mainlist->vogue;
                }
                mainlist=mainlist->next;
        }
        return max;
}

void free_content(list *mainlist){
        list *tempary;
        while (mainlist){
                free(mainlist->string);
                tempary=mainlist;
                mainlist=mainlist->next;
                free(tempary);
        }
}

int main(int argc, char **argv){
        int filedescriptor,maxvogue;
        list *contentFile,*tempary;
        if (argc!=2){
                fprintf(stderr,"wrong arguments\n");
                exit(1);
        }
        filedescriptor=open(argv[1],O_RDONLY|O_CREAT);
        if (filedescriptor==-1){
                perror(argv[1]);
                exit(1);
        }
        dup2(filedescriptor,0);
        contentFile=create_list();
        tempary=contentFile;
        maxvogue=search_max(contentFile);
        tempary=contentFile;
        while (tempary){
                if ((tempary->vogue)==maxvogue){
                        printf("the most popular string: %s\n",tempary->string);
                }
                tempary=tempary->next;
        }
        free_content(contentFile);
        setbuf(stdin,NULL);
        setbuf(stdout,NULL);
        close(filedescriptor);
        return 0;
}
